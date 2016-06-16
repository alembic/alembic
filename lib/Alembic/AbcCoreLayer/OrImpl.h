#ifndef _Alembic_AbcCoreLayer_OrImpl_h_
#define _Alembic_AbcCoreLayer_OrImpl_h_

#include <Alembic/AbcCoreLayer/Foundation.h>
//#include <Alembic/AbcCoreOgawa/OrData.h>
#include <Alembic/AbcCoreLayer/ArImpl.h>
//#include <Alembic/Abc/IObject.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

typedef Alembic::Util::shared_ptr<AbcA::ObjectHeader> ObjectHeaderPtr;

class CprImpl;

class OrImpl;
typedef Alembic::Util::shared_ptr< OrImpl > OrImplPtr;

//-*****************************************************************************
class OrImpl
    : public AbcA::ObjectReader
    , public Alembic::Util::enable_shared_from_this<OrImpl>
{

public:

    OrImpl( Alembic::Util::shared_ptr< ArImpl > iArchive,
            AbcA::ObjectReaderPtr iThisObject,
            Alembic::Util::shared_ptr< OrImpl > iParent);


//    OrImpl( AbcA::ObjectReaderPtr iParent,
//            Ogawa::IGroupPtr iParentGroup,
//            std::size_t iIndex,
//            ObjectHeaderPtr iHeader );

    OrImpl(){};

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

    //TODO REMOVE THIS ONCE THIS LAYER IS FUNCTIONAL
    virtual size_t getNumChildrenImpl(){};
    virtual const AbcA::ObjectHeader & getChildHeaderImpl( size_t i) {};
    virtual const AbcA::ObjectHeader * getChildHeaderImpl( const std::string &iName ){};
    virtual AbcA::ObjectReaderPtr getChildImpl( const std::string &iName ){};
    virtual AbcA::ObjectReaderPtr getChildImpl( size_t i ){};

    void layerInObjectHierarchy( AbcA::ObjectReaderPtr iObject );

protected:

    void initializeMaps( );

    void recordChildren( );

private:

    Alembic::Util::shared_ptr< ArImpl > getArchiveImpl() const;

    // The ObjectReader from the object's original, non-layered archive
    AbcA::ObjectReaderPtr m_originalObjectReader;

    //TODO TEMP TEMP TEMP REMOVE
    AbcA::CompoundPropertyReaderPtr m_originalCompoundPropertyReader;

    // The parent object
    Alembic::Util::shared_ptr< OrImpl > m_parent;

    Alembic::Util::shared_ptr< ArImpl > m_archive;


//    OrDataPtr m_data;
//
    AbcA::ObjectHeader m_header;

    Alembic::Util::shared_ptr< CprImpl > m_compoundPropertyReader;

    bool m_mapsInitialized;

    std::vector< OrImplPtr > m_childObjects;

    ChildNameMap m_childNameMap;

};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_OrImpl_h_
