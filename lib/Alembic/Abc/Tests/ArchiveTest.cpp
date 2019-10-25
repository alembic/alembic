//-*****************************************************************************
//
// Copyright (c) 2009-2015,
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

#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

#ifdef ALEMBIC_WITH_HDF5
#include <Alembic/AbcCoreHDF5/All.h>
#endif

namespace Abc = Alembic::Abc;
namespace AbcF = Alembic::AbcCoreFactory;
using namespace Abc;

void archiveInfoTest(bool useOgawa)
{
    std::string appWriter = "Alembic unit tests";
    std::string userStr = "abcdefg";
    {
        Alembic::AbcCoreAbstract::MetaData md;
        md.set("potato", "salad");
        md.set("taco", "bar");
        OArchive archive, archive2;
        if (useOgawa)
        {
            archive = CreateArchiveWithInfo(
                Alembic::AbcCoreOgawa::WriteArchive(), "archiveInfo.abc",
                appWriter, userStr, md );

            archive2 = CreateArchiveWithInfo(
                Alembic::AbcCoreOgawa::WriteArchive(), "archiveInfoWFPS.abc",
                42, appWriter, userStr, md );
        }
#ifdef ALEMBIC_WITH_HDF5
        else
        {
            archive = CreateArchiveWithInfo(
                Alembic::AbcCoreHDF5::WriteArchive(), "archiveInfo.abc",
                appWriter, userStr, md );

            archive2 = CreateArchiveWithInfo(
                Alembic::AbcCoreHDF5::WriteArchive(), "archiveInfoWFPS.abc",
                42, appWriter, userStr, md );
        }
#endif

        TESTING_ASSERT( archive.getPtr()->getMetaData().get("taco") == "bar" );
        TESTING_ASSERT( archive2.getPtr()->getMetaData().get("taco") == "bar" );
        //TESTING_ASSERT( archive2.getPtr()->getMetaData().get( kDCCFPSKey )
        //    == "42" );
    }

    {
        AbcF::IFactory factory;
        AbcF::IFactory::CoreType coreType;
        IArchive archive = factory.getArchive("archiveInfo.abc", coreType);
        TESTING_ASSERT( (useOgawa && coreType == AbcF::IFactory::kOgawa) ||
                        (!useOgawa && coreType == AbcF::IFactory::kHDF5) );

        TESTING_ASSERT( archive.getPtr()->getMetaData().get("taco") == "bar" );
        TESTING_ASSERT( archive.getPtr()->getMetaData().get("potato") ==
            "salad" );
        TESTING_ASSERT( archive.getArchiveVersion() ==
                        ALEMBIC_LIBRARY_VERSION );

        std::string appInfo;
        std::string abcVersionStr;
        Alembic::Util::uint32_t abcVersion = 0;
        std::string dateWritten;
        std::string userInfo;
        GetArchiveInfo( archive, appInfo, abcVersionStr, abcVersion,
            dateWritten, userInfo );
        TESTING_ASSERT( appWriter ==  appInfo );
        TESTING_ASSERT( userStr ==  userInfo );
        TESTING_ASSERT( abcVersion ==  ALEMBIC_LIBRARY_VERSION );
        std::cout << "Alembic version: " << abcVersionStr << std::endl;
        std::cout << "Date written: " << dateWritten << std::endl;
        TESTING_ASSERT( dateWritten != "" );
        TESTING_ASSERT( abcVersionStr != "" );

        // test it again but the DCC FPS variant, pass in a bogus value
        // to show that it gets reset to 0 when it doesn't exist
        double dcc_fps = 123;
        GetArchiveInfo( archive, appInfo, abcVersionStr, abcVersion,
            dateWritten, userInfo, dcc_fps);
        TESTING_ASSERT( appWriter ==  appInfo );
        TESTING_ASSERT( userStr ==  userInfo );
        TESTING_ASSERT( abcVersion ==  ALEMBIC_LIBRARY_VERSION );
        std::cout << "Alembic version: " << abcVersionStr << std::endl;
        std::cout << "Date written: " << dateWritten << std::endl;
        TESTING_ASSERT( dateWritten != "" );
        TESTING_ASSERT( abcVersionStr != "" );
        TESTING_ASSERT( dcc_fps == 0 );

        double start, end;
        GetArchiveStartAndEndTime( archive, start, end );
        TESTING_ASSERT( start == DBL_MAX && end == -DBL_MAX );
    }

    {
        AbcF::IFactory factory;
        AbcF::IFactory::CoreType coreType;
        IArchive archive = factory.getArchive("archiveInfoWFPS.abc", coreType);
        TESTING_ASSERT( (useOgawa && coreType == AbcF::IFactory::kOgawa) ||
                        (!useOgawa && coreType == AbcF::IFactory::kHDF5) );

        TESTING_ASSERT( archive.getPtr()->getMetaData().get("taco") == "bar" );
        TESTING_ASSERT( archive.getPtr()->getMetaData().get("potato") ==
            "salad" );
        TESTING_ASSERT( archive.getArchiveVersion() ==
                        ALEMBIC_LIBRARY_VERSION );

        std::string appInfo;
        std::string abcVersionStr;
        Alembic::Util::uint32_t abcVersion = 0;
        double dcc_fps = 123.0;
        std::string dateWritten;
        std::string userInfo;
        GetArchiveInfo( archive, appInfo, abcVersionStr, abcVersion,
            dateWritten, userInfo, dcc_fps );
        TESTING_ASSERT( appWriter ==  appInfo );
        TESTING_ASSERT( userStr ==  userInfo );
        TESTING_ASSERT( abcVersion ==  ALEMBIC_LIBRARY_VERSION );
        std::cout << "Alembic version: " << abcVersionStr << std::endl;
        std::cout << "Date written: " << dateWritten << std::endl;
        TESTING_ASSERT( dateWritten != "" );
        TESTING_ASSERT( abcVersionStr != "" );
        TESTING_ASSERT( dcc_fps == 42.0 );

        double start, end;
        GetArchiveStartAndEndTime( archive, start, end );
        TESTING_ASSERT( start == DBL_MAX && end == -DBL_MAX );
    }
}

void scopingTest(bool useOgawa)
{
    {
        OObject top;
        {
            OArchive archive;
            if (useOgawa)
            {
                archive = CreateArchiveWithInfo(
                    Alembic::AbcCoreOgawa::WriteArchive(),
                    "archiveScopeTest.abc",
                    "Alembic test", "", MetaData() );
            }
#ifdef ALEMBIC_WITH_HDF5
            else
            {
                archive = CreateArchiveWithInfo(
                    Alembic::AbcCoreHDF5::WriteArchive(),
                    "archiveScopeTest.abc",
                    "Alembic test", "", MetaData() );
            }
#endif

            top = archive.getTop();
        }
        OObject childA( top, "a");
        OObject childB( top, "b");
        ODoubleProperty prop(top.getProperties(), "prop", 0);
        TESTING_ASSERT(prop.getObject().getArchive().getName() ==
            "archiveScopeTest.abc");
    }

    {
        IObject top;
        {
            AbcF::IFactory factory;
            AbcF::IFactory::CoreType coreType;
            IArchive archive = factory.getArchive("archiveScopeTest.abc",
                                                  coreType);

           TESTING_ASSERT( (useOgawa && coreType == AbcF::IFactory::kOgawa) ||
                           (!useOgawa && coreType == AbcF::IFactory::kHDF5) );

            top = archive.getTop();

            double start, end;
            GetArchiveStartAndEndTime( archive, start, end );
            TESTING_ASSERT( start == DBL_MAX && end == -DBL_MAX );
        }
        TESTING_ASSERT(top.getNumChildren() == 2 );
        TESTING_ASSERT(top.getChildHeader("a") != NULL);
        TESTING_ASSERT(top.getChildHeader("b") != NULL);
        TESTING_ASSERT( ! top.getParent().valid() );
        TESTING_ASSERT( top.getArchive().getName() ==
            "archiveScopeTest.abc");
        IScalarProperty prop(top.getProperties(), "prop");
        TESTING_ASSERT(prop.valid());
        TESTING_ASSERT(prop.getObject().getArchive().getName() ==
            "archiveScopeTest.abc");
    }
}

int main( int argc, char *argv[] )
{
    archiveInfoTest(true);
    scopingTest(true);

#ifdef ALEMBIC_WITH_HDF5
    archiveInfoTest(false);
    scopingTest(false);
#endif

    return 0;
}
