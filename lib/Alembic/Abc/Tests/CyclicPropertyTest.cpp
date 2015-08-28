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

#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/Abc/All.h>

#ifdef ALEMBIC_WITH_HDF5
#include <Alembic/AbcCoreHDF5/All.h>
#endif

namespace AbcF = Alembic::AbcCoreFactory;

namespace Abc = Alembic::Abc;
using namespace Abc;

const chrono_t g_startTime = 123.0;
const chrono_t g_dt = 1.0 / 24.0;


//
// The tests in this file are intended to exercize the Abc API;
//  specifically writing and reading of cyclically sampled properties
//

void writeProperty(const std::string &archiveName, bool useOgawa)
{
    unsigned int numSamples = 5;

    // Create an archive for writing. Indicate that we want Alembic to
    //   throw exceptions on errors.
    OArchive archive;
    if ( useOgawa )
    {
        archive = OArchive( Alembic::AbcCoreOgawa::WriteArchive(),
                            archiveName, ErrorHandler::kThrowPolicy );
    }
#ifdef ALEMBIC_WITH_HDF5
    else
    {
        archive = OArchive( Alembic::AbcCoreHDF5::WriteArchive(),
                            archiveName, ErrorHandler::kThrowPolicy );
    }
#endif

    OObject archiveTop = archive.getTop();

    // Create a child, parented under the archive
    std::string name = "child";
    OObject child( archiveTop, name );
    OCompoundProperty childProps = child.getProperties();

    // Create cyclic time sampling
     std::vector<chrono_t> tvec;
    tvec.push_back(  g_startTime );
    tvec.push_back(  g_startTime + g_dt/3.0 );
    tvec.push_back(  g_startTime + 2.0*g_dt/3.0 );
    const chrono_t timePerCycle = g_dt;
    const size_t numSamplesPerCycle = 3;

    numSamples *= numSamplesPerCycle;
    const TimeSamplingType tSampTyp( numSamplesPerCycle, timePerCycle );
    TimeSamplingPtr ts( new TimeSampling( tSampTyp, tvec ) );

    // Create a scalar property on this child object named 'mass'
    //  with metadata that indicates it's expressed in kilograms
    MetaData units;
    units.set( "units", "kilogram" );
    ODoubleProperty mass( childProps,  // owner
                          "mass", // name
                          units,
                          ts ); // cyclic, specified above

    for (unsigned int tt=0; tt<numSamples; tt++)
    {
        double mm = (1.0 + 0.1*tt); // vary the mass
        mass.set( mm );
    }

    std::cout << archiveName << " was successfully written" << std::endl;
    // Done - the archive closes itself
}


template <class PROPERTY_CLASS>
void
printSampleValue( PROPERTY_CLASS& iProp, const ISampleSelector &iSS )
{
    std::cout << iProp.getValue( iSS ) << " ";
}


void readProperty(const std::string &archiveName, bool useOgawa)
{
    // Open an existing archive for reading. Indicate that we want
    //   Alembic to throw exceptions on errors.
    std::cout  << "Reading " << archiveName << std::endl;
    AbcF::IFactory factory;
    factory.setPolicy(  ErrorHandler::kThrowPolicy );
    AbcF::IFactory::CoreType coreType;
    IArchive archive = factory.getArchive(archiveName, coreType);
    ABCA_ASSERT( (useOgawa && coreType == AbcF::IFactory::kOgawa) ||
                    (!useOgawa && coreType == AbcF::IFactory::kHDF5),
                  "File did not open as the expected type." );

    IObject archiveTop = archive.getTop();

    // Determine the number of (top level) children the archive has
    const unsigned int numChildren =  archiveTop.getNumChildren();
    ABCA_ASSERT( numChildren == 1, "Wrong number of children (expected 1)");
    std::cout << "The archive has " << numChildren << " children:"
              << std::endl;


    // Iterate through them, print out their names
    IObject child( archiveTop, archiveTop.getChildHeader( 0 ).getName() );
    std::cout << "  " << child.getName();


    // Properties
    ICompoundProperty props = child.getProperties();
    size_t numProperties = props.getNumProperties();  // only top-level props
    ABCA_ASSERT( numProperties == 1,
                 "Expected 1 property, found " << numProperties);
    std::cout << " with one property";


    std::vector<std::string> propNames(1);
    propNames[0] = props.getPropertyHeader(0).getName();
    std::cout << " named " << propNames[0] << std::endl;

    PropertyType pType = props.getPropertyHeader(0).getPropertyType();
    ABCA_ASSERT( pType == kScalarProperty,
                 "Expected a scalar property, but didn't find one" );

    std::cout << " which is a scalar property";



    DataType dType = props.getPropertyHeader(0).getDataType();
    ABCA_ASSERT( dType.getPod() == kFloat64POD,
                 "Expected a double (kFloat64POD) property, but didn't"
                 " find one" );

    // We know this is a scalar property (I'm eliding the if/else
    //  statements required to recognize this)
    IDoubleProperty mass( props, propNames[0] );

    size_t numSamples = mass.getNumSamples();
    std::cout << ".. it has " << numSamples << " samples" << std::endl;
    //ABCA_ASSERT( numSamples == 5, "Expected 5 samples, found " << numSamples );

    TimeSamplingPtr ts = mass.getTimeSampling();

    std::cout << "..with time/value pairs: ";
    for (unsigned int ss=0; ss<numSamples; ss++)
    {
        ISampleSelector iss( (index_t) ss);
        std::cout << ts->getSampleTime( (index_t) ss ) << "/";
        printSampleValue( mass, iss );
        std::cout << " ";

        double timeDiff = ts->getSampleTime( (index_t) ss ) -
            (g_startTime + (ss*(g_dt/3.0)));
        ABCA_ASSERT( fabs(timeDiff) < 1e-12, "Incorrect sample time read" );


        double massDiff = mass.getValue( iss ) - (1.0 + 0.1*ss);
        ABCA_ASSERT( fabs(massDiff) < 1e-12, "Incorrect sample value read" );
    }
    ABCA_ASSERT(
        archive.getMaxNumSamplesForTimeSamplingIndex(1) == (index_t) numSamples,
        "Incorrect number of max samples for Time Sampling ID 1.");
    std::cout << std::endl;

    // Done - the archive closes itself
}




int main( int argc, char *argv[] )
{
    // Write and read a simple archive: ten children, each with one
    //  simple property
    bool useOgawa = false;
    try
    {
        std::string archiveName("cyclic_sampling_test.abc");
#ifdef ALEMBIC_WITH_HDF5
        writeProperty ( archiveName, useOgawa );
        readProperty  ( archiveName, useOgawa );
#endif
        useOgawa = true;
        writeProperty ( archiveName, useOgawa );
        readProperty  ( archiveName, useOgawa );
    }
    catch (char * str )
    {
        std::cout << "Exception raised: " << str;
        std::cout << " during cyclic sampling test ";
        if ( useOgawa )
        {
            std::cout << "using Ogawa" << std::endl;
        }
        else
        {
            std::cout << "using HDF5" << std::endl;
        }
        return 1;
    }

    return 0;
}
