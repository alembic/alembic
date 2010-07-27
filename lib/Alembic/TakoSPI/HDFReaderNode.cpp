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

#include <Alembic/TakoSPI/HDFReaderNode.h>
#include <Alembic/TakoSPI/PrivateUtil.h>

namespace Alembic {

namespace TakoSPI {

namespace {

template <typename U>
void ReadProp(hsize_t dSize, hid_t did, hid_t tid, Property & oProp)
{
    if (dSize > 1)
    {
        oProp = std::vector<U>();

        std::vector<U> * vecPtr =  boost::get< std::vector<U> >(&oProp);

        vecPtr->resize(dSize);

        U * dPtr = &(vecPtr->front());
        H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, dPtr);

        // special case, if the size is 2, and the attribute rs (real size)
        // exists on this data set, then reset the size of the array to this
        // size.  This is for cases where an array has 1 or 0 elements
        if (dSize == 2 && H5Aexists(did, "rs"))
        {
            hid_t aid = H5Aopen(did, "rs", H5P_DEFAULT);
            char realSize = 0;
            H5Aread(aid, H5T_NATIVE_CHAR, &realSize);
            H5Aclose(aid);
            vecPtr->resize(realSize);
        }
    }
    else
    {
        U val = 0;
        H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, &val);
        oProp = val;
    }
}

void ReadProperty(hid_t dataSetId, hid_t dataSpaceId, Property & oProp)
{
    hsize_t dims;
    H5Sget_simple_extent_dims(dataSpaceId, &dims, NULL);

    hid_t dataTypeId = H5Dget_type(dataSetId);


    H5T_class_t classType = H5Tget_class(dataTypeId);
    size_t dataSize = H5Tget_size(dataTypeId);

    switch (dataSize)
    {
        case 1:
        {
            // special case for an empty array of strings
            if (H5Aexists(dataSetId, "nolen"))
            {
                oProp = std::vector<std::string>();
                break;
            }

            std::vector<int32_t> lengths;
            if (H5Aexists(dataSetId, "len"))
            {
                hid_t attrId = H5Aopen(dataSetId, "len", H5P_DEFAULT);
                H5A_info_t info;
                H5Aget_info(attrId, &info);
                lengths.resize(info.data_size/sizeof(int32_t));
                int32_t * iPtr = &(lengths.front());
                H5Aread(attrId, H5T_NATIVE_INT, iPtr);
                H5Aclose(attrId);
            }

            std::string strBuf(dims, 0);
            char * charPtr = &strBuf[0];

            H5Dread(dataSetId, dataTypeId, H5S_ALL, H5S_ALL,
                H5P_DEFAULT, charPtr);

            // special case, if the size is 2, and the attribute rs (real size)
            // exists on this data set, then reset the size of the array to this
            // size.  This is for cases where an array has 1 or 0 elements
            if (dims == 2 && H5Aexists(dataSetId, "rs"))
            {
                hid_t aid = H5Aopen(dataSetId, "rs", H5P_DEFAULT);
                char realSize = 0;
                H5Aread(aid, H5T_NATIVE_CHAR, &realSize);
                H5Aclose(aid);
                strBuf.resize(realSize);

                if (lengths.empty())
                {
                    oProp = strBuf;
                }
                else
                {
                    // lengths is not empty so we have a vector of strings
                    size_t pos = 0;
                    size_t numStrs = lengths.size();
                    std::vector<std::string> strVec( numStrs );
                    for (size_t i = 0; i < numStrs; ++i)
                    {
                        size_t subSize = lengths[i];
                        strVec[i] = strBuf.substr(pos, subSize);
                        pos += subSize;
                    }
                    oProp = strVec;
                }
                // bail early
                break;
            }

            if (strBuf.size() != 1)
            {
                if (lengths.empty())
                {
                    oProp = strBuf;
                }
                else
                {
                    // lengths is not empty so we have a vector of strings
                    size_t pos = 0;
                    size_t numStrs = lengths.size();
                    std::vector<std::string> strVec( numStrs );
                    for (size_t i = 0; i < numStrs; ++i)
                    {
                        size_t subSize = lengths[i];
                        strVec[i] = strBuf.substr(pos, subSize);
                        pos += subSize;
                    }
                    oProp = strVec;
                }
            }
            else
            {
                // single char case
                int8_t val = strBuf[0];
                oProp = val;
            }
        }
        break;

        case 2:
        {
            ReadProp<int16_t>(dims, dataSetId, dataTypeId, oProp);
        }
        break;

        case 4:
        {
            if (classType == H5T_INTEGER)
            {
                ReadProp<int32_t>(dims, dataSetId, dataTypeId, oProp);
            }
            else if (classType == H5T_FLOAT)
            {
                ReadProp<float>(dims, dataSetId, dataTypeId, oProp);
            }
        }
        break;

        case 8:
        {
            if (classType == H5T_INTEGER)
            {
                ReadProp<int64_t>(dims, dataSetId, dataTypeId, oProp);
            }
            else if (classType == H5T_FLOAT)
            {
                ReadProp<double>(dims, dataSetId, dataTypeId, oProp);
            }
        }
        break;

        default:
        break;
    }

    H5Tclose(dataTypeId);
}

herr_t GetFrames(hid_t parentId, const char * name,
    const H5L_info_t *linfo, void *opdata)
{
    std::set<float> * pSet = (std::set<float> *) opdata;
    pSet->insert(static_cast<float>(atof(name)));

    return 0;
}

void GetScopeAndTypes(hid_t dataSetId, ScopeType & oScope,
    ArbAttrType & oInputType, ArbAttrType & oOutputType)
{
    // fill in the Arbitrary Attr info if it exists
    if (H5Aexists(dataSetId, "arb")) {
        int32_t value = 0;
        hid_t attrId = H5Aopen(dataSetId, "arb", H5P_DEFAULT);
        H5Aread(attrId, H5T_NATIVE_INT, &value);
        oScope = ScopeType(value & 0xff);
        oInputType = ArbAttrType((value & 0xff00) >> 8);
        oOutputType = ArbAttrType((value & 0xff0000) >> 16);
        H5Aclose(attrId);
    }
    else
    {
        // otherwise set default values
        oScope = SCOPE_PRIMITIVE;
        oInputType = ARBATTR_NONE;
        oOutputType = ARBATTR_NONE;
    }
}

void GetIndexArray(hid_t dataSetId,
                   std::vector<index_t> & oIndex )
{
    if (H5Aexists(dataSetId, "index"))
    {
        hid_t attrId = H5Aopen(dataSetId, "index", H5P_DEFAULT);
        H5A_info_t info;
        H5Aget_info(attrId, &info);
        oIndex.resize(info.data_size/sizeof(int32_t));
        int32_t * iPtr = reinterpret_cast<int32_t *>
            ( &(oIndex.front()) );
        H5Aread(attrId, H5T_NATIVE_INT, iPtr);
        H5Aclose(attrId);
    }
}

herr_t FindProperties(hid_t parentId, const char * name,
    const H5L_info_t *linfo, void *opdata)
{
    if (name[0] == '.')
        return 0;

    HDFReaderNodeIMPL::PropNameInfoMap * pMap =
        (HDFReaderNodeIMPL::PropNameInfoMap *) opdata;

    HDFReaderNodeIMPL::PropInfo propInfo;

    // we will set this to the correct value elsewhere
    propInfo.isSampled = false;

    hid_t dataSetId = H5Dopen2(parentId, name, H5P_DEFAULT);
    GetScopeAndTypes(dataSetId, propInfo.scope, propInfo.inputType,
        propInfo.outputType);

    H5Dclose(dataSetId);

    (*pMap)[name] = propInfo;
    return 0;
}

herr_t FindPropertiesAnimatedFrame(hid_t parentId, const char * name,
    const H5L_info_t *linfo, void *opdata)
{
    hid_t frameGroup =  H5Gopen2(parentId, name, H5P_DEFAULT);
    H5Literate(frameGroup, H5_INDEX_NAME, H5_ITER_INC, NULL, FindProperties,
        opdata);

    H5Gclose(frameGroup);
    return -1;
}

herr_t ReadProperties(hid_t parentId, const char * name,
    const H5L_info_t *linfo, void *opdata)
{
    // don't do anything if the property starts with a '.'
    // these are reserved for other uses in this system
    // (for example .samples is a group that could be parented here)
    if (name[0] == '.')
        return 0;

    PropertyMap * pMap = (PropertyMap *) opdata;

    // dont forget to close these later on
    hid_t dataSetId = H5Dopen2(parentId, name, H5P_DEFAULT);
    hid_t dataSpaceId = H5Dget_space(dataSetId);

    (*pMap)[name] = PropertyPair();
    PropertyPair & prop = (*pMap)[name];

    GetScopeAndTypes(dataSetId, prop.second.scope, prop.second.inputType,
        prop.second.outputType);
    GetIndexArray(dataSetId, prop.second.index);

    int numDims = H5Sget_simple_extent_ndims(dataSpaceId);

    // we aren't supporting complex dimensionality for a property
    if (numDims != 1)
    {
        H5Sclose(dataSpaceId);
        H5Dclose(dataSetId);
        return 0;
    }

    ReadProperty(dataSetId, dataSpaceId, prop.first);

    // make sure we close everything we opened
    H5Sclose(dataSpaceId);
    H5Dclose(dataSetId);

    return 0;
}
};

class HDFReaderNodeIMPL::PrivateData
{
    public:
        PrivateData()
        {
            mHasReadNonSampled = false;
            mSampledGroup = -1;
        }

        bool mHasReadNonSampled;
        std::string mName;
        hid_t mSampledGroup;
        PropertyMap mNonSampledMap;
        PropertyMap mSampledMap;
};

HDFReaderNodeIMPL::HDFReaderNodeIMPL(const std::string & iName)
    : mData(new PrivateData())
{
    mData->mName = "";
    hid_t fileId = H5Fopen(iName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if ( fileId <= 0 )
    {
        std::string excStr =
            ( boost::format( "Could not open HDF5 file: %s" )
              % iName ).str();
        boost::throw_exception( std::runtime_error( excStr ) );
    }
    mGroup = H5Gopen(fileId, "root", H5P_DEFAULT);

    if (fileId > -1)
        H5Fclose(fileId);
}

HDFReaderNodeIMPL::HDFReaderNodeIMPL(const std::string & iName, hid_t iParent)
    : mData(new PrivateData())
{
    mData->mName = iName;
    mGroup = H5Gopen(iParent, iName.c_str(), H5P_DEFAULT);

    // make sure that we just store the "short" name
    std::string::size_type lastSlash = mData->mName.rfind('/');
    if (lastSlash != std::string::npos)
    {
        mData->mName = std::string(mData->mName, lastSlash+1);
    }
}

void HDFReaderNodeIMPL::readProperties(float iFrame)
{
    if (!mData->mHasReadNonSampled)
    {
        mData->mHasReadNonSampled = true;

        // do the reading
        hid_t propGroup = -1;

        if (H5Lexists(mGroup, ".prop", H5P_DEFAULT) == 1)
        {
            propGroup = H5Gopen2(mGroup, ".prop", H5P_DEFAULT);
        }

        if (propGroup < 0)
        {
            return;
        }

        H5Literate(propGroup, H5_INDEX_NAME, H5_ITER_INC, NULL,
            ReadProperties, &(mData->mNonSampledMap));

        mData->mSampledGroup = -1;
        if (H5Lexists(propGroup, ".samples", H5P_DEFAULT) == 1)
        {
            mData->mSampledGroup = H5Gopen2(propGroup, ".samples", H5P_DEFAULT);
        }

        H5Gclose(propGroup);
    }

    if (mData->mSampledGroup < 0)
        return;

    hid_t sGroup = H5Gopen2(mData->mSampledGroup,
        floatToString(iFrame).c_str(), H5P_DEFAULT);

    if (sGroup > -1)
    {
        H5Literate(sGroup, H5_INDEX_NAME, H5_ITER_INC, NULL,
            ReadProperties, &(mData->mSampledMap));
        H5Gclose(sGroup);
    }
}

PropertyType
HDFReaderNodeIMPL::readProperty(const std::string & iName, float iFrame,
    PropertyPair & oProp)
{
    hid_t nonSampParentId = -1;
    hid_t sampParentId = -1;

    if (H5Lexists(mGroup, ".prop", H5P_DEFAULT) == 1)
    {
        nonSampParentId = H5Gopen2(mGroup, ".prop", H5P_DEFAULT);
    }
    else
    {
        return PROPERTY_NONE;
    }

    if (H5Lexists(nonSampParentId, ".samples", H5P_DEFAULT) == 1)
    {
        hid_t sampId = H5Gopen2(nonSampParentId, ".samples", H5P_DEFAULT);

        std::string name = floatToString(iFrame);
        if (H5Lexists(sampId, name.c_str(), H5P_DEFAULT) == 1)
        {
            sampParentId = H5Gopen2(sampId, name.c_str(), H5P_DEFAULT);
        }

        H5Gclose(sampId);
    }


    // which parent ID we will be using, we will use either from the sampled
    // or non sampled
    hid_t parentId = -1;

    bool hasNonSampProp = (nonSampParentId != -1 &&
                           H5Lexists(nonSampParentId, iName.c_str(),
                                     H5P_DEFAULT) == 1);

    bool hasSampProp = (sampParentId != -1 &&
                        H5Lexists(sampParentId, iName.c_str(),
                                  H5P_DEFAULT) == 1);

    PropertyType retVal = PROPERTY_STATIC;

    // sampled exists, non sampled does not, use sampled
    if (hasSampProp && !hasNonSampProp)
    {
        parentId = sampParentId;
        retVal = PROPERTY_ANIMATED;
    }
    else if (!hasSampProp && hasNonSampProp)
    {
        // sampled doesnt exist, non sampled does, use non sampled
        parentId = nonSampParentId;
    }
    // the last case neither exists or both exists which is an error case

    // remember we won't need to close parentId since it is either
    // nonSampParentId or sampParentId
    if (parentId != -1)
    {
        hid_t dataSetId = H5Dopen2(parentId, iName.c_str(), H5P_DEFAULT);
        hid_t dataSpaceId = H5Dget_space(dataSetId);
        int numDims = H5Sget_simple_extent_ndims(dataSpaceId);

        // we don't support higher order dimensions for now
        if (numDims != 1)
        {
            H5Sclose(dataSpaceId);
            H5Dclose(dataSetId);

            if (nonSampParentId != -1)
                H5Gclose(nonSampParentId);

            if (sampParentId != -1)
                H5Gclose(sampParentId);

            return PROPERTY_NONE;
        }


        GetScopeAndTypes(dataSetId, oProp.second.scope, oProp.second.inputType,
            oProp.second.outputType);
        GetIndexArray(dataSetId, oProp.second.index);

        ReadProperty(dataSetId, dataSpaceId, oProp.first);

        // make sure we close everything we opened
        H5Sclose(dataSpaceId);
        H5Dclose(dataSetId);

        if (nonSampParentId != -1)
            H5Gclose(nonSampParentId);

        if (sampParentId != -1)
            H5Gclose(sampParentId);

        return retVal;
    }

    if (nonSampParentId != -1)
        H5Gclose(nonSampParentId);

    if (sampParentId != -1)
        H5Gclose(sampParentId);

    return PROPERTY_NONE;
}

void HDFReaderNodeIMPL::getPropertyNamesAndInfo(PropNameInfoMap & oMap)
{
    hid_t propGroup = -1;

    if (H5Lexists(mGroup, ".prop", H5P_DEFAULT) == 1)
    {
        propGroup = H5Gopen2(mGroup, ".prop", H5P_DEFAULT);
    }

    if (propGroup < 0)
        return;

    H5Literate(propGroup, H5_INDEX_NAME, H5_ITER_INC, NULL,
        FindProperties, &oMap);

    // iterate over the sampled map, marking each property as sampled, and
    // then overwriting the property map with it.
    PropNameInfoMap sampledMap;
    PropNameInfoMap::iterator i, end;
    for (i = sampledMap.begin(), end = sampledMap.end(); i != end; ++i)
    {
        i->second.isSampled = true;
        oMap[i->first] = i->second;
    }

    H5Literate(propGroup, H5_INDEX_NAME, H5_ITER_INC, NULL,
        FindProperties, &oMap);

    H5Gclose(propGroup);
}

HDFReaderNodeIMPL::~HDFReaderNodeIMPL()
{
    if (mData->mSampledGroup > -1)
        H5Gclose(mData->mSampledGroup);

    if (mGroup > -1)
        H5Gclose(mGroup);
}

void HDFReaderNodeIMPL::clearNonSampledProperties()
{
    mData->mNonSampledMap.clear();
}

PropertyMap::const_iterator
HDFReaderNodeIMPL::beginNonSampledProperties() const
{
    return mData->mNonSampledMap.begin();
}

PropertyMap::const_iterator
HDFReaderNodeIMPL::endNonSampledProperties() const
{
    return mData->mNonSampledMap.end();
}

PropertyMap::const_iterator
HDFReaderNodeIMPL::beginSampledProperties() const
{
    return mData->mSampledMap.begin();
}

PropertyMap::const_iterator
HDFReaderNodeIMPL::endSampledProperties() const
{
    return mData->mSampledMap.end();
}

PropertyType
HDFReaderNodeIMPL::getProperty(const std::string & iName, PropertyPair & oProp)
{
    PropertyType retVal = PROPERTY_STATIC;

    PropertyMap::iterator it = mData->mNonSampledMap.find(iName);
    if (it == mData->mNonSampledMap.end()) {
        it = mData->mSampledMap.find(iName);

        if (it == mData->mSampledMap.end())
            return PROPERTY_NONE;

        retVal = PROPERTY_ANIMATED;
    }

    oProp = (*it).second;
    return retVal;
}

void HDFReaderNodeIMPL::getPropertyFrames(std::set<float>& ioSamples) const
{
    if (H5Lexists(mGroup, ".prop", H5P_DEFAULT) == 1)
    {
        hid_t propId = H5Gopen2(mGroup, ".prop", H5P_DEFAULT);
        if (H5Lexists(propId, ".samples", H5P_DEFAULT) == 1)
        {
            hid_t sampId = H5Gopen2(propId, ".samples", H5P_DEFAULT);
            H5Literate(sampId, H5_INDEX_NAME, H5_ITER_INC, NULL,
                GetFrames, &ioSamples);
            H5Gclose(sampId);
        }
        H5Gclose(propId);
    }
}

bool HDFReaderNodeIMPL::hasPropertyFrames() const
{
    bool retVal = false;

    // if we've already done a read then we will know whether or not we
    // have sampled data based on if we have the .samples group
    if (mData->mHasReadNonSampled)
    {
        retVal = (mData->mSampledGroup >= 0);
    }
    else if (H5Lexists(mGroup, ".prop", H5P_DEFAULT) == 1)
    {
        hid_t gid = H5Gopen2(mGroup, ".prop", H5P_DEFAULT);
        if (H5Lexists(gid, ".samples", H5P_DEFAULT) == 1)
        {
            retVal = true;
        }
        H5Gclose(gid);
    }

    return retVal;
}

const std::string & HDFReaderNodeIMPL::getName() const
{
    return mData->mName;
}

} // End namespace TakoSPI
} // End namespace Alembic

