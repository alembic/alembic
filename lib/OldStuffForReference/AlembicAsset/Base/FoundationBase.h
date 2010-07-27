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

#ifndef _AlembicAsset_Base_FoundationBase_h_
#define _AlembicAsset_Base_FoundationBase_h_

//-*****************************************************************************
// No HDF5 Stuff included in base, only in Body.
//-*****************************************************************************
#include <AlembicAsset/Base/PlainOldDataType.h>
#include <AlembicAsset/Base/DataType.h>
#include <AlembicAsset/Base/Exception.h>
#include <AlembicAsset/Base/Assert.h>
#include <AlembicAsset/Base/Time.h>
#include <AlembicUtil/Dimensions.h>
#include <AlembicUtil/OperatorBool.h>
#include <ImathVec.h>
#include <ImathMatrix.h>
#include <ImathQuat.h>
#include <ImathBox.h>
#include <boost/cstdint.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>
#include <assert.h>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace AlembicAsset {

//-*****************************************************************************
typedef AlembicUtil::Dimensions Dimensions;

//-*****************************************************************************
typedef std::vector<boost::uint8_t> Bytes;
typedef std::vector<seconds_t> SecondsArray;

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// IMPORTED IMATH TYPES
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
typedef Imath::V2s V2s;
typedef Imath::V2i V2i;
typedef Imath::V2f V2f;
typedef Imath::V2d V2d;

typedef Imath::V3s V3s;
typedef Imath::V3i V3i;
typedef Imath::V3f V3f;
typedef Imath::V3d V3d;

typedef Imath::Box2s Box2s;
typedef Imath::Box2i Box2i;
typedef Imath::Box2f Box2f;
typedef Imath::Box2d Box2d;

typedef Imath::Box3s Box3s;
typedef Imath::Box3i Box3i;
typedef Imath::Box3f Box3f;
typedef Imath::Box3d Box3d;

typedef Imath::M33f M33f;
typedef Imath::M33d M33d;
typedef Imath::M44f M44f;
typedef Imath::M44d M44d;

typedef Imath::Quatf Quatf;
typedef Imath::Quatd Quatd;

//-*****************************************************************************
enum PropertyType
{
    kSingularProperty,
    kMultiProperty,
    kUnknownPropertyType
};

//-*****************************************************************************
enum TimeSamplingType
{
    kUniformTimeSampling,
    kVariableTimeSampling,
    kUnknownTimeSamplingType
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// INFO STRUCTS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
//-*****************************************************************************
// OBJECT INFO
//-*****************************************************************************
//-*****************************************************************************
struct ObjectInfoBody
{
    ObjectInfoBody() throw()
      : name( "UNKNOWN" ), protocol( "UNKNOWN" ) {}
    
    ObjectInfoBody( const std::string &n,
                    const std::string &p ) throw()
      : name( n ), protocol( p ) {}
    
    
    std::string name;
    std::string protocol;
};

typedef boost::shared_ptr<ObjectInfoBody> ObjectInfo;

inline ObjectInfo MakeObjectInfo( void )
{
    return boost::make_shared<ObjectInfoBody>();
}

inline ObjectInfo MakeObjectInfo( const std::string &n,
                                  const std::string &p )
{
    return boost::make_shared<ObjectInfoBody>( n, p );
}

//-*****************************************************************************
//-*****************************************************************************
// PROPERTY INFO
//-*****************************************************************************
//-*****************************************************************************
struct PropertyInfoBody
{
    PropertyInfoBody() throw()
      : name( "UNKNOWN" ), protocol( "UNKNOWN" ),
        ptype( kUnknownPropertyType ), dtype( kUnknownPOD, 0 ) {}
    
    PropertyInfoBody( const std::string &n,
                      const std::string &p,
                      PropertyType pt,
                      const DataType &dt ) throw()
      : name( n ), protocol( p ), ptype( pt ), dtype( dt ) {}
    
    std::string name;
    std::string protocol;
    PropertyType ptype;
    DataType dtype;
};

typedef boost::shared_ptr<PropertyInfoBody> PropertyInfo;

inline PropertyInfo MakePropertyInfo( void )
{
    return boost::make_shared<PropertyInfoBody>();
}

inline PropertyInfo MakePropertyInfo( const std::string &n,
                                      const std::string &p,
                                      PropertyType pt,
                                      const DataType &dt )
{
    return boost::make_shared<PropertyInfoBody>( n, p, pt, dt );
}

//-*****************************************************************************
//-*****************************************************************************
// TIME SAMPLING INFO
//-*****************************************************************************
//-*****************************************************************************
struct TimeSamplingInfo
{
    typedef TimeSamplingInfo this_type;
    
    // Default constructor creates Unknown time sampling
    TimeSamplingInfo() throw()
      : type( kUnknownTimeSamplingType ),
        firstSampleSeconds( 0.0 ),
        firstSampleInterval( 1.0 ),
        secondSampleInterval( 1.0 ),
        numSamplesHint( 0 ) {}

    // Explicit constructor.
    explicit TimeSamplingInfo( TimeSamplingType tst,
                               seconds_t fss = 0.0,
                               seconds_t fsi = 1.0,
                               seconds_t ssi = 1.0,
                               size_t nsh = 0 ) throw()
      : type( tst ),
        firstSampleSeconds( fss ),
        firstSampleInterval( fsi ),
        secondSampleInterval( ssi ),
        numSamplesHint( nsh ) {}

    // Copy, assign
    TimeSamplingInfo( const TimeSamplingInfo &copy ) throw()
      : type( copy.type ),
        firstSampleSeconds( copy.firstSampleSeconds ),
        firstSampleInterval( copy.firstSampleInterval ),
        secondSampleInterval( copy.secondSampleInterval ),
        numSamplesHint( copy.numSamplesHint ) {}

    TimeSamplingInfo &operator=( const TimeSamplingInfo &copy ) throw()
    {
        type = copy.type;
        firstSampleSeconds = copy.firstSampleSeconds;
        firstSampleInterval = copy.firstSampleInterval;
        secondSampleInterval = copy.secondSampleInterval;
        numSamplesHint = copy.numSamplesHint;
        return *this;
    }

    bool equivalent( const TimeSamplingInfo &other ) const throw()
    {
        if ( other.type != type ) { return false; }

        switch ( type )
        {
        case kUniformTimeSampling:
            // first sample seconds & increment must match
            return ( firstSampleSeconds == other.firstSampleSeconds &&
                     firstSampleInterval == other.firstSampleInterval );
        case kVariableTimeSampling:
            // Variable time sampling is always equivalent to itself.
            return true;
        default:
            // unknown.
            return false;
        };
    }

    // Only for Uniform, eventually Open-Close
    seconds_t sampleToSeconds( size_t s ) const throw()
    {
        switch ( type )
        {
        case kUniformTimeSampling:
            return firstSampleSeconds +
                firstSampleInterval * ( seconds_t )s;
        default:
            // This is a programmer error.
            return Time::kRestTimeValue();
        };
    }

    // Only for Uniform, eventually Open-Close
    size_t secondsToSample( seconds_t s ) const throw()
    {
        if ( type == kUniformTimeSampling && firstSampleInterval != 0.0 )
        {
            return ( size_t )
                floor( 0.5 + ( ( s - firstSampleSeconds ) /
                               firstSampleInterval ) );
        }

        // This is a programmer error.
        assert( 0 );
        return 0;
    }

    // Uniform, Variable, eventually Open-Close
    TimeSamplingType type;

    // Seconds of the first sample. For Uniform & Open-Close,
    // ignored for Variable.
    seconds_t firstSampleSeconds;

    // Size of the increment between sample 0 & sample 1.
    // Used by Uniform & Open-Close, ignored for Variable
    seconds_t firstSampleInterval;

    // Size of the increment between sample 1 & sample 2.
    // Used by Open-Close, ignored for Uniform & Variable.
    seconds_t secondSampleInterval;

    // Hint about the expected number of samples. May be left to 0,
    // is solely used as a hint.
    size_t numSamplesHint;

    ALEMBIC_OPERATOR_BOOL_NOTHROW( ( type == kUniformTimeSampling ||
                                     type == kVariableTimeSampling ) );
};

//-*****************************************************************************
// overload == to mean "equivalent"
inline bool operator==( const TimeSamplingInfo &a, const TimeSamplingInfo &b )
    throw()
{
    return a.equivalent( b );
}

inline bool operator!=( const TimeSamplingInfo &a, const TimeSamplingInfo &b )
{
    return !a.equivalent( b );
}

} // End namespace AlembicAsset

#endif
