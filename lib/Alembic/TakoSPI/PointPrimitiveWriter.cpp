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

#include <Alembic/TakoSPI/PointPrimitiveWriter.h>
#include <Alembic/TakoSPI/TransformWriter.h>
#include <Alembic/TakoSPI/NodeTypeId.h>
#include <Alembic/TakoSPI/PrivateUtil.h>

namespace Alembic {

namespace TakoSPI {

namespace {

enum PointPrimitiveState
{
    POINT_NOT_WRITTEN,
    POINT_STATIC,
    POINT_ANIMATED
};
}

class PointPrimitiveWriterIMPL::PrivateData
{
    public:
        PrivateData()
        {
            mWriteState = POINT_NOT_WRITTEN;
        }

        PointPrimitiveState mWriteState;
};

PointPrimitiveWriterIMPL::PointPrimitiveWriterIMPL(
    const std::string & iName,
    TransformWriterIMPL & iParent)
  : WriterGlue<TakoAbstractV1::PointPrimitiveWriter>(iName, iParent.getHid()),
    mData(new PrivateData())
{
    int typeVal = NODE_TYPE_POINT_PRIMITIVE;
    H5LTset_attribute_int(getGroup(), ".", "type", &typeVal, 1);

    int version = POINT_PRIMITIVE_INITIAL_VERSION;
    H5LTset_attribute_int(getGroup(), ".", "vers", &version, 1);
}

PointPrimitiveWriterIMPL::~PointPrimitiveWriterIMPL() {}

void PointPrimitiveWriterIMPL::write(float iFrame,
    const std::vector<float> & iPosition,
    const std::vector<float> & iVelocity,
    const std::vector<int> & iParticleIds,
    const std::vector<float> & iWidth)
{
    size_t posSize = iPosition.size();
    if ( posSize != iWidth.size()*3 ||
        (!iVelocity.empty() && posSize != iVelocity.size()) ||
        (!iParticleIds.empty() && posSize != iParticleIds.size() * 3))
    {
        throw std::logic_error(
            "Size discrepancy of either position/velocity/ids/width vector");
    }

    std::string name = "static";
    if (iFrame != FLT_MAX)
    {
        if (mData->mWriteState == POINT_STATIC)
        {
            throw std::logic_error(
                "Static point data has been written so can't write animated!");
        }

        name = floatToString(iFrame);

        // if this is our first animated write, set the sample hint
        if (mData->mWriteState == POINT_NOT_WRITTEN)
        {
            char samp = 1;
            H5LTset_attribute_char(getGroup(), ".", "samp", &samp, 1);
        }

        mData->mWriteState = POINT_ANIMATED;
    }
    else if (mData->mWriteState == POINT_ANIMATED)
    {
        throw std::logic_error(
            "Animated point data has been written so can't write static!");
    }
    else
    {
        mData->mWriteState = POINT_STATIC;
    }

    hid_t gid = H5Gcreate(getGroup(), name.c_str(), H5P_DEFAULT,
        H5P_DEFAULT, H5P_DEFAULT);

    herr_t ret = 0;

    // only write out the data if the point cloud has points in it!
    if (posSize != 0)
    {
        hsize_t dims[1];

        dims[0] = iPosition.size();
        const float * floatPtr = &(iPosition[0]);
        H5LTmake_dataset_float(gid, ".position", 1, dims, floatPtr);

        if (!iVelocity.empty())
        {
            floatPtr = &(iVelocity[0]);
            H5LTmake_dataset_float(gid, ".velocity", 1, dims, floatPtr);
        }

        if (!iParticleIds.empty())
        {
            dims[0] = iParticleIds.size();
            const int * intPtr = &(iParticleIds[0]);
            H5LTmake_dataset_int(gid, ".particleIds", 1, dims, intPtr);
        }

        floatPtr = &(iWidth[0]);
        ret = H5LTmake_dataset_float(gid, ".width", 1, dims, floatPtr);
    }

    // everything was ok
    if (ret > -1 && gid > -1)
    {
        H5Gclose(gid);
    }
    else
    {
        // there was a problem

        // the group id is still open, close it before throwing
        if (gid > -1)
            H5Gclose(gid);

        throw std::runtime_error("Error writing point primitives");
    }
}

//-*****************************************************************************
void PointPrimitiveWriterIMPL::write(float iFrame,
                                     const std::vector<float> & iPosition,
                                     const std::vector<float> & iVelocity,
                                     const std::vector<int> & iParticleIds,
                                     const float iConstantwidth)
{
    size_t posSize = iPosition.size();

    if ((!iVelocity.empty() &&
         posSize != iVelocity.size()) ||
        (iParticleIds.empty() &&
         posSize != iParticleIds.size()*3))
    {
        throw std::logic_error(
            "size difference of either position/velocity/particleIds vector");
    }

    std::string name = "static";
    if (iFrame != FLT_MAX)
    {
        if (mData->mWriteState == POINT_STATIC)
        {
            throw std::logic_error(
                "Static point data has been written so can't write animated!");
        }

        name = floatToString(iFrame);

        // if this is our first animated write, set the sample hint
        if (mData->mWriteState == POINT_NOT_WRITTEN)
        {
            char samp = 1;
            H5LTset_attribute_char(getGroup(), ".", "samp", &samp, 1);
        }

        mData->mWriteState = POINT_ANIMATED;
    }
    else if (mData->mWriteState == POINT_ANIMATED)
    {
        throw std::logic_error(
            "Animated points have been written so can't write static!");
    }
    else
    {
        mData->mWriteState = POINT_STATIC;
    }

    hid_t gid = H5Gcreate(getGroup(), name.c_str(), H5P_DEFAULT,
        H5P_DEFAULT, H5P_DEFAULT);

    herr_t ret = 0;

    if (posSize != 0)
    {
        hsize_t dims[1];

        dims[0] = iPosition.size();
        const float * floatPtr = &(iPosition[0]);
        H5LTmake_dataset_float(gid, ".position", 1, dims, floatPtr);

        if (!iVelocity.empty())
        {
            floatPtr = &(iVelocity[0]);
            H5LTmake_dataset_float(gid, ".velocity", 1, dims, floatPtr);
        }

        if (!iParticleIds.empty())
        {
            dims[0] = iParticleIds.size();
            const int * intPtr = &(iParticleIds[0]);
            H5LTmake_dataset_int(gid, ".particleIds", 1, dims, intPtr);
        }

        dims[0] = 1;
        floatPtr = &iConstantwidth;
        ret = H5LTmake_dataset_float(gid, ".constantwidth", 1,
            dims, floatPtr);
    }

    // everything was ok
    if (ret > -1 && gid > -1) {
        H5Gclose(gid);
    }
    else
    {
        // there was a problem

        // the group id is still open, close it before throwing
        if (gid > -1)
            H5Gclose(gid);

        throw std::runtime_error("Error writing point primitives");
    }
}

} // End namespace TakoSPI

} // End namespace Alembic
