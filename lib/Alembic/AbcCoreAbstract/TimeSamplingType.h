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
class TimeSamplingType
{
public:
    // CONSTANTS
    static const size_t ACYCLIC_NUM_SAMPLES; // set to infinity
    static const chrono_t ACYCLIC_TIME_PER_CYCLE; // set to infinity
    
    // STATIC
    TimeSamplingType()
      : m_numSamplesPerCycle( 0 ),
        m_timePerCycle( 0.0 ) {}

    // UNIFORM
    explicit TimeSamplingType( chrono_t iTimePerCycle )
      : m_numSamplesPerCycle( 1 ),
        m_timePerCycle( iTimePerCycle ) {}

    // CYCLIC
    TimeSamplingType( size_t iNumSamplesPerCycle,
                      chrono_t iTimePerCycle )
      : m_numSamplesPerCycle( iNumSamplesPerCycle ),
        m_timePerCycle( iTimePerCycle ) {}

    // ACYCLIC
    // This enum exists solely as a way of distinguishing between
    // the argument-less static time sampling, and
    // the argument-less acyclic time sampling.
    enum AcyclicFlag { kAcyclic };
    TimeSamplingType( AcyclicFlag iAF )
      : m_numSamplesPerCycle( ACYCLIC_NUM_SAMPLES ),
        m_timePerCycle( ACYCLIC_TIME_PER_CYCLE ) {}

    // Copy
    TimeSamplingType( const TimeSamplingType &iCopy )
      : m_numSamplesPerCycle( iCopy.m_numSamplesPerCycle ),
        m_timePerCycle( iCopy.m_timePerCycle ) {}

    // Assignment
    TimeSamplingType& operator=( const TimeSamplingType &iCopy )
    {
        m_numSamplesPerCycle = iCopy.m_numSamplesPerCycle;
        m_timePerCycle = iCopy.m_timePerCycle;
    }

    // Asks if the sampling is:
    // Static (0 samples per cycle)
    // Uniform (1 sample per cycle)
    // Cyclic (N>1 samples per cycle)
    // Acyclic (INF samples per cycle - acyclic!)
    bool isStatic() const { return m_numSamplesPerCycle == 0; }
    bool isUniform() const { return m_numSamplesPerCycle == 1; }
    bool isCyclic() const
    {
        return ( ( m_numSamplesPerCycle > 1 ) &&
                 ( m_numSamplesPerCycle < ACYCLIC_NUM_SAMPLES ) );
    }
    bool isAcyclic() const
    { return m_numSamplesPerCycle == ACYCLIC_NUM_SAMPLES; }

    size_t getNumSamplesPerCycle() const { return m_numSamplesPerCycle; }

    chrono_t getTimePerCycle() const { return m_timePerCycle; }
    
private:
    size_t m_numSamplesPerCycle;
    chrono_t m_timePerCycle;
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif
