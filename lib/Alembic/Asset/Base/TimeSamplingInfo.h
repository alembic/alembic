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

#ifndef _Alembic_Asset_Base_TimeSamplingInfo_h_
#define _Alembic_Asset_Base_TimeSamplingInfo_h_

#include <Alembic/Asset/Base/FoundationBase.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
enum TimeSamplingType
{
    kUniformTimeSampling,
    kVariableTimeSampling,
    kUnknownTimeSamplingType
};

//-*****************************************************************************
//-*****************************************************************************
// TIME SAMPLING INFO
//-*****************************************************************************
//-*****************************************************************************
struct TimeSamplingInfo
{
    typedef TimeSamplingInfo this_type;
    
    // Default constructor creates Unknown time sampling
    TimeSamplingInfo() throw()
      : type( kUnknownTimeSamplingType ),
        firstSampleSeconds( 0.0 ),
        firstSampleInterval( 1.0 ),
        secondSampleInterval( 1.0 ),
        numSamplesHint( 0 ) {}

    // Explicit constructor.
    explicit TimeSamplingInfo( TimeSamplingType tst,
                               seconds_t fss = 0.0,
                               seconds_t fsi = 1.0,
                               seconds_t ssi = 1.0,
                               size_t nsh = 0 ) throw()
      : type( tst ),
        firstSampleSeconds( fss ),
        firstSampleInterval( fsi ),
        secondSampleInterval( ssi ),
        numSamplesHint( nsh ) {}

    // Copy, assign
    TimeSamplingInfo( const TimeSamplingInfo &copy ) throw()
      : type( copy.type ),
        firstSampleSeconds( copy.firstSampleSeconds ),
        firstSampleInterval( copy.firstSampleInterval ),
        secondSampleInterval( copy.secondSampleInterval ),
        numSamplesHint( copy.numSamplesHint ) {}

    TimeSamplingInfo &operator=( const TimeSamplingInfo &copy ) throw()
    {
        type = copy.type;
        firstSampleSeconds = copy.firstSampleSeconds;
        firstSampleInterval = copy.firstSampleInterval;
        secondSampleInterval = copy.secondSampleInterval;
        numSamplesHint = copy.numSamplesHint;
        return *this;
    }

    bool equivalent( const TimeSamplingInfo &other ) const throw()
    {
        if ( other.type != type ) { return false; }

        switch ( type )
        {
        case kUniformTimeSampling:
            // first sample seconds & increment must match
            return ( firstSampleSeconds == other.firstSampleSeconds &&
                     firstSampleInterval == other.firstSampleInterval );
        case kVariableTimeSampling:
            // Variable time sampling is always equivalent to itself.
            return true;
        default:
            // unknown.
            return false;
        };
    }

    // Only for Uniform, eventually Open-Close
    seconds_t sampleToSeconds( size_t s ) const throw()
    {
        switch ( type )
        {
        case kUniformTimeSampling:
            return firstSampleSeconds +
                firstSampleInterval * ( seconds_t )s;
        default:
            // This is a programmer error.
            return Time::kRestTimeValue();
        };
    }

    // Only for Uniform, eventually Open-Close
    size_t secondsToSample( seconds_t s ) const throw()
    {
        if ( type == kUniformTimeSampling && firstSampleInterval != 0.0 )
        {
            return ( size_t )
                floor( 0.5 + ( ( s - firstSampleSeconds ) /
                               firstSampleInterval ) );
        }

        // This is a programmer error.
        assert( 0 );
        return 0;
    }

    // Uniform, Variable, eventually Open-Close
    TimeSamplingType type;

    // Seconds of the first sample. For Uniform & Open-Close,
    // ignored for Variable.
    seconds_t firstSampleSeconds;

    // Size of the increment between sample 0 & sample 1.
    // Used by Uniform & Open-Close, ignored for Variable
    seconds_t firstSampleInterval;

    // Size of the increment between sample 1 & sample 2.
    // Used by Open-Close, ignored for Uniform & Variable.
    seconds_t secondSampleInterval;

    // Hint about the expected number of samples. May be left to 0,
    // is solely used as a hint.
    size_t numSamplesHint;

    ALEMBIC_OPERATOR_BOOL( ( type == kUniformTimeSampling ||
                                     type == kVariableTimeSampling ) );
};

//-*****************************************************************************
// overload == to mean "equivalent"
inline bool operator==( const TimeSamplingInfo &a, const TimeSamplingInfo &b )
    throw()
{
    return a.equivalent( b );
}

inline bool operator!=( const TimeSamplingInfo &a, const TimeSamplingInfo &b )
{
    return !a.equivalent( b );
}

} // End namespace Asset
} // End namespace Alembic

#endif
