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

#ifndef _Alembic_MD5Hash_MD5_h_
#define _Alembic_MD5Hash_MD5_h_

#include <Alembic/MD5Hash/Foundation.h>
#include <Alembic/MD5Hash/Process.h>

namespace Alembic {
namespace MD5Hash {

//-*****************************************************************************
class MD5
{
public:
    MD5() : m_process() {}
    MD5( const MD5 &copy )
      : m_process( copy.m_process ) {}
    MD5 &operator=( const MD5 &copy )
    {
        m_process = copy.m_process;
        return *this;
    }

    // This accumulates some number of bytes.
    template <class POD_TYPE>
    void update( const POD_TYPE *elements, size_t numElems );

    // From iterators.
    template <class ITERATOR>
    void updateIter( ITERATOR begin, ITERATOR end )
    {
        typedef typename ITERATOR::value_type value_type;
        size_t numElems = end - begin;
        this->update<value_type>( ( const value_type * )&(*begin),
                                  numElems );
    }
    
    // This bakes the current thing down into a digest.
    // It is doing final calculations that aren't completely cheap,
    // so cache the results!
    MD5Digest digest() const
    {
        return m_process.digest();
    }

    Process m_process;
};

//-*****************************************************************************
//-*****************************************************************************
// FUNCTIONS FOR MAKING MD5 HASHES FROM COMMON THINGS
//-*****************************************************************************
//-*****************************************************************************
MD5 &operator<<( MD5 &md5, const std::string &str );
MD5 &operator<<( MD5 &md5, std::istream &istr );

template <class ITERATOR>
inline MD5 &operator<<( MD5 &md5,
                        const std::pair<ITERATOR,ITERATOR> &range )
{
    md5.updateIter<ITERATOR>( range.first, range.second );
    return md5;
}

//-*****************************************************************************
#ifdef MD5HASH_BIG_ENDIAN
//#if 1

//-*****************************************************************************
template <class POD_TYPE>
void MD5::update( const POD_TYPE *data, size_t num )
{
    // We need to convert to little endian as we write.
    // Do this in a buffer.
    static UCHAR buf[1024*sizeof(POD_TYPE)];
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
        m_process.append( buf, j * sizeof( POD_TYPE ) );
    }
}

//-*****************************************************************************
// 8-bit is never needed to be swapped
template <>
inline void MD5::update<unsigned char>( const unsigned char *data,
                                        size_t num )
{
    m_process.append( data, num );
}

template <>
inline void MD5::update<char>( const char *data, size_t num )
{
    m_process.append( ( const UCHAR * )data, num );
}

//-*****************************************************************************
// Specializations required for float types.
template <>
inline void MD5::update<half>( const half *data,
                               size_t num )
{
    MD5HASH_STATIC_ASSERT( sizeof( half ) == sizeof( unsigned short ) );
    MD5::update<unsigned short>( ( const unsigned short * )data,
                                 num );
}

template <>
inline void MD5::update<float>( const float *data, size_t num )
{
    MD5HASH_STATIC_ASSERT( sizeof( float ) == sizeof( boost::uint32_t ) );
    MD5::update<boost::uint32_t>(
        ( const boost::uint32_t * )data, num );
}

template <>
inline void MD5::update<double>( const double *data, size_t num )
{
    MD5HASH_STATIC_ASSERT( sizeof( double ) == sizeof( boost::uint64_t ) );
    MD5::update<boost::uint64_t>(
        ( const boost::uint64_t * )data, num );
}

#else // MD5HASH_BIG_ENDIAN

template <class POD_TYPE>
inline void MD5::update( const POD_TYPE *data, size_t num )
{
    m_process.append( ( const UCHAR * )data,
                      num * sizeof( POD_TYPE ) );
}

#endif // MD5HASH_BIG_ENDIAN

} // End namespace MD5Hash
} // End namespace Alembic


#endif

