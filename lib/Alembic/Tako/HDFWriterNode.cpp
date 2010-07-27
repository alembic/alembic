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

#include <Alembic/Tako/HDFWriterNode.h>
#include <Alembic/Tako/HDFNode.h>
#include <Alembic/Tako/PrivateUtil.h>

namespace Alembic {

using Tako::HDFWriterNode;
using Tako::PropertyType;

namespace {

class WritePropertyVisitor : public boost::static_visitor<>
{
    public:
        explicit WritePropertyVisitor(hid_t iParent);
        void setName(const char * iName);
        void operator()(const int8_t & iVal) const;
        void operator()(const int16_t & iVal) const;
        void operator()(const int32_t & iVal) const;
        void operator()(const int64_t & iVal) const;
        void operator()(const float & iVal) const;
        void operator()(const double & iVal) const;
        void operator()(const std::string & iVal) const;
        void operator()(const std::vector<int16_t> & iVal) const;
        void operator()(const std::vector<int32_t> & iVal) const;
        void operator()(const std::vector<int64_t> & iVal) const;
        void operator()(const std::vector<float> & iVal) const;
        void operator()(const std::vector<double> & iVal) const;
        void operator()(const std::vector<std::string> & iVal) const;
        void writeInfo(const Tako::ArbAttrInfo & iInfo) const;
    private:

        void writeHelper(hsize_t iSize, hid_t iType, const void * iBuff) const;

        template <typename T>
        void writeHelperSmallVec(hsize_t iSize, hid_t iType, T iVal) const
        {
            // if we are dealing with an empty (or 1 value) value, write out
            // 2 values worth of data, and add an attribute rs which will give
            // the real size of the dataset
            char realSize = iSize;
            T val[2];
            val[0] = iVal;
            val[1] = 0;

            writeHelper(2, iType, val);
            H5LTset_attribute_char(mParent, mName.c_str(), "rs", &realSize, 1);
        };

        std::string mName;
        hid_t mParent;
};



WritePropertyVisitor::WritePropertyVisitor(hid_t iParent)
{
    mParent = iParent;
}

void WritePropertyVisitor::setName(const char *iName)
{
    mName = iName;
}

void WritePropertyVisitor::writeHelper(hsize_t iSize, hid_t iType,
    const void * iBuff) const
{
    hid_t sid = H5Screate_simple(1, &iSize, NULL);
    hid_t did = H5Dcreate2(mParent, mName.c_str(), iType, sid,
    H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    H5Dwrite(did, iType, H5S_ALL, H5S_ALL, H5P_DEFAULT, iBuff);

    H5Dclose(did);
    H5Sclose(sid);
}

void WritePropertyVisitor::operator()(const int8_t & iVal) const
{
    writeHelper(1, H5T_NATIVE_CHAR, &iVal);
}

void WritePropertyVisitor::operator()(const int16_t & iVal) const
{
    writeHelper(1, H5T_NATIVE_SHORT, &iVal);
}

void WritePropertyVisitor::operator()(const int32_t & iVal) const
{
    writeHelper(1, H5T_NATIVE_INT, &iVal);
}

void WritePropertyVisitor::operator()(const int64_t & iVal) const
{
    writeHelper(1, H5T_NATIVE_LLONG, &iVal);
}

void WritePropertyVisitor::operator()(const float & iVal) const
{
    writeHelper(1, H5T_NATIVE_FLOAT, &iVal);
}

void WritePropertyVisitor::operator()(const double & iVal) const
{
    writeHelper(1, H5T_NATIVE_DOUBLE, &iVal);
}

void WritePropertyVisitor::operator()(const std::string & iVal) const
{
    hsize_t dim = iVal.size();
    if (dim > 1)
    {
        writeHelper(dim, H5T_NATIVE_CHAR, iVal.c_str());
    }
    else
    {
        char val = 0;
        if (dim == 1)
        {
            val = iVal[0];
        }

        writeHelperSmallVec <char> (dim, H5T_NATIVE_CHAR, val);
    }
}

void WritePropertyVisitor::operator()(
    const std::vector<std::string> & iVal) const
{
    size_t numStrs = iVal.size();
    std::vector<int32_t> lengths(numStrs);
    std::string str;

    for (size_t i = 0; i < numStrs; ++i)
    {
        str += iVal[i];
        lengths[i] = iVal[i].size();
    }

    hsize_t dim = str.size();
    if (dim > 1)
    {
        writeHelper(dim, H5T_NATIVE_CHAR, str.c_str());
    }
    else
    {
        char val = 0;
        if (dim == 1)
        {
            val = str[0];
        }

        writeHelperSmallVec <char> (dim, H5T_NATIVE_CHAR, val);
    }

    if (!lengths.empty())
    {
        const int32_t * intPtr = &(lengths.front());
        H5LTset_attribute_int(mParent, mName.c_str(), "len", intPtr,
            lengths.size());
    }
    else
    {
        // special case for the empty no strings case
        int val = 0;
        H5LTset_attribute_int(mParent, mName.c_str(), "nolen", &val, 1);
    }
}

void WritePropertyVisitor::operator()(const std::vector<int16_t> & iVal) const
{
    hsize_t dim = iVal.size();
    if (dim > 1)
    {
        writeHelper(dim, H5T_NATIVE_SHORT, &iVal[0]);
    }
    else
    {
        int16_t val = 0;
        if (dim == 1)
        {
            val = iVal[0];
        }

        writeHelperSmallVec <int16_t> (dim, H5T_NATIVE_SHORT, val);
    }
}

void WritePropertyVisitor::operator()(const std::vector<int32_t> & iVal) const
{
    hsize_t dim = iVal.size();
    if (dim > 1)
    {
        writeHelper(dim, H5T_NATIVE_INT, &iVal[0]);
    }
    else
    {
        int32_t val = 0;
        if (dim == 1)
        {
            val = iVal[0];
        }

        writeHelperSmallVec <int32_t> (dim, H5T_NATIVE_INT, val);
    }
}

void WritePropertyVisitor::operator()(const std::vector<int64_t> & iVal) const
{
    hsize_t dim = iVal.size();
    if (dim > 1)
    {
        writeHelper(dim, H5T_NATIVE_LLONG, &iVal[0]);
    }
    else
    {
        int64_t val = 0;
        if (dim == 1)
        {
            val = iVal[0];
        }

        writeHelperSmallVec <int64_t> (dim, H5T_NATIVE_LLONG, val);
    }
}

void WritePropertyVisitor::operator()(const std::vector<float> & iVal) const
{
    hsize_t dim = iVal.size();
    if (dim > 1)
    {
        writeHelper(dim, H5T_NATIVE_FLOAT, &iVal[0]);
    }
    else
    {
        float val = 0;
        if (dim == 1)
        {
            val = iVal[0];
        }

        writeHelperSmallVec <float> (dim, H5T_NATIVE_FLOAT, val);
    }
}

void WritePropertyVisitor::operator()(const std::vector<double> & iVal) const
{
    hsize_t dim = iVal.size();
    if (dim > 1)
    {
        writeHelper(dim, H5T_NATIVE_DOUBLE, &iVal[0]);
    }
    else
    {
        double val = 0;
        if (dim == 1)
        {
            val = iVal[0];
        }

        writeHelperSmallVec <double> (dim, H5T_NATIVE_DOUBLE, val);
    }
}

void WritePropertyVisitor::writeInfo(const Tako::ArbAttrInfo & iInfo) const
{
    int32_t outType = (int32_t)(iInfo.outputType & 0xff) << 16;
    int32_t inType = (int32_t)(iInfo.inputType & 0xff) << 8;
    int32_t scope = (int32_t)(iInfo.scope & 0xff);
    int32_t value = outType | inType | scope;

    if (value != 0)
        H5LTset_attribute_int(mParent, mName.c_str(), "arb", &value, 1);

    if (!iInfo.index.empty())
    {
        const int32_t * intPtr = &(iInfo.index.front());
        H5LTset_attribute_int(mParent, mName.c_str(), "index", intPtr,
            iInfo.index.size());
    }
}
};

class Tako::HDFWriterNode::PrivateData
{
    public:
        PrivateData()
        {
            mPropertiesWritten = false;
            mSampleGrp = -1;
        }

        PropertyMap mNonSampledMap;
        PropertyMap mSampledMap;
        bool mPropertiesWritten;
        hid_t mSampleGrp;
};

HDFWriterNode::HDFWriterNode(const char * iName)
    : mData(new PrivateData())
{
    hid_t faid = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_libver_bounds(faid, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST);

    hid_t fid = H5Fcreate(iName, H5F_ACC_TRUNC, H5P_DEFAULT, faid);

    H5Pclose(faid);

    hid_t propGroup = H5Pcreate(H5P_GROUP_CREATE);
    H5Pset_link_creation_order(propGroup,
        H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);
    mGroup = H5Gcreate2(fid, "root", H5P_DEFAULT, propGroup, H5P_DEFAULT);
    H5Pclose(propGroup);

    // H5Fclose terminates access to an HDF5 file by flushing all data
    // to storage and terminating access to the file through fid.
    H5Fclose(fid);
}

HDFWriterNode::HDFWriterNode(const char * iName, hid_t iParent)
    : mData(new PrivateData())
{
    hid_t propGroup = H5Pcreate(H5P_GROUP_CREATE);
    H5Pset_link_creation_order(propGroup,
        H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);
    mGroup = H5Gcreate2(iParent, iName, H5P_DEFAULT, propGroup, H5P_DEFAULT);
    H5Pclose(propGroup);
}

HDFWriterNode::~HDFWriterNode()
{
    if (mData->mSampleGrp > -1)
        H5Gclose(mData->mSampleGrp);

    if (mGroup > -1)
        H5Gclose(mGroup);
}


PropertyType HDFWriterNode::getProperty(const char * iName,
    PropertyPair & oProp)
{
    PropertyType retVal = PROPERTY_STATIC;

    PropertyMap::iterator it = mData->mNonSampledMap.find(iName);
    if (it == mData->mNonSampledMap.end())
    {
        it = mData->mSampledMap.find(iName);

        if (it == mData->mSampledMap.end())
            return PROPERTY_NONE;

        retVal = PROPERTY_ANIMATED;
    }

    oProp = (*it).second;
    return retVal;
}

void HDFWriterNode::setNonSampledProperty(const char * iName,
    const PropertyPair & iProp)
{
    if (mData->mPropertiesWritten)
        throw std::logic_error("Properties already written!");

    PropertyMap::iterator it = mData->mSampledMap.find(iName);
    if (it  != mData->mSampledMap.end())
    {
        std::string msg = "Property already exists: ";
        msg += iName;
        throw std::logic_error(msg);
    }

    mData->mNonSampledMap[iName] = iProp;
}

void HDFWriterNode::setSampledProperty(const char * iName,
    const PropertyPair & iProp)
{
    if (mData->mPropertiesWritten)
        throw std::logic_error("Properties already written!");

    PropertyMap::iterator it = mData->mNonSampledMap.find(iName);
    if (it  != mData->mNonSampledMap.end())
    {
        std::string msg = "Property already exists: ";
        msg += iName;
        throw std::logic_error(msg);
    }

    mData->mSampledMap[iName] = iProp;
}

void HDFWriterNode::clearProperties()
{
    if (mData->mPropertiesWritten)
        throw std::logic_error("Properties already written!");

    mData->mNonSampledMap.clear();
    mData->mSampledMap.clear();
}

Tako::PropertyMap::const_iterator
HDFWriterNode::beginNonSampledProperties() const
{
    return mData->mNonSampledMap.begin();
}

Tako::PropertyMap::const_iterator
HDFWriterNode::endNonSampledProperties() const
{
    return mData->mNonSampledMap.end();
}

Tako::PropertyMap::const_iterator
HDFWriterNode::beginSampledProperties() const
{
    return mData->mSampledMap.begin();
}

Tako::PropertyMap::const_iterator
HDFWriterNode::endSampledProperties() const
{
    return mData->mSampledMap.end();
}

bool HDFWriterNode::propertiesWritten()
{
    return mData->mPropertiesWritten;
}

void HDFWriterNode::updateSample(const char * iName,
    const PropertyPair & iAttr)
{
    PropertyMap::iterator it = mData->mSampledMap.find(iName);
    if (it == mData->mSampledMap.end())
    {
        std::string msg = "Sampled property not found: ";
        msg += iName;
        throw std::logic_error(msg);
    }
    else if (it->second.first.type() != iAttr.first.type())
    {
        std::string msg = "Sampled property has a different type for: ";
        msg += iName;
        throw std::logic_error(msg);
    }

    it->second = iAttr;
}

void HDFWriterNode::writeProperties(float iFrame)
{
    if (!mData->mPropertiesWritten)
    {
        mData->mPropertiesWritten = true;

        // if the property maps are empty we don't need to do anything
        if (mData->mNonSampledMap.empty() && mData->mSampledMap.empty())
            return;

        hid_t propGrp = H5Gcreate2(mGroup, ".prop", H5P_DEFAULT, H5P_DEFAULT,
                H5P_DEFAULT);

        if (!mData->mNonSampledMap.empty())
        {
            PropertyMap::const_iterator it = mData->mNonSampledMap.begin();
            PropertyMap::const_iterator end = mData->mNonSampledMap.end();

            WritePropertyVisitor visit(propGrp);
            for (;it != end; it++)
            {
                visit.setName(it->first.c_str());
                boost::apply_visitor(visit, it->second.first);
                visit.writeInfo(it->second.second);
            }
        }

        if (!mData->mSampledMap.empty())
        {
            mData->mSampleGrp = H5Gcreate2(propGrp, ".samples", H5P_DEFAULT,
                H5P_DEFAULT, H5P_DEFAULT);
        }
        H5Gclose(propGrp);
    }

    // if sampled map isn't empty try to write it out
    if (!mData->mSampledMap.empty())
    {
        hid_t curSample = H5Gcreate2(mData->mSampleGrp,
            floatToString(iFrame).c_str(), H5P_DEFAULT, H5P_DEFAULT,
            H5P_DEFAULT);

        WritePropertyVisitor sampleVisit(curSample);

        PropertyMap::const_iterator it = mData->mSampledMap.begin();
        PropertyMap::const_iterator end = mData->mSampledMap.end();
        for (; it != end; it++)
        {
            sampleVisit.setName(it->first.c_str());
            boost::apply_visitor(sampleVisit, it->second.first);
            sampleVisit.writeInfo(it->second.second);
        }
        H5Gclose(curSample);
    }
}

} // End namespace Alembic
