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
void testWeirdStringScalar()
{
    std::string archiveName = "weirdStr.abc";

    Alembic::Util::string weirdStr = "Total failure";
    weirdStr[5] = '\0';

    Alembic::Util::wstring weirdWstr = L"Failure is always an option";
    weirdWstr[5] = L'\0';

    std::vector < Alembic::Util::string > weirdStrArray(3);
    weirdStrArray[0] = "watch";
    weirdStrArray[1] = "this";
    weirdStrArray[2] = "please";
    weirdStrArray[2][3] = '\0';

    Alembic::Util::string empty;
    Alembic::Util::wstring wempty;

    std::vector < Alembic::Util::string > allEmptyStr(3);

    std::vector < Alembic::Util::string > partEmptyStr(6);
    partEmptyStr[0] = "";
    partEmptyStr[1] = "";
    partEmptyStr[2] = "notEmpty!";
    partEmptyStr[3] = "";
    partEmptyStr[4] = "also not empty";
    partEmptyStr[5] = "";

    {
        A5::WriteArchive w;
        AbcA::ArchiveWriterPtr a = w(archiveName, AbcA::MetaData());
        AbcA::ObjectWriterPtr archive = a->getTop();

        AbcA::CompoundPropertyWriterPtr props = archive->getProperties();

        const AbcA::TimeSamplingType staticSampling;

        {

            AbcA::ScalarPropertyWriterPtr emptyWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("empty", AbcA::kScalarProperty,
                    AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kStringPOD, 1),
                    staticSampling));

            // this should fail because of the NULL char in the string
            TESTING_ASSERT_THROW(emptyWrtPtr->setSample(0, 0.0, &weirdStr),
                Alembic::Util::Exception);

            emptyWrtPtr->setSample(0, 0.0, &empty);

            AbcA::ScalarPropertyWriterPtr wemptyWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("wempty", AbcA::kScalarProperty,
                                        AbcA::MetaData(),
                                        AbcA::DataType(Alembic::Util::kWstringPOD, 1),
                                        staticSampling));

            // this should fail because of the NULL char in the string
            TESTING_ASSERT_THROW(wemptyWrtPtr->setSample(0, 0.0, &weirdWstr),
                                 Alembic::Util::Exception);

            wemptyWrtPtr->setSample(0, 0.0, &wempty);


            AbcA::ScalarPropertyWriterPtr allEmptyWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("allEmpty", AbcA::kScalarProperty,
                    AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kStringPOD, 3),
                    staticSampling));

            // one of the strings has a NULL char in it
            TESTING_ASSERT_THROW(
                allEmptyWrtPtr->setSample(0, 0.0, &(weirdStrArray.front())),
                Alembic::Util::Exception);

            allEmptyWrtPtr->setSample(0, 0.0, &(allEmptyStr.front()));

            AbcA::ScalarPropertyWriterPtr partEmptyStrPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("partEmpty", AbcA::kScalarProperty,
                    AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kStringPOD, 6),
                    staticSampling));

            partEmptyStrPtr->setSample(0, 0.0, &(partEmptyStr.front()));
        }
    }

    {
        A5::ReadArchive r;
        AbcA::ArchiveReaderPtr a = r( archiveName );
        AbcA::ObjectReaderPtr archive = a->getTop();
        AbcA::CompoundPropertyReaderPtr parent = archive->getProperties();

        for (size_t i = 0; i < parent->getNumProperties(); ++i)
        {
            AbcA::BasePropertyReaderPtr bp = parent->getProperty( i );
            if (bp->getName() == "partEmpty")
            {
                std::vector < Alembic::Util::string > val (6);
                AbcA::ScalarPropertyReaderPtr sp = bp->asScalarPtr();
                TESTING_ASSERT(sp->getDataType().getExtent() == 6);
                sp->getSample(0, &(val.front()));
                TESTING_ASSERT(val == partEmptyStr);
            }
            else if (bp->getName() == "allEmpty")
            {
                std::vector < Alembic::Util::string > val (3);
                AbcA::ScalarPropertyReaderPtr sp = bp->asScalarPtr();
                TESTING_ASSERT(sp->getDataType().getExtent() == 3);
                sp->getSample(0, &(val.front()));
                TESTING_ASSERT(val == allEmptyStr);
            }
            else if (bp->getName() == "empty")
            {
                Alembic::Util::string val;
                AbcA::ScalarPropertyReaderPtr sp = bp->asScalarPtr();
                sp->getSample(0, &val);
                TESTING_ASSERT(val == empty);
            }
            else if (bp->getName() == "wempty")
            {
                Alembic::Util::wstring val;
                AbcA::ScalarPropertyReaderPtr sp = bp->asScalarPtr();
                sp->getSample(0, &val);
                TESTING_ASSERT(val == wempty);
            }
            else
            {
                TESTING_ASSERT(false);
            }
        }

    }
}

//-*****************************************************************************
void testReadWriteScalars()
{

    std::string archiveName = "staticProperties.abc";

    {
        A5::WriteArchive w;
        AbcA::ArchiveWriterPtr a = w(archiveName, AbcA::MetaData());
        AbcA::ObjectWriterPtr archive = a->getTop();

        AbcA::CompoundPropertyWriterPtr props = archive->getProperties();

        const AbcA::TimeSamplingType staticSampling;

        {
            AbcA::ScalarPropertyWriterPtr boolWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("bool", AbcA::kScalarProperty,
                    AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kBooleanPOD, 1),
                    staticSampling));
            Alembic::Util::bool_t b = true;
            boolWrtPtr->setSample(0, 0.0, &b);
        }


        {
            AbcA::ScalarPropertyWriterPtr ucharWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("uchar", AbcA::kScalarProperty,
                    AbcA::MetaData(), AbcA::DataType(Alembic::Util::kUint8POD, 1),
                    staticSampling));

            Alembic::Util::uint8_t uc = 200;

            TESTING_ASSERT(ucharWrtPtr->getNumSamples() == 0);
            ucharWrtPtr->setSample(0, 0.0, &uc);
            TESTING_ASSERT(ucharWrtPtr->getNumSamples() == 1);
        }

        {
            AbcA::ScalarPropertyWriterPtr charWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("char", AbcA::kScalarProperty,
                    AbcA::MetaData(), AbcA::DataType(Alembic::Util::kInt8POD, 1),
                    staticSampling));
            Alembic::Util::int8_t c = -20;
            charWrtPtr->setSample(0, 0.0, &c);
        }

        {
            AbcA::ScalarPropertyWriterPtr ushortWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("ushort", AbcA::kScalarProperty,
                    AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kUint16POD, 1),
                    staticSampling));
            Alembic::Util::uint16_t us = 60000;
            ushortWrtPtr->setSample(0, 0.0, &us);
        }

        {
            AbcA::ScalarPropertyWriterPtr shortWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("short", AbcA::kScalarProperty,
                    AbcA::MetaData(), AbcA::DataType(Alembic::Util::kInt16POD, 1),
                    staticSampling));
            Alembic::Util::int16_t s = -20000;
            shortWrtPtr->setSample(0, 0.0, &s);
        }

        {
            AbcA::ScalarPropertyWriterPtr uintWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("uint", AbcA::kScalarProperty,
                    AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kUint32POD, 1),
                    staticSampling));
            Alembic::Util::uint32_t ui = 1000000;
            uintWrtPtr->setSample(0, 0.0, &ui);
        }

        {
            AbcA::ScalarPropertyWriterPtr intWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("int", AbcA::kScalarProperty,
                    AbcA::MetaData(), AbcA::DataType(Alembic::Util::kInt32POD, 1),
                    staticSampling));
            Alembic::Util::int32_t i = -1000000;
            intWrtPtr->setSample(0, 0.0, &i);
        }

        {
            AbcA::ScalarPropertyWriterPtr ui64WrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("uint64", AbcA::kScalarProperty,
                    AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kUint64POD, 1),
                    staticSampling));
            Alembic::Util::uint64_t ui = 5000000000LL;
            ui64WrtPtr->setSample(0, 0.0, &ui);
        }

        {
            AbcA::ScalarPropertyWriterPtr i64WrtPtr =
                props->createScalarProperty(
                AbcA::PropertyHeader("i64", AbcA::kScalarProperty,
                AbcA::MetaData(), AbcA::DataType(Alembic::Util::kInt64POD, 1),
                staticSampling));
            Alembic::Util::int64_t i = -5000000000LL;
            i64WrtPtr->setSample(0, 0.0, &i);
        }

        {
            AbcA::ScalarPropertyWriterPtr halfWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("half", AbcA::kScalarProperty,
                    AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kFloat16POD, 1),
                    staticSampling));

            Alembic::Util::float16_t h = 16.0;
            halfWrtPtr->setSample(0, 0.0, &h);
        }

        {
            AbcA::ScalarPropertyWriterPtr floatWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("float", AbcA::kScalarProperty,
                    AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kFloat32POD, 1),
                    staticSampling));
            Alembic::Util::float32_t f = 128.0;
            floatWrtPtr->setSample(0, 0.0, &f);
        }

        {
            AbcA::ScalarPropertyWriterPtr doubleWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("double", AbcA::kScalarProperty,
                    AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kFloat64POD, 1),
                    staticSampling));

            Alembic::Util::float64_t d = 32768.0;
            TESTING_ASSERT(doubleWrtPtr->getNumSamples() == 0);
            doubleWrtPtr->setSample(0, 0.0, &d);
            TESTING_ASSERT(doubleWrtPtr->getNumSamples() == 1);
        }

        {
            AbcA::ScalarPropertyWriterPtr strWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader("str", AbcA::kScalarProperty,
                    AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kStringPOD, 1),
                    staticSampling));

            Alembic::Util::string c = "This was a triumph!";
            strWrtPtr->setSample(0, 0.0, &c);
        }

        {
            AbcA::ScalarPropertyWriterPtr wstrWrtPtr =
                props->createScalarProperty(
                    AbcA::PropertyHeader(
                        "wstr", AbcA::kScalarProperty,
                        AbcA::MetaData(),
                        AbcA::DataType(Alembic::Util::kWstringPOD, 1),
                        staticSampling));

            Alembic::Util::wstring c( L"Matt Lauer can suck it! \u2697" );
            wstrWrtPtr->setSample(0, 0.0, &c);
        }

    }

    // now we read what we've written
    {
        A5::ReadArchive r;
        AbcA::ArchiveReaderPtr a = r( archiveName );
        AbcA::ObjectReaderPtr archive = a->getTop();
        AbcA::CompoundPropertyReaderPtr parent = archive->getProperties();

        TESTING_ASSERT(parent->getNumProperties() == 14);
        for ( size_t i = 0; i < parent->getNumProperties(); ++i )
        {
            AbcA::BasePropertyReaderPtr bp = parent->getProperty( i );

            // they are all supposed to be scalar
            TESTING_ASSERT( bp->isScalar() );

            AbcA::ScalarPropertyReaderPtr sp = bp->asScalarPtr();
            TESTING_ASSERT( sp->getNumSamples() == 1 );
            TESTING_ASSERT( sp->isConstant() );
            TESTING_ASSERT( sp->getParent() == parent);
            TESTING_ASSERT( sp->getDataType().getExtent() == 1);
            switch (sp->getDataType().getPod())
            {
                case Alembic::Util::kBooleanPOD:
                {
                    TESTING_ASSERT(sp->getName() == "bool");
                    Alembic::Util::bool_t val = false;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == true);
                }
                break;

                case Alembic::Util::kUint8POD:
                {
                    TESTING_ASSERT(sp->getName() == "uchar");
                    Alembic::Util::uint8_t val = 0;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == 200);
                }
                break;

                case Alembic::Util::kInt8POD:
                {
                    TESTING_ASSERT(sp->getName() == "char");
                    Alembic::Util::int8_t val = 0;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == -20);
                }
                break;

                case Alembic::Util::kUint16POD:
                {
                    TESTING_ASSERT(sp->getName() == "ushort");
                    Alembic::Util::uint16_t val = 0;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == 60000);
                }
                break;

                case Alembic::Util::kInt16POD:
                {
                    TESTING_ASSERT(sp->getName() == "short");
                    Alembic::Util::int16_t val = 0;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == -20000);
                }
                break;

                case Alembic::Util::kUint32POD:
                {
                    TESTING_ASSERT(sp->getName() == "uint");
                    Alembic::Util::uint32_t val = 0;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == 1000000);
                }
                break;

                case Alembic::Util::kInt32POD:
                {
                    TESTING_ASSERT(sp->getName() == "int");
                    Alembic::Util::int32_t val = 0;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == -1000000);
                }
                break;

                case Alembic::Util::kUint64POD:
                {
                    TESTING_ASSERT(sp->getName() == "uint64");
                    Alembic::Util::uint64_t val = 0;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == 5000000000LL);
                }
                break;

                case Alembic::Util::kInt64POD:
                {
                    TESTING_ASSERT(sp->getName() == "i64");
                    Alembic::Util::uint64_t val = 0;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == -5000000000LL);
                }
                break;

                case Alembic::Util::kFloat16POD:
                {
                    TESTING_ASSERT(sp->getName() == "half");
                    Alembic::Util::float16_t val = 0;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == 16.0);
                }
                break;

                case Alembic::Util::kFloat32POD:
                {
                    TESTING_ASSERT(sp->getName() == "float");
                    Alembic::Util::float32_t val = 0;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == 128.0);
                }
                break;

                case Alembic::Util::kFloat64POD:
                {
                    TESTING_ASSERT(sp->getName() == "double");
                    Alembic::Util::float64_t val = 0;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == 32768.0);
                }
                break;

                case Alembic::Util::kStringPOD:
                {
                    TESTING_ASSERT(sp->getName() == "str");
                    Alembic::Util::string val;
                    sp->getSample(0, &val);
                    TESTING_ASSERT(val == "This was a triumph!");
                }
                break;

            case Alembic::Util::kWstringPOD:
            {
                TESTING_ASSERT(sp->getName() == "wstr");
                Alembic::Util::wstring val;
                sp->getSample(0, &val);
                TESTING_ASSERT(val == L"Matt Lauer can suck it! \u2697");
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
void testRepeatedScalarData()
{
    std::string archiveName = "repeatScalarData.abc";

    {
        A5::WriteArchive w;
        AbcA::ArchiveWriterPtr a = w(archiveName, AbcA::MetaData());
        AbcA::ObjectWriterPtr archive = a->getTop();

        AbcA::CompoundPropertyWriterPtr parent = archive->getProperties();

        AbcA::ScalarPropertyWriterPtr swp =
            parent->createScalarProperty(AbcA::PropertyHeader("int32",
                AbcA::kScalarProperty, AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kInt32POD, 3),
                AbcA::TimeSamplingType(1.0)));

        std::vector <Alembic::Util::uint32_t> ui(3);
        ui[0] = 0;
        ui[1] = 1;
        ui[2] = 2;

        std::vector <Alembic::Util::uint32_t> ui2(3);
        ui2[0] = 41;
        ui2[1] = 43;
        ui2[2] = 47;

        swp->setSample(0, 0.0, &(ui.front()));
        swp->setSample(1, 1.0, &(ui.front()));
        swp->setSample(2, 2.0, &(ui2.front()));
        swp->setSample(3, 3.0, &(ui.front()));
        swp->setSample(4, 4.0, &(ui2.front()));
        swp->setSample(5, 5.0, &(ui2.front()));
        swp->setSample(6, 6.0, &(ui2.front()));
        swp->setSample(7, 7.0, &(ui.front()));
        swp->setSample(8, 8.0, &(ui.front()));
        swp->setSample(9, 9.0, &(ui.front()));

        // we already wrote this
        TESTING_ASSERT_THROW(swp->setSample(0, 0.0, &(ui2.front())),
            Alembic::Util::Exception);

        AbcA::ScalarPropertyWriterPtr swp2 =
            parent->createScalarProperty(AbcA::PropertyHeader("float32",
                AbcA::kScalarProperty, AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kFloat32POD, 1),
                AbcA::TimeSamplingType(1.0)));

        Alembic::Util::float32_t f = 42.0;
        Alembic::Util::float32_t f2 = -3.0;

        // have to write the indices in order starting from 0
        TESTING_ASSERT_THROW(swp2->setSample(7, 7.0, &f),
            Alembic::Util::Exception);

        swp2->setSample(0, 0.0, &f);

        // fails because we haven't written 1 yet
        TESTING_ASSERT_THROW(swp2->setSample(2, 2.0, &f),
            Alembic::Util::Exception);

        swp2->setSample(1, 1.0, &f);

        // for this sampling type, the time part should be ignored
        swp2->setSample(2, 7000.0, &f);
        swp2->setSample(3, 3.0, &f2);
        swp2->setSample(4, 4.0, &f2);
        swp2->setSample(5, 5.0, &f2);

        AbcA::ScalarPropertyWriterPtr swp3 =
            parent->createScalarProperty(AbcA::PropertyHeader("uint16",
                AbcA::kScalarProperty, AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kUint16POD, 1),
                AbcA::TimeSamplingType(2.0)));
        Alembic::Util::uint16_t ui16 = 17;

        swp3->setSample(0, 234.0, &ui16);
        swp3->setSample(1, 3.0, &ui16);
        swp3->setSample(2, -45.0, &ui16);
        swp3->setSample(3, 67.0, &ui16);

        AbcA::ScalarPropertyWriterPtr swp4 =
            parent->createScalarProperty(AbcA::PropertyHeader("str",
                AbcA::kScalarProperty, AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kStringPOD, 3),
                AbcA::TimeSamplingType(2.0)));

        std::vector < Alembic::Util::string > strVec(3);
        strVec[0] = "Please";
        strVec[1] = "";
        strVec[2] = "work";

        std::vector < Alembic::Util::string > strVec2(3);
        strVec2[0] = "Whats";
        strVec2[1] = "going";
        strVec2[2] = "on?";

        swp4->setSample(0, 234.0, &(strVec.front()));
        swp4->setSample(1, 234.0, &(strVec.front()));
        swp4->setSample(2, 234.0, &(strVec2.front()));
        swp4->setSample(3, 234.0, &(strVec2.front()));
    }

    {
        A5::ReadArchive r;
        AbcA::ArchiveReaderPtr a = r( archiveName );
        AbcA::ObjectReaderPtr archive = a->getTop();

        AbcA::CompoundPropertyReaderPtr parent = archive->getProperties();
        TESTING_ASSERT(parent->getNumProperties() == 4);
        for (size_t i = 0; i < parent->getNumProperties(); ++i)
        {
            AbcA::BasePropertyReaderPtr bp = parent->getProperty( i );
            AbcA::ScalarPropertyReaderPtr sp = bp->asScalarPtr();
            switch (sp->getDataType().getPod())
            {
                case Alembic::Util::kUint16POD:
                {
                    TESTING_ASSERT( sp->getNumSamples() == 1 );

                    const AbcA::TimeSampling t = sp->getTimeSampling();
                    TESTING_ASSERT( sp->isConstant() );

                    // this is how it used to work
                    //TESTING_ASSERT( sp->getNumSamples() == t->getNumSamples() );
                    //TESTING_ASSERT( t->getSampleTime(0) == 234.0 );

                    Alembic::Util::uint16_t us;

                    for ( size_t i = 0; i < sp->getNumSamples(); ++i )
                    {
                        us = 0;
                        sp->getSample( 0, &us);
                        TESTING_ASSERT(us == 17);
                    }
                }
                break;

                case Alembic::Util::kFloat32POD:
                {
                    TESTING_ASSERT( sp->getNumSamples() == 6 );
                    TESTING_ASSERT( sp->getDataType().getExtent() == 1);
                    TESTING_ASSERT( !sp->isConstant() );

                    Alembic::Util::float32_t f = 0;

                    // make sure we can't get a non-existant sample
                    TESTING_ASSERT_THROW(sp->getSample( 100, &f ),
                        Alembic::Util::Exception);

                    sp->getSample( 5, &f );
                    TESTING_ASSERT(f == -3.0);

                    sp->getSample( 1, &f );
                    TESTING_ASSERT(f == 42.0);

                    sp->getSample( 4, &f );
                    TESTING_ASSERT(f == -3.0);

                    sp->getSample( 0, &f );
                    TESTING_ASSERT(f == 42.0);

                    sp->getSample( 3, &f );
                    TESTING_ASSERT(f == -3.0);

                    sp->getSample( 2, &f );
                    TESTING_ASSERT(f == 42.0);
                }
                break;

                case Alembic::Util::kInt32POD:
                {
                    TESTING_ASSERT( sp->getNumSamples() == 10 );
                    TESTING_ASSERT( sp->getDataType().getExtent() == 3);
                    TESTING_ASSERT( !sp->isConstant() );

                    std::vector< Alembic::Util::uint32_t > ui(3);

                    // lets explicitly test each sample
                    sp->getSample( 0, &(ui.front()) );
                    TESTING_ASSERT(ui[0] == 0 && ui[1] == 1 && ui[2] == 2);

                    sp->getSample( 1, &(ui.front()) );
                    TESTING_ASSERT(ui[0] == 0 && ui[1] == 1 && ui[2] == 2);

                    sp->getSample( 2, &(ui.front()) );
                    TESTING_ASSERT(ui[0] == 41 && ui[1] == 43 && ui[2] == 47);

                    sp->getSample( 3, &(ui.front()) );
                    TESTING_ASSERT(ui[0] == 0 && ui[1] == 1 && ui[2] == 2);

                    sp->getSample( 4, &(ui.front()) );
                    TESTING_ASSERT(ui[0] == 41 && ui[1] == 43 && ui[2] == 47);

                    sp->getSample( 5, &(ui.front()) );
                    TESTING_ASSERT(ui[0] == 41 && ui[1] == 43 && ui[2] == 47);

                    sp->getSample( 6, &(ui.front()) );
                    TESTING_ASSERT(ui[0] == 41 && ui[1] == 43 && ui[2] == 47);

                    sp->getSample( 7, &(ui.front()) );
                    TESTING_ASSERT(ui[0] == 0 && ui[1] == 1 && ui[2] == 2);

                    sp->getSample( 8, &(ui.front()) );
                    TESTING_ASSERT(ui[0] == 0 && ui[1] == 1 && ui[2] == 2);

                    sp->getSample( 9, &(ui.front()) );
                    TESTING_ASSERT(ui[0] == 0 && ui[1] == 1 && ui[2] == 2);

                }
                break;

                case Alembic::Util::kStringPOD:
                {
                    TESTING_ASSERT( sp->getNumSamples() == 4 );
                    TESTING_ASSERT( sp->getDataType().getExtent() == 3);
                    TESTING_ASSERT( !sp->isConstant() );

                    std::vector< Alembic::Util::string > val(3);
                    sp->getSample(0, &(val.front()));
                    TESTING_ASSERT( val[0] == "Please");
                    TESTING_ASSERT( val[1] == "");
                    TESTING_ASSERT( val[2] == "work");

                    sp->getSample(1, &(val.front()));
                    TESTING_ASSERT( val[0] == "Please");
                    TESTING_ASSERT( val[1] == "");
                    TESTING_ASSERT( val[2] == "work");

                    sp->getSample(2, &(val.front()));
                    TESTING_ASSERT( val[0] == "Whats");
                    TESTING_ASSERT( val[1] == "going");
                    TESTING_ASSERT( val[2] == "on?");

                    sp->getSample(3, &(val.front()));
                    TESTING_ASSERT( val[0] == "Whats");
                    TESTING_ASSERT( val[1] == "going");
                    TESTING_ASSERT( val[2] == "on?");
                }
                break;

                default:
                    TESTING_ASSERT(false);
                break;
            }
        } // for
    }
}

int main ( int argc, char *argv[] )
{
    testWeirdStringScalar();
    testRepeatedScalarData();
    testReadWriteScalars();
    return 0;
}
