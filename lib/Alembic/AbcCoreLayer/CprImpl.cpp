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
CprImpl::CprImpl( OrImplPtr iObject,
                  AbcA::BasePropertyReaderPtr iOriginalReader,
                  CprImplPtr iParentCpr)
	: m_object( iObject )
	, m_originalPropertyReader( iOriginalReader )
	, m_parent( iParentCpr )
	, m_mapsInitialized( false )
{
    ABCA_ASSERT( m_object, "Invalid object in CprImpl(Object)" );
    ABCA_ASSERT( m_originalPropertyReader, "Invalid data in CprImpl(Object)" );
}

//-*****************************************************************************
CprImpl::~CprImpl()
{
    // Nothing
}

//-*****************************************************************************
void CprImpl::layerInProperties( AbcA::CompoundPropertyReaderPtr iProps )
{
	initializeMaps();

	size_t numProps = iProps->getNumProperties();

	for( size_t i = 0; i < numProps; i++ )
	{
		AbcA::BasePropertyReaderPtr propertyReader = iProps->getProperty( i );

		const std::string &name = propertyReader->getName();

		ChildNameMap::iterator matchingEntry = m_childNameMap.find( name );

		if( matchingEntry == m_childNameMap.end() )
		{
			addChildReader( propertyReader );
		}
		else
		{
			CprImplPtr ourCpr = m_childCprs[ matchingEntry->second ];
			if( ourCpr->m_originalPropertyReader->isCompound() && iProps->isCompound() )
			{
				ourCpr->layerInProperties( propertyReader->asCompoundPtr() );
			}
			else
			{
				m_childCprs[ matchingEntry->second ] = CprImplPtr( new CprImpl( m_object, propertyReader, shared_from_this()));
			}
		}
	}
}

//-*****************************************************************************
const AbcA::PropertyHeader &CprImpl::getHeader() const
{
    return m_originalPropertyReader->getHeader();
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
	initializeMaps();

    return m_childCprs.size();
}

//-*****************************************************************************
const AbcA::PropertyHeader & CprImpl::getPropertyHeader( size_t i )
{
	initializeMaps();

    return m_childCprs[i]->getHeader();
}

//-*****************************************************************************
const AbcA::PropertyHeader *
CprImpl::getPropertyHeader( const std::string &iName )
{
	initializeMaps();

	ChildNameMap::iterator itr = m_childNameMap.find( iName );

	if( itr !=  m_childNameMap.end() )
	{
		return &m_childCprs[ itr->second ]->getHeader();
	}

	return 0;
}

//-*****************************************************************************
AbcA::ScalarPropertyReaderPtr
CprImpl::getScalarProperty( const std::string &iName )
{
	initializeMaps();

	ChildNameMap::iterator itr = m_childNameMap.find( iName );

	ABCA_ASSERT( ( itr != m_childNameMap.end() ),
				"There is no child property with that name");

	if( itr != m_childNameMap.end() )
	{
		return m_childCprs[ itr->second ]->m_originalPropertyReader->asScalarPtr();
	}

	return AbcA::ScalarPropertyReaderPtr();
}

//-*****************************************************************************
AbcA::ArrayPropertyReaderPtr
CprImpl::getArrayProperty( const std::string &iName )
{
	initializeMaps();

	ChildNameMap::iterator itr = m_childNameMap.find( iName );

	ABCA_ASSERT( ( itr != m_childNameMap.end() ),
					"There is no child property with that name");

		if( itr != m_childNameMap.end() )
	{
		return m_childCprs[ itr->second ]->m_originalPropertyReader->asArrayPtr();
	}

	return AbcA::ArrayPropertyReaderPtr();
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr
CprImpl::getCompoundProperty( const std::string &iName )
{
    const size_t &childIndex = m_childNameMap[ iName ];

	ChildNameMap::iterator itr = m_childNameMap.find( iName );

	ABCA_ASSERT( ( itr != m_childNameMap.end() ),
					"There is no child property with that name");

	if( itr != m_childNameMap.end() )
	{
		return m_childCprs[ itr->second ];
	}

	return AbcA::CompoundPropertyReaderPtr();
}

//-*****************************************************************************
void CprImpl::initializeMaps( )
{
	if( !m_mapsInitialized )
	{
		m_mapsInitialized = true;

		recordProperties();
	}
}

//-*****************************************************************************
void CprImpl::recordProperties()
{
	if( m_originalPropertyReader->isCompound() )
	{
		AbcA::CompoundPropertyReaderPtr originalCpr = m_originalPropertyReader->asCompoundPtr();

		size_t numProperties = originalCpr->getNumProperties();

		for( size_t i = 0; i < numProperties; i++ )
		{
			AbcA::BasePropertyReaderPtr propertyReader = originalCpr->getProperty( i );

			addChildReader( propertyReader );
		}
	}
}

//-*****************************************************************************
void CprImpl::addChildReader( AbcA::BasePropertyReaderPtr propertyReader )
{
	const std::string &propName = propertyReader->getName();

	m_childNameMap[propName] = m_childCprs.size();

	CprImplPtr newChild;

	newChild = CprImplPtr( new CprImpl( m_object, propertyReader, shared_from_this()));

	m_childCprs.push_back( newChild );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreLayer
} // End namespace Alembic
