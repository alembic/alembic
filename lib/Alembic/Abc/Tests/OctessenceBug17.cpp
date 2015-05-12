//-*****************************************************************************
//
// Copyright (c) 2009-2015,
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

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreOgawa/All.h>

#include <stdlib.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

namespace Abc = Alembic::Abc;
using namespace Abc;

//-*****************************************************************************
static const chrono_t g_doubleStartTime = 0.1;
static const chrono_t g_dt = 1.0 / 24.0;

static const size_t g_numDoubleSamps = 5;

//-*****************************************************************************
void scramble_heap()
{
    const size_t BUFFSIZE = 8192 / 4;
    char *buf_ptrs[BUFFSIZE];

    for ( size_t i = 0 ; i < BUFFSIZE ; i++ )
    {
        size_t alloc_size = 4 * i + 1;
        buf_ptrs[i] = (char *)malloc( alloc_size );
        memset( buf_ptrs[i], 0xff, alloc_size );
    }

    for ( size_t i = 0 ; i < BUFFSIZE ; i++ )
    {
        free( buf_ptrs[i] );
    }
}

//-*****************************************************************************
void simpleTestOut( const std::string &iArchiveName )
{
    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(),
                      iArchiveName );
    OObject archiveTop = archive.getTop();

    std::vector < chrono_t > timeSamps(g_numDoubleSamps);

    TimeSamplingType utst( g_dt  ); // uniform time sampling
    TimeSamplingPtr uts( new TimeSampling(g_dt, g_doubleStartTime) );

    TimeSamplingType atst( TimeSamplingType::kAcyclic ); // acyclic
    chrono_t t = g_doubleStartTime;
    for ( size_t j = 0 ; j < g_numDoubleSamps ; j++ )
    {
        timeSamps[j] = t;
        t += g_dt;
    }
    TimeSamplingPtr ats( new TimeSampling(atst, timeSamps) );

    OObject c0( archiveTop, "c0" );
    OCompoundProperty c0Props = c0.getProperties();
    ODoubleProperty dp0( c0Props, "uniformdoubleprop", uts );
    ODoubleProperty dp1( c0Props, "acyclicdoubleprop", ats );

    t = g_doubleStartTime;

    for ( size_t j = 0 ; j < g_numDoubleSamps ; j++ )
    {
        dp0.set( 1.0 + 0.1 * j );

        dp1.set( 1.0 + 0.1 * j );

        t += g_dt;
    }

    TimeSamplingType _utst = dp0.getTimeSampling()->getTimeSamplingType();
    TESTING_ASSERT( _utst.isUniform() );
}

//-*****************************************************************************
void simpleTestIn( const std::string &iArchiveName )
{
    IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(),
                      iArchiveName, ErrorHandler::kThrowPolicy );

    IObject archiveTop = archive.getTop();
    IObject c0( archiveTop, "c0" );
    ICompoundProperty c0Props = c0.getProperties();

    TimeSamplingPtr uts = c0Props.getPtr()->
        getScalarProperty( "uniformdoubleprop" )->getTimeSampling();

    TimeSamplingPtr ats = c0Props.getPtr()->
        getScalarProperty( "acyclicdoubleprop" )->getTimeSampling();

    //IDoubleProperty dp0( c0Props, "uniformdoubleprop" );
    //IDoubleProperty dp1( c0Props, "acyclicdoubleprop" );
    //size_t numReadDoubleSamps = dp0.getNumSamples();
    //TESTING_ASSERT( numReadDoubleSamps == g_numDoubleSamps );
    //TESTING_ASSERT( dp1.getNumSamples() == numReadDoubleSamps );


    //scramble_heap();

    //const TimeSamplingType &utst = uts.getTimeSamplingType();
    const TimeSamplingType utst = c0Props.getPtr()->getScalarProperty(
        "uniformdoubleprop" )->getTimeSampling()->getTimeSamplingType();
    TESTING_ASSERT(utst.isUniform());

    for ( size_t j = 0 ; j < g_numDoubleSamps ; j++ )
    {
        chrono_t utime = uts->getSampleTime( j );
        chrono_t atime = ats->getSampleTime( j );

        chrono_t reftime = g_doubleStartTime + ( j * g_dt );

        std::cout << "reference time: " << reftime << std::endl;

        std::cout << "uniform sample time: " << utime << std::endl;

        std::cout << "acyclic sample time: " << atime << std::endl;

        chrono_t ABSERROR = 0.00001;

        TESTING_ASSERT( Imath::equalWithAbsError( utime, reftime, ABSERROR ) );
        TESTING_ASSERT( Imath::equalWithAbsError( atime, reftime, ABSERROR ) );
        TESTING_ASSERT( Imath::equalWithAbsError( atime, utime, ABSERROR ) );
    }
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    const std::string arkive( "octessence17.abc" );

    simpleTestOut( arkive );
    simpleTestIn( arkive );

    return 0;
}
