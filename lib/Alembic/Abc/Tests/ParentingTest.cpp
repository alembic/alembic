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
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

#ifdef ALEMBIC_WITH_HDF5
#include <Alembic/AbcCoreHDF5/All.h>
#endif

#include <ImathMath.h>

#include <limits>

namespace Abc = Alembic::Abc;
using namespace Abc;

namespace AbcF = Alembic::AbcCoreFactory;

using Alembic::AbcCoreAbstract::chrono_t;
using Alembic::AbcCoreAbstract::index_t;
using Alembic::Util::uint32_t;
using Alembic::Util::float32_t;

static const V3f scalarV3fval( -1.0f, 32.0f, -90.0f );

static const chrono_t startTime = 5.7;

static const chrono_t dt = 1.0 / 24.0;

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

    // all child Objects in an Archive are actually children of the single
    // top Object in an Archive
    OObject topobj = archive.getTop();

    // 0th archive child
    OObject ac0( topobj, "ac0" );
    // 0th archive child child
    OObject acc0( ac0, "acc0" );

    // all property manipulation is through an Object's single CompoundProperty
    OCompoundProperty ac0Props = ac0.getProperties();
    OCompoundProperty acc0Props = acc0.getProperties();

    // some scalar props
    OV3fProperty ac0V3fp0( ac0Props, "ac0V3fp0" );
    ac0V3fp0.set( scalarV3fval );

    TimeSampling ts(dt, startTime);
    Alembic::Util::uint32_t tsidx = archive.addTimeSampling(ts);

    // now some array props
    OV3fArrayProperty acc0V3fap0( acc0Props, "acc0V3fap0", tsidx );

    chrono_t t = startTime;
    const size_t numPoints = 7;
    const size_t numSamps = 5;

    std::vector<V3f> points( numPoints );

    for ( size_t i = 0 ; i < numSamps ; ++i )
    {
        for ( std::vector<V3f>::iterator iter = points.begin() ;
              iter != points.end() ; ++iter )
        {
            (*iter) = V3f( i + t, i + t, i + t );
        }

        acc0V3fap0.set( points );
        t += dt;
    }

    // OK, test some parentage
    {
        OObject _foo;
        OObject _parent = acc0;
        while ( _parent )
        {
            std::cout << "_parent.getFullName(): " << _parent.getFullName()
                      << std::endl;
            _foo = _parent.getParent();
            _parent = _foo;
        }
    }

    {
        OCompoundProperty _foo;
        OCompoundProperty _parent = acc0Props;
        while ( _parent )
        {
            std::cout << "_parent.getName(): " << _parent.getName()
                      << std::endl;
            _foo = _parent.getParent();
            _parent = _foo;
        }
    }

    // if the program exits, it means parenting works.
}

//-*****************************************************************************
void simpleTestIn( const std::string &iArchiveName )
{
    AbcF::IFactory factory;
    factory.setPolicy(  ErrorHandler::kThrowPolicy );
    AbcF::IFactory::CoreType coreType;
    IArchive archive = factory.getArchive(iArchiveName, coreType);

    // an archive has a single top object which contains all its children
    IObject topObject = archive.getTop();

    IObject ac0( topObject, "ac0" );
    IObject acc0( ac0, "acc0" );

    // an object contains a single compound property that contains all
    // sub-properties; all property access is through that.
    ICompoundProperty ac0Props = ac0.getProperties();
    ICompoundProperty acc0Props = acc0.getProperties();

    IV3fProperty ac0V3fp0( ac0Props, "ac0V3fp0" );

    IV3fArrayProperty acc0V3fap0( acc0Props, "acc0V3fap0" );
    V3fArraySamplePtr acc0V3fap0SampPtr;

    const size_t numSamps = acc0V3fap0.getNumSamples();

    std::cout << "acc0V3fap0 has " << numSamps << " samples." << std::endl;

    for ( size_t i = 0 ; i < numSamps ; ++i )
    {
        acc0V3fap0.get( acc0V3fap0SampPtr, i );
        size_t numPoints = acc0V3fap0SampPtr->size();

        chrono_t time = acc0V3fap0.getTimeSampling()->getSampleTime( i );

        chrono_t compTime = startTime + ( i * dt );

        std::cout << "acc0fap0 at sample " << i << " is at time "
                  << time
                  << " and has " << numPoints << " points "
                  << " with the values:" << std::endl;

        float32_t elementVal = i + time;

        TESTING_ASSERT( (*acc0V3fap0SampPtr)[0][0] == elementVal );

        TESTING_ASSERT( Imath::equalWithAbsError( time, compTime,
                                                  CHRONO_EPSILON ) );


        for ( size_t j = 0 ; j < numPoints ; ++j )
        {
            std::cout << (*acc0V3fap0SampPtr)[j] << ", ";
        }
        std::cout << std::endl << std::endl;
    }

    // OK, test some parentage
    {
        IObject _foo;
        IObject _parent = acc0;
        while ( _parent )
        {
            std::cout << "_parent.getFullName(): " << _parent.getFullName()
                      << std::endl;
            _foo = _parent.getParent();
            _parent = _foo;
        }
    }

    {
        ICompoundProperty _foo;
        ICompoundProperty _parent = acc0Props;
        while ( _parent )
        {
            std::cout << "_parent.getName(): " << _parent.getName()
                      << std::endl;
            _foo = _parent.getParent();
            _parent = _foo;
        }
    }

    // if the program exits, it means parenting works
}

void scopingTest(bool useOgawa)
{
    {
        ODoubleProperty propScalar;
        ODoubleArrayProperty propArray;
        {

            OArchive archive;
            if (useOgawa)
            {
                archive = CreateArchiveWithInfo(
                    Alembic::AbcCoreOgawa::WriteArchive(), "propScopeTest.abc",
                    "Alembic test", "", MetaData() );
            }
#ifdef ALEMBIC_WITH_HDF5
            else
            {
                archive = CreateArchiveWithInfo(
                    Alembic::AbcCoreHDF5::WriteArchive(), "propScopeTest.abc",
                    "Alembic test", "", MetaData() );
            }
#endif

            OObject childA( archive.getTop(), "a" );

            propScalar = ODoubleProperty(childA.getProperties(), "scalar", 0);

            propArray = ODoubleArrayProperty(childA.getProperties(),
                "array", 0);
        }

        std::vector< double > values(3, 2.0);
        propArray.set( values );
        propScalar.set( 4.0 );
        propScalar.set( 5.0 );
        TESTING_ASSERT(
            propArray.getParent().getObject().getArchive().getName() ==
            "propScopeTest.abc");
        TESTING_ASSERT(
            propScalar.getParent().getObject().getArchive().getName() ==
            "propScopeTest.abc");
    }

    {
        IDoubleProperty propScalar;
        IDoubleArrayProperty propArray;
        {
            AbcF::IFactory factory;
            AbcF::IFactory::CoreType coreType;
            IArchive archive = factory.getArchive("propScopeTest.abc",
                                                  coreType);
            IObject top(archive.getTop(), "a");
            propScalar = IDoubleProperty(top.getProperties(), "scalar");
            propArray = IDoubleArrayProperty(top.getProperties(), "array");
        }
        TESTING_ASSERT(
            propArray.getParent().getObject().getArchive().getName() ==
            "propScopeTest.abc");
        TESTING_ASSERT(
            propScalar.getParent().getObject().getArchive().getName() ==
            "propScopeTest.abc");

        DoubleArraySamplePtr samp;
        size_t sampNum = 0;
        propArray.get(samp, sampNum);
        TESTING_ASSERT( samp->size() == 3 );
        TESTING_ASSERT( (*samp)[0] == 2.0 && (*samp)[1] == 2.0 &&
                        (*samp)[2] == 2.0 );

        double scalarVal = 0.0;
        propScalar.get( scalarVal, sampNum );
        TESTING_ASSERT( scalarVal == 4.0 );
        sampNum ++;
        propScalar.get( scalarVal, sampNum );
        TESTING_ASSERT( scalarVal == 5.0 );
    }
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    const std::string arkive( "parentstest.abc" );

    bool useOgawa = true;
    simpleTestOut( arkive, useOgawa );
    simpleTestIn( arkive );
    scopingTest(useOgawa);
#ifdef ALEMBIC_WITH_HDF5
    useOgawa = false;
    simpleTestOut( arkive, useOgawa );
    simpleTestIn( arkive );
    scopingTest(useOgawa);
#endif
    return 0;
}
