//-*****************************************************************************
//
// Copyright (c) 2016,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#include <Alembic/AbcCoreLayer/ArImpl.h>
#include <Alembic/AbcCoreFactory/IFactory.h>
#include <Alembic/AbcCoreLayer/OrImpl.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
ArImpl::ArImpl( ArchiveReaderPtrs & iArchives )
{
    m_archiveVersion = -1;
    m_header.reset( new AbcA::ObjectHeader() );
    m_archives.reserve( iArchives.size() );
    ArchiveReaderPtrs::iterator it = iArchives.begin();

    for ( ; it != iArchives.end(); ++it )
    {
        // bad archive ptr?  skip to the next one
        if ( !( *it ) )
        {
            continue;
        }

        m_archives.push_back( *it );

        if ( !m_fileName.empty() )
        {
            m_fileName += ",";
        }
        m_fileName += (*it)->getName();

        // go over this archives time samplings and add them to our list
        Util::uint32_t numSamplings = (*it)->getNumTimeSamplings();
        for ( Util::uint32_t i = 0; i < numSamplings; ++i )
        {
            Util::uint32_t j = 0;
            for ( j = 0; j < m_timeSamples.size(); ++j )
            {
                if ( m_timeSamples[j] == (*it)->getTimeSampling( i ) )
                {
                    break;
                }
            }

            // it wasn't found, add it and the max samples
            if ( j == m_timeSamples.size() )
            {
                m_timeSamples.push_back( (*it)->getTimeSampling( i ) );
                m_maxSamples.push_back(
                    (*it)->getMaxNumSamplesForTimeSamplingIndex( i ) );
            }
            else
            {
                m_maxSamples[j] = std::max( m_maxSamples[j],
                    (*it)->getMaxNumSamplesForTimeSamplingIndex( i ) );
            }
        }

        // the data stored in the top level meta data is special
        // lets combine them all together
        const AbcA::MetaData & md = (*it)->getMetaData();
        AbcA::MetaData::const_iterator mit;
        for ( mit = md.begin(); mit != md.end(); ++mit )
        {
            std::string val = m_header->getMetaData().get( mit->first );
            if ( !val.empty() )
            {
                val += " , ";
            }
            val += mit->second;
            m_header->getMetaData().set( mit->first, val );
        }

        m_archiveVersion = std::max( m_archiveVersion,
                                     (*it)->getArchiveVersion() );
    }
}

//-*****************************************************************************
ArImpl::~ArImpl()
{

}

//-*****************************************************************************
const std::string &ArImpl::getName() const
{
    return m_fileName;
}

//-*****************************************************************************
const AbcA::MetaData & ArImpl::getMetaData() const
{
    return m_header->getMetaData();
}

//-*****************************************************************************
AbcA::ObjectReaderPtr ArImpl::getTop()
{

    std::vector< AbcA::ObjectReaderPtr > tops;
    tops.reserve( m_archives.size() );
    ArchiveReaderPtrs::iterator arItr = m_archives.begin();
    for ( ; arItr != m_archives.end(); ++arItr )
    {
        tops.push_back( (*arItr)->getTop() );
    }

    return OrImplPtr( new OrImpl( shared_from_this(), tops, m_header ) );
}

//-*****************************************************************************
AbcA::TimeSamplingPtr ArImpl::getTimeSampling( Util::uint32_t iIndex )
{
    if( iIndex < m_timeSamples.size() )
    {
        return m_timeSamples[ iIndex ];
    }

    return AbcA::TimeSamplingPtr();
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
    if( iIndex < m_maxSamples.size() )
    {
        return m_maxSamples[iIndex];
    }

    return 0;
}

//-*****************************************************************************
AbcA::ReadArraySampleCachePtr ArImpl::getReadArraySampleCachePtr()
{
    return AbcA::ReadArraySampleCachePtr();
}

//-*****************************************************************************
void ArImpl::setReadArraySampleCachePtr( AbcA::ReadArraySampleCachePtr iPtr )
{
    // don't even bother
}

//-*****************************************************************************
Util::uint32_t ArImpl::getNumTimeSamplings()
{
    return m_timeSamples.size();
}

//-*****************************************************************************
Util::int32_t ArImpl::getArchiveVersion()
{
    return m_archiveVersion;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
