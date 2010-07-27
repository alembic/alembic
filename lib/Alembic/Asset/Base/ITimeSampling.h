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

#ifndef _Alembic_Asset_Base_ITimeSampling_h_
#define _Alembic_Asset_Base_ITimeSampling_h_

#include <Alembic/Asset/Base/FoundationBase.h>
#include <Alembic/Asset/Base/TimeSamplingInfo.h>
#include <Alembic/Asset/Base/ISlabCache.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
// Abstract base class.
class ITimeSampling
{
public:
    ITimeSampling(){}
    virtual ~ITimeSampling(){}

    // is it uniform or variable?
    virtual TimeSamplingType type() const = 0;
    bool isUniform() const { return ( type() == kUniformTimeSampling ); }
    bool isVariable() const { return ( type() == kVariableTimeSampling ); }

    // Return the number of samples
    virtual size_t numSamples() const = 0;

    // Return the seconds of a particular sample
    virtual seconds_t sampleSeconds( size_t samp ) const = 0;

    // Return the lower or upper sample number for a given time.
    virtual size_t lowerBoundSample( seconds_t s ) const = 0;
    size_t lowerBoundSample( const Time &t ) const
    {
        return lowerBoundSample( t.seconds() );
    }
    virtual size_t upperBoundSample( seconds_t s ) const = 0;
    size_t upperBoundSample( const Time &t ) const
    {
        return upperBoundSample( t.seconds() );
    }
};

typedef boost::shared_ptr<ITimeSampling> SharedITimeSampling;

//-*****************************************************************************
class IUniformTimeSampling : public ITimeSampling
{
public:
    IUniformTimeSampling( seconds_t firstSampSecs,
                          seconds_t firstSampIntvl,
                          size_t numSamps )
      : m_firstSampleSeconds( firstSampSecs ),
        m_firstSampleInterval( firstSampIntvl ),
        m_numSamples( numSamps ) {}
    
    virtual TimeSamplingType type() const;
    virtual size_t numSamples() const;
    virtual seconds_t sampleSeconds( size_t samp ) const;
    virtual size_t lowerBoundSample( seconds_t t ) const;
    virtual size_t upperBoundSample( seconds_t t ) const;

    seconds_t firstSampleSeconds() const { return m_firstSampleSeconds; }
    seconds_t firstSampleInterval() const { return m_firstSampleInterval; }

protected:
    seconds_t m_firstSampleSeconds;
    seconds_t m_firstSampleInterval;
    size_t m_numSamples;
};

typedef boost::shared_ptr<IUniformTimeSampling> SharedIUniformTimeSampling;

inline SharedIUniformTimeSampling
MakeSharedIUniformTimeSampling( seconds_t fss,
                                seconds_t fsi,
                                size_t numSamps )
{
    return boost::make_shared<IUniformTimeSampling>( fss, fsi, numSamps );
}

//-*****************************************************************************
class IVariableTimeSampling : public ITimeSampling
{
public:
    IVariableTimeSampling( ISlabCache::Handle stimes );

    virtual TimeSamplingType type() const;
    virtual size_t numSamples() const;
    virtual seconds_t sampleSeconds( size_t samp ) const;
    virtual size_t lowerBoundSample( seconds_t t ) const;
    virtual size_t upperBoundSample( seconds_t t ) const;

    const seconds_t *sampleTimes() const
    {
        return m_sampleTimes;
    }
    
protected:
    ISlabCache::Handle m_sampleTimesSlabHandle;
    const seconds_t *m_sampleTimes;
    size_t m_numSamples;
};

typedef boost::shared_ptr<IVariableTimeSampling> SharedIVariableTimeSampling;
inline SharedIVariableTimeSampling
MakeSharedIVariableTimeSampling( ISlabCache::Handle stimes )
{
    return boost::make_shared<IVariableTimeSampling>( stimes );
}

} // End namespace Asset
} // End namespace Alembic

#endif
