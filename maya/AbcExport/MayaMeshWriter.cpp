//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

// assumption is we don't support multiple uv sets as well as animated uvs
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
        uvs.reserve(len);
        for (unsigned int i = 0; i < len; i++)
        {
            uvs.push_back(uArray[i]); uvs.push_back(1-vArray[i]);
        }

        MIntArray uvCounts, uvIds;
        status = lMesh.getAssignedUVs(uvCounts, uvIds, &uvSetName);
        indices.clear();
        indices.reserve(uvIds.length());
        unsigned int faceCount = uvCounts.length();
        unsigned int uvIndex = 0;
        for (unsigned int f = 0; f < faceCount; f++)
        {
            len = uvCounts[f];
            for (int i = len-1; i >= 0; i--)
                indices.push_back(uvIds[uvIndex+i]);
            uvIndex += len;
        }
    }
}


MayaMeshWriter::MayaMeshWriter(MDagPath & iDag,
    Alembic::Abc::OObject & iParent, Alembic::Util::uint32_t iTimeIndex,
    const JobArgs & iArgs)
  : mNoNormals(iArgs.noNormals),
    mWriteUVs(iArgs.writeUVs),
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
    if (iArgs.stripNamespace)
    {
        name = util::stripNamespaces(name);
    }

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
        if (AttributesWriter::hasAnyAttr(lMesh, iArgs))
        {
            cp = mSubDSchema.getArbGeomParams();
        }
        mAttrs = AttributesWriterPtr(new AttributesWriter(cp, obj, lMesh,
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
        if (AttributesWriter::hasAnyAttr(lMesh, iArgs))
        {
            cp = mPolySchema.getArbGeomParams();
        }

        // set the rest of the props and write to the writer node
        mAttrs = AttributesWriterPtr(new AttributesWriter(cp, obj, lMesh,
            iTimeIndex, iArgs));

        writePoly(uvSamp);
    }

    // look for facesets
    std::size_t attrCount = lMesh.attributeCount();
    for (std::size_t i = 0; i < attrCount; ++i)
    {
        MObject attr = lMesh.attribute(i);
        MFnAttribute mfnAttr(attr);
        MPlug plug = lMesh.findPlug(attr, true);

        // if it is not readable, then bail without any more checking
        if (!mfnAttr.isReadable() || plug.isNull())
            continue;

        MString propName = plug.partialName(0, 0, 0, 0, 0, 1);
        std::string propStr = propName.asChar();

        if (propStr.substr(0, 8) == "FACESET_")
        {
            MStatus status;
            MFnIntArrayData arr(plug.asMObject(), &status);

            // not the correct kind of data
            if (status != MS::kSuccess)
                continue;

            std::string faceSetName = propStr.substr(8);
            std::size_t numData = arr.length();
            std::vector<Alembic::Util::int32_t> faceVals(numData);
            for (std::size_t j = 0; j < numData; ++j)
            {
                faceVals[j] = arr[j];
            }

            bool isVisible = true;
            MString visName = "FACESETVIS_";
            visName += faceSetName.c_str();
            MPlug visPlug = lMesh.findPlug(visName, true);
            if (!visPlug.isNull())
            {
                isVisible = visPlug.asBool();
            }

            Alembic::AbcGeom::OFaceSet faceSet;
            if (mPolySchema)
            {
                faceSet = mPolySchema.createFaceSet(faceSetName);
            }
            else
            {
                faceSet = mSubDSchema.createFaceSet(faceSetName);
            }
            Alembic::AbcGeom::OFaceSetSchema::Sample samp;
            samp.setFaces(Alembic::Abc::Int32ArraySample(faceVals));

            if (!isVisible)
            {
                Alembic::AbcGeom::OVisibilityProperty visProp =
                    Alembic::AbcGeom::CreateVisibilityProperty( faceSet, 0 );
                Alembic::Abc::int8_t visVal = 0;
                visProp.set(visVal);
            }

            faceSet.getSchema().set(samp);
        }
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
        // so we won't write any of them out
        if (!userSetNormals)
        {
            return;
        }
    }

    bool flipNormals = false;
    plug = lMesh.findPlug("flipNormals", true, &status);
    if ( status == MS::kSuccess )
        flipNormals = plug.asBool();

    // get the per vertex per face normals (aka vertex)
    unsigned int numFaces = lMesh.numPolygons();

    for (size_t faceIndex = 0; faceIndex < numFaces; faceIndex++ )
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
