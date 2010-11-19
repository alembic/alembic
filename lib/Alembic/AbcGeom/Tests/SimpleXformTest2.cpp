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

#include <iostream>

using namespace Alembic::AbcGeom;

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
        sample.setTranslation( V3d( i+1, i*2, sampleTime ) );
        a.getSchema().set( sample, OSampleSelector( i, sampleTime ) );
    }

    std::cout << std::endl;


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

                std::cout << "with the value:" << std::endl << sample
                          << std::endl;

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

    IObject root( archive.getTop(), "root" );

    ISimpleXform c( root, "c" );

    std::cout << "c has " << c.getSchema().getNumSamples()
              << " samples." << std::endl;

    TimeSamplingType tst = c.getSchema().getTimeSampling().getTimeSamplingType();

    if ( c.getSchema().getTimeSampling().isStatic() )
    {
        std::cout << "c is static time" << std::endl;
    }
    if ( tst.isIdentity() ) { std::cout << "c is identity time" << std::endl; }
    if ( tst.isUniform() ) { std::cout << "c is uniform time" << std::endl; }
    if ( tst.isCyclic() ) { std::cout << "c is cyclic time" << std::endl; }
    if ( tst.isAcyclic() ) { std::cout << "c is acyclic time" << std::endl; }

    if ( c.getSchema().isConstant() )
    {
        std::cout << "c is constant!" << std::endl;
    }

    for ( size_t i = 0 ; i < c.getSchema().getNumSamples() ; i++ )
    {
        std::cout << i << "th read sample's translation at "
                  << c.getSchema().getTimeSampling().getSampleTime( i )
                  << ": " << std::endl
                  << c.getSchema().getValue().getTranslation() << std::endl;
    }
}

//-*****************************************************************************
int main(int argc, char **argv)
{
    std::string arkive( "simpleXformTest2.abc" );

    writeArchive( arkive );
    readArchive( arkive );

    // fail this test manually until it is correct
    return -1;
}
