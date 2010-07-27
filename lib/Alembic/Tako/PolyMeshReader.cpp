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

#include <Alembic/Tako/PolyMeshReader.h>
#include <Alembic/Tako/TransformReader.h>
#include <Alembic/Tako/PrivateUtil.h>

namespace Alembic {

using Tako::PolyMeshReader;

class Tako::PolyMeshReader::PrivateData
{
    public:
        PrivateData()
        {
            mLastFacePointsLink = 0;
            mVersion = -1;
            mIsSampled = false;
        }

        haddr_t mLastFacePointsLink;
        int mVersion;
        bool mIsSampled;
};

PolyMeshReader::PolyMeshReader(const char * iName, TransformReader & iParent)
    : HDFReaderNode(iName, iParent.getHid()), mData(new PrivateData())
{
    if (H5Aexists(mGroup, "vers") == 1)
    {
        hid_t aid = H5Aopen(mGroup, "vers", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &mData->mVersion);
        H5Aclose(aid);
    }

    if (H5Aexists(mGroup, "samp") == 1)
    {
        char samp = 0;
        hid_t aid = H5Aopen(mGroup, "samp", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_CHAR, &samp);
        H5Aclose(aid);
        mData->mIsSampled = (samp != 0);
    }
}

PolyMeshReader::~PolyMeshReader() {}

void PolyMeshReader::getFrames(std::set<float>& ioFrames) const
{
    H5Literate(mGroup, H5_INDEX_NAME, H5_ITER_INC, NULL, GetFrames,
        &ioFrames);
}

bool PolyMeshReader::hasFrames() const
{
    return mData->mIsSampled;
}


PolyMeshReader::TopologyType PolyMeshReader::read(float iFrame)
{
    std::string name = "static";

    if (mData->mIsSampled)
    {
        name = floatToString(iFrame);
    }

    // if we have already read the point data for static then we don't
    // need to do it again
    if (!mData->mIsSampled && !mPoints.empty())
        return TOPOLOGY_STATIC;

    hid_t gid = H5Gopen2(mGroup, name.c_str(), H5P_DEFAULT);

    if (gid < 0)
    {
        return READ_ERROR;
    }

    // read the points
    hid_t did = H5Dopen2(gid, ".points", H5P_DEFAULT);
    hid_t sid = H5Dget_space(did);
    hid_t tid = H5Dget_type(did);

    hsize_t dims[1];
    H5Sget_simple_extent_dims(sid, dims, NULL);
    mPoints.resize(dims[0]);
    float * fPtr = &mPoints[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    // read the normals if exist
    if (H5Lexists(gid, ".normals", H5P_DEFAULT) == 1)
    {
        did = H5Dopen2(gid, ".normals", H5P_DEFAULT);
        if (did >= 0)
        {
            sid = H5Dget_space(did);
            tid = H5Dget_type(did);
            H5Sget_simple_extent_dims(sid, dims, NULL);
            mNormals.resize(dims[0]);
            fPtr = &mNormals[0];
            H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
            H5Tclose(tid);
            H5Sclose(sid);
            H5Dclose(did);
        }
    }
    else
    {
        mNormals.clear();
    }

    // now read the face points and face list, only if neccessary
    H5L_info_t info;
    H5Lget_info(gid, ".facePoints", &info, H5P_DEFAULT);
    if (info.type == H5L_TYPE_HARD && info.u.address ==
        mData->mLastFacePointsLink)
    {
        H5Gclose(gid);
        return TOPOLOGY_HOMOGENOUS;
    }

    did = H5Dopen2(gid, ".facePoints", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    mFacePoints.resize(dims[0]);
    index_t * iPtr = &mFacePoints[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, iPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);


    did = H5Dopen2(gid, ".faceList", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    mFaceList.resize(dims[0]);
    iPtr = &mFaceList[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, iPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    // CJH: The logic here is:
    // By default, the mesh is heterogenous.
    // If the mesh is not sampled (static only), it will be
    // TOPOLOGY_STATIC.
    // If the mesh is sampled, we look to see what the
    // address of the previous load's face points thingy was.
    // If it was zero, we're either loading the first sample,
    // in which case we assume TOPOLOGY_HOMOGENEOUS.
    // If it was not zero, we assume TOPOLOGY_HETEROGENOUS
    // because we actually got down this far, and there had been
    // no extra faceList or facePoints data, we would have returned
    // above.

    TopologyType rType = TOPOLOGY_HETEROGENOUS;

    if (!mData->mIsSampled)
        rType = TOPOLOGY_STATIC;

    // not static but nothing has been loaded previously so homogenous
    else if (mData->mLastFacePointsLink == 0)
        rType = TOPOLOGY_HOMOGENOUS;

    if (info.type == H5L_TYPE_HARD)
        mData->mLastFacePointsLink = info.u.address;

    H5Gclose(gid);
    return rType;
}

} // End namespace Alembic

