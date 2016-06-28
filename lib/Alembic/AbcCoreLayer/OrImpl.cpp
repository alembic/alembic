#include <Alembic/AbcCoreLayer/OrImpl.h>
#include <Alembic/AbcCoreLayer/CprImpl.h>
#include <Alembic/Abc/ICompoundProperty.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//-*****************************************************************************
// OBJECT READER IMPLEMENTATION
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
OrImpl::OrImpl( ArImplPtr iArchive,
                std::vector< AbcA::ObjectReaderPtr > & iTops,
                ObjectHeaderPtr iHeader )
              : m_parent( OrImplPtr() )
              , m_index( 0 )
              , m_archive( iArchive )
              , m_header( iHeader )
{
    ABCA_ASSERT( m_archive, "Invalid archive in OrImpl(Archive)" );
    init( iTops );
}

OrImpl::OrImpl( OrImplPtr iParent, size_t iIndex )
              : m_parent( iParent )
              , m_index( iIndex )
{
    ABCA_ASSERT( m_parent, "Invalid object in OrImpl(OrImplPtr, size_t)" );

    m_archive = m_parent->m_archive;
    m_header = m_parent->m_childHeaders[m_index];

    // get our objects for the init
    std::vector< ObjectAndIndex >  & childVec =
        m_parent->m_children[m_index];

    std::vector< AbcA::ObjectReaderPtr > objVec;
    objVec.reserve( childVec.size() );

    std::vector< ObjectAndIndex >::iterator it = childVec.begin();
    for ( ; it != childVec.end(); ++it )
    {
        objVec.push_back( it->first->getChild( it->second ) );
    }
    init( objVec );
}

//-*****************************************************************************
OrImpl::~OrImpl()
{
    // Nothing.
}

//-*****************************************************************************
const AbcA::ObjectHeader & OrImpl::getHeader() const
{
    return *m_header;
}

//-*****************************************************************************
AbcA::ArchiveReaderPtr OrImpl::getArchive()
{
    return m_archive;
}

//-*****************************************************************************
AbcA::ObjectReaderPtr OrImpl::getParent()
{
    return m_parent;
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr OrImpl::getProperties()
{
    return CprImplPtr( new CprImpl( shared_from_this(), m_properties ) );
}

//-*****************************************************************************
size_t OrImpl::getNumChildren()
{
    return m_childHeaders.size();
}

//-*****************************************************************************
const AbcA::ObjectHeader & OrImpl::getChildHeader( size_t i )
{
    ABCA_ASSERT( i < m_childHeaders.size(),
        "Out of range index in OrData::getChildHeader: " << i );

    return *( m_childHeaders[i] );
}

//-*****************************************************************************
const AbcA::ObjectHeader * OrImpl::getChildHeader( const std::string &iName )
{
    ChildNameMap::iterator findChildItr = m_childNameMap.find( iName );

    if( findChildItr != m_childNameMap.end() )
    {
        return m_childHeaders[ findChildItr->second ].get();
    }

    return 0;
}

//-*****************************************************************************
AbcA::ObjectReaderPtr OrImpl::getChild( const std::string &iName )
{
    ChildNameMap::iterator findChildItr = m_childNameMap.find( iName );

    if( findChildItr != m_childNameMap.end() )
    {
        return OrImplPtr( new OrImpl( shared_from_this(),
                                      findChildItr->second ) );
    }

    return AbcA::ObjectReaderPtr();
}

AbcA::ObjectReaderPtr OrImpl::getChild( size_t i )
{
    if ( i < m_childHeaders.size() )
    {
        return OrImplPtr( new OrImpl( shared_from_this(), i ) );
    }

    return AbcA::ObjectReaderPtr();
}

//-*****************************************************************************
AbcA::ObjectReaderPtr OrImpl::asObjectPtr()
{
    return shared_from_this();
}

//-*****************************************************************************
bool OrImpl::getPropertiesHash( Util::Digest & oDigest )
{
    if ( ! m_parent )
    {
        return false;
    }

    const std::vector< ObjectAndIndex >  & childVec =
        m_parent->m_children[m_index];

    if ( childVec.size() == 1 )
    {
        return childVec[0].first->getPropertiesHash( oDigest );
    }

    return false;
}

//-*****************************************************************************
bool OrImpl::getChildrenHash( Util::Digest & oDigest )
{

    if ( ! m_parent )
    {
        return false;
    }

    const std::vector< ObjectAndIndex >  & childVec =
        m_parent->m_children[m_index];

    // TODO, it wouldn't be too expensive to check that only one of these
    // has any children
    if ( childVec.size() == 1 )
    {
        return childVec[0].first->getChildrenHash( oDigest );
    }

    return false;
}

//-*****************************************************************************
// This layers the children together, and creates
void OrImpl::init( std::vector< AbcA::ObjectReaderPtr > & iObjects )
{

    std::vector< AbcA::ObjectReaderPtr >::iterator it =
        iObjects.begin();

    m_properties.reserve( iObjects.size() );

    // TODO support pruning here, probably with some custom MetaData?
    // objHeader.getMetaData()["prune"] == "1"
    for ( ; it != iObjects.end(); ++it )
    {
        m_properties.push_back( (*it)->getProperties() );
        for ( size_t i = 0; i < (*it)->getNumChildren(); ++i )
        {
            AbcA::ObjectHeader objHeader = (*it)->getChildHeader( i );
            ChildNameMap::iterator nameIt = m_childNameMap.find(
                objHeader.getName() );

            size_t index = 0;

            // brand new child, add it and continue
            if ( nameIt == m_childNameMap.end() )
            {
                index = m_childNameMap.size();
                m_childNameMap[ objHeader.getName() ] = index;
                ObjectHeaderPtr headerPtr(
                    new AbcA::ObjectHeader( objHeader ) );
                m_childHeaders.push_back( headerPtr );
                m_children.resize( index + 1 );
                m_children[ index ].push_back( ObjectAndIndex( *it, i ) );
                continue;
            }

            // add parent and index to the existing child element, and then
            // update the MetaData
            index = nameIt->second;
            m_children[ index ].push_back( ObjectAndIndex( *it, i ) );

            // update the found childs meta data
            m_childHeaders[ index ]->getMetaData().appendOnlyUnique(
                objHeader.getMetaData() );
        }
    }
}


} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreLayer
} // End namespace Alembic

