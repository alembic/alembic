//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#include <AlembicAsset/Base/ISlabCache.h>

namespace AlembicAsset {

//-*****************************************************************************
//-*****************************************************************************
// Lock Guard Class
//-*****************************************************************************
//-*****************************************************************************
ISlabCache::LockGuard::LockGuard( ISlabCache &c, Record &r )
  : m_cache( c ),
    m_record( r )
{
    m_cache.increment( m_record );
}

//-*****************************************************************************
ISlabCache::LockGuard::~LockGuard()
{
    m_cache.decrement( m_record );
}

//-*****************************************************************************
//-*****************************************************************************
// CACHE CLASS
//-*****************************************************************************
//-*****************************************************************************
ISlabCache::ISlabCache()
{
    // Nothing
}

//-*****************************************************************************
ISlabCache::~ISlabCache()
{
    // Nothing
}

//-*****************************************************************************
ISlabCache::Handle ISlabCache::find( const Key &k )
{
    Map::iterator found = m_map.find( k );
    if ( found != m_map.end() )
    {
        Record &r = (*((*found).second));
        return Handle( new LockGuard( *this, r ) );
    }
    else
    {
        return Handle();
    }
}

//-*****************************************************************************
ISlabCache::Handle ISlabCache::store( const Key &k, SharedSlab ssb )
{
    AAH5_ASSERT( m_map.count( k ) == 0,
                 "Can't store a slab that's already stored." );
    SharedRecord sr( new Record );
    (*sr).key = k;
    (*sr).slab = ssb;
    (*sr).count = 1; // CJH - maybe 0?
    Map::value_type v( k, sr );
    m_map.insert( v );

    return Handle( new LockGuard( *this, (*sr) ) );
}

//-*****************************************************************************
void ISlabCache::increment( ISlabCache::Record &r )
{
    ++r.count;
}

//-*****************************************************************************
void ISlabCache::decrement( ISlabCache::Record &r )
{
    --r.count;
    if ( r.count <= 0 )
    {
        // CJH: Need something better.
        // m_map.erase( r.key );
    }
}

} // End namespace AlembicAsset

