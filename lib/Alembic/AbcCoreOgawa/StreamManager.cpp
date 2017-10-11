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

#include <Alembic/AbcCoreOgawa/StreamManager.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

// Lets define a compare exchange macro for use below

// C++11 std::atomics version
#if !defined( ALEMBIC_LIB_USES_TR1 ) && __cplusplus >= 201103L
#define COMPARE_EXCHANGE( V, COMP, EXCH ) V.compare_exchange_weak( COMP, EXCH, std::memory_order_seq_cst, std::memory_order_seq_cst )
// Windows
#elif defined( _MSC_VER )
#define COMPARE_EXCHANGE( V, COMP, EXCH ) (InterlockedCompareExchange64( &V, EXCH, COMP ) == COMP)

Alembic::Util::int64_t ffsll( Alembic::Util::int64_t iValue )
{
    if ( !iValue )
    {
        return 0;
    }

    for ( Alembic::Util::int64_t bit = 0; bit < 64; ++bit )
    {
        if ( iValue & ( Alembic::Util::int64_t( 1 ) << bit ) )
        {
            return bit + 1;
        }
    }

    return 0;
}


// gcc 4.8 and above not using C++11
#elif defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 8
#define COMPARE_EXCHANGE( V, COMP, EXCH ) __atomic_compare_exchange_n( &V, &COMP, EXCH, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST )
// gcc 4.1 and above not using C++11
#elif defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1
#define COMPARE_EXCHANGE( V, COMP, EXCH ) __sync_bool_compare_and_swap( &V, COMP, EXCH )
#else
#error Please contact alembic-discuss@googlegroups.com for support.
#endif

StreamManager::StreamManager( std::size_t iNumStreams )
{

    m_streams = 0;
    m_curStream = 0;
    m_numStreams = iNumStreams;

    // only do this if we have more than 1 stream
    // otherwise we can just return default
    if ( iNumStreams > 1 )
    {
        m_streamIDs.resize( m_numStreams );
        for ( std::size_t i = 0; i < m_numStreams; ++i )
        {
            m_streamIDs[i] = i;
            if ( m_numStreams < sizeof(m_streams) * 8 )
            {
                m_streams |= Alembic::Util::int64_t( 1 ) << i;
            }
        }
    }

    m_default = StreamIDPtr( new StreamID( NULL, 0 ) );
}

StreamManager::~StreamManager()
{
}

StreamIDPtr StreamManager::get()
{

    if ( m_numStreams < 2 )
    {
        return m_default;
    }

    // we've got too many streams so use the locking version
    if ( m_numStreams > sizeof(m_streams) * 8 )
    {
        Alembic::Util::scoped_lock l( m_lock );

        // we've used up more than we have, just return the default
        if ( m_curStream >= m_numStreams )
        {
            return m_default;
        }

        return StreamIDPtr( new StreamID( this,
            m_streamIDs[ m_curStream ++ ] ) );
    }

    // CAS (compare and swap) non locking version
    Alembic::Util::int64_t val = 0;
    Alembic::Util::int64_t oldVal = 0;
    Alembic::Util::int64_t newVal = 0;

    do
    {
        oldVal = m_streams;
        val = ffsll( oldVal );

        if ( val == 0 )
        {
            return m_default;
        }

        newVal = oldVal & ~( Alembic::Util::int64_t( 1 ) << (val - 1) );
    }
    while ( ! COMPARE_EXCHANGE( m_streams, oldVal, newVal ) );

    return StreamIDPtr( new StreamID( this, ( std::size_t ) val - 1 ) );
}

void StreamManager::put( std::size_t iStreamID )
{

    // we've got too many streams so use the locking version
    if ( m_numStreams > sizeof(m_streams) * 8 )
    {
        // shouldn't ever hit this case, it's why we have m_default
        assert( iStreamID < m_numStreams && m_curStream > 0 );

        Alembic::Util::scoped_lock l( m_lock );
        m_streamIDs[ --m_curStream ] = iStreamID;
        return;
    }

    // CAS (compare and swap) non locking version
    Alembic::Util::int64_t oldVal = 0;
    Alembic::Util::int64_t newVal = 0;

    do
    {
        oldVal = m_streams;
        newVal = oldVal | ( Alembic::Util::int64_t( 1 ) << iStreamID );
    }
    while ( ! COMPARE_EXCHANGE( m_streams, oldVal, newVal ) );
}

StreamID::StreamID( StreamManager * iManager, std::size_t iStreamID ) :
    m_manager( iManager ), m_streamID( iStreamID )
{
}

StreamID::~StreamID()
{
    // if our manager is valid, give back our ID
    if ( m_manager != NULL )
    {
        m_manager->put( m_streamID );
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Ogawa
} // End namespace Alembic
