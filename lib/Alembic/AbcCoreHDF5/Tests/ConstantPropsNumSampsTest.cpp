//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

//-*****************************************************************************
namespace A5 = Alembic::AbcCoreHDF5;

namespace AbcA = Alembic::AbcCoreAbstract;

using namespace AbcA;

//-*****************************************************************************
void testProps()
{
    Alembic::Util::int32_t intVal = 15;

    float32_t f0 = 67.3f;

    size_t floatArraySize = 14;

    chrono_t t0 = 0.25;

    chrono_t dt = 1.0 / 24.0;

    DataType floatDT( kFloat32POD, 1 );

    Dimensions dims( floatArraySize );

    std::string archiveName = "constantPropsTest.abc";

    std::vector<float32_t> fvals;

    for ( size_t i = 0 ; i < floatArraySize ; i++ )
    {
        fvals.push_back( f0 + i );
    }


    {
        A5::WriteArchive w;
        ArchiveWriterPtr a = w(archiveName, AbcA::MetaData());
        std::vector < chrono_t > samps(1, t0);
        AbcA::TimeSampling ts(TimeSamplingType(dt), samps);
        a->addTimeSampling(ts);

        ObjectWriterPtr top = a->getTop();
        ObjectWriterPtr child = top->createChild(
            ObjectHeader( "wow", MetaData() ) );

        CompoundPropertyWriterPtr props = child->getProperties();

        ScalarPropertyWriterPtr intProp = props->createScalarProperty("intProp",
            MetaData(), DataType( kInt32POD, 1 ), 1);

        ArrayPropertyWriterPtr floatArrayProp = props->createArrayProperty(
            "floatArrayProp", MetaData(), DataType( kFloat32POD, 1 ), 1);

        intProp->setSample( &intVal );

        for ( size_t i = 0 ; i < 500 ; i++ )
        {
            floatArrayProp->setSample(ArraySample( &(fvals.front()), floatDT,
                dims ) );
        }

    }

    {
        A5::ReadArchive r;
        ArchiveReaderPtr a = r( archiveName );
        ObjectReaderPtr top = a->getTop();
        TESTING_ASSERT( top->getNumChildren() == 1 );

        AbcA::ObjectReaderPtr child = top->getChild(0);
        TESTING_ASSERT( child->getName() == "wow" );

        CompoundPropertyReaderPtr props = child->getProperties();

        TESTING_ASSERT( props->getNumProperties() == 2 );

        ScalarPropertyReaderPtr intProp = props->getScalarProperty( "intProp" );
        TESTING_ASSERT( intProp->isConstant() );
        TESTING_ASSERT( intProp->getNumSamples() == 1 );

        ArrayPropertyReaderPtr floatArrayProp = \
            props->getArrayProperty( "floatArrayProp" );

        TESTING_ASSERT( floatArrayProp->getNumSamples() == 500 );
        TESTING_ASSERT( floatArrayProp->isConstant() );

    }
}

int main ( int argc, char *argv[] )
{
    testProps();
    return 0;
}
