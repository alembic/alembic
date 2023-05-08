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

#include <Alembic/AbcCoreAbstract/TimeSamplingType.h>

#include <Imath/ImathMath.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
// static instancing
namespace {
const uint32_t ACYCLIC_NUM_SAMPLES = std::numeric_limits<uint32_t>::max();

// Not all the way to the max.
const chrono_t ACYCLIC_TIME_PER_CYCLE = std::numeric_limits<chrono_t>::max() / 32.0;

// Work around the imprecision of comparing floating values.
static const chrono_t kCYCLE_EPSILON = 1e-9;
}

//-*****************************************************************************
// get these constants; similar to std::numeric_limits<>()
uint32_t TimeSamplingType::AcyclicNumSamples()
{
    return ACYCLIC_NUM_SAMPLES;
}

chrono_t TimeSamplingType::AcyclicTimePerCycle()
{
    return ACYCLIC_TIME_PER_CYCLE;
}

bool TimeSamplingType::operator==( const TimeSamplingType & iRhs ) const
{
    return ( m_numSamplesPerCycle == iRhs.m_numSamplesPerCycle &&
             Imath::equalWithAbsError( m_timePerCycle, iRhs.m_timePerCycle,
                                       kCYCLE_EPSILON ) );
}

//-*****************************************************************************
//! Prints out relevant information about the TimeSamplingType instance
std::ostream &operator<<( std::ostream &ostr, const TimeSamplingType &tst )
{
    std::string baseType( "" );

    if ( tst.isUniform() ) { baseType = "Uniform"; }
    else if ( tst.isCyclic() ) { baseType = "Cyclic"; }
    else { baseType = "Acyclic"; }

    ostr << baseType << " time sampling";

    if ( tst.isUniform() )
    {
        ostr << " with " << tst.getTimePerCycle() << " chrono_ts/cycle";
    }
    else if ( tst.isCyclic() )
    {
        ostr << " with " << tst.getNumSamplesPerCycle() << " samps/cycle "
             << "and " << tst.getTimePerCycle() << " chrono_ts/cycle";
    }

    return ostr;
}



} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreAbstract
} // End namespace Alembic

