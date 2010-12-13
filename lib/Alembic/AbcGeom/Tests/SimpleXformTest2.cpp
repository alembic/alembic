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

void castTest( const OObject& foo )
{
    std::cout << "foo is a const reference named " << foo.getName()
              << std::endl;
}

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
        std::cout << "a's " << i << "th written sample is at " << sampleTime << std::endl;
        sample.makeIdentity();
        V3d trans( i + 1.0, i * 2.0, sampleTime );
        std::cout << "with the translation value " << trans << std::endl
                  << std::endl;
        sample.setTranslation( trans );
        a.getSchema().set( sample, OSampleSelector( i, sampleTime ) );
    }

    std::cout << std::endl;

    castTest( a );

    //b is static
    OSimpleXform b( root, "b" );
    sample.makeIdentity();
    sample.setTranslation( V3d( 5, 10, 15 ) );
    b.getSchema().set( sample );

    {
        //c has cyclic sampling at -0.25 0 0.25
        OSimpleXform c( root, "c",
                        AbcA::TimeSamplingType( 3, frameInterval ) );

        chrono_t sampleTime = frameInterval * -0.25;

        sampleTime += frameInterval*startFrame;

        for ( int i = 0 ; i < 5 ; ++i )
        {
            for (int j = 0; j < 3; ++j)
            {
                sample.makeIdentity();

                sample.setTranslation(
                    V3d( sampleTime * 24.0,
                         ( j == 1 ? 1 : 0 ), i * 100 + j ) );

                std::cout << "c's " << i * 3 + j << "th written sample is at "
                          << sampleTime << std::endl;
                c.getSchema().set(sample, OSampleSelector( i * 3 + j,
                                                           sampleTime ) );

                std::cout << "with the translation value "
                          << sample.getTranslation() << std::endl << std::endl;

                sampleTime += frameInterval * 0.25;
            }

            sampleTime += frameInterval * 0.25;
        }
    }
}

//-*****************************************************************************
void readArchive( const std::string &iName )
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), iName );

    chrono_t startFrame = 1001.0;
    chrono_t frameInterval = 1.0 / 24.0;

    IObject root( archive.getTop(), "root" );

    IObject ao( root, "a" );

    TESTING_ASSERT( ISimpleXform::matches( ao.getHeader() ) );

    ISimpleXform aowrapped( ao, kWrapExisting );

    ISimpleXform c( root, "c" );
    ISimpleXform a( root, "a" );

    std::cout << "a has " << a.getSchema().getNumSamples()
              << " samples." << std::endl;

    TESTING_ASSERT( aowrapped.getSchema().getNumSamples() ==
                    a.getSchema().getNumSamples() );

    std::cout << std::endl << "aowrapped has " << aowrapped.getSchema().getNumSamples()
              << " samples." << std::endl << std::endl;

    TimeSamplingType tst = a.getSchema().getTimeSampling().getTimeSamplingType();

    TESTING_ASSERT( tst.isUniform() );

    for ( size_t i = 0 ; i < a.getSchema().getNumSamples() ; i++ )
    {
        chrono_t sampleTime = frameInterval * startFrame + ( i * frameInterval );
        chrono_t t = a.getSchema().getTimeSampling().getSampleTime( i );
        TESTING_ASSERT( Imath::equalWithAbsError( t, sampleTime, 0.00001 ) );

        V3d shouldBeTrans( i + 1.0, i * 2.0, sampleTime );
        V3d trans = a.getSchema().getValue( i ).getTranslation();

        TESTING_ASSERT( trans == shouldBeTrans );

        TESTING_ASSERT( trans ==
                        aowrapped.getSchema().getValue( i ).getTranslation() );

        std::cout << "a's " << i << "th sample has translation " << trans
                  << " at time " << t << std::endl
                  << std::endl;
    }

    {
        chrono_t sampleTime = frameInterval * -0.25;

        sampleTime += frameInterval * startFrame;

        TESTING_ASSERT( c.getSchema().getNumSamples() == 15 );

        for ( size_t i = 0 ; i < 5 ; i++ )
        {
            for ( size_t j = 0 ; j < 3 ; j++ )
            {
                V3d shouldBeTrans( sampleTime * 24.0,
                                   ( j == 1 ? 1.0 : 0.0 ),
                                   i * 100.0 + j );

                size_t idx = i * 3 + j;
                V3d trans = c.getSchema().getValue( idx ).getTranslation();

                std::cerr << "trans: " << trans << ", should be: "
                          << shouldBeTrans << std::endl;

                TESTING_ASSERT( Imath::equalWithAbsError(
                                    trans.x, shouldBeTrans.x, VAL_EPSILON )
                                && Imath::equalWithAbsError(
                                    trans.y, shouldBeTrans.y, VAL_EPSILON )
                                && Imath::equalWithAbsError(
                                    trans.z, shouldBeTrans.z, VAL_EPSILON ) );

                std::cout << "c's " << idx << "th sample has translation "
                          << trans << " at time " << sampleTime
                          << std::endl << std::endl;

                sampleTime += frameInterval * 0.25;
            }
            sampleTime += frameInterval * 0.25;
        }
    }
}

//-*****************************************************************************
int main(int argc, char **argv)
{
    std::string arkive( "simpleXformTest2.abc" );

    writeArchive( arkive );
    readArchive( arkive );

    // fail this test manually until it is correct
    return 0;
}
