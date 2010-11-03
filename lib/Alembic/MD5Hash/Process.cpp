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

// This code implements RFC1321, with 8% of the code in this file
// covered under the MD5 RSA Data Security License using the following license:

//-*****************************************************************************
// MD5    - RSA Data Security, Inc., MD5 message-digest algorithm             //
//                                                                            //
// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All            //
// rights reserved.                                                           //
//                                                                            //
// License to copy and use this software is granted provided that it          //
// is identified as the "RSA Data Security, Inc. MD5 Message-Digest           //
// Algorithm" in all material mentioning or referencing this software         //
// or this function.                                                          //
//                                                                            //
// License is also granted to make and use derivative works provided          //
// that such works are identified as "derived from the RSA Data               //
// Security, Inc. MD5 Message-Digest Algorithm" in all material               //
// mentioning or referencing the derived work.                                //
//                                                                            //
// RSA Data Security, Inc. makes no representations concerning either         //
// the merchantability of this software or the suitability of this            //
// software for any particular purpose. It is provided "as is"                //
// without express or implied warranty of any kind.                           //
//                                                                            //
// These notices must be retained in any copies of any part of this           //
// documentation and/or software.                                             //
//-*****************************************************************************

#include <Alembic/MD5Hash/Process.h>
#include <Alembic/MD5Hash/FoundationPrivate.h>

namespace Alembic {
namespace MD5Hash {

//-*****************************************************************************
#define T_MASK ((UINT4)~0)

//-*****************************************************************************
Process::Process()
{
    m_count[0] = 0;
    m_count[1] = 0;
    
    m_abcd[0] = 0x67452301U;
    m_abcd[1] = /*0xefcdab89*/ T_MASK ^ 0x10325476U;
    m_abcd[2] = /*0x98badcfe*/ T_MASK ^ 0x67452301U;
    m_abcd[3] = 0x10325476U;

    std::fill( m_buf, m_buf+64, ( UCHAR )0 );
}

//-*****************************************************************************
Process::Process( const Process &copy )
{
    m_count[0] = copy.m_count[0];
    m_count[1] = copy.m_count[1];

    m_abcd[0] = copy.m_abcd[0];
    m_abcd[1] = copy.m_abcd[1];
    m_abcd[2] = copy.m_abcd[2];
    m_abcd[3] = copy.m_abcd[3];

    std::copy( copy.m_buf, copy.m_buf + 64, m_buf );
}


//-*****************************************************************************
Process& Process::operator=( const Process &copy )
{
    m_count[0] = copy.m_count[0];
    m_count[1] = copy.m_count[1];

    m_abcd[0] = copy.m_abcd[0];
    m_abcd[1] = copy.m_abcd[1];
    m_abcd[2] = copy.m_abcd[2];
    m_abcd[3] = copy.m_abcd[3];

    std::copy( copy.m_buf, copy.m_buf + 64, m_buf );

    return *this;
}

//-*****************************************************************************
void Process::append( const UCHAR *data, size_t nbytesUnsigned )
{
    const UCHAR *p = data;
    size_t nbytes = ( size_t )nbytesUnsigned;
    size_t left = nbytes;
    size_t offset = ( m_count[0] >> 3 ) & ( ( size_t )63 );
    size_t nbits = ( size_t )( nbytes << 3 );

    if ( nbytes <= 0 )
    {
	return;
    }

    // Update the message length.
    m_count[1] += nbytes >> 29;
    m_count[0] += nbits;
    if ( m_count[0] < nbits )
    {
        ++m_count[1];
    }

    // Process an initial partial block.
    if ( offset )
    {
	size_t copy = ( offset + nbytes > 64 ?
                        64 - offset :
                        nbytes );

	memcpy( m_buf + offset, p, copy );
	if ( offset + copy < 64 )
        {
	    return;
        }
	p += copy;
	left -= copy;
        process64bytes( m_buf );
    }

    // Process full blocks.
    for ( ; left >= 64; p += 64, left -= 64 )
    {
        process64bytes( p );
    }

    // Process a final partial block.
    if ( left )
    {
	memcpy( m_buf, p, left );
    }
}

//-*****************************************************************************
MD5Digest Process::digest() const
{
    Process tmp( *this );
    return tmp.unrecoverableBakeIntoDigest();
}

//-*****************************************************************************
#define T1 /* 0xd76aa478 */ (T_MASK ^ 0x28955b87U)
#define T2 /* 0xe8c7b756 */ (T_MASK ^ 0x173848a9U)
#define T3    0x242070dbU
#define T4 /* 0xc1bdceee */ (T_MASK ^ 0x3e423111U)
#define T5 /* 0xf57c0faf */ (T_MASK ^ 0x0a83f050U)
#define T6    0x4787c62aU
#define T7 /* 0xa8304613 */ (T_MASK ^ 0x57cfb9ecU)
#define T8 /* 0xfd469501 */ (T_MASK ^ 0x02b96afeU)
#define T9    0x698098d8U
#define T10 /* 0x8b44f7af */ (T_MASK ^ 0x74bb0850U)
#define T11 /* 0xffff5bb1 */ (T_MASK ^ 0x0000a44eU)
#define T12 /* 0x895cd7be */ (T_MASK ^ 0x76a32841U)
#define T13    0x6b901122U
#define T14 /* 0xfd987193 */ (T_MASK ^ 0x02678e6cU)
#define T15 /* 0xa679438e */ (T_MASK ^ 0x5986bc71U)
#define T16    0x49b40821U
#define T17 /* 0xf61e2562 */ (T_MASK ^ 0x09e1da9dU)
#define T18 /* 0xc040b340 */ (T_MASK ^ 0x3fbf4cbfU)
#define T19    0x265e5a51U
#define T20 /* 0xe9b6c7aa */ (T_MASK ^ 0x16493855U)
#define T21 /* 0xd62f105d */ (T_MASK ^ 0x29d0efa2U)
#define T22    0x02441453U
#define T23 /* 0xd8a1e681 */ (T_MASK ^ 0x275e197eU)
#define T24 /* 0xe7d3fbc8 */ (T_MASK ^ 0x182c0437U)
#define T25    0x21e1cde6U
#define T26 /* 0xc33707d6 */ (T_MASK ^ 0x3cc8f829U)
#define T27 /* 0xf4d50d87 */ (T_MASK ^ 0x0b2af278U)
#define T28    0x455a14edU
#define T29 /* 0xa9e3e905 */ (T_MASK ^ 0x561c16faU)
#define T30 /* 0xfcefa3f8 */ (T_MASK ^ 0x03105c07U)
#define T31    0x676f02d9U
#define T32 /* 0x8d2a4c8a */ (T_MASK ^ 0x72d5b375U)
#define T33 /* 0xfffa3942 */ (T_MASK ^ 0x0005c6bdU)
#define T34 /* 0x8771f681 */ (T_MASK ^ 0x788e097eU)
#define T35    0x6d9d6122U
#define T36 /* 0xfde5380c */ (T_MASK ^ 0x021ac7f3U)
#define T37 /* 0xa4beea44 */ (T_MASK ^ 0x5b4115bbU)
#define T38    0x4bdecfa9U
#define T39 /* 0xf6bb4b60 */ (T_MASK ^ 0x0944b49fU)
#define T40 /* 0xbebfbc70 */ (T_MASK ^ 0x4140438fU)
#define T41    0x289b7ec6U
#define T42 /* 0xeaa127fa */ (T_MASK ^ 0x155ed805U)
#define T43 /* 0xd4ef3085 */ (T_MASK ^ 0x2b10cf7aU)
#define T44    0x04881d05U
#define T45 /* 0xd9d4d039 */ (T_MASK ^ 0x262b2fc6U)
#define T46 /* 0xe6db99e5 */ (T_MASK ^ 0x1924661aU)
#define T47    0x1fa27cf8U
#define T48 /* 0xc4ac5665 */ (T_MASK ^ 0x3b53a99aU)
#define T49 /* 0xf4292244 */ (T_MASK ^ 0x0bd6ddbbU)
#define T50    0x432aff97U
#define T51 /* 0xab9423a7 */ (T_MASK ^ 0x546bdc58U)
#define T52 /* 0xfc93a039 */ (T_MASK ^ 0x036c5fc6U)
#define T53    0x655b59c3U
#define T54 /* 0x8f0ccc92 */ (T_MASK ^ 0x70f3336dU)
#define T55 /* 0xffeff47d */ (T_MASK ^ 0x00100b82U)
#define T56 /* 0x85845dd1 */ (T_MASK ^ 0x7a7ba22eU)
#define T57    0x6fa87e4fU
#define T58 /* 0xfe2ce6e0 */ (T_MASK ^ 0x01d3191fU)
#define T59 /* 0xa3014314 */ (T_MASK ^ 0x5cfebcebU)
#define T60    0x4e0811a1U
#define T61 /* 0xf7537e82 */ (T_MASK ^ 0x08ac817dU)
#define T62 /* 0xbd3af235 */ (T_MASK ^ 0x42c50dcaU)
#define T63    0x2ad7d2bbU
#define T64 /* 0xeb86d391 */ (T_MASK ^ 0x14792c6eU)

//-*****************************************************************************
void Process::process64bytesLE( const UCHAR *data )
{
    // On little-endian machines, we can process properly aligned
    // data without copying it.
    if ( !( ( data - ( const UCHAR * ) 0 ) & 3 ) )
    {
        // data are properly aligned
        process64bytesAligned( ( const UINT4 * )data );
    }
    else
    {
        // not aligned
        UINT4 xbuf[16];
        memcpy( ( void * )xbuf, ( const void * )data, 64 );
        process64bytesAligned( xbuf );
    }   
}

//-*****************************************************************************
void Process::process64bytesBE( const UCHAR *data )
{
    // Define storage only for big-endian CPUs.
    UINT4 xbuf[16];

    // On big-endian machines, we must arrange the bytes in the
    // right order.
    const UCHAR *xp = data;
    for ( int i = 0; i < 16; ++i, xp += 4 )
    {
        xbuf[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);
    }

    process64bytesAligned( xbuf );
}

//-*****************************************************************************
void Process::process64bytesAligned( const UINT4 *DATA )
{
    UINT4 work = 0;
    UINT4 a = m_abcd[0];
    UINT4 b = m_abcd[1];
    UINT4 c = m_abcd[2];
    UINT4 d = m_abcd[3];
    
#define ROTATE_LEFT( X, N ) (((X) << (N)) | ((X) >> (32U - (N))))

    //-*************************************************************************
    // First Pass
    // SET( ABCD k s Ti ) denotes the operation:
    // A = B + ((A + F(B,C,D) + DATA[K] + Ti ) <<< S )
#define F( X, Y, Z ) ( ( (X) & (Y) ) | ( ~(X) & (Z) ) )
    
#define SET( A, B, C, D, K, S, Ti )                     \
    work = (A) + F( (B), (C), (D) ) + DATA[K] + (Ti);   \
    (A) = ROTATE_LEFT( work, (S) ) + (B)
    
    // Chew it up!
    SET( a, b, c, d,  0,  7,  T1 );
    SET( d, a, b, c,  1, 12,  T2 );
    SET( c, d, a, b,  2, 17,  T3 );
    SET( b, c, d, a,  3, 22,  T4 );
    SET( a, b, c, d,  4,  7,  T5 );
    SET( d, a, b, c,  5, 12,  T6 );
    SET( c, d, a, b,  6, 17,  T7 );
    SET( b, c, d, a,  7, 22,  T8 );
    SET( a, b, c, d,  8,  7,  T9 );
    SET( d, a, b, c,  9, 12, T10 );
    SET( c, d, a, b, 10, 17, T11 );
    SET( b, c, d, a, 11, 22, T12 );
    SET( a, b, c, d, 12,  7, T13 );
    SET( d, a, b, c, 13, 12, T14 );
    SET( c, d, a, b, 14, 17, T15 );
    SET( b, c, d, a, 15, 22, T16 );
#undef SET

    //-*************************************************************************
    // Second Pass
    // SET( ABCD k s Ti ) denotes the operation
    // A = B + ((A + G(B,C,D) + DATA[k] + Ti ) <<< S )
#define G( X, Y, Z ) ( ( (X) & (Z) ) | ( (Y) & ~(Z) ) )
    
#define SET( A, B, C, D, K, S, Ti )                     \
    work = (A) + G( (B), (C), (D) ) + DATA[K] + (Ti);   \
    (A) = ROTATE_LEFT( work, (S) ) + (B)
    
    // Chew it up!
    SET( a, b, c, d,  1,  5, T17 );
    SET( d, a, b, c,  6,  9, T18 );
    SET( c, d, a, b, 11, 14, T19 );
    SET( b, c, d, a,  0, 20, T20 );
    SET( a, b, c, d,  5,  5, T21 );
    SET( d, a, b, c, 10,  9, T22 );
    SET( c, d, a, b, 15, 14, T23 );
    SET( b, c, d, a,  4, 20, T24 );
    SET( a, b, c, d,  9,  5, T25 );
    SET( d, a, b, c, 14,  9, T26 );
    SET( c, d, a, b,  3, 14, T27 );
    SET( b, c, d, a,  8, 20, T28 );
    SET( a, b, c, d, 13,  5, T29 );
    SET( d, a, b, c,  2,  9, T30 );
    SET( c, d, a, b,  7, 14, T31 );
    SET( b, c, d, a, 12, 20, T32 );
#undef SET

    //-*************************************************************************
    // Third Pass
    // SET( ABCD K S Ti ) denotes the operation
    // A = B + ((A + H(B,C,D) + DATA[K] + Ti ) <<< S)
#define H( X, Y, Z ) ( (X) ^ (Y) ^ (Z) )
    
#define SET( A, B, C, D, K, S, Ti )                     \
    work = (A) + H( (B), (C), (D) ) + DATA[K] + (Ti);   \
    (A) = ROTATE_LEFT( work, (S) ) + (B)
        
    // Chew it up!
    SET( a, b, c, d,  5,  4, T33 );
    SET( d, a, b, c,  8, 11, T34 );
    SET( c, d, a, b, 11, 16, T35 );
    SET( b, c, d, a, 14, 23, T36 );
    SET( a, b, c, d,  1,  4, T37 );
    SET( d, a, b, c,  4, 11, T38 );
    SET( c, d, a, b,  7, 16, T39 );
    SET( b, c, d, a, 10, 23, T40 );
    SET( a, b, c, d, 13,  4, T41 );
    SET( d, a, b, c,  0, 11, T42 );
    SET( c, d, a, b,  3, 16, T43 );
    SET( b, c, d, a,  6, 23, T44 );
    SET( a, b, c, d,  9,  4, T45 );
    SET( d, a, b, c, 12, 11, T46 );
    SET( c, d, a, b, 15, 16, T47 );
    SET( b, c, d, a,  2, 23, T48 );
#undef SET

    //-*************************************************************************
    // Fourth Pass
    // SET( ABCD K S Ti ) denotes the operation
    // A = B + ((A + I(B,C,D) + DATA[K] + Ti ) <<< S)
#define I( X, Y, Z ) ( (Y) ^ ( (X) | ~(Z) ) )
    
#define SET( A, B, C, D, K, S, Ti )                     \
    work = (A) + I( (B), (C), (D) ) + DATA[K] + (Ti);   \
    (A) = ROTATE_LEFT( work, (S) ) + (B)
    
    // Chew it up!
    SET( a, b, c, d,  0,  6, T49 );
    SET( d, a, b, c,  7, 10, T50 );
    SET( c, d, a, b, 14, 15, T51 );
    SET( b, c, d, a,  5, 21, T52 );
    SET( a, b, c, d, 12,  6, T53 );
    SET( d, a, b, c,  3, 10, T54 );
    SET( c, d, a, b, 10, 15, T55 );
    SET( b, c, d, a,  1, 21, T56 );
    SET( a, b, c, d,  8,  6, T57 );
    SET( d, a, b, c, 15, 10, T58 );
    SET( c, d, a, b,  6, 15, T59 );
    SET( b, c, d, a, 13, 21, T60 );
    SET( a, b, c, d,  4,  6, T61 );
    SET( d, a, b, c, 11, 10, T62 );
    SET( c, d, a, b,  2, 15, T63 );
    SET( b, c, d, a,  9, 21, T64 );
#undef SET

#undef ROTATE_LEFT

    // Increment the accumulation buffer registers
    // by the values they had at the beginning of this block process
    m_abcd[0] += a;
    m_abcd[1] += b;
    m_abcd[2] += c;
    m_abcd[3] += d;
}

//-*****************************************************************************
MD5Digest Process::unrecoverableBakeIntoDigest()
{
    static const UCHAR pad[64] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    UCHAR data[8];
    
    // Save the length before padding.
    for ( int i = 0; i < 8; ++i )
    {
	data[i] = ( UCHAR )( m_count[i >> 2] >> ( ( i & 3 ) << 3 ) );
    }
    
    // Pad to 56 bytes mod 64.
    append( pad, ( ( 55 - ( m_count[0] >> 3 ) ) & 63 ) + 1 );
    
    // Append the length.
    append( data, 8 );

    // Assign the digest.
    MD5Digest digest;
    for ( int i = 0; i < 16; ++i )
    {
	digest[i] = ( UCHAR )( m_abcd[i >> 2] >> ( ( i & 3 ) << 3 ) );
    }

    return digest;
}

} // End namespace MD5Hash
} // End namespace Alembic

