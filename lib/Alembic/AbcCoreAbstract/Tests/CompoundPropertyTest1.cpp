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
//#include <Alembic/HDF5/All.h>
#include <Alembic/Util/All.h>

#include <iostream>
#include <vector>

//-*****************************************************************************
namespace A5 = Alembic::AbcCoreHDF5;

namespace ABCA = Alembic::AbcCoreAbstract;

using ABCA::chrono_t;
using Alembic::Util::float32_t;
using Alembic::Util::int32_t;
using Alembic::Util::byte_t;
using Alembic::Util::Dimensions;

//-*****************************************************************************
void WriteTestArchive( const std::string &iArchiveName )
{
    ABCA::MetaData md;
    md.setUnique( "name", "arkive" );
    ABCA::ObjectWriterPtr finalChild;

    A5::WriteArchive aw;

    ABCA::ArchiveWriterPtr archive = aw( iArchiveName, md );

    ABCA::ObjectWriterPtr topObj = archive->getTop();
    // at this point, the archive has a single H5 Group under the root
    // group called "ABC".

    {
        // as things go out of scope, they automatically clean themselves up.
        // when the archive goes out of scope, it writes itself to disk.

        md.set( "name", "childObject0" );
        ABCA::ObjectWriterPtr archiveChild = topObj->createChild(
            ABCA::ObjectHeader( "archiveChild", md ) );
        // the hdf5 layout of the archive is now "/ABC/archiveChild/"

        md.set( "name", "childObject1" );
        ABCA::ObjectWriterPtr archiveChildChild = archiveChild->createChild(
            ABCA::ObjectHeader( "archiveChildChild", md ) );
        // "/ABC/archiveChild/archiveChildChild"

        md.set( "name", "childObject2" );
        finalChild = archiveChildChild->createChild(
            ABCA::ObjectHeader( "archiveChildChildChild", md ) );
        // "/ABC/archiveChild/archiveChildChild/archiveChildChildChild"
    }
    // At this point, only the final child and the archive are still in scope,
    // but we should still be able to write some properties to the final child,
    // even though its parents are "gone", and all access needs to be done
    // hierarchically.

#if 0

    // OK, let's write some properties.  First, get a shared_ptr to the
    // CompoundPropertyWriter for the final object.
    ABC::CompoundPropertyWriterPtr fcProps = finalChild->getProperties();

    // time sampling; nothing too fancy here, it's not the focus of this test
    // just do uniform time sampling with a period of 1.0, starting at 0.0
    const ABC::TimeSamplingType tsampType( 1, 1.0 );

    // Now, make some sample data.  We'll just use some ints.
    const size_t numVals = 50;
    const Alembic::Util::Dimensions dims( numVals );
    const ABC::DataType dtype( Alembic::Util::kInt32POD, 1 );

    md.set( "name", "int32_t array property" );
    ABC::PropertyHeader ph( "firstArrayProp", ABC::kArrayProperty, md,
                            dtype, tsampType );

    ABC::ArrayPropertyWriterPtr arrayPropWtrPtr =
        fcProps->createArrayProperty( ph );

    std::vector<Alembic::Util::int32_t> intData0;
    for ( size_t i = 0 ; i < numVals ; ++i )
    {
        intData0.push_back( i + 2 );
    }
    ABC::ArraySample as0( (const void*)&(intData0.front()), dtype, dims );
    arrayPropWtrPtr->setSample( 0, 0.0, as0 );


    std::vector<Alembic::Util::int32_t> intData1;
    for ( size_t i = 0 ; i < 30 ; ++i )
    {
        intData1.push_back( i + 3 );
    }
    ABC::ArraySample as1( (const void*)&(intData1.front()), dtype,
                          Dimensions( 30 ) );
    arrayPropWtrPtr->setSample( 1, 1.0, as1 );

    std::vector<Alembic::Util::int32_t> intData2;
    for ( size_t i = 0 ; i < numVals ; ++i )
    {
        intData2.push_back( i + 5 );
    }
    ABC::ArraySample as2( (const void*)&(intData2.front()), dtype, dims );
    arrayPropWtrPtr->setSample( 2, 2.0, as2 );

    std::vector<Alembic::Util::int32_t> intData3;
    for ( size_t i = 0 ; i < numVals ; ++i )
    {
        intData3.push_back( i + 7 );
    }
    ABC::ArraySample as3( (const void*)&(intData3.front()), dtype, dims );
    arrayPropWtrPtr->setSample( 3, 3.0, as3 );

    // for our fifth sample, we'll re-use the data from the first one.
    // Alembic detects that the data is the same as the first sample,
    // and automatically prevents it from being written out twice.
    //arrayPropWtrPtr->setSample( 4, 4.0, as0 );
    std::vector<int32_t> intData4;
    for ( size_t i = 0 ; i < numVals ; ++i )
    {
        intData4.push_back( i + 4 );
    }
    ABC::ArraySample as4( (const void*)&(intData4.front()), dtype, dims );
    arrayPropWtrPtr->setSample( 4, 4.0, as4 );

    /* at this point, the HDF5 file looks like this:
h5ls -r firstArchive.abc
/                        Group
/ABC                     Group
/ABC/archiveChild        Group
/ABC/archiveChild/archiveChildChild Group
/ABC/archiveChild/archiveChildChild/archiveChildChildChild Group
/ABC/archiveChild/archiveChildChild/archiveChildChildChild/.prop Group
/ABC/archiveChild/archiveChildChild/archiveChildChildChild/.prop/intArrayProp.smp0 Dataset {50}
/ABC/archiveChild/archiveChildChild/archiveChildChildChild/.prop/intArrayProp.smpi Group
/ABC/archiveChild/archiveChildChild/archiveChildChildChild/.prop/intArrayProp.smpi/.smp_00000001 Dataset {50}
/ABC/archiveChild/archiveChildChild/archiveChildChildChild/.prop/intArrayProp.smpi/.smp_00000002 Dataset {50}
/ABC/archiveChild/archiveChildChild/archiveChildChildChild/.prop/intArrayProp.smpi/.smp_00000003 Dataset {50}
/ABC/archiveChild/archiveChildChild/archiveChildChildChild/.prop/intArrayProp.smpi/.smp_00000004 Dataset, same as /ABC/archiveChild/archiveChildChild/archiveChildChildChild/.prop/intArrayProp.smp0

    Note that at no point did we name anything "ABC" or ".prop"; that is done
    automatically by the low-level HDF5-writing logic in AbcCoreHDF5.

    Also note: we were able to write the sample data to the property on the
    final child, even though all that child's parents had gone out of scope.
    Awesome.
    */


    std::vector<Alembic::Util::int32_t> intData5;
    for ( size_t i = 0 ; i < 2 ; ++i )
    {
        intData5.push_back( i );
    }
    ABC::ArraySample as5( (const void*)&(intData5.front()), dtype,
                          Dimensions( 2 ) );
    arrayPropWtrPtr->setSample( 5, 5.0, as5 );

    std::vector<Alembic::Util::int32_t> intData6;
    for ( size_t i = 0 ; i < 78 ; ++i )
    {
        intData6.push_back( i + 1 );
    }
    ABC::ArraySample as6( (const void*)&(intData6.front()), dtype,
                          Dimensions( 78 ) );
    arrayPropWtrPtr->setSample( 6, 6.0, as6 );

    // OK, let's try a second array property
    ABC::PropertyHeader ph2( "secondArrayProp", ABC::kArrayProperty,
                             ABC::MetaData(), dtype, tsampType );
    ABC::ArrayPropertyWriterPtr secondArrayProp =
        fcProps->createArrayProperty( ph2 );

    std::vector<int32_t> secondPropDataVec;
    for ( size_t i = 0 ; i < 2 ; ++i )
    {
        secondPropDataVec.push_back( i );
    }
    ABC::ArraySample as2nd0( (const void*)&(secondPropDataVec.front()),
                             dtype, Dimensions( 2 ) );
    secondArrayProp->setSample( 0, 0.0, as2nd0 );

    // OK, now a ScalarProperty.
    ABC::PropertyHeader scalarPH( "scalarProp", ABC::kScalarProperty,
                                  ABC::MetaData(),
                                  ABC::DataType( Alembic::Util::kFloat32POD, 1 ),
                                  tsampType );

    ABC::ScalarPropertyWriterPtr scalarPropWtrPtr =
        fcProps->createScalarProperty( scalarPH );

    float32_t scalarSampleVal = 42.0;
    scalarPropWtrPtr->setSample( 0, 0.0, (const void*)&scalarSampleVal );
#endif

}

#if 0
//-*****************************************************************************
void ReadTestArchive( const std::string &iArchiveName )
{
    // When opening an archive for reading, you can pass in a pointer to
    // an ABC::ReadArraySampleCache, but if you don't, it will construct one
    // for you by default.  If you don't want to cache, just pass in NULL
    // as the second argument.
    A5::ReadArchive ar;
    ABC::ArchiveReaderPtr arkive = ar( iArchiveName );

    ABC::ObjectReaderPtr topObj = arkive->getTop();

    // Just stashing away a compound property reader pointer; we know the last
    // child object in our hierarchy has all the properties, because we created
    // it in the WriteTestArchive function.
    ABC::CompoundPropertyReaderPtr fcProps;

    // Now do a depth-first traversal of our archive; this is a little ugly
    ABC::ObjectReaderPtr tmpOrp1;
    ABC::ObjectReaderPtr tmpOrp2 = topObj->getChild( 0 );
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
    ABC::ScalarPropertyReaderPtr sProp;
    ABC::ArrayPropertyReaderPtr aProp;
    for ( size_t i = 0 ; i < fcProps->getNumProperties() ; ++i )
    {
        ABC::BasePropertyReaderPtr bp = fcProps->getProperty( i );
        std::cout << "Found " << bp->getName() << std::endl;
        if ( bp->isScalar() )
        {
            sProp = bp->asScalarPtr();
            std::cout << "sProp has " << sProp->getNumSamples()
                      << " samples." << std::endl;
        }
        if ( bp->isArray() && bp->getName() == "secondArrayProp" )
        {
            aProp = bp->asArrayPtr();
            std::cout << "aProp has " << aProp->getNumSamples()
                      << " samples." << std::endl;
        }
    }

    // OK, now we have pointers to the two properties, one scalar, the other
    // array.  Let's read the scalar one first.
    float32_t sVal = 0;
    for ( size_t i = 0 ; i < sProp->getNumSamples() ; ++i )
    {
        sProp->getSample( i, (Alembic::Util::byte_t*)(&sVal) );

        ABCA_ASSERT( sVal == 42.0, "Should be 42." );

        std::cout << sProp->getName() << " at sample " << i << " has the value "
                  << sVal << std::endl << std::endl;
    }

    // OK, now the array property.
    ABC::ArraySamplePtr samp;
    for ( size_t i = 0 ; i < aProp->getNumSamples() ; ++i )
    {
        aProp->getSample( i, samp );
        size_t numPoints = samp->getDimensions().numPoints();

        std::cout << "At Sample " << i << ", " << aProp->getName()
                  << " has " << numPoints << " points, with the values: "
                  << std::endl;

        int32_t *vals = (int32_t*)(samp->getData());

        for ( size_t j = 0 ; j < numPoints ; ++j )
        {
            std::cout << j << ": " << vals[j] << ", ";
        }
        std::cout << std::endl << std::endl;

        samp->reset();
    }

    #if 0
    // currently, this doesn't work, but I *think* it should.
    // check to ensure our last array sample's sole value is 99
    aProp->getSample( 5, samp );
    byte_t *unoByto = samp->getBytes().get();
    int32_t singleElementArrayVal = ((int32_t)(*unoByto))[0];
    ABCA_ASSERT( singleElementArrayVal == 99,
                 "Value should be 99." );
    #endif

}
#endif

//-*****************************************************************************
int main( int, char** )
{
    const std::string firstArchive( "firstArchive.abc" );

    WriteTestArchive( firstArchive );
    //ReadTestArchive( firstArchive );

}
