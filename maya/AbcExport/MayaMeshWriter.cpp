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

// --------------------------------------------------------------
// getOutConnectedSG( const MObject &shape )
//
// Return the output connected shading groups from a shape object
//---------------------------------------------------------------

MObjectArray
getOutConnectedSG( const MDagPath &shapeDPath )
{
    MStatus status;

    // Array of connected Shaging Engines
    MObjectArray connSG;

    // Iterator through the dependency graph to find if there are
    // shading engines connected
    MObject obj(shapeDPath.node()); // non const MObject
    MItDependencyGraph itDG( obj, MFn::kShadingEngine,
                             MItDependencyGraph::kDownstream,
                             MItDependencyGraph::kBreadthFirst,
                             MItDependencyGraph::kNodeLevel, &status );

    if( status == MS::kFailure )
        return connSG;

    // we want to prune the iteration if the node is not a shading engine
    itDG.enablePruningOnFilter();

    // iterate through the output connected shading engines
    for( ; itDG.isDone()!= true; itDG.next() )
        connSG.append( itDG.thisNode() );

    return connSG;
}

// -----------------------------------------------------------------------------------------------------------
// getSetComponents( const MDagPath &dagPath, const MObject &SG, GetMembersMap& gmMap, MObject &compObj )
//
// Return the members of a shading engine for a specific dagpath.
// GetMembersMap is a caching mechanism.
// If it's face mapping, return the indices, otherwise it's the whole object, and so we
// return kFailure.
//------------------------------------------------------------------------------------------------------------

MStatus
getSetComponents( const MDagPath &dagPath, const MObject &SG, GetMembersMap& gmMap, MObject &compObj )
{
    const MString instObjGroupsAttrName( "instObjGroups" );

    // Check if SG is really a shading engine
    if( SG.hasFn(MFn::kShadingEngine) != true )
    {
        MFnDependencyNode fnDepNode( SG );
        MString message;
        message.format("Node ^1s is not a valid shading engine...", fnDepNode.name() );
        MGlobal::displayError(message);

        return MS::kFailure;
    }

    // get the instObjGroups iog plug
    MStatus status;
    MFnDependencyNode depNode(dagPath.node());
    MPlug iogPlug( depNode.findPlug(instObjGroupsAttrName, false, &status) );
    if( status == MS::kFailure )
        return MS::kFailure;

    // If the first plug in the array is connected, we have a whole object mapping. Return.
    if( iogPlug.numElements()<=0 )
        return MS::kFailure;

    MPlugArray iogConnections;
    iogPlug.elementByLogicalIndex(0, &status).connectedTo(iogConnections, false, true, &status );
    // this is not a shading group. skip it.
    if( status == MS::kFailure )
        return MS::kFailure;

    // Function set for the shading engine
    MFnSet fnSet( SG );

    // Retrieve members
    MSelectionList selList;
    GetMembersMap::iterator it = gmMap.find(SG);
    if(it != gmMap.end())
        selList = it->second;
    else
    {
        fnSet.getMembers(selList, false);
        gmMap[SG] = selList;
    }

    // Iteration through the list
    MStatus             retStat = MS::kFailure;
    MDagPath            curDagPath;
    MItSelectionList    itSelList( selList );
    for( ; itSelList.isDone()!=true; itSelList.next() )
    {
        // Test if it's a face mapping
        if( itSelList.hasComponents() == true )
        {
            itSelList.getDagPath( curDagPath, compObj );

            // Test if component object is valid and if it's the right object
            if( (compObj.isNull()==false) && (curDagPath==dagPath) )
            {
                return MS::kSuccess;
            }
        }
    }

    // SG is a shading engine but has no components connected to the dagPath.
    // This means we have a whole object mapping!
    return MS::kSuccess;
}

}

// assumption is we don't support multiple uv sets
void MayaMeshWriter::getUVs(std::vector<float> & uvs,
    std::vector<Alembic::Util::uint32_t> & indices)
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }

    MString uvSetName = lMesh.currentUVSetName(&status);
    if (status == MS::kSuccess && uvSetName != MString(""))
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

        unsigned int len = uArray.length();
        uvs.clear();
        uvs.reserve(len * 2);
        for (unsigned int i = 0; i < len; i++)
        {
            uvs.push_back(uArray[i]); uvs.push_back(vArray[i]);
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
                lMesh.getPolygonUVid(f, i, uvId);
                indices.push_back(uvId);
            }
        }
    }
}

MayaMeshWriter::MayaMeshWriter(MDagPath & iDag,
    Alembic::Abc::OObject & iParent, Alembic::Util::uint32_t iTimeIndex,
    const JobArgs & iArgs, GetMembersMap& gmMap)
  : mNoNormals(iArgs.noNormals),
    mWriteUVs(iArgs.writeUVs),
    mWriteColorSets(iArgs.writeColorSets),
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
        mIsGeometryAnimated = true;

    std::vector<float> uvs;
    std::vector<Alembic::Util::uint32_t> indices;

    MString name = lMesh.name();
    name = util::stripNamespaces(name, iArgs.stripNamespace);

    // check to see if this poly has been tagged as a SubD
    MPlug plug = lMesh.findPlug("SubDivisionMesh");
    if ( !plug.isNull() && plug.asBool() )
    {
        Alembic::AbcGeom::OSubD obj(iParent, name.asChar(), iTimeIndex);
        mSubDSchema = obj.getSchema();

        Alembic::AbcGeom::OV2fGeomParam::Sample uvSamp;
        if ( mWriteUVs )
        {
            getUVs(uvs, indices);

            if (!uvs.empty())
            {
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
            iTimeIndex, iArgs));

        writeSubD(uvSamp);
    }
    else
    {
        Alembic::AbcGeom::OPolyMesh obj(iParent, name.asChar(), iTimeIndex);
        mPolySchema = obj.getSchema();

        Alembic::AbcGeom::OV2fGeomParam::Sample uvSamp;

        if ( mWriteUVs )
        {
            getUVs(uvs, indices);

            if (!uvs.empty())
            {

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
            iTimeIndex, iArgs));

        writePoly(uvSamp);
    }

    if (mWriteColorSets)
    {
        MStringArray colorSetNames;
        lMesh.getColorSetNames(colorSetNames);

        if (colorSetNames.length() > 0)
        {

            // Create the color sets compound prop
            Alembic::Abc::OCompoundProperty arbParams;
            if (mPolySchema)
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
            writeColor();
        }
    }

    // write out facesets
    if(!iArgs.writeFaceSets)
        return;

    // get the connected shading engines
    MObjectArray connSGObjs (getOutConnectedSG(mDagPath));
    const unsigned int sgCount = connSGObjs.length();

    for (unsigned int i = 0; i < sgCount; ++i)
    {
        MObject connSGObj, compObj;

        connSGObj = connSGObjs[i];

        MFnDependencyNode fnDepNode(connSGObj);
        MString connSgObjName = fnDepNode.name();

        // retrive the component MObject
        status = getSetComponents(mDagPath, connSGObj, gmMap, compObj);

        if (status != MS::kSuccess)
        {
            MFnDependencyNode fnDepNode(connSGObj);
            MString message;
            message.format("Could not retrive face indices from set ^1s.",  connSgObjName);
            MGlobal::displayError(message);
            continue;
        }

        // retrieve the face indices
        MIntArray indices;
        MFnSingleIndexedComponent compFn;
        compFn.setObject(compObj);
        compFn.getElements(indices);
        const unsigned int numData = indices.length();

        // encountered the whole object mapping. skip it.
        if (numData == 0)
            continue;

        std::vector<Alembic::Util::int32_t> faceIndices(numData);
        for (unsigned int j = 0; j < numData; ++j)
        {
            faceIndices[j] = indices[j];
        }

        connSgObjName = util::stripNamespaces(connSgObjName,
                                              iArgs.stripNamespace);

        Alembic::AbcGeom::OFaceSet faceSet;
        std::string faceSetName(connSgObjName.asChar());

        MPlug abcFacesetNamePlug = fnDepNode.findPlug("AbcFacesetName", true);
        if (!abcFacesetNamePlug.isNull())
        {
            faceSetName = abcFacesetNamePlug.asString().asChar();
        }

        if (mPolySchema)
        {
            if (mPolySchema.hasFaceSet(faceSetName))
            {
                faceSet = mPolySchema.getFaceSet(faceSetName);
            }
            else
            {
                faceSet = mPolySchema.createFaceSet(faceSetName);
            }
        }
        else
        {
            if (mSubDSchema.hasFaceSet(faceSetName))
            {
                faceSet = mSubDSchema.getFaceSet(faceSetName);
            }
            else
            {
                faceSet = mSubDSchema.createFaceSet(faceSetName);
            }
        }
        Alembic::AbcGeom::OFaceSetSchema::Sample samp;
        samp.setFaces(Alembic::Abc::Int32ArraySample(faceIndices));

        Alembic::AbcGeom::OFaceSetSchema faceSetSchema = faceSet.getSchema();

        faceSetSchema.set(samp);
        faceSetSchema.setFaceExclusivity(Alembic::AbcGeom::kFaceSetExclusive);

        MFnDependencyNode iNode(connSGObj);

        Alembic::Abc::OCompoundProperty cp;
        Alembic::Abc::OCompoundProperty up;
        if (AttributesWriter::hasAnyAttr(iNode, iArgs))
        {
            cp = faceSetSchema.getArbGeomParams();
            up = faceSetSchema.getUserProperties();
        }

        AttributesWriter attrWriter(cp, up, faceSet, iNode, iTimeIndex, iArgs);
        attrWriter.write();
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

    if ( mWriteUVs )
    {
        getUVs(uvs, indices);

        if (!uvs.empty())
        {
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

    fillTopology(points, facePoints, pointCounts);

    Alembic::AbcGeom::ON3fGeomParam::Sample normalsSamp;
    std::vector<float> normals;
    getPolyNormals(normals);
    if (!normals.empty())
    {
        normalsSamp.setScope( Alembic::AbcGeom::kFacevaryingScope );
        normalsSamp.setVals(Alembic::AbcGeom::N3fArraySample(
            (const Imath::V3f *) &normals.front(), normals.size() / 3));
    }

    Alembic::AbcGeom::OPolyMeshSchema::Sample samp(
        Alembic::Abc::V3fArraySample((const Imath::V3f *)&points.front(),
            points.size() / 3),
        Alembic::Abc::Int32ArraySample(facePoints),
        Alembic::Abc::Int32ArraySample(pointCounts), iUVs, normalsSamp);

    // if this mesh is animated, write out the animated geometry
    if (mIsGeometryAnimated)
    {
        mPolySchema.set(samp);
    }
    else
    {
        mPolySchema.set(samp);
    }
    writeColor();
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

    fillTopology(points, facePoints, pointCounts);

    Alembic::AbcGeom::OSubDSchema::Sample samp(
        Alembic::AbcGeom::V3fArraySample((const Imath::V3f *)&points.front(),
            points.size() / 3),
        Alembic::Abc::Int32ArraySample(facePoints),
        Alembic::Abc::Int32ArraySample(pointCounts));
    samp.setUVs( iUVs );

    MPlug plug = lMesh.findPlug("facVaryingInterpolateBoundary");
    if (!plug.isNull())
        samp.setFaceVaryingInterpolateBoundary(plug.asInt());

    plug = lMesh.findPlug("interpolateBoundary");
    if (!plug.isNull())
        samp.setInterpolateBoundary(plug.asInt());

    plug = lMesh.findPlug("faceVaryingPropagateCorners");
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

    mSubDSchema.set(samp);
    writeColor();
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
