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
//CprImpl::CprImpl( AbcA::CompoundPropertyReaderPtr iParent,
//                  Ogawa::IGroupPtr iGroup,
//                  PropertyHeaderPtr iHeader,
//                  std::size_t iThreadId,
//                  const std::vector< AbcA::MetaData > & iIndexedMetaData )
//    : m_parent( iParent )
//    , m_header( iHeader )
//{
//    ABCA_ASSERT( m_parent, "Invalid parent in CprImpl(Compound)" );
//    ABCA_ASSERT( m_header, "invalid header in CprImpl(Compound)" );
//
//    AbcA::PropertyType pType = m_header->header.getPropertyType();
//    if ( pType != AbcA::kCompoundProperty )
//    {
//        ABCA_THROW( "Tried to create compound property with the wrong "
//                    "property type: " << pType );
//    }
//
//    // Set object.
//    AbcA::ObjectReaderPtr optr = m_parent->getObject();
//    ABCA_ASSERT( optr, "Invalid object in CprImpl::CprImpl(Compound)" );
//    m_object = optr;
//
//    m_data.reset( new CprData( iGroup, iThreadId, *( m_object->getArchive() ),
//                               iIndexedMetaData ) );
//}

//-*****************************************************************************
CprImpl::CprImpl( OrImplPtr iObject,
                  AbcA::CompoundPropertyReaderPtr iOriginalCpr,
                  CprImplPtr iParentCpr)
    : m_object( iObject )
    , m_originalCpr( iOriginalCpr )
	, m_parent( iParentCpr )
    , m_mapsInitialized( false )
{
    ABCA_ASSERT( m_object, "Invalid object in CprImpl(Object)" );
    ABCA_ASSERT( m_originalCpr, "Invalid data in CprImpl(Object)" );
}

//-*****************************************************************************
CprImpl::CprImpl( OrImplPtr iObject,
                  AbcA::BasePropertyReaderPtr iOriginalCpr,
                  CprImplPtr iParentCpr)
	: m_object( iObject )
	, m_originalCpr( iOriginalCpr )
	, m_parent( iParentCpr )
	, m_mapsInitialized( false )
{
    ABCA_ASSERT( m_object, "Invalid object in CprImpl(Object)" );
    ABCA_ASSERT( m_originalCpr, "Invalid data in CprImpl(Object)" );
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

		std::string name = propertyReader->getName();

		ChildNameMap::iterator matchingEntry = m_childNameMap.find( name );

		if( matchingEntry == m_childNameMap.end() )
		{
			addChildReader( propertyReader );
		}
		else
		{
			CprImplPtr ourCpr = m_childCprs[ matchingEntry->second ];
			if( ourCpr->m_originalCpr->isCompound() && iProps->isCompound() )
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
    return m_originalCpr->getHeader();
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
		return m_childCprs[ itr->second ]->m_originalCpr->asScalarPtr();
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
		return m_childCprs[ itr->second ]->m_originalCpr->asArrayPtr();
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
	if( m_originalCpr->isCompound() )
	{
		AbcA::CompoundPropertyReaderPtr originalCpr = m_originalCpr->asCompoundPtr();

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

	if( propertyReader->isCompound() )
	{
//		propertyReader
//			= CprImplPtr( new CprImpl( m_object, propertyReader->asCompoundPtr(), shared_from_this()));
		newChild = CprImplPtr( new CprImpl( m_object, propertyReader->asCompoundPtr(), shared_from_this()));

	}
	else
	{
		newChild = CprImplPtr( new CprImpl( m_object, propertyReader, shared_from_this()));
	}

	m_childCprs.push_back( newChild );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreLayer
} // End namespace Alembic
