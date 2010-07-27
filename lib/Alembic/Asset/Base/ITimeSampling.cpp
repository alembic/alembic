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

#include <Alembic/Asset/Base/ITimeSampling.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// UNIFORM TIME SAMPLING CLASS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
TimeSamplingType IUniformTimeSampling::type() const
{
    return kUniformTimeSampling;
}

//-*****************************************************************************
size_t IUniformTimeSampling::numSamples() const
{
    return m_numSamples;
}

//-*****************************************************************************
seconds_t IUniformTimeSampling::sampleSeconds( size_t samp ) const
{
    return m_firstSampleSeconds +
        (( seconds_t )samp) * m_firstSampleInterval;
}

//-*****************************************************************************
size_t IUniformTimeSampling::lowerBoundSample( seconds_t t ) const
{   
    if ( m_firstSampleInterval == 0.0 ) { return 0; }

    double tNorm;
    if ( m_firstSampleInterval < 0.0 )
    {
        tNorm = ( t - m_firstSampleSeconds ) /
            std::min( m_firstSampleInterval, -1.0e-9 );
    }
    else
    {
        tNorm = ( t - m_firstSampleSeconds ) /
            std::max( m_firstSampleInterval, 1.0e-9 );
    }
    
    int64_t n = ( int64_t )floor( tNorm + 0.000001 );
    
    return ( n < 0 ) ? 0 :
        ( n >= ( int64_t )m_numSamples ) ? m_numSamples - 1 :
        ( size_t )n;
}

//-*****************************************************************************
size_t IUniformTimeSampling::upperBoundSample( seconds_t t ) const
{
    if ( m_firstSampleInterval == 0.0 ) { return 0; }

    double tNorm;
    if ( m_firstSampleInterval < 0.0 )
    {
        tNorm = ( t - m_firstSampleSeconds ) /
            std::min( m_firstSampleInterval, -1.0e-9 );
    }
    else
    {
        tNorm = ( t - m_firstSampleSeconds ) /
            std::max( m_firstSampleInterval, 1.0e-9 );
    }
    
    int64_t n = ( int64_t )ceil( tNorm - 0.000001 );
    
    return ( n < 0 ) ? 0 :
        ( n >= ( int64_t )m_numSamples ) ? m_numSamples - 1 :
        ( size_t )n;
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// VARIABLE TIME SAMPLING
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
IVariableTimeSampling::IVariableTimeSampling( ISlabCache::Handle stimes )
  : m_sampleTimesSlabHandle( stimes )
{
    m_sampleTimes =
        ( const seconds_t * )((*m_sampleTimesSlabHandle)
                              .slab().rawData());
    m_numSamples = m_sampleTimesSlabHandle->slab().numElements();
    AAST_ASSERT( m_numSamples > 0,
                 "Variable Time Sampling needs more than 0 time samples." );
    
    // CJH - might need to do this no matter what.
    // If we allow for notes to be put onto the slab buffers, we can at least
    // make sure this only happens once per slab buffer load.
    // CJH - disable it because it signifies unwritten samples?
#ifdef DEBUG
    for ( size_t i = 1; i < m_numSamples; ++i )
    {
        AAST_ASSERT( m_sampleTimes[i] > m_sampleTimes[i-1],
                     "Non-sequential time samples in IVariableTimeSampling" );
    }
#endif
}

//-*****************************************************************************
TimeSamplingType IVariableTimeSampling::type() const
{
    return kVariableTimeSampling;
}

//-*****************************************************************************
size_t IVariableTimeSampling::numSamples() const
{
    return m_numSamples;
}

//-*****************************************************************************
seconds_t IVariableTimeSampling::sampleSeconds( size_t samp ) const
{
    AAST_ASSERT( samp < m_numSamples, "Out of range sample time" );
    return m_sampleTimes[samp];
}

//-*****************************************************************************
size_t IVariableTimeSampling::lowerBoundSample( seconds_t t ) const
{
    // This relies on sampleTimes being ordered properly. we probably
    // do need to check that somewhere.
    // So - lower bound actually returns the first location where
    // we could insert the value without violating the ordering.
    // The iterator we've been given is guaranteed to be either
    // equal to 't' or greater than 't'. If equal, we return idx.
    // If greater, we return idx-1.
    const double *lbIter =
        std::lower_bound( m_sampleTimes, m_sampleTimes + m_numSamples,
                          ( double )t );
    
    size_t idx = lbIter - m_sampleTimes;

    if ( idx == 0 ) { return 0; }
    else if ( idx >= m_numSamples ) { return m_numSamples-1; }
    else
    {
        if ( m_sampleTimes[idx] == t )
        {
            return idx;
        }
        else
        {
            return idx-1;
        }
    }
}

//-*****************************************************************************
size_t IVariableTimeSampling::upperBoundSample( seconds_t t ) const
{
    // This relies on sampleTimes being ordered properly. we probably
    // do need to check that somewhere.
    const double *ubIter =
        std::upper_bound( m_sampleTimes, m_sampleTimes + m_numSamples,
                          ( double )t );
    size_t idx = ubIter - m_sampleTimes;
    return std::min( idx, ( size_t )m_numSamples-1 );
}   

} // End namespace Asset
} // End namespace Alembic



