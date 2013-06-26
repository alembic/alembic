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

#include <Alembic/AbcCoreOgawa/MetaDataMap.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
Util::uint32_t MetaDataMap::getIndex( const std::string & iStr )
{
    if ( iStr.empty() )
    {
        return 0;
    }
    // we only want small meta data strings in our map since they are the
    // most likely to be repeated over and over
    else if ( iStr.size() < 256 )
    {
        std::map< std::string, Util::uint32_t >::iterator it =
            m_map.find( iStr );

        if ( it != m_map.end() )
        {
            return it->second + 1;
        }
        // 255 is reserved for meta data which we need to
        // explicitly write (and 0 means empty metadata)
        else if ( it == m_map.end() && m_map.size() < 254 )
        {
            Util::uint32_t index = m_map.size();
            m_map[iStr] = index;
            return index + 1;
        }
    }

    // too long, or no room left for this entry
    return 255;
}

//-*****************************************************************************
void MetaDataMap::write( Ogawa::OGroupPtr iParent )
{

    if ( m_map.empty() )
    {
        iParent->addEmptyData();
        return;
    }

    std::vector< std::string > mdVec;
    mdVec.resize( m_map.size() );

    // lets put each string into it's vector slot
    std::map< std::string, Util::uint32_t >::iterator it, itEnd;
    for ( it = m_map.begin(), itEnd = m_map.end(); it != itEnd; ++it )
    {
        mdVec[ it->second ] = it->first;
    }

    // now place it all into one continuous buffer
    std::vector< Util::uint8_t > buf;
    std::vector< std::string >::iterator jt, jtEnd;
    for ( jt = mdVec.begin(), jtEnd = mdVec.end(); jt != jtEnd; ++jt )
    {

        // all these strings are less than 256 chars so just push back size
        // as 1 byte
        buf.push_back( jt->size() );
        buf.insert( buf.end(), jt->begin(), jt->end() );
    }

    iParent->addData( buf.size(), ( const void * )&buf.front() );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
