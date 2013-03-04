#ifndef _Ogawa_IArchive_h_
#define _Ogawa_IArchive_h_

#include <Ogawa/ns.h>
#include <Ogawa/IStreams.h>
#include <Ogawa/IGroup.h>

#include <istream>
#include <string>
#include <vector>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class IArchive
{
public:
    IArchive(const std::string & iFileName, std::size_t iNumStreams=1);
    IArchive(const std::vector< std::istream * > & iStreams);
    ~IArchive();

    bool isValid() const;

    bool isFrozen() const;

    uint16_t getVersion() const;

    IGroupPtr getGroup() const;

private:
    void init();
    IStreamsPtr mStreams;
    IGroupPtr mGroup;
};

typedef std::tr1::shared_ptr< IArchive > IArchivePtr;

} // End namespace OGAWA_LIB_VERSION_NS

using namespace OGAWA_LIB_VERSION_NS;

} // End namespace Ogawa

#endif
