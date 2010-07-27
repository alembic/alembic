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

#include <Alembic/AbcCoreAbstract/TimeSampling.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
// CJH: Cover-all note here.
// No optimization of this class and no caching of intermediate structures
// until we know everything works.
//-*****************************************************************************

//-*****************************************************************************
TimeSampling::TimeSampling( const TimeSamplingType &iTimeSamplingType,
                            size_t iNumSamples,
                            ReadOnlyTimesPtr iSampleTimes )
  : m_timeSamplingType( iTimeSamplingType ),
    m_numSamples( iNumSamples ),
    m_sampleTimes( iSampleTimes )
{
    ABCA_ASSERT( m_sampleTimes, "Invalid sample times" );
    ABCA_ASSERT( m_numSamples > 0, "Zero time samples" );
    ABCA_ASSERT( m_sampleTimes->size() > 0, "Empty sample times" );
    ABCA_ASSERT( !m_timeSamplingType.isStatic(),
                 "Invalid to create a static TimeSampling" );

    if ( m_timeSamplingType.isUniform() )
    {
        ABCA_ASSERT( m_sampleTimes->size() >= 1,
                     "Uniform sampling requires one time sample value" );
    }
    else if ( m_timeSamplingType.isCyclic() )
    {
        ABCA_ASSERT( m_sampleTimes->size() >=
                     m_timeSamplingType.getNumSamplesPerCycle(),
                     "Cyclic sampling requires a fixed num time samples" );
    }
    else
    {
        ABCA_ASSERT( m_timeSamplingType.isAcyclic(),
                     "Corrupt time sampling type" );
        ABCA_ASSERT( m_sampleTimes->size() >= m_numSamples,
                     "Acyclic sampling requires full time samples" );
    }
}

//-*****************************************************************************
chrono_t TimeSampling::getSampleTime( index_t iIndex ) const
{
    ABCA_ASSERT( iIndex >= 0 && iIndex < m_numSamples,
                 "Out-of-range sample index: " << iIndex
                 << ", range [0-" << m_numSamples
                 << ")" );

    if ( m_timeSamplingType.isAcyclic() )
    {
        return (*m_sampleTimes)[iIndex];
    }
    else if ( m_timeSamplingType.isUniform() )
    {
        const chrono_t sample0Time = m_sampleTimes->front();
        return sample0Time +
            ( m_timeSamplingType.getTimePerCycle() * ( chrono_t )iIndex );
    }
    else
    {
        ABCA_ASSERT( m_timeSamplingType.isCyclic(), "should be cyclic" );

        // CJH: Yes, I know I could mod this. Being pedantic.
        const size_t N = m_timeSamplingType.getNumSamplesPerCycle();
        const size_t numCycles = iIndex / N;
        const size_t cycleBlock = numCycles * N;
        assert( cycleBlock <= iIndex );
        const size_t rem = iIndex - cycleBlock;
        assert( rem < N );

        const chrono_t sampleRemTime = (*m_sampleTimes)[rem];

        return sampleRemTime +
            ( m_timeSamplingType.getTimePerCycle() * ( chrono_t )numCycles );
    }
}

//-*****************************************************************************
std::pair<index_t, chrono_t>
TimeSampling::getFloorIndex( chrono_t iTime ) const
{
    const chrono_t minTime = m_sampleTimes->front();
    if ( iTime <= minTime )
    {
        return std::pair<index_t, chrono_t>( 0, minTime );
    }

    if ( m_timeSamplingType.isAcyclic() )
    {
        // This is the hardest one.
        // For now, just loop.
        //
        // For later, use binary search of sorted array.
        const chrono_t maxTime = (*m_sampleTimes)[m_numSamples-1];
        if ( iTime >= maxTime )
        {
            return std::pair<index_t, chrono_t>( m_numSamples-1,
                                                 maxTime );
        }

        assert( iTime >= minTime );
        chrono_t prevTime = minTime;
        for ( index_t idx = 1; idx < m_numSamples; ++idx )
        {
            chrono_t thisTime = (*m_sampleTimes)[idx];
            assert( iTime >= prevTime );
            if ( iTime < thisTime )
            {
                // Okay, tIdx is greater than us.
                // iIdx-1 is less than or equal to us.
                // Yes, inefficient here too, will optimize later.
                return std::pair<index_t, chrono_t>( idx-1,
                                                     prevTime );
            }

            prevTime = thisTime;
        }

        // Dang, we got all the way to the end, and no thing was ever
        // greater than us.
        // This is troublesome, because we should have picked it up
        // on maxTime.
        ABCA_THROW( "Corrupt acyclic time samples, iTime = "
                    << iTime << ", maxTime = " << maxTime );
        return std::pair<index_t, chrono_t>( m_numSamples-1, maxTime );
    }
    else if ( m_timeSamplingType.isUniform() )
    {
        // Get sample index
        size_t sampIdx = ( size_t )floor( ( iTime - minTime ) /
                                          m_timeSamplingType.getTimePerCycle() );

        // Clamp it.
        assert( sampIdx >= 0 );
        sampIdx = ( sampIdx >= m_numSamples ) ? m_numSamples-1 : sampIdx;

        // Get the samp time again.
        chrono_t sampTime = minTime +
            ( m_timeSamplingType.getTimePerCycle() * sampIdx );

        // Return the pair.
        return std::pair<index_t, chrono_t>( sampIdx, sampTime );
    }
    else
    {
        ABCA_ASSERT( m_timeSamplingType.isCyclic(), "should be cyclic" );

        // NOT IMPLEMENTED
        ABCA_THROW( "CYCLIC TIMESAMPLING NOT IMPLEMENTED" );
        return std::pair<index_t, chrono_t>( 0, 0.0 );
    }
}

//-*****************************************************************************
std::pair<index_t, chrono_t>
TimeSampling::getCeilIndex( chrono_t iTime ) const
{
    // NOT IMPLEMENTED
    ABCA_THROW( "TIMESAMPLING CEIL NOT IMPLEMENTED" );
    return std::pair<index_t, chrono_t>( 0, 0.0 );
}

//-*****************************************************************************
std::pair<index_t, chrono_t>
TimeSampling::getNearIndex( chrono_t iTime ) const
{
    // NOT IMPLEMENTED
    ABCA_THROW( "TIMESAMPLING CEIL NOT IMPLEMENTED" );
    return std::pair<index_t, chrono_t>( 0, 0.0 );
}

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic
