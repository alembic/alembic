//-*****************************************************************************
//
// Copyright (c) 2016,
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

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreLayer/Read.h>
#include <Alembic/AbcCoreLayer/Util.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreFactory/All.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

using namespace Alembic::Abc;

//-*****************************************************************************
void layerTest()
{
    std::string fileName = "objectLayer1.abc";
    std::string fileName2 = "objectLayer2.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName );
        OObject child( archive.getTop(), "child" );
        OObject childCool( child, "cool" );
        OObject childGuy( child, "guy" );

        OObject childA( archive.getTop(), "childA" );
        OObject childAA( childA, "A" );
    }

    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName2 );
        OObject child( archive.getTop(), "child" );
        OObject childCool( child, "cool" );
        OObject childGal( child, "gal" );

        OObject childA( archive.getTop(), "childB" );
        OObject childAA( childA, "B" );
    }

    {
        std::vector< std::string > files;
        files.push_back( fileName );
        files.push_back( fileName2 );

        Alembic::AbcCoreFactory::IFactory factory;
        IArchive archive = factory.getArchive( files );

        // child, childA, childB
        TESTING_ASSERT( archive.getTop().getNumChildren() == 3 );

        IObject child = archive.getTop().getChild("child");
        TESTING_ASSERT( child.getNumChildren() == 3 );
        TESTING_ASSERT( child.getChild("cool").valid() );
        TESTING_ASSERT( child.getChild("cool").getNumChildren() == 0 );
        TESTING_ASSERT( child.getChild("guy").valid() );
        TESTING_ASSERT( child.getChild("guy").getNumChildren() == 0 );
        TESTING_ASSERT( child.getChild("gal").valid() );
        TESTING_ASSERT( child.getChild("gal").getNumChildren() == 0 );

        IObject childA = archive.getTop().getChild("childA");
        TESTING_ASSERT( childA.getNumChildren() == 1 );
        TESTING_ASSERT( childA.getChild("A").valid() );
        TESTING_ASSERT( childA.getChild("A").getNumChildren() == 0 );

        IObject childB = archive.getTop().getChild("childB");
        TESTING_ASSERT( childB.getNumChildren() == 1 );
        TESTING_ASSERT( childB.getChild("B").valid() );
        TESTING_ASSERT( childB.getChild("B").getNumChildren() == 0 );
    }
}

//-*****************************************************************************
void pruneTest()
{
    std::string fileName = "objectPrune1.abc";
    std::string fileName2 = "objectPrune2.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName );
        OObject child( archive.getTop(), "child" );
        OObject childCool( child, "cool" );
        OObject childGuy( child, "guy" );

        OObject childA( archive.getTop(), "childA" );
        OObject childAA( childA, "A" );

        OObject childB( archive.getTop(), "childB" );
        OObject childBB( childB, "B" );
    }

    {
        MetaData md;
        Alembic::AbcCoreLayer::SetPrune( md, true );

        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName2 );
        OObject child( archive.getTop(), "child" );
        OObject childGuy( child, "guy", md );

        OObject childA( archive.getTop(), "childA" );
        OObject childAA( childA, "A", md );
        OObject childAB( childA, "B", md );

        OObject childB( archive.getTop(), "childB", md );
    }

    {
        std::vector< std::string > files;
        files.push_back( fileName );
        files.push_back( fileName2 );

        Alembic::AbcCoreFactory::IFactory factory;
        IArchive archive = factory.getArchive( files );

        // child, childA, childB
        TESTING_ASSERT( archive.getTop().getNumChildren() == 2 );

        IObject child = archive.getTop().getChild("child");
        TESTING_ASSERT( child.getNumChildren() == 1 );
        TESTING_ASSERT( child.getChild("cool").valid() );
        TESTING_ASSERT( child.getChild("cool").getNumChildren() == 0 );

        IObject childA = archive.getTop().getChild("childA");
        TESTING_ASSERT( childA.getNumChildren() == 0 );
    }
}

//-*****************************************************************************
void replaceTest()
{
    std::string fileName = "objectReplace1.abc";
    std::string fileName2 = "objectReplace2.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName );
        OObject child( archive.getTop(), "child" );

        OObject childCool( child, "cool" );
        OObject childCooler( childCool, "cooler" );

        OObject childGuy( child, "guy" );
        OObject childGuyA( childGuy, "A" );

        OObject childA( archive.getTop(), "childA" );
        OObject childAA( childA, "A" );

        OObject childB( archive.getTop(), "childB" );
        OObject childBB( childB, "B" );
    }

    {
        MetaData md;
        Alembic::AbcCoreLayer::SetReplace( md, true );

        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName2 );
        OObject child( archive.getTop(), "child" );
        OObject childGuy( child, "guy", md );

        OObject childCool( child, "cool", md );
        OObject childCoolA( childCool, "A" );
        OObject childCoolB( childCool, "B", md );

        OObject childA( archive.getTop(), "childA", md );

        OObject childB( archive.getTop(), "childB", md );
        OObject childBA( childB, "A", md );
        OObject childBC( childB, "C", md );
        OObject childBCA( childBC, "A", md );
    }

    {
        std::vector< std::string > files;
        files.push_back( fileName );
        files.push_back( fileName2 );

        Alembic::AbcCoreFactory::IFactory factory;
        IArchive archive = factory.getArchive( files );

        IObject root = archive.getTop();

        // child, childA, childB
        TESTING_ASSERT( root.getNumChildren() == 3 );

        IObject child( root, "child" );
        TESTING_ASSERT( child.getNumChildren() == 2 );
        TESTING_ASSERT( IObject( child, "cool" ).valid() );
        TESTING_ASSERT( IObject( child, "guy" ).valid() );
        TESTING_ASSERT( IObject( child, "cool" ).getNumChildren() == 2 );
        TESTING_ASSERT( IObject( child, "guy" ).getNumChildren() == 0 );

        IObject childCool( child, "cool" );
        TESTING_ASSERT( IObject( childCool, "A" ).valid() );
        TESTING_ASSERT( IObject( childCool, "A" ).getNumChildren() == 0 );
        TESTING_ASSERT( IObject( childCool, "B" ).valid() );
        TESTING_ASSERT( IObject( childCool, "B" ).getNumChildren() == 0 );

        IObject childA( root, "childA" );
        TESTING_ASSERT( childA.getNumChildren() == 0 );

        IObject childB( root, "childB" );
        TESTING_ASSERT( childB.getNumChildren() == 2 );
        TESTING_ASSERT( IObject( childB, "A" ).valid() );
        TESTING_ASSERT( IObject( childB, "C" ).valid() );
        TESTING_ASSERT( IObject( childB, "A" ).getNumChildren() == 0 );
        TESTING_ASSERT( IObject( childB, "C" ).getNumChildren() == 1 );

        IObject childBC( childB, "C" );
        TESTING_ASSERT( IObject( childBC, "A" ).valid() );
        TESTING_ASSERT( IObject( childBC, "A" ).getNumChildren() == 0 );
    }
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    layerTest();
    pruneTest();
    replaceTest();
    return 0;
}
