//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/Abc/All.h>
#include "Assert.h"

namespace Abc = Alembic::Abc;
using namespace Abc;

void archiveInfoTest()
{
    std::string appWriter = "Alembic unit tests";
    std::string userStr = "abcdefg";
    {
        Alembic::AbcCoreAbstract::MetaData md;
        md.set("potato", "salad");
        md.set("taco", "bar");
        OArchive archive = CreateArchiveWithInfo(
            Alembic::AbcCoreHDF5::WriteArchive(), "archiveInfo.abc",
            appWriter, userStr, md );

        TESTING_ASSERT( archive.getPtr()->getMetaData().get("taco") == "bar" );
    }

    {
        IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
            "archiveInfo.abc" );
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
    }
}

int main( int argc, char *argv[] )
{
    archiveInfoTest();
    return 0;
}
