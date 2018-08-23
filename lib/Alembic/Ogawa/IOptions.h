#ifndef _Alembic_Ogawa_Options_h_
#define _Alembic_Ogawa_Options_h_

#include <Alembic/Util/Export.h>
#include <Alembic/Util/Foundation.h>

namespace Alembic {
namespace Ogawa {
namespace ALEMBIC_VERSION_NS {

struct ALEMBIC_EXPORT IStreamOptions : public Alembic::Util::option_base
{
    enum FileAccessType
    {
        kFileStreams,
        kMemoryMapFiles
    };

    virtual FileAccessType getFileAccessStrategy() { return kMemoryMapFiles; }
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Ogawa

} // End namespace Alembic

#endif //_Alembic_Ogawa_Options_h_
