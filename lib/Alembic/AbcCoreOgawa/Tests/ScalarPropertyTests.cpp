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

namespace AbcA = Alembic::AbcCoreAbstract;

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
        AO::WriteArchive w;
        AbcA::ArchiveWriterPtr a = w(archiveName, AbcA::MetaData());
        AbcA::ObjectWriterPtr archive = a->getTop();

        AbcA::CompoundPropertyWriterPtr props = archive->getProperties();

        {

            AbcA::ScalarPropertyWriterPtr emptyWrtPtr =
                props->createScalarProperty("empty", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kStringPOD, 1), 0);

            // this should fail because of the NULL char in the string
            TESTING_ASSERT_THROW(emptyWrtPtr->setSample(&weirdStr),
                Alembic::Util::Exception);

            emptyWrtPtr->setSample(&empty);

            AbcA::ScalarPropertyWriterPtr wemptyWrtPtr =
                props->createScalarProperty("wempty", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kWstringPOD, 1), 0);

            // this should fail because of the NULL char in the string
            TESTING_ASSERT_THROW(wemptyWrtPtr->setSample(&weirdWstr),
                                 Alembic::Util::Exception);

            wemptyWrtPtr->setSample(&wempty);


            AbcA::ScalarPropertyWriterPtr allEmptyWrtPtr =
                props->createScalarProperty("allEmpty", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kStringPOD, 3), 0);

            // one of the strings has a NULL char in it
            TESTING_ASSERT_THROW(
                allEmptyWrtPtr->setSample(&(weirdStrArray.front())),
                Alembic::Util::Exception);

            allEmptyWrtPtr->setSample(&(allEmptyStr.front()));

            AbcA::ScalarPropertyWriterPtr partEmptyStrPtr =
                props->createScalarProperty("partEmpty", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kStringPOD, 6), 0);

            partEmptyStrPtr->setSample(&(partEmptyStr.front()));
        }
    }

    {
        AO::ReadArchive r;
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
        AO::WriteArchive w;
        AbcA::ArchiveWriterPtr a = w(archiveName, AbcA::MetaData());
        AbcA::ObjectWriterPtr archive = a->getTop();

        AbcA::CompoundPropertyWriterPtr props = archive->getProperties();

        const AbcA::TimeSamplingType staticSampling;

        {
            AbcA::DataType dtype(Alembic::Util::kUint8POD, 1);
            TESTING_ASSERT_THROW(props->createScalarProperty("/slashy",
                AbcA::MetaData(), dtype, 0 ), Alembic::Util::Exception);
        }

        {
            AbcA::ScalarPropertyWriterPtr boolWrtPtr =
                props->createScalarProperty("bool",  AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kBooleanPOD, 1), 0);
            Alembic::Util::bool_t b = true;
            boolWrtPtr->setSample(&b);
        }


        {
            AbcA::ScalarPropertyWriterPtr ucharWrtPtr =
                props->createScalarProperty("uchar",  AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kUint8POD, 1), 0);

            Alembic::Util::uint8_t uc = 200;

            TESTING_ASSERT(ucharWrtPtr->getNumSamples() == 0);
            ucharWrtPtr->setSample(&uc);
            TESTING_ASSERT(ucharWrtPtr->getNumSamples() == 1);
        }

        {
            AbcA::ScalarPropertyWriterPtr charWrtPtr =
                props->createScalarProperty("char",  AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kInt8POD, 1), 0);

            Alembic::Util::int8_t c = -20;
            charWrtPtr->setSample(&c);
        }

        {
            AbcA::ScalarPropertyWriterPtr ushortWrtPtr =
                props->createScalarProperty("ushort", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kUint16POD, 1), 0);

            Alembic::Util::uint16_t us = 60000;
            ushortWrtPtr->setSample(&us);
        }

        {
            AbcA::ScalarPropertyWriterPtr shortWrtPtr =
                props->createScalarProperty("short", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kInt16POD, 1), 0);
            Alembic::Util::int16_t s = -20000;
            shortWrtPtr->setSample(&s);
        }

        {
            AbcA::ScalarPropertyWriterPtr uintWrtPtr =
                props->createScalarProperty("uint", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kUint32POD, 1), 0);
            Alembic::Util::uint32_t ui = 1000000;
            uintWrtPtr->setSample(&ui);
        }

        {
            AbcA::ScalarPropertyWriterPtr intWrtPtr =
                props->createScalarProperty("int", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kInt32POD, 1), 0);
            Alembic::Util::int32_t i = -1000000;
            intWrtPtr->setSample(&i);
        }

        {
            AbcA::ScalarPropertyWriterPtr ui64WrtPtr =
                props->createScalarProperty("uint64", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kUint64POD, 1), 0);
            Alembic::Util::uint64_t ui = 5000000000LL;
            ui64WrtPtr->setSample(&ui);
        }

        {
            AbcA::ScalarPropertyWriterPtr i64WrtPtr =
                props->createScalarProperty("i64", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kInt64POD, 1), 0);

            Alembic::Util::int64_t i = -5000000000LL;
            i64WrtPtr->setSample(&i);
        }

        {
            AbcA::ScalarPropertyWriterPtr halfWrtPtr =
                props->createScalarProperty("half", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kFloat16POD, 1), 0);

            Alembic::Util::float16_t h = 16.0;
            halfWrtPtr->setSample(&h);
        }

        {
            AbcA::ScalarPropertyWriterPtr floatWrtPtr =
                props->createScalarProperty("float", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kFloat32POD, 1), 0);

            Alembic::Util::float32_t f = 128.0;
            floatWrtPtr->setSample(&f);
        }

        {
            AbcA::ScalarPropertyWriterPtr doubleWrtPtr =
                props->createScalarProperty("double", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kFloat64POD, 1), 0);

            Alembic::Util::float64_t d = 32768.0;
            TESTING_ASSERT(doubleWrtPtr->getNumSamples() == 0);
            doubleWrtPtr->setSample(&d);
            TESTING_ASSERT(doubleWrtPtr->getNumSamples() == 1);
        }

        {
            AbcA::ScalarPropertyWriterPtr strWrtPtr =
                props->createScalarProperty("str", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kStringPOD, 1), 0);

            Alembic::Util::string c = "This was a triumph!";
            strWrtPtr->setSample(&c);
        }

        {
            AbcA::ScalarPropertyWriterPtr wstrWrtPtr =
                props->createScalarProperty("wstr", AbcA::MetaData(),
                    AbcA::DataType(Alembic::Util::kWstringPOD, 1), 0);

            Alembic::Util::wstring c( L"Matt Lauer can suck it! \u2697" );
            wstrWrtPtr->setSample(&c);
        }

    }

    // now we read what we've written
    {
        AO::ReadArchive r;
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
                    Alembic::Util::int64_t val = 0;
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
        AO::WriteArchive w;
        AbcA::ArchiveWriterPtr a = w(archiveName, AbcA::MetaData());
        AbcA::ObjectWriterPtr archive = a->getTop();

        AbcA::CompoundPropertyWriterPtr parent = archive->getProperties();

        AbcA::ScalarPropertyWriterPtr swp =
            parent->createScalarProperty("int32", AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kInt32POD, 3), 0);

        std::vector <Alembic::Util::uint32_t> ui(3);
        ui[0] = 0;
        ui[1] = 1;
        ui[2] = 2;

        std::vector <Alembic::Util::uint32_t> ui2(3);
        ui2[0] = 41;
        ui2[1] = 43;
        ui2[2] = 47;

        swp->setSample(&(ui.front()));
        swp->setSample(&(ui.front()));
        swp->setSample(&(ui2.front()));
        swp->setSample(&(ui.front()));
        swp->setSample(&(ui2.front()));
        swp->setSample(&(ui2.front()));
        swp->setSample(&(ui2.front()));
        swp->setSample(&(ui.front()));
        swp->setSample(&(ui.front()));
        swp->setSample(&(ui.front()));

        AbcA::ScalarPropertyWriterPtr swp2 =
            parent->createScalarProperty("float32", AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kFloat32POD, 1), 0);

        Alembic::Util::float32_t f = 42.0;
        Alembic::Util::float32_t f2 = -3.0;

        swp2->setSample(&f);
        swp2->setSample(&f);
        swp2->setSample(&f);
        swp2->setSample(&f2);
        swp2->setSample(&f2);
        swp2->setSample(&f2);

        AbcA::ScalarPropertyWriterPtr swp3 =
            parent->createScalarProperty("uint16", AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kUint16POD, 1), 0);

        Alembic::Util::uint16_t ui16 = 17;

        swp3->setSample(&ui16);
        swp3->setSample(&ui16);
        swp3->setSample(&ui16);
        swp3->setSample(&ui16);

        AbcA::ScalarPropertyWriterPtr swp4 =
            parent->createScalarProperty("str", AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kStringPOD, 3), 0);

        std::vector < Alembic::Util::string > strVec(3);
        strVec[0] = "Please";
        strVec[1] = "";
        strVec[2] = "work";

        std::vector < Alembic::Util::string > strVec2(3);
        strVec2[0] = "Whats";
        strVec2[1] = "going";
        strVec2[2] = "on?";

        swp4->setSample(&(strVec.front()));
        swp4->setSample(&(strVec.front()));
        swp4->setSample(&(strVec2.front()));
        swp4->setSample(&(strVec2.front()));
    }

    {
        AO::ReadArchive r;
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
                    TESTING_ASSERT( sp->getNumSamples() == 4 );

                    const AbcA::TimeSamplingPtr t = sp->getTimeSampling();
                    TESTING_ASSERT( sp->isConstant() );

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

AbcA::ScalarPropertyWriterPtr createObjectAndScalar(
    AbcA::ObjectWriterPtr iParent, const std::string & iName )
{
    AbcA::ObjectWriterPtr child =
        iParent->createChild(AbcA::ObjectHeader(iName, AbcA::MetaData()));

    AbcA::CompoundPropertyWriterPtr props = child->getProperties();

    AbcA::ScalarPropertyWriterPtr swp =
            props->createScalarProperty(iName, AbcA::MetaData(),
                AbcA::DataType(Alembic::Util::kInt32POD, 1), 0);
    return swp;
}

// tests a known issue where you can't write to scalar and array properties
// parented to the top compound if the OObject gets destroyed.
// http://code.google.com/p/alembic/issues/detail?id=171
void testPropScoping()
{
    AO::WriteArchive w;
    AbcA::ArchiveWriterPtr a = w("scopingTest.abc", AbcA::MetaData());
    AbcA::ObjectWriterPtr archive = a->getTop();
    AbcA::ScalarPropertyWriterPtr s = createObjectAndScalar(archive, "test");
    Alembic::Util::int32_t val = 42;
    s->setSample(&val);

}

void testScalarSamples()
{
    std::string archiveName = "numScalarSamplesTest.abc";

    AbcA::DataType dtype(Alembic::Util::kFloat32POD);
    float samp = 0.0f;
    {
        AO::WriteArchive w;
        AbcA::ArchiveWriterPtr a = w(archiveName, AbcA::MetaData());
        AbcA::ObjectWriterPtr archive = a->getTop();
        AbcA::ObjectWriterPtr obj = archive->createChild(
            AbcA::ObjectHeader("test", AbcA::MetaData()));

        AbcA::CompoundPropertyWriterPtr parent = obj->getProperties();
        AbcA::ScalarPropertyWriterPtr prop;

        AbcA::CompoundPropertyWriterPtr smallProp =
            parent->createCompoundProperty("small", AbcA::MetaData());
        smallProp->createScalarProperty("a", AbcA::MetaData(), dtype, 0);
        prop = smallProp->createScalarProperty("b", AbcA::MetaData(), dtype, 0);
        for (std::size_t i = 0; i < 10; ++i, ++samp)
        {
            prop->setSample(&samp);
        }
        smallProp->createArrayProperty("c", AbcA::MetaData(), dtype, 0);

        AbcA::CompoundPropertyWriterPtr mdProp =
            parent->createCompoundProperty("md", AbcA::MetaData());
        mdProp->createScalarProperty("a", AbcA::MetaData(), dtype, 0);
        prop = mdProp->createScalarProperty("b", AbcA::MetaData(), dtype, 0);
        for (std::size_t i = 0; i < 150; ++i, ++samp)
        {
            prop->setSample(&samp);
        }
        mdProp->createScalarProperty("c", AbcA::MetaData(), dtype, 0);

        AbcA::CompoundPropertyWriterPtr mdlgProp =
            parent->createCompoundProperty("mdlg", AbcA::MetaData());
        mdlgProp->createScalarProperty("a", AbcA::MetaData(), dtype, 0);
        prop = mdlgProp->createScalarProperty("b", AbcA::MetaData(), dtype, 0);
        for (std::size_t i = 0; i < 300; ++i, ++samp)
        {
            prop->setSample(&samp);
        }
        mdlgProp->createScalarProperty("c", AbcA::MetaData(), dtype, 0);

        AbcA::CompoundPropertyWriterPtr lgProp =
            parent->createCompoundProperty("lg", AbcA::MetaData());
        lgProp->createScalarProperty("a", AbcA::MetaData(), dtype, 0);
        prop = lgProp->createScalarProperty("b", AbcA::MetaData(), dtype, 0);
        for (std::size_t i = 0; i < 33000; ++i, ++samp)
        {
            prop->setSample(&samp);
        }
        lgProp->createScalarProperty("c", AbcA::MetaData(), dtype, 0);

        AbcA::CompoundPropertyWriterPtr insaneProp =
            parent->createCompoundProperty("insane", AbcA::MetaData());
        insaneProp->createScalarProperty("a", AbcA::MetaData(), dtype, 0);
        prop = insaneProp->createScalarProperty("b", AbcA::MetaData(), dtype, 0);
        for (std::size_t i = 0; i < 66000; ++i, ++samp)
        {
            prop->setSample(&samp);
        }
        insaneProp->createScalarProperty("c", AbcA::MetaData(), dtype, 0);
    }

    {
        AO::ReadArchive r;
        AbcA::ArchiveReaderPtr a = r( archiveName );
        AbcA::ObjectReaderPtr archive = a->getTop();
        AbcA::ObjectReaderPtr obj = archive->getChild(0);
        AbcA::CompoundPropertyReaderPtr parent = obj->getProperties();

        AbcA::CompoundPropertyReaderPtr smallProp =
            parent->getCompoundProperty("small");
        TESTING_ASSERT(smallProp->getNumProperties() == 3);
        TESTING_ASSERT(smallProp->getScalarProperty("b")->getNumSamples() == 10);

        AbcA::CompoundPropertyReaderPtr mdProp =
            parent->getCompoundProperty("md");
        TESTING_ASSERT(mdProp->getNumProperties() == 3);
        TESTING_ASSERT(mdProp->getScalarProperty("b")->getNumSamples() == 150);

        AbcA::CompoundPropertyReaderPtr mdlgProp =
            parent->getCompoundProperty("mdlg");
        TESTING_ASSERT(mdlgProp->getNumProperties() == 3);
        TESTING_ASSERT(mdlgProp->getScalarProperty("b")->getNumSamples() == 300);

        AbcA::CompoundPropertyReaderPtr lgProp =
            parent->getCompoundProperty("lg");
        TESTING_ASSERT(lgProp->getNumProperties() == 3);
        TESTING_ASSERT(lgProp->getScalarProperty("b")->getNumSamples()
                       == 33000);

        AbcA::CompoundPropertyReaderPtr insaneProp =
            parent->getCompoundProperty("insane");
        TESTING_ASSERT(insaneProp->getNumProperties() == 3);
        TESTING_ASSERT(insaneProp->getScalarProperty("b")->getNumSamples()
                       == 66000);
    }
}

int main ( int argc, char *argv[] )
{
    testWeirdStringScalar();
    testRepeatedScalarData();
    testReadWriteScalars();
    testPropScoping();
    testScalarSamples();
    return 0;
}
