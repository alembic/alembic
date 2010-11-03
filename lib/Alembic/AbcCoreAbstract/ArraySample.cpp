//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include <Alembic/AbcCoreAbstract/ArraySample.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
ArraySample::Key ArraySample::getKey() const
{
    MD5 md5;

    // Accumulate the POD + Extent.
    uint8_t fakeData[2];
    fakeData[0] = ( uint8_t )m_dataType.getPod();
    fakeData[1] = ( uint8_t )m_dataType.getExtent();
    md5.update( ( const uint8_t * )fakeData, 2 );

    // Depending on data type, loop over everything.
    size_t numPoints = m_dimensions.numPoints();
    size_t numPods = m_dataType.getExtent() * numPoints;
    size_t numBytes = m_dataType.getNumBytes() * numPoints;

    switch ( m_dataType.getPod() )
    {
    case kBooleanPOD:
    case kUint8POD:
        md5.update( ( const uint8_t * )m_data, numPods ); break;
    case kInt8POD:
        md5.update( ( const int8_t * )m_data, numPods ); break;

    case kUint16POD:
        md5.update( ( const uint16_t * )m_data, numPods ); break;
    case kInt16POD:
        md5.update( ( const int16_t * )m_data, numPods ); break;

    case kUint32POD:
        md5.update( ( const uint32_t * )m_data, numPods ); break;
    case kInt32POD:
        md5.update( ( const int32_t * )m_data, numPods ); break;

    case kUint64POD:
        md5.update( ( const uint64_t * )m_data, numPods ); break;
    case kInt64POD:
        md5.update( ( const int64_t * )m_data, numPods ); break;

    case kFloat16POD:
        md5.update( ( const float16_t * )m_data, numPods ); break;

    case kFloat32POD:
        md5.update( ( const float32_t * )m_data, numPods ); break;

    case kFloat64POD:
        md5.update( ( const float64_t * )m_data, numPods ); break;

    case kStringPOD:
        for ( size_t j = 0; j < numPoints; ++j )
        {
            const std::string &str =
                static_cast<const std::string*>( m_data )[j];
            md5.update( ( const int8_t * )str.c_str(), str.length() );

            // append a 0 for the NULL seperator character
            int8_t zero = 0;
            md5.update( &zero, 1 );
        }
        break;

    case kWstringPOD:
        for ( size_t j = 0; j < numPoints; ++j )
        {
            const std::wstring &wstr =
                static_cast<const std::wstring*>( m_data )[j];
            if ( sizeof( wchar_t ) == sizeof( int8_t ) )
            {
                md5.update( ( const int8_t * )wstr.c_str(), wstr.length() );
                static const int8_t zero8 = 0;
                md5.update( &zero8, 1 );
            }
            else if ( sizeof( wchar_t ) == sizeof( int16_t ) )
            {
                md5.update( ( const int16_t * )wstr.c_str(), wstr.length() );
                static const int16_t zero16 = 0;
                md5.update( &zero16, 1 );
            }
            else if ( sizeof( wchar_t ) == sizeof( int32_t ) )
            {
                md5.update( ( const int32_t * )wstr.c_str(), wstr.length() );
                static const int32_t zero32 = 0;
                md5.update( &zero32, 1 );
            }
        }
        break;

    default:
        ABCA_THROW( "Can't calculate key for: " << m_dataType ); break;

    }

    return md5.digest();
}

//-*****************************************************************************
ArraySamplePtr AllocateArraySample( const DataType &iDtype,
                                    const Dimensions &iDims )
{
    switch ( iDtype.getPod() )
    {
    case kBooleanPOD:
        return TAllocateArraySample<bool_t>( iDtype.getExtent(), iDims );

    case kUint8POD:
        return TAllocateArraySample<uint8_t>( iDtype.getExtent(), iDims );
    case kInt8POD:
        return TAllocateArraySample<int8_t>( iDtype.getExtent(), iDims );

    case kUint16POD:
        return TAllocateArraySample<uint16_t>( iDtype.getExtent(), iDims );
    case kInt16POD:
        return TAllocateArraySample<int16_t>( iDtype.getExtent(), iDims );

    case kUint32POD:
        return TAllocateArraySample<uint32_t>( iDtype.getExtent(), iDims );
    case kInt32POD:
        return TAllocateArraySample<int32_t>( iDtype.getExtent(), iDims );

    case kUint64POD:
        return TAllocateArraySample<uint64_t>( iDtype.getExtent(), iDims );
    case kInt64POD:
        return TAllocateArraySample<int64_t>( iDtype.getExtent(), iDims );

    case kFloat16POD:
        return TAllocateArraySample<float16_t>( iDtype.getExtent(), iDims );
    case kFloat32POD:
        return TAllocateArraySample<float32_t>( iDtype.getExtent(), iDims );
    case kFloat64POD:
        return TAllocateArraySample<float64_t>( iDtype.getExtent(), iDims );

    case kStringPOD:
        return TAllocateArraySample<string>( iDtype.getExtent(), iDims );
    case kWstringPOD:
        return TAllocateArraySample<wstring>( iDtype.getExtent(), iDims );

    default:
        return ArraySamplePtr();
    }
}

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic
