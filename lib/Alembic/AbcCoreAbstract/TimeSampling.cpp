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

#include <Alembic/AbcCoreAbstract/DataType.h>

#include <ImathMath.h>

#include <limits>

#include <algorithm>

namespace Alembic {
namespace AbcCoreAbstract {
namespace ALEMBIC_VERSION_NS {

static const DataType kChrono_TDataType( kChrono_TPOD, 1 );

//! Work around the imprecision of comparing floating values.
static const chrono_t kCHRONO_EPSILON = \
    std::numeric_limits<chrono_t>::epsilon() * 32.0;

static const chrono_t kCHRONO_TOLERANCE = kCHRONO_EPSILON * 32.0;

//-*****************************************************************************
// For properties that use acyclic time sampling, the array of
// times at which samples were taken will be stored as an
// ArraySample <chrono_t>, and will get the benefit provided by
// the default HDF5 data caching.
// After profiling real applications we may change Properties
// to store shared_ptrs to these and/or make use of singletons
// for constants or static time sampling. For now these objects
// are small and are created and passed by value as needed.
//-*****************************************************************************

//-*****************************************************************************
TimeSampling::TimeSampling( const TimeSamplingType &iTimeSamplingType,
                            size_t iNumSamples,
                            ArraySamplePtr iSampleTimes )
  : m_timeSamplingType( iTimeSamplingType )
  , m_numSamples( iNumSamples )
  , m_sampleTimes( iSampleTimes )
{
    //! The number of stored samples should never be greater than the number
    //! of samples per cycle.  Users of the library never manually construct
    //! a TimeSampling object, though, so this assert is just an internal
    //! sanity check; if the test fails, it just means that Alembic is storing
    //! more data than it needs to.
    //!
    //! Due to an implementation detail, the "size" of the iSampleTimes
    //! for Identity Time Sampling will be 1, and will contain '0' as the
    //! only value.
    assert( iSampleTimes->size() <= iTimeSamplingType.getNumSamplesPerCycle() ||
            iTimeSamplingType.isIdentity() && iSampleTimes->size() <= 1 );

    ABCA_ASSERT( sizeof(chrono_t) == iSampleTimes->getDataType().getNumBytes(),
                "Internal error sizeof( chrono_t ) mismatch to kChrono_TPOD");

    ABCA_ASSERT( m_sampleTimes ||
                 ( m_numSamples == 0 && m_timeSamplingType.isIdentity() ),
                 "Non-identity time sampling with more than zero samples "
                 << "requires a valid array of sample times." );

    ABCA_ASSERT( m_sampleTimes->getDimensions().rank() == 1
                 && m_sampleTimes->getDataType().getPod() == kChrono_TPOD,
                 "Invalid sampleTimes ArraySample, must be rank 1 and "
                 << "type chrono_t" );
}

//-*****************************************************************************
TimeSampling::TimeSampling()
  : m_timeSamplingType( TimeSamplingType() )
  , m_numSamples( 0 )
{
    Dimensions dims;
    dims.setRank( 1 );
    m_sampleTimes = ArraySamplePtr( new ArraySample( (const void *)NULL,
                                                     kChrono_TDataType,
                                                     dims ) );
}

//-*****************************************************************************
TimeSampling::TimeSampling( const TimeSampling & copy)
  : m_timeSamplingType( copy.m_timeSamplingType )
  , m_numSamples( copy.m_numSamples )
  , m_sampleTimes( copy.m_sampleTimes )
{
    // nothing else
}

//-*****************************************************************************
const chrono_t *TimeSampling::_getTimeSamplesAsChrono_tPtr() const
{
    const chrono_t *rawMemory;
    //! FYI This is reaching into the array_sample ptr and it is _NOT_
    //! incrementing the reference count of ArraySample pointer.
    //! This is ok though, because all the while that we ourselves are
    //! in memory the memory of m_sampleTimes will always be valid
    //! because we are holding m_sampleTimes (and that means ref count will
    //! be at least 1
    rawMemory = reinterpret_cast<const chrono_t*>( m_sampleTimes->getData() );
    return rawMemory;
}

//-*****************************************************************************
chrono_t TimeSampling::getSampleTime( index_t iIndex ) const
{
    ABCA_ASSERT( iIndex >= 0 && iIndex < m_numSamples,
                 "Out-of-range sample index: " << iIndex
                 << ", range [0-" << m_numSamples
                 << "]" );

    if ( m_timeSamplingType.isIdentity() )
    {
        return ( chrono_t )iIndex;
    }
    else if ( m_timeSamplingType.isAcyclic() )
    {
        return this->_getTimeSamplesAsChrono_tPtr()[iIndex];
    }
    else if ( m_timeSamplingType.isUniform() )
    {
        const chrono_t sample0Time = this->_getTimeSamplesAsChrono_tPtr()[0];
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

        const chrono_t sampleRemTime = this->_getTimeSamplesAsChrono_tPtr()[rem];

        return sampleRemTime +
            ( m_timeSamplingType.getTimePerCycle() * ( chrono_t )numCycles );
    }
}

//-*****************************************************************************
std::pair<index_t, chrono_t>
TimeSampling::getFloorIndex( chrono_t iTime ) const
{
    //! Return the index of the sampled time that is <= iTime

    if ( m_numSamples < 1 ) { return std::pair<index_t, chrono_t>( 0, 0.0 ); }

    if ( m_timeSamplingType.isIdentity() )
    {
        index_t idx = ( index_t )floor( iTime );
        idx = idx < 0 ? 0 : idx >= m_numSamples ? m_numSamples - 1 : idx;
        return std::pair<index_t, chrono_t>( idx, ( chrono_t )idx );
    }

    iTime += kCHRONO_EPSILON;

    const chrono_t minTime = this->getSampleTime( 0 );
    if ( iTime <= minTime )
    {
        return std::pair<index_t, chrono_t>( 0, minTime );
    }

    const chrono_t maxTime = this->getSampleTime( m_numSamples - 1 );
    if ( iTime >= maxTime )
    {
        return std::pair<index_t, chrono_t>( m_numSamples-1, maxTime );
    }

    if ( m_timeSamplingType.isAcyclic() )
    {
        // For now, just loop.
        //
        // For later, use binary search of sorted array.

        assert( iTime >= minTime );
        chrono_t prevTime = minTime;
        const chrono_t *sampleTimes;
        sampleTimes = this->_getTimeSamplesAsChrono_tPtr();
        for ( index_t idx = 1; idx < m_sampleTimes->size(); ++idx )
        {
            chrono_t thisTime = sampleTimes[idx];
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
        return std::pair<index_t, chrono_t>( m_sampleTimes->size()-1, maxTime );
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

        // Because of roundoff error, this sampTime could actually
        // be greater than the given time.
        if ( sampTime > iTime )
        {
            assert( sampIdx > 0 );
            sampIdx -= 1;
            sampTime -= ( m_timeSamplingType.getTimePerCycle() );
            assert( sampTime < iTime );
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

        if ( Imath::equalWithAbsError( 1.0 - rawNumCyclesFractional, 0.0,
                                       kCHRONO_TOLERANCE ) )
        {
            rawNumCyclesIntregal += 1;
        }

        const size_t numCycles = ( size_t )rawNumCyclesIntregal;
        const chrono_t cycleBlockTime = ( numCycles * period );


        assert( elapsedTime >= cycleBlockTime ||
                Imath::equalWithAbsError( cycleBlockTime - elapsedTime, 0.0,
                                          kCHRONO_TOLERANCE ) );

        const chrono_t rem = iTime - cycleBlockTime;

        assert( rem < period + minTime );
        const size_t cycleBlockIndex = N * numCycles;

        index_t sampIdx = 0;

        const chrono_t *sampleTimes = this->_getTimeSamplesAsChrono_tPtr();
        for ( index_t i = 0 ; i < N ; ++i )
        {
            chrono_t sampleTimesTime = sampleTimes[i];

            if ( sampleTimesTime > rem )
            {
                sampIdx = i - 1;
                break;
            }

            sampIdx = i;
        }

        if ( sampIdx < 0 ) { sampIdx = 0; }

        const chrono_t sampTime = cycleBlockTime +
            this->_getTimeSamplesAsChrono_tPtr()[sampIdx];

        return std::pair<index_t, chrono_t>( cycleBlockIndex + sampIdx,
                                             sampTime );
    }
}

//-*****************************************************************************
static std::pair<index_t, chrono_t>
getCeilIndexHelper( const TimeSampling *iThat, const chrono_t iTime,
                    const size_t iFloorIndex, const chrono_t iFloorTime,
                    const size_t iMaxIndex )
{
    if ( iFloorIndex == iMaxIndex ||
         Imath::equalWithAbsError( iFloorTime, iTime,
                                   kCHRONO_TOLERANCE ) )
    {
        return std::pair<index_t, chrono_t>( iFloorIndex, iFloorTime );
    }

    assert( iFloorIndex < iMaxIndex );
    chrono_t ceilTime = iThat->getSampleTime( iFloorIndex + 1 );
    assert( ceilTime >= iTime );
    return std::pair<index_t, chrono_t>( iFloorIndex + 1, ceilTime );
}

//-*****************************************************************************
std::pair<index_t, chrono_t>
TimeSampling::getCeilIndex( chrono_t iTime ) const
{
    //! Return the index of the sampled time that is >= iTime

    if ( m_numSamples < 1 ) { return std::pair<index_t, chrono_t>( 0, 0.0 ); }

    if ( m_timeSamplingType.isIdentity() )
    {
        index_t idx = ( index_t )ceil( iTime );
        idx = idx >= m_numSamples ? m_numSamples - 1 : idx;
        idx = idx < 0 ? 0 : idx;
        return std::pair<index_t, chrono_t>( idx, ( chrono_t )idx );
    }

    iTime -= kCHRONO_EPSILON;

    const index_t _maxind = m_numSamples - 1;
    const size_t maxIndex = _maxind > -1 ? _maxind : 0;

    const chrono_t minTime = this->getSampleTime( 0 );
    if ( iTime <= minTime )
    {
        return std::pair<index_t, chrono_t>( 0, minTime );
    }

    const chrono_t maxTime = this->getSampleTime( maxIndex );
    if ( iTime >= maxTime )
    {
        return std::pair<index_t, chrono_t>( maxIndex, maxTime );
    }

    std::pair<index_t, chrono_t> floorPair = this->getFloorIndex( iTime );

    return getCeilIndexHelper( this, iTime, floorPair.first, floorPair.second,
                               maxIndex );
}

//-*****************************************************************************
std::pair<index_t, chrono_t>
TimeSampling::getNearIndex( chrono_t iTime ) const
{
    //! Return the index of the sampled time that is:
    //! (iTime - floorTime < ceilTime - iTime) ? getFloorIndex( iTime )
    //! : getCeilIndex( iTime );

    if ( m_numSamples < 1 ) { return std::pair<index_t, chrono_t>( 0, 0.0 ); }

    if ( m_timeSamplingType.isIdentity() )
    {
        // floor of blah + 0.5 is round( blah )
        index_t idx = ( index_t )floor( iTime + 0.5 );
        idx = idx >= m_numSamples ? m_numSamples - 1 : idx;
        idx = idx < 0 ? 0 : idx;
        return std::pair<index_t, chrono_t>( idx, ( chrono_t )idx );
    }

    const index_t _maxind = m_numSamples - 1;
    const size_t maxIndex = _maxind > -1 ? _maxind : 0;

    const chrono_t minTime = this->getSampleTime( 0 );
    if ( iTime <= minTime )
    {
        return std::pair<index_t, chrono_t>( 0, minTime );
    }

    const chrono_t maxTime = this->getSampleTime( maxIndex );
    if ( iTime >= maxTime )
    {
        return std::pair<index_t, chrono_t>( maxIndex, maxTime );
    }

    std::pair<index_t, chrono_t> floorPair = this->getFloorIndex( iTime );
    std::pair<index_t, chrono_t> ceilPair = this->getCeilIndex( iTime );

    assert( ( floorPair.second <= iTime ||
              Imath::equalWithAbsError( iTime, floorPair.second,
                                        kCHRONO_TOLERANCE ) ) &&
            ( iTime <= ceilPair.second ||
              Imath::equalWithAbsError( iTime, ceilPair.second,
                                        kCHRONO_TOLERANCE ) ) );

    chrono_t deltaFloor = fabs( iTime - floorPair.second );
    chrono_t deltaCeil = fabs( ceilPair.second - iTime );

    if ( deltaFloor <= deltaCeil ) { return floorPair; }
    else { return ceilPair; }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreAbstract
} // End namespace Alembic
