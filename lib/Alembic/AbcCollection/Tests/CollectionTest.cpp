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

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCollection/All.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>


namespace Abc =  Alembic::Abc;
namespace AbcCol = Alembic::AbcCollection;
namespace AbcA = Alembic::AbcCoreAbstract;

void write()
{
    Abc::OArchive archive(Alembic::AbcCoreHDF5::WriteArchive(), "Collection.abc" );

    Abc::OObject root(archive, Abc::kTop);
    Abc::OObject test(root, "test");

    AbcA::TimeSamplingPtr ts ( new AbcA::TimeSampling( 1/24.0, 2.0 ) );
    AbcA::MetaData md;
    md.set("coupe", "de ville");

    AbcCol::OCollections group(test, "Group1");
    AbcCol::OCollections group2(test, "Group2");

    std::vector< std::string > strVec;
    strVec.push_back("/a/b/c/1");
    strVec.push_back("/a/b/c/2");
    strVec.push_back("/a/b/c/3");

    Abc::OStringArrayProperty prop;

    // this should throw because the name of the collection is bad
    TESTING_ASSERT_THROW(prop = group2.getSchema().createCollection("/slashy"),
        Alembic::Util::Exception);

    TESTING_ASSERT(!prop.valid());

    prop = group.getSchema().createCollection("prop");
    prop.set(Abc::StringArraySample(strVec));

    TESTING_ASSERT(group.getSchema().getNumCollections() == 1);

    {
        group.getSchema().createCollection("cool", md, ts);
        TESTING_ASSERT(
            group.getSchema().getCollection(1).getMetaData().get("coupe") ==
            "de ville");

        strVec.clear();
        strVec.push_back("/foo");
        strVec.push_back("/bar");
        group.getSchema().getCollection(1).set(Abc::StringArraySample(strVec));
    }

    strVec.clear();
    strVec.push_back("potato");
    group.getSchema().getCollection("cool").set(Abc::StringArraySample(strVec));

}

void read()
{
    Abc::IArchive archive(Alembic::AbcCoreHDF5::ReadArchive(), "Collection.abc");
    Abc::IObject test(archive.getTop(), "test");
    AbcCol::ICollections group(test, "Group1");
    AbcCol::ICollections group2(test, "Group2");

    TESTING_ASSERT(group.getSchema().getNumCollections() == 2);
    TESTING_ASSERT(group2.getSchema().getNumCollections() == 0);

    TESTING_ASSERT(!group2.getSchema().getCollection(45));
    TESTING_ASSERT(!group2.getSchema().getCollection("potato"));

    Abc::IStringArrayProperty prop = group.getSchema().getCollection("prop");
    Abc::IStringArrayProperty prop2 = group.getSchema().getCollection("cool");
    TESTING_ASSERT(group.getSchema().getCollection(0).getName() == "cool" ||
        group.getSchema().getCollection(0).getName() == "prop");
    TESTING_ASSERT(group.getSchema().getCollection(1).getName() == "cool" ||
        group.getSchema().getCollection(1).getName() == "prop");
    TESTING_ASSERT(prop2.getMetaData().get("coupe") == "de ville");
    TESTING_ASSERT(archive.getTimeSampling(1) == prop2.getTimeSampling());
    TESTING_ASSERT(prop2.getNumSamples() == 2);
    Abc::StringArraySamplePtr samp = prop.getValue(0);
    TESTING_ASSERT(samp->size() == 3);
    TESTING_ASSERT((*samp)[0] == "/a/b/c/1");
    TESTING_ASSERT((*samp)[1] == "/a/b/c/2");
    TESTING_ASSERT((*samp)[2] == "/a/b/c/3");
}

int main(int argc, char *argv[])
{
    write();
    read();
    return 0;
}

