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
OrImpl::OrImpl( Alembic::Util::shared_ptr< ArImpl > iArchive,
                AbcA::ObjectReaderPtr iThisObject,
                Alembic::Util::shared_ptr< OrImpl > iParent
                )
                : m_archive( iArchive )
                , m_originalObjectReader( iThisObject )
                , m_parent( iParent )
                , m_mapsInitialized( false )
{
    ABCA_ASSERT( m_archive, "Invalid archive in OrImpl(Archive)" );

    m_header = m_originalObjectReader->getHeader();
}

//-*****************************************************************************
OrImpl::~OrImpl()
{
    // Nothing.
}

//-*****************************************************************************
const AbcA::ObjectHeader & OrImpl::getHeader() const
{
    m_originalObjectReader->getHeader();
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
	initializeMaps();

    return m_compoundPropertyReader;
}

//-*****************************************************************************
size_t OrImpl::getNumChildren()
{
	initializeMaps();

    return m_childObjects.size();
}

//-*****************************************************************************
const AbcA::ObjectHeader & OrImpl::getChildHeader( size_t i )
{
	initializeMaps();

    return m_childObjects[ i ]->m_header;
}

//-*****************************************************************************
const AbcA::ObjectHeader * OrImpl::getChildHeader( const std::string &iName )
{
	initializeMaps();

	ChildNameMap::iterator findChildItr = m_childNameMap.find( iName );

	if( findChildItr != m_childNameMap.end() )
	{
		return &m_childObjects[ findChildItr->second ]->m_header;
	}

	return 0;
}

//-*****************************************************************************
AbcA::ObjectReaderPtr OrImpl::getChild( const std::string &iName )
{
	initializeMaps();

	ChildNameMap::iterator findChildItr = m_childNameMap.find( iName );

	if( findChildItr != m_childNameMap.end() )
	{
		return m_childObjects[ findChildItr->second ];
	}

	return AbcA::ObjectReaderPtr();
}

AbcA::ObjectReaderPtr OrImpl::getChild( size_t i )
{
	initializeMaps();

	return m_childObjects[ i ];
}

//-*****************************************************************************
AbcA::ObjectReaderPtr OrImpl::asObjectPtr()
{
    return shared_from_this();
}

//-*****************************************************************************
bool OrImpl::getPropertiesHash( Util::Digest & oDigest )
{
    return m_originalObjectReader->getPropertiesHash( oDigest );
}

//-*****************************************************************************
bool OrImpl::getChildrenHash( Util::Digest & oDigest )
{
    return m_originalObjectReader->getChildrenHash( oDigest );
}

//-*****************************************************************************
Alembic::Util::shared_ptr< ArImpl > OrImpl::getArchiveImpl() const
{
    return m_archive;
}

//-*****************************************************************************
void OrImpl::initializeMaps( )
{
	if( !m_mapsInitialized )
	{
		m_mapsInitialized = true;

		recordChildren();

		m_compoundPropertyReader =
		Alembic::Util::shared_ptr< CprImpl >( new CprImpl( shared_from_this(), m_originalObjectReader->getProperties() ) );
	}
}

//-*****************************************************************************
void OrImpl::recordChildren(  )
{
	size_t numChildren = m_originalObjectReader->getNumChildren();

	for( int i = 0; i< numChildren; i++ )
	{
       AbcA::ObjectReaderPtr origChildPtr = m_originalObjectReader->getChild( i );

       OrImplPtr childPtr = OrImplPtr( new OrImpl( this->m_archive,
                                       origChildPtr,
                                       shared_from_this() ) );

       m_childNameMap[ childPtr->m_header.getName() ] = m_childObjects.size();

       m_childObjects.push_back( childPtr );
	}
}

//-*****************************************************************************
void OrImpl::layerInObjectHierarchy( AbcA::ObjectReaderPtr iObject )
{
	initializeMaps();

	if( m_header.getName() == iObject->getHeader().getName() )
	{
		m_compoundPropertyReader->layerInProperties( iObject->getProperties() );

		size_t numChildren = iObject->getNumChildren();

		for( size_t i = 0; i < numChildren; i++ )
		{
			AbcA::ObjectReaderPtr child = iObject->getChild( i );

			const std::string &childName = child->getHeader().getName();

			ChildNameMap::iterator findChild = m_childNameMap.find( childName );

			OrImplPtr childLayeredObjReader;

			if( findChild == m_childNameMap.end() )
			{
				childLayeredObjReader =
						OrImplPtr( new OrImpl(this->m_archive, child, shared_from_this()) );

				m_childNameMap[ childName ] = m_childObjects.size();

				m_childObjects.push_back( childLayeredObjReader );
			}
			else
			{
				childLayeredObjReader = m_childObjects[ findChild->second ];
			}

			childLayeredObjReader->layerInObjectHierarchy( child );
		}
	}
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreLayer
} // End namespace Alembic

