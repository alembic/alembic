//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#include <Alembic/Tako/SubDWriter.h>
#include <Alembic/Tako/TransformWriter.h>
#include <Alembic/Tako/NodeTypeId.h>
#include <Alembic/Tako/PrivateUtil.h>

namespace Alembic {

using Tako::SubDWriter;

namespace {

// helper function for writing a single int attribute
void writeIntAttr(hid_t iGroup, const char * iName, int iValue)
{
    hid_t sid = H5Screate(H5S_SCALAR);
    hid_t aid = H5Acreate2(iGroup, iName,
        H5T_NATIVE_INT, sid, H5P_DEFAULT, H5P_DEFAULT);

    H5Awrite(aid, H5T_NATIVE_INT, &iValue);
    H5Sclose(sid);
    H5Aclose(aid);
}

void verifyGeometryLists(Tako::index_t iNumValues,
    const std::vector<Tako::index_t> & iFacePoints,
    const std::vector<Tako::index_t> & iFaceList)
{
    // make sure there are enough values for at least 1 polygon and
    // make sure the vectors of points are correctly aligned
    if (iNumValues < 9 || iNumValues % 3 != 0)
        throw std::logic_error("Bad point count!");

    if (iFacePoints.size() < 3)
        throw std::logic_error("Not enough face points!");

    std::vector<Tako::index_t>::const_iterator i, end;
    for (i = iFacePoints.begin(), end = iFacePoints.end(); i != end; ++i)
    {
        if (*i > iNumValues/3)
            throw std::logic_error("Bad face point!");
    }

    if (iFaceList.empty())
        throw std::logic_error("Bad face index!");

    i = iFaceList.begin();
    Tako::index_t lastIndex = *i;
    Tako::index_t maxIndex = iFacePoints.size();
    i++;
    for (end = iFaceList.end(); i != end; ++i)
    {
        Tako::index_t curIndex = *i;
        if (curIndex > maxIndex || lastIndex + 2 >= curIndex)
            throw std::logic_error("Bad face index!");

        lastIndex = curIndex;
    }
}


void verifyCreases(Tako::index_t iNumGeoPoints, Tako::index_t iNumValues,
    const std::vector<Tako::index_t> & iIndices,
    const std::vector<Tako::index_t> & iLengths)
{
    if (iNumValues != iLengths.size())
        throw std::logic_error(
            "Size of crease lengths and sharpness do not match!");

    std::vector<Tako::index_t>::const_iterator i, end;
    for (i = iIndices.begin(), end = iIndices.end(); i != end; ++i)
    {
        if (*i > iNumGeoPoints)
            throw std::logic_error("Bad crease index!");
    }

    Tako::index_t total = 0;
    for (i = iLengths.begin(), end = iLengths.end(); i != end; ++i)
    {
        // need 2 points to form an edge
        if (*i < 2)
            throw std::logic_error("Bad crease length!");

        total += *i;
    }

    if (total != iIndices.size())
        throw std::logic_error(
            "Total crease lengths don't cover all of the crease indeces.");
}

void verifyCorners(Tako::index_t iNumGeoPoints, Tako::index_t iNumValues,
    const std::vector<Tako::index_t> & iIndices)
{
    if (iNumValues != iIndices.size())
    {
        throw std::logic_error(
            "Size of corner indices and sharpness do not match!");
    }

    std::vector<Tako::index_t>::const_iterator i, end;
    for (i = iIndices.begin(), end = iIndices.end(); i != end; ++i)
    {
        if (*i > iNumGeoPoints)
            throw std::logic_error("Bad corner index!");
    }
}

void verifyHolePolyIndices(Tako::index_t iNumFacePoints,
    const std::vector<Tako::index_t> & iIndices)
{
    std::vector<Tako::index_t>::const_iterator i, end;
    for (i = iIndices.begin(), end = iIndices.end(); i != end; ++i)
    {
        if (*i > iNumFacePoints)
            throw std::logic_error("Bad hole poly index!");
    }
}

enum MeshState
{
    MESH_NOT_WRITTEN,
    MESH_STATIC,
    MESH_ANIMATED
};
}

class  Tako::SubDWriter::PrivateData
{
    public:
        PrivateData()
        {
            mGeometryNumVals = 0;
            mGeometryNumFaces = 0;
            mGeometryLastFrameId = -1;
            mGeometryWriteState = MESH_NOT_WRITTEN;

            mCreasesNumVals = 0;
            mCreasesLastFrameId = -1;
            mCreasesWriteState = MESH_NOT_WRITTEN;

            mCornersNumVals = 0;
            mCornersLastFrameId = -1;
            mCornersWriteState = MESH_NOT_WRITTEN;

            mHolePolyIndicesWriteState = MESH_NOT_WRITTEN;

            mCurrentFrame = FLT_MAX;
            mStartCalled = false;
        }

        index_t mGeometryNumVals;
        index_t mGeometryNumFaces;
        hid_t mGeometryLastFrameId;
        MeshState mGeometryWriteState;

        index_t mCreasesNumVals;
        hid_t mCreasesLastFrameId;
        MeshState mCreasesWriteState;

        index_t mCornersNumVals;
        hid_t mCornersLastFrameId;
        MeshState mCornersWriteState;

        MeshState mHolePolyIndicesWriteState;

        float mCurrentFrame;
        bool mStartCalled;
};

SubDWriter::SubDWriter(const char * iName, TransformWriter & iParent)
    : HDFWriterNode(iName, iParent.getHid()), mData(new PrivateData())
{
    int typeVal = NODE_TYPE_SUBD;
    H5LTset_attribute_int(mGroup, ".", "type", &typeVal, 1);

    int version = SUBD_INITIAL_VERSION;
    H5LTset_attribute_int(mGroup, ".", "vers", &version, 1);
}

SubDWriter::~SubDWriter()
{
    if (mData->mGeometryLastFrameId > -1)
        H5Gclose(mData->mGeometryLastFrameId);

    if (mData->mCreasesLastFrameId > -1)
        H5Gclose(mData->mCreasesLastFrameId);

    if (mData->mCornersLastFrameId > -1)
        H5Gclose(mData->mCornersLastFrameId);
}

void SubDWriter::start(float iFrame)
{
    if (mData->mStartCalled)
        throw std::logic_error("start already called, please call end first.");

    mData->mStartCalled = true;
    mData->mCurrentFrame = iFrame;
}

void SubDWriter::end()
{
    if (!mData->mStartCalled)
        throw std::logic_error("end can not be called before start.");

    mData->mStartCalled = false;
}

void SubDWriter::writeFaceVaryingInterpolateBoundary(int iValue) const
{
    writeIntAttr(mGroup, "faceVaryingInterpolateBoundary", iValue);
}

void SubDWriter::writeFaceVaryingPropagateCorners(int iValue) const
{
    writeIntAttr(mGroup, "faceVaryingPropagateCorners", iValue);
}

void SubDWriter::writeInterpolateBoundary(int iValue) const
{
    writeIntAttr(mGroup, "interpolateBoundary", iValue);
}

void SubDWriter::writeGeometry(const std::vector<float> & iPoints,
    const std::vector<index_t> & iFacePoints,
    const std::vector<index_t> & iFaceList)
{
    if (!mData->mStartCalled)
        throw std::logic_error("start must be called before writing geometry.");

    std::string name = "static";
    if (mData->mCurrentFrame != FLT_MAX)
    {
        if (mData->mGeometryWriteState == MESH_STATIC)
        {
            throw std::logic_error(
                "Static subd has been written so can't write animated!");
        }

        name = floatToString(mData->mCurrentFrame);

        // write the geo animated hint only if it is animated, and this is
        // our first animated write
        if (mData->mGeometryWriteState == MESH_NOT_WRITTEN)
        {
            char val = 2;
            H5LTset_attribute_char(mGroup, ".", "geo", &val, 1);
        }

        mData->mGeometryWriteState = MESH_ANIMATED;
    }
    else if (mData->mGeometryWriteState == MESH_ANIMATED)
    {
        // we are trying to write static data, but animated data already exists
        throw std::logic_error(
            "Animated subd has been written so can't write static!");
    }
    else
    {
        mData->mGeometryWriteState = MESH_STATIC;
    }

    index_t numVals = iPoints.size();

    verifyGeometryLists(numVals, iFacePoints, iFaceList);
    mData->mGeometryNumVals = numVals;
    mData->mGeometryNumFaces = iFaceList.size();

    if (mData->mGeometryLastFrameId > -1)
        H5Gclose(mData->mGeometryLastFrameId);

    mData->mGeometryLastFrameId = H5Gcreate(mGroup, name.c_str(),
        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    hsize_t dims[1];
    dims[0] = numVals;

    const float * fPtr = &iPoints[0];
    H5LTmake_dataset_float(mData->mGeometryLastFrameId, ".points", 1, dims,
        fPtr);

    const int32_t * facePtr = reinterpret_cast<const int32_t *>
        (&iFacePoints.front());

    dims[0] = iFacePoints.size();
    H5LTmake_dataset_int(mData->mGeometryLastFrameId, ".facePoints", 1,
        dims, facePtr);

    const int32_t * faceListPtr = reinterpret_cast<const int32_t *>
        (&iFaceList.front());

    dims[0] = iFaceList.size();
    herr_t ret = H5LTmake_dataset_int(mData->mGeometryLastFrameId,
        ".faceList", 1, dims, faceListPtr);
    if (ret < 0)
    {
        throw std::runtime_error("Error writing subD mesh geometry info");
    }
}

// .facePoints and .faceList is reused from the previous write
void SubDWriter::writeGeometry(const std::vector<float> & iPoints) const
{
    if (!mData->mStartCalled)
        throw std::logic_error("start must be called before writing geometry.");

    if (mData->mGeometryLastFrameId <= -1)
        throw std::logic_error("Topology has not yet been written!");

    // can't have both static and animated data
    if (mData->mCurrentFrame == FLT_MAX)
        throw std::logic_error("Can not write static geometry data!");

    index_t numVals = iPoints.size();
    if (numVals != mData->mGeometryNumVals)
        throw std::logic_error("Point counts have changed!");

    hid_t gid = H5Gcreate(mGroup, floatToString(mData->mCurrentFrame).c_str(),
        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    hsize_t dims[1];
    dims[0] = numVals;

    const float * fPtr = &iPoints[0];
    herr_t ret = H5LTmake_dataset_float(gid, ".points", 1, dims, fPtr);
    if (ret < 0)
    {
        throw std::runtime_error("Error writing subD mesh geometry info");
    }

    H5Lcreate_hard(mData->mGeometryLastFrameId, ".facePoints", gid,
        ".facePoints",  H5P_DEFAULT, H5P_DEFAULT);

    H5Lcreate_hard(mData->mGeometryLastFrameId, ".faceList", gid, ".faceList",
        H5P_DEFAULT, H5P_DEFAULT);

    H5Gclose(gid);
}

void SubDWriter::writeCreases(const std::vector<float> & iSharpness,
            const std::vector<index_t> & iIndices,
            const std::vector<index_t> & iLengths)
{
    if (!mData->mStartCalled)
        throw std::logic_error("start must be called before writing creases.");

    if (mData->mGeometryLastFrameId == -1)
        throw std::logic_error("Geometry must be written before creases.");

    std::string name = "static";
    if (mData->mCurrentFrame != FLT_MAX)
    {
        if (mData->mCreasesWriteState == MESH_STATIC)
        {
            throw std::logic_error(
                "Static crease has been written so can't write animated!");
        }

        name = floatToString(mData->mCurrentFrame);

        mData->mCreasesWriteState = MESH_ANIMATED;
    }
    else if (mData->mCreasesWriteState == MESH_ANIMATED)
    {
        // we are trying to write static data, but animated data already exists
        throw std::logic_error(
            "Animated crease has been written so can't write static!");
    }
    else
    {
        mData->mCreasesWriteState = MESH_STATIC;
    }

    index_t numVals = iSharpness.size();
    verifyCreases(mData->mGeometryNumVals / 3, numVals, iIndices, iLengths);
    mData->mCreasesNumVals = numVals;

    if (mData->mCreasesLastFrameId > -1)
    {
        H5Gclose(mData->mCreasesLastFrameId);
    }
    else
    {
        // first time writing the crease info, set the hint attribute

        char val = 1;
        if (mData->mCurrentFrame != FLT_MAX)
        {
            val = 2;
        }
        H5LTset_attribute_char(mGroup, ".", "crease", &val, 1);
    }

    if (H5Lexists(mGroup, name.c_str(), H5P_DEFAULT) == 0)
    {
        mData->mCreasesLastFrameId = H5Gcreate(mGroup, name.c_str(),
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    else
    {
        mData->mCreasesLastFrameId = H5Gopen(mGroup, name.c_str(), H5P_DEFAULT);
    }

    hsize_t dims[1];
    dims[0] = numVals;

    const float * floatPtr = &iSharpness[0];
    H5LTmake_dataset_float(mData->mCreasesLastFrameId, ".creaseSharpness", 1,
        dims, floatPtr);


    const int32_t * indicesPtr = reinterpret_cast<const int32_t *>
        (&iIndices.front());

    dims[0] = iIndices.size();
    H5LTmake_dataset_int(mData->mCreasesLastFrameId, ".creaseIndices", 1,
        dims, indicesPtr);

    const int32_t * lengthPtr = reinterpret_cast<const int32_t *>
        (&iLengths.front());

    dims[0] = iLengths.size();
    herr_t ret = H5LTmake_dataset_int(mData->mCreasesLastFrameId,
        ".creaseLengths", 1, dims, lengthPtr);

    if (ret < 0)
    {
        throw std::runtime_error("Error writing subD mesh crease info");
    }
}

void SubDWriter::writeCreases(const std::vector<float> & iSharpness) const
{
    if (!mData->mStartCalled)
        throw std::logic_error("start must be called before writing creases.");

    if (mData->mGeometryLastFrameId < 0)
        throw std::logic_error("Geometry must be written before creases.");

    if (mData->mCreasesLastFrameId < 0)
        throw std::logic_error("Crease topology has not yet been written!");

    // can't have both static and animated data
    if (mData->mCurrentFrame == FLT_MAX)
        throw std::logic_error("Can not write static data!");

    index_t numVals = iSharpness.size();
    if (numVals != mData->mCreasesNumVals)
        throw std::logic_error(
            "Number of crease sharpness values have changed!");

    hid_t gid = -1;

    std::string name = floatToString(mData->mCurrentFrame);
    if (H5Lexists(mGroup, name.c_str(), H5P_DEFAULT) == 0)
    {
        gid = H5Gcreate(mGroup, name.c_str(), H5P_DEFAULT, H5P_DEFAULT,
            H5P_DEFAULT);
    }
    else
    {
        gid = H5Gopen(mGroup, name.c_str(), H5P_DEFAULT);
    }

    hsize_t dims[1];
    dims[0] = numVals;

    const float * floatPtr = &iSharpness[0];
    herr_t ret = H5LTmake_dataset_float(gid, ".creaseSharpness", 1, dims,
        floatPtr);

    if (ret < 0)
    {
        throw std::runtime_error("Error writing subD mesh crease sharpness");
    }

    H5Lcreate_hard(mData->mCreasesLastFrameId, ".creaseLengths", gid,
        ".creaseLengths",  H5P_DEFAULT, H5P_DEFAULT);

    H5Lcreate_hard(mData->mCreasesLastFrameId, ".creaseIndices", gid,
        ".creaseIndices",  H5P_DEFAULT, H5P_DEFAULT);

    H5Gclose(gid);
}

void SubDWriter::writeCorners(const std::vector<float> & iSharpness,
    const std::vector<index_t> & iIndices)
{
    if (!mData->mStartCalled)
        throw std::logic_error("start must be called before writing corners.");

    if (mData->mGeometryLastFrameId == -1)
        throw std::logic_error("Geometry must be written before corners.");

    std::string name = "static";
    if (mData->mCurrentFrame != FLT_MAX)
    {
        if (mData->mCornersWriteState == MESH_STATIC)
        {
            throw std::logic_error(
                "Static corner has been written so can't write animated!");
        }

        name = floatToString(mData->mCurrentFrame);

        mData->mCornersWriteState = MESH_ANIMATED;
    }
    else if (mData->mCornersWriteState == MESH_ANIMATED)
    {
        // we are trying to write static data, but animated data already exists
        throw std::logic_error(
            "Animated corner has been written so can't write static!");
    }
    else
    {
        mData->mCornersWriteState = MESH_STATIC;
    }

    index_t numVals = iSharpness.size();
    verifyCorners(mData->mGeometryNumVals / 3, numVals, iIndices);
    mData->mCornersNumVals = numVals;

    if (mData->mCornersLastFrameId > -1)
    {
        H5Gclose(mData->mCornersLastFrameId);
    }
    else
    {
        // first time writing the corner info, set the hint attribute

        char val = 1;
        if (mData->mCurrentFrame != FLT_MAX)
        {
            val = 2;
        }
        H5LTset_attribute_char(mGroup, ".", "corner", &val, 1);
    }

    if (H5Lexists(mGroup, name.c_str(), H5P_DEFAULT) == 0)
    {
        mData->mCornersLastFrameId = H5Gcreate(mGroup, name.c_str(),
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    else
    {
        mData->mCornersLastFrameId = H5Gopen(mGroup, name.c_str(), H5P_DEFAULT);
    }

    hsize_t dims[1];
    dims[0] = numVals;

    const float * floatPtr = &iSharpness[0];
    H5LTmake_dataset_float(mData->mCornersLastFrameId, ".cornerSharpness",
        1, dims, floatPtr);

    const int32_t * indicesPtr = reinterpret_cast<const int32_t *>
        (&iIndices.front());

    dims[0] = iIndices.size();
    herr_t ret = H5LTmake_dataset_int(mData->mCornersLastFrameId,
        ".cornerIndices", 1, dims, indicesPtr);
    if (ret < 0)
    {
        throw std::runtime_error("Error writing subD mesh corner info");
    }
}

void SubDWriter::writeCorners(const std::vector<float> & iSharpness) const {
    if (!mData->mStartCalled)
        throw std::logic_error("start must be called before writing creases.");

    if (mData->mGeometryLastFrameId < 0)
        throw std::logic_error("Geometry must be written before corners.");

    if (mData->mCornersLastFrameId < 0)
        throw std::logic_error("Corner indices have not yet been written!");

    // can't have both static and animated data
    if (mData->mCurrentFrame == FLT_MAX)
        throw std::logic_error("Can not write static corner data!");

    index_t numVals = iSharpness.size();
    if (numVals != mData->mCornersNumVals)
        throw std::logic_error(
            "Number of corner sharpness values have changed!");

    std::string name = floatToString(mData->mCurrentFrame);
    hid_t gid = -1;

    if (H5Lexists(mGroup, name.c_str(), H5P_DEFAULT) == 0)
    {
        gid = H5Gcreate(mGroup, floatToString(mData->mCurrentFrame).c_str(),
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    else
    {
        gid = H5Gopen(mGroup, name.c_str(), H5P_DEFAULT);
    }

    hsize_t dims[1];
    dims[0] = numVals;

    const float * floatPtr = &iSharpness[0];
    herr_t ret = H5LTmake_dataset_float(gid, ".cornerSharpness", 1, dims,
        floatPtr);
    if (ret < 0)
    {
        throw std::runtime_error("Error writing subD mesh corner sharpness");
    }

    H5Lcreate_hard(mData->mCornersLastFrameId, ".cornerIndices", gid,
        ".cornerIndices",  H5P_DEFAULT, H5P_DEFAULT);

    H5Gclose(gid);
}

void SubDWriter::writeHolePolyIndices(const std::vector<index_t> & iIndices) {
    if (!mData->mStartCalled)
    {
        throw std::logic_error(
            "start must be called before writing hole poly indices.");
    }

    if (mData->mGeometryLastFrameId < 0)
    {
        throw std::logic_error(
            "Geometry must be written before hole poly indices.");
    }
    bool notWritten = (mData->mHolePolyIndicesWriteState == MESH_NOT_WRITTEN);

    std::string name = "static";
    if (mData->mCurrentFrame != FLT_MAX)
    {
        if (mData->mHolePolyIndicesWriteState == MESH_STATIC)
            throw std::logic_error(
                "Static hole has been written so can't write animated!");

        name = floatToString(mData->mCurrentFrame);

        mData->mHolePolyIndicesWriteState = MESH_ANIMATED;
    }
    else if (mData->mHolePolyIndicesWriteState == MESH_ANIMATED)
    {
        // we are trying to write static data, but animated data already exists
        throw std::logic_error(
            "Animated hole has been written so can't write static!");
    }
    else
    {
        mData->mHolePolyIndicesWriteState = MESH_STATIC;
    }

    // first time writing anything, set the hole hint
    if (notWritten)
    {
        char val = 1;
        if (mData->mCurrentFrame != FLT_MAX)
        {
            val = 2;
        }
        H5LTset_attribute_char(mGroup, ".", "hole", &val, 1);
    }

    verifyHolePolyIndices(mData->mGeometryNumFaces, iIndices);

    hid_t gid = -1;
    if (H5Lexists(mGroup, name.c_str(), H5P_DEFAULT) == 0)
    {
        gid = H5Gcreate(mGroup, name.c_str(),
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    else
    {
        gid = H5Gopen(mGroup, name.c_str(), H5P_DEFAULT);
    }

    hsize_t dims[1];
    const int32_t * indicesPtr = reinterpret_cast<const int32_t *>
        (&iIndices.front());

    dims[0] = iIndices.size();
    herr_t ret = H5LTmake_dataset_int(gid, ".holePolyIndices", 1, dims,
        indicesPtr);
    if (ret < 0)
    {
        throw std::runtime_error(
            "Error writing subD mesh hole poly indices");
    }

    H5Gclose(gid);
}

} // End namespace Alembic
