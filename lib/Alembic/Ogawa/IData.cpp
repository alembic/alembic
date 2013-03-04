#include <Ogawa/IGroup.h>
#include <Ogawa/IData.h>
#include <Ogawa/IStreams.h>

#include <Ogawa/constants.h>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class IData::PrivateData
{
public:
    PrivateData(IStreamsPtr iStreams)
    {
        streams = iStreams;
    };

    ~PrivateData() {};

    IStreamsPtr streams;

    // set after freeze
    uint64_t pos;
    std::size_t size;
};

IData::~IData()
{

}

IData::IData(IStreamsPtr iStreams, uint64_t iPos, std::size_t iThreadId) :
    mData(new IData::PrivateData(iStreams))
{
    mData->size = 0;

    // strip off the top bit (indicates data) to get our seek position
    mData->pos = iPos & INVALID_GROUP;

    uint64_t size = 0;

    mData->streams->read(iThreadId, mData->pos, 8, &size);
    mData->size = (std::size_t)(size);
}

void IData::read(std::size_t iSize, void * iData, std::size_t iOffset,
                 std::size_t iThreadId)
{
    // don't read anything if we will read beyond our buffer
    if (iSize == 0 || mData->size == 0 || iOffset + iSize > mData->size)
    {
        return;
    }

    // +8 is to account for the size
    mData->streams->read(iThreadId, mData->pos + iOffset + 8, iSize, iData);
}

std::size_t IData::getSize() const
{
    return mData->size;
}


}
}