#ifndef _Ogawa_IData_h_
#define _Ogawa_IData_h_

#include <Ogawa/ns.h>
#include <Ogawa/IStreams.h>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class IData
{
public:

    ~IData();

    void read(std::size_t iSize, void * iData, std::size_t iOffset=0,
              std::size_t iThreadId=0);

    std::size_t getSize() const;

private:
    friend class IGroup;
    IData(IStreamsPtr iStreams, uint64_t iPos, std::size_t iThreadId);

    class PrivateData;
    std::auto_ptr< PrivateData > mData;
};

typedef std::tr1::shared_ptr< IData > IDataPtr;

} // End namespace OGAWA_LIB_VERSION_NS

using namespace OGAWA_LIB_VERSION_NS;

} // End namespace Ogawa

#endif