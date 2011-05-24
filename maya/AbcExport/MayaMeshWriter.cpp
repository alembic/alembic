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
    std::vector<uint32_t> & indices)
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
        size_t faceCount = uvCounts.length();
        size_t uvIndex = 0;
        for (size_t f = 0; f < faceCount; f++)
        {
            len = uvCounts[f];
            for (int i = len-1; i >= 0; i--)
                indices.push_back(uvIds[uvIndex+i]);
            uvIndex += len;
        }
    }
}


MayaMeshWriter::MayaMeshWriter(MDagPath & iDag,
    Alembic::Abc::OObject & iParent, uint32_t iTimeIndex,
    bool iWriteVisibility, bool iWriteUVs, bool iForceStatic)
  : mIsGeometryAnimated(false),
    mDagPath(iDag),
    mNumPoints(0)
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }

    // intermediate objects aren't translated
    MObject surface = iDag.node();

    if (!iForceStatic && util::isAnimated(surface))
        mIsGeometryAnimated = true;

    std::vector<float> uvs;
    std::vector<uint32_t> indices;

    // check to see if this poly has been tagged as a SubD
    MPlug plug = lMesh.findPlug("SubDivisionMesh");
    if ( !plug.isNull() && plug.asBool() )
    {
        Alembic::AbcGeom::OSubD obj(iParent, lMesh.name().asChar(), iTimeIndex);
        mSubDSchema = obj.getSchema();

        Alembic::AbcGeom::OV2fGeomParam::Sample uvSamp;
        if ( iWriteUVs )
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

        Alembic::Abc::OCompoundProperty cp = mSubDSchema.getArbGeomParams();

        mAttrs = AttributesWriterPtr(new AttributesWriter(cp, lMesh,
            iTimeIndex, iWriteVisibility, iForceStatic));

        writeSubD(iDag, uvSamp);
    }
    else
    {
        Alembic::AbcGeom::OPolyMesh obj(iParent, lMesh.name().asChar(),
            iTimeIndex);
        mPolySchema = obj.getSchema();

        Alembic::AbcGeom::OV2fGeomParam::Sample uvSamp;

        if ( iWriteUVs )
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

        Alembic::Abc::OCompoundProperty cp = mPolySchema.getArbGeomParams();

        // set the rest of the props and write to the writer node
        mAttrs = AttributesWriterPtr(new AttributesWriter(cp, lMesh,
            iTimeIndex, iWriteVisibility, iForceStatic));

       writePoly(uvSamp);
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
    MPlug plug = lMesh.findPlug("noNormals", true, &status);
    if ( status == MS::kSuccess && plug.asBool() == true )
        return;

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

            oNormals.push_back(normal[0]);
            oNormals.push_back(normal[1]);
            oNormals.push_back(normal[2]);
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
    size_t numPoints = lMesh.numVertices();

    // the topology has changed since the last sample, so get and write it all
    if (numPoints != mNumPoints)
    {
        std::vector<float> points;
        std::vector<int32_t> facePoints;
        std::vector<int32_t> faceList;

        mNumPoints = numPoints;
        fillTopology(points, facePoints, faceList);
        if (mPolySchema.valid())
        {
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
                Alembic::Abc::V3fArraySample(
                    (const Imath::V3f *)&points.front(), points.size() / 3 ),
                Alembic::Abc::Int32ArraySample( facePoints ),
                Alembic::Abc::Int32ArraySample( faceList ),
                Alembic::AbcGeom::OV2fGeomParam::Sample(),
                normalsSamp);

            mPolySchema.set(samp);
        }
        else if (mSubDSchema.valid())
        {
            Alembic::AbcGeom::OSubDSchema::Sample samp(
                Alembic::Abc::V3fArraySample( 
                    ( const Imath::V3f * )&points.front(), points.size() / 3 ),
                Alembic::Abc::Int32ArraySample( facePoints ),
                Alembic::Abc::Int32ArraySample( faceList ) );

            mSubDSchema.set(samp);
        }
    }
    // topology has not changed just write the point data
    else
    {
        std::vector<float> points;
        MFloatPointArray pts;
        lMesh.getPoints(pts);

        size_t i;
        points.resize(pts.length() * 3);

        // repack the float
        for (i = 0; i < pts.length(); i++)
        {
            size_t local = i * 3;
            points[local] = pts[i].x;
            points[local+1] = pts[i].y;
            points[local+2] = pts[i].z;
        }

        if (mPolySchema.valid())
        {
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
                Alembic::Abc::V3fArraySample(
                    (const Imath::V3f *)&points.front(), points.size() / 3),
                Alembic::Abc::Int32ArraySample(),
                Alembic::Abc::Int32ArraySample(),
                Alembic::AbcGeom::OV2fGeomParam::Sample(),
                normalsSamp);

            mPolySchema.set(samp);
        }
        else if (mSubDSchema.valid())
        {
            Alembic::AbcGeom::OSubDSchema::Sample samp;
            samp.setPositions( Alembic::Abc::V3fArraySample(
                (const Imath::V3f *) &points.front(), points.size() / 3) );
            mSubDSchema.set(samp);
        }
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
    std::vector<int32_t> facePoints;
    std::vector<int32_t> pointCounts;

    fillTopology(points, facePoints, pointCounts);

    mNumPoints = lMesh.numVertices();

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

void MayaMeshWriter::writeSubD(MDagPath & iDag,
    const Alembic::AbcGeom::OV2fGeomParam::Sample & iUVs)
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }

    std::vector<float> points;
    std::vector<int32_t> facePoints;
    std::vector<int32_t> pointCounts;

    fillTopology(points, facePoints, pointCounts);

    Alembic::AbcGeom::OSubDSchema::Sample samp(
        Alembic::AbcGeom::V3fArraySample((const Imath::V3f *)&points.front(),
            points.size() / 3),
        Alembic::Abc::Int32ArraySample(facePoints),
        Alembic::Abc::Int32ArraySample(pointCounts));
    samp.setUVs( iUVs );

    mNumPoints = lMesh.numVertices();

    // look for some of the renderman specific extra flags
    // The attribute is called facevaryingType because at one time it
    // was a more complicated enum
    MPlug plug = lMesh.findPlug("facevaryingType");
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
        std::size_t numCreases = creaseData.length();
        creaseIndices.resize(numCreases * 2);
        creaseLengths.resize(numCreases, 2);
        creaseSharpness.resize(numCreases);
        for (std::size_t i = 0; i < numCreases; ++i)
        {
            int verts[2];
            lMesh.getEdgeVertices(edgeIds[i], verts);
            creaseIndices[2 * i] = verts[0];
            creaseIndices[2 * i + 1] = verts[1];
            creaseSharpness[i] = creaseData[i];
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
        std::size_t numCorners = cornerIds.length();
        cornerIndices.resize(numCorners);
        cornerSharpness.resize(numCorners);
        for (std::size_t i = 0; i < numCorners; ++i)
        {
            cornerIndices[i] = cornerIds[i];
            cornerSharpness[i] = cornerData[i];
        }
        samp.setCornerSharpnesses(
            Alembic::Abc::FloatArraySample(cornerSharpness));

        samp.setCornerIndices(
            Alembic::Abc::Int32ArraySample(cornerIndices));
    }


    MUintArray holes = lMesh.getInvisibleFaces();
    std::size_t numHoles = holes.length();
    std::vector <Alembic::Util::int32_t> holeIndices(numHoles);
    for (std::size_t i = 0; i < numHoles; ++i)
    {
        holeIndices[i] = holes[i];
    }

    if (!holeIndices.empty())
    {
        samp.setHoles(holeIndices);
    }

    mSubDSchema.set(samp);

}

// the arrays being passed in are assumed to be empty
void MayaMeshWriter::fillTopology(
    std::vector<float> & oPoints,
    std::vector<int32_t> & oFacePoints,
    std::vector<int32_t> & oPointCounts)
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
