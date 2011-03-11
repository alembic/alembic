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

#include <Alembic/AbcCoreAbstract/All.h>

#include "Assert.h"

#include <float.h>
#include <stdlib.h>

#include <ImathMath.h>

#include <vector>
#include <iostream>
#include <boost/format.hpp>
#include <boost/preprocessor/stringize.hpp>

//-*****************************************************************************
namespace AbcA = Alembic::AbcCoreAbstract::v1;
using AbcA::chrono_t;

typedef std::vector<chrono_t> TimeVector;

//-*****************************************************************************
AbcA::ArraySamplePtr
buildTimeSamplesPtr( TimeVector &tvec )
{
    AbcA::ArraySamplePtr time_samples(
        new AbcA::ArraySample(
            reinterpret_cast<const void *>( &tvec.front() ),
            AbcA::DataType( AbcA::kChrono_TPOD , 1 ),
            Alembic::Util::Dimensions( tvec.size () ) ) );

    return time_samples;
}

//-*****************************************************************************
void validateTimeSampling( const AbcA::TimeSampling &timeSampling,
                           const AbcA::TimeSamplingType &timeSamplingType,
                           const TimeVector &timeVector,
                           const size_t numSamplesPerCycle,
                           const chrono_t timePerCycle,
                           const size_t numSamps )
{
    const chrono_t period = timeSamplingType.getTimePerCycle();

    if ( timePerCycle > 0.0 &&
         timePerCycle < AbcA::TimeSamplingType::AcyclicTimePerCycle() )
    {
        TESTING_MESSAGE_ASSERT( period == timePerCycle,
                     "calculated cycle period does not match given time/cycle" );
    }

    std::cout << "***********************************************************"
              << std::endl;

    if ( timeSamplingType.isIdentity() )
    { std::cout << "time sampling type is identity" << std::endl; }
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

    const size_t numSamples = timeSampling.getNumSamples();
    TESTING_MESSAGE_ASSERT(
        numSamps == numSamples,
        "Number of samples given does not match number of samples returned"
               );

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
    // static case
    else if ( numSamplesPerCycle == 0 || timePerCycle == 0.0 )
    {
        TESTING_MESSAGE_ASSERT(
            numSamplesPerCycle == 0 && timePerCycle == 0.0,
            "Given number of samples and given time per cycle should be zero."
                   );

        TESTING_MESSAGE_ASSERT( timeSamplingType.isIdentity(),
                     "Time sampling should be identity." );
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
void testTimeSampling( const AbcA::TimeSampling &timeSampling,
                       const AbcA::TimeSamplingType &timeSamplingType )
{
    const size_t numSamples = timeSampling.getNumSamples();
    const size_t lastIndex = numSamples - 1;
    const chrono_t minTime = timeSampling.getSampleTime( 0 );
    const chrono_t maxTime = timeSampling.getSampleTime( lastIndex );

    const chrono_t timePerCycle = timeSamplingType.getTimePerCycle();
    const size_t numSamplesPerCycle = timeSamplingType.getNumSamplesPerCycle();

    const size_t numStoredTimes = timeSampling.getNumStoredTimes();

    std::cout << "Here are the time sample times, contained in the "
              << "TimeSampling instance." << std::endl
              << "Only the first " << numStoredTimes << " values are stored; "
              << "the rest are computed." << std::endl << std::endl;

    for ( size_t i = 0; i < numSamples ; ++i )
    {
        std::cout << i << ": " << timeSampling.getSampleTime( i )
                  << std::endl;

        chrono_t timeI = timeSampling.getSampleTime( i );
        size_t floorIndex = timeSampling.getFloorIndex(
            timeI ).first;

        size_t ceilIndex = timeSampling.getCeilIndex(
            timeI ).first;

        size_t nearIndex = timeSampling.getNearIndex( timeI ).first;

        // floor
        TESTING_MESSAGE_ASSERT(
            floorIndex == i,
            ( boost::format( "Sample %d is time %d, and "
                             "getFloorIndex( %d ) is %d. It should be %d" )
              % i % timeI % timeI % floorIndex % i )
                   );

        // ceiling
        TESTING_MESSAGE_ASSERT(
            ceilIndex == i,
            ( boost::format( "Sample %d is time %d, and "
                             "getCeilIndex( %d ) is %d. It should be %d" )
              % i % timeI % timeI % ceilIndex % i )
                   );

        // nearest
        TESTING_MESSAGE_ASSERT(
            nearIndex == i,
            ( boost::format( "Sample %d is time %d, and "
                             "getNearIndex( %d ) is %d. It should be %d" )
              % i % timeI % timeI % nearIndex % i )
                   );

        if ( i > 0 )
        {
            chrono_t timeIm1 = timeSampling.getSampleTime( i - 1 );
            TESTING_MESSAGE_ASSERT( timeIm1 < timeI,
                         "Times should be monotonically increasing." );

            if ( i < lastIndex )
            {
                chrono_t timeIp1 = timeSampling.getSampleTime( i + 1 );
                chrono_t smidgeOver = ( timeIp1 - timeI ) / 4.0;
                chrono_t smidgeUnder = ( timeI - timeIm1 ) / 4.0;
                chrono_t smidgeOverTimeI = timeI + smidgeOver;
                chrono_t smidgeUnderTimeI = timeI - smidgeUnder;

                // floor index
                TESTING_MESSAGE_ASSERT(
                    timeSampling.getFloorIndex( smidgeOverTimeI ).first == i,
                    "Time a smidge over time sampled at 'i' should have a floor "
                    "index of 'i'."
                           );

                TESTING_MESSAGE_ASSERT(
                    timeSampling.getFloorIndex( smidgeUnderTimeI ).first == i - 1,
                    "Time a smidge under time sampled at 'i' should have a "
                    "floor index of 'i - 1'."
                           );

                // ceiling index
                TESTING_MESSAGE_ASSERT(
                    timeSampling.getCeilIndex( smidgeOverTimeI ).first == i + 1,
                    "Time a smidge over time sampled at 'i' should have a "
                    "ceiling index of 'i + 1'."
                           );

                TESTING_MESSAGE_ASSERT(
                    timeSampling.getCeilIndex( smidgeUnderTimeI ).first == i,
                    "Time a smidge under time sampled at 'i' should have a "
                    "ceiling index of 'i'."
                           );

                // near index
                TESTING_MESSAGE_ASSERT(
                    timeSampling.getNearIndex( smidgeOverTimeI ).first == i,
                    "Time a smidge over time sampled at 'i' should have a "
                    "nearest index of 'i'."
                           );

                TESTING_MESSAGE_ASSERT(
                    timeSampling.getCeilIndex( smidgeUnderTimeI ).first == i,
                    "Time a smidge under time sampled at 'i' should have a "
                    "nearest index of 'i'."
                           );
            }
        }

        if ( ( timeSamplingType.isCyclic() || timeSamplingType.isUniform() )
             && i > numSamplesPerCycle )
        {
            chrono_t cur = timeSampling.getSampleTime( i );
            chrono_t prev = timeSampling.getSampleTime( i - numSamplesPerCycle );
            TESTING_MESSAGE_ASSERT( Imath::equalWithAbsError( ( cur - prev ), timePerCycle,
                                                   0.00001 ),
                         "Calculated time per cycle is different than given." );
        }
    }

    std::cout << std::endl << std::endl;
}

//-*****************************************************************************
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

    AbcA::ArraySamplePtr rotPtr = buildTimeSamplesPtr( tvec );

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, numSamps, rotPtr );

    TESTING_MESSAGE_ASSERT( tSampTyp.isCyclic(), "Should be cyclic." );

    std::cout << "Testing cyclic time, 1" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tSamp, tSampTyp );
}

//-*****************************************************************************
void testCyclicTime2()
{
    // shutter-open, shutter-close
    TimeVector tvec;
    tvec.push_back( -1.0 / 48.0 );
    tvec.push_back( 1.0 / 48.0 );

    const chrono_t timePerCycle = 3.0 / 48.0;
    const size_t numSamplesPerCycle = 2;
    const size_t numSamps = 43;

    AbcA::ArraySamplePtr rotPtr = buildTimeSamplesPtr( tvec );

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, numSamps, rotPtr );

    TESTING_MESSAGE_ASSERT( tSampTyp.isCyclic(), "Should be cyclic." );

    std::cout << "Testing cyclic time, 2" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tSamp, tSampTyp );
}

//-*****************************************************************************
void testCyclicTime3()
{
    // shutter-open, shutter-close
    TimeVector tvec;
    tvec.push_back( 0.0 );
    tvec.push_back( 1.0 );

    const chrono_t timePerCycle = 2.0;
    const size_t numSamplesPerCycle = 2;
    const size_t numSamps = 70;

    AbcA::ArraySamplePtr rotPtr = buildTimeSamplesPtr( tvec );

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, numSamps, rotPtr );

    TESTING_MESSAGE_ASSERT( tSampTyp.isCyclic(), "Should be cyclic." );

    std::cout << "Testing cyclic time, 3" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tSamp, tSampTyp );
}

//-*****************************************************************************
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

    AbcA::ArraySamplePtr tptr = buildTimeSamplesPtr( tvec );

    const AbcA::TimeSamplingType tst( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tsamp( tst, numSamps, tptr );

    TESTING_MESSAGE_ASSERT( tst.isCyclic(), "Should be cyclic." );

    std::cout << "Testing cyclic time, 4" << std::endl;

    validateTimeSampling( tsamp, tst, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tsamp, tst );
}

//-*****************************************************************************
void testUniformTime1()
{
    // sample once at each frame
    TimeVector tvec;
    tvec.push_back( 0.0 );

    const chrono_t timePerCycle = 1.0 / 24.0;
    const size_t numSamplesPerCycle = 1;
    const size_t numSamps = 43;

    AbcA::ArraySamplePtr rotPtr = buildTimeSamplesPtr( tvec );

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, numSamps, rotPtr );

    TESTING_MESSAGE_ASSERT( tSampTyp.isUniform(), "Should be uniform." );

    std::cout << "Testing uniform time, 1" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tSamp, tSampTyp );
}

//-*****************************************************************************
void testUniformTime2()
{
    // sample once at each frame, starting at -10th frame
    TimeVector tvec;
    tvec.push_back( -10.0 / 24.0 );

    const chrono_t timePerCycle = 1.0 / 24.0;
    const size_t numSamplesPerCycle = 1;
    const size_t numSamps = 57;

    AbcA::ArraySamplePtr rotPtr = buildTimeSamplesPtr( tvec );

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, numSamps, rotPtr );

    TESTING_MESSAGE_ASSERT( tSampTyp.isUniform(), "Should be uniform." );

    std::cout << "Testing uniform time, 2" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tSamp, tSampTyp );
}
//-*****************************************************************************
void testUniformTime3()
{
    // sample once at each frame, starting at +1th frame
    TimeVector tvec;
    tvec.push_back( 2.0 / 24.0 );

    const chrono_t timePerCycle = 1.0 / 24.0;
    const size_t numSamplesPerCycle = 1;
    const size_t numSamps = 49;

    AbcA::ArraySamplePtr rotPtr = buildTimeSamplesPtr( tvec );

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, numSamps, rotPtr );

    TESTING_MESSAGE_ASSERT( tSampTyp.isUniform(), "Should be uniform." );

    std::cout << "Testing uniform time, 3" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tSamp, tSampTyp );
}

//-*****************************************************************************
void testIdentityTime1()
{
    // sample once at each frame
    TimeVector tvec;

    tvec.push_back( 0.0 );

    const chrono_t timePerCycle = 0.0;
    const size_t numSamplesPerCycle = 0;
    const size_t numSamps = 43;

    Alembic::Util::Dimensions dims;
    dims.setRank( 1 );

    // Make a null ArraySamplePtr by hand
    AbcA::ArraySamplePtr rotPtr = AbcA::ArraySamplePtr(
        new AbcA::ArraySample( (const void *)NULL,
                               AbcA::DataType( AbcA::kChrono_TPOD, 1 ),
                               dims ) );


    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, numSamps, rotPtr );

    TESTING_MESSAGE_ASSERT( tSampTyp.isIdentity(), "Should be identity." );

    std::cout << "Testing identity time, 1" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tSamp, tSampTyp );
}

//-*****************************************************************************
void testIdentityTime2()
{
    const chrono_t timePerCycle = 0.0;
    const size_t numSamplesPerCycle = 0;
    const size_t numSamps = 5;

    TimeVector tvec;
    tvec.push_back( 0.0 );

    Alembic::Util::Dimensions dims;
    dims.setRank( 1 );

    // Make a null ArraySamplePtr by hand
    AbcA::ArraySamplePtr rotPtr = AbcA::ArraySamplePtr(
        new AbcA::ArraySample( (const void *)NULL,
                               AbcA::DataType( AbcA::kChrono_TPOD, 1 ),
                               dims ) );

    // default TimeSamplingType
    const AbcA::TimeSamplingType tSampTyp;
    const AbcA::TimeSampling tSamp( tSampTyp, numSamps, rotPtr );

    TESTING_MESSAGE_ASSERT( tSampTyp.isIdentity(), "Should be identity." );

    std::cout << "Testing identity time, 2" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tSamp, tSampTyp );
}

//-*****************************************************************************
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

    AbcA::ArraySamplePtr rotPtr = buildTimeSamplesPtr( tvec );

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, numSamps, rotPtr );

    TESTING_MESSAGE_ASSERT( tSampTyp.isAcyclic(), "Should be acyclic." );

    std::cout << "Testing acyclic time, 1" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tSamp, tSampTyp );
}

//-*****************************************************************************
void testAcyclicTime2()
{
    TimeVector tvec;

    // construct with explicit values
    const chrono_t timePerCycle = AbcA::TimeSamplingType::AcyclicTimePerCycle();
    const size_t numSamplesPerCycle =
        AbcA::TimeSamplingType::AcyclicNumSamples();
    const size_t numSamps = 44;

    chrono_t ranTime = 0.0;
    srand48( numSamps );

    for ( size_t i = 0 ; i < numSamps ; ++i )
    {
        // sample randomly
        ranTime += drand48();
        tvec.push_back( ranTime );
    }

    AbcA::ArraySamplePtr rotPtr = buildTimeSamplesPtr( tvec );

    const AbcA::TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    const AbcA::TimeSampling tSamp( tSampTyp, numSamps, rotPtr );

    TESTING_MESSAGE_ASSERT( tSampTyp.isAcyclic(), "Should be acyclic." );

    std::cout << "Testing acyclic time, 2" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tSamp, tSampTyp );
}

//-*****************************************************************************
void testAcyclicTime3()
{
    TimeVector tvec;

    // construct with acyclic enum
    const chrono_t timePerCycle = AbcA::TimeSamplingType::AcyclicTimePerCycle();
    const size_t numSamplesPerCycle =
        AbcA::TimeSamplingType::AcyclicNumSamples();
    const size_t numSamps = 79;

    chrono_t ranTime = 0.0;
    srand48( numSamps );

    for ( size_t i = 0 ; i < numSamps ; ++i )
    {
        // sample randomly
        ranTime += drand48() * (chrono_t)i;
        tvec.push_back( ranTime );
    }

    AbcA::ArraySamplePtr rotPtr = buildTimeSamplesPtr( tvec );

    AbcA::TimeSamplingType::AcyclicFlag acf;

    const AbcA::TimeSamplingType tSampTyp( acf );
    const AbcA::TimeSampling tSamp( tSampTyp, numSamps, rotPtr );

    TESTING_MESSAGE_ASSERT( tSampTyp.isAcyclic(), "Should be acyclic." );

    std::cout << "Testing acyclic time, 3" << std::endl;
    validateTimeSampling( tSamp, tSampTyp, tvec, numSamplesPerCycle,
                          timePerCycle, numSamps );

    testTimeSampling( tSamp, tSampTyp );
}

//-*****************************************************************************
void testBadTypes()
{
    AbcA::TimeSamplingType t;
    TESTING_ASSERT_THROW( t =
                          AbcA::TimeSamplingType(1,
                                                 AbcA::TimeSamplingType::AcyclicTimePerCycle()),
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
    // cyclic is trickiest
    testCyclicTime1();
    testCyclicTime2();
    testCyclicTime3();
    testCyclicTime4();

    // uniform is probably most common
    testUniformTime1();
    testUniformTime2();
    testUniformTime3();

    // identity; this is the default TimeSamplingType
    testIdentityTime1();
    testIdentityTime2();

    // acyclic is pretty easy, too
    testAcyclicTime1();
    testAcyclicTime2();
    testAcyclicTime3();

    // make sure these bad types throw
    testBadTypes();

    return 0;
}
