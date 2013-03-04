#ifndef _Ogawa_IGroup_h_
#define _Ogawa_IGroup_h_

#include <Ogawa/ns.h>
#include <Ogawa/IStreams.h>
#include <Ogawa/IData.h>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class IGroup;
typedef std::tr1::shared_ptr< IGroup > IGroupPtr;

class IGroup
{
public:
    ~IGroup();

    IGroupPtr getGroup(std::size_t iIndex, std::size_t iThreadIndex = 0);
    IDataPtr getData(std::size_t iIndex, std::size_t iThreadIndex = 0);

    std::size_t getNumChildren() const;

    bool isChildGroup(std::size_t iIndex) const;
    bool isChildData(std::size_t iIndex) const;

    bool isEmptyChildGroup(std::size_t iIndex) const;
    bool isEmptyChildData(std::size_t iIndex) const;

private:
    friend class IArchive;
    IGroup(IStreamsPtr iStreams, uint64_t iPos, std::size_t iThreadIndex);

    class PrivateData;
    std::auto_ptr< PrivateData > mData;
};

typedef std::tr1::shared_ptr< IGroup > IGroupPtr;

} // End namespace OGAWA_LIB_VERSION_NS

using namespace OGAWA_LIB_VERSION_NS;

} // End namespace Ogawa

#endif