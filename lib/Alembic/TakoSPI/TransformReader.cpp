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

#include <Alembic/TakoSPI/TransformReader.h>
#include <Alembic/TakoSPI/PolyMeshReader.h>
#include <Alembic/TakoSPI/SubDReader.h>
#include <Alembic/TakoSPI/NurbsCurveReader.h>
#include <Alembic/TakoSPI/NurbsSurfaceReader.h>
#include <Alembic/TakoSPI/CameraReader.h>
#include <Alembic/TakoSPI/GenericNodeReader.h>
#include <Alembic/TakoSPI/PointPrimitiveReader.h>
#include <Alembic/TakoSPI/NodeTypeId.h>
#include <Alembic/TakoSPI/PrivateUtil.h>

namespace Alembic {

namespace TakoSPI {

namespace {

class CallUpdateVisitor : public boost::static_visitor<>
{
    public:
        CallUpdateVisitor(uint16_t & ioIndex,
            std::vector<double> & iVec):
            mIoIndex(ioIndex), mSampledVec(iVec) {}

        void operator()(Translate & iVal)
        {
            iVal.updateSamples(mIoIndex, mSampledVec);
        }

        void operator()(Rotate & iVal)
        {
            iVal.updateSamples(mIoIndex, mSampledVec);
        }

        void operator()(Scale & iVal)
        {
            iVal.updateSamples(mIoIndex, mSampledVec);
        }

        void operator()(Matrix4x4 & iVal)
        {
            iVal.updateSamples(mIoIndex, mSampledVec);
        }

    private:
        uint16_t & mIoIndex;
        const std::vector<double> & mSampledVec;
};

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

void ReadValueFromStream(std::stringstream & iStrm, hid_t iGroup,
    const std::vector<double> & iVec, bool isSampled, Value& oValue)
{
    if (!isSampled)
    {
        double v = 0.0;
        iStrm.read(reinterpret_cast<char *>(&v), sizeof(v));
        oValue.mValue = v;
        oValue.mIsStatic = true;
    }
    else
    {
        uint16_t index = 0;
        iStrm.read(reinterpret_cast<char *>(&index), sizeof(index));

        oValue.mIsStatic = false;

        if (index < iVec.size())
        {
            oValue.mValue = iVec[index];
        }
        else
        {
            oValue.mValue = 0.0;
            // we could also throw:
            // throw std::runtime_error("Not enough sampled data");
        }
    }
}

herr_t GetChildren(hid_t parentId, const char * name,
    const H5L_info_t *linfo, void *opdata)
{
    if (name[0] != '.')
    {
        std::vector<std::string> * nameVec = (std::vector<std::string> *)
            opdata;
        nameVec->push_back(name);
    }
    return 0;
}

herr_t GetFramesAttr(hid_t pid, const char * name, const H5A_info_t *linfo,
    void *opdata)
{
    char first = name[0];

    // with how we are storing attributes on the transform group node, as long
    // as we encounter -, 0 ,1, 2, 3, 4, 5, 6, 7, 8, or 9 then we have a number
    if (first == '-' || (first > '/' && first < ':'))
    {
        std::set<float> * pSet = (std::set<float> *) opdata;
        pSet->insert(atof(name));
    }
    return 0;
}
}  // namespace

class TransformReaderIMPL::PrivateData
{
    public:
        PrivateData()
        {
            mVersion = -1;
            mHasReadOps = false;
            mInheritsTransform = true;
        }

        std::vector<TransformOperationVariant> mOperations;
        std::vector<std::string> mChildNames;
        int mVersion;
        bool mHasReadOps;
        bool mInheritsTransform;
};

TransformReaderIMPL::TransformReaderIMPL(const std::string & iName)
  : ReaderGlue<TakoAbstractV1::TransformReader>(iName),
    mData(new PrivateData())
{
    // root is intrinsically identity and has no transform stack
    mData->mHasReadOps = true;

    hid_t prop = H5Gget_create_plist(getGroup());
    unsigned int orderFlags = 0;
    H5Pget_link_creation_order(prop, &orderFlags);
    H5Pclose(prop);

    H5_index_t orderType = H5_INDEX_NAME;
    if (orderFlags & H5P_CRT_ORDER_INDEXED)
        orderType = H5_INDEX_CRT_ORDER;

    H5Literate(getGroup(), orderType, H5_ITER_INC, NULL, GetChildren,
        &(mData->mChildNames));


    if (H5Aexists(getGroup(), "vers") == 1)
    {
        hid_t aid = H5Aopen(getGroup(), "vers", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &mData->mVersion);
        H5Aclose(aid);
    }
}

TransformReaderIMPL::TransformReaderIMPL(
    const std::string & iName, TransformReaderIMPL & iParent)
  : ReaderGlue<TakoAbstractV1::TransformReader>(iName, iParent.getGroup()),
    mData(new PrivateData())
{
    hid_t prop = H5Gget_create_plist(getGroup());
    unsigned int orderFlags = 0;
    H5Pget_link_creation_order(prop, &orderFlags);
    H5Pclose(prop);

    H5_index_t orderType = H5_INDEX_NAME;
    if (orderFlags & H5P_CRT_ORDER_INDEXED)
        orderType = H5_INDEX_CRT_ORDER;

    H5Literate(getGroup(), orderType, H5_ITER_INC, NULL, GetChildren,
        &(mData->mChildNames));

    if (H5Aexists(getGroup(), "vers") == 1)
    {
        hid_t aid = H5Aopen(getGroup(), "vers", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &mData->mVersion);

        H5Aclose(aid);
    }

    if (H5Aexists(getGroup(), "inheritsTransform"))
    {
        int val = 1;
        hid_t aid = H5Aopen(getGroup(), "inheritsTransform", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &val);
        H5Aclose(aid);
        mData->mInheritsTransform = (val == 1);
    }
}

TransformReader::SampleType
TransformReaderIMPL::read(float iFrame)
{
    // read any data for this frame and put it in a double vector
    std::vector<double> sampleVec;

    SampleType rType = getSamples(iFrame, sampleVec);
    if ( rType == READ_ERROR )
    {
        return rType;
    }
    std::cout << "Num samples read: " << sampleVec.size() << std::endl;

    // we have already read the transform stack
    if (mData->mHasReadOps)
    {
        if (rType == TransformReader::READ_ERROR)
            return rType;

        std::vector<TransformOperationVariant>::iterator op, end;
        uint16_t curIndex = 0;
        CallUpdateVisitor visit(curIndex, sampleVec);
        for (op = mData->mOperations.begin(), end = mData->mOperations.end();
            op != end; ++op)
        {
            boost::apply_visitor(visit, *op);
        }
        return rType;
    }

    // need to read the transform stack
    mData->mHasReadOps = true;

    if (H5Aexists(getGroup(), "stack") <= 0)
        return rType;

    // open the data set that contains the transform operation stack
    hid_t aid = H5Aopen(getGroup(), "stack", H5P_DEFAULT);
    hid_t tid = H5Aget_type(aid);

    hsize_t dataSize = H5Aget_storage_size(aid);

    std::string buff(dataSize, 0);
    char * buffPtr = &buff[0];

    H5Aread(aid, tid, buffPtr);

    H5Tclose(tid);
    H5Aclose(aid);

    std::stringstream strm(buff);

    char opType = 0;
    strm.read(&opType, 1);
    TransformOperationType trOpType = (TransformOperationType)opType;

    mData->mOperations.clear();

    while (!strm.eof())
    {
        switch (trOpType)
        {
            case cScaleOperation:
            {
                Value scaleX;
                Value scaleY;
                Value scaleZ;

                char type = 0;
                char flags = 0;
                strm.read(&type, sizeof(type));
                Scale::ScaleType sType = (Scale::ScaleType)type;
                strm.read(&flags, sizeof(flags));

                ReadValueFromStream(strm, getGroup(), sampleVec,
                    (flags & 0x01) != 0, scaleX);

                ReadValueFromStream(strm, getGroup(), sampleVec,
                    (flags & 0x02) != 0, scaleY);

                ReadValueFromStream(strm, getGroup(), sampleVec,
                    (flags & 0x04) != 0, scaleZ);

                TransformOperationVariant s(
                    Scale(sType, scaleX, scaleY, scaleZ));

                mData->mOperations.push_back(s);
            }
            break;

            case cTranslateOperation:
            {
                Value transX;
                Value transY;
                Value transZ;

                char type = 0;
                char flags = 0;
                strm.read(&type, sizeof(type));
                Translate::TranslateType tType = (Translate::TranslateType)type;
                strm.read(&flags, sizeof(flags));

                ReadValueFromStream(strm, getGroup(), sampleVec,
                    (flags & 0x01) != 0, transX);

                ReadValueFromStream(strm, getGroup(), sampleVec,
                    (flags & 0x02) != 0, transY);

                ReadValueFromStream(strm, getGroup(), sampleVec,
                    (flags & 0x04) != 0, transZ);

                TransformOperationVariant t(
                    Translate(tType, transX, transY, transZ));

                mData->mOperations.push_back(t);
            }
            break;

            case cRotateOperation:
            {
                Value angle;

                char type = 0;
                char flags = 0;
                strm.read(&type, sizeof(type));
                Rotate::RotateType rType = (Rotate::RotateType)type;
                strm.read(&flags, sizeof(flags));

                ReadValueFromStream(strm, getGroup(), sampleVec,
                    (flags & 0x01) != 0, angle);

                float xAxis = 0.0f;
                strm.read(reinterpret_cast<char *>(&xAxis), sizeof(xAxis));

                float yAxis = 0.0f;
                strm.read(reinterpret_cast<char *>(&yAxis), sizeof(yAxis));

                float zAxis = 0.0f;
                strm.read(reinterpret_cast<char *>(&zAxis), sizeof(zAxis));

                TransformOperationVariant r(
                    Rotate(rType, angle, xAxis, yAxis, zAxis));

                mData->mOperations.push_back(r);
            }
            break;

            case cMatrixOperation:
            {
                Value v[16];
                char type = 0;
                uint16_t flags = 0;
                strm.read(&type, sizeof(type));
                Matrix4x4::Matrix4x4Type mType = (Matrix4x4::Matrix4x4Type)type;
                strm.read(reinterpret_cast<char*>(&flags), sizeof(flags));
                uint16_t i;
                for (i = 0; i < 16; i++)
                {
                    ReadValueFromStream(strm, getGroup(), sampleVec,
                        (flags & (1 << i)) != 0, v[i]);
                }

                TransformOperationVariant m(Matrix4x4(mType,
                    v[ 0], v[ 1], v[ 2], v[ 3],
                    v[ 4], v[ 5], v[ 6], v[ 7],
                    v[ 8], v[ 9], v[10], v[11],
                    v[12], v[13], v[14], v[15]));

                mData->mOperations.push_back(m);
            }
            break;
        }

        // try to read the next operation type
        strm.read(&opType, 1);
        trOpType = (TransformOperationType)opType;
    }

    return rType;
}

TransformReader::SampleType TransformReaderIMPL::getSamples(float iFrame,
    std::vector<double> & oVec)
{
    SampleType rType = SAMPLES_NONE;

    if (mData->mVersion == TRANSFORM_INITIAL_SAMPLED_VERSION)
    {
        std::string frameStr = floatToString(iFrame);

        hid_t aid = H5Aopen(getGroup(), frameStr.c_str(), H5P_DEFAULT);

        if (aid > 0)
        {
            hid_t sid = H5Aget_space(aid);
            hid_t tid = H5Aget_type(aid);

            hsize_t dims[1];
            H5Sget_simple_extent_dims(sid, dims, NULL);
            oVec.resize(dims[0]);
            double * dPtr = &(oVec.front());
            H5Aread(aid, tid, dPtr);

            H5Tclose(tid);
            H5Sclose(sid);
            H5Aclose(aid);
            rType = SAMPLES_READ;
        }
        else
        {
            // abort();
            rType = READ_ERROR;
        }
    }

    return rType;
}

TransformReaderIMPL::~TransformReaderIMPL() {}

Mat4x4 TransformReaderIMPL::getMatrix() const
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

TransformReader::const_stack_iterator TransformReaderIMPL::begin() const
{
    return mData->mOperations.begin();
}

TransformReader::const_stack_iterator TransformReaderIMPL::end() const
{
    return mData->mOperations.end();
}

size_t TransformReaderIMPL::numChildren(void) const
{
    return mData->mChildNames.size();
}

ChildNodePtr TransformReaderIMPL::getChild(size_t iIndex)
{
    ChildNodePtr retVal;

    if (iIndex >= mData->mChildNames.size())
        return retVal;

    const std::string & iName = mData->mChildNames[iIndex];

    int type = -1;
    H5LTget_attribute_int(getGroup(), iName.c_str(), "type",  &type);

    switch (type)
    {
        case NODE_TYPE_TRANSFORM:
        {
            retVal = TransformReaderPtr(
                new TransformReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_POLYMESH:
        {
            retVal = PolyMeshReaderPtr(
                new PolyMeshReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_SUBD:
        {
            retVal = SubDReaderPtr(
                new SubDReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_NURBSCURVE:
        {
            retVal = NurbsCurveReaderPtr(
                new NurbsCurveReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_NURBSSURFACE:
        {
            retVal = NurbsSurfaceReaderPtr(
                new NurbsSurfaceReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_CAMERA:
        {
            retVal = CameraReaderPtr(
                new CameraReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_GENERIC_NODE:
        {
            retVal = GenericNodeReaderPtr(
                new GenericNodeReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_POINT_PRIMITIVE:
            retVal = PointPrimitiveReaderPtr(
                new PointPrimitiveReaderIMPL(iName, *this));
        break;

        default:
        break;
    }

    return retVal;
}

ChildNodePtr TransformReaderIMPL::getChild( const std::string &iName )
{
    ChildNodePtr retVal;
    
    int type = -1;
    H5LTget_attribute_int(getGroup(), iName.c_str(), "type",  &type);

    switch (type)
    {
        case NODE_TYPE_TRANSFORM:
        {
            retVal = TransformReaderPtr(
                new TransformReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_POLYMESH:
        {
            retVal = PolyMeshReaderPtr(
                new PolyMeshReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_SUBD:
        {
            retVal = SubDReaderPtr(
                new SubDReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_NURBSCURVE:
        {
            retVal = NurbsCurveReaderPtr(
                new NurbsCurveReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_NURBSSURFACE:
        {
            retVal = NurbsSurfaceReaderPtr(
                new NurbsSurfaceReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_CAMERA:
        {
            retVal = CameraReaderPtr(
                new CameraReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_GENERIC_NODE:
        {
            retVal = GenericNodeReaderPtr(
                new GenericNodeReaderIMPL(iName, *this));
        }
        break;

        case NODE_TYPE_POINT_PRIMITIVE:
            retVal = PointPrimitiveReaderPtr(
                new PointPrimitiveReaderIMPL(iName, *this));
        break;

        default:
        break;
    }

    return retVal;
}

hid_t TransformReaderIMPL::getHid() const
{
    return getGroup();
}

void TransformReaderIMPL::getFrames(std::set<float>& ioFrames) const
{
    if (mData->mVersion == TRANSFORM_INITIAL_SAMPLED_VERSION)
    {
        H5Aiterate2(getGroup(),  H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, NULL,
            GetFramesAttr, &ioFrames);
    }
}

bool TransformReaderIMPL::hasFrames() const
{
    return (mData->mVersion == TRANSFORM_INITIAL_SAMPLED_VERSION);
}

bool TransformReaderIMPL::inheritsTransform() const
{
    return mData->mInheritsTransform;
}

} // End namespace TakoSPI

} // End namespace Alembic
