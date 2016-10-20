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

#include <Alembic/AbcCoreAbstract/All.h>

#include "Assert.h"

#include <float.h>
#include <stdlib.h>

#include <ImathMath.h>
#include <ImathRandom.h>

#include <vector>
#include <iostream>

//-*****************************************************************************
namespace AbcA = Alembic::AbcCoreAbstract::v9;
using AbcA::chrono_t;
using AbcA::index_t;

typedef std::vector<chrono_t> TimeVector;

//-*****************************************************************************
void validateTimeSampling( const AbcA::TimeSampling &timeSampling,
                           const AbcA::TimeSamplingType &timeSamplingType,
                           const TimeVector &timeVector,
                           const size_t numSamplesPerCycle,
                           const chrono_t timePerCycle )
{
    const chrono_t period = timeSamplingType.getTimePerCycle();

    TESTING_MESSAGE_ASSERT( period == timePerCycle,
        "calculated cycle period does not match given time/cycle" );


    std::cout << "***********************************************************"
              << std::endl;

    if ( timeSamplingType.isUniform() )
    { std::cout << "time sampling type is uniform" << std::endl; }
    if ( timeSamplingType.isCyclic() )
    { std::cout << "time sampling type is cyclic" << std::endl; }
    if ( timeSamplingType.isAcyclic() )
    { std::cout << "time sampling type is acyclic" << std::endl; }

    std::cout << "Number of samples per cycle is "
              << timeSamplingType.getNumSamplesPerCycle()
              << std::endl << std::endl;

    std::cout << "Given times:" << std::endl;
    for ( size_t i = 0 ; i < timeVector.size() ; ++i )
    {
        std::cout << i << ": " << timeVector[i] << " " << std::endl;
    }
    std::cout << std::endl << "with a period of " << period << std::endl
              << std::endl;

    TESTING_MESSAGE_ASSERT( timeSamplingType.isAcyclic() ||
        numSamplesPerCycle == timeSampling.getNumStoredTimes(),
        "Number of samples given doesn't match number returned" );

    //-*************************************************************************
    // acyclic case
    if ( timePerCycle == AbcA::TimeSamplingType::AcyclicTimePerCycle()
         || numSamplesPerCycle == AbcA::TimeSamplingType::AcyclicNumSamples() )
    {
        TESTING_MESSAGE_ASSERT(
            timePerCycle == AbcA::TimeSamplingType::AcyclicTimePerCycle()
            && numSamplesPerCycle == AbcA::TimeSamplingType::AcyclicNumSamples(),
            "Given time and samples per cycle should be infinite."
                   );

        TESTING_MESSAGE_ASSERT( timeSamplingType.isAcyclic(),
                     "Time sampling should be acyclic." );
    }
    // uniform case
    else if ( numSamplesPerCycle == 1 )
    {
        TESTING_MESSAGE_ASSERT( timeSamplingType.isUniform(),
                     "Time sampling should be uniform." );
    }
    // cyclic case
    else if ( numSamplesPerCycle > 0 && timePerCycle > 0.0 )
    {
        TESTING_MESSAGE_ASSERT( timeSamplingType.isCyclic(),
                     "Time sampling should be cyclic." );
    }
    else
    {
        TESTING_MESSAGE_ASSERT( false, "Could not validate time sampling." );
    }
}

//-*****************************************************************************
template <class TIME>
void testTimeSampling( const AbcA::TimeSampling &timeSampling,
                       const AbcA::TimeSamplingType &timeSamplingType,
                       index_t numSamples )
{

    const TIME timePerCycle = timeSamplingType.getTimePerCycle();
    const index_t numSamplesPerCycle = timeSamplingType.getNumSamplesPerCycle();

    const index_t numStoredTimes = timeSampling.getNumStoredTimes();

    TESTING_MESSAGE_ASSERT( timeSamplingType.isAcyclic() ||
        numStoredTimes == numSamplesPerCycle,
        "The number of stored times in the vector of times must match the "
        "number of samples per cycle." );

    std::cout << "Here are the time sample times, contained in the "
              << "TimeSampling instance." << std::endl
              << "Only the first " << numStoredTimes << " values are stored; "
              << "the rest are computed." << std::endl << std::endl;

    for ( index_t i = 0; i < numSamples ; ++i )
    {
        std::cout << i << ": " << timeSampling.getSampleTime( i )
                  << std::endl;

        TIME timeI = timeSampling.getSampleTime( i );
        index_t floorIndex = timeSampling.getFloorIndex(
            timeI, numSamples ).first;

        index_t ceilIndex = timeSampling.getCeilIndex(
            timeI, numSamples ).first;

        index_t nearIndex = timeSampling.getNearIndex(
            timeI, numSamples ).first;

        // floor
        std::stringstream floorMsg;
        floorMsg << "Sample " << i << " is time " << timeI <<
            ", and getFloorIndex( " << timeI << " ) is " << floorIndex <<
            ". It should be " << i;
        TESTING_MESSAGE_ASSERT( floorIndex == i, floorMsg.str() );

        // ceiling
        std::stringstream ceilMsg;
        ceilMsg << "Sample " << i << " is time " << timeI <<
            ", and getCeilIndex( " << timeI << " ) is " << ceilIndex <<
            ". It should be " << i;
        TESTING_MESSAGE_ASSERT( ceilIndex == i, ceilMsg.str() );

        // nearest
        std::stringstream nearMsg;
        nearMsg << "Sample " << i << " is time " << timeI <<
            ", and getNearIndex( " << timeI << " ) is " << nearIndex <<
            ". It should be " << i;
        TESTING_MESSAGE_ASSERT( nearIndex == i, nearMsg.str() );


        chrono_t smidgeOver = 0.01;
        chrono_t smidgeUnder = 0.01;
        index_t loi = i;
        index_t hii = i;

        // base smidge off of the previous time
        if ( i > 0 )
        {
            TIME timeIm1 = timeSampling.getSampleTime( i - 1 );
            TESTING_MESSAGE_ASSERT( timeIm1 < timeI,
                         "Times should be monotonically increasing." );
            smidgeUnder = ( timeI - timeIm1 ) / 4.0;
            loi = i - 1;
        }

        // base smidge over on a future time
        if ( i < numSamples - 1 )
        {
            chrono_t timeIp1 = timeSampling.getSampleTime( i + 1 );
            smidgeOver = ( timeIp1 - timeI ) / 4.0;
            hii = i + 1;
        }

        chrono_t smidgeOverTimeI = timeI + smidgeOver;
        chrono_t smidgeUnderTimeI = timeI - smidgeUnder;

        // floor index
        TESTING_MESSAGE_ASSERT( timeSampling.getFloorIndex(
            smidgeOverTimeI, numSamples ).first == i,
            "Time a smidge over time sampled at 'i' should have a floor index "
            "of 'i'." );

        TESTING_MESSAGE_ASSERT( timeSampling.getFloorIndex(
            smidgeUnderTimeI, numSamples ).first == loi,
            "Time a smidge under time sampled at 'i' should have a floor "
            "index of 'i - 1'." );

        // ceiling index
        TESTING_MESSAGE_ASSERT( timeSampling.getCeilIndex(
            smidgeOverTimeI, numSamples ).first == hii,
            "Time a smidge over time sampled at 'i' should have a ceiling "
            "index of 'i + 1'." );

        TESTING_MESSAGE_ASSERT( timeSampling.getCeilIndex(
            smidgeUnderTimeI, numSamples ).first == i,
            "Time a smidge under time sampled at 'i' should have a ceiling "
            "index of 'i'." );

        // near index
        TESTING_MESSAGE_ASSERT( timeSampling.getNearIndex(
            smidgeOverTimeI, numSamples ).first == i,
            "Time a smidge over time sampled at 'i' should have a nearest "
            "index of 'i'." );

        TESTING_MESSAGE_ASSERT( timeSampling.getCeilIndex(
            smidgeUnderTimeI, numSamples ).first == i,
            "Time a smidge under time sampled at 'i' should have a nearest "
            "index of 'i'." );

        if ( ( timeSamplingType.isCyclic() || timeSamplingType.isUniform() )
             && i > numSamplesPerCycle )
        {
            TIME cur = timeSampling.getSampleTime( i );
            TIME prev = timeSampling.getSampleTime( i - numSamplesPerCycle );
            TESTING_MESSAGE_ASSERT( Imath::equalWithAbsError(
                (double)( cur - prev ), (double)timePerCycle, 0.00001 ),
            "Calculated time per cycle is different than given." );
        }
    }

    std::cout << std::endl << std::endl;
}

//-*****************************************************************************
template <class TIME>
void testCyclicTime1()
{
    // random weird cycle
    TimeVector tvec;
    tvec.push_back( -0.7 );
    tvec.push_back( -0.1 );
    tvec.push_back( 0.2 );

    const chrono_t timePerCycle = 1.0; // 0.1 more time than tvec[2] - tvec[0]
    const size_t numSamplesPerCycle = 3;
    const size_t numSamps = 97;

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, tvec );

    TESTING_MESSAGE_ASSERT( tSampTyp.isCyclic(), "Should be cyclic." );

    std::cout << "Testing cyclic time, 1" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle );

    testTimeSampling<TIME>( tSamp, tSampTyp, numSamps );
}

//-*****************************************************************************
template <class TIME>
void testCyclicTime2()
{
    // shutter-open, shutter-close
    TimeVector tvec;
    tvec.push_back( -1.0 / 48.0 );
    tvec.push_back( 1.0 / 48.0 );

    const chrono_t timePerCycle = 3.0 / 48.0;
    const size_t numSamplesPerCycle = 2;
    const size_t numSamps = 43;

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, tvec );

    TESTING_MESSAGE_ASSERT( tSampTyp.isCyclic(), "Should be cyclic." );

    std::cout << "Testing cyclic time, 2" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle );

    testTimeSampling<TIME>( tSamp, tSampTyp, numSamps );
}

//-*****************************************************************************
template <class TIME>
void testCyclicTime3()
{
    // shutter-open, shutter-close
    TimeVector tvec;
    tvec.push_back( 0.0 );
    tvec.push_back( 1.0 );

    const chrono_t timePerCycle = 2.0;
    const size_t numSamplesPerCycle = 2;
    const size_t numSamps = 70;

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, tvec );

    TESTING_MESSAGE_ASSERT( tSampTyp.isCyclic(), "Should be cyclic." );

    std::cout << "Testing cyclic time, 3" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle );

    testTimeSampling<TIME>( tSamp, tSampTyp, numSamps );
}

//-*****************************************************************************
template <class TIME>
void testCyclicTime4()
{
    const chrono_t startFrame = 1001.0;
    const chrono_t ftime = 1.0 / 24.0;
    const chrono_t timePerCycle = ftime;
    const size_t numSamplesPerCycle = 3;
    const size_t numSamps = 20;

    const chrono_t first = ( startFrame * ftime ) - ( ftime / 4.0 );
    const chrono_t second = startFrame * ftime;
    const chrono_t third = ( startFrame * ftime ) + ( ftime / 4.0 );

    TimeVector tvec;
    tvec.push_back( first );
    tvec.push_back( second );
    tvec.push_back( third );

    const AbcA::TimeSamplingType tst( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tsamp( tst, tvec );

    TESTING_MESSAGE_ASSERT( tst.isCyclic(), "Should be cyclic." );

    std::cout << "Testing cyclic time, 4" << std::endl;

    validateTimeSampling( tsamp, tst, tvec, numSamplesPerCycle,
                          timePerCycle );

    testTimeSampling<TIME>( tsamp, tst, numSamps );
}

//-*****************************************************************************
template <class TIME>
void testUniformTime1()
{
    // sample once at each frame
    TimeVector tvec;
    tvec.push_back( 0.0 );

    const chrono_t timePerCycle = 1.0 / 24.0;
    const size_t numSamplesPerCycle = 1;
    const size_t numSamps = 43;

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, tvec );

    TESTING_MESSAGE_ASSERT( tSampTyp.isUniform(), "Should be uniform." );

    std::cout << "Testing uniform time, 1" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle );

    testTimeSampling<TIME>( tSamp, tSampTyp, numSamps );
}

//-*****************************************************************************
template <class TIME>
void testUniformTime2()
{
    // sample once at each frame, starting at -10th frame
    TimeVector tvec;
    tvec.push_back( -10.0 / 24.0 );

    const chrono_t timePerCycle = 1.0 / 24.0;
    const size_t numSamplesPerCycle = 1;
    const size_t numSamps = 57;

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, tvec );

    TESTING_MESSAGE_ASSERT( tSampTyp.isUniform(), "Should be uniform." );

    std::cout << "Testing uniform time, 2" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle );

    testTimeSampling<TIME>( tSamp, tSampTyp, numSamps );
}
//-*****************************************************************************
template <class TIME>
void testUniformTime3()
{
    // sample once at each frame, starting at +1th frame
    TimeVector tvec;
    tvec.push_back( 2.0 / 24.0 );

    const chrono_t timePerCycle = 1.0 / 24.0;
    const size_t numSamplesPerCycle = 1;
    const size_t numSamps = 49;

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, tvec );

    TESTING_MESSAGE_ASSERT( tSampTyp.isUniform(), "Should be uniform." );

    std::cout << "Testing uniform time, 3" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle );

    testTimeSampling<TIME>( tSamp, tSampTyp, numSamps );
}

//-*****************************************************************************
template <class TIME>
void testDefaultTime1()
{
    // sample once at each frame
    TimeVector tvec;

    AbcA::TimeSamplingType tSampType;
    AbcA::TimeSampling tSamp;
    tvec.push_back( 0.0 );

    TESTING_MESSAGE_ASSERT( tSamp.getTimeSamplingType().isUniform(),
        "Should be uniform." );

    std::cout << "Testing default, 1" << std::endl;
    validateTimeSampling( tSamp, tSampType, tvec, 1,
                          1 );

    testTimeSampling<TIME>( tSamp, tSampType, 100 );
}

//-*****************************************************************************
template <class TIME>
void testAcyclicTime1()
{
    TimeVector tvec;

    // construct with explicit values
    const chrono_t timePerCycle = AbcA::TimeSamplingType::AcyclicTimePerCycle();
    const size_t numSamplesPerCycle =
        AbcA::TimeSamplingType::AcyclicNumSamples();
    const size_t numSamps = 44;

    for ( size_t i = 0 ; i < numSamps ; ++i )
    {
        // sample once at each frame, starting at +0th frame
        tvec.push_back( (chrono_t)i * ( 1.0 / 24.0 ) );
    }

    const AbcA::TimeSamplingType tSampTyp( AbcA::TimeSamplingType::kAcyclic );
    const AbcA::TimeSampling tSamp( tSampTyp, tvec );

    TESTING_MESSAGE_ASSERT( tSampTyp.isAcyclic(), "Should be acyclic." );

    std::cout << "Testing acyclic time, 1" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle );

    testTimeSampling<TIME>( tSamp, tSampTyp, numSamps );
}

//-*****************************************************************************
template <class TIME>
void testAcyclicTime2()
{
    TimeVector tvec;

    // construct with explicit values
    const chrono_t timePerCycle = AbcA::TimeSamplingType::AcyclicTimePerCycle();
    const size_t numSamplesPerCycle =
        AbcA::TimeSamplingType::AcyclicNumSamples();
    const size_t numSamps = 44;

    chrono_t ranTime = 0.0;
    Imath::srand48( numSamps );

    for ( size_t i = 0 ; i < numSamps ; ++i )
    {
        // sample randomly
        ranTime += Imath::drand48();
        tvec.push_back( ranTime );
    }

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, tvec );

    TESTING_MESSAGE_ASSERT( tSampTyp.isAcyclic(), "Should be acyclic." );

    std::cout << "Testing acyclic time, 2" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle );

    testTimeSampling<TIME>( tSamp, tSampTyp, numSamps );
}

//-*****************************************************************************
template <class TIME>
void testAcyclicTime3()
{
    TimeVector tvec;

    // construct with acyclic enum
    const chrono_t timePerCycle = AbcA::TimeSamplingType::AcyclicTimePerCycle();
    const size_t numSamplesPerCycle =
        AbcA::TimeSamplingType::AcyclicNumSamples();
    const size_t numSamps = 79;

    chrono_t ranTime = 0.0;
    Imath::srand48( numSamps );

    for ( size_t i = 0 ; i < numSamps ; ++i )
    {
        // sample randomly
        ranTime += Imath::drand48() * (chrono_t)i;
        tvec.push_back( ranTime );
    }

    AbcA::TimeSamplingType::AcyclicFlag acf = AbcA::TimeSamplingType::kAcyclic;

    const AbcA::TimeSamplingType tSampTyp( acf );
    const AbcA::TimeSampling tSamp( tSampTyp, tvec );

    TESTING_MESSAGE_ASSERT( tSampTyp.isAcyclic(), "Should be acyclic." );

    std::cout << "Testing acyclic time, 3" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle );

    testTimeSampling<TIME>( tSamp, tSampTyp, numSamps );
}

//-*****************************************************************************
void testBadTypes()
{
    AbcA::TimeSamplingType t;
    TESTING_ASSERT_THROW( t =
        AbcA::TimeSamplingType(1, AbcA::TimeSamplingType::AcyclicTimePerCycle()),
        Alembic::Util::Exception);

    TESTING_ASSERT_THROW(t = AbcA::TimeSamplingType(1, 0.0),
        Alembic::Util::Exception);

    TESTING_ASSERT_THROW(t = AbcA::TimeSamplingType(0, 1.0),
        Alembic::Util::Exception);

    TESTING_ASSERT_THROW(t = AbcA::TimeSamplingType(0, 0.0),
        Alembic::Util::Exception);

    TESTING_ASSERT_THROW(t = AbcA::TimeSamplingType(
                             AbcA::TimeSamplingType::AcyclicNumSamples(), 3.0),
        Alembic::Util::Exception);

    TESTING_ASSERT_THROW(t = AbcA::TimeSamplingType(0.0),
        Alembic::Util::Exception);

    chrono_t val = AbcA::TimeSamplingType::AcyclicTimePerCycle();

    TESTING_ASSERT_THROW(t = AbcA::TimeSamplingType(val),
        Alembic::Util::Exception);
}

//-*****************************************************************************
int main( int, char** )
{
    // make sure somebody didn't mess up the ALEMBIC_LIBRARY_VERSION
    TESTING_ASSERT(ALEMBIC_LIBRARY_VERSION > 9999 &&
        ALEMBIC_LIBRARY_VERSION < 999999);

    // cyclic is trickiest
    testCyclicTime1<chrono_t>();
    testCyclicTime2<chrono_t>();
    testCyclicTime3<chrono_t>();
    testCyclicTime4<chrono_t>();

    // uniform is probably most common
    testUniformTime1<chrono_t>();
    testUniformTime2<chrono_t>();
    testUniformTime3<chrono_t>();

    // default, uniform sampling
    testDefaultTime1<chrono_t>();

    // acyclic is pretty easy, too
    testAcyclicTime1<chrono_t>();
    testAcyclicTime2<chrono_t>();
    testAcyclicTime3<chrono_t>();

    // test with doubles
    testCyclicTime1<double>();
    testUniformTime1<double>();
    testAcyclicTime1<double>();

    // test with floats
    testCyclicTime1<float>();
    testUniformTime1<float>();
    testAcyclicTime1<float>();

    // make sure these bad types throw
    testBadTypes();

    return 0;
}
