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

namespace ABC = Alembic::AbcCoreAbstract::v1;

using ABC::chrono_t;
using Alembic::Util::float32_t;
using Alembic::Util::int32_t;
using Alembic::Util::byte_t;
using Alembic::Util::Dimensions;

//-*****************************************************************************

void testReadWriteEmptyArchive()
{
    std::string archiveName = "emptyArchive.abc";

    {
        A5::WriteArchive w;
        ABC::ArchiveWriterPtr a = w(archiveName, ABC::MetaData());
        ABC::ObjectWriterPtr archive = a->getTop();

        TESTING_ASSERT(archive->getFullName() == "/");

        // use this to get current reporting mechanism, then supress it
        // since we expect to see H5F errors because we will be trying to open
        // a file that is already open for writing.
        H5E_auto_t func;
        void * client_data;
        H5Eget_auto2(H5E_DEFAULT, &func, &client_data);
        H5Eset_auto2(H5E_DEFAULT, NULL, NULL);

        // can't write an already open archive
        TESTING_ASSERT_THROW(w(archiveName, ABC::MetaData()),
            Alembic::Util::Exception);

        Alembic::AbcCoreHDF5::ReadArchive r;

        // can't read an already open archive
        TESTING_ASSERT_THROW(r( archiveName ), Alembic::Util::Exception);

        // turn the error reporting back on for later tests
        H5Eset_auto2(H5E_DEFAULT, func, client_data);

    }

    {
        A5::ReadArchive r;
        ABC::ArchiveReaderPtr a = r( archiveName );
        ABC::ObjectReaderPtr archive = a->getTop();
        TESTING_ASSERT(archive->getName() == "ABC");
        TESTING_ASSERT(archive->getFullName() == "/");
        TESTING_ASSERT(archive->getParent() == NULL);
        TESTING_ASSERT(archive->getNumChildren() == 0);

        ABC::CompoundPropertyReaderPtr parent = archive->getProperties();
        TESTING_ASSERT(parent->getNumProperties() == 0);

    }
}

int main ( int argc, char *argv[] )
{
    testReadWriteEmptyArchive();
    return 0;
}
