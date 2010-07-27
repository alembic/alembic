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

#ifndef _Alembic_Asset_SampleSelector_h_
#define _Alembic_Asset_SampleSelector_h_

#include <Alembic/Asset/Foundation.h>
#include <Alembic/Asset/TimeSampling.h>
#include <Alembic/Asset/Time.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
// This is for TimeSample selection
enum SampleSelectType
{
    // The sample associated with 'no time'
    kStaticSample,

    // The sample associated with a specific index
    kAnimIndexSample,

    // The sample whose time is closest to the
    // specified time.
    kAnimNearestSample,

    // The sample whose time is as large as
    // possible without being greater than a specified time
    kAnimFloorSample,

    // The sample whose time is as small as
    // possible without being less than a specified time
    kAnimCeilSample,

    // Unknown sample select type
    kUnknownSampleSelectType = 127
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// Read SAMPLE SELECTOR
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
class ReadSampleSelector
{
public:
    //-*************************************************************************
    // CONSTRUCTORS
    //-*************************************************************************
    
    // Default is "static" sample.
    ReadSampleSelector()
      : m_type( kStaticSample ),
        m_index( -1 ),
        m_seconds( Time::kNonTimeValue() ) {}

    // Specify an "anim" sample, using just an index.
    // This works for any sampling type, since on input the samples
    // are totally well defined.
    explicit ReadSampleSelector( size_t idx )
      : m_type( kAnimIndexSample ),
        m_index( idx ),
        m_seconds( Time::kNonTimeValue() ) {}

    // Specify an "anim" sample, using just a time value.
    // With input samples, you need to explicitly specify which
    // kind of anim sample - nearest, floor, or ceiling.
    // Nearest is the default.
    explicit ReadSampleSelector( seconds_t secs,
                                 SampleSelectType typ = kAnimNearestSample )
      : m_type( typ ),
        m_index( -1 ),
        m_seconds( secs ) {}

    //-*************************************************************************
    // Direct access
    SampleSelectType type() const { return m_type; }
    
    //-*************************************************************************
    // The primary goal of this class is to uniquely identify a sample index,
    // based on the various selector fields within, and hashed against an
    // actual "ITimeSampling" class. It will return -1 for the static
    // sample, if that is what was requested.
    //-*************************************************************************
    TimeSampleIndex index( const ITimeSampling &ots ) const;

    //-*************************************************************************
    // This works for all sample types - with input, we can always tell which
    // sample we're referring to.
    seconds_t seconds( const ITimeSampling &ots ) const;

protected:
    //-*************************************************************************
    // DATA
    //-*************************************************************************
    SampleSelectType m_type;
    TimeSampleIndex m_index;
    seconds_t m_seconds;
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// WRITE SAMPLE SELECTOR
// 
// The Output Sample Selector class provides a flexible way of specifying
// which sample you are writing. It can be the "static" sample, which is the
// sample associated with the time "NonTime" - it can be an animation sample,
// which (depending on the time sampling type) may require a time value.
//
// If the property being written to has variable time sampling, both a
// time value and a sample number are required. If the property being
// written to has uniform time sampling, only a sample time or a sample
// index is required, either can be derived from the other.
//
// This class exists to cut down on the functional explosion of calling
// patterns on the property classes and their derivations.
// Prior to this class, there were "setStatic", "setAnim", and so on.
//
// There are so many ways to skin this cat - we could write a global
// "set" function that is overloaded, but this works well enough, and
// there's no such thing as the ideal API. Let there be rough edges! Let
// there occasionally be inconsistencies! Huzzah!!!
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
class WriteSampleSelector
{
public:
    //-*************************************************************************
    // CONSTRUCTORS
    //-*************************************************************************
    
    // Default is "static" sample.
    WriteSampleSelector()
      : m_type( kStaticSample ),
        m_index( -1 ),
        m_seconds( Time::kNonTimeValue() ) {}

    // Specify an "anim" sample, using just an index.
    // This only works when the target property has non-variable
    // sampling.
    explicit WriteSampleSelector( size_t idx )
      : m_type( kAnimIndexSample ),
        m_index( idx ),
        m_seconds( Time::kNonTimeValue() ) {}

    // Specify an "anim" sample, using just a time value.
    // This only works when the target property has non-variable
    // sampling.
    explicit WriteSampleSelector( seconds_t secs )
      : m_type( kAnimNearestSample ),
        m_index( -1 ),
        m_seconds( secs ) {}

    // Specify an "anim" sample with an explicit index, for varying
    // time sampling.
    WriteSampleSelector( size_t idx, seconds_t secs )
      : m_type( kAnimIndexSample ),
        m_index( idx ),
        m_seconds( secs ) {}
    
    //-*************************************************************************
    // The primary goal of this class is to uniquely identify a sample index,
    // based on the various selector fields within, and hashed against an
    // actual "WriteTimeSampling" class. It will return -1 for the static
    // sample, if that is what was requested.
    //-*************************************************************************
    TimeSampleIndex index( const WriteTimeSampling &ots ) const;

    //-*************************************************************************
    seconds_t seconds( const WriteTimeSampling &ots ) const;

protected:
    //-*************************************************************************
    // DATA
    //-*************************************************************************
    SampleSelectType m_type;
    TimeSampleIndex m_index;
    seconds_t m_seconds;
};

} // End namespace AlembicAsset

#endif
