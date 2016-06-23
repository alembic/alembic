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
}

//-*****************************************************************************
Alembic::AbcCoreAbstract::ArchiveReaderPtr
ReadArchive::operator()( ArchiveReaderPtrs & iArchives ) const
{
    AbcA::ArchiveReaderPtr archivePtr = Alembic::Util::shared_ptr<ArImpl>(
        new ArImpl( iArchives ) );

    return archivePtr;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreLayer
} // End namespace Alembic
