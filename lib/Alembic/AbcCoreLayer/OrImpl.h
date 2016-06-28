#ifndef _Alembic_AbcCoreLayer_OrImpl_h_
#define _Alembic_AbcCoreLayer_OrImpl_h_

#include <Alembic/AbcCoreLayer/Foundation.h>
#include <Alembic/AbcCoreLayer/ArImpl.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

typedef std::pair < AbcA::ObjectReaderPtr, size_t > ObjectAndIndex;

//-*****************************************************************************
class OrImpl
    : public AbcA::ObjectReader
    , public Alembic::Util::enable_shared_from_this<OrImpl>
{

public:

    OrImpl( ArImplPtr iArchive,
            std::vector< AbcA::ObjectReaderPtr > & iTops,
            ObjectHeaderPtr iHeader);

    OrImpl( OrImplPtr iParent, size_t iIndex );

    virtual ~OrImpl();

    //-*************************************************************************
    // ABSTRACT
    //-*************************************************************************
    virtual const AbcA::ObjectHeader & getHeader() const;

    virtual AbcA::ArchiveReaderPtr getArchive();

    virtual AbcA::ObjectReaderPtr getParent();

    virtual AbcA::CompoundPropertyReaderPtr getProperties();

    virtual size_t getNumChildren();

    virtual const AbcA::ObjectHeader & getChildHeader( size_t i );

    virtual const AbcA::ObjectHeader * getChildHeader
    ( const std::string &iName );

    virtual AbcA::ObjectReaderPtr getChild( const std::string &iName );

    virtual AbcA::ObjectReaderPtr getChild( size_t i );

    virtual AbcA::ObjectReaderPtr asObjectPtr();

    virtual bool getPropertiesHash( Util::Digest & oDigest );

    virtual bool getChildrenHash( Util::Digest & oDigest );

private:

    // builds up our data
    void init( std::vector< AbcA::ObjectReaderPtr > & iObjects );

    // The parent object
    OrImplPtr m_parent;

    // The index within the parent object where we can get our data
    size_t m_index;

    ArImplPtr m_archive;

    // this objects header
    ObjectHeaderPtr m_header;

    // all of our compounded child headers
    std::vector< ObjectHeaderPtr > m_childHeaders;

    // each child is made up of the original parent objects and the index
    // in each of them where that child lives
    std::vector< std::vector< ObjectAndIndex > > m_children;

    // all of our top properties
    // TODO should we replace this with the top compound CprImpl in a weak_ptr?
    std::vector< AbcA::CompoundPropertyReaderPtr > m_properties;

    ChildNameMap m_childNameMap;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_OrImpl_h_
