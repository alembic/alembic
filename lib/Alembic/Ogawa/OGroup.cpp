#include <Ogawa/OGroup.h>
#include <Ogawa/OArchive.h>
#include <Ogawa/OData.h>
#include <Ogawa/OStream.h>

#include <Ogawa/constants.h>
#include <stdint.h>
#include <vector>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

typedef std::pair< OGroupPtr, std::size_t > ParentPair;
typedef std::vector< ParentPair > ParentPairVec;

class OGroup::PrivateData
{
public:
    PrivateData() {};
    ~PrivateData() {};

    OStreamPtr stream;

    // used before freeze
    ParentPairVec parents;

    // used before and after freeze
    std::vector<uint64_t> childVec;

    // set after freeze
    uint64_t pos;
};

OGroup::OGroup(OGroupPtr iParent, std::size_t iIndex)
    : mData(new OGroup::PrivateData())
{
    mData->stream = iParent->mData->stream;
    mData->parents.push_back( ParentPair(iParent, iIndex) );
    mData->pos = INVALID_GROUP;
}

OGroup::OGroup(OStreamPtr iStream)
    : mData(new OGroup::PrivateData())
{
    mData->stream = iStream;
    mData->parents.push_back(ParentPair(OGroupPtr(), 0));
    mData->pos = INVALID_GROUP;
}

OGroup::~OGroup()
{
    freeze();
}

OGroupPtr OGroup::addGroup()
{
    OGroupPtr child;
    if (!isFrozen())
    {
        mData->childVec.push_back(0);
        child.reset(new OGroup(shared_from_this(), mData->childVec.size() - 1));
    }
    return child;
}

ODataPtr OGroup::createData(std::size_t iSize, const void * iData)
{
    ODataPtr child;
    if (isFrozen())
    {
        return child;
    }

    if (iSize == 0)
    {
        mData->childVec.push_back(EMPTY_DATA);
        child.reset(new OData());
        return child;
    }

    uint64_t pos = mData->stream->getAndSeekEndPos();

    uint64_t size = iSize;
    mData->stream->write(&size, 8);
    mData->stream->write(iData, iSize);

    child.reset(new OData(mData->stream, pos, iSize));

    return child;
}

ODataPtr OGroup::addData(std::size_t iSize, const void * iData)
{
    ODataPtr child = OGroup::createData(iSize, iData);
    if (child)
    {
        // flip top bit for data so we can easily distinguish between it and
        // a group
        mData->childVec.push_back(child->getPos() | 0x8000000000000000ULL);
    }
    return child;
}

ODataPtr OGroup::createData(std::size_t iNumData, const std::size_t * iSizes,
                         const void ** iDatas)
{
    ODataPtr child;
    if (isFrozen())
    {
        return child;
    }

    uint64_t totalSize = 0;
    for (std::size_t i = 0; i < iNumData; ++i)
    {
        totalSize += iSizes[i];
    }

    if (totalSize == 0)
    {
        mData->childVec.push_back(EMPTY_DATA);
        child.reset(new OData());
        return child;
    }

    uint64_t pos = mData->stream->getAndSeekEndPos();

    mData->stream->write(&totalSize, 8);
    for (std::size_t i = 0; i < iNumData; ++i)
    {
        std::size_t size = iSizes[i];
        if (size != 0)
        {
            mData->stream->write(iDatas[i], size);
        }
    }

    child.reset(new OData(mData->stream, pos, totalSize));

    return child;
}

ODataPtr OGroup::addData(std::size_t iNumData, const std::size_t * iSizes,
                            const void ** iDatas)
{
    ODataPtr child = createData(iNumData, iSizes, iDatas);
    if (child)
    {
        // flip top bit for data so we can easily distinguish between it and
        // a group
        mData->childVec.push_back(child->getPos() | 0x8000000000000000ULL);
    }
    return child;
}

void OGroup::addData(ODataPtr iData)
{
    if (!isFrozen())
    {
        mData->childVec.push_back(iData->getPos());
    }
}

void OGroup::addGroup(OGroupPtr iGroup)
{
    if (!isFrozen())
    {
        if (iGroup->isFrozen())
        {
            mData->childVec.push_back(iGroup->mData->pos);
        }
        else
        {
            mData->childVec.push_back(EMPTY_GROUP);
            iGroup->mData->parents.push_back(
                ParentPair(shared_from_this(), mData->childVec.size() - 1));
        }
    }
}

void OGroup::addEmptyGroup()
{
    if (!isFrozen())
    {
        mData->childVec.push_back(EMPTY_GROUP);
    }
}

void OGroup::addEmptyData()
{
    if (!isFrozen())
    {
        mData->childVec.push_back(EMPTY_DATA);
    }
}

// no more children can be added, commit to the stream
void OGroup::freeze()
{
    // bail if we've already done this work
    if (isFrozen())
    {
        return;
    }

    // we ended up not adding any children, so no need to commit this group
    // to disk, use empty group instead
    if (mData->childVec.empty())
    {
        mData->pos = 0;
    }
    else
    {
        mData->pos = mData->stream->getAndSeekEndPos();
        uint64_t size = mData->childVec.size();
        mData->stream->write(&size, 8);
        mData->stream->write(&mData->childVec.front(), size*8);
    }

    // go through and update each of the parents
    ParentPairVec::iterator it;
    for(it = mData->parents.begin(); it != mData->parents.end(); ++it)
    {
        // special group owned by the archive
        if (!it->first && it->second == 0)
        {
            mData->stream->seek(8);
            mData->stream->write(&mData->pos, 8);
            continue;
        }
        else if (it->first->isFrozen())
        {
            mData->stream->seek(it->first->mData->pos + (it->second + 1) * 8);
            mData->stream->write(&mData->pos, 8);
        }
        it->first->mData->childVec[it->second] = mData->pos;
    }

    mData->parents.clear();

}

bool OGroup::isFrozen()
{
    return mData->pos != INVALID_GROUP;
}

std::size_t OGroup::getNumChildren() const
{
    return mData->childVec.size();
}

bool OGroup::isChildGroup(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            (mData->childVec[iIndex] & EMPTY_DATA) == 0);
}

bool OGroup::isChildData(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            (mData->childVec[iIndex] & EMPTY_DATA) != 0);
}

bool OGroup::isChildEmptyGroup(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            mData->childVec[iIndex] == EMPTY_GROUP);
}

bool OGroup::isChildEmptyData(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
        mData->childVec[iIndex] == EMPTY_DATA);
}

void OGroup::replaceData(std::size_t iIndex, ODataPtr iData)
{
    if (!isChildData(iIndex))
    {
        return;
    }

    uint64_t pos = iData->getPos();
    if (isFrozen())
    {
        mData->stream->seek(mData->pos + (iIndex + 1) * 8);
        mData->stream->write(&pos, 8);
    }
    mData->childVec[iIndex] = pos;
}

}
}