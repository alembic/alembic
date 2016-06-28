#ifndef _Alembic_AbcCoreLayer_ArImpl_h_
#define _Alembic_AbcCoreLayer_ArImpl_h_

#include <Alembic/AbcCoreLayer/Foundation.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//class OrData;

//-*****************************************************************************
class ArImpl
    : public AbcA::ArchiveReader
    , public Alembic::Util::enable_shared_from_this<ArImpl>
{
private:
    friend class ReadArchive;

    ArImpl( ArchiveReaderPtrs & iArchives );


public:

    virtual ~ArImpl();

    //-*************************************************************************
    // ABSTRACT FUNCTIONS
    //-*************************************************************************
    virtual const std::string &getName() const;

    virtual const AbcA::MetaData &getMetaData() const;

    virtual AbcA::ObjectReaderPtr getTop();

    virtual AbcA::TimeSamplingPtr getTimeSampling( Util::uint32_t iIndex );

    virtual AbcA::ArchiveReaderPtr asArchivePtr();

    virtual AbcA::ReadArraySampleCachePtr getReadArraySampleCachePtr();

    virtual void setReadArraySampleCachePtr( AbcA::ReadArraySampleCachePtr iPtr );

    virtual AbcA::index_t getMaxNumSamplesForTimeSamplingIndex(
        Util::uint32_t iIndex );

    virtual Util::uint32_t getNumTimeSamplings();

    virtual Util::int32_t getArchiveVersion();

private:
    std::string m_fileName;

    ArchiveReaderPtrs m_archives;

    std::vector <  AbcA::TimeSamplingPtr > m_timeSamples;
    std::vector <  AbcA::index_t > m_maxSamples;
    ObjectHeaderPtr m_header;

    Util::int32_t m_archiveVersion;

    // TODO, should we keep the top object (OrImplPtr) in a weak ptr
    // or can we just rebuild it everytime getTop is called?
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreLayer
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_ArImpl_h_
