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

#ifndef _Alembic_AbcCoreAbstract_TimeSamplingType_h_
#define _Alembic_AbcCoreAbstract_TimeSamplingType_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
//! The TimeSamplingType class controls how properties in Alembic relate
//! time values to their sample indices.
//!
//! The default behavior is that there is no intrinsic notion of time,
//! and the "time" for a sample is just the integer sample index converted
//! to a double-precision float (chrono_t). This is called "Identity"
//! time sampling.
//!
//! The second behavior is where there is a time value associated with sample
//! zero (which will not be stored as part of this type), and a uniform
//! time amount between each subsequent sample. This is called "Uniform"
//! time sampling, and would correspond to sampling every frame at 1/24
//! per second, or similar.
//!
//! The third behavior is where there is a period of time over which a fixed
//! number of samples are distributed unevenly - imagine a render scene sampled
//! across a shutter period at shutter-begin-open, shutter-full-open,
//! shutter-begin-close, shutter-full-close. This is (perhaps confusingly)
//! called "Cyclic" time sampling.
//!
//! The final behavior is where the time samples are totally uneven. We
//! make a restriction that they must be strictly increasing,
//! as the indices are increasing.  This is so we can bisection search to find
//! the lower or upper bounds when searching for floor, ceiling, or nearest
//! samples by comparing time.  This is called "Acyclic" time sampling.
//!
//! An additional concept that is encapsulated by the \ref TimeSamplingType
//! class is the idea of "Time Retention". When properties are written with
//! constant data (i.e., data that does not change over the sample range,
//! regardless of how many samples that were written), Alembic will only store
//! the single sample and will mark the property as "Constant".  In such a
//! situation, the time values that were associated with the samples that were
//! written to the property are still "data". However, in the common case,
//! a normal user probably doesn't care about the time values associated
//! with a constant property. These time values actually amount to a somewhat
//! significant amount of storage, especially in a large model with many
//! properties. Therefore, the TimeSamplingType additionally contains
//! a boolean flag indicating whether or not time values should be retained
//! in the case of a constant property. This boolean flag is called
//! "RetainConstantSampleTimes", and it defaults to false.
class TimeSamplingType
{
public:
    //! CONSTANTS
    //! ...
    //static const uint32_t ACYCLIC_NUM_SAMPLES; // set to infinity
    //static const chrono_t ACYCLIC_TIME_PER_CYCLE; // set to infinity

    //! IDENTITY
    //! ...
    TimeSamplingType()
      : m_numSamplesPerCycle( 0 ),
        m_timePerCycle( 0.0 ),
        m_retainConstantSampleTimes( false ) {}

    //! UNIFORM
    //! ...
    explicit TimeSamplingType( chrono_t iTimePerCycle )
      : m_numSamplesPerCycle( 1 )
      , m_timePerCycle( iTimePerCycle )
      , m_retainConstantSampleTimes( false )
    {
        ABCA_ASSERT( m_timePerCycle > 0.0 &&
                     m_timePerCycle < ACYCLIC_TIME_PER_CYCLE,
                     "Time per cycle must be greater than 0 " <<
                     "and can not be ACYCLIC_TIME_PER_CYCLE." );
    }

    //! CYCLIC
    //! ...
    TimeSamplingType( uint32_t iNumSamplesPerCycle,
                      chrono_t iTimePerCycle )
      : m_numSamplesPerCycle( iNumSamplesPerCycle )
      , m_timePerCycle( iTimePerCycle )
      , m_retainConstantSampleTimes( false )
    {
        ABCA_ASSERT(
            // Identity
            ( m_timePerCycle == 0.0 && m_numSamplesPerCycle == 0 ) ||

            // Acyclic
            ( m_timePerCycle == ACYCLIC_TIME_PER_CYCLE &&
              m_numSamplesPerCycle == ACYCLIC_NUM_SAMPLES ) ||

            // valid time per cycle
            ( m_timePerCycle > 0.0 &&
              m_timePerCycle < ACYCLIC_TIME_PER_CYCLE &&

              // and valid samples per cycle
              m_numSamplesPerCycle > 0 &&
              m_numSamplesPerCycle < ACYCLIC_NUM_SAMPLES ),
            "Invalid Time Sampling Type, time per cycle: "
            << m_timePerCycle << " samples per cycle: "
            <<  m_numSamplesPerCycle );

    }

    //! ACYCLIC
    //! This enum exists solely as a way of distinguishing between
    //! the argument-less static time sampling, and
    //! the argument-less acyclic time sampling.
    enum AcyclicFlag { kAcyclic };
    explicit TimeSamplingType( AcyclicFlag iAF )
      : m_numSamplesPerCycle( ACYCLIC_NUM_SAMPLES )
      , m_timePerCycle( ACYCLIC_TIME_PER_CYCLE )
      , m_retainConstantSampleTimes( false ) {}

    //! Using Default Copy Constructor
    //! Using Default Assignment Operator

    //! Asks if the sampling is:
    //! Identity (0 samples per cycle)
    //! Uniform (1 sample per cycle)
    //! Cyclic (N>1 samples per cycle)
    //! Acyclic (INF samples per cycle - acyclic!)
    bool isIdentity() const { return m_numSamplesPerCycle == 0; }
    bool isUniform() const { return m_numSamplesPerCycle == 1; }
    bool isCyclic() const
    {
        return ( ( m_numSamplesPerCycle > 1 ) &&
                 ( m_numSamplesPerCycle < ACYCLIC_NUM_SAMPLES ) );
    }
    bool isAcyclic() const
    { return m_numSamplesPerCycle == ACYCLIC_NUM_SAMPLES; }

    static const uint32_t AcyclicNumSamples();
    static const chrono_t AcyclicTimePerCycle();

    uint32_t getNumSamplesPerCycle() const { return m_numSamplesPerCycle; }

    chrono_t getTimePerCycle() const { return m_timePerCycle; }

    //! The flag which indicates whether or not to retain time
    //! samples on properties that are constant. When properties
    //! are written with samples that end up being constant, Alembic
    //! condenses them to a single constant sample. In such a case,
    //! the times that were associated with each sample are still
    //! potentially interesting or useful data. However, the "normal"
    //! usage would be to discard this time data and treat the property
    //! as "static" (constant with no associated times). This flag
    //! indicates the desired behavior.
    bool getRetainConstantSampleTimes() const
    { return m_retainConstantSampleTimes; }

    void setRetainConstantSampleTimes( bool iRCST )
    { m_retainConstantSampleTimes = iRCST; }

private:
    uint32_t m_numSamplesPerCycle;
    chrono_t m_timePerCycle;
    bool m_retainConstantSampleTimes;

    static const uint32_t ACYCLIC_NUM_SAMPLES;
    static const chrono_t ACYCLIC_TIME_PER_CYCLE;

public:
    friend std::ostream &operator<<( std::ostream &ostr,
                                     const TimeSamplingType &tst );
};


} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif
