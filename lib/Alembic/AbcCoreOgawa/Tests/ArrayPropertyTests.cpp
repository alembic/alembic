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

using namespace Alembic::Util;

//-*****************************************************************************
void testDuplicateArray()
{
    std::string archiveName = "repeatArray.abc";

    size_t numVals = 35;

    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();

        ABCA::CompoundPropertyWriterPtr parent = archive->getProperties();

        ABCA::ArrayPropertyWriterPtr awp =
            parent->createArrayProperty("a",  ABCA::MetaData(),
                                        ABCA::DataType(Alembic::Util::kInt16POD, 1), 0);

        ABCA::ArrayPropertyWriterPtr bwp =
            parent->createArrayProperty("b", ABCA::MetaData(),
                                        ABCA::DataType(Alembic::Util::kInt16POD, 1), 0);

        ABCA::ArrayPropertyWriterPtr cwp =
            parent->createArrayProperty("c", ABCA::MetaData(),
                                        ABCA::DataType(Alembic::Util::kInt8POD, 1), 0);

        Alembic::Util::Dimensions dims(numVals);
        ABCA::DataType i16d(Alembic::Util::kInt16POD, 1);
        std::vector <Alembic::Util::int16_t> vali(numVals, 0);
        vali[0] = 7;
        vali[1] = 7;
        vali[2] = 7;

        awp->setSample(ABCA::ArraySample(&(vali.front()), i16d, dims));

        vali[0] = 8;
        vali[1] = 16;
        vali[2] = 32;
        awp->setSample(ABCA::ArraySample(&(vali.front()), i16d, dims));

        vali[0] = 7;
        vali[1] = 7;
        vali[2] = 7;
        awp->setSample(ABCA::ArraySample(&(vali.front()), i16d, dims));

        // lets write b and make sure it shares
        vali[0] = 8;
        vali[1] = 16;
        vali[2] = 32;
        bwp->setSample(ABCA::ArraySample(&(vali.front()), i16d, dims));

        vali[0] = 7;
        vali[1] = 7;
        vali[2] = 7;
        bwp->setSample(ABCA::ArraySample(&(vali.front()), i16d, dims));

        vali[0] = 8;
        vali[1] = 16;
        vali[2] = 32;
        bwp->setSample(ABCA::ArraySample(&(vali.front()), i16d, dims));

        // c shouldn't share because it's pod type and extent is different
        std::vector <Alembic::Util::int8_t> vals(numVals*2);
        ABCA::DataType i8d(Alembic::Util::kInt8POD, 1);

        vals[0] = 8;
        vals[1] = 0;
        vals[2] = 16;
        vals[3] = 0;
        vals[4] = 32;
        vals[5] = 0;
        dims = Alembic::Util::Dimensions(numVals*2);
        cwp->setSample(ABCA::ArraySample(&(vals.front()), i8d, dims));


        vals[0] = 7;
        vals[1] = 0;
        vals[2] = 7;
        vals[3] = 0;
        vals[4] = 7;
        vals[5] = 0;
        cwp->setSample(ABCA::ArraySample(&(vals.front()), i8d, dims));
        cwp->setSample(ABCA::ArraySample(&(vals.front()), i8d, dims));
        for (std::size_t i = 0; i < 128; ++i)
        {
            cwp->setSample(ABCA::ArraySample(&(vals.front()), i8d, dims));
        }
    }

    {
        AO::ReadArchive r;
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();
        ABCA::CompoundPropertyReaderPtr parent = archive->getProperties();

        TESTING_ASSERT(parent->getNumProperties() == 3);
        for (size_t i = 0; i < parent->getNumProperties(); ++i)
        {
            ABCA::BasePropertyReaderPtr bp = parent->getProperty( i );
            TESTING_ASSERT(bp->isArray());
            ABCA::ArrayPropertyReaderPtr ap = bp->asArrayPtr();
            if (ap->getName() == "c")
            {
                TESTING_ASSERT(ap->getNumSamples() == 131);
            }
            else
            {
                TESTING_ASSERT(ap->getNumSamples() == 3);
            }


            if (ap->getName() == "a")
            {
                Dimensions dims0;
                Dimensions dims1;
                Dimensions dims2;
                ap->getDimensions(0, dims0);
                ap->getDimensions(1, dims1);
                ap->getDimensions(2, dims2);

                TESTING_ASSERT(dims0.numPoints() == numVals);
                TESTING_ASSERT(dims1.numPoints() == numVals);
                TESTING_ASSERT(dims2.numPoints() == numVals);

                ABCA::ArraySamplePtr samp0;
                ABCA::ArraySamplePtr samp1;
                ABCA::ArraySamplePtr samp2;
                ap->getSample(0, samp0);
                ap->getSample(1, samp1);
                ap->getSample(2, samp2);

                TESTING_ASSERT(samp0->getDimensions().numPoints() == numVals);
                TESTING_ASSERT(samp1->getDimensions().numPoints() == numVals);
                TESTING_ASSERT(samp2->getDimensions().numPoints() == numVals);

                Alembic::Util::int16_t * data = (Alembic::Util::int16_t *)(samp0->getData());
                TESTING_ASSERT(data[0] == 7);
                TESTING_ASSERT(data[1] == 7);
                TESTING_ASSERT(data[2] == 7);
                for (size_t j = 3; j < numVals; ++j)
                {
                    TESTING_ASSERT(data[j] == 0);
                }

                data = (Alembic::Util::int16_t *)(samp1->getData());
                TESTING_ASSERT(data[0] == 8);
                TESTING_ASSERT(data[1] == 16);
                TESTING_ASSERT(data[2] == 32);
                for (size_t j = 3; j < numVals; ++j)
                {
                    TESTING_ASSERT(data[j] == 0);
                }

                data = (Alembic::Util::int16_t *)(samp2->getData());
                TESTING_ASSERT(data[0] == 7);
                TESTING_ASSERT(data[1] == 7);
                TESTING_ASSERT(data[2] == 7);
                for (size_t j = 3; j < numVals; ++j)
                {
                    TESTING_ASSERT(data[j] == 0);
                }
            }
            else if (ap->getName() == "b")
            {
                ABCA::ArraySamplePtr samp0;
                ABCA::ArraySamplePtr samp1;
                ABCA::ArraySamplePtr samp2;
                ap->getSample(0, samp0);
                ap->getSample(1, samp1);
                ap->getSample(2, samp2);

                TESTING_ASSERT(samp0->getDimensions().numPoints() == numVals);
                TESTING_ASSERT(samp1->getDimensions().numPoints() == numVals);
                TESTING_ASSERT(samp2->getDimensions().numPoints() == numVals);

                Dimensions dims0;
                Dimensions dims1;
                Dimensions dims2;
                ap->getDimensions(0, dims0);
                ap->getDimensions(1, dims1);
                ap->getDimensions(2, dims2);

                TESTING_ASSERT(dims0.numPoints() == numVals);
                TESTING_ASSERT(dims1.numPoints() == numVals);
                TESTING_ASSERT(dims2.numPoints() == numVals);

                Alembic::Util::int16_t * data = (Alembic::Util::int16_t *)(samp0->getData());
                TESTING_ASSERT(data[0] == 8);
                TESTING_ASSERT(data[1] == 16);
                TESTING_ASSERT(data[2] == 32);
                for (size_t j = 3; j < numVals; ++j)
                {
                    TESTING_ASSERT(data[j] == 0);
                }

                data = (Alembic::Util::int16_t *)(samp1->getData());
                TESTING_ASSERT(data[0] == 7);
                TESTING_ASSERT(data[1] == 7);
                TESTING_ASSERT(data[2] == 7);
                for (size_t j = 3; j < numVals; ++j)
                {
                    TESTING_ASSERT(data[j] == 0);
                }

                data = (Alembic::Util::int16_t *)(samp2->getData());
                TESTING_ASSERT(data[0] == 8);
                TESTING_ASSERT(data[1] == 16);
                TESTING_ASSERT(data[2] == 32);
                for (size_t j = 3; j < numVals; ++j)
                {
                    TESTING_ASSERT(data[j] == 0);
                }
            }
            else if (ap->getName() == "c")
            {
                ABCA::ArraySamplePtr samp0;
                ABCA::ArraySamplePtr samp1;
                ABCA::ArraySamplePtr samp2;
                ap->getSample(0, samp0);
                ap->getSample(1, samp1);
                ap->getSample(2, samp2);

                TESTING_ASSERT(samp0->getDimensions().numPoints() == numVals*2);
                TESTING_ASSERT(samp1->getDimensions().numPoints() == numVals*2);
                TESTING_ASSERT(samp2->getDimensions().numPoints() == numVals*2);

                Dimensions dims0;
                Dimensions dims1;
                Dimensions dims2;
                ap->getDimensions(0, dims0);
                ap->getDimensions(1, dims1);
                ap->getDimensions(2, dims2);

                TESTING_ASSERT(dims0.numPoints() == numVals*2);
                TESTING_ASSERT(dims1.numPoints() == numVals*2);
                TESTING_ASSERT(dims2.numPoints() == numVals*2);

                Alembic::Util::int8_t * data = (Alembic::Util::int8_t *)(samp0->getData());
                TESTING_ASSERT(data[0] == 8);
                TESTING_ASSERT(data[1] == 0);
                TESTING_ASSERT(data[2] == 16);
                TESTING_ASSERT(data[3] == 0);
                TESTING_ASSERT(data[4] == 32);
                TESTING_ASSERT(data[5] == 0);
                for (size_t j = 6; j < numVals*2; ++j)
                {
                    TESTING_ASSERT(data[j] == 0);
                }

                data = (Alembic::Util::int8_t *)(samp1->getData());
                TESTING_ASSERT(data[0] == 7);
                TESTING_ASSERT(data[1] == 0);
                TESTING_ASSERT(data[2] == 7);
                TESTING_ASSERT(data[3] == 0);
                TESTING_ASSERT(data[4] == 7);
                TESTING_ASSERT(data[5] == 0);
                for (size_t j = 6; j < numVals*2; ++j)
                {
                    TESTING_ASSERT(data[j] == 0);
                }

                data = (Alembic::Util::int8_t *)(samp2->getData());
                TESTING_ASSERT(data[0] == 7);
                TESTING_ASSERT(data[1] == 0);
                TESTING_ASSERT(data[2] == 7);
                TESTING_ASSERT(data[3] == 0);
                TESTING_ASSERT(data[4] == 7);
                TESTING_ASSERT(data[5] == 0);
                for (size_t j = 6; j < numVals*2; ++j)
                {
                    TESTING_ASSERT(data[j] == 0);
                }
            }
        }
    }
}

//-*****************************************************************************
void testReadWriteArrays()
{

    std::string archiveName = "arrayProperties.abc";

    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();

        ABCA::CompoundPropertyWriterPtr props = archive->getProperties();

        {
            ABCA::DataType dtype(Alembic::Util::kBooleanPOD, 1);
            ABCA::ArrayPropertyWriterPtr boolWrtPtr =
                props->createArrayProperty("bool", ABCA::MetaData(), dtype, 0 );


            std::vector < Alembic::Util::bool_t > vals(3);
            vals[0] = false;
            vals[1] = true;
            vals[2] = false;
            Alembic::Util::Dimensions dims(vals.size());
            boolWrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }


        {
            ABCA::DataType dtype(Alembic::Util::kUint8POD, 1);
            ABCA::ArrayPropertyWriterPtr uint8WrtPtr =
                props->createArrayProperty("uint8", ABCA::MetaData(), dtype, 0);

            std::vector< Alembic::Util::uint8_t > vals(4);
            vals[0] = 200;
            vals[1] = 45;
            vals[2] = 37;
            vals[3] = 192;

            TESTING_ASSERT(uint8WrtPtr->getNumSamples() == 0);
            Alembic::Util::Dimensions dims(vals.size());
            uint8WrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
            TESTING_ASSERT(uint8WrtPtr->getNumSamples() == 1);
        }

        {
            ABCA::DataType dtype(Alembic::Util::kUint8POD, 1);
            ABCA::ArrayPropertyWriterPtr uint8WrtPtr =
                props->createArrayProperty("uint8_newDims",
                                           ABCA::MetaData(), dtype, 0);

            std::vector< Alembic::Util::uint8_t > vals(4);
            vals[0] = 200;
            vals[1] = 45;
            vals[2] = 37;
            vals[3] = 192;

            TESTING_ASSERT(uint8WrtPtr->getNumSamples() == 0);
            Alembic::Util::Dimensions dims;
            dims.setRank(2);
            dims[0] = 2;
            dims[1] = 2;
            uint8WrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
            TESTING_ASSERT(uint8WrtPtr->getNumSamples() == 1);
        }

        {
            ABCA::DataType dtype(Alembic::Util::kInt8POD, 1);
            ABCA::ArrayPropertyWriterPtr charWrtPtr =
                props->createArrayProperty("int8",  ABCA::MetaData(), dtype, 0 );

            std::vector < Alembic::Util::int8_t > vals(2);
            vals[0] = -20;
            vals[1] = 45;

            Alembic::Util::Dimensions dims(vals.size());
            charWrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }

        {
            ABCA::DataType dtype(Alembic::Util::kUint16POD, 1);
            ABCA::ArrayPropertyWriterPtr uint16WrtPtr =
                props->createArrayProperty("uint16", ABCA::MetaData(),
                                           ABCA::DataType(Alembic::Util::kUint16POD, 1), 0 );

            std::vector < Alembic::Util::uint16_t > vals(3);
            vals[0] = 60000;
            vals[1] = 2;
            vals[2] = 3987;

            Alembic::Util::Dimensions dims(vals.size());
            uint16WrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));

        }

        {
            ABCA::DataType dtype(Alembic::Util::kInt16POD, 1);
            ABCA::ArrayPropertyWriterPtr int16WrtPtr =
                props->createArrayProperty("int16", ABCA::MetaData(),
                                           dtype, 0);
            std::vector < Alembic::Util::int16_t > vals(2);
            vals[0] = -20000;
            vals[1] = 77;
            Alembic::Util::Dimensions dims(vals.size());
            int16WrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }

        {
            ABCA::DataType dtype(Alembic::Util::kUint32POD, 1);
            ABCA::ArrayPropertyWriterPtr uint32WrtPtr =
                props->createArrayProperty("uint32",  ABCA::MetaData(), dtype,
                                           0);

            std::vector < Alembic::Util::uint32_t > vals(1);
            vals[0] = 1000000;

            Alembic::Util::Dimensions dims(vals.size());
            uint32WrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }

        {
            ABCA::DataType dtype(Alembic::Util::kInt32POD, 1);
            ABCA::ArrayPropertyWriterPtr int32WrtPtr =
                props->createArrayProperty("int32", ABCA::MetaData(), dtype, 0);
            std::vector < Alembic::Util::int32_t > vals(4);
            vals[0] = -1000000;
            vals[1] = 42;
            vals[2] = -20000;
            vals[3] = 123456;

            Alembic::Util::Dimensions dims(vals.size());
            int32WrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }

        {
            ABCA::DataType dtype(Alembic::Util::kUint64POD, 1);
            ABCA::ArrayPropertyWriterPtr ui64WrtPtr =
                props->createArrayProperty("uint64", ABCA::MetaData(), dtype, 0);
            std::vector < Alembic::Util::uint64_t > vals(3);
            vals[0] = 5000000000LL;
            vals[1] = 1234567891011LL;
            vals[2] = 12;

            Alembic::Util::Dimensions dims(vals.size());
            ui64WrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }

        {
            ABCA::DataType dtype(Alembic::Util::kInt64POD, 1);
            ABCA::ArrayPropertyWriterPtr i64WrtPtr =
                props->createArrayProperty("int64", ABCA::MetaData(), dtype, 0);

            std::vector < Alembic::Util::int64_t > vals(2);
            vals[0] = -5000000000LL;
            vals[1] = 9876543210LL;
            Alembic::Util::Dimensions dims(vals.size());
            i64WrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }

        {
            ABCA::DataType dtype(Alembic::Util::kFloat16POD, 1);
            ABCA::ArrayPropertyWriterPtr halfWrtPtr =
                props->createArrayProperty("float16",
                                           ABCA::MetaData(), dtype, 0);

            std::vector < Alembic::Util::float16_t > vals(2);
            vals[0] = 16.0;
            vals[1] = -3.0;

            Alembic::Util::Dimensions dims(vals.size());
            halfWrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }

        {
            ABCA::DataType dtype(Alembic::Util::kFloat32POD, 2);
            ABCA::ArrayPropertyWriterPtr floatWrtPtr =
                props->createArrayProperty("float32", ABCA::MetaData(), dtype,
                                           0);
            std::vector < Alembic::Util::float32_t > vals(4);
            vals[0] = 128.0;
            vals[1] = -13.25;
            vals[2] = 35.5;
            vals[3] = 128.125;
            Alembic::Util::Dimensions dims(vals.size()/2);
            floatWrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }

        {
            ABCA::DataType dtype(Alembic::Util::kFloat32POD, 1);
            ABCA::ArrayPropertyWriterPtr floatWrtPtr =
                props->createArrayProperty("float32_ext1",
                                           ABCA::MetaData(), dtype, 0);
            std::vector < Alembic::Util::float32_t > vals(4);
            vals[0] = 128.0;
            vals[1] = -13.25;
            vals[2] = 35.5;
            vals[3] = 128.125;
            Alembic::Util::Dimensions dims(vals.size());
            floatWrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }

        {
            ABCA::DataType dtype(Alembic::Util::kFloat64POD, 1);
            ABCA::ArrayPropertyWriterPtr doubleWrtPtr =
                props->createArrayProperty("float64",  ABCA::MetaData(),
                                           dtype, 0);

            std::vector < Alembic::Util::float64_t > vals(30);
            for (size_t i = 0; i < vals.size(); ++i)
                vals[i] = i * 100.0;
            TESTING_ASSERT(doubleWrtPtr->getNumSamples() == 0);
            Alembic::Util::Dimensions dims(vals.size());
            doubleWrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
            TESTING_ASSERT(doubleWrtPtr->getNumSamples() == 1);
        }

        {
            ABCA::DataType dtype(Alembic::Util::kFloat64POD, 3);
            ABCA::ArrayPropertyWriterPtr doubleWrtPtr =
                props->createArrayProperty("float64_ext3",
                                           ABCA::MetaData(), dtype, 0);

            std::vector < Alembic::Util::float64_t > vals(30);
            for (size_t i = 0; i < vals.size(); ++i)
                vals[i] = i * 100.0;
            TESTING_ASSERT(doubleWrtPtr->getNumSamples() == 0);
            Alembic::Util::Dimensions dims(vals.size() / 3);
            doubleWrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
            TESTING_ASSERT(doubleWrtPtr->getNumSamples() == 1);
        }

        {
            ABCA::DataType dtype(Alembic::Util::kStringPOD, 1);
            ABCA::ArrayPropertyWriterPtr strWrtPtr =
                props->createArrayProperty("str",
                                           ABCA::MetaData(), dtype, 0);

            std::vector < Alembic::Util::string > vals(4);
            vals[0] = "Now it's time";
            vals[1] = "";
            vals[2] = "for";
            vals[3] = "cake!";
            Alembic::Util::Dimensions dims(vals.size());
            strWrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }

        {
            ABCA::DataType dtype(Alembic::Util::kWstringPOD, 1);
            ABCA::ArrayPropertyWriterPtr wstrWrtPtr =
                props->createArrayProperty("wstr", ABCA::MetaData(), dtype, 0);

            std::vector < Alembic::Util::wstring > vals(4);
            vals[0] = L"We do what we must ";
            vals[1] = L"because we can.";
            vals[2] = L"";
            vals[3] = L"\uf8e4 \uf8e2 \uf8d3";
            Alembic::Util::Dimensions dims(vals.size());
            wstrWrtPtr->setSample(
                ABCA::ArraySample(&(vals.front()), dtype, dims));
        }

    }

    // now we read what we've written
    {
        AO::ReadArchive r;
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();
        ABCA::CompoundPropertyReaderPtr parent = archive->getProperties();

        TESTING_ASSERT(parent->getNumProperties() == 17);

        ABCA::ArraySampleKey key;
        for ( size_t i = 0; i < parent->getNumProperties(); ++i )
        {
            ABCA::BasePropertyReaderPtr bp = parent->getProperty( i );

            // they are all supposed to be arrays
            TESTING_ASSERT( bp->isArray() );

            ABCA::ArrayPropertyReaderPtr ap = bp->asArrayPtr();
            TESTING_ASSERT( ap->getNumSamples() == 1 );
            TESTING_ASSERT( ap->isConstant() );
            TESTING_ASSERT( ap->getParent() == parent);

            if (ap->getName() != "float64_ext3" && ap->getName() != "float32")
                TESTING_ASSERT( ap->getDataType().getExtent() == 1);

            switch (ap->getDataType().getPod())
            {
                case Alembic::Util::kBooleanPOD:
                {
                    TESTING_ASSERT(ap->getName() == "bool");
                    TESTING_ASSERT(!ap->isScalarLike());
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(val->getDimensions().numPoints() == 3);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);

                    Dimensions dims0;
                    ap->getDimensions(0, dims0);
                    TESTING_ASSERT(dims0.numPoints() == 3);
                    TESTING_ASSERT(dims0.rank() == 1);

                    Alembic::Util::bool_t * data =
                        (Alembic::Util::bool_t *)(val->getData());
                    TESTING_ASSERT(data[0] == false);
                    TESTING_ASSERT(data[1] == true);
                    TESTING_ASSERT(data[2] == false);

                    // read as something else
                    Alembic::Util::int32_t data2[3];
                    ap->getAs(0, data2, Alembic::Util::kInt32POD);
                    TESTING_ASSERT(data2[0] == 0);
                    TESTING_ASSERT(data2[1] == 1);
                    TESTING_ASSERT(data2[2] == 0);
                    // read it as it is
                    Alembic::Util::bool_t data3[3];
                    ap->getAs(0, data3, Alembic::Util::kBooleanPOD);
                    TESTING_ASSERT(data3[0] == false);
                    TESTING_ASSERT(data3[1] == true);
                    TESTING_ASSERT(data3[2] == false);

                    TESTING_ASSERT(ap->getKey(0, key));
                    TESTING_ASSERT(key.numBytes == 3);
                    TESTING_ASSERT(key.origPOD == Alembic::Util::kBooleanPOD);
                    TESTING_ASSERT(key.readPOD == Alembic::Util::kBooleanPOD);
                    TESTING_ASSERT(key.digest.str() ==
                        "bbde8ffe4b7e061c7e03081c2bf184c4");
                }
                break;

                case Alembic::Util::kUint8POD:
                {
                    TESTING_ASSERT(ap->getName() == "uint8" ||
                        ap->getName() == "uint8_newDims");
                    TESTING_ASSERT(!ap->isScalarLike());
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);

                    Alembic::Util::uint8_t * data =
                        (Alembic::Util::uint8_t *)(val->getData());
                    TESTING_ASSERT(data[0] == 200);
                    TESTING_ASSERT(data[1] == 45);
                    TESTING_ASSERT(data[2] == 37);
                    TESTING_ASSERT(data[3] == 192);
                    TESTING_ASSERT(val->getDimensions().numPoints() == 4);

                    // read as something else
                    Alembic::Util::int32_t data2[4];
                    ap->getAs(0, data2, kInt32POD);
                    TESTING_ASSERT(data2[0] == 200);
                    TESTING_ASSERT(data2[1] == 45);
                    TESTING_ASSERT(data2[2] == 37);
                    TESTING_ASSERT(data2[3] == 192);

                    // read it as it is
                    Alembic::Util::uint8_t data3[4];
                    ap->getAs(0, data3, kUint8POD);
                    TESTING_ASSERT(data3[0] == 200);
                    TESTING_ASSERT(data3[1] == 45);
                    TESTING_ASSERT(data3[2] == 37);
                    TESTING_ASSERT(data3[3] == 192);

                    TESTING_ASSERT(ap->getKey(0, key));
                    TESTING_ASSERT(key.numBytes == 4);
                    TESTING_ASSERT(key.origPOD == Alembic::Util::kUint8POD);
                    TESTING_ASSERT(key.readPOD == Alembic::Util::kUint8POD);
                    TESTING_ASSERT(key.digest.str() ==
                        "2972c92c7f534e37ee116ae01c707741");

                    if (ap->getName() == "uint8")
                    {
                        Dimensions dims0;
                        ap->getDimensions(0, dims0);
                        TESTING_ASSERT(dims0.rank() == 1);
                        TESTING_ASSERT(dims0[0] == 4);
                    }
                    else if (ap->getName() == "uint8_newDims")
                    {
                        Dimensions dims0;
                        ap->getDimensions(0, dims0);
                        TESTING_ASSERT(dims0.rank() == 2);
                        TESTING_ASSERT(dims0[0] == 2);
                        TESTING_ASSERT(dims0[1] == 2);
                    }
                }
                break;

                case Alembic::Util::kInt8POD:
                {
                    TESTING_ASSERT(ap->getName() == "int8");
                    Dimensions dims0;
                    ap->getDimensions(0, dims0);
                    TESTING_ASSERT(dims0.rank() == 1);
                    TESTING_ASSERT(dims0.numPoints() == 2);
                    TESTING_ASSERT(!ap->isScalarLike());
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(val->getDimensions().numPoints() == 2);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    Alembic::Util::int8_t * data =
                        (Alembic::Util::int8_t *)(val->getData());
                    TESTING_ASSERT(data[0] == -20);
                    TESTING_ASSERT(data[1] == 45);

                    // read as something else
                    Alembic::Util::int32_t data2[2];
                    ap->getAs(0, data2, kInt32POD);
                    TESTING_ASSERT(data2[0] == -20);
                    TESTING_ASSERT(data2[1] == 45);

                    // read it as it is
                    Alembic::Util::int8_t data3[2];
                    ap->getAs(0, data3, kInt8POD);
                    TESTING_ASSERT(data3[0] == -20);
                    TESTING_ASSERT(data3[1] == 45);
                }
                break;

                case Alembic::Util::kUint16POD:
                {
                    TESTING_ASSERT(ap->getName() == "uint16");
                    TESTING_ASSERT(!ap->isScalarLike());
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(val->getDimensions().numPoints() == 3);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    Dimensions dims0;
                    ap->getDimensions(0, dims0);
                    TESTING_ASSERT(dims0.rank() == 1);
                    TESTING_ASSERT(dims0.numPoints() == 3);
                    Alembic::Util::uint16_t * data =
                        (Alembic::Util::uint16_t *)(val->getData());
                    TESTING_ASSERT(data[0] == 60000);
                    TESTING_ASSERT(data[1] == 2);
                    TESTING_ASSERT(data[2] == 3987);

                    // read as something else
                    Alembic::Util::int32_t data2[3];
                    ap->getAs(0, data2, kInt32POD);
                    TESTING_ASSERT(data2[0] == 60000);
                    TESTING_ASSERT(data2[1] == 2);
                    TESTING_ASSERT(data2[2] == 3987);

                    // read it as it is
                    Alembic::Util::uint16_t data3[3];
                    ap->getAs(0, data3, kUint16POD);
                    TESTING_ASSERT(data3[0] == 60000);
                    TESTING_ASSERT(data3[1] == 2);
                    TESTING_ASSERT(data3[2] == 3987);
                }
                break;

                case Alembic::Util::kInt16POD:
                {
                    TESTING_ASSERT(ap->getName() == "int16");
                    TESTING_ASSERT(!ap->isScalarLike());
                    Dimensions dims0;
                    ap->getDimensions(0, dims0);
                    TESTING_ASSERT(dims0.rank() == 1);
                    TESTING_ASSERT(dims0.numPoints() == 2);
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(val->getDimensions().numPoints() == 2);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    Alembic::Util::int16_t * data =
                        (Alembic::Util::int16_t *)(val->getData());
                    TESTING_ASSERT(data[0] == -20000);
                    TESTING_ASSERT(data[1] == 77);

                    // read as something else
                    Alembic::Util::int32_t data2[2];
                    ap->getAs(0, data2, kInt32POD);
                    TESTING_ASSERT(data2[0] == -20000);
                    TESTING_ASSERT(data2[1] == 77);

                    // read it as it is
                    Alembic::Util::int16_t data3[2];
                    ap->getAs(0, data3, kInt16POD);
                    TESTING_ASSERT(data3[0] == -20000);
                    TESTING_ASSERT(data3[1] == 77);
                }
                break;

                case Alembic::Util::kUint32POD:
                {
                    TESTING_ASSERT(ap->getName() == "uint32");
                    TESTING_ASSERT(ap->isScalarLike());
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(val->getDimensions().numPoints() == 1);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    Dimensions dims0;
                    ap->getDimensions(0, dims0);
                    TESTING_ASSERT(dims0.rank() == 1);
                    TESTING_ASSERT(dims0.numPoints() == 1);
                    Alembic::Util::uint32_t * data =
                        (Alembic::Util::uint32_t *)(val->getData());
                    TESTING_ASSERT(data[0] == 1000000);

                    // read as something else
                    Alembic::Util::int16_t data2;
                    ap->getAs(0, &data2, kInt16POD);
                    TESTING_ASSERT(data2 == 32767); // maxed out*/

                    // read it as it is
                    Alembic::Util::int32_t data3;
                    ap->getAs(0, &data3, kUint32POD);
                    TESTING_ASSERT(data3 == 1000000);
                }
                break;

                case Alembic::Util::kInt32POD:
                {
                    TESTING_ASSERT(ap->getName() == "int32");
                    TESTING_ASSERT(!ap->isScalarLike());
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(val->getDimensions().numPoints() == 4);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    Dimensions dims0;
                    ap->getDimensions(0, dims0);
                    TESTING_ASSERT(dims0.rank() == 1);
                    TESTING_ASSERT(dims0.numPoints() == 4);
                    Alembic::Util::int32_t * data =
                        (Alembic::Util::int32_t *)(val->getData());
                    TESTING_ASSERT(data[0] == -1000000);
                    TESTING_ASSERT(data[1] == 42);
                    TESTING_ASSERT(data[2] == -20000);
                    TESTING_ASSERT(data[3] == 123456);

                    // read as something else
                    Alembic::Util::int16_t data2[4];
                    ap->getAs(0, data2, kInt16POD);
                    TESTING_ASSERT(data2[0] == -32768); // maxed out negative
                    TESTING_ASSERT(data2[1] == 42);
                    TESTING_ASSERT(data2[2] == -20000);
                    TESTING_ASSERT(data2[3] == 32767);  // maxed out positive

                    // read it as it is
                    Alembic::Util::int32_t data3[4];
                    ap->getAs(0, data3, kInt32POD);
                    TESTING_ASSERT(data3[0] == -1000000);
                    TESTING_ASSERT(data3[1] == 42);
                    TESTING_ASSERT(data3[2] == -20000);
                    TESTING_ASSERT(data3[3] == 123456);

                    // can't read as strings, and it shouldn't touch the buffer
                    TESTING_ASSERT_THROW(ap->getAs(0, data3, kStringPOD),
                        Alembic::Util::Exception);

                    TESTING_ASSERT_THROW(ap->getAs(0, data3, kWstringPOD),
                        Alembic::Util::Exception);
                    TESTING_ASSERT(data3[0] == -1000000);
                    TESTING_ASSERT(data3[1] == 42);
                    TESTING_ASSERT(data3[2] == -20000);
                    TESTING_ASSERT(data3[3] == 123456);
                }
                break;

                case Alembic::Util::kUint64POD:
                {
                    TESTING_ASSERT(ap->getName() == "uint64");
                    TESTING_ASSERT(!ap->isScalarLike());
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(val->getDimensions().numPoints() == 3);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    Dimensions dims0;
                    ap->getDimensions(0, dims0);
                    TESTING_ASSERT(dims0.rank() == 1);
                    TESTING_ASSERT(dims0.numPoints() == 3);
                    Alembic::Util::uint64_t * data =
                        (Alembic::Util::uint64_t *)(val->getData());
                    TESTING_ASSERT(data[0] == 5000000000LL);
                    TESTING_ASSERT(data[1] == 1234567891011LL);
                    TESTING_ASSERT(data[2] == 12);

                    // read as something else
                    Alembic::Util::int16_t data2[3];
                    ap->getAs(0, data2, kInt16POD);
                    TESTING_ASSERT(data2[0] == 32767); // maxed out positive
                    TESTING_ASSERT(data2[1] == 32767); // maxed out positive
                    TESTING_ASSERT(data2[2] == 12);

                    // read it as it is
                    Alembic::Util::uint64_t data3[3];
                    ap->getAs(0, data3, kUint64POD);
                    TESTING_ASSERT(data3[0] == 5000000000LL);
                    TESTING_ASSERT(data3[1] == 1234567891011LL);
                    TESTING_ASSERT(data3[2] == 12);
                }
                break;

                case Alembic::Util::kInt64POD:
                {
                    TESTING_ASSERT(ap->getName() == "int64");
                    TESTING_ASSERT(!ap->isScalarLike());
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(val->getDimensions().numPoints() == 2);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    Dimensions dims0;
                    ap->getDimensions(0, dims0);
                    TESTING_ASSERT(dims0.rank() == 1);
                    TESTING_ASSERT(dims0.numPoints() == 2);
                    Alembic::Util::int64_t * data =
                        (Alembic::Util::int64_t *)(val->getData());
                    TESTING_ASSERT(data[0] == -5000000000LL);
                    TESTING_ASSERT(data[1] == 9876543210LL);

                    // read as something else
                    Alembic::Util::int16_t data2[2];
                    ap->getAs(0, data2, kInt16POD);
                    TESTING_ASSERT(data2[0] == -32768); // maxed out negative
                    TESTING_ASSERT(data2[1] == 32767); // maxed out positive

                    // read it as it is
                    Alembic::Util::int64_t data3[2];
                    ap->getAs(0, data3, kInt64POD);
                    TESTING_ASSERT(data3[0] == -5000000000LL);
                    TESTING_ASSERT(data3[1] == 9876543210LL);
                }
                break;

                case Alembic::Util::kFloat16POD:
                {
                    TESTING_ASSERT(ap->getName() == "float16");
                    TESTING_ASSERT(!ap->isScalarLike());
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(val->getDimensions().numPoints() == 2);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    Dimensions dims0;
                    ap->getDimensions(0, dims0);
                    TESTING_ASSERT(dims0.rank() == 1);
                    TESTING_ASSERT(dims0.numPoints() == 2);
                    Alembic::Util::float16_t * data =
                        (Alembic::Util::float16_t *)(val->getData());
                    TESTING_ASSERT(data[0] == 16.0);
                    TESTING_ASSERT(data[1] == -3.0);

                    // read it as something else
                    Alembic::Util::float32_t data2[2];
                    ap->getAs(0, data2, kFloat32POD);
                    TESTING_ASSERT(data2[0] == 16.0);
                    TESTING_ASSERT(data2[1] == -3.0);

                    // read it as it is
                    Alembic::Util::float16_t data3[2];
                    ap->getAs(0, data3, kFloat16POD);
                    TESTING_ASSERT(data3[0] == 16.0);
                    TESTING_ASSERT(data3[1] == -3.0);

                }
                break;

                case Alembic::Util::kFloat32POD:
                {
                    TESTING_ASSERT(ap->getName() == "float32" ||
                        ap->getName() == "float32_ext1");
                    TESTING_ASSERT(!ap->isScalarLike());
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    ABCA::ArraySampleKey key;

                    if (ap->getName() == "float32_ext1")
                    {
                        Dimensions dims0;
                        ap->getDimensions(0, dims0);
                        TESTING_ASSERT(dims0.rank() == 1);
                        TESTING_ASSERT(dims0.numPoints() == 4);
                        ap->getKey(0, key);
                        TESTING_ASSERT(key.numBytes == 16);
                    }

                    if (ap->getName() == "float32")
                    {
                        TESTING_ASSERT( ap->getDataType().getExtent() == 2);
                        Dimensions dims0;
                        ap->getDimensions(0, dims0);
                        TESTING_ASSERT(dims0.rank() == 1);
                        TESTING_ASSERT(dims0.numPoints() == 2);
                        ap->getKey(0, key);
                        TESTING_ASSERT(key.numBytes == 16);
                    }

                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    Alembic::Util::float32_t * data =
                        (Alembic::Util::float32_t *)(val->getData());
                    TESTING_ASSERT(data[0] == 128.0);
                    TESTING_ASSERT(data[1] == -13.25);
                    TESTING_ASSERT(data[2] == 35.5);
                    TESTING_ASSERT(data[3] == 128.125);

                    // read as something else
                    Alembic::Util::float64_t data2[4];
                    ap->getAs(0, data2, kFloat64POD);
                    TESTING_ASSERT(data2[0] == 128.0);
                    TESTING_ASSERT(data2[1] == -13.25);
                    TESTING_ASSERT(data2[2] == 35.5);
                    TESTING_ASSERT(data2[3] == 128.125);

                    // read it as it is
                    Alembic::Util::float32_t data3[4];
                    ap->getAs(0, data3, kFloat32POD);
                    TESTING_ASSERT(data3[0] == 128.0);
                    TESTING_ASSERT(data3[1] == -13.25);
                    TESTING_ASSERT(data3[2] == 35.5);
                    TESTING_ASSERT(data3[3] == 128.125);

                    // read it as an int32_t
                    Alembic::Util::int32_t data4[4];
                    ap->getAs(0, data4, kInt32POD);
                    TESTING_ASSERT(data4[0] == 128);
                    TESTING_ASSERT(data4[1] == -13);
                    TESTING_ASSERT(data4[2] == 35);
                    TESTING_ASSERT(data4[3] == 128);

                    // can't read as strings, and it shouldn't touch the buffer
                    TESTING_ASSERT_THROW(ap->getAs(0, data4, kStringPOD),
                        Alembic::Util::Exception);

                    TESTING_ASSERT_THROW(ap->getAs(0, data4, kWstringPOD),
                        Alembic::Util::Exception);
                    TESTING_ASSERT(data4[0] == 128);
                    TESTING_ASSERT(data4[1] == -13);
                    TESTING_ASSERT(data4[2] == 35);
                    TESTING_ASSERT(data4[3] == 128);
                }
                break;

                case Alembic::Util::kFloat64POD:
                {
                    TESTING_ASSERT(ap->getName() == "float64" ||
                        ap->getName() == "float64_ext3");
                    TESTING_ASSERT(!ap->isScalarLike());

                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    ABCA::ArraySampleKey key;

                    if (ap->getName() == "float64")
                    {
                        Dimensions dims0;
                        ap->getDimensions(0, dims0);
                        TESTING_ASSERT(dims0.rank() == 1);
                        TESTING_ASSERT(dims0.numPoints() == 30);
                        ap->getKey(0, key);
                        TESTING_ASSERT(key.numBytes == 240);
                    }

                    if (ap->getName() == "float64_ext3")
                    {
                        Dimensions dims0;
                        ap->getDimensions(0, dims0);
                        TESTING_ASSERT(dims0.rank() == 1);
                        TESTING_ASSERT(dims0.numPoints() == 10);
                        TESTING_ASSERT( ap->getDataType().getExtent() == 3);
                        ap->getKey(0, key);
                        TESTING_ASSERT(key.numBytes == 240);
                    }

                    Alembic::Util::float64_t * data =
                        (Alembic::Util::float64_t *)(val->getData());

                    Alembic::Util::float32_t data2[30];
                    ap->getAs(0, data2, kFloat32POD);

                    Alembic::Util::float64_t data3[30];
                    ap->getAs(0, data3, kFloat64POD);

                    Alembic::Util::uint32_t data4[30];
                    ap->getAs(0, data4, kUint32POD);

                    for (size_t i = 0; i < val->size(); ++i)
                    {
                        TESTING_ASSERT(data[i] == i* 100);
                        TESTING_ASSERT(data2[i] == i* 100);
                        TESTING_ASSERT(data3[i] == i* 100);
                        TESTING_ASSERT(data4[i] == i* 100);
                    }
                }
                break;

                case Alembic::Util::kStringPOD:
                {
                    TESTING_ASSERT(ap->getName() == "str");
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(!ap->isScalarLike());
                    TESTING_ASSERT(val->getDimensions().numPoints() == 4);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    Dimensions dims0;
                    ap->getDimensions(0, dims0);
                    TESTING_ASSERT(dims0.rank() == 1);
                    TESTING_ASSERT(dims0.numPoints() == 4);
                    Alembic::Util::string * data =
                        (Alembic::Util::string *)(val->getData());
                    TESTING_ASSERT(data[0] == "Now it's time");
                    TESTING_ASSERT(data[1] == "");
                    TESTING_ASSERT(data[2] == "for");
                    TESTING_ASSERT(data[3] == "cake!");

                    Alembic::Util::string data2[4];
                    ap->getAs(0, data2, kStringPOD);
                    TESTING_ASSERT(data2[0] == "Now it's time");
                    TESTING_ASSERT(data2[1] == "");
                    TESTING_ASSERT(data2[2] == "for");
                    TESTING_ASSERT(data2[3] == "cake!");

                    // can't read strings as anything else
                    Alembic::Util::int32_t data3[4];
                    TESTING_ASSERT_THROW(ap->getAs(0, data3, kInt32POD),
                        Alembic::Util::Exception);
                }
                break;

                case Alembic::Util::kWstringPOD:
                {
                    TESTING_ASSERT(ap->getName() == "wstr");
                    ABCA::ArraySamplePtr val;
                    ap->getSample(0, val);
                    TESTING_ASSERT(!ap->isScalarLike());
                    TESTING_ASSERT(val->getDimensions().numPoints() == 4);
                    TESTING_ASSERT(val->getDimensions().rank() == 1);
                    Dimensions dims0;
                    ap->getDimensions(0, dims0);
                    TESTING_ASSERT(dims0.rank() == 1);
                    TESTING_ASSERT(dims0.numPoints() == 4);
                    Alembic::Util::wstring * data =
                        (Alembic::Util::wstring *)(val->getData());
                    TESTING_ASSERT(data[0] == L"We do what we must ");
                    TESTING_ASSERT(data[1] == L"because we can.");
                    TESTING_ASSERT(data[2] == L"");
                    TESTING_ASSERT(data[3] ==  L"\uf8e4 \uf8e2 \uf8d3");

                    Alembic::Util::wstring data2[4];
                    ap->getAs(0, data2, kWstringPOD);
                    TESTING_ASSERT(data[0] == L"We do what we must ");
                    TESTING_ASSERT(data[1] == L"because we can.");
                    TESTING_ASSERT(data[2] == L"");
                    TESTING_ASSERT(data[3] ==  L"\uf8e4 \uf8e2 \uf8d3");

                    // can't read strings as anything else
                    Alembic::Util::int32_t data3[4];
                    TESTING_ASSERT_THROW(ap->getAs(0, data3, kInt32POD),
                        Alembic::Util::Exception);
                }
                break;

                default:
                    TESTING_ASSERT(false);
                break;
            }
        }
    }  // end of reading
}

//-*****************************************************************************
void testEmptyArray()
{
    std::string archiveName = "emptyArray.abc";
    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();

        ABCA::CompoundPropertyWriterPtr parent = archive->getProperties();

        ABCA::ArrayPropertyWriterPtr awp =
            parent->createArrayProperty("emptyInt32", ABCA::MetaData(),
                                        ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);
        TESTING_ASSERT(awp->getNumSamples() == 0);
    }

    {
        AO::ReadArchive r;
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();
        ABCA::CompoundPropertyReaderPtr parent = archive->getProperties();

        TESTING_ASSERT(parent->getNumProperties() == 1);

        ABCA::BasePropertyReaderPtr bp = parent->getProperty( 0 );
        TESTING_ASSERT(bp->isArray());
        TESTING_ASSERT(bp->asScalarPtr() == NULL);
        TESTING_ASSERT(bp->getParent() == parent);
        ABCA::ArrayPropertyReaderPtr ap = bp->asArrayPtr();
        TESTING_ASSERT(ap->getNumSamples() == 0);
        TESTING_ASSERT(ap->getNumSamples() == 0);
        TESTING_ASSERT(ap->isConstant());
        ABCA::ArraySamplePtr as;

        TESTING_ASSERT_THROW(ap->getSample(0, as), Alembic::Util::Exception);
    }

    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();

        ABCA::CompoundPropertyWriterPtr parent = archive->getProperties();

        // no extent should throw
        TESTING_ASSERT_THROW(parent->createArrayProperty("emptyInt32",
                                                         ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 0), 0),
                             Alembic::Util::Exception);

        // 2 parts of the enum that shouldn't be written out
        TESTING_ASSERT_THROW(parent->createArrayProperty("emptyInt32",
                                                         ABCA::MetaData(), ABCA::DataType(Alembic::Util::kUnknownPOD, 1), 0),
                             Alembic::Util::Exception);

        TESTING_ASSERT_THROW(parent->createArrayProperty("emptyInt32",
                                                         ABCA::MetaData(),
                                                         ABCA::DataType(Alembic::Util::kNumPlainOldDataTypes, 1), 0),
                             Alembic::Util::Exception);

        // illegal time sampling index
        TESTING_ASSERT_THROW(parent->createArrayProperty("emptyInt32",
                                                         ABCA::MetaData(),
                                                         ABCA::DataType(Alembic::Util::kInt32POD, 1), 45),
                             Alembic::Util::Exception);

        // lets do it again for the Scalars

        // no extent should throw
        TESTING_ASSERT_THROW(parent->createScalarProperty("emptyInt32",
                                                          ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 0), 0),
                             Alembic::Util::Exception);

        // 2 parts of the enum that shouldn't be written out
        TESTING_ASSERT_THROW(parent->createScalarProperty("emptyInt32",
                                                          ABCA::MetaData(),
                                                          ABCA::DataType(Alembic::Util::kUnknownPOD, 1), 0),
                             Alembic::Util::Exception);

        TESTING_ASSERT_THROW(parent->createScalarProperty("emptyInt32",
                                                          ABCA::MetaData(),
                                                          ABCA::DataType(Alembic::Util::kNumPlainOldDataTypes, 1), 0),
                             Alembic::Util::Exception);

        // illegal time sampling index
        TESTING_ASSERT_THROW(parent->createScalarProperty("emptyInt32",
                                                          ABCA::MetaData(),
                                                          ABCA::DataType(Alembic::Util::kInt32POD, 1), 42),
                             Alembic::Util::Exception);

        parent->createArrayProperty("veryEmptyInt32",
                                    ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0);

        // this should throw because the name isn't unique
        TESTING_ASSERT_THROW(parent->createScalarProperty("veryEmptyInt32",
                                                          ABCA::MetaData(), ABCA::DataType(Alembic::Util::kInt32POD, 1), 0),
                             Alembic::Util::Exception);

        ABCA::ArrayPropertyWriterPtr ap = parent->createArrayProperty(
            "emptyInt64", ABCA::MetaData(),
            ABCA::DataType(Alembic::Util::kInt64POD, 1), 0);

        // In order to write a zero-length array, you still have to specify
        // non-rank-0 dimensions.
        ABCA::Dimensions zeroLengthDims;
        zeroLengthDims.setRank( 1 );
        zeroLengthDims[0] = 0;
        ABCA::ArraySample samp(NULL, ABCA::DataType(Alembic::Util::kInt64POD, 1),
                              zeroLengthDims );
        ap->setSample( samp );
    }

    {
        AO::ReadArchive r;
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();
        ABCA::CompoundPropertyReaderPtr parent = archive->getProperties();

        TESTING_ASSERT(parent->getNumProperties() == 2);

        for (size_t i = 0; i < parent->getNumProperties(); ++i)
        {
            ABCA::BasePropertyReaderPtr bp = parent->getProperty( i );
            TESTING_ASSERT(bp->isArray());
            TESTING_ASSERT(bp->asScalarPtr() == NULL);
            TESTING_ASSERT(bp->getParent() == parent);
            ABCA::ArrayPropertyReaderPtr ap = bp->asArrayPtr();
            if (ap->getName() == "veryEmptyInt32")
            {
                TESTING_ASSERT(ap->getNumSamples() == 0);

                // no samples?  it's scalar like
                TESTING_ASSERT(ap->isScalarLike());
            }
            else if (ap->getName() == "emptyInt64")
            {
                ABCA::ArraySampleKey key;

                TESTING_ASSERT(ap->getNumSamples() == 1);
                ABCA::ArraySamplePtr samp;
                ap->getSample(0, samp);
                Dimensions dims0;
                TESTING_ASSERT(dims0.numPoints() == 0);
                TESTING_ASSERT(samp->getDimensions().numPoints() == 0);
                TESTING_ASSERT(!ap->isScalarLike());
                TESTING_ASSERT(ap->getKey(0, key));
                TESTING_ASSERT(key.numBytes == 0);
                TESTING_ASSERT(key.origPOD == Alembic::Util::kInt64POD);
                TESTING_ASSERT(key.readPOD == Alembic::Util::kInt64POD);
                TESTING_ASSERT(key.digest.str() ==
                               "00000000000000000000000000000000");
            }
        }
    }
}

//-*****************************************************************************
void testExtentArrayStrings()
{
    std::string archiveName = "extentStrArray.abc";
    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();

        ABCA::CompoundPropertyWriterPtr parent = archive->getProperties();

        ABCA::DataType dtype(Alembic::Util::kStringPOD, 2);

        ABCA::ArrayPropertyWriterPtr awp =
            parent->createArrayProperty("str", ABCA::MetaData(), dtype, 0);

        std::vector < Alembic::Util::string > vals(6);
        vals[0] = "Peanut";
        vals[1] = "butter";
        vals[2] = "jelly";
        vals[3] = "time";
        vals[4] = "nom nom";
        vals[5] = "";

        std::vector < Alembic::Util::string > vals2(4);
        vals2[0] = "";
        vals2[1] = "Is the cake really a lie?";
        vals2[2] = "";
        vals2[3] = "I certainly hope not.";

        std::vector < Alembic::Util::string > vals3(4);
        vals3[0] = "Is the cake really a lie?";
        vals3[1] = "";
        vals3[2] = "";
        vals3[3] = "I certainly hope not.";

        std::vector < Alembic::Util::string > vals4(6);
        vals4[0] = "a";
        vals4[1] = "b";
        vals4[2] = "c";
        vals4[3] = "d";
        vals4[4] = "e";
        vals4[5] = "f";

        awp->setSample(
            ABCA::ArraySample(&(vals.front()), dtype,
                             Alembic::Util::Dimensions(3)));

        awp->setSample(
            ABCA::ArraySample(&(vals2.front()), dtype,
                             Alembic::Util::Dimensions(2)));

        std::vector < Alembic::Util::string > badVals(1);
        badVals[0] = "This better fail.";
        TESTING_ASSERT_THROW(awp->setSample(
                                 ABCA::ArraySample(&(badVals.front()),
                                                  ABCA::DataType(Alembic::Util::kStringPOD, 1),
                                                  Alembic::Util::Dimensions(1))), Alembic::Util::Exception);

        awp->setSample(
            ABCA::ArraySample(&(vals3.front()), dtype,
                             Alembic::Util::Dimensions(2)));

        awp->setSample(
            ABCA::ArraySample(&(vals2.front()), dtype,
                             Alembic::Util::Dimensions(2)));

        awp->setSample(
            ABCA::ArraySample(&(vals4.front()), dtype,
                             Alembic::Util::Dimensions(3)));

        std::vector < Alembic::Util::float32_t > vals5(3);
        vals5[0] = 1.0;
        vals5[1] = 2.0;
        vals5[2] = 3.0;

        // this should fail since we are trying to write float data to
        // a string array property
        TESTING_ASSERT_THROW(awp->setSample(
                                 ABCA::ArraySample(&(vals5.front()),
                                                  ABCA::DataType(Alembic::Util::kFloat32POD, 1),
                                                  Alembic::Util::Dimensions(3))), Alembic::Util::Exception);
    }

    {
        AO::ReadArchive r;
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();
        ABCA::CompoundPropertyReaderPtr parent = archive->getProperties();
        TESTING_ASSERT(parent->getNumProperties() == 1);

        ABCA::BasePropertyReaderPtr bp = parent->getProperty( 0 );
        TESTING_ASSERT(bp->isArray());
        ABCA::ArrayPropertyReaderPtr ap = bp->asArrayPtr();
        TESTING_ASSERT(ap->getNumSamples() == 5);

        ABCA::ArraySamplePtr val;

        ap->getSample(0, val);
        TESTING_ASSERT(val->getDimensions().numPoints() == 3);
        TESTING_ASSERT(val->getDimensions().rank() == 1);

        Dimensions dims0;
        ap->getDimensions(0, dims0);
        TESTING_ASSERT(dims0.numPoints() == 3);
        TESTING_ASSERT(dims0.rank() == 1);

        Alembic::Util::string * data = (Alembic::Util::string *)(val->getData());

        TESTING_ASSERT(data[0] == "Peanut");
        TESTING_ASSERT(data[1] == "butter");
        TESTING_ASSERT(data[2] == "jelly");
        TESTING_ASSERT(data[3] == "time");
        TESTING_ASSERT(data[4] == "nom nom");
        TESTING_ASSERT(data[5] == "");

        ABCA::ArraySampleKey key;
        TESTING_ASSERT(ap->getKey(0, key));

        // includes NULL seperator
        TESTING_ASSERT(key.numBytes == 34);
        TESTING_ASSERT(key.origPOD == Alembic::Util::kStringPOD);
        TESTING_ASSERT(key.readPOD == Alembic::Util::kStringPOD);
        TESTING_ASSERT(key.digest.str() ==
                       "88d5ea1d772131992c9be9a192938df7");

        ap->getSample(1, val);
        TESTING_ASSERT(val->getDimensions().numPoints() == 2);
        TESTING_ASSERT(val->getDimensions().rank() == 1);

        ap->getDimensions(1, dims0);
        TESTING_ASSERT(dims0.numPoints() == 2);
        TESTING_ASSERT(dims0.rank() == 1);

        data = (Alembic::Util::string *)(val->getData());
        TESTING_ASSERT(data[0] == "");
        TESTING_ASSERT(data[1] == "Is the cake really a lie?");
        TESTING_ASSERT(data[2] == "");
        TESTING_ASSERT(data[3] == "I certainly hope not.");
        TESTING_ASSERT(ap->getKey(1, key));
        TESTING_ASSERT(key.numBytes == 50);
        TESTING_ASSERT(key.origPOD == Alembic::Util::kStringPOD);
        TESTING_ASSERT(key.readPOD == Alembic::Util::kStringPOD);
        TESTING_ASSERT(key.digest.str() ==
                       "126ebab40166a642d3c8cc4c5929074d");

        ap->getSample(2, val);
        TESTING_ASSERT(val->getDimensions().numPoints() == 2);
        TESTING_ASSERT(val->getDimensions().rank() == 1);
        ap->getDimensions(2, dims0);
        TESTING_ASSERT(dims0.numPoints() == 2);
        TESTING_ASSERT(dims0.rank() == 1);
        data = (Alembic::Util::string *)(val->getData());
        TESTING_ASSERT(data[0] == "Is the cake really a lie?");
        TESTING_ASSERT(data[1] == "");
        TESTING_ASSERT(data[2] == "");
        TESTING_ASSERT(data[3] == "I certainly hope not.");
        TESTING_ASSERT(ap->getKey(2, key));
        TESTING_ASSERT(key.numBytes == 50);
        TESTING_ASSERT(key.origPOD == Alembic::Util::kStringPOD);
        TESTING_ASSERT(key.readPOD == Alembic::Util::kStringPOD);
        TESTING_ASSERT(key.digest.str() ==
                       "a197a3747b0666dfacdc17634f5cabff");

        ap->getSample(3, val);
        ap->getDimensions(3, dims0);
        TESTING_ASSERT(dims0.numPoints() == 2);
        TESTING_ASSERT(dims0.rank() == 1);
        TESTING_ASSERT(val->getDimensions().numPoints() == 2);
        TESTING_ASSERT(val->getDimensions().rank() == 1);
        data = (Alembic::Util::string *)(val->getData());
        TESTING_ASSERT(data[0] == "");
        TESTING_ASSERT(data[1] == "Is the cake really a lie?");
        TESTING_ASSERT(data[2] == "");
        TESTING_ASSERT(data[3] == "I certainly hope not.");
        TESTING_ASSERT(ap->getKey(3, key));
        TESTING_ASSERT(key.numBytes == 50);
        TESTING_ASSERT(key.origPOD == Alembic::Util::kStringPOD);
        TESTING_ASSERT(key.readPOD == Alembic::Util::kStringPOD);
        TESTING_ASSERT(key.digest.str() ==
                       "126ebab40166a642d3c8cc4c5929074d");

        ap->getSample(4, val);
        ap->getDimensions(4, dims0);
        TESTING_ASSERT(dims0.numPoints() == 3);
        TESTING_ASSERT(dims0.rank() == 1);
        TESTING_ASSERT(val->getDimensions().numPoints() == 3);
        TESTING_ASSERT(val->getDimensions().rank() == 1);
        data = (Alembic::Util::string *)(val->getData());
        TESTING_ASSERT(data[0] == "a");
        TESTING_ASSERT(data[1] == "b");
        TESTING_ASSERT(data[2] == "c");
        TESTING_ASSERT(data[3] == "d");
        TESTING_ASSERT(data[4] == "e");
        TESTING_ASSERT(data[5] == "f");
        TESTING_ASSERT(ap->getKey(4, key));
        TESTING_ASSERT(key.numBytes == 12);
        TESTING_ASSERT(key.origPOD == Alembic::Util::kStringPOD);
        TESTING_ASSERT(key.readPOD == Alembic::Util::kStringPOD);
    }
}

//-*****************************************************************************
void testArrayStringsRepeats()
{
    std::string archiveName = "strArrayRepeats.abc";
    std::vector < Alembic::Util::string > vals(6);
    vals[0] = "if you want";
    vals[1] = "a revolution";
    vals[2] = "the only";
    vals[3] = "solution";
    vals[4] = "...";
    vals[5] = "evolve";

    std::vector < Alembic::Util::string > vals2(4);
    vals2[0] = "bom bom";
    vals2[1] = "bom";
    vals2[2] = "bom bom";
    vals2[3] = "bom";

    std::vector < Alembic::Util::string > valsEmpty(4);

    ABCA::DataType dtype(Alembic::Util::kStringPOD);

    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();

        ABCA::CompoundPropertyWriterPtr parent = archive->getProperties();

        ABCA::ArrayPropertyWriterPtr awp =
            parent->createArrayProperty("str", ABCA::MetaData(), dtype, 0);

        // 0
        awp->setSample(ABCA::ArraySample(&(valsEmpty.front()), dtype,
            Alembic::Util::Dimensions(valsEmpty.size())));

        // 1
        awp->setSample(ABCA::ArraySample(&(valsEmpty.front()), dtype,
            Alembic::Util::Dimensions(valsEmpty.size())));

        // 2
        awp->setSample(ABCA::ArraySample(&(valsEmpty.front()), dtype,
            Alembic::Util::Dimensions(valsEmpty.size())));

        // 3
        awp->setSample(
            ABCA::ArraySample(&(vals.front()), dtype,
                        Alembic::Util::Dimensions(vals.size())));

        // 4
        awp->setSample(
            ABCA::ArraySample(&(vals.front()), dtype,
                        Alembic::Util::Dimensions(vals.size())));

        // 5
        awp->setSample(
            ABCA::ArraySample(&(vals.front()), dtype,
                        Alembic::Util::Dimensions(vals.size())));

        // 6
        awp->setSample(
            ABCA::ArraySample(&(vals2.front()), dtype,
                        Alembic::Util::Dimensions(vals2.size())));

        // 7
        awp->setSample(
            ABCA::ArraySample(&(vals2.front()), dtype,
                        Alembic::Util::Dimensions(vals2.size())));

        // 8
        awp->setSample(ABCA::ArraySample(&(valsEmpty.front()), dtype,
            Alembic::Util::Dimensions(valsEmpty.size())));

        // 9
        awp->setSample(ABCA::ArraySample(&(valsEmpty.front()), dtype,
            Alembic::Util::Dimensions(valsEmpty.size())));

        dtype.setExtent(2);
        ABCA::ArrayPropertyWriterPtr awp2 =
            parent->createArrayProperty("str2", ABCA::MetaData(), dtype, 0);

        // 0
        awp2->setSample(
            ABCA::ArraySample(&(vals.front()), dtype,
                        Alembic::Util::Dimensions(vals.size() / 2)));

        // 1
        awp2->setSample(
            ABCA::ArraySample(&(vals.front()), dtype,
                        Alembic::Util::Dimensions(vals.size() / 2)));

        // 2
        awp2->setSample(
            ABCA::ArraySample(&(vals.front()), dtype,
                        Alembic::Util::Dimensions(vals.size() / 2)));

        // 3
        awp2->setSample(
            ABCA::ArraySample(NULL, dtype, Alembic::Util::Dimensions(0)));

        // 4
        awp2->setSample(
            ABCA::ArraySample(&(vals2.front()), dtype,
                        Alembic::Util::Dimensions(vals2.size() / 2)));

        // 5
        awp2->setSample(
            ABCA::ArraySample(&(vals2.front()), dtype,
                        Alembic::Util::Dimensions(vals2.size() / 2)));

    }

    {
        AO::ReadArchive r;
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();
        ABCA::CompoundPropertyReaderPtr parent = archive->getProperties();
        TESTING_ASSERT(parent->getNumProperties() == 2);

        ABCA::ArrayPropertyReaderPtr ap = parent->getArrayProperty("str");
        TESTING_ASSERT(ap->getNumSamples() == 10);

        ABCA::ArrayPropertyReaderPtr ap2 = parent->getArrayProperty("str2");
        TESTING_ASSERT(ap2->getNumSamples() == 6);

        ABCA::ArraySamplePtr val;
        for (int i = 0; i < 10; ++i)
        {
            ap->getSample(i, val);
            Alembic::Util::string * data =
                (Alembic::Util::string *)(val->getData());
            if (i < 3 || i > 7)
            {
                TESTING_ASSERT(val->getDimensions().numPoints() == 4);
                TESTING_ASSERT(val->getDimensions().rank() == 1);
                TESTING_ASSERT(data[0] == "");
                TESTING_ASSERT(data[1] == "");
                TESTING_ASSERT(data[2] == "");
                TESTING_ASSERT(data[3] == "");
            }

            if (i == 3 || i == 4  || i == 5)
            {
                TESTING_ASSERT(val->getDimensions().numPoints() == 6);
                TESTING_ASSERT(val->getDimensions().rank() == 1);
                for (unsigned int j = 0; j < vals.size(); ++j)
                {
                    TESTING_ASSERT(data[j] == vals[j]);
                }
            }

            if (i == 6 || i == 7)
            {
                TESTING_ASSERT(val->getDimensions().numPoints() == 4);
                TESTING_ASSERT(val->getDimensions().rank() == 1);
                for (unsigned int j = 0; j < vals2.size(); ++j)
                {
                    TESTING_ASSERT(data[j] == vals2[j]);
                }
            }
        }

        for (int i = 0; i < 6; ++i)
        {
            ap2->getSample(i, val);
            Alembic::Util::string * data =
                (Alembic::Util::string *)(val->getData());

            if (i < 3)
            {
                TESTING_ASSERT(val->getDimensions().numPoints() == 3);
                TESTING_ASSERT(val->getDimensions().rank() == 1);
                for (unsigned int j = 0; j < vals.size(); ++j)
                {
                    TESTING_ASSERT(data[j] == vals[j]);
                }
            }

            if (i == 3)
            {
                TESTING_ASSERT(val->getDimensions().numPoints() == 0);
                TESTING_ASSERT(val->getDimensions().rank() == 1);
            }

            if (i > 3)
            {
                TESTING_ASSERT(val->getDimensions().numPoints() == 2);
                TESTING_ASSERT(val->getDimensions().rank() == 1);
                for (unsigned int j = 0; j < vals2.size(); ++j)
                {
                    TESTING_ASSERT(data[j] == vals2[j]);
                }
            }
        }
    }
}

void testArraySamples()
{
    std::string archiveName = "numArraySamplesTest.abc";

    ABCA::DataType dtype(Alembic::Util::kStringPOD);
    std::vector < Alembic::Util::string > vals(1);
    vals[0] = "yummy";
    ABCA::ArraySample samp(&(vals.front()), dtype,
        Alembic::Util::Dimensions(vals.size()));

    {
        AO::WriteArchive w;
        ABCA::ArchiveWriterPtr a = w(archiveName, ABCA::MetaData());
        ABCA::ObjectWriterPtr archive = a->getTop();
        ABCA::ObjectWriterPtr obj = archive->createChild(
            ABCA::ObjectHeader("test", ABCA::MetaData()));

        ABCA::CompoundPropertyWriterPtr parent = obj->getProperties();
        ABCA::ArrayPropertyWriterPtr prop;

        ABCA::CompoundPropertyWriterPtr smallProp =
            parent->createCompoundProperty("small", ABCA::MetaData());
        smallProp->createArrayProperty("a", ABCA::MetaData(), dtype, 0);
        prop = smallProp->createArrayProperty("b", ABCA::MetaData(), dtype, 0);
        for (std::size_t i = 0; i < 10; ++i)
        {
            prop->setSample(samp);
        }
        smallProp->createArrayProperty("c", ABCA::MetaData(), dtype, 0);

        ABCA::CompoundPropertyWriterPtr mdProp =
            parent->createCompoundProperty("md", ABCA::MetaData());
        mdProp->createArrayProperty("a", ABCA::MetaData(), dtype, 0);
        prop = mdProp->createArrayProperty("b", ABCA::MetaData(), dtype, 0);
        for (std::size_t i = 0; i < 150; ++i)
        {
            prop->setSample(samp);
        }
        mdProp->createArrayProperty("c", ABCA::MetaData(), dtype, 0);

        ABCA::CompoundPropertyWriterPtr mdlgProp =
            parent->createCompoundProperty("mdlg", ABCA::MetaData());
        mdlgProp->createArrayProperty("a", ABCA::MetaData(), dtype, 0);
        prop = mdlgProp->createArrayProperty("b", ABCA::MetaData(), dtype, 0);
        for (std::size_t i = 0; i < 300; ++i)
        {
            prop->setSample(samp);
        }
        mdlgProp->createArrayProperty("c", ABCA::MetaData(), dtype, 0);

        ABCA::CompoundPropertyWriterPtr lgProp =
            parent->createCompoundProperty("lg", ABCA::MetaData());
        lgProp->createArrayProperty("a", ABCA::MetaData(), dtype, 0);
        prop = lgProp->createArrayProperty("b", ABCA::MetaData(), dtype, 0);
        for (std::size_t i = 0; i < 33000; ++i)
        {
            prop->setSample(samp);
        }
        lgProp->createArrayProperty("c", ABCA::MetaData(), dtype, 0);

        ABCA::CompoundPropertyWriterPtr insaneProp =
            parent->createCompoundProperty("insane", ABCA::MetaData());
        insaneProp->createArrayProperty("a", ABCA::MetaData(), dtype, 0);
        prop = insaneProp->createArrayProperty("b", ABCA::MetaData(), dtype, 0);
        for (std::size_t i = 0; i < 66000; ++i)
        {
            prop->setSample(samp);
        }
        insaneProp->createArrayProperty("c", ABCA::MetaData(), dtype, 0);
    }

    {
        AO::ReadArchive r;
        ABCA::ArchiveReaderPtr a = r( archiveName );
        ABCA::ObjectReaderPtr archive = a->getTop();
        ABCA::ObjectReaderPtr obj = archive->getChild(0);
        ABCA::CompoundPropertyReaderPtr parent = obj->getProperties();

        ABCA::CompoundPropertyReaderPtr smallProp =
            parent->getCompoundProperty("small");
        TESTING_ASSERT(smallProp->getNumProperties() == 3);
        TESTING_ASSERT(smallProp->getArrayProperty("b")->getNumSamples() == 10);

        ABCA::CompoundPropertyReaderPtr mdProp =
            parent->getCompoundProperty("md");
        TESTING_ASSERT(mdProp->getNumProperties() == 3);
        TESTING_ASSERT(mdProp->getArrayProperty("b")->getNumSamples() == 150);

        ABCA::CompoundPropertyReaderPtr mdlgProp =
            parent->getCompoundProperty("mdlg");
        TESTING_ASSERT(mdlgProp->getNumProperties() == 3);
        TESTING_ASSERT(mdlgProp->getArrayProperty("b")->getNumSamples() == 300);

        ABCA::CompoundPropertyReaderPtr lgProp =
            parent->getCompoundProperty("lg");
        TESTING_ASSERT(lgProp->getNumProperties() == 3);
        TESTING_ASSERT(lgProp->getArrayProperty("b")->getNumSamples()
                       == 33000);

        ABCA::CompoundPropertyReaderPtr insaneProp =
            parent->getCompoundProperty("insane");
        TESTING_ASSERT(insaneProp->getNumProperties() == 3);
        TESTING_ASSERT(insaneProp->getArrayProperty("b")->getNumSamples()
                       == 66000);
    }
}

int main ( int argc, char *argv[] )
{
    testEmptyArray();
    testDuplicateArray();
    testReadWriteArrays();
    testExtentArrayStrings();
    testArrayStringsRepeats();
    testArraySamples();
    return 0;
}
