#ifndef _Alembic_AbcCoreLayer_CprImpl_h_
#define _Alembic_AbcCoreLayer_CprImpl_h_

#include <Alembic/AbcCoreLayer/Foundation.h>
#include <Alembic/AbcCoreAbstract/ForwardDeclarations.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

class OrImpl;

//-*****************************************************************************
class CprImpl
    : public AbcA::CompoundPropertyReader
    , public Alembic::Util::enable_shared_from_this<CprImpl>
{
public:

    // For construction from a compound property reader
//    CprImpl( AbcA::CompoundPropertyReaderPtr iParent,
//             Ogawa::IGroupPtr iGroup,
//             PropertyHeaderPtr iHeader,
//             std::size_t iThreadId,
//             const std::vector< AbcA::MetaData > & iIndexedMetaData );

    CprImpl( Alembic::Util::shared_ptr< OrImpl > iObject,
    		 AbcA::CompoundPropertyReaderPtr iOriginalCpr,
    		 CprImplPtr iParentCpr = CprImplPtr());

    CprImpl( Alembic::Util::shared_ptr< OrImpl > iObject,
			 AbcA::BasePropertyReaderPtr iOriginalCpr,
			 CprImplPtr iParentCpr = CprImplPtr());

    virtual ~CprImpl();

    //-*************************************************************************
    // FROM ABSTRACT BasePropertyReader
    //-*************************************************************************
    virtual const AbcA::PropertyHeader & getHeader() const;

    virtual AbcA::ObjectReaderPtr getObject();

    virtual AbcA::CompoundPropertyReaderPtr getParent();

    virtual AbcA::CompoundPropertyReaderPtr asCompoundPtr();

    //-*************************************************************************
    // FROM ABSTRACT CompoundPropertyReader
    //-*************************************************************************
    virtual size_t getNumProperties();

    virtual const AbcA::PropertyHeader & getPropertyHeader( size_t i );

    virtual const AbcA::PropertyHeader *
    getPropertyHeader( const std::string &iName );

    virtual AbcA::ScalarPropertyReaderPtr
    getScalarProperty( const std::string &iName );

    virtual AbcA::ArrayPropertyReaderPtr
    getArrayProperty( const std::string &iName );

    virtual AbcA::CompoundPropertyReaderPtr
    getCompoundProperty( const std::string &iName );

    //-*************************************************************************
	// Native to this CprImpl implemlentation
	//-*************************************************************************

    void layerInProperties( AbcA::CompoundPropertyReaderPtr iProps );

    void addChildReader( AbcA::BasePropertyReaderPtr propertyReader );

    //TODO: Remove this stuff after we put all the layering in AbcCorelayer
protected:

	virtual size_t getNumPropertiesImpl() { return 0; }

    virtual const AbcA::PropertyHeader & getPropertyHeaderImpl( size_t i ) { static AbcA::PropertyHeader sHeader; return sHeader; }

    virtual const AbcA::PropertyHeader *
    getPropertyHeaderImpl( const std::string &iName ) { return 0; }

    virtual AbcA::ScalarPropertyReaderPtr
    getScalarPropertyImpl( const std::string &iName ) { return AbcA::ScalarPropertyReaderPtr(); }

    virtual AbcA::ArrayPropertyReaderPtr
    getArrayPropertyImpl( const std::string &iName ) { return AbcA::ArrayPropertyReaderPtr(); }

    virtual AbcA::CompoundPropertyReaderPtr
    getCompoundPropertyImpl( const std::string &iName ) { AbcA::CompoundPropertyReaderPtr(); }

private:

    void initializeMaps();

    void recordProperties();

    bool m_mapsInitialized;

    // Pointer to parent.
    CprImplPtr m_parent;

    // My header
    //PropertyHeaderPtr m_header;

    // My Object
    OrImplPtr m_object;

    //CprDataPtr m_data;

    AbcA::BasePropertyReaderPtr m_originalCpr;

    std::vector< CprImplPtr > m_childCprs;

    std::vector< AbcA::PropertyHeader > m_propertyHeaders;

    ChildNameMap m_childNameMap;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreLayer
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_CprImpl_h_

