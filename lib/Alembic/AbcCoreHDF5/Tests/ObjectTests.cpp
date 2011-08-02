//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <Alembic/AbcCoreHDF5/Tests/Assert.h>

//-*****************************************************************************
namespace A5 = Alembic::AbcCoreHDF5;

namespace AbcA = Alembic::AbcCoreAbstract::v1;

//-*****************************************************************************
void testObjects()
{
    std::string archiveName = "objectTest.abc";
    {
        A5::WriteArchive w;
        AbcA::ArchiveWriterPtr a = w(archiveName, AbcA::MetaData());
        AbcA::ObjectWriterPtr archive = a->getTop();
        AbcA::ObjectWriterPtr child1 = archive->createChild(
            AbcA::ObjectHeader("wow", AbcA::MetaData()));
        AbcA::ObjectWriterPtr child2 = archive->createChild(
            AbcA::ObjectHeader("bar", AbcA::MetaData()));
        AbcA::ObjectWriterPtr child3 = archive->createChild(
            AbcA::ObjectHeader("foo", AbcA::MetaData()));
        TESTING_ASSERT(archive->getNumChildren() == 3);

        TESTING_ASSERT(child1->getNumChildren() == 0);
        child1->createChild(
            AbcA::ObjectHeader("food", AbcA::MetaData()));
        TESTING_ASSERT(child1->getNumChildren() == 1);

        child2->createChild(
            AbcA::ObjectHeader("hat", AbcA::MetaData()));
        TESTING_ASSERT_THROW(child2->createChild(
            AbcA::ObjectHeader("hat", AbcA::MetaData())),
            Alembic::Util::Exception);
        child2->createChild(
            AbcA::ObjectHeader("bowling", AbcA::MetaData()));
        TESTING_ASSERT(child2->getNumChildren() == 2);

        child3->createChild(
            AbcA::ObjectHeader("hamburger", AbcA::MetaData()));
        child3->createChild(
            AbcA::ObjectHeader("burrito", AbcA::MetaData()));
        child3->createChild(
            AbcA::ObjectHeader("pizza", AbcA::MetaData()));
        TESTING_ASSERT(child3->getNumChildren() == 3);
    }

    {
        A5::ReadArchive r;
        AbcA::ArchiveReaderPtr a = r( archiveName );
        AbcA::ObjectReaderPtr archive = a->getTop();
        TESTING_ASSERT(archive->getNumChildren() == 3);

        AbcA::ObjectReaderPtr child = archive->getChild(0);
        TESTING_ASSERT(child->getName() == "wow");
        TESTING_ASSERT(child->getNumChildren() == 1);
        AbcA::ObjectReaderPtr gchild = child->getChild(0);
        TESTING_ASSERT(gchild->getNumChildren() == 0);
        TESTING_ASSERT(gchild->getName() == "food");

        child = archive->getChild(1);
        TESTING_ASSERT(child->getName() == "bar");
        TESTING_ASSERT(child->getNumChildren() == 2);
        gchild = child->getChild(0);
        TESTING_ASSERT(gchild->getNumChildren() == 0);
        TESTING_ASSERT(gchild->getName() == "hat");
        gchild = child->getChild(1);
        TESTING_ASSERT(gchild->getNumChildren() == 0);
        TESTING_ASSERT(gchild->getName() == "bowling");

        child = archive->getChild(2);
        TESTING_ASSERT(child->getName() == "foo");
        TESTING_ASSERT(child->getNumChildren() == 3);
        gchild = child->getChild(0);
        TESTING_ASSERT(gchild->getNumChildren() == 0);
        TESTING_ASSERT(gchild->getName() == "hamburger");
        gchild = child->getChild(1);
        TESTING_ASSERT(gchild->getNumChildren() == 0);
        TESTING_ASSERT(gchild->getName() == "burrito");
        gchild = child->getChild(2);
        TESTING_ASSERT(gchild->getNumChildren() == 0);
        TESTING_ASSERT(gchild->getName() == "pizza");

        A5::ReadArchive r2;
        AbcA::ArchiveReaderPtr a2 = r2( archiveName );
        AbcA::ObjectReaderPtr archive2 = a2->getTop();
        AbcA::ObjectReaderPtr child2 = archive->getChild(0);
        AbcA::ObjectReaderPtr gchild2 = child2->getChild(0);
        TESTING_ASSERT(gchild2->getNumChildren() == 0);
        TESTING_ASSERT(gchild2->getName() == "food");
    }
}

int main ( int argc, char *argv[] )
{
    testObjects();
    return 0;
}
