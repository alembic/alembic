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

#include <Alembic/Tako/PolyMeshWriter.h>
#include <Alembic/Tako/TransformWriter.h>
#include <Alembic/Tako/NodeTypeId.h>
#include <Alembic/Tako/PrivateUtil.h>

namespace Alembic {

using Tako::PolyMeshWriter;

namespace {

void verifyLists(Tako::index_t iNumValues,
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

enum MeshState
{
    MESH_NOT_WRITTEN,
    MESH_STATIC,
    MESH_ANIMATED
};
}

class PolyMeshWriter::PrivateData
{
    public:
        PrivateData()
        {
            mNumVals = 0;
            mLastFrameId = -1;
            mWriteState = MESH_NOT_WRITTEN;
        }

        index_t mNumVals;
        hid_t mLastFrameId;
        MeshState mWriteState;
};

PolyMeshWriter::PolyMeshWriter(const char * iName, TransformWriter & iParent)
    : HDFWriterNode(iName, iParent.getHid()), mData(new PrivateData())
{
    int typeVal = NODE_TYPE_POLYMESH;
    H5LTset_attribute_int(mGroup, ".", "type", &typeVal, 1);

    int version = POLYMESH_INITIAL_VERSION;
    H5LTset_attribute_int(mGroup, ".", "vers", &version, 1);
}

PolyMeshWriter::~PolyMeshWriter()
{
    if (mData->mLastFrameId > -1)
        H5Gclose(mData->mLastFrameId);
}


void PolyMeshWriter::write(float iFrame,
    const std::vector<float> & iPoints,
    const std::vector<float> & iNormals,
    const std::vector<index_t> & iFacePoints,
    const std::vector<index_t> & iFaceList)
{
    std::string name = "static";
    if (iFrame != FLT_MAX)
    {
        name = floatToString(iFrame);

        if (mData->mWriteState == MESH_STATIC)
        {
            throw std::logic_error(
                "Static data has been written so can not write animated data!");
        }

        // if this is our first animated write, set the sample hint
        if (mData->mWriteState == MESH_NOT_WRITTEN)
        {
            char samp = 1;
            H5LTset_attribute_char(mGroup, ".", "samp", &samp, 1);
        }

        mData->mWriteState = MESH_ANIMATED;
    }
    else if (mData->mWriteState == MESH_ANIMATED)
    {
        // we are trying to write static data, but animated data already exists
        throw std::logic_error(
            "Animated data has been written so can not write static data!");
    }
    else
    {
        mData->mWriteState = MESH_STATIC;
    }

    index_t numVals = iPoints.size();

    verifyLists(numVals, iFacePoints, iFaceList);
    mData->mNumVals = numVals;

    // make sure the old last frame gets closed, before assigning a new value
    if (mData->mLastFrameId > -1)
        H5Gclose(mData->mLastFrameId);

    mData->mLastFrameId = H5Gcreate(mGroup, name.c_str(),
        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    hsize_t dims[1];
    dims[0] = numVals;

    const float * floatPtr = &iPoints[0];
    H5LTmake_dataset_float(mData->mLastFrameId, ".points", 1, dims, floatPtr);

    if (iNormals.size() > 0)
    {
        floatPtr = &iNormals[0];
        dims[0] = iNormals.size();
        H5LTmake_dataset_float(mData->mLastFrameId, ".normals", 1, dims,
            floatPtr);
    }

    const int32_t * facePtr = reinterpret_cast<const int32_t *>(
        &(iFacePoints.front()) );

    dims[0] = iFacePoints.size();
    H5LTmake_dataset_int(mData->mLastFrameId, ".facePoints", 1, dims, facePtr);

    const int32_t * faceListPtr = reinterpret_cast<const int32_t *>(
        &(iFaceList.front()) );

    dims[0] = iFaceList.size();
    herr_t ret = H5LTmake_dataset_int(mData->mLastFrameId, ".faceList", 1,
        dims, faceListPtr);

    if (ret < 0)
    {
        throw std::runtime_error("Error writing poly mesh");
    }
}

// .facePoints and .faceList is reused from the previous write
void PolyMeshWriter::write(
    float iFrame,
    const std::vector<float> & iPoints,
    const std::vector<float> & iNormals) const
{
    if (mData->mLastFrameId <= -1)
        throw std::logic_error("Topology has not yet been written!");

    // can't have both static and animated data
    if (iFrame == FLT_MAX)
        throw std::logic_error("Can not write static data!");

    index_t numVals = iPoints.size();
    if (numVals != mData->mNumVals)
        throw std::logic_error("Point counts has changed!");

    hid_t gid = H5Gcreate(mGroup, floatToString(iFrame).c_str(),
        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    hsize_t dims[1];
    dims[0] = numVals;

    const float * floatPtr = &iPoints[0];
    herr_t ret = H5LTmake_dataset_float(gid, ".points", 1, dims, floatPtr);
    if (ret < 0)
    {
        throw std::runtime_error("Error writing poly mesh points");
    }

    if (iNormals.size() > 0)
    {
        floatPtr = &iNormals[0];
        dims[0] = iNormals.size();
        ret = H5LTmake_dataset_float(gid, ".normals", 1, dims, floatPtr);
        if (ret < 0)
        {
            throw std::runtime_error("Error writing poly mesh normals");
        }
    }

    H5Lcreate_hard(mData->mLastFrameId, ".facePoints", gid, ".facePoints",
        H5P_DEFAULT, H5P_DEFAULT);

    H5Lcreate_hard(mData->mLastFrameId, ".faceList", gid, ".faceList",
        H5P_DEFAULT, H5P_DEFAULT);

    H5Gclose(gid);
}

} // End namespace Alembic
