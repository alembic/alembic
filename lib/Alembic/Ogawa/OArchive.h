#ifndef _Ogawa_OArchive_h_
#define _Ogawa_OArchive_h_

#include <Ogawa/ns.h>
#include <Ogawa/OGroup.h>
#include <Ogawa/OStream.h>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class OArchive
{
public:
    OArchive(const std::string & iFileName);
    OArchive(std::ostream * iStream);
    ~OArchive();

    OGroupPtr getGroup();

    bool isValid();

private:
    OStreamPtr mStream;
    OGroupPtr mGroup;
};

typedef std::tr1::shared_ptr< OArchive > OArchivePtr;

} // End namespace OGAWA_LIB_VERSION_NS

using namespace OGAWA_LIB_VERSION_NS;

} // End namespace Ogawa

#endif
