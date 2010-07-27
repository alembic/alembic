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

#include <AlembicAsset/Base/Time.h>
#include <AlembicAsset/Base/PlainOldDataType.h>
#include <AlembicUtil/OperatorBool.h>
#include <limits>

#include <boost/python.hpp>

#include <Python.h>

namespace AlembicAsset {

//-*****************************************************************************
// Define a 'seconds' datatype, which is just a double.
// For storing time values, this is all you really need.
typedef float64_t chrono_t;
typedef chrono_t seconds_t;

//-*****************************************************************************
enum RestTimeFlag { kRestTime = 1 };

//-*****************************************************************************
// "Time" is for passing around a quantity of time with a unit.
// I have not yet defined arithmetic operators for this. I perhaps will
// someday, but NOT TODAY.
class Time
{
public:
    typedef Time this_type;

    // Unit values are to be regarded as "units per second";
    static inline const chrono_t kSeconds() throw() { return 1.0; }
    static inline const chrono_t kFilm() throw() { return 24.0; }
    static inline const chrono_t kPAL() throw() { return 25.0; }
    static inline const chrono_t kNTSC() throw() { return 29.97; }

    // The value we interpret as "no time" or "rest time"
    static inline const chrono_t kRestTimeValue() throw()
    { return std::numeric_limits<chrono_t>::max(); }

    Time() throw() : m_value( 0.0 ), m_secondsPerUnit( 1.0 ) {}

    explicit Time( RestTimeFlag rtf ) throw()
      : m_value( kRestTimeValue() ),
        m_secondsPerUnit( 1.0 ) {}

    explicit Time( seconds_t secs ) throw()
      : m_value( secs ),
        m_secondsPerUnit( 1.0 ) {}

    Time( chrono_t val, chrono_t ups ) throw()
      : m_value( val ),
        m_secondsPerUnit( 1.0/ups ) {}

    Time( const Time &copy ) throw()
      : m_value( copy.m_value ),
        m_secondsPerUnit( copy.m_secondsPerUnit ) {}

    Time &operator=( const Time &copy ) throw()
    {
        m_value = copy.m_value;
        m_secondsPerUnit = copy.m_secondsPerUnit;
        return *this;
    }

    chrono_t value() const throw() { return m_value; }
    void setValue( chrono_t d ) throw() { m_value = d; }

    chrono_t unitsPerSecond() const throw()
    { return 1.0 / m_secondsPerUnit; }
    void setUnitsPerSecond( chrono_t ups ) throw()
    { m_secondsPerUnit = 1.0 / ups; }

    seconds_t secondsPerUnit() const throw()
    { return m_secondsPerUnit; }
    void setSecondsPerUnit( seconds_t spu ) throw()
    { m_secondsPerUnit = spu; }

    chrono_t as( chrono_t ups ) const throw()
    {
        // convert self to seconds
        // chrono_t selfInSeconds = m_value / m_unitsPerSecond;
        // convert seconds to other
        // chrono_t selfInOther = selfInSeconds * ups;
        return ( m_value * m_secondsPerUnit ) * ups;
    }

    seconds_t seconds() const throw()
    {
        return ( m_value * m_secondsPerUnit );
    }

    bool isRestTime() const throw()
    {
        return ( m_value == kRestTimeValue() );
    }

    ALEMBIC_OPERATOR_BOOL_NOTHROW( m_value != kRestTimeValue() );

protected:
    chrono_t m_value;
    chrono_t m_secondsPerUnit;
};

//-*****************************************************************************
inline bool operator==( const Time &a, const Time &b )
{
    if ( a.isRestTime() && b.isRestTime() ) { return true; }
    return ( a.seconds() == b.seconds() );
}

//-*****************************************************************************
inline bool operator!=( const Time &a, const Time &b )
{
    return !operator==( a, b );
}

//-*****************************************************************************
inline std::ostream &operator<<( std::ostream &ostr, const Time &t )
{
    ostr << t.value() << " (unitsPerSecond="
         << t.unitsPerSecond() << ")";
    return ostr;
}

} // End namespace AlembicAsset

#endif
