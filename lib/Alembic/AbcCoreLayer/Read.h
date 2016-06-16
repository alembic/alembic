#ifndef _Alembic_AbcCoreLayer_Read_h_
#define _Alembic_AbcCoreLayer_Read_h_

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/Util/Export.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//! Will return a shared pointer to the archive reader
//! This version creates a cache associated with the archive.
class ALEMBIC_EXPORT ReadArchive
{
public:
    ReadArchive();

    // Open the file iNumStreams times and manage them internally
    ReadArchive( size_t iNumStreams );

    // Read from the provided streams, we do not own these, expect them
    // to remain open and all have the same data in them, and do not try to
    // delete them
    ReadArchive( const std::list< std::vector< std::istream * > >& iStreams );

    // open the file
    ::Alembic::AbcCoreAbstract::ArchiveReaderPtr
    operator()( const std::list< std::string > &iFileName ) const;

    // The given cache is ignored.
    ::Alembic::AbcCoreAbstract::ArchiveReaderPtr
    operator()( const std::list< std::string > &iFileName,
                ::Alembic::AbcCoreAbstract::ReadArraySampleCachePtr iCache
              ) const;

private:
    size_t m_numStreams;
    std::list< std::vector< std::istream * > > m_streams;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreLayer
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_Read_h_
