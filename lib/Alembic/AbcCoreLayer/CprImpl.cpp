#include <Alembic/AbcCoreLayer/CprImpl.h>
#include <Alembic/AbcCoreLayer/OrImpl.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//-*****************************************************************************
// CLASS
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
CprImpl::CprImpl( OrImplPtr iObject, CompoundReaderPtrs & iCompounds )
    : m_object( iObject )
    , m_parent( CprImplPtr() )
    , m_index( 0 )
{
    ABCA_ASSERT( m_object, "Invalid object in CprImpl(Object)" );
    std::string empty;

    // top level compounds have the same metadata as the parent object
    m_header.reset( new AbcA::PropertyHeader( empty,
        m_object->getHeader().getMetaData() ) );

    init( iCompounds );
}

CprImpl::CprImpl( CprImplPtr iParent, size_t iIndex )
    : m_parent( iParent )
    , m_index( iIndex )
{
    ABCA_ASSERT( m_parent, "Invalid compound in CprImpl(CprImplPtr, size_t)" );
    m_object = m_parent->m_object;

    m_header = m_parent->m_propertyHeaders[m_index];

    // get our compounds for the init
    CompoundReaderPtrs  & childVec = m_parent->m_children[m_index];

    CompoundReaderPtrs cmpndVec;
    cmpndVec.reserve( childVec.size() );

    std::string name = m_header->getName();

    CompoundReaderPtrs::iterator it = childVec.begin();
    for ( ; it != childVec.end(); ++it )
    {
        cmpndVec.push_back( (*it)->getCompoundProperty( name ) );
    }
    init( cmpndVec );
}

//-*****************************************************************************
CprImpl::~CprImpl()
{
    // Nothing
}

//-*****************************************************************************
const AbcA::PropertyHeader &CprImpl::getHeader() const
{
    return *( m_header );
}

AbcA::ObjectReaderPtr CprImpl::getObject()
{
    return m_object->asObjectPtr();
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr CprImpl::getParent()
{
    return m_parent;
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr CprImpl::asCompoundPtr()
{
    return shared_from_this();
}

//-*****************************************************************************
size_t CprImpl::getNumProperties()
{
    return m_propertyHeaders.size();
}

//-*****************************************************************************
const AbcA::PropertyHeader & CprImpl::getPropertyHeader( size_t i )
{
    ABCA_ASSERT( i < m_propertyHeaders.size(),
        "Out of range index in CprImpl::getPropertyHeader: " << i );

    return *( m_propertyHeaders[i] );
}

//-*****************************************************************************
const AbcA::PropertyHeader *
CprImpl::getPropertyHeader( const std::string &iName )
{

    ChildNameMap::iterator itr = m_childNameMap.find( iName );

    if( itr !=  m_childNameMap.end() )
    {
        return m_propertyHeaders[ itr->second ].get();
    }

    return 0;
}

//-*****************************************************************************
AbcA::ScalarPropertyReaderPtr
CprImpl::getScalarProperty( const std::string &iName )
{
    ChildNameMap::iterator itr = m_childNameMap.find( iName );

    ABCA_ASSERT( ( itr != m_childNameMap.end() ),
        "There is no child scalar property with that name");

    if( itr != m_childNameMap.end() )
    {
        return m_children[ itr->second ][0]->getScalarProperty( itr->first );
    }

    return AbcA::ScalarPropertyReaderPtr();
}

//-*****************************************************************************
AbcA::ArrayPropertyReaderPtr
CprImpl::getArrayProperty( const std::string &iName )
{
    ChildNameMap::iterator itr = m_childNameMap.find( iName );

    ABCA_ASSERT( ( itr != m_childNameMap.end() ),
                    "There is no child array property with that name");

    if( itr != m_childNameMap.end() )
    {
        return m_children[ itr->second ][0]->getArrayProperty( itr->first );
    }

    return AbcA::ArrayPropertyReaderPtr();
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr
CprImpl::getCompoundProperty( const std::string &iName )
{
    ChildNameMap::iterator itr = m_childNameMap.find( iName );

    ABCA_ASSERT( ( itr != m_childNameMap.end() ),
                    "There is no child compound property with that name");

    if( itr != m_childNameMap.end() )
    {
        return CprImplPtr( new CprImpl( shared_from_this(), itr->second ) );
    }

    return AbcA::CompoundPropertyReaderPtr();
}

//-*****************************************************************************
void CprImpl::init( CompoundReaderPtrs & iCompounds )
{
    CompoundReaderPtrs::iterator it = iCompounds.begin();

    // TODO support pruning here, probably with some custom MetaData?
    // propHeader.getMetaData()["prune"] == "1"
    for ( ; it != iCompounds.end(); ++it )
    {
        for ( size_t i = 0; i < (*it)->getNumProperties(); ++i )
        {
            AbcA::PropertyHeader propHeader = (*it)->getPropertyHeader( i );
            ChildNameMap::iterator nameIt = m_childNameMap.find(
                propHeader.getName() );

            // brand new child, add it and continue
            if ( nameIt == m_childNameMap.end() )
            {
                size_t index = m_childNameMap.size();
                m_childNameMap[ propHeader.getName() ] = index;

                // TODO? We may not need the whole property header, we may only
                // need the name especially if Compound MetaData doesn't need to
                // be combined
                PropertyHeaderPtr propPtr(
                    new AbcA::PropertyHeader( propHeader ) );
                m_propertyHeaders.push_back( propPtr );
                m_children.resize( index + 1 );
                m_children[ index ].push_back( *it );
                continue;
            }
            // only add this onto an existing one IF its a compound and the
            // prop added previously is a compound
            else if ( propHeader.isCompound() &&
                      m_propertyHeaders[ nameIt->second ]->isCompound() )
            {
                // add parent and index to the existing child element, and then
                // update the MetaData
                size_t index = nameIt->second;
                m_children[ index ].push_back( *it );

                // TODO, are there cases where the MetaData should be combined
                // for Compound properties?
            }
        }
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreLayer
} // End namespace Alembic
