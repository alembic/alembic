#include <Ogawa/OGroup.h>
#include <Ogawa/OData.h>
#include <Ogawa/OStream.h>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class OData::PrivateData
{
public:

    // for the empty dataset
    PrivateData()
    {
        pos = 0;
        size = 0;
    }

    PrivateData(OStreamPtr iStream, uint64_t iPos, std::size_t iSize) :
        stream(iStream), pos(iPos), size(iSize) {}

    ~PrivateData() {}

    OStreamPtr stream;

    uint64_t pos;
    std::size_t size;
};

OData::OData() : mData(new OData::PrivateData())
{
}

OData::OData(OStreamPtr iStream, uint64_t iPos, std::size_t iSize)
    : mData(new OData::PrivateData(iStream, iPos, iSize))
{
}

OData::~OData()
{
}

void OData::rewrite(std::size_t iSize, void * iData, std::size_t iOffset)
{

    // don't write anything if we will write beyond our buffer or the
    // stream is invalid
    if (!mData->stream || iSize == 0 || mData->size == 0 ||
        iOffset + iSize > mData->size)
    {
        return;
    }

    // +8 is to account for the written out size
    mData->stream->seek(mData->pos + iOffset + 8);
    mData->stream->write(iData, iSize);
}

std::size_t OData::getSize() const
{
    return mData->size;
}

uint64_t OData::getPos() const
{
    return mData->pos;
}

}
}