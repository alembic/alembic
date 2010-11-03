//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include <Alembic/MD5Hash/MD5.h>
#include <Alembic/MD5Hash/FoundationPrivate.h>

namespace Alembic {
namespace MD5Hash {

//-*****************************************************************************
#ifdef MD5HASH_BIG_ENDIAN

//-*****************************************************************************
template <class POD_TYPE>
inline void StoreLittleEndian( void *destination,
                               const POD_TYPE &source )
{
    boost::detail::store_little_endian<POD_TYPE,sizeof(POD_TYPE)>(
        destination, source );
}

//-*****************************************************************************
template <class POD_TYPE>
static void pupdate( Process &iProcess, const POD_TYPE *data, size_t num )
{
    // We need to convert to little endian as we write.
    // Do this in a buffer.
    static UCHAR buf[ 1024 * sizeof( POD_TYPE ) ];
    
    for ( size_t i = 0; i < num; /* Nothing */ )
    {
        // Load up 1024 elements (at most) of them.
        size_t j;
        for ( j = 0; j < 1024 && i < num; ++j, ++i )
        {
            StoreLittleEndian<POD_TYPE>(
                // Destination
                ( void * )( buf + (j*sizeof(POD_TYPE)) ),
                // Source
                data[i] );
        }

        // j represents the number of elements that have been stored.
        // j * sizeof(T) is the number of bytes.
        iProcess.append( buf, j * sizeof( POD_TYPE ) );
    }
}

//-*****************************************************************************
// 8-bit is never needed to be swapped
void MD5::update( const uint8_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num ); }

//-*****************************************************************************
void MD5::update( const int8_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num ); }

//-*****************************************************************************
void MD5::update( const uint16_t *data, size_t num )
{ pupdate( m_process, data, num ); }

//-*****************************************************************************
void MD5::update( const int16_t *data, size_t num )
{ pupdate( m_process, data, num ); }

//-*****************************************************************************
void MD5::update( const uint32_t *data, size_t num )
{ pupdate( m_process, data, num ); }

//-*****************************************************************************
void MD5::update( const int32_t *data, size_t num )
{ pupdate( m_process, data, num ); }

//-*****************************************************************************
void MD5::update( const uint64_t *data, size_t num )
{ pupdate( m_process, data, num ); }

//-*****************************************************************************
void MD5::update( const int64_t *data, size_t num )
{ pupdate( m_process, data, num ); }

//-*****************************************************************************
// Float types converted by transforming to integer types.
// This may not always work.
void MD5::update( const float16_t *data, size_t num )
{
    MD5HASH_STATIC_ASSERT( sizeof( float16_t ) == sizeof( uint16_t ) );
    pupdate( m_process, ( const uint16_t * )data, num );
}

//-*****************************************************************************
void MD5::update( const float32_t *data, size_t num )
{
    MD5HASH_STATIC_ASSERT( sizeof( float32_t ) == sizeof( uint32_t ) );
    pupdate( m_process, ( const uint32_t * )data, num );
}

//-*****************************************************************************
void MD5::update( const float64_t *data, size_t num )
{
    MD5HASH_STATIC_ASSERT( sizeof( float64_t ) == sizeof( uint64_t ) );
    pupdate( m_process, ( const uint64_t * )data, num );
}

#else // MD5HASH_BIG_ENDIAN

//-*****************************************************************************
void MD5::update( const uint8_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num * sizeof( uint8_t ) ); }

//-*****************************************************************************
void MD5::update( const int8_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num * sizeof( int8_t ) ); }

//-*****************************************************************************
void MD5::update( const uint16_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num * sizeof( uint16_t ) ); }

//-*****************************************************************************
void MD5::update( const int16_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num * sizeof( int16_t ) ); }

//-*****************************************************************************
void MD5::update( const uint32_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num * sizeof( uint32_t ) ); }

//-*****************************************************************************
void MD5::update( const int32_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num * sizeof( int32_t ) ); }

//-*****************************************************************************
void MD5::update( const uint64_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num * sizeof( uint64_t ) ); }

//-*****************************************************************************
void MD5::update( const int64_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num * sizeof( int64_t ) ); }

//-*****************************************************************************
void MD5::update( const float16_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num * sizeof( float16_t ) ); }

//-*****************************************************************************
void MD5::update( const float32_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num * sizeof( float32_t ) ); }

//-*****************************************************************************
void MD5::update( const float64_t *data, size_t num )
{ m_process.append( ( const UCHAR * )data, num * sizeof( float64_t ) ); }

#endif // MD5HASH_BIG_ENDIAN

//-*****************************************************************************
MD5 &operator<<( MD5 &md5, const std::string &str )
{
    md5.update( ( const UCHAR * )( str.c_str() ), str.size() );
    return md5;
}

//-*****************************************************************************
MD5 &operator<<( MD5 &md5, std::istream &istr )
{
    UCHAR data[1024];
    size_t bytes;
    while ( istr )
    {
        istr.read( ( char * )data, 1024 );
        bytes = istr.gcount();
        md5.update( data, bytes );
    }

    return md5;
}

} // End namespace MD5Hash
} // End namespace Alembic



