//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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
    std::vector<int32_t> & indices)
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


MayaMeshWriter::MayaMeshWriter(
    double iFrame,
    MDagPath & iDag,
    Alembic::Abc::OObject & iParent,
    Alembic::AbcCoreAbstract::v1::TimeSamplingType & iTimeType,
    bool iWriteVisibility,
    bool iWriteUVs)
  : mIsGeometryAnimated(false),
    mDagPath(iDag),
    mNumPoints(0),
    mCurIndex(0)
{
    MStatus status = MS::kSuccess;
    MFnMesh lMesh( mDagPath, &status );
    if ( !status )
    {
        MGlobal::displayError( "MFnMesh() failed for MayaMeshWriter" );
    }

    // intermediate objects aren't translated
    MObject surface = iDag.node();

    if (util::isAnimated(surface))
        mIsGeometryAnimated = true;

    // check to see if this poly has been tagged as a SubD
    MPlug plug = lMesh.findPlug("SubDivisionMesh");
    if ( !plug.isNull() && plug.asBool() )
    {
        Alembic::AbcGeom::OSubD obj(iParent, lMesh.name().asChar(), iTimeType);
        mSubDSchema = obj.getSchema();

        if ( iWriteUVs )
        {
            std::vector<float> uvs;
            std::vector<int32_t> indices;
            getUVs(uvs, indices);

            if (!uvs.empty())
            {
                // for now unpack the UVs, until they are part of the schema
                size_t numIndices = indices.size();
                std::vector<float> st(numIndices * 2);
                for (size_t i = 0; i < numIndices; ++i)
                {
                    st[2*i] = uvs[2 * indices[i]];
                    st[2*i + 1] = uvs[2 * indices[i] + 1];
                }

                Alembic::AbcCoreAbstract::v1::MetaData uvsMeta;
                SetGeometryScope( uvsMeta,
                    Alembic::AbcGeom::kFacevaryingScope );
                Alembic::AbcGeom::OV2fArrayProperty stProp( mSubDSchema,
                    "st", uvsMeta );
                stProp.set( Alembic::AbcGeom::V2fArraySample(
                    (const Imath::V2f *) &st.front(), st.size() / 2) );
            }
        }

        mAttrs = AttributesWriterPtr(new AttributesWriter(
            iFrame, obj, lMesh, iTimeType, iWriteVisibility));

        writeSubD(iFrame, iDag);
    }
    else
    {
        Alembic::AbcGeom::OPolyMesh obj(iParent, lMesh.name().asChar(),
            iTimeType);
        mPolySchema = obj.getSchema();

        if ( iWriteUVs )
        {
            std::vector<float> uvs;
            std::vector<int32_t> indices;
            getUVs(uvs, indices);

            if (!uvs.empty())
            {
                // for now unpack the UVs, until they are part of the schema
                size_t numIndices = indices.size();
                std::vector<float> st(numIndices * 2);
                for (size_t i = 0; i < numIndices; ++i)
                {
                    st[2*i] = uvs[2 * indices[i]];
                    st[2*i + 1] = uvs[2 * indices[i] + 1];
                }

                Alembic::AbcCoreAbstract::v1::MetaData uvsMeta;
                SetGeometryScope( uvsMeta,
                    Alembic::AbcGeom::kFacevaryingScope );
                Alembic::AbcGeom::OV2fArrayProperty stProp( mSubDSchema,
                    "st", uvsMeta );
                stProp.set( Alembic::AbcGeom::V2fArraySample(
                    (const Imath::V2f *) &st.front(), st.size() / 2) );
            }
        }

        // set the rest of the props and write to the writer node
        mAttrs = AttributesWriterPtr(new AttributesWriter(
            iFrame, obj, lMesh, iTimeType, iWriteVisibility));

       writePoly(iFrame);
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
    if ( status != MS::kSuccess || plug.asBool() == true )
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

void MayaMeshWriter::write(double iFrame)
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
            std::vector<float> normals;
            getPolyNormals(normals);

            Alembic::AbcGeom::OPolyMeshSchema::Sample samp(
                Alembic::Abc::V3fArraySample(
                    (const Imath::V3f *)&points.front(), points.size() / 3 ),
                Alembic::Abc::Int32ArraySample( facePoints ),
                Alembic::Abc::Int32ArraySample( faceList ) );

            Alembic::Abc::OSampleSelector s(mCurIndex++, iFrame/24.0);
            mPolySchema.set(samp, s);
        }
        else if (mSubDSchema.valid())
        {
            Alembic::AbcGeom::OSubDSchema::Sample samp(
                Alembic::Abc::V3fArraySample( 
                    ( const Imath::V3f * )&points.front(), points.size() / 3 ),
                Alembic::Abc::Int32ArraySample( facePoints ),
                Alembic::Abc::Int32ArraySample( faceList ) );

            Alembic::Abc::OSampleSelector s(mCurIndex++, iFrame/24.0);
            mSubDSchema.set(samp, s);
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
            std::vector<float> normals;
            getPolyNormals(normals);

            Alembic::Abc::OSampleSelector s(mCurIndex++, iFrame/24.0);
            Alembic::AbcGeom::OPolyMeshSchema::Sample samp(
                Alembic::Abc::V3fArraySample(
                    (const Imath::V3f *)&points.front(), points.size() / 3) );

            mPolySchema.set(samp, s);
        }
        else if (mSubDSchema.valid())
        {
            Alembic::Abc::OSampleSelector s(mCurIndex++, iFrame/24.0);
            Alembic::AbcGeom::OSubDSchema::Sample samp;
            samp.setPositions( Alembic::Abc::V3fArraySample(
                (const Imath::V3f *) &points.front(), points.size() / 3) );
            mSubDSchema.set(samp, s);
        }
    }

    mAttrs->write(iFrame);
}

bool MayaMeshWriter::isAnimated() const
{
    return mIsGeometryAnimated || (mAttrs != NULL && mAttrs->isAnimated());
}

void MayaMeshWriter::writePoly(double iFrame)
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

    std::vector<float> normals;
    getPolyNormals(normals);
    Alembic::AbcGeom::OPolyMeshSchema::Sample samp(
        Alembic::Abc::V3fArraySample((const Imath::V3f *)&points.front(),
            points.size() / 3),
        Alembic::Abc::Int32ArraySample(facePoints),
        Alembic::Abc::Int32ArraySample(pointCounts) );

    // if this mesh is animated, write out the animated geometry
    if (mIsGeometryAnimated)
    {
        mPolySchema.set(samp,
            Alembic::Abc::OSampleSelector(mCurIndex++, iFrame/ 24.0));
    }
    else
    {
        mPolySchema.set(samp);
    }

}

void MayaMeshWriter::writeSubD(double iFrame, MDagPath & iDag)
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
        Alembic::Abc::Int32ArraySample(pointCounts) );

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

    MObjectArray sets, comps;

    unsigned int instNum = iDag.instanceNumber();
    lMesh.getConnectedSetsAndMembers(instNum, sets, comps, false);

    // more than one set could add to creases, corners, and holes
    std::vector <int32_t> creaseIndices;
    std::vector <int32_t> creaseLengths;
    std::vector <float> creaseSharpness;

    std::vector <int32_t> cornerIndices;
    std::vector <float> cornerSharpness;

    std::vector <int32_t> holeIndices;

    // currently we are assuming that in Maya the sets won't change over time
    size_t i;
    for (i = 0; i < sets.length(); ++i)
    {
        MFnSet setFn(sets[i]);
        MFnSet::Restriction rest = setFn.restriction();

        if (rest == MFnSet::kEdgesOnly &&
            !setFn.findPlug("spSubdivCrease").isNull())
        {
            float weight = 0.0;
            setFn.findPlug("spSubdivCrease").getValue(weight);

            std::vector <int32_t> creaseIndices;

            MItMeshEdge edges(iDag, comps[i]);

            // we may want to have an optimization pass on the indices array
            // as we may have an extended edge of more than 2 points
            for (;!edges.isDone();edges.next())
            {
                creaseIndices.push_back(edges.index(0));
                creaseIndices.push_back(edges.index(1));
                creaseLengths.push_back(2);
                creaseSharpness.push_back(weight);
            }
            samp.setCreases(
                Alembic::Abc::Int32ArraySample(creaseIndices),
                Alembic::Abc::Int32ArraySample(creaseLengths),
                Alembic::Abc::FloatArraySample(creaseSharpness) );
        }
        else if (rest == MFnSet::kVerticesOnly &&
            !setFn.findPlug("spSubDivCorner").isNull())
        {
            float weight = 0.0;
            setFn.findPlug("spSubdivCorner").getValue(weight);
            MItMeshVertex verts(iDag, comps[i]);
            for (; !verts.isDone(); verts.next())
            {
                cornerIndices.push_back(verts.index());
                cornerSharpness.push_back(weight);
            }
            samp.setCorners(
                Alembic::Abc::Int32ArraySample(cornerIndices),
                Alembic::Abc::FloatArraySample(cornerSharpness) );
        }
        else if (rest == MFnSet::kFacetsOnly &&
            !setFn.findPlug("spSubdivHole").isNull())
        {
            float weight = 0.0;
            setFn.findPlug("spSubdivHole").getValue(weight);

            // find out if this weight is still meaningful
            // and if it is a good workflow going forward
            if (weight == 0.0)
                continue;

            MItMeshPolygon polys(iDag, comps[i]);
            for (;polys.isDone(); polys.next())
            {
                holeIndices.push_back(polys.index());
            }
            samp.setHoles(
                Alembic::Abc::Int32ArraySample(holeIndices) );
        }
    }

    // if this mesh is animated, write out the animated geometry first
    if (mIsGeometryAnimated)
    {
        mSubDSchema.set(samp,
            Alembic::Abc::OSampleSelector(mCurIndex++, iFrame/24.0));
    }
    else
    {
        mSubDSchema.set(samp);
    }
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

    if (pts.length() < 3)
    {
        MString err = "Cannot form valid mesh with ";
        err += pts.length();
        err += " points.";
        MGlobal::displayError(err);
        return;
    }

    unsigned int numPolys = lMesh.numPolygons();

    if (numPolys == 0)
    {
        MGlobal::displayWarning("lMesh.numPolygons() returned 0");
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
