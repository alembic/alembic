#include <Alembic/AbcCoreLayer/ArImpl.h>
#include <Alembic/AbcCoreFactory/IFactory.h>
#include <Alembic/AbcCoreLayer/OrImpl.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
ArImpl::ArImpl( const std::list<std::string>& iFileNames,
                std::size_t iNumStreams )
{
	Alembic::AbcCoreFactory::IFactory factory;
	std::list<std::string>::const_iterator itr = iFileNames.begin();

	while( itr != iFileNames.end() )
	{
		const std::string &filename = *itr;
		Alembic::Abc::IArchive archive = factory.getArchive( filename );

		ABCA_ASSERT( archive.valid(),
		                 "The specified archive is invalid" );

		if( archive.valid() )
		{
			m_archives.push_back( archive );

			if( itr != iFileNames.begin() )
			{
				m_fileName += ",";
			}

			m_fileName += filename;
		}

		itr++;
	}

	updateBaseArchiveReaderPtr();
}

//-*****************************************************************************
ArImpl::ArImpl( const std::string& iFileName,
                std::size_t iNumStreams )
{
	Alembic::AbcCoreFactory::IFactory factory;
	Alembic::Abc::IArchive archive = factory.getArchive( iFileName );

	ABCA_ASSERT( archive.valid(),
					 "The specified archive is invalid" );

	if( archive.valid() )
	{
		m_archives.push_back( archive );

		if( m_fileName.length() > 0)
		{
			m_fileName += ",";
		}

		m_fileName += iFileName;
	}

	updateBaseArchiveReaderPtr();
}

//-*****************************************************************************
ArImpl::ArImpl( const std::list< std::vector< std::istream * > >& iStreams )
{
	Alembic::AbcCoreFactory::IFactory factory;
	Alembic::AbcCoreFactory::IFactory::CoreType coreType;
	std::list< std::vector< std::istream * > >::const_iterator itr = iStreams.begin();

	while( itr != iStreams.end() )
	{
		const std::vector< std::istream * > &stream = *itr;
		Alembic::Abc::IArchive archive = factory.getArchive( stream, coreType );

		ABCA_ASSERT( archive.valid(),
						 "The specified archive stream is invalid" );

		if( archive.valid() )
		{
			m_archives.push_back( archive );
		}

		itr++;
	}

	updateBaseArchiveReaderPtr();
}

//-*****************************************************************************
ArImpl::ArImpl( const std::vector< std::istream * > & iStreams )
{
	Alembic::AbcCoreFactory::IFactory factory;
	Alembic::AbcCoreFactory::IFactory::CoreType coreType;
	Alembic::Abc::IArchive archive = factory.getArchive( iStreams, coreType );

	ABCA_ASSERT( archive.valid(),
					 "The specified archive stream is invalid" );

	if( archive.valid() )
	{
		m_archives.push_back( archive );
	}

	updateBaseArchiveReaderPtr();
}

//-*****************************************************************************
ArImpl::~ArImpl()
{

}

//-*****************************************************************************
void ArImpl::updateBaseArchiveReaderPtr()
{
	if( m_archives.size() )
	{
		m_baseArchiveReader = m_archives.begin()->getPtr();
	}
}

//-*****************************************************************************
const std::string &ArImpl::getName() const
{
    return m_fileName;
}

//-*****************************************************************************
const AbcA::MetaData &ArImpl::getMetaData() const
{
    if( m_archives.size() )
    {
    	return m_baseArchiveReader->getMetaData();
    }

    static AbcA::MetaData defaultMetaData;
    return defaultMetaData;
}

//-*****************************************************************************
AbcA::ObjectReaderPtr ArImpl::getTop()
{
	Alembic::Util::scoped_lock l( m_orlock );

	if( m_archives.size() )
	{
		AbcA::ObjectReaderPtr ret = m_top.lock();
		if ( ! ret )
		{
			std::list< Alembic::Abc::IArchive >::iterator archiveItr = m_archives.begin();

			//Initialize the base ObjectReader with the first archive in our list
			AbcA::ObjectReaderPtr top = archiveItr->getPtr()->getTop();

			Alembic::Util::shared_ptr<OrImpl> baseObjectReader = Alembic::Util::shared_ptr<OrImpl>(
					new OrImpl( shared_from_this() ,
                    top,
                    Alembic::Util::shared_ptr< OrImpl >() ) );

			m_top = ret = baseObjectReader;

			//Layer in the remaining archives
			archiveItr++;

			while( archiveItr != m_archives.end() )
			{
				top = archiveItr->getPtr()->getTop();

				baseObjectReader->layerInObjectHierarchy( top );

				archiveItr++;
			}
		}

		return ret;
	}
	else
	{
		return AbcA::ObjectReaderPtr();
	}
}

//-*****************************************************************************
AbcA::TimeSamplingPtr ArImpl::getTimeSampling( Util::uint32_t iIndex )
{
	if( m_archives.size() )
	{
		return m_baseArchiveReader->getTimeSampling( iIndex );
	}

	return AbcA::TimeSamplingPtr();
}

//-*****************************************************************************
AbcA::ArchiveReaderPtr ArImpl::asArchivePtr()
{
	if( m_archives.size() )
	{
		return m_baseArchiveReader->asArchivePtr();
	}

	return AbcA::ArchiveReaderPtr();
}

//-*****************************************************************************
AbcA::index_t
ArImpl::getMaxNumSamplesForTimeSamplingIndex( Util::uint32_t iIndex )
{
	if( m_archives.size() )
	{
		return m_archives.begin()->getMaxNumSamplesForTimeSamplingIndex( iIndex );
	}

	return 0;
}

//-*****************************************************************************
AbcA::ReadArraySampleCachePtr ArImpl::getReadArraySampleCachePtr()
{
	if( m_archives.size() )
	{
		return m_archives.begin()->getReadArraySampleCachePtr();
	}

	return AbcA::ReadArraySampleCachePtr();
}

//-*****************************************************************************
void ArImpl::setReadArraySampleCachePtr( AbcA::ReadArraySampleCachePtr iPtr )
{
	if( m_archives.size() )
	{
		return m_baseArchiveReader->setReadArraySampleCachePtr( iPtr );
	}
}

//-*****************************************************************************
Util::uint32_t ArImpl::getNumTimeSamplings()
{
	if( m_archives.size() )
	{
		return m_baseArchiveReader->getNumTimeSamplings();
	}

	return 0;
}

//-*****************************************************************************
Util::int32_t ArImpl::getArchiveVersion()
{
	if( m_archives.size() )
	{
		return m_baseArchiveReader->getArchiveVersion();
	}

	return -1;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
