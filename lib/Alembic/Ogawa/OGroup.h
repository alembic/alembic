#ifndef _Ogawa_OGroup_h_
#define _Ogawa_OGroup_h_

#include <Ogawa/ns.h>
#include <Ogawa/OStream.h>
#include <Ogawa/OData.h>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class OGroup;
typedef std::tr1::shared_ptr< OGroup > OGroupPtr;

class OGroup : public std::tr1::enable_shared_from_this< OGroup >
{
public:
    ~OGroup();

    // create a group and add it as a child to this group
    OGroupPtr addGroup();

    // write the data stream and add it as a child to this group
    ODataPtr addData(std::size_t iSize, const void * iData);

    // write data streams from multiple sources as one continuous data stream
    // and add it as a child to this group
    ODataPtr addData(std::size_t iNumData, const std::size_t * iSizes,
                     const void ** iDatas);

    // write a data stream but DON'T add it as a child to this group
    // If ODataPtr isn't added to this or any other group, you will
    // end up abandoning it within the file and waste disk space.
    ODataPtr createData(std::size_t iSize, const void * iData);

    // write data streams as one continuous data stream but DON'T add it as a
    // child to this group.
    // If ODataPtr isn't added to this or any other group, you will
    // end up abandoning it within the file and waste disk space.
    ODataPtr createData(std::size_t iNumData, const std::size_t * iSizes,
                     const void ** iDatas);

    // reference existing data
    void addData(ODataPtr iData);

    // reference an existing group
    void addGroup(OGroupPtr iGroup);

    // convenience function for adding a default NULL group
    void addEmptyGroup();

    // convenience function for adding empty data
    void addEmptyData();

    // can no longer add any more children, we can still update them
    // via the replace calls though
    void freeze();

    bool isFrozen();

    std::size_t getNumChildren() const;

    bool isChildGroup(std::size_t iIndex) const;

    bool isChildData(std::size_t iIndex) const;

    bool isChildEmptyGroup(std::size_t iIndex) const;

    bool isChildEmptyData(std::size_t iIndex) const;

    void replaceData(std::size_t iIndex, ODataPtr iData);

    // currently I'm going to leave this out, because a bad implementation
    // could cause all sorts of subtle race conditions when unfrozen children
    // are suddenly frozen.  It may also not be necessary (you can still
    // reference an existing group)
    // if this is necessary, an easy compromise might be that iGroup HAS to
    // be frozen, much like how replaceData deals with something implicitly
    // frozen
    //void replaceGroup(std::size_t iIndex, OGroupPtr iGroup);

private:
    friend class OArchive;
    OGroup(OStreamPtr iStream);

    OGroup(OGroupPtr iParent, std::size_t iIndex);

    class PrivateData;
    std::auto_ptr< PrivateData > mData;
};

} // End namespace OGAWA_LIB_VERSION_NS

using namespace OGAWA_LIB_VERSION_NS;

} // End namespace Ogawa

#endif