#ifndef _Ogawa_IStreams_h_
#define _Ogawa_IStreams_h_

#include <Ogawa/ns.h>

#include <istream>
#include <memory>
#include <tr1/memory>
#include <stdint.h>
#include <string>
#include <vector>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class IStreams
{
public:
    IStreams(const std::string & iFileName, std::size_t iNumStreams=1);
    IStreams(const std::vector< std::istream * > & iStreams);
    ~IStreams();

    bool isValid();
    bool isFrozen();
    uint16_t getVersion();

    // locks on the threadId, seeks to iPos, and reads iSize bytes into oBuf
    void read(std::size_t iThreadId, uint64_t iPos,
              std::size_t iSize, void * oBuf);

private:
    // noncopyable
    IStreams(const IStreams &);
    const IStreams & operator=(const IStreams &);

    void init();

    class PrivateData;
    std::auto_ptr< PrivateData > mData;
};
typedef std::tr1::shared_ptr< IStreams > IStreamsPtr;

} // End namespace OGAWA_LIB_VERSION_NS

using namespace OGAWA_LIB_VERSION_NS;

} // End namespace Ogawa

#endif