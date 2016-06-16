#ifndef _Alembic_AbcCoreLayer_Foundation_h_
#define _Alembic_AbcCoreLayer_Foundation_h_

#include <Alembic/Util/Export.h>
#include <Alembic/AbcCoreAbstract/ForwardDeclarations.h>
#include <Alembic/AbcCoreLayer/Read.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

namespace AbcA = ::Alembic::AbcCoreAbstract;

typedef std::map< std::string, size_t > ChildNameMap;

class OrImpl;
typedef Alembic::Util::shared_ptr< OrImpl > OrImplPtr;

class CprImpl;
typedef Alembic::Util::shared_ptr< CprImpl > CprImplPtr;

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_Foundation_h_
