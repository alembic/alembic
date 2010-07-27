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

#include <Alembic/TakoSPI/TransformWriter.h>
#include <Alembic/TakoSPI/NodeTypeId.h>
#include <Alembic/TakoSPI/PrivateUtil.h>

#include <Alembic/TakoSPI/PolyMeshWriter.h>
#include <Alembic/TakoSPI/SubDWriter.h>
#include <Alembic/TakoSPI/NurbsCurveWriter.h>
#include <Alembic/TakoSPI/NurbsSurfaceWriter.h>
#include <Alembic/TakoSPI/CameraWriter.h>
#include <Alembic/TakoSPI/PointPrimitiveWriter.h>
#include <Alembic/TakoSPI/GenericNodeWriter.h>

namespace Alembic {

namespace TakoSPI {

namespace {

class ReturnMatrixVisitor : public boost::static_visitor< Mat4x4 >
{
    public:
        ReturnMatrixVisitor() {}

        Mat4x4 operator()(const Translate & iVal)
        {
            return iVal.get();
        }

        Mat4x4 operator()(const Rotate & iVal)
        {
            return iVal.get();
        }

        Mat4x4 operator()(const Scale & iVal)
        {
            return iVal.get();
        }

        Mat4x4 operator()(const Matrix4x4 & iVal)
        {
            return iVal.get();
        }
};

class EncodeVisitor : public boost::static_visitor<>
{
    public:
        EncodeVisitor(std::iostream& ioStrm, std::vector<double> & ioVec)
            : mStrm(ioStrm), mVec(ioVec)
        {
        }

        void operator()(const Translate & iVal)
        {
            iVal.encode(mStrm, mVec);
        }

        void operator()(const Rotate & iVal)
        {
            iVal.encode(mStrm, mVec);
        }

        void operator()(const Scale & iVal)
        {
            iVal.encode(mStrm, mVec);
        }

        void operator()(const Matrix4x4 & iVal)
        {
            iVal.encode(mStrm, mVec);
        }

    private:
        std::iostream & mStrm;
        std::vector<double> & mVec;
};
}

class TransformWriterIMPL::PrivateData
{
    public:
        PrivateData()
        {
            mAreSamplesWritable = false;
            mSampleIndexCount = 0;
        }

        bool mAreSamplesWritable;
        std::vector<TransformOperationVariant> mOperations;
        unsigned int mSampleIndexCount;
};

TransformWriterIMPL::TransformWriterIMPL(const std::string & iName)
  : WriterGlue<TakoAbstractV1::TransformWriter>(iName),
    mData(new PrivateData())
{
    // transform stack is implicitly identity in this case
    // and since mSampleIndexCount is 0 no samples can ever
    // be written.
    mData->mAreSamplesWritable = true;

    int typeVal = NODE_TYPE_TRANSFORM;
    H5LTset_attribute_int(getGroup(), ".", "type", &typeVal, 1);

    int version = TRANSFORM_INITIAL_STATIC_VERSION;
    H5LTset_attribute_int(getGroup(), ".", "vers", &version, 1);
}

TransformWriterIMPL::TransformWriterIMPL(const std::string & iName,
                                         TransformWriterIMPL& iParent)
  : WriterGlue<TakoAbstractV1::TransformWriter>( iName,
                                               iParent.getGroup() ),
    mData(new PrivateData())
{
    int typeVal = NODE_TYPE_TRANSFORM;
    H5LTset_attribute_int(getGroup(), ".", "type", &typeVal, 1);

    int version = TRANSFORM_INITIAL_STATIC_VERSION;
    H5LTset_attribute_int(getGroup(), ".", "vers", &version, 1);
}

TransformWriterIMPL::~TransformWriterIMPL() {}

void TransformWriterIMPL::push(TransformOperationVariant iOp)
{
    if (!mData->mAreSamplesWritable)
        mData->mOperations.push_back(iOp);
    else
        throw std::logic_error(
            "Transform stack has been written can't push new operation");
}

void TransformWriterIMPL::clear()
{
    if (!mData->mAreSamplesWritable)
    {
        mData->mOperations.clear();
        mData->mSampleIndexCount = 0;
    }
    else
    {
        throw std::logic_error(
            "Transform stack has been written and can no longer be cleared.");
    }
}

void TransformWriterIMPL::writeTransformStack(float iFrame,
    bool inheritsTransform)
{
    if (mData->mAreSamplesWritable)
    {
        throw std::logic_error("Transform stack has already been written.");
    }

    mData->mAreSamplesWritable = true;

    // by default inheritsTransform is true, only save it if false
    if (!inheritsTransform)
    {
        int val = 0;
        H5LTset_attribute_int(getGroup(), ".", "inheritsTransform", &val, 1);
    }

    // encode it in the same order it was pushed in
    std::vector<TransformOperationVariant>::const_iterator i, end;
    std::stringstream dataStrm;

    std::vector<double> sampledData;
    EncodeVisitor visit(dataStrm, sampledData);
    for (i = mData->mOperations.begin(), end = mData->mOperations.end();
        i != end; i++)
    {
        boost::apply_visitor(visit, *i);
    }

    std::string dataStr = dataStrm.str();
    size_t dataSize = dataStr.size();
    if (dataSize > 0)
    {
        herr_t ret = H5LTset_attribute_char(getGroup(), ".", "stack",
            dataStr.c_str(), dataSize);

        if (ret < 0)
        {
            throw std::runtime_error("Error writing stack for transform node");
        }
    }

    // if there is any sampled data write it out at the given frame
    if (!sampledData.empty())
    {
        mData->mSampleIndexCount = sampledData.size();

        int version = TRANSFORM_INITIAL_SAMPLED_VERSION;
        H5LTset_attribute_int(getGroup(), ".", "vers", &version, 1);

        writeSamples(iFrame, sampledData);
    }
    // park the calculated matrix somewhere?
}

TransformWriterIMPL::const_stack_iterator TransformWriterIMPL::begin() const
{
    return mData->mOperations.begin();
}

TransformWriterIMPL::const_stack_iterator TransformWriterIMPL::end() const
{
    return mData->mOperations.end();
}

void TransformWriterIMPL::writeSamples(float iFrame,
    const std::vector<double> & iSamples)
{
    // you should probably throw here instead of just returning
    if (iSamples.size() != mData->mSampleIndexCount)
    {
        std::stringstream strm;
        strm << "Sampled transform data is not the expected size. Expected " <<
            mData->mSampleIndexCount << " got " << iSamples.size();
        throw std::logic_error(strm.str().c_str());
    }

    // there is nothing to write, just return
    if (mData->mSampleIndexCount == 0)
        return;

    hsize_t dataSize = iSamples.size();
    const double * doublePtr = &iSamples[0];

    hid_t sid = H5Screate_simple(1, &dataSize, NULL);
    hid_t aid = H5Acreate2(getGroup(), floatToString(iFrame).c_str(),
        H5T_NATIVE_DOUBLE, sid, H5P_DEFAULT, H5P_DEFAULT);

    herr_t ret = H5Awrite(aid, H5T_NATIVE_DOUBLE, doublePtr);
    if (ret < 0)
    {
        throw std::runtime_error("Error writing transform node sample data");
    }

    H5Sclose(sid);
    H5Aclose(aid);
}

Mat4x4 TransformWriterIMPL::getMatrix() const
{
    std::vector<TransformOperationVariant>::const_reverse_iterator it, end;
    Mat4x4 mat;
    mat.makeIdentity();

    ReturnMatrixVisitor visit;
    for (it = mData->mOperations.rbegin(), end = mData->mOperations.rend();
        it != end; it++)
    {
        mat *= boost::apply_visitor(visit, *it);
    }
    return mat;
}

hid_t TransformWriterIMPL::getHid() const
{
    return getGroup();
}

//-*****************************************************************************
// Factory functions
TransformWriterPtr
TransformWriterIMPL::createTransformChild( const std::string & iName )
{
    TransformWriterPtr twp(
        new TakoSPI::TransformWriterIMPL( iName, *this ) );
    return twp;
}

PolyMeshWriterPtr
TransformWriterIMPL::createPolyMeshChild( const std::string & iName )
{
    PolyMeshWriterPtr twp(
        new TakoSPI::PolyMeshWriterIMPL( iName, *this ) );
    return twp;
}

SubDWriterPtr
TransformWriterIMPL::createSubDChild( const std::string & iName )
{
    SubDWriterPtr twp(
        new TakoSPI::SubDWriterIMPL( iName, *this ) );
    return twp;
}

NurbsCurveWriterPtr
TransformWriterIMPL::createNurbsCurveChild( const std::string & iName )
{
    NurbsCurveWriterPtr twp(
        new TakoSPI::NurbsCurveWriterIMPL( iName, *this ) );
    return twp;
}

NurbsSurfaceWriterPtr
TransformWriterIMPL::createNurbsSurfaceChild( const std::string & iName )
{
    NurbsSurfaceWriterPtr twp(
        new TakoSPI::NurbsSurfaceWriterIMPL( iName, *this ) );
    return twp;
}

CameraWriterPtr
TransformWriterIMPL::createCameraChild( const std::string & iName )
{
    CameraWriterPtr twp(
        new TakoSPI::CameraWriterIMPL( iName, *this ) );
    return twp;
}

PointPrimitiveWriterPtr
TransformWriterIMPL::createPointPrimitiveChild( const std::string & iName )
{
    PointPrimitiveWriterPtr twp(
        new TakoSPI::PointPrimitiveWriterIMPL( iName, *this ) );
    return twp;
}

GenericNodeWriterPtr
TransformWriterIMPL::createGenericNodeChild( const std::string & iName )
{
    GenericNodeWriterPtr twp(
        new TakoSPI::GenericNodeWriterIMPL( iName, *this ) );
    return twp;
}

} // End namespace TakoSPI

} // End namespace Alembic
