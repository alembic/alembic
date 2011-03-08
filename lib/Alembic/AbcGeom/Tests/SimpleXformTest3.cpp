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

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include "Assert.h"

#include <ImathMath.h>

#include <limits>

#include <iostream>

using namespace Alembic::AbcGeom;

static const double VAL_EPSILON = std::numeric_limits<double>::epsilon() \
    * 1024.0;

//-*****************************************************************************
void writeArchive( const std::string& iName )
{
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(), iName );

    OSimpleXform root( archive.getTop(), "root" );

    SimpleXformSample sample;

    chrono_t frameInterval = 1.0/24.0;

    TimeSamplingType ts( frameInterval );

    double startFrame = 1001.0;

    //a is animated on whole frames
    OSimpleXform a( root, "a", ts );
    for ( int i = 0; i < 20; ++i )
    {
        chrono_t sampleTime = frameInterval * startFrame + ( i * frameInterval );
        sample.reset();
        V3d trans( i + 1.0, i * 2.0, sampleTime );

        if ( i == 2 )
        { sample.setScale( trans ); }

        sample.setTranslation( trans );
        a.getSchema().set( sample, OSampleSelector( i, sampleTime ) );
    }

    // set one more sample, rotation
    sample.setXYZRotation( V3d( 10.0, 2.0, 1.0 ) );

    a.getSchema().set( sample,
                       OSampleSelector( 20, ( frameInterval * startFrame +
                                              ( 20 * frameInterval ) ) ) );

    a.getSchema().setFromPrevious( OSampleSelector(
                                       21,
                                       ( frameInterval * startFrame +
                                         ( 21 * frameInterval ) ) ) );

    sample.reset();
    sample.setShear( V3d( -5.0, 2.8, 12.0 ) );

    a.getSchema().set( sample,
                       OSampleSelector( 22, ( frameInterval * startFrame +
                                              ( 22 * frameInterval ) ) ) );

    a.getSchema().setFromPrevious( OSampleSelector(
                                       23,
                                       ( frameInterval * startFrame +
                                         ( 23 * frameInterval ) ) ) );

    sample.setShear( V3d( 2.0, 1.4, 0.0 ) );
    a.getSchema().set( sample,
                       OSampleSelector( 24, ( frameInterval * startFrame +
                                              ( 24 * frameInterval ) ) ) );


    std::cout << std::endl;
}

//-*****************************************************************************
void readArchive( const std::string &iName )
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), iName );

    chrono_t startFrame = 1001.0;
    chrono_t frameInterval = 1.0 / 24.0;

    IObject root( archive.getTop(), "root" );

    ISimpleXform a( root, "a" );

    TimeSamplingType tst = a.getSchema().getTimeSampling().getTimeSamplingType();

    TESTING_ASSERT( tst.isUniform() );

    for ( size_t i = 0 ; i < a.getSchema().getNumSamples() ; i++ )
    {
        chrono_t sampleTime = frameInterval * startFrame + ( i * frameInterval );
        chrono_t t = a.getSchema().getTimeSampling().getSampleTime( i );
        TESTING_ASSERT( Imath::equalWithAbsError( t, sampleTime, 0.00001 ) );

        std::cout << "time " << t << " at index " << i
                  << ":\n" << a.getSchema().getValue( i ) << std::endl;
    }
}

//-*****************************************************************************
int main(int argc, char **argv)
{
    std::string arkive( "simpleXformTest3.abc" );

    writeArchive( arkive );
    readArchive( arkive );

    // fail this test manually until it is correct
    return 0;
}
