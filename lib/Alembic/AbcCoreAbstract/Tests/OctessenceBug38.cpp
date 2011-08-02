//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/Util/All.h>

#include "Assert.h"

#include <iostream>
#include <vector>

//-*****************************************************************************
namespace AbcA = Alembic::AbcCoreAbstract;

using namespace Alembic::Util;

//-*****************************************************************************
void WriteTestArchive( const std::string &iArchiveName )
{
    AbcA::MetaData md;
    md.setUnique( "name", "arkive" );

    Alembic::AbcCoreHDF5::WriteArchive aw;

    AbcA::ArchiveWriterPtr archive = aw( iArchiveName, md );
    // at this point, the archive has a single H5 Group under the root
    // group called "ABC".

    AbcA::ObjectWriterPtr top = archive->getTop();
    AbcA::ObjectWriterPtr finalChild;

    {
        // as things go out of scope, they automatically clean themselves up.
        // when the archive goes out of scope, it writes itself to disk.

        md.set( "name", "childObject0" );
        AbcA::ObjectWriterPtr archiveChild = top->createChild(
            AbcA::ObjectHeader( "archiveChild", md ) );

        md.set( "name", "childObject1" );
        AbcA::ObjectWriterPtr archiveChildChild = archiveChild->createChild(
            AbcA::ObjectHeader( "archiveChildChild", md ) );

        md.set( "name", "childObject2" );
        finalChild = archiveChildChild->createChild(
            AbcA::ObjectHeader( "archiveChildChildChild", md ) );
    }

    // OK, let's write some properties.  First, get a shared_ptr to the
    // CompoundPropertyWriter for the final object.
    AbcA::CompoundPropertyWriterPtr fcProps = finalChild->getProperties();

    const AbcA::TimeSamplingType tsampType( 2, 2.0 );

    // Now, make some sample data.  We'll just use some ints.
    const size_t numVals = 50;
    const Dimensions dims( numVals );
    const AbcA::DataType dtype( Alembic::Util::kInt32POD, 1 );

    AbcA::PropertyHeader ph( "firstInt32ArrayProp", AbcA::kArrayProperty,
                             AbcA::MetaData(), dtype, tsampType );

    AbcA::ArrayPropertyWriterPtr arrayPropWtrPtr =
        fcProps->createArrayProperty( ph );

    std::vector<Alembic::Util::int32_t> intData0;
    for ( size_t i = 0 ; i < numVals ; ++i )
    {
        intData0.push_back( i );
    }
    AbcA::ArraySample as0( (const void*)&(intData0.front() ), dtype, dims );
    arrayPropWtrPtr->setSample( 0, 0.0, as0 );


    std::vector<Alembic::Util::int32_t> intData1;
    for ( size_t i = 0 ; i < 30 ; ++i )
    {
        intData1.push_back( i + 1 );
    }
    AbcA::ArraySample as1( (const void*)&( intData1.front() ), dtype,
                           Dimensions( 30 ) );
    arrayPropWtrPtr->setSample( 1, 1.0, as1 );

    std::vector<Alembic::Util::int32_t> intData2;
    for ( size_t i = 0 ; i < numVals ; ++i )
    {
        intData2.push_back( i + 2 );
    }
    AbcA::ArraySample as2( (const void*)&( intData2.front() ), dtype, dims );
    arrayPropWtrPtr->setSample( 2, 2.0, as2 );

    std::vector<Alembic::Util::int32_t> intData3;
    for ( size_t i = 0 ; i < numVals ; ++i )
    {
        intData3.push_back( i + 3 );
    }
    AbcA::ArraySample as3( (const void*)&( intData3.front() ), dtype, dims );
    arrayPropWtrPtr->setSample( 3, 3.0, as3 );

    std::vector<int32_t> intData4;
    for ( size_t i = 0 ; i < numVals ; ++i )
    {
        intData4.push_back( i + 4 );
    }
    AbcA::ArraySample as4( (const void*)&( intData4.front() ), dtype, dims );
    arrayPropWtrPtr->setSample( 4, 4.0, as4 );

    std::vector<Alembic::Util::int32_t> intData5;
    for ( size_t i = 0 ; i < 2 ; ++i )
    {
        intData5.push_back( i + 5 );
    }
    AbcA::ArraySample as5( (const void*)&( intData5.front() ), dtype,
                           Dimensions( 2 ) );
    arrayPropWtrPtr->setSample( 5, 5.0, as5 );

    std::vector<Alembic::Util::int32_t> intData6;
    for ( size_t i = 0 ; i < 78 ; ++i )
    {
        intData6.push_back( i + 6 );
    }
    AbcA::ArraySample as6( (const void*)&( intData6.front() ), dtype,
                           Dimensions( 78 ) );
    arrayPropWtrPtr->setSample( 6, 6.0, as6 );

    // OK, let's try a second array property, Acyclic time sampling
    AbcA::PropertyHeader ph2( "secondInt32ArrayProp", AbcA::kArrayProperty,
                              AbcA::MetaData(), dtype,
                              AbcA::TimeSamplingType(
                                  AbcA::TimeSamplingType::kAcyclic ) );
    AbcA::ArrayPropertyWriterPtr secondArrayProp =
        fcProps->createArrayProperty( ph2 );

    std::vector<int32_t> secondPropDataVec0;
    for ( size_t i = 0 ; i < 2 ; ++i )
    {
        secondPropDataVec0.push_back( i );
    }
    AbcA::ArraySample as2nd0( (const void*)&( secondPropDataVec0.front() ),
                             dtype, Dimensions( 2 ) );
    secondArrayProp->setSample( 0, 0.0, as2nd0 );

    std::vector<int32_t> secondPropDataVec1;
    for ( size_t i = 0 ; i < 10 ; ++i )
    {
        secondPropDataVec1.push_back( i + 1 );
    }
    AbcA::ArraySample as2nd1( (const void*)&( secondPropDataVec1.front() ),
                              dtype, Dimensions( 10 ) );
    secondArrayProp->setSample( 1, 0.5, as2nd1 );


    std::vector<int32_t> secondPropDataVec2;
    for ( size_t i = 0 ; i < 23 ; ++i )
    {
        secondPropDataVec2.push_back( i + 2 );
    }
    AbcA::ArraySample as2nd2( (const void*)&( secondPropDataVec2.front() ),
                              dtype, Dimensions( 23 ) );
    secondArrayProp->setSample( 2, 2.3, as2nd2 );

    // OK, now a ScalarProperty.
    AbcA::PropertyHeader scalarPH( "scalarProp", AbcA::kScalarProperty,
                                  AbcA::MetaData(),
                                  AbcA::DataType( Alembic::Util::kFloat32POD, 1 ),
                                  tsampType );

    AbcA::ScalarPropertyWriterPtr scalarPropWtrPtr =
        fcProps->createScalarProperty( scalarPH );

    float32_t scalarVal = 42.0f;

    scalarPropWtrPtr->setSample( 0, 0.0, (const void*)&scalarVal );

}

//-*****************************************************************************
void ReadTestArchive( const std::string &iArchiveName )
{
    // When opening an archive for reading, you can pass in a pointer to
    // an AbcA::ReadArraySampleCache, but if you don't, it will construct one
    // for you by default.  If you don't want to cache, just pass in NULL
    // as the second argument.
    Alembic::AbcCoreHDF5::ReadArchive ar;
    AbcA::ArchiveReaderPtr arkive = ar( iArchiveName );

    // Just stashing away a compound property reader pointer; we know the last
    // child object in our hierarchy has all the properties, because we created
    // it in the WriteTestArchive function.
    AbcA::CompoundPropertyReaderPtr fcProps;

    // Now do a depth-first traversal of our archive; this is a little ugly
    AbcA::ObjectReaderPtr tmpOrp1;
    AbcA::ObjectReaderPtr tmpOrp2 = arkive->getTop();
    while ( true )
    {
        tmpOrp1 = tmpOrp2;

        if ( tmpOrp1->getNumChildren() < 1 )
        {
            std::cout << "Found my last lonely child, named "
                      << tmpOrp1->getFullName() << std::endl;
            fcProps = tmpOrp1->getProperties();

            std::cout << "It has " << fcProps->getNumProperties()
                      << " sub-properties." << std::endl;
            break;
        }

        tmpOrp2 = tmpOrp1->getChild( 0 );
    }

    // OK, fcProps is a shared pointer to the compound property reader that
    // was in the last child object of the archive.  Let's get the simple
    // properties out of it.
    AbcA::ScalarPropertyReaderPtr sProp;
    AbcA::ArrayPropertyReaderPtr aProp0;
    AbcA::ArrayPropertyReaderPtr aProp1;
    for ( size_t i = 0 ; i < fcProps->getNumProperties() ; ++i )
    {
        AbcA::BasePropertyReaderPtr bp = fcProps->getProperty( i );
        std::cout << "Found " << bp->getName() << std::endl;
        if ( bp->isScalar() )
        {
            sProp = bp->asScalarPtr();
            std::cout << "sProp has " << sProp->getNumSamples()
                      << " samples." << std::endl;
        }
        if ( bp->isArray() && bp->getName() == "secondInt32ArrayProp" )
        {
            aProp1 = bp->asArrayPtr();
            std::cout << "aProp1 has " << aProp1->getNumSamples()
                      << " samples." << std::endl;
        }
        if ( bp->isArray() && bp->getName() == "firstInt32ArrayProp" )
        {
            aProp0 = bp->asArrayPtr();
            std::cout << "aProp0 has " << aProp0->getNumSamples()
                      << " samples." << std::endl;
        }
    }

    // OK, now we have pointers to the two properties, one scalar, the other
    // array.  Let's read the scalar one first.
    for ( size_t i = 0 ; i < sProp->getNumSamples() ; ++i )
    {
        float32_t sVal = 0;
        sProp->getSample( i, (void*)(&sVal) );

        TESTING_ASSERT( sVal == 42.0 );

        std::cout << sProp->getName() << " at sample " << i << " has the value "
                  << sVal << std::endl << std::endl;
    }

    // OK, now the first int array property.
    AbcA::ArraySamplePtr samp;

    std::cout << "FIRST INT ARRAY PROPERTY (Cyclic time sampling)"
              << std::endl;
    for ( size_t i = 0 ; i < aProp0->getNumSamples() ; ++i )
    {
        aProp0->getSample( i, samp );
        const AbcA::TimeSampling ts = aProp0->getTimeSampling();
        size_t numPoints = samp->getDimensions().numPoints();

        TESTING_ASSERT( (AbcA::chrono_t)i == ts.getSampleTime( i ) );

        std::cout << "At Sample " << i << ", " << aProp0->getName()
                  << " is at time " << ts.getSampleTime( i )
                  << " and has " << numPoints << " points, with the values: "
                  << std::endl;

        int32_t *vals = (int32_t*)(samp->getData());

        for ( size_t j = 0 ; j < numPoints ; ++j )
        {
            TESTING_ASSERT( vals[j] == i + j );
            std::cout << j << ": " << vals[j] << ", ";
        }
        std::cout << std::endl << std::endl;
        samp->reset();
    }

    // now the second int array
    std::cout << "SECOND INT ARRAY PROPERTY (Acyclic time sampling)"
              << std::endl;
    for ( size_t i = 0 ; i < aProp1->getNumSamples() ; ++i )
    {
        aProp1->getSample( i, samp );
        const AbcA::TimeSampling ts = aProp1->getTimeSampling();
        size_t numPoints = samp->getDimensions().numPoints();

        std::cout << "At Sample " << i << ", " << aProp1->getName()
                  << " is at time " << ts.getSampleTime( i )
                  << " and has " << numPoints << " points,"
                  << std::endl << "with the values: " << std::endl;

        int32_t *vals = (int32_t*)(samp->getData());

        for ( size_t j = 0 ; j < numPoints ; ++j )
        {
            TESTING_ASSERT( vals[j] == i + j );
            std::cout << j << ": " << vals[j] << ", ";
        }
        std::cout << std::endl << std::endl;

        samp->reset();
    }
}

//-*****************************************************************************
int main( int, char** )
{
    const std::string firstArchive( "firstArchive.abc" );

    WriteTestArchive( firstArchive );
    ReadTestArchive( firstArchive );

    return 0;
}
