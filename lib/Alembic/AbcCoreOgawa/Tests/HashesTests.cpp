//-*****************************************************************************
//
// Copyright (c) 2013,
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
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/Util/All.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

#include <iostream>
#include <vector>

//-*****************************************************************************
namespace AO = Alembic::AbcCoreOgawa;

namespace ABCA = Alembic::AbcCoreAbstract;
void testArrayPropHashes(bool iUseMMap)
{
    std::string archiveName = "arrayHashTest.abc";
    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();
        ABCA::ObjectWriterPtr child;

        // add a time sampling for later use
        std::vector < double > timeSamps(1,-4.0);
        ABCA::TimeSamplingType tst(3.0);
        ABCA::TimeSampling ts(tst, timeSamps);
        a->addTimeSampling(ts);

        // 2 objects without any properties whatsoever
        archive->createChild(ABCA::ObjectHeader("emptyA", ABCA::MetaData()));
        archive->createChild(ABCA::ObjectHeader("emptyB", ABCA::MetaData()));

        // 2 objects with with the same property with no samples
        child = archive->createChild(ABCA::ObjectHeader(
            "1propA", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propB", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        // 2 objects with with the same property with a differnt name from above
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAName", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBName", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        // 2 objects with with the same property with a differnt MetaData
        ABCA::MetaData m;
        m.set("Bleep", "bloop");
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAMeta", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty", m,
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBMeta", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty", m,
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        // 2 objects with with the same property with a different POD
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAPod", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kFloat32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBPod", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kFloat32POD, 1), 0);

        // 2 objects with with the same property with a different extent
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAExtent", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 2), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBExtent", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 2), 0);

        // 2 objects with with the same property with a differnt time sampling
        child = archive->createChild(ABCA::ObjectHeader(
            "1propATS", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 1);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBTS", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 1);

        // 2 objects with 1 sample
        ABCA::ArrayPropertyWriterPtr awp;
        std::vector <Alembic::Util::int32_t> vali(4, 0);
        Alembic::Util::Dimensions dims(4);
        ABCA::DataType i32d(Alembic::Util::kInt32POD, 1);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propA1Samp", ABCA::MetaData()));
        awp = child->getProperties()->createArrayProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        awp->setSample(ABCA::ArraySample(&(vali.front()), i32d, dims));

        child = archive->createChild(ABCA::ObjectHeader(
            "1propB1Samp", ABCA::MetaData()));
        awp = child->getProperties()->createArrayProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        awp->setSample(ABCA::ArraySample(&(vali.front()), i32d, dims));

        // 2 objects with 2 samples, no repeats
        std::vector <Alembic::Util::int32_t> valiB(4, 1);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propA2Samp", ABCA::MetaData()));
        awp = child->getProperties()->createArrayProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        awp->setSample(ABCA::ArraySample(&(vali.front()), i32d, dims));
        awp->setSample(ABCA::ArraySample(&(valiB.front()), i32d, dims));

        child = archive->createChild(ABCA::ObjectHeader(
            "1propB2Samp", ABCA::MetaData()));
        awp = child->getProperties()->createArrayProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        awp->setSample(ABCA::ArraySample(&(vali.front()), i32d, dims));
        awp->setSample(ABCA::ArraySample(&(valiB.front()), i32d, dims));

        // 2 objects with 4 samples, with repeats
        child = archive->createChild(ABCA::ObjectHeader(
            "1propA4Samp", ABCA::MetaData()));
        awp = child->getProperties()->createArrayProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        awp->setSample(ABCA::ArraySample(&(vali.front()), i32d, dims));
        awp->setSample(ABCA::ArraySample(&(vali.front()), i32d, dims));
        awp->setSample(ABCA::ArraySample(&(valiB.front()), i32d, dims));
        awp->setSample(ABCA::ArraySample(&(valiB.front()), i32d, dims));

        child = archive->createChild(ABCA::ObjectHeader(
            "1propB4Samp", ABCA::MetaData()));
        awp = child->getProperties()->createArrayProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        awp->setSample(ABCA::ArraySample(&(vali.front()), i32d, dims));
        awp->setSample(ABCA::ArraySample(&(vali.front()), i32d, dims));
        awp->setSample(ABCA::ArraySample(&(valiB.front()), i32d, dims));
        awp->setSample(ABCA::ArraySample(&(valiB.front()), i32d, dims));

        // 2 objects with 1 samplem different dimensions
        Alembic::Util::Dimensions dimsB;
        dimsB.setRank(2);
        dimsB[0] = 2;
        dimsB[1] = 2;

        child = archive->createChild(ABCA::ObjectHeader(
            "1propA1Dims", ABCA::MetaData()));
        awp = child->getProperties()->createArrayProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        awp->setSample(ABCA::ArraySample(&(vali.front()), i32d, dimsB));

        child = archive->createChild(ABCA::ObjectHeader(
            "1propB1Dims", ABCA::MetaData()));
        awp = child->getProperties()->createArrayProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        awp->setSample(ABCA::ArraySample(&(vali.front()), i32d, dimsB));

        // 2 objects with with the same 2 properties with no samples
        child = archive->createChild(ABCA::ObjectHeader(
            "2propA", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty", ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        child->getProperties()->createArrayProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "2propB", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        child->getProperties()->createArrayProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        // 2 objects with with the same 2 properties created in opposite order
        child = archive->createChild(ABCA::ObjectHeader(
            "2propASwap", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        child->getProperties()->createArrayProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "2propBSwap", ABCA::MetaData()));
        child->getProperties()->createArrayProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        child->getProperties()->createArrayProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
    }

    {
        AO::ReadArchive r(1, iUseMMap);
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();

        TESTING_ASSERT(archive->getNumChildren() == 26);

        // every 2 hashes should be the same
        for (size_t i = 0; i < archive->getNumChildren(); i += 2)
        {
            Alembic::Util::Digest dA, dB;
            TESTING_ASSERT(archive->getChild(i)->getPropertiesHash(dA) &&
                archive->getChild(i+1)->getPropertiesHash(dB));
            TESTING_ASSERT(dA == dB);
        }

        // make sure that every 2 child objects have different properties hashes
        for (size_t i = 0; i < archive->getNumChildren() / 2; ++i)
        {
            Alembic::Util::Digest dA;
            archive->getChild(i*2)->getPropertiesHash(dA);
            for (size_t j = i + 1; j < archive->getNumChildren() / 2; ++j)
            {
                Alembic::Util::Digest dB;
                archive->getChild(j*2)->getPropertiesHash(dB);
                TESTING_ASSERT(dA != dB);
            }
        }
    }
}

void testScalarPropHashes(bool iUseMMap)
{
    std::string archiveName = "scalarHashTest.abc";
    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();
        ABCA::ObjectWriterPtr child;

        // add a time sampling for later use
        std::vector < double > timeSamps(1,-4.0);
        ABCA::TimeSamplingType tst(3.0);
        ABCA::TimeSampling ts(tst, timeSamps);
        a->addTimeSampling(ts);

        // 2 objects without any properties whatsoever
        archive->createChild(ABCA::ObjectHeader("emptyA", ABCA::MetaData()));
        archive->createChild(ABCA::ObjectHeader("emptyB", ABCA::MetaData()));

        // 2 objects with with the same property with no samples
        child = archive->createChild(ABCA::ObjectHeader(
            "1propA", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propB", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        // 2 objects with with the same property with a differnt name from above
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAName", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBName", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        // 2 objects with with the same property with a differnt MetaData
        ABCA::MetaData m;
        m.set("Bleep", "bloop");
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAMeta", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty", m,
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBMeta", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty", m,
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        // 2 objects with with the same property with a different POD
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAPod", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kFloat32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBPod", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kFloat32POD, 1), 0);

        // 2 objects with with the same property with a different extent
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAExtent", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 2), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBExtent", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 2), 0);

        // 2 objects with with the same property with a differnt time sampling
        child = archive->createChild(ABCA::ObjectHeader(
            "1propATS", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 1);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBTS", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 1);

        // 2 objects with 1 sample
        ABCA::ScalarPropertyWriterPtr swp;
        Alembic::Util::int32_t vali = 0;
        ABCA::DataType i32d(Alembic::Util::kInt32POD, 1);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propA1Samp", ABCA::MetaData()));
        swp = child->getProperties()->createScalarProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        swp->setSample(&vali);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propB1Samp", ABCA::MetaData()));
        swp = child->getProperties()->createScalarProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        swp->setSample(&vali);

        // 2 objects with 2 samples, no repeats
        Alembic::Util::int32_t valiB = 1;

        child = archive->createChild(ABCA::ObjectHeader(
            "1propA2Samp", ABCA::MetaData()));
        swp = child->getProperties()->createScalarProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        swp->setSample(&vali);
        swp->setSample(&valiB);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propB2Samp", ABCA::MetaData()));
        swp = child->getProperties()->createScalarProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        swp->setSample(&vali);
        swp->setSample(&valiB);

        // 2 objects with 4 samples, with repeats
        child = archive->createChild(ABCA::ObjectHeader(
            "1propA4Samp", ABCA::MetaData()));
        swp = child->getProperties()->createScalarProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        swp->setSample(&vali);
        swp->setSample(&vali);
        swp->setSample(&valiB);
        swp->setSample(&valiB);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propB4Samp", ABCA::MetaData()));
        swp = child->getProperties()->createScalarProperty("int",
            ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        swp->setSample(&vali);
        swp->setSample(&vali);
        swp->setSample(&valiB);
        swp->setSample(&valiB);

        // 2 objects with with the same 2 properties with no samples
        child = archive->createChild(ABCA::ObjectHeader(
            "2propA", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty", ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        child->getProperties()->createScalarProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "2propB", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        child->getProperties()->createScalarProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        // 2 objects with with the same 2 properties created in opposite order
        child = archive->createChild(ABCA::ObjectHeader(
            "2propASwap", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        child->getProperties()->createScalarProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "2propBSwap", ABCA::MetaData()));
        child->getProperties()->createScalarProperty("null",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        child->getProperties()->createScalarProperty("empty",  ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
    }

    {
        AO::ReadArchive r(1, iUseMMap);
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();

        TESTING_ASSERT(archive->getNumChildren() == 24);

        // every 2 hashes should be the same
        for (size_t i = 0; i < archive->getNumChildren(); i += 2)
        {
            Alembic::Util::Digest dA, dB;
            TESTING_ASSERT(archive->getChild(i)->getPropertiesHash(dA) &&
                archive->getChild(i+1)->getPropertiesHash(dB));
            TESTING_ASSERT(dA == dB);
        }

        // make sure that every 2 child objects have different properties hashes
        for (size_t i = 0; i < archive->getNumChildren() / 2; ++i)
        {
            Alembic::Util::Digest dA;
            archive->getChild(i*2)->getPropertiesHash(dA);
            for (size_t j = i + 1; j < archive->getNumChildren() / 2; ++j)
            {
                Alembic::Util::Digest dB;
                archive->getChild(j*2)->getPropertiesHash(dB);
                TESTING_ASSERT(dA != dB);
            }
        }
    }
}

void testCompoundPropHashes(bool iUseMMap)
{
    std::string archiveName = "compoundHashTest.abc";
    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();
        ABCA::ObjectWriterPtr child;

        // 2 objects without any properties whatsoever
        archive->createChild(ABCA::ObjectHeader("emptyA", ABCA::MetaData()));
        archive->createChild(ABCA::ObjectHeader("emptyB", ABCA::MetaData()));

        // 2 objects with with the same property with no samples
        child = archive->createChild(ABCA::ObjectHeader(
            "1propA", ABCA::MetaData()));
        child->getProperties()->createCompoundProperty("empty",
            ABCA::MetaData());

        child = archive->createChild(ABCA::ObjectHeader(
            "1propB", ABCA::MetaData()));
        child->getProperties()->createCompoundProperty("empty",
            ABCA::MetaData());

        // 2 objects with with the same property with a differnt name from above
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAName", ABCA::MetaData()));
        child->getProperties()->createCompoundProperty("null",
            ABCA::MetaData());

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBName", ABCA::MetaData()));
        child->getProperties()->createCompoundProperty("null",
            ABCA::MetaData());

        // 2 objects with with the same property with a different MetaData
        ABCA::MetaData m;
        m.set("Bleep", "bloop");
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAMeta", ABCA::MetaData()));
        child->getProperties()->createCompoundProperty("empty", m);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBMeta", ABCA::MetaData()));
        child->getProperties()->createCompoundProperty("empty", m);

        // 2 objects with compound that has a child compound
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAChild", ABCA::MetaData()));
        child->getProperties()->createCompoundProperty("withChild",
            ABCA::MetaData())->createCompoundProperty("child",
            ABCA::MetaData());

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBChild", ABCA::MetaData()));
        child->getProperties()->createCompoundProperty("withChild",
            ABCA::MetaData())->createCompoundProperty("child",
            ABCA::MetaData());

        // 2 objects with compound that has various children
        ABCA::CompoundPropertyWriterPtr childProp;
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAProps", ABCA::MetaData()));
        childProp = child->getProperties()->createCompoundProperty("child",
            ABCA::MetaData());
        childProp->createCompoundProperty("child", ABCA::MetaData());
        childProp->createScalarProperty("empty", ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBProps", ABCA::MetaData()));
        childProp = child->getProperties()->createCompoundProperty("child",
            ABCA::MetaData());
        childProp->createCompoundProperty("child", ABCA::MetaData());
        childProp->createScalarProperty("empty", ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        // 2 objects with compound that has hierarchy
        ABCA::CompoundPropertyWriterPtr grandChildProp;
        child = archive->createChild(ABCA::ObjectHeader(
            "1propAHier", ABCA::MetaData()));
        childProp = child->getProperties()->createCompoundProperty("child",
            ABCA::MetaData());
        grandChildProp = childProp->createCompoundProperty("child",
            ABCA::MetaData());
        childProp->createScalarProperty("empty", ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        grandChildProp->createScalarProperty("empty", ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        grandChildProp->createCompoundProperty("child", ABCA::MetaData());

        child = archive->createChild(ABCA::ObjectHeader(
            "1propBHier", ABCA::MetaData()));
        childProp = child->getProperties()->createCompoundProperty("child",
            ABCA::MetaData());
        grandChildProp = childProp->createCompoundProperty("child",
            ABCA::MetaData());
        childProp->createScalarProperty("empty", ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        grandChildProp->createScalarProperty("empty", ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        grandChildProp->createCompoundProperty("child", ABCA::MetaData());
    }

    {
        AO::ReadArchive r(1, iUseMMap);
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();

        TESTING_ASSERT(archive->getNumChildren() == 14);

        // every 2 hashes should be the same
        for (size_t i = 0; i < archive->getNumChildren(); i += 2)
        {
            Alembic::Util::Digest dA, dB;
            TESTING_ASSERT(archive->getChild(i)->getPropertiesHash(dA) &&
                archive->getChild(i+1)->getPropertiesHash(dB));
            TESTING_ASSERT(dA == dB);
        }

        // make sure that every 2 child objects have different properties hashes
        for (size_t i = 0; i < archive->getNumChildren() / 2; ++i)
        {
            Alembic::Util::Digest dA;
            archive->getChild(i*2)->getPropertiesHash(dA);
            for (size_t j = i + 1; j < archive->getNumChildren() / 2; ++j)
            {
                Alembic::Util::Digest dB;
                archive->getChild(j*2)->getPropertiesHash(dB);
                TESTING_ASSERT(dA != dB);
            }
        }
    }
}

void testObjectHashes(bool iUseMMap)
{
    std::string archiveName = "objectHashTest.abc";
    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();
        ABCA::ObjectWriterPtr child;

        // 2 objects without any children whatsoever
        archive->createChild(ABCA::ObjectHeader("emptyA", ABCA::MetaData()));
        archive->createChild(ABCA::ObjectHeader("emptyB", ABCA::MetaData()));

        // 2 objects with one child object
        child = archive->createChild(ABCA::ObjectHeader("1ChildA",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("child", ABCA::MetaData()));

        child = archive->createChild(ABCA::ObjectHeader("1ChildB",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("child", ABCA::MetaData()));

        // 2 objects with one child object with a different name
        child = archive->createChild(ABCA::ObjectHeader("1ChildAName",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("empty", ABCA::MetaData()));

        child = archive->createChild(ABCA::ObjectHeader("1ChildBName",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("empty", ABCA::MetaData()));

        // 2 objects with one child object with different MetaData
        ABCA::MetaData m;
        m.set("Bleep", "bloop");
        child = archive->createChild(ABCA::ObjectHeader("1ChildAMeta",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("child", m));

        child = archive->createChild(ABCA::ObjectHeader("1ChildBMeta",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("child", m));

        // 2 objects with one child object additional prop
        child = archive->createChild(ABCA::ObjectHeader("1ChildAProp",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("child", ABCA::MetaData())
            )->getProperties()->createCompoundProperty("child",
            ABCA::MetaData());

        child = archive->createChild(ABCA::ObjectHeader("1ChildBProp",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("child", ABCA::MetaData())
            )->getProperties()->createCompoundProperty("child",
            ABCA::MetaData());

        // 2 objects with one child object with additional grand child
        child = archive->createChild(ABCA::ObjectHeader("1ChildAChild",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("child", ABCA::MetaData())
            )->createChild(ABCA::ObjectHeader("grandchild", ABCA::MetaData()));

        child = archive->createChild(ABCA::ObjectHeader("1ChildBChild",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("child", ABCA::MetaData())
            )->createChild(ABCA::ObjectHeader("grandchild", ABCA::MetaData()));

        // 2 objects with two children objects
        child = archive->createChild(ABCA::ObjectHeader("2ChildA",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("childA", ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("childB", ABCA::MetaData()));

        child = archive->createChild(ABCA::ObjectHeader("2ChildB",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("childA", ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("childB", ABCA::MetaData()));

        // 2 objects with two children objects with swapped children
        child = archive->createChild(ABCA::ObjectHeader("2ChildASwap",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("childB", ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("childA", ABCA::MetaData()));

        child = archive->createChild(ABCA::ObjectHeader("2ChildBSwap",
            ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("childB", ABCA::MetaData()));
        child->createChild(ABCA::ObjectHeader("childA", ABCA::MetaData()));
    }

    {
        AO::ReadArchive r(1, iUseMMap);
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();

        TESTING_ASSERT(archive->getNumChildren() == 16);

        // every 2 hashes should be the same
        for (size_t i = 0; i < archive->getNumChildren(); i += 2)
        {
            Alembic::Util::Digest dA, dB;
            TESTING_ASSERT(archive->getChild(i)->getChildrenHash(dA) &&
                archive->getChild(i+1)->getChildrenHash(dB));
            TESTING_ASSERT(dA == dB);
        }

        // make sure that every 2 child objects have different properties hashes
        for (size_t i = 0; i < archive->getNumChildren() / 2; ++i)
        {
            Alembic::Util::Digest dA;
            archive->getChild(i*2)->getChildrenHash(dA);
            for (size_t j = i + 1; j < archive->getNumChildren() / 2; ++j)
            {
                Alembic::Util::Digest dB;
                archive->getChild(j*2)->getChildrenHash(dB);
                TESTING_ASSERT(dA != dB);
            }
        }
    }
}

void testStringHashes(bool iUseMMap)
{
    std::string archiveName = "stringsHashTest.abc";
    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();
        ABCA::CompoundPropertyWriterPtr props = archive->getProperties();

        ABCA::DataType wdtype(Alembic::Util::kWstringPOD, 1);
        ABCA::ArrayPropertyWriterPtr wstrWrtPtr =
            props->createArrayProperty("wstr", ABCA::MetaData(), wdtype, 0);

        std::vector < Alembic::Util::wstring > wvals(4);
        wvals[0] = L"Mash potatoes ";
        wvals[1] = L"with some delicious gravy.";
        wvals[2] = L"";
        wvals[3] = L"\uf8e4 \uf8e2 \uf8d3";

        for (size_t i = 0; i < wvals.size(); ++i)
        {
            Alembic::Util::Dimensions wdims(i);
            wstrWrtPtr->setSample(
                ABCA::ArraySample(&(wvals.front()), wdtype, wdims));
        }

        ABCA::DataType sdtype(Alembic::Util::kStringPOD, 1);
        ABCA::ArrayPropertyWriterPtr strWrtPtr =
            props->createArrayProperty("str", ABCA::MetaData(), sdtype, 0);

        std::vector < Alembic::Util::string > svals(4);
        svals[0] = "Sunday, Monday";
        svals[1] = "Tuesday, Wednesday, Thursday";
        svals[2] = "";
        svals[3] = "Some other days";

        for (size_t i = 0; i < svals.size(); ++i)
        {
            Alembic::Util::Dimensions sdims(i);
            strWrtPtr->setSample(
                ABCA::ArraySample(&(svals.front()), sdtype, sdims));
        }
    }

    {
        AO::ReadArchive r(1, iUseMMap);
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();
        ABCA::CompoundPropertyReaderPtr parent = archive->getProperties();
        ABCA::ArrayPropertyReaderPtr wap = parent->getArrayProperty("wstr");
        std::vector< ABCA::ArraySampleKey > keys(wap->getNumSamples());
        for (size_t i = 0; i < wap->getNumSamples(); ++i)
        {
            wap->getKey( (ABCA::index_t) i, keys[i] );
        }

        for (size_t i = 0; i < keys.size(); ++i)
        {
            for (size_t j = i+1; j < keys.size(); ++j)
            {
                TESTING_ASSERT( keys[i] != keys[j] );
            }
        }

        ABCA::ArrayPropertyReaderPtr sap = parent->getArrayProperty("str");
        keys.resize(sap->getNumSamples());
        for (size_t i = 0; i < sap->getNumSamples(); ++i)
        {
            sap->getKey( (ABCA::index_t) i, keys[i] );
        }

        for (size_t i = 0; i < keys.size(); ++i)
        {
            for (size_t j = i+1; j < keys.size(); ++j)
            {
                TESTING_ASSERT( keys[i] != keys[j] );
            }
        }
    }
}

void runTests(bool iUseMMap)
{
    testArrayPropHashes(iUseMMap);
    testScalarPropHashes(iUseMMap);
    testCompoundPropHashes(iUseMMap);
    testObjectHashes(iUseMMap);
    testStringHashes(iUseMMap);
}

int main ( int argc, char *argv[] )
{
    runTests(true);     // Use mmap
    runTests(false);    // Use streams
    return 0;
}
