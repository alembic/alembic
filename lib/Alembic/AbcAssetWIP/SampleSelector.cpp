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

#include <Alembic/Asset/SampleSelector.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
//-*****************************************************************************
// INPUT SAMPLE SELECTOR
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
TimeSampleIndex ReadSampleSelector::index( const ReadTimeSampling &its ) const
{
    switch ( m_type )
    {
    case kAnimIndexSample:
        return m_index;
    case kAnimNearestSample:
        return its.secondsToNearestIndex( m_seconds );
    case kAnimFloorSample:
        return its.secondsToFloorIndex( m_seconds );
    case kAnimCeilSample:
        return its.secondsToCeilIndex( m_seconds );
    case kStaticSample:
    default:
        return -1;
    };
}

//-*****************************************************************************
seconds_t ReadSampleSelector::seconds( const ReadTimeSampling &its ) const
{
    switch ( m_type )
    {
    case kAnimIndexSample:
        return its.indexToSeconds( m_index );
        
    case kAnimNearestSample:
    case kAnimFloorSample:
    case kAnimCeilSample:
        return m_seconds;
        
    case kStaticSample:
    default:
        return Time::kNonTimeValue();
    }
}

//-*****************************************************************************
//-*****************************************************************************
// OUTPUT SAMPLE SELECTOR
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
TimeSampleIndex WriteSampleSelector::index( const WriteTimeSampling &ots ) const
{
    switch ( m_type )
    {
    case kAnimIndexSample:
        return m_index;
    case kAnimNearestSample:
        return ots.secondsToNearestIndex( m_seconds );
    case kAnimFloorSample:
        return ots.secondsToFloorIndex( m_seconds );
    case kAnimCeilSample:
        return ots.secondsToCeilIndex( m_seconds );
    case kStaticSample:
    default:
        return -1;
    };
}

//-*****************************************************************************
seconds_t WriteSampleSelector::seconds( const WRiteTimeSampling &ots ) const
{
    switch ( m_type )
    {
    case kAnimIndexSample:
        if ( ots.isVariable() )
        {
            ABCA_ASSERT( m_seconds != Time::kNonTimeValue(),
                         "Invalid WriteSampleSelector." );
            return m_seconds;
        }
        else
        {
            return ots.indexToSeconds( m_index );
        }
    case kAnimNearestSample:
        ABCA_ASSERT( !ots.isVariable(),
                     "Invalid WriteSampleSelector." );
        return ots.indexToSeconds(
            ots.secondsToNearestIndex( m_seconds ) );
    case kAnimFloorSample:
        ABCA_ASSERT( !ots.isVariable(),
                     "Invalid WriteSampleSelector." );
        return ots.indexToSeconds(
            ots.secondsToFloorIndex( m_seconds ) );
    case kAnimCeilSample:
        ABCA_ASSERT( !ots.isVariable(),
                     "Invalid WriteSampleSelector." );
        return ots.indexToSeconds(
            ots.secondsToCeilIndex( m_seconds ) );
    case kStaticSample:
    default:
        return Time::kNonTimeValue();
    }
}

} // End namespace Asset
} // End namespace Alembic

