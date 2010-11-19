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

#ifndef _Alembic_AbcCoreAbstract_TimeSampling_h_
#define _Alembic_AbcCoreAbstract_TimeSampling_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/TimeSamplingType.h>
#include <Alembic/AbcCoreAbstract/ArraySample.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {


//-*****************************************************************************
//! The TimeSampling class's whole job is to report information about the
//! time values that are associated with the samples that were written
//! to a property. Most of the time, the sampling will be uniform or
//! cyclic, in which case this is mostly an algorithmic interface to
//! the small sample times buffer. In the case where the sampling is
//! truly acyclic, this class acts as an accessor to that array of times.
class TimeSampling
{
public:
    //! The TimeSampling class is really a portable interface, usable
    //! via aggregation, that the SimplePropertyReaders and Writers (and
    //! their derived classes) can use to provide consistent time-sampling
    //! introspection to clients.
    //! The iSampleTimes passed in have a shared ptr so that this instance
    //! may ensure they exist.
    TimeSampling( const TimeSamplingType & iTimeSamplingType,

                  //! We have to pass in the number of samples explicitly
                  //! because the sample times, in the case of a
                  //! uniform or cyclic time sampling will only contain
                  //! as many times as there are per cycle.
                  size_t iNumSamples,

                  //! The num sample times is always equal to
                  //! std::min( iNumSamples,
                  //!           iTimeSamplingType.getNumSamplesPerCycle() )
                  //! Therefore we just need a pointer to times
                  //! here. We can infer the size appropriately.

                  //! Float64 (chrono_t) with rank 1 - the actual times
                  //! when samples were recorded for this property.
                  ArraySamplePtr iSampleTimes );

    TimeSampling( const TimeSampling &copy );

    TimeSampling();

    //! Get the number of samples
    //! In the case of a property which had no samples written,
    //! this can be zero.
    size_t getNumSamples() const
    {
        return m_numSamples;
    }

    TimeSamplingType getTimeSamplingType() const
    {
        return m_timeSamplingType;
    }

    //! Get the time of any sample
    //! This will return NON_TIME if the sampling is static
    //! it is invalid to call this for out-of-range indices.
    chrono_t getSampleTime( index_t iIndex ) const;

    //! Find the largest valid index that has a time less than or equal
    //! to the given time. Invalid to call this with zero samples.
    //! If the minimum sample time is greater than iTime, index
    //! 0 will be returned.
    std::pair<index_t, chrono_t> getFloorIndex( chrono_t iTime ) const;

    //! Find the smallest valid index that has a time greater
    //! than the given time. Invalid to call this with zero samples.
    //! If the maximum sample time is less than iTime, index
    //! numSamples-1 will be returned.
    std::pair<index_t, chrono_t> getCeilIndex( chrono_t iTime ) const;

    //! Find the valid index with the closest time to the given
    //! time. Invalid to call this with zero samples.
    std::pair<index_t, chrono_t> getNearIndex( chrono_t iTime ) const;

    //! This returns whether or not the time sampling is static
    //! Static time sampling means one or less samples with identity
    //! time sampling.
    bool isStatic() const
    {
        return m_timeSamplingType.isIdentity() &&
            m_numSamples < 2;
    }

    //! How many stored times are there really?
    size_t getNumStoredTimes() const
    {
        return m_sampleTimes->size();
    }

protected:
    //! A TimeSamplingType
    //! This is "Identity", "Uniform", "Cyclic", or "Acyclic".
    //! In the case of identity time sampling, no actual times are
    //! required.
    TimeSamplingType m_timeSamplingType;

    //! The number of samples.
    //! This needs to be distinct from the sample times below, because
    //! if the time sampling is anything other than acyclic, there will
    //! only be numSamplesPerCycle time samples below.
    size_t m_numSamples;

    ArraySamplePtr  m_sampleTimes;

    //! Convenience, internal utility to access the array of
    //! choron_t values held in m_sampleTimes as a simple array.
    const chrono_t *_getTimeSamplesAsChrono_tPtr() const;
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif

