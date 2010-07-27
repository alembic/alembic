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

#ifndef _Alembic_Asset_Base_Slab_h_
#define _Alembic_Asset_Base_Slab_h_

#include <Alembic/Asset/Base/FoundationBase.h>
#include <Alembic/Asset/Base/DataType.h>
#include <Alembic/Asset/Base/Assert.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
void AccumulateMD5( MD5Hash::MD5 &md5,
                    const DataType &dtype,
                    const Dimensions &dims,
                    const void *data );


//-*****************************************************************************
inline void AccumulateMD5( MD5Hash::MD5 &md5,
                           const DataType &dtype,
                           size_t numElements,
                           const void *data )
{
    Dimensions dims( 1 );
    dims[0] = numElements;
    AccumulateMD5( md5, dtype, dims, data );
}

//-*****************************************************************************
// What is a Slab? It is a raw data buffer, somewhat equivalent to
// the streambuf in <iostream>. It can be resized, read from, written to.
// It is a low-level tool, and thus deals with untyped, raw data.
//
// Resizing will leave data in an undefined state.
//
// For now, just using std::vector. Later days may require something more
// robust, but this is fine for now.
//-*****************************************************************************
class Slab
{
public:
    //-*************************************************************************
    // TYPEDEFS
    //-*************************************************************************
    typedef MD5Hash::MD5Digest HashID;

    //-*************************************************************************
    // CONSTANTS
    //-*************************************************************************
    // We're limiting the individual slabs to have a maximum size of
    // two gigabytes, which is within the 32-bit int range.
    static const size_t MAX_NUM_BYTES = ( size_t )( 1U << 30 );

    //-*************************************************************************
    // PUBLIC API
    //-*************************************************************************

    // Reconfiguration
    void reset( const DataType &dt, const Dimensions &dims );
    void reset( const DataType &dt, size_t oneDim );

    // Constructors.
    Slab() throw() : m_numElements( 0 ) {}
    Slab( const DataType &dt, const Dimensions &dims )
    { reset( dt, dims ); }
    Slab( const DataType &dt, size_t oneDim )
    { reset( dt, oneDim ); }

    // config access
    const DataType &dataType() const throw() { return m_dataType; }
    const Dimensions &dimensions() const throw() { return m_dimensions; }
    size_t rank() const { return m_dimensions.size(); }

    // More reconfiguration
    void setDataType( const DataType &dtype )
    { reset( dtype, m_dimensions ); }
    void setDimensions( const Dimensions &dims )
    { reset( m_dataType, dims ); }
    void setRank0Size( size_t num )
    { reset( m_dataType, num ); }

    // Raw data access
    void *rawData() { return ( void * )( &(m_rawData[0]) ); }
    const void *rawData() const
    { return ( const void * )( &(m_rawData[0]) ); }

    // Total num bytes
    size_t bytes() const { return m_rawData.size(); }

    // Total num elements
    size_t numElements() const { return m_numElements; }
    size_t size() const { return m_numElements; }

    //-*************************************************************************
    // HASH ID CALCULATION
    //-*************************************************************************
    // Accumulate data into the hashing function given.
    void accumulateMD5( MD5Hash::MD5 &md5 ) const
    {
        AccumulateMD5( md5, m_dataType, m_dimensions,
                       ( const void * )&m_rawData.front() );
    }

    // Return Hash ID
    HashID calculateHashID() const
    {
        MD5Hash::MD5 md5;
        accumulateMD5( md5 );
        return md5.digest();
    }

protected:
    DataType m_dataType;
    Dimensions m_dimensions;
    Bytes m_rawData;
    size_t m_numElements;
};

//-*****************************************************************************
inline Slab::HashID CalculateHashID( const DataType &dtype,
                                     const Dimensions &dims,
                                     const void *data )
{
    MD5Hash::MD5 md5;
    AccumulateMD5( md5, dtype, dims, data );
    return md5.digest();
}

//-*****************************************************************************
inline Slab::HashID CalculateHashID( const DataType &dtype,
                                     size_t numElements,
                                     const void *data )
{
    MD5Hash::MD5 md5;
    AccumulateMD5( md5, dtype, numElements, data );
    return md5.digest();
}

//-*****************************************************************************
typedef boost::shared_ptr<Slab> SharedSlab;

inline SharedSlab MakeSharedSlab( void )
{
    return boost::make_shared<Slab>();
}

inline SharedSlab MakeSharedSlab( const DataType &dt,
                                  const Dimensions &dims )
{
    return boost::make_shared<Slab>( dt, dims );
}

inline SharedSlab MakeSharedSlab( const DataType &dt,
                                  size_t rank0dim )
{
    return boost::make_shared<Slab>( dt, rank0dim );
}

} // End namespace Asset
} // End namespace Alembic

//-*****************************************************************************
//-*****************************************************************************
// CHECK MAX SIZE
//-*****************************************************************************
//-*****************************************************************************
// Just ultra-check everything. 1073741824 is 2^30
BOOST_STATIC_ASSERT( Alembic::Asset::Slab::MAX_NUM_BYTES == 1073741824 );

#endif
