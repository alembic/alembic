#include <Ogawa/IArchive.h>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

IArchive::IArchive(const std::string & iFileName, std::size_t iNumStreams) :
    mStreams(new IStreams(iFileName, iNumStreams))
{
    init();
}

IArchive::IArchive(const std::vector< std::istream * > & iStreams) :
    mStreams(new IStreams(iStreams))
{
    init();
}

void IArchive::init()
{
    if (mStreams->isValid())
    {
        uint64_t pos = 0;
        mStreams->read(0, 8, 8, &pos);
        mGroup.reset(new IGroup(mStreams, pos, 0));
    }
}

IArchive::~IArchive()
{
}

bool IArchive::isValid() const
{
    return mStreams->isValid();
}

bool IArchive::isFrozen() const
{
    return mStreams->isFrozen();
}

uint16_t IArchive::getVersion() const
{
    return mStreams->getVersion();
}

IGroupPtr IArchive::getGroup() const
{
    return mGroup;
}

}
}