#include <Alembic/AbcCoreLayer/Read.h>
#include <Alembic/AbcCoreLayer/Foundation.h>
#include <Alembic/AbcCoreLayer/ArImpl.h>

//#include <Alembic/AbcCoreOgawa/ReadWrite.h>
//#include <Alembic/AbcCoreOgawa/Foundation.h>
//#include <Alembic/AbcCoreOgawa/AwImpl.h>
//#include <Alembic/AbcCoreOgawa/ArImpl.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
ReadArchive::ReadArchive()
{
    m_numStreams = 1;
}

//-*****************************************************************************
ReadArchive::ReadArchive( size_t iNumStreams )
{
    m_numStreams = iNumStreams;
}

//-*****************************************************************************
ReadArchive::ReadArchive( const std::list< std::vector< std::istream * > >& iStreams )
    : m_numStreams( 1 ), m_streams( iStreams )
{
}

//-*****************************************************************************
Alembic::AbcCoreAbstract::ArchiveReaderPtr
ReadArchive::operator()( const std::list< std::string > &iFileNames ) const
{
    AbcA::ArchiveReaderPtr archivePtr;

    if ( m_streams.empty() )
    {
        archivePtr = Alembic::Util::shared_ptr<ArImpl>(
            new ArImpl( iFileNames, m_numStreams ) );
    }
    else
    {
        archivePtr = Alembic::Util::shared_ptr<ArImpl>(
            new ArImpl( m_streams ) );
    }
    return archivePtr;
}

//-*****************************************************************************
// The given cache is ignored.
Alembic::AbcCoreAbstract::ArchiveReaderPtr
ReadArchive::operator()( const std::list< std::string > &iFileNames,
        ::Alembic::AbcCoreAbstract::ReadArraySampleCachePtr iCache ) const
{
    AbcA::ArchiveReaderPtr archivePtr;

    if ( m_streams.empty() )
    {
        archivePtr = Alembic::Util::shared_ptr<ArImpl> (
            new ArImpl( iFileNames, m_numStreams ) );
    }
    else
    {
        archivePtr = Alembic::Util::shared_ptr<ArImpl> (
            new ArImpl( m_streams ) );
    }
    return archivePtr;
}


} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreLayer
} // End namespace Alembic
