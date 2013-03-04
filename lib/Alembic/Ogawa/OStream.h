#ifndef _Ogawa_OStream_h_
#define _Ogawa_OStream_h_

#include <Ogawa/ns.h>

#include <ostream>
#include <stdint.h>
#include <string>
#include <tr1/memory>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class OStream
{
public:
    OStream(const std::string & iFileName);
    OStream(std::ostream * iStream);
    ~OStream();

    bool isValid();

    uint64_t getAndSeekEndPos();
    void write(const void * iBuf, std::size_t iSize);
    void seek(uint64_t iPos);

private:
    // noncopyable
    OStream(const OStream &);
    const OStream & operator=(const OStream &);

    class PrivateData;
    std::auto_ptr< PrivateData > mData;

    void init();
};

typedef std::tr1::shared_ptr< OStream > OStreamPtr;

} // End namespace OGAWA_LIB_VERSION_NS

using namespace OGAWA_LIB_VERSION_NS;

} // End namespace Ogawa

#endif