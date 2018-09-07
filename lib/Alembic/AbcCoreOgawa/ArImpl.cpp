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

#include <Alembic/AbcCoreOgawa/ArImpl.h>
#include <Alembic/AbcCoreOgawa/OrData.h>
#include <Alembic/AbcCoreOgawa/OrImpl.h>
#include <Alembic/AbcCoreOgawa/ReadUtil.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
ArImpl::ArImpl( const std::string &iFileName,
                std::size_t iNumStreams,
                bool iUseMMap)
  : m_fileName( iFileName )
  , m_archive( iFileName, iNumStreams, iUseMMap )
  , m_header( new AbcA::ObjectHeader() )
  , m_manager( iNumStreams )
{
    ABCA_ASSERT( m_archive.isValid(),
                 "Could not open as Ogawa file: " << m_fileName );

    ABCA_ASSERT( m_archive.isFrozen(),
        "Ogawa file not cleanly closed while being written: " << m_fileName );

    init();
}

//-*****************************************************************************
ArImpl::ArImpl( const std::vector< std::istream * > & iStreams )
  : m_archive( iStreams )
  , m_header( new AbcA::ObjectHeader() )
  , m_manager( iStreams.size() )
{
    ABCA_ASSERT( m_archive.isValid(),
                 "Could not open as Ogawa file from provided streams." );

    ABCA_ASSERT( m_archive.isFrozen(),
        "Ogawa streams not cleanly closed while being written. " );

    init();
}

//-*****************************************************************************
void ArImpl::init()
{
    Ogawa::IGroupPtr group = m_archive.getGroup();

    int version = -1;
    std::size_t numChildren = group->getNumChildren();

    if ( numChildren > 5 && group->isChildData( 0 ) &&
         group->isChildData( 1 ) && group->isChildGroup( 2 ) &&
         group->isChildData( 3 ) && group->isChildData( 4 ) &&
         group->isChildData( 5 ) )
    {
        Ogawa::IDataPtr data = group->getData( 0, 0 );
        if ( data->getSize() == 4 )
        {
            data->read( 4, &version, 0, 0 );
        }
    }
    else
    {
        ABCA_THROW( "Invalid Alembic file." );
    }

    ABCA_ASSERT( version >= 0 && version <= ALEMBIC_OGAWA_FILE_VERSION,
        "Unsupported file version detected: " << version );

    // if it isn't there, something is wrong
    int fileVersion = 0;

    Ogawa::IDataPtr data = group->getData( 1, 0 );
    if ( data->getSize() == 4 )
    {
        data->read( 4, &fileVersion, 0, 0 );
    }

    ABCA_ASSERT( fileVersion >= 9999,
        "Unsupported Alembic version detected: " << fileVersion );

    m_archiveVersion = fileVersion;

    ReadTimeSamplesAndMax( group->getData( 4, 0 ),
                           m_timeSamples, m_maxSamples );

    ReadIndexedMetaData( group->getData( 5, 0 ), m_indexMetaData );

    m_data.reset( new OrData( group->getGroup( 2, false, 0 ), "", 0, *this,
                              m_indexMetaData ) );

    m_header->setName( "ABC" );
    m_header->setFullName( "/" );

    // read archive metadata
    data = group->getData( 3, 0 );
    if ( data->getSize() > 0 )
    {
        char * buf = new char[ data->getSize() ];
        data->read( data->getSize(), buf, 0, 0 );
        std::string metaData(buf, data->getSize() );
        m_header->getMetaData().deserialize( metaData );
        delete [] buf;
    }

}

//-*****************************************************************************
const std::string &ArImpl::getName() const
{
    return m_fileName;
}

//-*****************************************************************************
const AbcA::MetaData &ArImpl::getMetaData() const
{
    return m_header->getMetaData();
}

//-*****************************************************************************
AbcA::ObjectReaderPtr ArImpl::getTop()
{
    Alembic::Util::scoped_lock l( m_orlock );

    AbcA::ObjectReaderPtr ret = m_top.lock();
    if ( ! ret )
    {
        // time to make a new one
        ret = Alembic::Util::shared_ptr<OrImpl>(
            new OrImpl( shared_from_this(), m_data, m_header ) );
        m_top = ret;
    }

    return ret;
}

//-*****************************************************************************
AbcA::TimeSamplingPtr ArImpl::getTimeSampling( Util::uint32_t iIndex )
{
    ABCA_ASSERT( iIndex < m_timeSamples.size(),
        "Invalid index provided to getTimeSampling." );

    return m_timeSamples[iIndex];
}

//-*****************************************************************************
AbcA::ArchiveReaderPtr ArImpl::asArchivePtr()
{
    return shared_from_this();
}

//-*****************************************************************************
AbcA::index_t
ArImpl::getMaxNumSamplesForTimeSamplingIndex( Util::uint32_t iIndex )
{
    if ( iIndex < m_maxSamples.size() )
    {
        return m_maxSamples[iIndex];
    }

    return INDEX_UNKNOWN;
}

//-*****************************************************************************
StreamIDPtr ArImpl::getStreamID()
{
    return m_manager.get();
}

//-*****************************************************************************
ArImpl::~ArImpl()
{
}

//-*****************************************************************************
const std::vector< AbcA::MetaData > & ArImpl::getIndexedMetaData()
{
    return m_indexMetaData;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
