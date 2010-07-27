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

#include <Alembic/TakoSPI/PointPrimitiveReader.h>
#include <Alembic/TakoSPI/TransformReader.h>
#include <Alembic/TakoSPI/PrivateUtil.h>

namespace Alembic {

namespace TakoSPI {

class PointPrimitiveReaderIMPL::PrivateData
{
    public:
        PrivateData()
        {
            mIsSampled = false;
            mVersion = -1;
        }
        bool mIsSampled;
        int  mVersion;
};

PointPrimitiveReaderIMPL::PointPrimitiveReaderIMPL(
    const std::string & iName,
    TransformReaderIMPL & iParent)
  : ReaderGlue<PointPrimitiveReader>(iName, iParent.getHid()),
    mData(new PrivateData())
{
    mConstantWidth = 0.0;

    if (H5Aexists(getGroup(), "vers") == 1)
    {
        hid_t aid = H5Aopen(getGroup(), "vers", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &mData->mVersion);
        H5Aclose(aid);
    }

    if (H5Aexists(getGroup(), "samp") == 1)
    {
        char samp = 0;
        hid_t aid = H5Aopen(getGroup(), "samp", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_CHAR, &samp);
        H5Aclose(aid);
        mData->mIsSampled = (samp != 0);
    }
}

PointPrimitiveReaderIMPL::~PointPrimitiveReaderIMPL() {}

void PointPrimitiveReaderIMPL::getFrames(std::set<float>& ioFrames) const
{
    H5Literate(getGroup(), H5_INDEX_NAME, H5_ITER_INC, NULL, GetFrames,
        &ioFrames);
}

bool PointPrimitiveReaderIMPL::hasFrames() const
{
    return mData->mIsSampled;
}

unsigned int PointPrimitiveReaderIMPL::read(float iFrame)
{
    std::string name = "static";

    if (mData->mIsSampled)
    {
        name = floatToString(iFrame);
    }

    if (!mData->mIsSampled && !mPosition.empty())
        return POINT_STATIC;

    hid_t gid = H5Gopen2(getGroup(), name.c_str(), H5P_DEFAULT);

    if (gid < 0)
        return READ_ERROR;

    if (H5Lexists(gid, ".position", H5P_DEFAULT) == 1)
    {
        hid_t did = H5Dopen2(gid, ".position", H5P_DEFAULT);
        hid_t sid = H5Dget_space(did);
        hid_t tid = H5Dget_type(did);

        hsize_t dims[1];
        H5Sget_simple_extent_dims(sid, dims, NULL);
        mPosition.resize(dims[0]);
        float * fPtr = &(mPosition[0]);
        H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
        H5Tclose(tid);
        H5Sclose(sid);
        H5Dclose(did);

        if (H5Lexists(gid, ".velocity", H5P_DEFAULT) == 1)
        {
            did = H5Dopen2(gid, ".velocity", H5P_DEFAULT);
            sid = H5Dget_space(did);
            tid = H5Dget_type(did);


            H5Sget_simple_extent_dims(sid, dims, NULL);
            mVelocity.resize(dims[0]);
            fPtr = &(mVelocity[0]);
            H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
            H5Tclose(tid);
            H5Sclose(sid);
            H5Dclose(did);
        }
        else
        {
            mVelocity.clear();
        }

        if (H5Lexists(gid, ".particleIds", H5P_DEFAULT) == 1)
        {
            did = H5Dopen2(gid, ".particleIds", H5P_DEFAULT);
            sid = H5Dget_space(did);
            tid = H5Dget_type(did);

            H5Sget_simple_extent_dims(sid, dims, NULL);
            mParticleIds.resize(dims[0]);
            int * iPtr = &(mParticleIds[0]);
            H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, iPtr);
            H5Tclose(tid);
            H5Sclose(sid);
            H5Dclose(did);
        }
        else
        {
            mParticleIds.clear();
        }

        if (H5Lexists(gid, ".width", H5P_DEFAULT) == 1)
        {
            mConstantWidth = 0.0;

            did = H5Dopen2(gid, ".width", H5P_DEFAULT);
            sid = H5Dget_space(did);
            tid = H5Dget_type(did);

            H5Sget_simple_extent_dims(sid, dims, NULL);
            mWidth.resize(dims[0]);
            fPtr = &(mWidth[0]);
            H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
            H5Tclose(tid);
            H5Sclose(sid);
            H5Dclose(did);
        }
        else if (H5Lexists(gid, ".constantwidth", H5P_DEFAULT) == 1)
        {
            mWidth.clear();
            did = H5Dopen2(gid, ".constantwidth", H5P_DEFAULT);
            sid = H5Dget_space(did);
            tid = H5Dget_type(did);

            H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, &mConstantWidth);
            H5Tclose(tid);
            H5Sclose(sid);
            H5Dclose(did);
        }
    }
    else
    {
        mConstantWidth = 0.0;
        mPosition.clear();
        mVelocity.clear();
        mParticleIds.clear();
        mWidth.clear();
    }

    if (gid > -1)
        H5Gclose(gid);

    if (mData->mIsSampled)
        return POINT_ANIMATED;
    else
        return POINT_STATIC;
}

} // End namespace TakoSPI

} // End namespace Alembic


