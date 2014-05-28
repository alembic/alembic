//-*****************************************************************************
//
// Copyright (c) 2009-2014,
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

#include <Alembic/AbcCoreAbstract/DataType.h>

#include <ImathMath.h>

#include <limits>

#include <algorithm>

namespace Alembic {
namespace AbcCoreAbstract {
namespace ALEMBIC_VERSION_NS {

static const DataType kChrono_TDataType( kChrono_TPOD, 1 );

//! Work around the imprecision of comparing floating values.
static const chrono_t kCHRONO_EPSILON = 1e-5;

//-*****************************************************************************
TimeSampling::TimeSampling( const TimeSamplingType &iTimeSamplingType,
                            const std::vector< chrono_t > & iSampleTimes )
  : m_timeSamplingType( iTimeSamplingType )
  , m_sampleTimes( iSampleTimes )
{
    init();
}

TimeSampling::TimeSampling( chrono_t iTimePerCycle,
                            chrono_t iStartTime )
  : m_timeSamplingType( iTimePerCycle )
{
    m_sampleTimes.resize(1);
    m_sampleTimes[0] = iStartTime;
    init();
}

void TimeSampling::init()
{
    size_t numSamples = m_sampleTimes.size();
    ABCA_ASSERT ( m_timeSamplingType.isAcyclic() || numSamples ==
        m_timeSamplingType.getNumSamplesPerCycle(),
        "Incorrect number of time samples specified, expected " <<
        m_timeSamplingType.getNumSamplesPerCycle() << ", got: " <<
        numSamples );

    // we need to check to make sure we are strictly increasing
    if ( numSamples > 1 )
    {
        chrono_t curVal = m_sampleTimes[0];
        for ( size_t i = 1; i < numSamples; ++i )
        {
            chrono_t newVal = m_sampleTimes[i];
            ABCA_ASSERT( curVal < newVal, "Sample " << i << " value: " <<
                newVal << " is not greater than the previous sample: " <<
                curVal );
            curVal = newVal;
        }

        // make sure cyclic samples fall within the time per cycle
        if ( m_timeSamplingType.isCyclic() )
        {
            curVal = m_sampleTimes[m_sampleTimes.size() - 1] - m_sampleTimes[0];
            ABCA_ASSERT ( curVal <= m_timeSamplingType.getTimePerCycle(),
                "Cyclic samples provided are greater than the time per cycle."
                " Expected: " << m_timeSamplingType.getTimePerCycle() <<
                " Found: " << curVal );
        }
    }

}

//-*****************************************************************************
TimeSampling::TimeSampling()
  : m_timeSamplingType( TimeSamplingType() )
{
    m_sampleTimes.resize(1);
    m_sampleTimes[0] = 0.0;
}

//-*****************************************************************************
TimeSampling::TimeSampling( const TimeSampling & copy)
  : m_timeSamplingType( copy.m_timeSamplingType )
  , m_sampleTimes( copy.m_sampleTimes )
{
    // nothing else
}

//-*****************************************************************************
chrono_t TimeSampling::getSampleTime( index_t iIndex ) const
{
    if ( m_timeSamplingType.isUniform() )
    {
        return m_sampleTimes[0] +
            ( m_timeSamplingType.getTimePerCycle() * ( chrono_t )iIndex );
    }
    else if ( m_timeSamplingType.isAcyclic() )
    {
        ABCA_ASSERT( ( size_t )iIndex < m_sampleTimes.size(),
            "Out-of-range acyclic index: " << iIndex
            << ", range [0-" <<  m_sampleTimes.size()-1 << "]" );
        return m_sampleTimes[iIndex];
    }
    else
    {
        ABCA_ASSERT( m_timeSamplingType.isCyclic(), "should be cyclic" );

        index_t N = ( index_t ) m_timeSamplingType.getNumSamplesPerCycle();
        return m_sampleTimes[iIndex % N] +
            ( m_timeSamplingType.getTimePerCycle() * ( iIndex / N ) );
    }
}

//-*****************************************************************************
std::pair<index_t, chrono_t>
TimeSampling::getFloorIndex( chrono_t iTime, index_t iNumSamples ) const
{
    //! Return the index of the sampled time that is <= iTime

    // no samples? return an arbitrary default value
    if ( iNumSamples < 1 )
    {
        return std::pair<index_t, chrono_t>( 0, 0.0 );
    }

    const chrono_t minTime = this->getSampleTime( 0 );
    if ( iTime <= minTime )
    {
        return std::pair<index_t, chrono_t>( 0, minTime );
    }

    const chrono_t maxTime = this->getSampleTime( iNumSamples - 1 );
    if ( iTime >= maxTime )
    {
        return std::pair<index_t, chrono_t>( iNumSamples-1, maxTime );
    }

    if ( m_timeSamplingType.isAcyclic() )
    {

        index_t loIdx = 0;
        index_t hiIdx = m_sampleTimes.size() - 1;
        index_t idx = hiIdx / 2;

        while ( loIdx < idx && idx < hiIdx )
        {
            chrono_t thisTime = m_sampleTimes[idx];
            if ( iTime == thisTime )
            {
                return std::pair<index_t, chrono_t>( idx, thisTime );
            }
            else if ( iTime < thisTime )
            {
                hiIdx = idx;
            }
            // greater than
            else
            {
                loIdx = idx;
            }
            idx = ( hiIdx + loIdx ) / 2;
        }

        chrono_t hiTime = m_sampleTimes[hiIdx];

        if ( Imath::equalWithAbsError( iTime, hiTime, kCHRONO_EPSILON ) )
        {
            return std::pair<index_t, chrono_t>( hiIdx, hiTime );
        }
        return std::pair<index_t, chrono_t>( loIdx, m_sampleTimes[loIdx] );
    }
    else if ( m_timeSamplingType.isUniform() )
    {
        chrono_t cycleTime = m_timeSamplingType.getTimePerCycle();

        // Get sample index
        index_t sampIdx = ( index_t ) ( ( iTime - minTime ) / cycleTime );

        // Clamp it.
        if ( sampIdx >= iNumSamples )
        {
            sampIdx = iNumSamples - 1;
        }
        else if ( sampIdx < 0 )
        {
            sampIdx = 0;
        }

        // Get the sample time
        chrono_t sampTime = minTime + ( cycleTime * sampIdx );
        chrono_t hiTime = minTime + ( cycleTime * ( sampIdx + 1 ) );

        // If roundoff error puts the sampTime beyond our acceptable tolerance
        // choose the next lowest index.
        if ( sampTime > iTime && sampIdx > 0 &&
            !Imath::equalWithAbsError( iTime, sampTime, kCHRONO_EPSILON ) )
        {
            sampIdx --;
            sampTime = minTime + ( cycleTime * sampIdx );
        }
        // If roundoff error chooses an index that is too low, get the
        // next highest one
        else if ( sampIdx < iNumSamples - 1 &&
            Imath::equalWithAbsError( iTime, hiTime, kCHRONO_EPSILON ) )
        {
            sampIdx ++;
            sampTime = hiTime;
        }

        return std::pair<index_t, chrono_t>( sampIdx, sampTime );
    }
    else
    {
        ABCA_ASSERT( m_timeSamplingType.isCyclic(), "should be cyclic" );

        const size_t N = m_timeSamplingType.getNumSamplesPerCycle();
        const chrono_t period = m_timeSamplingType.getTimePerCycle();
        const chrono_t elapsedTime = iTime - minTime;

        double rawNumCycles = elapsedTime / period;
        double rawNumCyclesIntregal;
        double rawNumCyclesFractional = modf( rawNumCycles,
                                              &rawNumCyclesIntregal );

        // if the fractional part is close enough to 1, add another
        // whole cycle on
        if ( Imath::equalWithAbsError( rawNumCyclesFractional, 1.0,
                                       kCHRONO_EPSILON ) )
        {
            rawNumCyclesIntregal += 1;

            // resetting for clarity, even though this variable is never
            // used again
            rawNumCyclesFractional = 0.0;
        }

        const index_t numCycles = ( index_t )rawNumCyclesIntregal;
        const chrono_t cycleBlockTime = ( numCycles * period );
        const index_t cycleBlockIndex = N * numCycles;
        const chrono_t rem = iTime - cycleBlockTime;
        index_t sampIdx = 0;

        // in practice this doesn't need to be a binary search since N is
        // almost always a very small number
        while( sampIdx < ( index_t ) N && m_sampleTimes[sampIdx] < rem )
        {
            ++sampIdx;
        }

        if ( sampIdx == ( index_t ) N )
        {
            sampIdx--;
        }

        chrono_t sampTime = cycleBlockTime + m_sampleTimes[sampIdx];

        // sampIdx, and sampTime could correspond to the ceilTime because
        // we loop until greater than rem
        if ( !Imath::equalWithAbsError( iTime, sampTime, kCHRONO_EPSILON ) &&
             sampIdx > 0 && iTime < sampTime )
        {
            sampIdx--;
            sampTime = cycleBlockTime + m_sampleTimes[sampIdx];
        }

        return std::pair<index_t, chrono_t>( cycleBlockIndex + sampIdx,
                                             sampTime );
    }
}

//-*****************************************************************************
std::pair<index_t, chrono_t>
TimeSampling::getCeilIndex( chrono_t iTime, index_t iNumSamples ) const
{
    //! Return the index of the sampled time that is >= iTime

    const index_t maxIndex = iNumSamples - 1;

    // make sure we aren't at or less than the min time
    const chrono_t minTime = this->getSampleTime( 0 );
    if ( iTime <= minTime )
    {
        return std::pair<index_t, chrono_t>( 0, minTime );
    }

    // make sure we aren't at or greater than the max time
    const chrono_t maxTime = this->getSampleTime( maxIndex );
    if ( iTime >= maxTime )
    {
        return std::pair<index_t, chrono_t>( maxIndex, maxTime );
    }

    std::pair<index_t, chrono_t> floorPair = this->getFloorIndex( iTime,
        iNumSamples );

    // if we are at maxIndex or the time returned is close enough to iTime
    // then just return the floor
    if ( floorPair.first == maxIndex ||
         Imath::equalWithAbsError(iTime, floorPair.second, kCHRONO_EPSILON ) )
    {
        return floorPair;
    }

    std::pair<index_t, chrono_t> ceilPair( floorPair.first + 1,
        this->getSampleTime( floorPair.first + 1 ) );
    return ceilPair;

}

//-*****************************************************************************
std::pair<index_t, chrono_t>
TimeSampling::getNearIndex( chrono_t iTime, index_t iNumSamples ) const
{
    //! Return the index of the sampled time that is:
    //! (iTime - floorTime < ceilTime - iTime) ? getFloorIndex( iTime )
    //! : getCeilIndex( iTime );

    if ( iNumSamples < 1 )
    {
        return std::pair<index_t, chrono_t>( 0, 0.0 );
    }

    std::pair<index_t, chrono_t> floorPair =
        this->getFloorIndex( iTime, iNumSamples );

    if ( floorPair.first == iNumSamples - 1)
    {
        return floorPair;
    }

    std::pair<index_t, chrono_t> ceilPair( floorPair.first + 1,
        this->getSampleTime( floorPair.first + 1 ) );

    if ( fabs( iTime - floorPair.second ) <= fabs( ceilPair.second - iTime ) )
    {
        return floorPair;
    }

    return ceilPair;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreAbstract
} // End namespace Alembic
