#include <Ogawa/IGroup.h>
#include <Ogawa/IArchive.h>
#include <Ogawa/IStreams.h>

#include <Ogawa/constants.h>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class IGroup::PrivateData
{
public:
    PrivateData(IStreamsPtr iStreams)
    {
        streams = iStreams;
    }

    ~PrivateData() {}

    IStreamsPtr streams;

    std::vector<uint64_t> childVec;
};

IGroup::IGroup(IStreamsPtr iStreams, uint64_t iPos, std::size_t iThreadIndex) :
    mData(new IGroup::PrivateData(iStreams))
{
    // all done, we have no children, or our streams aren't good
    if (iPos == EMPTY_GROUP || !mData->streams || !mData->streams->isValid())
    {
        return;
    }

    uint64_t numChildren = 0;
    mData->streams->read(iThreadIndex, iPos, 8, &numChildren);

    // 0 should NOT have been written, this groups should have been the
    // special EMPTY_GROUP instead

    mData->childVec.resize(numChildren);
    mData->streams->read(iThreadIndex, iPos + 8, numChildren * 8,
                         &(mData->childVec.front()));
}

IGroup::~IGroup()
{

}

IGroupPtr IGroup::getGroup(std::size_t iIndex, std::size_t iThreadIndex)
{
    IGroupPtr child;
    if (isChildGroup(iIndex))
    {
        child.reset(new IGroup(mData->streams, mData->childVec[iIndex],
                               iThreadIndex));
    }
    return child;
}

IDataPtr IGroup::getData(std::size_t iIndex, std::size_t iThreadIndex)
{
    IDataPtr child;
    if (isChildData(iIndex))
    {
        child.reset(new IData(mData->streams, mData->childVec[iIndex],
                              iThreadIndex));
    }
    return child;
}

std::size_t IGroup::getNumChildren() const
{
    return mData->childVec.size();
}

bool IGroup::isChildGroup(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            (mData->childVec[iIndex] & EMPTY_DATA) == 0);
}

bool IGroup::isChildData(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            (mData->childVec[iIndex] & EMPTY_DATA) != 0);
}

bool IGroup::isEmptyChildGroup(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            mData->childVec[iIndex] == EMPTY_GROUP);
}

bool IGroup::isEmptyChildData(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
        mData->childVec[iIndex] == EMPTY_DATA);
}

}
}