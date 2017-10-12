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
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

#include <ImathMath.h>

#include <limits>

#ifdef ALEMBIC_WITH_HDF5
#include <Alembic/AbcCoreHDF5/All.h>
#endif

namespace Abc = Alembic::Abc;
namespace AbcF = Alembic::AbcCoreFactory;
using namespace Abc;

//-*****************************************************************************
static const V3f scalarV3fval( -1.0f, 32.0f, -90.0f );
static const chrono_t v3fStartTime = -4.3;
static const chrono_t intStartTime = 2.9;
static const chrono_t dt = 1.0 / 24.0;

static const size_t numV3fPoints = 20;
static const size_t numV3fSamps = 31;
static const size_t numIntPoints = 11;
static const size_t numIntSamps = 7;

static const chrono_t CHRONO_EPSILON = \
    std::numeric_limits<chrono_t>::epsilon() * 32.0;

//-*****************************************************************************
void simpleTestOut( const std::string &iArchiveName, bool useOgawa )
{
    OArchive archive;
    if (useOgawa)
    {
        archive = OArchive( Alembic::AbcCoreOgawa::WriteArchive(),
            iArchiveName, ErrorHandler::kThrowPolicy );
    }
#ifdef ALEMBIC_WITH_HDF5
    else
    {
        archive = OArchive( Alembic::AbcCoreHDF5::WriteArchive(),
            iArchiveName, ErrorHandler::kThrowPolicy );
    }
#endif

    OObject archiveTop( archive, kTop );

    // 0th archive child
    OObject ac0( archiveTop, "ac0" );
    // 0th archive child child
    OObject acc0( ac0, "acc0" );

    // 1th archive child
    OObject ac1( archiveTop, "ac1" );

    OInt32ArrayProperty ac1iap0( ac1.getProperties(), "iap0" );

    // all property manipulation is through a hidden compound property named
    // ".prop" automatically by Alembic.
    //
    // All property access is through that.
    OCompoundProperty ac0Props = ac0.getProperties();
    OCompoundProperty acc0Props = acc0.getProperties();

    // Here we create a new Compound Property named "acc0CProp0" to hold
    // a couple Int Scalar Properties
    OCompoundProperty acc0CProp0( acc0Props, "acc0CProp0" );
    OInt32Property acc0cp0ip0( acc0CProp0, "acc0cp0ip0" );
    acc0cp0ip0.set( 0 );
    OInt32Property acc0cp0ip1( acc0CProp0, "acc0cp0ip1" );
    acc0cp0ip1.set( 1 );

    // some scalar props
    OV3fProperty ac0V3fp0( ac0Props, "ac0V3fp0" );
    const AbcA::PropertyHeader * ac0V3fp0Header =
        ac0Props.getPropertyHeader( "ac0V3fp0" );
    TESTING_ASSERT( OV3fProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( IV3fProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! OFloatProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! IFloatProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! ON3fProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! IN3fProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! OV3dProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! IV3dProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! OV2fProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! IV2fProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! OV2dProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! IV2dProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! OInt32Property::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! IInt32Property::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! ODoubleProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! IDoubleProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! OV3fArrayProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! IV3fArrayProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! OFloatArrayProperty::matches( *ac0V3fp0Header ) );
    TESTING_ASSERT( ! IFloatArrayProperty::matches( *ac0V3fp0Header ) );
    ac0V3fp0.set( scalarV3fval );

    ON3fProperty ac0N3fp0( ac0Props, "ac0N3fp0" );
    const AbcA::PropertyHeader * ac0N3fp0Header =
        ac0Props.getPropertyHeader( "ac0N3fp0" );
    TESTING_ASSERT( ON3fProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( IN3fProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! OFloatProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! OFloatProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! OV3fProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! ON3dProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! ON2fProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! ON2dProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! OInt32Property::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! ODoubleProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! ON3fArrayProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! OFloatArrayProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! IFloatProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! IFloatProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! IV3fProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! IN3dProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! IN2fProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! IN2dProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! IInt32Property::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! IDoubleProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! IN3fArrayProperty::matches( *ac0N3fp0Header ) );
    TESTING_ASSERT( ! IFloatArrayProperty::matches( *ac0N3fp0Header ) );
    ac0N3fp0.set( scalarV3fval );

    OP3fProperty ac0P3fp0( ac0Props, "ac0P3fp0" );
    const AbcA::PropertyHeader * ac0P3fp0Header =
        ac0Props.getPropertyHeader( "ac0P3fp0" );
    TESTING_ASSERT( OP3fProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( IP3fProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! OFloatProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! OV3fProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! OP3dProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! OP2fProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! OP2dProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! OInt32Property::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! ODoubleProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! OP3fArrayProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! OFloatArrayProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! IFloatProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! IV3fProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! IP3dProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! IP2fProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! IP2dProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! IInt32Property::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! IDoubleProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! IP3fArrayProperty::matches( *ac0P3fp0Header ) );
    TESTING_ASSERT( ! IFloatArrayProperty::matches( *ac0P3fp0Header ) );
    ac0P3fp0.set( scalarV3fval );

    OInt32Property acc0ip0( acc0Props, "acc0ip0" );
    const AbcA::PropertyHeader * acc0ip0Header =
        acc0Props.getPropertyHeader( "acc0ip0" );
    TESTING_ASSERT( OInt32Property::matches( *acc0ip0Header ) );
    TESTING_ASSERT( IInt32Property::matches( *acc0ip0Header ) );
    TESTING_ASSERT( ! OFloatProperty::matches( *acc0ip0Header ) );
    TESTING_ASSERT( ! OInt32ArrayProperty::matches( *acc0ip0Header ) );
    TESTING_ASSERT( ! IFloatProperty::matches( *acc0ip0Header ) );
    TESTING_ASSERT( ! IInt32ArrayProperty::matches( *acc0ip0Header ) );
    TESTING_ASSERT( ! IBox2iProperty::matches( *acc0ip0Header ) );
    acc0ip0.set( 99 );

    OBoolProperty acc0bp0( acc0Props, "acc0bp0" );
    const AbcA::PropertyHeader * acc0bp0Header =
        acc0Props.getPropertyHeader( "acc0bp0" );
    TESTING_ASSERT( OBoolProperty::matches( *acc0bp0Header ) );
    TESTING_ASSERT( IBoolProperty::matches( *acc0bp0Header ) );
    TESTING_ASSERT( ! OInt32Property::matches( *acc0bp0Header ) );
    TESTING_ASSERT( ! IInt32Property::matches( *acc0bp0Header ) );
    TESTING_ASSERT( ! OBoolArrayProperty::matches( *acc0bp0Header ) );
    TESTING_ASSERT( ! IBoolArrayProperty::matches( *acc0bp0Header ) );

    acc0bp0.set( false );

    TimeSamplingPtr ts( new TimeSampling(1.0/24.0, v3fStartTime) );

    // now some array props
    OV3fArrayProperty acc0V3fap0( acc0Props, "acc0V3fap0", ts );
    const AbcA::PropertyHeader * acc0V3fap0Header =
        acc0Props.getPropertyHeader( "acc0V3fap0" );
    TESTING_ASSERT( OV3fArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( IV3fArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( ! OFloatArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( ! IFloatArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( ! OInt32ArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( ! OP3fArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( ! OV3fProperty::matches( *acc0V3fap0Header ) );

    OP3fArrayProperty acc0P3fap0( acc0Props, "acc0P3fap0", ts );
    const AbcA::PropertyHeader * acc0P3fap0Header =
        acc0Props.getPropertyHeader( "acc0P3fap0" );
    TESTING_ASSERT( OP3fArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( IP3fArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( ! OFloatArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( ! IFloatArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( ! OInt32ArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( ! OV3fArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( ! OP3fProperty::matches( *acc0P3fap0Header ) );

    ON3fArrayProperty acc0N3fap0( acc0Props, "acc0N3fap0", ts );

    ts.reset( new TimeSampling(1.0/24.0, intStartTime) );
    OInt32ArrayProperty ac0iap0( ac0Props, "ac0iap0", ts );

    // make some data for our array props
    std::vector<V3f> v3fpoints( numV3fPoints );
    std::vector<Alembic::Util::int32_t> intpoints( numIntPoints );

    chrono_t t = v3fStartTime;

    for ( size_t i = 0 ; i < numV3fSamps ; ++i )
    {
        for ( std::vector<V3f>::iterator iter = v3fpoints.begin() ;
              iter != v3fpoints.end() ; ++iter )
        {
            (*iter) = V3f( i + t, i + t, i + t );
        }

        acc0V3fap0.set( v3fpoints );
        acc0P3fap0.set( v3fpoints );
        acc0N3fap0.set( v3fpoints );

        t += dt;
    }

    t = intStartTime;

    for ( size_t i = 0 ; i < numIntSamps ; ++i )
    {
        for ( size_t j = 0 ; j < numIntPoints ; ++j )
        {
            intpoints[j] = j + i;
        }

        ac0iap0.set( intpoints );
        t += dt;
    }

    // test that we did everything right
    TESTING_ASSERT( ac0.getNumChildren() == 1 );
    TESTING_ASSERT( ac0Props.getNumProperties() == 4 );

    TESTING_ASSERT( acc0.getNumChildren() == 0 );
    TESTING_ASSERT( acc0Props.getNumProperties() == 6 );

}

//-*****************************************************************************
void simpleTestIn( const std::string &iArchiveName, bool useOgawa)
{
    AbcF::IFactory factory;
    factory.setPolicy(  ErrorHandler::kThrowPolicy );
    AbcF::IFactory::CoreType coreType;
    IArchive archive = factory.getArchive(iArchiveName, coreType);
    TESTING_ASSERT( (useOgawa && coreType == AbcF::IFactory::kOgawa) ||
                    (!useOgawa && coreType == AbcF::IFactory::kHDF5) );

    IObject archiveTop = archive.getTop();

    IObject ac0( archiveTop, "ac0" );
    IObject acc0( ac0, "acc0" );

    IObject ac1( archiveTop, "ac1" );

    IInt32ArrayProperty ac1iap0( ac1.getProperties(), "iap0" );
    const AbcA::PropertyHeader * iap0Header =
        ac1.getProperties().getPropertyHeader( "iap0" );
    TESTING_ASSERT( IInt32ArrayProperty::matches( *iap0Header ) );
    TESTING_ASSERT( OInt32ArrayProperty::matches( *iap0Header ) );
    TESTING_ASSERT( ! IFloatArrayProperty::matches( *iap0Header ) );
    TESTING_ASSERT( ! IV3fArrayProperty::matches( *iap0Header ) );

    ISampleSelector ac1iap0iss;

    AbcA::index_t sampIdx = ac1iap0iss.getIndex( ac1iap0.getTimeSampling(),
        ac1iap0.getNumSamples() );

    std::cout << "sampIdx: " << sampIdx << std::endl;


    // an object contains a single compound property that contains all
    // sub-properties; all property access is through that.
    ICompoundProperty ac0Props = ac0.getProperties();

    ICompoundProperty acc0Props = acc0.getProperties();

    TESTING_ASSERT( ac0.getNumChildren() == 1 );
    TESTING_ASSERT( ac0Props.getNumProperties() == 4 );

    TESTING_ASSERT( acc0.getNumChildren() == 0 );
    TESTING_ASSERT( acc0Props.getNumProperties() == 6 );


    std::cout << "ac0 has " << ac0.getNumChildren() << " children and "
              << ac0Props.getNumProperties() << " properties." << std::endl;

    std::cout << "acc0 has " << acc0.getNumChildren() << " children and "
              << acc0Props.getNumProperties() << " properties." << std::endl;

    for ( size_t i = 0 ; i < acc0Props.getNumProperties() ; ++i )
    {
        std::cout << "acc0.getPropertyHeader( " << i << " ).getName(): "
                  << acc0Props.getPropertyHeader( i ).getName() << std::endl;
    }

    ICompoundProperty acc0CProp0( acc0Props, "acc0CProp0" );

    IInt32Property acc0cp0ip0( acc0CProp0, "acc0cp0ip0" );
    IInt32Property acc0cp0ip1( acc0CProp0, "acc0cp0ip1" );

    int acc0cp0ip0val = acc0cp0ip0.getValue();
    int acc0cp0ip1val = acc0cp0ip1.getValue();

    TESTING_ASSERT( acc0cp0ip0val == 0 );
    TESTING_ASSERT( acc0cp0ip1val == 1 );

    std::cout << "acc0cp0ip0 is " << acc0cp0ip0val << std::endl;
    std::cout << "acc0cp0ip1 is " << acc0cp0ip1val << std::endl;


    IV3fProperty ac0V3fp0( ac0Props, "ac0V3fp0", kStrictMatching );
    IN3fProperty ac0N3fp0( ac0Props, "ac0N3fp0", kStrictMatching );
    IP3fProperty ac0P3fp0( ac0Props, "ac0P3fp0", kStrictMatching );

    // we expect this to throw when doing strict matching
    TESTING_ASSERT_THROW (
        IP3fProperty( ac0Props, "ac0N3fp0", kStrictMatching ),
        Alembic::Util::Exception );

    IInt32Property acc0ip0( acc0Props, "acc0ip0" );

    IBoolProperty acc0bp0( acc0Props, "acc0bp0" );

    TESTING_ASSERT( ! acc0bp0.getValue() );

    std::cout << "bool scalar property acc0bp0 is false!  Huzzah!"
              << std::endl;

    TESTING_ASSERT( acc0ip0.getValue() == 99 );

    TESTING_ASSERT( ac0V3fp0.getValue() == scalarV3fval );
    TESTING_ASSERT( ac0N3fp0.getValue() == scalarV3fval );
    TESTING_ASSERT( ac0P3fp0.getValue() == scalarV3fval );

    std::cout << "acc0ip0 is " << acc0ip0.getValue() << std::endl;

    std::cout << "ac0V3fp0 is " << ac0V3fp0.getValue() << std::endl;

    IV3fArrayProperty acc0V3fap0( acc0Props, "acc0V3fap0", kStrictMatching );
    const AbcA::PropertyHeader * acc0V3fap0Header =
        acc0Props.getPropertyHeader( "acc0V3fap0" );
    TESTING_ASSERT( OV3fArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( IV3fArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( ! OFloatArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( ! IFloatArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( ! IInt32ArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( ! IP3fArrayProperty::matches( *acc0V3fap0Header ) );
    TESTING_ASSERT( ! IV3fProperty::matches( *acc0V3fap0Header ) );
    V3fArraySamplePtr acc0V3fap0SampPtr;

    IP3fArrayProperty acc0P3fap0( acc0Props, "acc0P3fap0", kStrictMatching );
    const AbcA::PropertyHeader * acc0P3fap0Header =
        acc0Props.getPropertyHeader( "acc0P3fap0" );
    TESTING_ASSERT( OP3fArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( IP3fArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( ! OFloatArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( ! IFloatArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( ! IInt32ArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( ! IV3fArrayProperty::matches( *acc0P3fap0Header ) );
    TESTING_ASSERT( ! IP3fProperty::matches( *acc0P3fap0Header ) );
    P3fArraySamplePtr acc0P3fap0SampPtr;

    IN3fArrayProperty acc0N3fap0( acc0Props, "acc0N3fap0", kStrictMatching );
    const AbcA::PropertyHeader * acc0N3fap0Header =
        acc0Props.getPropertyHeader( "acc0N3fap0" );
    TESTING_ASSERT( ON3fArrayProperty::matches( *acc0N3fap0Header ) );
    TESTING_ASSERT( IN3fArrayProperty::matches( *acc0N3fap0Header ) );
    TESTING_ASSERT( ! OFloatArrayProperty::matches( *acc0N3fap0Header ) );
    TESTING_ASSERT( ! IFloatArrayProperty::matches( *acc0N3fap0Header ) );
    TESTING_ASSERT( ! IInt32ArrayProperty::matches( *acc0N3fap0Header ) );
    TESTING_ASSERT( ! IV3fArrayProperty::matches( *acc0N3fap0Header ) );
    TESTING_ASSERT( ! IP3fProperty::matches( *acc0N3fap0Header ) );
    N3fArraySamplePtr acc0N3fap0SampPtr;

    TESTING_ASSERT_THROW (
        IP3fProperty( ac0Props, "acc0V3fap0", kStrictMatching ),
        Alembic::Util::Exception );

    IInt32ArrayProperty ac0iap0( ac0Props, "ac0iap0" );
    Int32ArraySamplePtr ac0iap0SampPtr;

    const size_t numReadV3fSamps = acc0V3fap0.getNumSamples();
    const size_t numReadN3fSamps = acc0N3fap0.getNumSamples();
    const size_t numReadP3fSamps = acc0P3fap0.getNumSamples();
    const size_t numReadIntSamps = ac0iap0.getNumSamples();

    TESTING_ASSERT( numReadIntSamps == numIntSamps );
    TESTING_ASSERT( numReadV3fSamps == numV3fSamps );
    TESTING_ASSERT( numReadN3fSamps == numV3fSamps );
    TESTING_ASSERT( numReadP3fSamps == numV3fSamps );

    std::cout << "acc0V3fap0 has " << numReadV3fSamps << " samples." << std::endl;

    std::cout << "ac0iap0 has " << numReadIntSamps << " samples." << std::endl;

    // first the V3f array property
    for ( size_t i = 0 ; i < numV3fSamps ; ++i )
    {
        acc0V3fap0.get( acc0V3fap0SampPtr, i );
        acc0N3fap0.get( acc0N3fap0SampPtr, i );
        acc0P3fap0.get( acc0P3fap0SampPtr, i );

        size_t numPoints = acc0V3fap0SampPtr->size();
        chrono_t time = acc0V3fap0.getTimeSampling()->getSampleTime( i );

        chrono_t compTime = v3fStartTime + ( i * dt );

        TESTING_ASSERT( numPoints == numV3fPoints );
        TESTING_ASSERT( numPoints == acc0N3fap0SampPtr->size() );
        TESTING_ASSERT( numPoints == acc0P3fap0SampPtr->size() );
        TESTING_ASSERT( Imath::equalWithAbsError( time, compTime,
                                                  CHRONO_EPSILON ) );

        float32_t elementVal = i + time;

        TESTING_ASSERT( (*acc0V3fap0SampPtr)[0][0] == elementVal );
        TESTING_ASSERT( (*acc0N3fap0SampPtr)[0][0] == elementVal );
        TESTING_ASSERT( (*acc0P3fap0SampPtr)[0][0] == elementVal );

        std::cout << "acc0V3fap0 at sample " << i << " is at time "
                  << time
                  << " and has " << numPoints << " points "
                  << " with the values:" << std::endl;

        for ( size_t j = 0 ; j < numPoints ; ++j )
        {
            std::cout << (*acc0V3fap0SampPtr)[j] << ", ";
        }

        std::cout << std::endl << std::endl;
    }

    // now the int array property
    for ( size_t i = 0 ; i < numIntSamps ; ++i )
    {
        ac0iap0.get( ac0iap0SampPtr, i );

        size_t numPoints = ac0iap0SampPtr->size();
        chrono_t time = ac0iap0.getTimeSampling()->getSampleTime( i );

        chrono_t compTime = intStartTime + ( i * dt );

        TESTING_ASSERT( Imath::equalWithAbsError( time, compTime,
                                                  CHRONO_EPSILON ) );

        TESTING_ASSERT( numPoints == numIntPoints );

        std::cout << "ac0iap0 at sample " << i << " is at time "
                  << time
                  << " and has " << numPoints << " points"
                  << " with the values:" << std::endl;

        for ( size_t j = 0 ; j < numPoints ; ++j )
        {
            Alembic::Util::int32_t val = (*ac0iap0SampPtr)[j];

            TESTING_ASSERT( val == ( Alembic::Util::int32_t ) ( i + j ) );

            std::cout << val << ", ";
        }

        std::cout << std::endl << std::endl;
    }
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    const std::string arkive( "test2archive.abc" );

    bool useOgawa = true;
    simpleTestOut( arkive, useOgawa );
    simpleTestIn( arkive, useOgawa );

#ifdef ALEMBIC_WITH_HDF5
    useOgawa = false;
    simpleTestOut( arkive, useOgawa );
    simpleTestIn( arkive, useOgawa );
#endif

    return 0;
}
