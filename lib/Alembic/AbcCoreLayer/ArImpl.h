#ifndef _Alembic_AbcCoreLayer_ArImpl_h_
#define _Alembic_AbcCoreLayer_ArImpl_h_

#include <Alembic/AbcCoreLayer/Foundation.h>

#include <Alembic/Abc/IArchive.h>

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

    ArImpl( const std::list<std::string>& iFileNames,
            size_t iNumStreams=1 );

    ArImpl( const std::string& iFileName,
                size_t iNumStreams=1 );

    ArImpl( const std::list< std::vector< std::istream * > >& iStreams );

    ArImpl( const std::vector< std::istream * >& iStreams );

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
    void updateBaseArchiveReaderPtr();

    std::string m_fileName;
    std::list< Alembic::Abc::IArchive > m_archives;
    AbcA::ArchiveReaderPtr m_baseArchiveReader;

    Alembic::Util::weak_ptr< AbcA::ObjectReader > m_top;
    Alembic::Util::mutex m_orlock;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreLayer
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_ArImpl_h_
