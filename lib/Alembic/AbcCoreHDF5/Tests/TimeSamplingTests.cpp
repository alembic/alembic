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

#include <iostream>
#include <vector>

#include <hdf5.h>

//-*****************************************************************************
namespace A5 = Alembic::AbcCoreHDF5;

namespace AbcA = Alembic::AbcCoreAbstract;

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
        TESTING_ASSERT_THROW(parent->createScalarProperty("uniform",
            AbcA::MetaData(), AbcA::DataType(Alembic::Util::kInt32POD, 1), 42),
            Alembic::Util::Exception);

        std::vector < double > timeSamps(1,-4.0);
        AbcA::TimeSamplingType tst(3.0);
        AbcA::TimeSampling ts(tst, timeSamps);
        uint32_t tsid = a->addTimeSampling(ts);

        AbcA::ScalarPropertyWriterPtr swp =
            parent->createScalarProperty("uniform", AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kInt32POD, 1), tsid);

        Alembic::Util::int32_t i = 0;

        swp->setSample(&i);

        i+=3;
        swp->setSample(&i);

        i+=3;
        swp->setSample(&i);

        timeSamps.clear();
        timeSamps.push_back(4.0);
        timeSamps.push_back(4.25);
        timeSamps.push_back(4.5);
        tst = AbcA::TimeSamplingType(3, 2.0);
        ts = AbcA::TimeSampling(tst, timeSamps);
        tsid = a->addTimeSampling(ts);

        AbcA::ScalarPropertyWriterPtr swp2 =
            parent->createScalarProperty("cyclic", AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kUint32POD, 1), tsid);

        Alembic::Util::uint32_t ui = 0;

        swp2->setSample(&ui);

        ui++;
        swp2->setSample(&ui);

        ui++;
        swp2->setSample(&ui);

        ui++;
        swp2->setSample(&ui);

        ui++;
        swp2->setSample(&ui);

        ui++;
        swp2->setSample(&ui);

        timeSamps.clear();
        timeSamps.push_back(-17.0);
        timeSamps.push_back(32.0);
        timeSamps.push_back(50.0);
        timeSamps.push_back(60.2);
        timeSamps.push_back(101.1);
        timeSamps.push_back(700.0);
        timeSamps.push_back(747.0);

        tst = AbcA::TimeSamplingType(AbcA::TimeSamplingType::kAcyclic);
        ts = AbcA::TimeSampling(tst, timeSamps);
        tsid = a->addTimeSampling(ts);

        AbcA::ScalarPropertyWriterPtr swp3 =
            parent->createScalarProperty("acyclic", AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kUint16POD, 1), tsid);

        Alembic::Util::uint16_t s = 0;
        swp3->setSample(&s);

        s++;
        swp3->setSample(&s);

        s++;

        swp3->setSample(&s);


        s++;
        swp3->setSample(&s);

        s++;
        swp3->setSample(&s);

        s++;
        swp3->setSample(&s);

        s++;
        swp3->setSample(&s);

        // Setting more than what we have acyclic samples for
        TESTING_ASSERT_THROW(swp3->setSample(&s),
            Alembic::Util::Exception);

        swp3->setTimeSamplingIndex(tsid);

        AbcA::ScalarPropertyWriterPtr swp4 =
            parent->createScalarProperty("identity", AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kInt16POD, 1), 0);

        Alembic::Util::int16_t ss = 35;


        swp4->setSample(&ss);

        ss = 37;
        swp4->setSample(&ss);

        ss = 1000;
        swp4->setSample(&ss);

        timeSamps.clear();
        timeSamps.push_back(0.0);
        tst = AbcA::TimeSamplingType(1.0);
        ts = AbcA::TimeSampling(tst, timeSamps);
        tsid = a->addTimeSampling(ts);

        AbcA::ScalarPropertyWriterPtr swp5 =
            parent->createScalarProperty("defaultUniform", AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kFloat32POD, 1), tsid);

        Alembic::Util::float32_t f = 0;

        swp5->setSample(&f);

        f+=1.1;
        swp5->setSample(&f);

        f+=1.1;
        swp5->setSample(&f);

        f+=1.1;
        swp5->setSample(&f);
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
            const AbcA::TimeSamplingPtr t = sp->getTimeSampling();

            switch (sp->getDataType().getPod())
            {
                case Alembic::Util::kInt16POD:
                {
                    // identity
                    TESTING_ASSERT( t->getTimeSamplingType().isUniform() );
                }
                break;

                case Alembic::Util::kUint16POD:
                {
                    // acylic
                    TESTING_ASSERT( t->getSampleTime(0) == -17.0 );
                    TESTING_ASSERT( sp->getNumSamples() == 7);
                    TESTING_ASSERT( t->getTimeSamplingType().isAcyclic() );
                    TESTING_ASSERT( t->getSampleTime(1) == 32.0 );
                    TESTING_ASSERT( t->getSampleTime(2) == 50.0 );
                    TESTING_ASSERT( t->getSampleTime(3) == 60.2 );
                    TESTING_ASSERT( t->getSampleTime(4) == 101.1 );
                    TESTING_ASSERT( t->getSampleTime(5) == 700.0 );
                    TESTING_ASSERT( t->getSampleTime(6) == 747.0 );
                    TESTING_ASSERT_THROW( t->getSampleTime(7),
                        Alembic::Util::Exception);
                }
                break;

                case Alembic::Util::kFloat32POD:
                {
                    TESTING_ASSERT( sp->getNumSamples() == 4);
                    TESTING_ASSERT( t->getTimeSamplingType().isUniform() );
                    TESTING_ASSERT( t->getSampleTime(0) == 0.0 );
                    TESTING_ASSERT( t->getSampleTime(1) == 1.0 );
                    TESTING_ASSERT( t->getSampleTime(2) == 2.0 );
                    TESTING_ASSERT( t->getSampleTime(3) == 3.0 );
                }
                break;

                case Alembic::Util::kInt32POD:
                {
                    // uniform
                    TESTING_ASSERT( sp->getNumSamples() == 3);
                    TESTING_ASSERT( t->getTimeSamplingType().isUniform() );
                    TESTING_ASSERT( t->getSampleTime(0) == -4.0 );
                    TESTING_ASSERT( t->getSampleTime(1) == -1.0 );
                    TESTING_ASSERT( t->getSampleTime(2) == 2.0 );
                }
                break;

                case Alembic::Util::kUint32POD:
                {
                    // cyclic
                    TESTING_ASSERT( sp->getNumSamples() == 6);
                    TESTING_ASSERT( t->getTimeSamplingType().isCyclic() );
                    TESTING_ASSERT( t->getSampleTime(0) == 4.0 );
                    TESTING_ASSERT( t->getSampleTime(1) == 4.25 );
                    TESTING_ASSERT( t->getSampleTime(2) == 4.5 );
                    TESTING_ASSERT( t->getSampleTime(3) == 6.0 );
                    TESTING_ASSERT( t->getSampleTime(4) == 6.25 );
                    TESTING_ASSERT( t->getSampleTime(5) == 6.5 );
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
