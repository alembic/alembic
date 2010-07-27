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

class TimeSampling
{
public:
    TimeSampling( const TimeSamplingType &iTimeSamplingType,
                  size_t iNumSamples,
                  ReadOnlyTimes iSampleTimes );
    
    // Get the time of any sample.
    // This will return NON_TIME if the sampling is static
    // it is invalid to call this for out-of-range indices.
    chrono_t getSampleTime( size_t iIndex ) const;

    // Find the largest valid index that has a time less
    // than the given time. Invalid to call this with zero samples.
    std::pair<index_t, chrono_t> getFloorIndex( chrono_t iTime ) const;

    // Find the smallest valid index that has a time greater
    // than the given time. Invalid to call this with zero samples.
    std::pair<index_t, chrono_t> getCeilIndex( chrono_t iTime ) const;

    // Find the valid index with the closest time to the given
    // time. Invalid to call this with zero samples.
    std::pair<index_t, chrono_t> getNearIndex( chrono_t iTime ) const;

protected:
    TimeSamplingType m_timeSamplingType;
    size_t m_numSamples;
    ReadOnlyTimes m_sampleTimes;
};
