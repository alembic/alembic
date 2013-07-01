//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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
#include <Alembic/AbcCoreHDF5/All.h>

namespace Abc = Alembic::Abc;
using namespace Abc;

using Alembic::AbcCoreAbstract::chrono_t;
using Alembic::AbcCoreAbstract::index_t;
using Alembic::Util::uint32_t;
using Alembic::Util::float32_t;

//-*****************************************************************************
void simpleTestOut()
{
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                      "slappyJim.abc", ErrorHandler::kThrowPolicy );

    OObject slim( OObject( archive, kTop ), "slim" );

    OObject jubby( slim, "jubby" );
    OObject mespa( jubby, "mespa" );

    OCompoundProperty clampto( OCompoundProperty( mespa, kTop ),
                               "clampto" );

    OV3fProperty clanker( clampto, "clanker", 0 );

    OInt32Property clunker( clampto, "clunker", 0 );

    // By scoping it, it deletes itself immediately.
    OBoolProperty( OCompoundProperty( slim, kTop ),
                   "slumber" ).set( false );

    // Make a strangely sampled V3dArray
    {
        OObject spaniard( jubby, "spaniard" );

        chrono_t sampTime = 0.0;
        std::vector<V3d> points( ( size_t )71 );

        // create our random samples
        std::vector < chrono_t > timeSamps(37);
        for ( index_t idx = 0; idx < 37; ++idx )
        {
            timeSamps[idx] = sampTime;
            sampTime += 1.0 / ( idx + 1 );
        }

        TimeSampling ts(TimeSamplingType( TimeSamplingType::kAcyclic ),
            timeSamps);
        Alembic::Util::uint32_t tsidx = archive.addTimeSampling(ts);

        OV3dArrayProperty pointy(
            OCompoundProperty( spaniard, kTop ),
            "pointy", tsidx);

        for ( index_t idx = 0; idx < 36; ++idx )
        {
            if ( idx == 0 )
            {
                for ( std::vector<V3d>::iterator piter = points.begin();
                      piter != points.end(); ++piter )
                {
                    (*piter) = V3d( idx, idx, idx );
                }
            }
            pointy.set( points );
        }

        // Test that zero-sized samples work.
        points.resize( 0 );
        pointy.set( points );
    }

    //-*************************************************************************
    // STRING PROPERTIES
    //-*************************************************************************
    OObject oscar( mespa, "oscar" );
    OStringProperty wildeName(
        OCompoundProperty( oscar, kTop ),
        "wildeName" );
    wildeName.set( "Oscar" );
    wildeName.set( "Wilde" );

    // This is Lewis Carroll, not Oscar Wilde, but whatever.
    OStringArrayProperty jabber(
        OCompoundProperty( oscar, kTop ),
        "jabberwocky" );

    {
        std::vector<std::string> stanza1;
        stanza1.push_back( "twas" );
        stanza1.push_back( "brillig" );
        stanza1.push_back( "and" );
        stanza1.push_back( "the" );
        stanza1.push_back( "slithy" );
        stanza1.push_back( "toves" );
        jabber.set( stanza1 );
    }

    {
        std::vector<std::string> stanza2;
        stanza2.push_back( "did" );
        stanza2.push_back( "gyre" );
        stanza2.push_back( "and" );
        stanza2.push_back( "gimble" );
        stanza2.push_back( "in" );
        stanza2.push_back( "the" );
        stanza2.push_back( "wobe" );
        jabber.set( stanza2 );
    }

    {
        std::vector<std::string> emptyStanza;
        jabber.set( emptyStanza );
    }

    OFloatProperty radius( clampto, "radius" );
    radius.set( 217.0f );

    for ( index_t idx = 0; idx < 1; ++idx )
    {

        V3f v( ( float )idx );
        clanker.set( v );

        clunker.set( ( Alembic::Util::int32_t )( 731 - ( int )idx ) );
    }

    std::cout << "Gonna try to close: " << archive.getName()
              << std::endl;
}

//-*****************************************************************************
void simpleTestIn()
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                      "slappyJim.abc", ErrorHandler::kThrowPolicy );

    IObject slim( IObject( archive, kTop ), "slim" );
    IObject jubby( slim, "jubby" );
    IObject mespa( jubby, "mespa" );

    ICompoundProperty clampto(
        ICompoundProperty( mespa, kTop ), "clampto" );

    IV3fProperty clanker( clampto, "clanker" );
    IInt32Property clunker( clampto, "clunker" );

    // By scoping it, it deletes itself immediately.
    std::cout << "Do I Slumber? "
              << IBoolProperty(
                  ICompoundProperty( slim, kTop ),
                  "slumber" ).getValue()
              << std::endl;

    // Strangely sampled.
    {
        IObject spaniard( jubby, "spaniard" );
        IV3dArrayProperty pointy(
            ICompoundProperty( spaniard, kTop ), "pointy" );

        std::cout << "Num pointy spaniard samples: "
                  << pointy.getNumSamples() << std::endl;

        size_t pointySampleIdx = pointy.getNumSamples() / 2;
        V3dArraySamplePtr pointySamp;
        pointy.get( pointySamp, pointySampleIdx );
        ABCA_ASSERT( pointySamp, "should be valid" );

        size_t numPoints = pointySamp->size();
        std::cout << "Num points in sample: " << numPoints << std::endl;

        for ( size_t i = 0; i < numPoints; ++i )
        {
            std::cout << "pointy[" << i << "] = " << (*pointySamp)[i]
                      << std::endl;
        }

        // Test that we can get a zero-sized sample.
        std::cout << "Testing getting a zero-length sample"
                  << std::endl;
        pointy.get( pointySamp, pointy.getNumSamples()-1 );
        std::cout << "Zero length sample num points: "
                  << pointySamp->size() << std::endl;
    }


    //-*************************************************************************
    // STRING PROPERTIES
    //-*************************************************************************
    IObject oscar( mespa, "oscar" );
    IStringProperty wildeName(
        ICompoundProperty( oscar, kTop ),
        "wildeName" );
    std::cout << "Oscar Wilde Names: " << std::endl;
    size_t numNames = wildeName.getNumSamples();
    for ( size_t i = 0; i < numNames; ++i )
    {
        std::cout << "\t" << i << ": "
                  << wildeName.getValue( i ) << std::endl;
    }

    IStringArrayProperty jabber(
        ICompoundProperty( oscar, kTop ), "jabberwocky" );
    std::cout << "Jabberwocky first two lines: " << std::endl;
    size_t numLines = jabber.getNumSamples();
    std::cout << "Num lines: " << numLines << std::endl;
    for ( size_t i = 0; i < numLines; ++i )
    {
        StringArraySamplePtr line = jabber.getValue( i );
        if ( line )
        {
            size_t numWords = line->size();
            if ( numWords )
            {
                std::cout << "\t";
                for ( size_t j = 0; j < numWords; ++j )
                {
                    std::cout << " " << (*line)[j];
                }
                std::cout << std::endl;
            }
            else
            {
                std::cout << "\tNo words in this line." << std::endl;
            }
        }
    }

    IFloatProperty radius( clampto, "radius" );

    std::cout << "Num clanker samples: "
              << clanker.getNumSamples() << std::endl;

    std::cout << "Gonna try to close: " << archive.getName()
              << std::endl;
}

int main( int argc, char *argv[] )
{
    ABCA_ASSERT( sizeof( char ) == sizeof( Alembic::Util::int8_t ),
                 "Alembic::Util::int8_t isn't 1 byte" );
    ABCA_ASSERT(
        ( sizeof( wchar_t ) == sizeof( Alembic::Util::int8_t ) ) ||
        ( sizeof( wchar_t ) == sizeof( Alembic::Util::int16_t ) ) ||
        ( sizeof( wchar_t ) == sizeof( Alembic::Util::int32_t ) ),
        "wchar_t is an unexpedted size." );

    simpleTestOut();
    simpleTestIn();

    return 0;
}
