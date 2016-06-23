#ifndef _Alembic_AbcCoreLayer_CprImpl_h_
#define _Alembic_AbcCoreLayer_CprImpl_h_

#include <Alembic/AbcCoreLayer/Foundation.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

typedef Alembic::Util::shared_ptr< AbcA::PropertyHeader > PropertyHeaderPtr;

//-*****************************************************************************
class CprImpl
    : public AbcA::CompoundPropertyReader
    , public Alembic::Util::enable_shared_from_this< CprImpl >
{
public:

    CprImpl( OrImplPtr iObject,
             CompoundReaderPtrs & iCompounds );

    CprImpl( CprImplPtr iParent, size_t iIndex );

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

private:
    void init( CompoundReaderPtrs & iCompounds );

    // The parent Object
    OrImplPtr m_object;

    // Pointer to parent.
    CprImplPtr m_parent;

    size_t m_index;

    // this compounds header
    PropertyHeaderPtr m_header;

    // each child is made up of the original parent compound, array and scalar
    // properties will only have 1 entry, compounds could have more
    std::vector< CompoundReaderPtrs > m_children;

    std::vector< PropertyHeaderPtr > m_propertyHeaders;

    ChildNameMap m_childNameMap;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreLayer
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_CprImpl_h_

