#ifndef _Alembic_AbcCoreLayer_CprImpl_h_
#define _Alembic_AbcCoreLayer_CprImpl_h_

#include <Alembic/AbcCoreLayer/Foundation.h>
#include <Alembic/AbcCoreAbstract/ForwardDeclarations.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class CprImpl
    : public AbcA::CompoundPropertyReader
    , public Alembic::Util::enable_shared_from_this<CprImpl>
{
public:

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
	// Native to this CprImpl implementation
	//-*************************************************************************

    void layerInProperties( AbcA::CompoundPropertyReaderPtr iProps );

    void addChildReader( AbcA::BasePropertyReaderPtr propertyReader );

private:

    void initializeMaps();

    void recordProperties();

    bool m_mapsInitialized;

    // Pointer to parent.
    CprImplPtr m_parent;

    // My Object
    OrImplPtr m_object;

    AbcA::BasePropertyReaderPtr m_originalPropertyReader;

    std::vector< CprImplPtr > m_childCprs;

    std::vector< AbcA::PropertyHeader > m_propertyHeaders;

    ChildNameMap m_childNameMap;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreLayer
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_CprImpl_h_

