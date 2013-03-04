#include <Ogawa/OArchive.h>
#include <Ogawa/OStream.h>
#include <Ogawa/OGroup.h>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

OArchive::OArchive(const std::string & iFileName) :
    mStream(new OStream(iFileName))
{
    mGroup.reset(new OGroup(mStream));
}

OArchive::OArchive(std::ostream * iStream) :
    mStream(new OStream(iStream)), mGroup(new OGroup(mStream))
{
}

OArchive::~OArchive()
{
}

bool OArchive::isValid()
{
    return mStream->isValid();
}

OGroupPtr OArchive::getGroup()
{
    return mGroup;
}

}
}