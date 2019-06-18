//-*****************************************************************************
//
// Copyright (c) 2009-2014,
//  Sony Pictures Imageworks Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic, nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#include "MayaMeshWriter.h"
#include "MayaUtility.h"

#include <maya/MItSelectionList.h>
#include <maya/MFnSingleIndexedComponent.h>

namespace {

void getColorSet(MFnMesh & iMesh, const MString * iColorSet, bool isRGBA,
    std::vector<float> & oColors,
    std::vector< Alembic::Util::uint32_t > & oColorIndices)
{
    MColorArray colorArray;
    iMesh.getColors(colorArray, iColorSet);

    bool addDefaultColor = true;

    int numFaces = iMesh.numPolygons();
    for (int faceIndex = 0; faceIndex < numFaces; faceIndex++)
    {
        MIntArray vertexList;
        iMesh.getPolygonVertices(faceIndex, vertexList);

        int numVertices = iMesh.polygonVertexCount(faceIndex);
        for ( int v = numVertices-1; v >=0; v-- )
        {
            int colorIndex = 0;
            iMesh.getColorIndex(faceIndex, v, colorIndex, iColorSet);

            if (colorIndex == -1)
            {
                if (addDefaultColor)
                {
                    addDefaultColor = false;
                    colorArray.append(MColor(1.0, 1.0, 1.0, 1.0));
                }
                colorIndex = colorArray.length() - 1;
            }
            oColorIndices.push_back(colorIndex);
        }
    }

    int colorLen = colorArray.length();

    for (int i = 0; i < colorLen; ++i)
    {
        MColor color = colorArray[i];
        oColors.push_back(color.r);
        oColors.push_back(color.g);
        oColors.push_back(color.b);
        if (isRGBA)
        {
            oColors.push_back(color.a);
        }
    }
};

void getUVSet(const MFnMesh & iMesh, const MString & iUVSetName,
    std::vector<float> & oUVs, std::vector<Alembic::Util::uint32_t> & oIndices)
{
    MFloatArray uArray, vArray;
    iMesh.getUVs(uArray, vArray, &iUVSetName);
    if ( uArray.length() != vArray.length() )
    {
        MString msg = "UV Set " + iUVSetName +
            " uArray and vArray not the same length";
        MGlobal::displayError(msg);
        return;
    }

    unsigned int arLen = uArray.length();
    oUVs.clear();
    oUVs.reserve(arLen * 2);
    for (unsigned int i = 0; i < arLen; ++i)
    {
        oUVs.push_back(uArray[i]);
        oUVs.push_back(vArray[i]);
    }

    oIndices.clear();
    oIndices.reserve(iMesh.numFaceVertices());
    int faceCount = iMesh.numPolygons();
    int uvId = 0;
    for (int j = 0; j < faceCount; ++j)
    {
        int vc = iMesh.polygonVertexCount(j);
        for (int i = vc - 1; i >= 0; i--)
        {
            iMesh.getPolygonUVid(j, i, uvId, &iUVSetName);
            oIndices.push_back(uvId);
        }
    }
}

}

void MayaMeshWriter::getUVs(std::vector<float> & uvs,
    std::vector<Alembic::Util::uint32_t> & indices,
    std::string & name)
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }

    MString uvSetName = lMesh.currentUVSetName(&status);
    if (uvSetName.length() != 0)
    {
        MFloatArray uArray, vArray;
        status = lMesh.getUVs(uArray, vArray, &uvSetName);
        // convert the raw uv list into vector uvsvec.clear();
        if ( uArray.length() != vArray.length() )
        {
            MString msg = "uv Set" + uvSetName +
                "uArray and vArray not the same length";
            MGlobal::displayError(msg);
            return;
        }

        if (uvSetName != "map1")
        {
            name = uvSetName.asChar();
        }

        unsigned int len = uArray.length();
        uvs.clear();
        uvs.reserve(len * 2);
        for (unsigned int i = 0; i < len; i++)
        {
            uvs.push_back(uArray[i]);
            uvs.push_back(vArray[i]);
        }

        indices.clear();
        indices.reserve(lMesh.numFaceVertices());
        int faceCount = lMesh.numPolygons();
        int uvId = 0;
        for (int f = 0; f < faceCount; f++)
        {
            int len = lMesh.polygonVertexCount(f);
            for (int i = len-1; i >= 0; i--)
            {
                lMesh.getPolygonUVid(f, i, uvId, &uvSetName);
                indices.push_back(uvId);
            }
        }
    }
}

MayaMeshWriter::MayaMeshWriter(MDagPath & iDag,
    Alembic::Abc::OObject & iParent, Alembic::Util::uint32_t iTimeIndex,
    const JobArgs & iArgs)
  : mNoNormals(iArgs.noNormals),
    mWriteGeometry(iArgs.writeGeometry),
    mWriteUVs(iArgs.writeUVs),
    mWriteColorSets(iArgs.writeColorSets),
    mWriteUVSets(iArgs.writeUVSets),
    mIsGeometryAnimated(false),
    mDagPath(iDag)
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }

    // intermediate objects aren't translated
    MObject surface = iDag.node();

    if (iTimeIndex != 0 && util::isAnimated(surface))
    {
        mIsGeometryAnimated = true;
    }
    else
    {
        iTimeIndex = 0;
    }

    std::vector<float> uvs;
    std::vector<Alembic::Util::uint32_t> indices;
    std::string uvSetName;

    MString name = lMesh.name();
    name = util::stripNamespaces(name, iArgs.stripNamespace);

    // check to see if this poly has been tagged as a SubD
    MPlug plug = lMesh.findPlug("SubDivisionMesh", true);

    // if there is flag "autoSubd", and NO "SubDivisionMesh" was defined,
    // let's check whether the mesh has crease edge, crease vertex or holes
    // then the mesh will be treated as SubD
    bool hasToWriteSubd = false;
    if( plug.isNull() && iArgs.autoSubd )
    {
        MUintArray edgeIds, vertexIds;
        MDoubleArray edgeCreaseData, vertexCreaseData;
        lMesh.getCreaseEdges(edgeIds, edgeCreaseData);
        lMesh.getCreaseVertices(vertexIds, vertexCreaseData);
        hasToWriteSubd = ( (edgeIds.length() > 0) || (vertexIds.length() > 0) );
#if MAYA_API_VERSION >= 201100
        if (!hasToWriteSubd)
        {
            MUintArray invisibleFaceIds = lMesh.getInvisibleFaces();
            hasToWriteSubd = ( hasToWriteSubd || (invisibleFaceIds.length() > 0) );
        }
#endif
    }

    Alembic::Abc::SparseFlag sf = Alembic::Abc::kFull;
    if ( !mWriteGeometry )
    {
        sf = Alembic::Abc::kSparse;
    }

    if ( (!plug.isNull() && plug.asBool()) || hasToWriteSubd )
    {
        Alembic::AbcGeom::OSubD obj(iParent, name.asChar(), sf, iTimeIndex);
        mSubDSchema = obj.getSchema();

        Alembic::AbcGeom::OV2fGeomParam::Sample uvSamp;
        if (mWriteUVs || mWriteUVSets)
        {
            getUVs(uvs, indices, uvSetName);

            if (!uvs.empty())
            {
                if (!uvSetName.empty())
                {
                    mSubDSchema.setUVSourceName(uvSetName);
                }

                uvSamp.setScope( Alembic::AbcGeom::kFacevaryingScope );
                uvSamp.setVals(Alembic::AbcGeom::V2fArraySample(
                    (const Imath::V2f *) &uvs.front(), uvs.size() / 2));
                if (!indices.empty())
                {
                    uvSamp.setIndices(Alembic::Abc::UInt32ArraySample(
                        &indices.front(), indices.size()));
                }
            }
        }

        Alembic::Abc::OCompoundProperty cp;
        Alembic::Abc::OCompoundProperty up;
        if (AttributesWriter::hasAnyAttr(lMesh, iArgs))
        {
            cp = mSubDSchema.getArbGeomParams();
            up = mSubDSchema.getUserProperties();
        }
        mAttrs = AttributesWriterPtr(new AttributesWriter(cp, up, obj, lMesh,
            iTimeIndex, iArgs, true));

        if (!mIsGeometryAnimated || iArgs.setFirstAnimShape)
        {
            writeSubD(uvSamp);
        }
    }
    else
    {
        Alembic::AbcGeom::OPolyMesh obj(iParent, name.asChar(), sf, iTimeIndex);
        mPolySchema = obj.getSchema();

        Alembic::AbcGeom::OV2fGeomParam::Sample uvSamp;

        if (mWriteUVs || mWriteUVSets)
        {
            getUVs(uvs, indices, uvSetName);

            if (!uvs.empty())
            {
                if (!uvSetName.empty())
                {
                    mPolySchema.setUVSourceName(uvSetName);
                }
                uvSamp.setScope( Alembic::AbcGeom::kFacevaryingScope );
                uvSamp.setVals(Alembic::AbcGeom::V2fArraySample(
                    (const Imath::V2f *) &uvs.front(), uvs.size() / 2));
                if (!indices.empty())
                {
                    uvSamp.setIndices(Alembic::Abc::UInt32ArraySample(
                        &indices.front(), indices.size()));
                }
            }
        }

        Alembic::Abc::OCompoundProperty cp;
        Alembic::Abc::OCompoundProperty up;
        if (AttributesWriter::hasAnyAttr(lMesh, iArgs))
        {
            cp = mPolySchema.getArbGeomParams();
            up = mPolySchema.getUserProperties();
        }

        // set the rest of the props and write to the writer node
        mAttrs = AttributesWriterPtr(new AttributesWriter(cp, up, obj, lMesh,
            iTimeIndex, iArgs, true));

        if (!mIsGeometryAnimated || iArgs.setFirstAnimShape)
        {
            writePoly(uvSamp);
        }
    }

    if (mWriteColorSets)
    {
        MStringArray colorSetNames;
        lMesh.getColorSetNames(colorSetNames);

        if (colorSetNames.length() > 0)
        {

            // Create the color sets compound prop
            Alembic::Abc::OCompoundProperty arbParams;
            if (mPolySchema.valid())
            {
                arbParams =  mPolySchema.getArbGeomParams();
            }
            else
            {
                arbParams =  mSubDSchema.getArbGeomParams();
            }

            std::string currentColorSet = lMesh.currentColorSetName().asChar();
            for (unsigned int i=0; i < colorSetNames.length(); ++i)
            {
                // Create an array property for each color set
                std::string colorSetPropName = colorSetNames[i].asChar();

                Alembic::AbcCoreAbstract::MetaData md;
                if (currentColorSet == colorSetPropName)
                {
                    md.set("mayaColorSet", "1");
                }
                else
                {
                    md.set("mayaColorSet", "0");
                }

                if (lMesh.getColorRepresentation(colorSetNames[i]) ==
                    MFnMesh::kRGB)
                {
                    Alembic::AbcGeom::OC3fGeomParam colorProp(arbParams,
                        colorSetPropName, true,
                        Alembic::AbcGeom::kFacevaryingScope, 1, iTimeIndex, md);
                    mRGBParams.push_back(colorProp);
                }
                else
                {
                    Alembic::AbcGeom::OC4fGeomParam colorProp(arbParams,
                        colorSetPropName, true,
                        Alembic::AbcGeom::kFacevaryingScope, 1, iTimeIndex, md);
                    mRGBAParams.push_back(colorProp);
                }
            }

            if (!mIsGeometryAnimated || iArgs.setFirstAnimShape)
            {
                writeColor();
            }
        }
    }

    if (mWriteUVSets)
    {
        MStringArray uvSetNames;
        lMesh.getUVSetNames(uvSetNames);
        unsigned int uvSetNamesLen = uvSetNames.length();

        if (uvSetNamesLen > 1)
        {
            // Create the uv sets compound prop
            Alembic::Abc::OCompoundProperty arbParams;
            if (mPolySchema.valid())
            {
                arbParams =  mPolySchema.getArbGeomParams();
            }
            else
            {
                arbParams =  mSubDSchema.getArbGeomParams();
            }

            MString currentUV = lMesh.currentUVSetName();

            for (unsigned int i = 0; i < uvSetNamesLen; ++i)
            {
                // Create an array property for each uv set
                MString uvSetPropName = uvSetNames[i];

                // the current UV set gets mapped to the primary UVs
                if (currentUV == uvSetPropName)
                {
                    continue;
                }

                if (uvSetPropName.length() > 0 &&
                    lMesh.numUVs(uvSetPropName) > 0)
                {
                    mUVparams.push_back(Alembic::AbcGeom::OV2fGeomParam(
                        arbParams, uvSetPropName.asChar(), true,
                        Alembic::AbcGeom::kFacevaryingScope, 1, iTimeIndex));
                }
            }

            if (!mIsGeometryAnimated || iArgs.setFirstAnimShape)
            {
                writeUVSets();
            }
        }
    }

    // write out facesets
    if(!iArgs.writeFaceSets)
        return;

    MFnDependencyNode meshDep(mDagPath.node());
    MPlug iogPlug( meshDep.findPlug("instObjGroups", false, &status) );
    if( status == MS::kFailure )
    {
        return;
    }

    // we can have multiple connects to the same shading engine
    std::map< std::string,
        std::pair< MObject, std::vector< MPlug > > > facesetMap;

    // mesh.instObjGroups is an array of compounds
    for (unsigned int i = 0; i < iogPlug.numElements(); ++i)
    {
        MPlug compPlug = iogPlug.elementByPhysicalIndex(i);
        for (unsigned int j = 0; j < compPlug.numChildren(); ++j)
        {
            // no elements in the child could mean the whole mesh is shaded
            MPlug objGroupPlug = compPlug.child(j);
            for (unsigned int k = 0; k < objGroupPlug.numElements(); ++k)
            {
                // mesh.instObjGroups[i].objectGroups[k]
                MPlug objGrpCompPlug = objGroupPlug.elementByPhysicalIndex(k);
                if (objGrpCompPlug.isNull())
                {
                    continue;
                }

                MPlugArray dests;
                if (!objGrpCompPlug.destinations(dests))
                {
                    continue;
                }

                MObject shadingObj;

                // look for a shading engine, can we have more than 1?
                for (unsigned int p = 0; p < dests.length(); ++p)
                {
                    if (dests[p].isNull())
                    {
                        continue;
                    }

                    MObject testObj = dests[p].node();
                    if (!testObj.hasFn(MFn::kShadingEngine))
                    {
                        continue;
                    }
                    else
                    {
                        shadingObj = testObj;
                    }
                }

                if (shadingObj.isNull())
                {
                    continue;
                }

                // mesh.instObjGroups[i].objectGroups[k].objectGrpCompList
                for (unsigned int l = 0; l < objGrpCompPlug.numChildren(); ++l)
                {
                    MPlug childPlug = objGrpCompPlug.child(l);
                    MString plugName = childPlug.name();
                    plugName = plugName.substringW(plugName.length() - 17, plugName.length() - 1);
                    if (plugName == "objectGrpCompList")
                    {
                        MFnDependencyNode fnDepNode(shadingObj);
                        std::string faceSetName = fnDepNode.name().asChar();
                        std::map< std::string, std::pair< MObject,
                            std::vector< MPlug > > >::iterator it =
                            facesetMap.find(faceSetName);

                        if (it == facesetMap.end())
                        {
                            std::pair< MObject, std::vector< MPlug > > opPair;
                            opPair.first = shadingObj;
                            opPair.second.push_back(childPlug);
                            facesetMap[faceSetName] = opPair;
                        }
                        else
                        {
                            it->second.second.push_back(childPlug);
                        }
                        // we can move on to the next
                        // mesh.instObjGroups[i].objectGroups[k]
                        break;
                    }
                } //  for l
            } // for k
        } // for j
    } // for i

    Alembic::Abc::OObject parentObj;
    if (mPolySchema.valid())
    {
        parentObj = mPolySchema.getObject();
    }
    else
    {
        parentObj = mSubDSchema.getObject();
    }

    std::map< std::string,
        std::pair< MObject, std::vector< MPlug > > >::iterator it;
    for (it = facesetMap.begin(); it != facesetMap.end(); ++it)
    {

        MayaFaceSetWriterPtr facePtr(new MayaFaceSetWriter(
            it->second.first, it->second.second, parentObj,
            iTimeIndex, iArgs));
        mFaceSets.push_back(facePtr);
    }
}

bool MayaMeshWriter::isSubD()
{
    return mSubDSchema.valid();
}

unsigned int MayaMeshWriter::getNumCVs()
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }
    return lMesh.numVertices();
}

unsigned int MayaMeshWriter::getNumFaces()
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }
    return lMesh.numPolygons();
}

void MayaMeshWriter::getPolyNormals(std::vector<float> & oNormals)
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }

    // no normals bail early
    if (mNoNormals)
    {
        return;
    }

    MPlug plug = lMesh.findPlug("noNormals", true, &status);
    if (status == MS::kSuccess && plug.asBool() == true)
    {
        return;
    }
    // we need to check the locked state of the normals
    else if ( status != MS::kSuccess )
    {
        bool userSetNormals = false;

        // go through all per face-vertex normals and verify if any of them
        // has been tweaked by users
        unsigned int numFaces = lMesh.numPolygons();
        for (unsigned int faceIndex = 0; faceIndex < numFaces; faceIndex++)
        {
            MIntArray normals;
            lMesh.getFaceNormalIds(faceIndex, normals);
            unsigned int numNormals = normals.length();
            for (unsigned int n = 0; n < numNormals; n++)
            {
                if (lMesh.isNormalLocked(normals[n]))
                {
                    userSetNormals = true;
                    break;
                }
            }
        }

        // we looped over all the normals and they were all calculated by Maya
        // so we just need to check to see if any of the edges are hard
        // before we decide not to write the normals.
        if (!userSetNormals)
        {
            bool hasHardEdges   = false;

            // go through all edges and verify if any of them is hard edge
            unsigned int numEdges = lMesh.numEdges();
            for (unsigned int edgeIndex = 0; edgeIndex < numEdges; edgeIndex++)
            {
                if (!lMesh.isEdgeSmooth(edgeIndex))
                {
                    hasHardEdges = true;
                    break;
                }
            }

            // all the edges were smooth, we don't need to write the normals
            if (!hasHardEdges)
            {
                return;
            }
        }
    }

    bool flipNormals = false;
    plug = lMesh.findPlug("flipNormals", true, &status);
    if ( status == MS::kSuccess )
        flipNormals = plug.asBool();

    // get the per vertex per face normals (aka vertex)
    unsigned int numFaces = lMesh.numPolygons();

    for (unsigned int faceIndex = 0; faceIndex < numFaces; faceIndex++ )
    {
        MIntArray vertexList;
        lMesh.getPolygonVertices(faceIndex, vertexList);

        // re-pack the order of normals in this vector before writing into prop
        // so that Renderman can also use it
        unsigned int numVertices = vertexList.length();
        for ( int v = numVertices-1; v >=0; v-- )
        {
            unsigned int vertexIndex = vertexList[v];
            MVector normal;
            lMesh.getFaceVertexNormal(faceIndex, vertexIndex, normal);

            if (flipNormals)
                normal = -normal;

            oNormals.push_back(static_cast<float>(normal[0]));
            oNormals.push_back(static_cast<float>(normal[1]));
            oNormals.push_back(static_cast<float>(normal[2]));
        }
    }
}

void MayaMeshWriter::writeUVSets()
{

    MStatus status = MS::kSuccess;
    const MFnMesh lMesh(mDagPath, &status);
    if (!status)
    {
        MGlobal::displayError(
            "MFnMesh() failed for MayaMeshWriter::writeUV" );
        return;
    }

    //Write uvs
    const UVParamsVec::const_iterator uvItEnd = mUVparams.end();
    for (UVParamsVec::iterator uvIt = mUVparams.begin();
        uvIt != uvItEnd; ++uvIt)
    {
        std::vector<float> uvs;
        std::vector<Alembic::Util::uint32_t> indices;

        MString uvSetName(uvIt->getName().c_str());
        getUVSet(lMesh, uvSetName, uvs, indices);

        //cast the vector to the sample type
        Alembic::AbcGeom::OV2fGeomParam::Sample sample(
            Alembic::Abc::V2fArraySample(
                (const Imath::V2f *) &uvs.front(), uvs.size() / 2),
            Alembic::Abc::UInt32ArraySample(indices),
            Alembic::AbcGeom::kFacevaryingScope);

        uvIt->set(sample);
    }
}

void MayaMeshWriter::writeColor()
{

    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError(
            "MFnMesh() failed for MayaMeshWriter::writeColor" );
        return;
    }

    //Write colors
    std::vector<Alembic::AbcGeom::OC4fGeomParam>::iterator rgbaIt;
    std::vector<Alembic::AbcGeom::OC4fGeomParam>::iterator rgbaItEnd;
    rgbaIt = mRGBAParams.begin();
    rgbaItEnd = mRGBAParams.end();

    for (; rgbaIt != rgbaItEnd; ++rgbaIt)
    {
        std::vector<float> colors;
        std::vector< Alembic::Util::uint32_t > colorIndices;

        MString colorSetName(rgbaIt->getName().c_str());
        getColorSet(lMesh, &colorSetName, true, colors, colorIndices);

        //cast the vector to the sample type
        Alembic::AbcGeom::OC4fGeomParam::Sample samp(
            Alembic::Abc::C4fArraySample(
                (const Imath::C4f *) &colors.front(), colors.size()/4),
            Alembic::Abc::UInt32ArraySample(colorIndices),
            Alembic::AbcGeom::kFacevaryingScope );

        rgbaIt->set(samp);
    }

    std::vector<Alembic::AbcGeom::OC3fGeomParam>::iterator rgbIt;
    std::vector<Alembic::AbcGeom::OC3fGeomParam>::iterator rgbItEnd;
    rgbIt = mRGBParams.begin();
    rgbItEnd = mRGBParams.end();
    for (; rgbIt != rgbItEnd; ++rgbIt)
    {

        std::vector<float> colors;
        std::vector< Alembic::Util::uint32_t > colorIndices;

        MString colorSetName(rgbIt->getName().c_str());
        getColorSet(lMesh, &colorSetName, false, colors, colorIndices);

        //cast the vector to the sample type
        Alembic::AbcGeom::OC3fGeomParam::Sample samp(
            Alembic::Abc::C3fArraySample(
                (const Imath::C3f *) &colors.front(), colors.size()/3),
            Alembic::Abc::UInt32ArraySample(colorIndices),
            Alembic::AbcGeom::kFacevaryingScope);

        rgbIt->set(samp);
    }
}

void MayaMeshWriter::write()
{

    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }

    Alembic::AbcGeom::OV2fGeomParam::Sample uvSamp;
    std::vector<float> uvs;
    std::vector<Alembic::Util::uint32_t> indices;
    std::string uvSetName;

    if (mWriteUVs || mWriteUVSets)
    {
        getUVs(uvs, indices, uvSetName);

        if (!uvs.empty())
        {
            if (!uvSetName.empty())
            {
                if (mPolySchema.valid())
                {
                    mPolySchema.setUVSourceName(uvSetName);
                }
                else if (mSubDSchema.valid())
                {
                    mSubDSchema.setUVSourceName(uvSetName);
                }
            }
            uvSamp.setScope( Alembic::AbcGeom::kFacevaryingScope );
            uvSamp.setVals(Alembic::AbcGeom::V2fArraySample(
                (const Imath::V2f *) &uvs.front(), uvs.size() / 2));
            if (!indices.empty())
            {
                uvSamp.setIndices(Alembic::Abc::UInt32ArraySample(
                    &indices.front(), indices.size()));
            }
        }
    }

    std::vector<float> points;
    std::vector<Alembic::Util::int32_t> facePoints;
    std::vector<Alembic::Util::int32_t> faceList;

    if (mPolySchema.valid())
    {
        writePoly(uvSamp);
    }
    else if (mSubDSchema.valid())
    {
        writeSubD(uvSamp);
    }

    std::vector< MayaFaceSetWriterPtr >::iterator it;
    for (it = mFaceSets.begin(); it != mFaceSets.end(); ++it)
    {
        (*it)->write();
    }

}

bool MayaMeshWriter::isAnimated() const
{
    return mIsGeometryAnimated;
}

void MayaMeshWriter::writePoly(
    const Alembic::AbcGeom::OV2fGeomParam::Sample & iUVs)
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }

    std::vector<float> points;
    std::vector<Alembic::Util::int32_t> facePoints;
    std::vector<Alembic::Util::int32_t> pointCounts;

    if( mWriteGeometry )
    {
       fillTopology(points, facePoints, pointCounts);
    }

    Alembic::AbcGeom::ON3fGeomParam::Sample normalsSamp;
    std::vector<float> normals;
    getPolyNormals(normals);
    if (!normals.empty())
    {
        normalsSamp.setScope( Alembic::AbcGeom::kFacevaryingScope );
        normalsSamp.setVals(Alembic::AbcGeom::N3fArraySample(
            (const Imath::V3f *) &normals.front(), normals.size() / 3));
    }

    Alembic::AbcGeom::OPolyMeshSchema::Sample samp;

    if ( mWriteGeometry )
    {
        samp.setPositions(Alembic::Abc::V3fArraySample(
            (const Imath::V3f *)&points.front(), points.size() / 3) );
        samp.setFaceIndices(Alembic::Abc::Int32ArraySample(facePoints));
        samp.setFaceCounts(Alembic::Abc::Int32ArraySample(pointCounts));
    }

    samp.setUVs( iUVs );
    samp.setNormals( normalsSamp );

    mPolySchema.set(samp);
    writeColor();
    writeUVSets();
}

void MayaMeshWriter::writeSubD(
    const Alembic::AbcGeom::OV2fGeomParam::Sample & iUVs)
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }

    std::vector<float> points;
    std::vector<Alembic::Util::int32_t> facePoints;
    std::vector<Alembic::Util::int32_t> pointCounts;

    if( mWriteGeometry )
    {
        fillTopology(points, facePoints, pointCounts);
    }

    Alembic::AbcGeom::OSubDSchema::Sample samp;

    if ( !mWriteGeometry )
    {
        samp.setUVs( iUVs );
        mSubDSchema.set(samp);
        writeColor();
        writeUVSets();
        return;
    }

    samp.setPositions(Alembic::AbcGeom::V3fArraySample(
        (const Imath::V3f *)&points.front(), points.size() / 3));
    samp.setFaceIndices(Alembic::Abc::Int32ArraySample(facePoints));
    samp.setFaceCounts(Alembic::Abc::Int32ArraySample(pointCounts));

    MPlug plug = lMesh.findPlug("faceVaryingInterpolateBoundary", true);
    if (!plug.isNull())
        samp.setFaceVaryingInterpolateBoundary(plug.asInt());

    plug = lMesh.findPlug("interpolateBoundary", true);
    if (!plug.isNull())
        samp.setInterpolateBoundary(plug.asInt());

    plug = lMesh.findPlug("faceVaryingPropagateCorners", true);
    if (!plug.isNull())
        samp.setFaceVaryingPropagateCorners(plug.asInt());

    std::vector <Alembic::Util::int32_t> creaseIndices;
    std::vector <Alembic::Util::int32_t> creaseLengths;
    std::vector <float> creaseSharpness;

    std::vector <Alembic::Util::int32_t> cornerIndices;
    std::vector <float> cornerSharpness;

    MUintArray edgeIds;
    MDoubleArray creaseData;
    if (lMesh.getCreaseEdges(edgeIds, creaseData) == MS::kSuccess)
    {
        unsigned int numCreases = creaseData.length();
        creaseIndices.resize(numCreases * 2);
        creaseLengths.resize(numCreases, 2);
        creaseSharpness.resize(numCreases);
        for (unsigned int i = 0; i < numCreases; ++i)
        {
            int verts[2];
            lMesh.getEdgeVertices(edgeIds[i], verts);
            creaseIndices[2 * i] = verts[0];
            creaseIndices[2 * i + 1] = verts[1];
            creaseSharpness[i] = static_cast<float>(creaseData[i]);
        }

        samp.setCreaseIndices(Alembic::Abc::Int32ArraySample(creaseIndices));
        samp.setCreaseLengths(Alembic::Abc::Int32ArraySample(creaseLengths));
        samp.setCreaseSharpnesses(
            Alembic::Abc::FloatArraySample(creaseSharpness));
    }

    MUintArray cornerIds;
    MDoubleArray cornerData;
    if (lMesh.getCreaseVertices(cornerIds, cornerData) == MS::kSuccess)
    {
        unsigned int numCorners = cornerIds.length();
        cornerIndices.resize(numCorners);
        cornerSharpness.resize(numCorners);
        for (unsigned int i = 0; i < numCorners; ++i)
        {
            cornerIndices[i] = cornerIds[i];
            cornerSharpness[i] = static_cast<float>(cornerData[i]);
        }
        samp.setCornerSharpnesses(
            Alembic::Abc::FloatArraySample(cornerSharpness));

        samp.setCornerIndices(
            Alembic::Abc::Int32ArraySample(cornerIndices));
    }

#if MAYA_API_VERSION >= 201100
    MUintArray holes = lMesh.getInvisibleFaces();
    unsigned int numHoles = holes.length();
    std::vector <Alembic::Util::int32_t> holeIndices(numHoles);
    for (unsigned int i = 0; i < numHoles; ++i)
    {
        holeIndices[i] = holes[i];
    }

    if (!holeIndices.empty())
    {
        samp.setHoles(holeIndices);
    }
#endif

    samp.setUVs( iUVs );
    mSubDSchema.set(samp);
    writeColor();
    writeUVSets();
}

// the arrays being passed in are assumed to be empty
void MayaMeshWriter::fillTopology(
    std::vector<float> & oPoints,
    std::vector<Alembic::Util::int32_t> & oFacePoints,
    std::vector<Alembic::Util::int32_t> & oPointCounts)
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }

    MFloatPointArray pts;

    lMesh.getPoints(pts);

    if (pts.length() < 3 && pts.length() > 0)
    {
        MString err = lMesh.fullPathName() +
            " is not a valid mesh, because it only has ";
        err += pts.length();
        err += " points.";
        MGlobal::displayError(err);
        return;
    }

    unsigned int numPolys = lMesh.numPolygons();

    if (numPolys == 0)
    {
        MGlobal::displayWarning(lMesh.fullPathName() + " has no polygons.");
        return;
    }

    unsigned int i;
    int j;

    oPoints.resize(pts.length() * 3);

    // repack the float
    for (i = 0; i < pts.length(); i++)
    {
        size_t local = i * 3;
        oPoints[local] = pts[i].x;
        oPoints[local+1] = pts[i].y;
        oPoints[local+2] = pts[i].z;
    }

    /*
        oPoints -
        oFacePoints - vertex list
        oPointCounts - number of points per polygon
    */

    MIntArray faceArray;

    for (i = 0; i < numPolys; i++)
    {
        lMesh.getPolygonVertices(i, faceArray);

        if (faceArray.length() < 3)
        {
            MGlobal::displayWarning("Skipping degenerate polygon");
            continue;
        }

        // write backwards cause polygons in Maya are in a different order
        // from Renderman (clockwise vs counter-clockwise?)
        int faceArrayLength = faceArray.length() - 1;
        for (j = faceArrayLength; j > -1; j--)
        {
            oFacePoints.push_back(faceArray[j]);
        }

        oPointCounts.push_back(faceArray.length());
    }
}
