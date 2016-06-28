#ifndef _Alembic_AbcCoreLayer_Read_h_
#define _Alembic_AbcCoreLayer_Read_h_

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreLayer/Foundation.h>
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

    // open the file
    Alembic::AbcCoreAbstract::ArchiveReaderPtr
    operator()(ArchiveReaderPtrs & ) const;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreLayer
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_Read_h_
