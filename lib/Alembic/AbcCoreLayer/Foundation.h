#ifndef _Alembic_AbcCoreLayer_Foundation_h_
#define _Alembic_AbcCoreLayer_Foundation_h_

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/Util/All.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

namespace AbcA = ::Alembic::AbcCoreAbstract;

typedef std::map< std::string, size_t > ChildNameMap;

class ArImpl;
typedef Alembic::Util::shared_ptr< ArImpl > ArImplPtr;

class OrImpl;
typedef Alembic::Util::shared_ptr< OrImpl > OrImplPtr;

typedef Alembic::Util::shared_ptr<AbcA::ObjectHeader> ObjectHeaderPtr;

class CprImpl;
typedef Alembic::Util::shared_ptr< CprImpl > CprImplPtr;

typedef std::vector< Alembic::AbcCoreAbstract::ArchiveReaderPtr >
        ArchiveReaderPtrs;

typedef std::vector< Alembic::AbcCoreAbstract::CompoundPropertyReaderPtr >
        CompoundReaderPtrs;

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_Foundation_h_
