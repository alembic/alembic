//-*****************************************************************************
//
// Copyright (c) 2013,
//  Sony Pictures Imageworks Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic, nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#include <Alembic/AbcCoreOgawa/AwImpl.h>
#include <Alembic/AbcCoreOgawa/OwData.h>
#include <Alembic/AbcCoreOgawa/OwImpl.h>
#include <Alembic/AbcCoreOgawa/WriteUtil.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
AwImpl::AwImpl( const std::string &iFileName,
                const AbcA::MetaData &iMetaData )
  : m_fileName( iFileName )
  , m_metaData( iMetaData )
  , m_archive( iFileName )
  , m_metaDataMap( new MetaDataMap() )
{

    // add default time sampling
    AbcA::TimeSamplingPtr ts( new AbcA::TimeSampling() );
    m_timeSamples.push_back(ts);
    m_maxSamples.push_back(0);

    if ( !m_archive.isValid() )
    {
        ABCA_THROW( "Could not open file: " << m_fileName );
    }

    init();
}

//-*****************************************************************************
AwImpl::AwImpl( std::ostream * iStream,
                const AbcA::MetaData &iMetaData )
  : m_metaData( iMetaData )
  , m_archive( iStream )
  , m_metaDataMap( new MetaDataMap() )
{
    // add default time sampling
    AbcA::TimeSamplingPtr ts( new AbcA::TimeSampling() );
    m_timeSamples.push_back(ts);
    m_maxSamples.push_back(0);

    if ( !m_archive.isValid() )
    {
        ABCA_THROW( "Could not use the given ostream." );
    }

    init();
}

//-*****************************************************************************
void AwImpl::init()
{
    // set the version using Ogawa native calls
    // This expresses the AbcCoreOgawa version - how properties,
    // are stored within Ogawa, etc.
    Util::int32_t version = ALEMBIC_OGAWA_FILE_VERSION;
    m_archive.getGroup()->addData( 4, &version );

    // This is the Alembic library version XXYYZZ
    // Where XX is the major version, YY is the minor version
    // and ZZ is the patch version
    Util::int32_t libraryVersion = ALEMBIC_LIBRARY_VERSION;
    m_archive.getGroup()->addData( 4, &libraryVersion );

    m_metaData.set("_ai_AlembicVersion", AbcA::GetLibraryVersion());

    m_data.reset( new OwData( m_archive.getGroup()->addGroup() ) );

    // seed with the common empty keys
    AbcA::ArraySampleKey emptyKey;
    emptyKey.numBytes = 0;
    Ogawa::ODataPtr emptyData( new Ogawa::OData() );

    emptyKey.origPOD = Alembic::Util::kInt8POD;
    emptyKey.readPOD = Alembic::Util::kInt8POD;
    WrittenSampleIDPtr wsid( new WrittenSampleID( emptyKey, emptyData, 0 ) );
    m_writtenSampleMap.store( wsid );

    emptyKey.origPOD = Alembic::Util::kStringPOD;
    emptyKey.readPOD = Alembic::Util::kStringPOD;
    wsid.reset( new WrittenSampleID( emptyKey, emptyData, 0 ) );
    m_writtenSampleMap.store( wsid );

    emptyKey.origPOD = Alembic::Util::kWstringPOD;
    emptyKey.readPOD = Alembic::Util::kWstringPOD;
    wsid.reset( new WrittenSampleID( emptyKey, emptyData, 0 ) );
    m_writtenSampleMap.store( wsid );
}

//-*****************************************************************************
const std::string &AwImpl::getName() const
{
    return m_fileName;
}

//-*****************************************************************************
const AbcA::MetaData &AwImpl::getMetaData() const
{
    return m_metaData;
}

//-*****************************************************************************
AbcA::ArchiveWriterPtr AwImpl::asArchivePtr()
{
    return shared_from_this();
}

//-*****************************************************************************
AbcA::ObjectWriterPtr AwImpl::getTop()
{
    AbcA::ObjectWriterPtr ret = m_top.lock();
    if ( ! ret )
    {
        // time to make a new one
        ret = Alembic::Util::shared_ptr<OwImpl>(
            new OwImpl( asArchivePtr(), m_data, m_metaData ) );
        m_top = ret;
    }

    return ret;
}

//-*****************************************************************************
Util::uint32_t AwImpl::addTimeSampling( const AbcA::TimeSampling & iTs )
{
    index_t numTS = m_timeSamples.size();
    for (index_t i = 0; i < numTS; ++i)
    {
        if (iTs == *(m_timeSamples[i]))
            return i;
    }

    // we've got a new TimeSampling, write it and add it to our vector
    AbcA::TimeSamplingPtr ts( new AbcA::TimeSampling(iTs) );
    m_timeSamples.push_back(ts);
    m_maxSamples.push_back(0);

    index_t latestSample = m_timeSamples.size() - 1;

    std::stringstream strm;
    strm << latestSample;
    std::string name = strm.str();

    return latestSample;
}

//-*****************************************************************************
AbcA::TimeSamplingPtr AwImpl::getTimeSampling( Util::uint32_t iIndex )
{
    ABCA_ASSERT( iIndex < m_timeSamples.size(),
        "Invalid index provided to getTimeSampling." );

    return m_timeSamples[iIndex];
}

//-*****************************************************************************
AbcA::index_t
AwImpl::getMaxNumSamplesForTimeSamplingIndex( Util::uint32_t iIndex )
{
    if ( iIndex < m_maxSamples.size() )
    {
        return m_maxSamples[iIndex];
    }
    return INDEX_UNKNOWN;
}

//-*****************************************************************************
void AwImpl::setMaxNumSamplesForTimeSamplingIndex( Util::uint32_t iIndex,
                                                   AbcA::index_t iMaxIndex )
{
    if ( iIndex < m_maxSamples.size() )
    {
        m_maxSamples[iIndex] = iMaxIndex;
    }
}

//-*****************************************************************************
AwImpl::~AwImpl()
{

    // empty out the map so any dataset IDs will be freed up
    m_writtenSampleMap.clear();

    // write out our child headers
    if ( m_data )
    {
        Util::SpookyHash hash;
        m_data->writeHeaders( m_metaDataMap, hash );
    }

    // let go of our reference to the data for the top object
    m_data.reset();

    // encode and write the time samplings and max samples into data
    if ( m_archive.isValid() )
    {
        // encode and write the Metadata for the archive, since the top level
        // meta data can be kinda big and is very specialized don't worry
        // about putting it into the meta data map
        std::string metaData = m_metaData.serialize();
        m_archive.getGroup()->addData( metaData.size(), metaData.c_str() );

        std::vector< Util::uint8_t > data;
        Util::uint32_t numSamplings = getNumTimeSamplings();
        for ( Util::uint32_t i = 0; i < numSamplings; ++i )
        {
            Util::uint32_t maxSample = m_maxSamples[i];
            AbcA::TimeSamplingPtr timePtr = getTimeSampling( i );
            WriteTimeSampling( data, maxSample, *timePtr );
        }

        m_archive.getGroup()->addData( data.size(), &( data.front() ) );
        m_metaDataMap->write( m_archive.getGroup() );
    }

}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
