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
                std::size_t iNumStreams );
  : m_fileName( iFileName ), m_archive(iFileName, iNumStreams)
{
    ABCA_ASSERT( m_archive.valid(),
                 "Could not open as Ogawa file: " << m_fileName );

    ABCA_ASSERT( m_archive.frozen(),
        "Ogawa file not cleanly closed while being written: " << m_fileName );
    Ogawa::IGroupPtr group = m_archive.getGroup();

    int version = -INT_MAX;
    std::size_t numChildren = group->getNumChildren();
    if ( numChildren > 0 && group->isChildData( 0 ) )
    {
        Ogawa::IDataPtr data = group->getData( 0 );
        if ( data->getSize() == 4 )
        {
            data->read( 4, &version );
        }
    }

    ABCA_ASSERT( version >= 0 && version <= ALEMBIC_OGAWA_FILE_VERSION,
        "Unsupported file version detected: " << version );

    // if it isn't there, it's pre 1.0
    int fileVersion = 9999;
    if ( numChildren > 1 && group->isChildData( 1 ) )
    {
        Ogawa::IDataPtr data = group->getData( 1 );
        if ( data->getSize() == 4 )
        {
            data->read( 4, &fileVersion );
        }
    }

    m_archiveVersion = fileVersion;

    if ( numChildren > 1 && group->isChildGroup( numChildren - 2 ) )
    {
        m_data.reset( new OrData( group->getGroup( numChildren - 2 ),
                                  m_archiveVersion ) );
    }

    if ( numChildren > 0 && group->isChildData( numChildren - 1 ) )
    {
        ReadTimeSamplesAndMax( group->getData( numChildren - 1 ),
                               m_timeSamples, m_maxSamples );
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
    return m_data->getMetaData();
}

//-*****************************************************************************
AbcA::ObjectReaderPtr ArImpl::getTop()
{
    AbcA::ObjectReaderPtr ret = m_top.lock();
    if ( ! ret )
    {
        // time to make a new one
        ret.reset( new OrImpl( shared_from_this(), m_data ) );
        m_top = ret;
    }

    return ret;
}

//-*****************************************************************************
AbcA::TimeSamplingPtr ArImpl::getTimeSampling( uint32_t iIndex )
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
AbcA::index_t ArImpl::getMaxNumSamplesForTimeSamplingIndex( uint32_t iIndex )
{
    if ( iIndex < m_maxSamples.size() )
    {
        return m_maxSamples[iIndex];
    }

    return INDEX_UNKNOWN;
}

//-*****************************************************************************
ArImpl::~ArImpl()
{
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
