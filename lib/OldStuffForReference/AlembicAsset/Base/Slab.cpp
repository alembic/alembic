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

#include <AlembicAsset/Base/Slab.h>
#include <boost/cstdint.hpp>
//#include <boost/detail/endian.hpp>
//#include <boost/spirit/home/support/detail/integer/endian.hpp>
#include <algorithm>

namespace AlembicAsset {

//-*****************************************************************************
void Slab::reset( const DataType &dtype, const Dimensions &dims )
{
    m_dataType = dtype;
    m_dimensions = dims;

    const size_t dtypeBytes = m_dataType.bytes();
    const size_t rank = m_dimensions.size();
    size_t numBytes = 0;
    if ( rank < 1 )
    {
        m_numElements = 0;
        numBytes = 0;
    }
    else
    {
        m_numElements = 1;
        for ( size_t d = 0; d < rank; ++d )
        {
            m_numElements *= m_dimensions[d];
            numBytes = m_numElements * dtypeBytes;
            if ( numBytes > MAX_NUM_BYTES )
            {
                AAH5_THROW( "Slab reset to larger than: "
                            << MAX_NUM_BYTES << " maximum bytes" );
            }
        }
    }

    // Resize the actual storage.
    m_rawData.resize( numBytes );
}

//-*****************************************************************************
void Slab::reset( const DataType &dtype, size_t num )
{
    m_dataType = dtype;
    m_dimensions.resize( 1 );
    m_numElements = num;
    m_dimensions[0] = m_numElements;

    size_t numBytes = m_numElements * m_dataType.bytes();
    AAH5_ASSERT( numBytes > 0 || num == 0,
                 "Error in Slab::reset()" );
    
    if ( numBytes > MAX_NUM_BYTES )
    {
        AAH5_THROW( "Slab reset to larger than: "
                    << MAX_NUM_BYTES << " maximum bytes" );
    }
    
    m_rawData.resize( numBytes );
}

//-*****************************************************************************
// Use our lovely POD templates to simplify this slightly.
template <PlainOldDataType PODT>
struct podAccumulate
{
    typedef typename PODTraitsFromEnum<PODT>::value_type value_type;
    static inline void doIt( MD5Hash::MD5 &md5,
                             const uint8_t *bytes,
                             size_t numPods )
    {
        if ( numPods > 0 )
        {
            const value_type *values = ( const value_type * )bytes;
            md5.update<value_type>( values, numPods );
        }
    }
};

//-*****************************************************************************
// Okay, so - in order to maintain data integrity, we have to evaluate the
// data in a fixed byte-order. We're using little-endian as the Alembic
// basis.
// CJH: Add DataType and Dimensions to accumulation
void AccumulateMD5( MD5Hash::MD5 &md5,
                    const DataType &dtype,
                    const Dimensions &dims,
                    const void *data )
{
    size_t numPods = dtype.extent * dims.numPoints();
    if ( numPods <= 0 )
    {
        // If we have zero pieces of data, we need to distinguish
        // slabs of different datatypes.
        // We really should be doing this anyway.
        uint8_t fakeBytes[2];
        fakeBytes[0] = ( uint8_t )dtype.pod;
        fakeBytes[1] = ( uint8_t )dtype.extent;
        
        podAccumulate<kUint8POD>::doIt( md5,
                                        ( const uint8_t * )fakeBytes,
                                        2 );
        return;
    }
    size_t numBytes = numPods * dtype.bytes();
    
    const uint8_t *bytes = ( const uint8_t * )data;
    
    switch ( dtype.pod )
    {
    case kUint8POD:
        podAccumulate<kUint8POD>::doIt( md5, bytes, numPods ); break;
    case kInt8POD:
        podAccumulate<kInt8POD>::doIt( md5, bytes, numPods ); break;
        
    case kUint16POD:
        podAccumulate<kUint16POD>::doIt( md5, bytes, numPods ); break;
    case kInt16POD:
        podAccumulate<kInt16POD>::doIt( md5, bytes, numPods ); break;
        
    case kUint32POD:
        podAccumulate<kUint32POD>::doIt( md5, bytes, numPods ); break;
    case kInt32POD:
        podAccumulate<kInt32POD>::doIt( md5, bytes, numPods ); break;
        
    case kUint64POD:
        podAccumulate<kUint64POD>::doIt( md5, bytes, numPods ); break;
    case kInt64POD:
        podAccumulate<kInt64POD>::doIt( md5, bytes, numPods ); break;
        
    case kFloat16POD:
        podAccumulate<kFloat16POD>::doIt( md5, bytes, numPods ); break;
    case kFloat32POD:
        podAccumulate<kFloat32POD>::doIt( md5, bytes, numPods ); break;
    case kFloat64POD:
        podAccumulate<kFloat64POD>::doIt( md5, bytes, numPods ); break;
        
    default:
        // data is bad, but we can pretend it's uint8
        md5.update( bytes, numBytes ); break;
    }
}

} // End namespace AlembicAsset
