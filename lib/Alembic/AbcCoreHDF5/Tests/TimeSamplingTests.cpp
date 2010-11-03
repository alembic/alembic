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

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/Util/All.h>
#include <Alembic/MD5Hash/All.h>

#include <Alembic/AbcCoreHDF5/Tests/Assert.h>

#include <iostream>
#include <vector>

#include <hdf5.h>

//-*****************************************************************************
namespace A5 = Alembic::AbcCoreHDF5;

namespace AbcA = Alembic::AbcCoreAbstract::v1;

//-*****************************************************************************
void testTimeSamplingScalar()
{
    std::string archiveName = "timeSamplingScalar.abc";
    {
        A5::WriteArchive w;
        AbcA::ArchiveWriterPtr a = w(archiveName, AbcA::MetaData());
        AbcA::ObjectWriterPtr archive = a->getTop();

        AbcA::CompoundPropertyWriterPtr parent = archive->getProperties();

        // illegal time value
        TESTING_ASSERT_THROW(parent->createScalarProperty(
            AbcA::PropertyHeader("uniform", AbcA::kScalarProperty,
            AbcA::MetaData(), AbcA::DataType(Alembic::Util::kInt32POD, 1),
            AbcA::TimeSamplingType(-1.0))), Alembic::Util::Exception);

        TESTING_ASSERT_THROW(parent->createScalarProperty(
            AbcA::PropertyHeader("uniform", AbcA::kScalarProperty,
            AbcA::MetaData(), AbcA::DataType(Alembic::Util::kInt32POD, 1),
            AbcA::TimeSamplingType(1, 0.0))), Alembic::Util::Exception);

        AbcA::ScalarPropertyWriterPtr swp =
            parent->createScalarProperty(AbcA::PropertyHeader("uniform",
                AbcA::kScalarProperty, AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kInt32POD, 1),
                AbcA::TimeSamplingType(3.0)));

        Alembic::Util::int32_t i = 0;

        swp->setSample(0, -4.0, &i);

        i+=3;
        swp->setSample(1, 0.0, &i);

        i+=3;
        swp->setSample(2, 0.0, &i);

        // you can't skip samples
        TESTING_ASSERT_THROW(swp->setSample(5, 0.0, &i),
            Alembic::Util::Exception);

        AbcA::ScalarPropertyWriterPtr swp2 =
            parent->createScalarProperty(AbcA::PropertyHeader("cyclic",
                AbcA::kScalarProperty, AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kUint32POD, 1),
                AbcA::TimeSamplingType(3, 2.0)));

        Alembic::Util::uint32_t ui = 0;

        swp2->setSample(0, 4.0, &ui);

        // writing this value for cyclic time would be greater than
        // our time per cycle
        TESTING_ASSERT_THROW(swp2->setSample(1, 34.0, &ui),
            Alembic::Util::Exception);

        ui++;
        swp2->setSample(1, 4.25, &ui);

        ui++;
        swp2->setSample(2, 4.5, &ui);

        ui++;
        swp2->setSample(3, 5.0, &ui);

        ui++;
        swp2->setSample(4, 5.25, &ui);

        ui++;
        swp2->setSample(5, 5.5, &ui);

        AbcA::ScalarPropertyWriterPtr swp3 =
            parent->createScalarProperty(AbcA::PropertyHeader("acyclic",
                AbcA::kScalarProperty, AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kUint16POD, 1),
                AbcA::TimeSamplingType(AbcA::TimeSamplingType::kAcyclic)));

        Alembic::Util::uint16_t s = 0;
        swp3->setSample(0, -17.0, &s);

        s++;
        swp3->setSample(1, 32.0, &s);

        s++;

        // time must be greater than the last sample!
        TESTING_ASSERT_THROW(swp3->setSample(2, 32.0, &s),
            Alembic::Util::Exception);
        TESTING_ASSERT_THROW(swp3->setSample(2, 7.0, &s),
            Alembic::Util::Exception);
        swp3->setSample(2, 50.0, &s);


        s++;
        swp3->setSample(3, 60.2, &s);

        s++;
        swp3->setSample(4, 101.1, &s);

        s++;
        swp3->setSample(5, 700.0, &s);

        s++;
        swp3->setSample(6, 747.0, &s);

        AbcA::ScalarPropertyWriterPtr swp4 =
            parent->createScalarProperty(AbcA::PropertyHeader("identity",
                AbcA::kScalarProperty, AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kInt16POD, 1),
                AbcA::TimeSamplingType()));
        Alembic::Util::int16_t ss = 35;

        // -17 shouldn't matter because we are be identity
        swp4->setSample(0, -17.0, &ss);

        ss = 37;
        swp4->setSample(1, 43.0, &ss);

        ss = 1000;
        swp4->setSample(2, 50.0, &ss);

        AbcA::ScalarPropertyWriterPtr swp5 =
            parent->createScalarProperty(AbcA::PropertyHeader("defaultUniform",
                AbcA::kScalarProperty, AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kFloat32POD, 1),
                AbcA::TimeSamplingType(1.0)));

        Alembic::Util::float32_t f = 0;

        swp5->setSample(0, 0.0, &f);

        f+=1.1;
        swp5->setSample(1, 0.0, &f);

        f+=1.1;
        swp5->setSample(2, 0.0, &f);

        f+=1.1;
        swp5->setSample(3, 0.0, &f);
    }

    {
        A5::ReadArchive r;
        AbcA::ArchiveReaderPtr a = r( archiveName );
        AbcA::ObjectReaderPtr archive = a->getTop();
        AbcA::CompoundPropertyReaderPtr parent = archive->getProperties();
        TESTING_ASSERT(parent->getNumProperties() == 5);

        for ( size_t i = 0; i < parent->getNumProperties(); ++i)
        {
            AbcA::BasePropertyReaderPtr bp = parent->getProperty( i );
            AbcA::ScalarPropertyReaderPtr sp = bp->asScalarPtr();
            const AbcA::TimeSampling t = sp->getTimeSampling();

            switch (sp->getDataType().getPod())
            {
                case Alembic::Util::kInt16POD:
                {
                    // identity
                    TESTING_ASSERT( sp->getTimeSamplingType().isIdentity() );
                }
                break;

                case Alembic::Util::kUint16POD:
                {
                    // acylic
                    TESTING_ASSERT( t.getSampleTime(0) == -17.0 );
                    TESTING_ASSERT( t.getNumSamples() == 7);
                    TESTING_ASSERT( sp->getTimeSamplingType().isAcyclic() );
                    TESTING_ASSERT( t.getSampleTime(1) == 32.0 );
                    TESTING_ASSERT( t.getSampleTime(2) == 50.0 );
                    TESTING_ASSERT( t.getSampleTime(3) == 60.2 );
                    TESTING_ASSERT( t.getSampleTime(4) == 101.1 );
                    TESTING_ASSERT( t.getSampleTime(5) == 700.0 );
                    TESTING_ASSERT( t.getSampleTime(6) == 747.0 );
                    TESTING_ASSERT_THROW( t.getSampleTime(7),
                        Alembic::Util::Exception);
                }
                break;

                case Alembic::Util::kFloat32POD:
                {
                    // uniform start time of 0
                    std::cout << "t.getNumSamples(): " <<
                        t.getNumSamples() << std::endl;
                    TESTING_ASSERT( t.getNumSamples() == 4);
                    TESTING_ASSERT( sp->getTimeSamplingType().isUniform() );
                    TESTING_ASSERT( t.getSampleTime(0) == 0.0 );
                    TESTING_ASSERT( t.getSampleTime(1) == 1.0 );
                    TESTING_ASSERT( t.getSampleTime(2) == 2.0 );
                    TESTING_ASSERT( t.getSampleTime(3) == 3.0 );
                    TESTING_ASSERT_THROW( t.getSampleTime(4),
                        Alembic::Util::Exception);
                }
                break;

                case Alembic::Util::kInt32POD:
                {
                    // uniform
                    TESTING_ASSERT( t.getNumSamples() == 3);
                    TESTING_ASSERT( sp->getTimeSamplingType().isUniform() );
                    TESTING_ASSERT( t.getSampleTime(0) == -4.0 );
                    TESTING_ASSERT( t.getSampleTime(1) == -1.0 );
                    TESTING_ASSERT( t.getSampleTime(2) == 2.0 );
                    TESTING_ASSERT_THROW( t.getSampleTime(3),
                        Alembic::Util::Exception);
                }
                break;

                case Alembic::Util::kUint32POD:
                {
                    // cyclic
                    TESTING_ASSERT( t.getNumSamples() == 6);
                    TESTING_ASSERT( sp->getTimeSamplingType().isCyclic() );
                    TESTING_ASSERT( t.getSampleTime(0) == 4.0 );
                    TESTING_ASSERT( t.getSampleTime(1) == 4.25 );
                    TESTING_ASSERT( t.getSampleTime(2) == 4.5 );
                    TESTING_ASSERT( t.getSampleTime(3) == 6.0 );
                    TESTING_ASSERT( t.getSampleTime(4) == 6.25 );
                    TESTING_ASSERT( t.getSampleTime(5) == 6.5 );
                    TESTING_ASSERT_THROW( t.getSampleTime(6),
                        Alembic::Util::Exception);
                }
                break;

                default:
                    TESTING_ASSERT(false);
                break;
            }
        }  // for
    }
}

int main ( int argc, char *argv[] )
{
    testTimeSamplingScalar();
    return 0;
}
