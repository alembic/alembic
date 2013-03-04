#ifndef _Ogawa_OData_h_
#define _Ogawa_OData_h_

#include <Ogawa/ns.h>
#include <Ogawa/OStream.h>
#include <tr1/memory>
#include <memory>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class OData
{
public:

    // creates a default empty group
    OData();

    ~OData();

    // rewrites over part of the already written data, does not change
    // the size of the already written data.  If what is attempting
    // to be rewritten exceeds the boundaries of what is already written,
    // the existing data will be unchanged
    void rewrite(std::size_t iSize, void * iData, std::size_t iOffset=0);

    std::size_t getSize() const;

private:
    friend class OGroup; // friend so we can call the constructor below
    OData(OStreamPtr iStream, uint64_t iPos, std::size_t iSize);
    uint64_t getPos() const;

    class PrivateData;
    std::auto_ptr< PrivateData > mData;
};

typedef std::tr1::shared_ptr< OData > ODataPtr;

} // End namespace OGAWA_LIB_VERSION_NS

using namespace OGAWA_LIB_VERSION_NS;

} // End namespace Ogawa

#endif