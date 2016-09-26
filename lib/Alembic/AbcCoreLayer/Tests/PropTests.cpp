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
    std::string fileName = "propLayer1.abc";
    std::string fileName2 = "propLayer2.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName );
        OCompoundProperty child( archive.getTop().getProperties(), "child" );
        OCompoundProperty childCool( child, "cool" );
        OCompoundProperty childGuy( child, "guy" );

        OCompoundProperty childA( archive.getTop().getProperties(), "childA" );
        OCompoundProperty childAA( childA, "A" );
    }

    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName2 );
        OCompoundProperty child( archive.getTop().getProperties(), "child" );
        OCompoundProperty childCool( child, "cool" );
        OCompoundProperty childGal( child, "gal" );

        OCompoundProperty childA( archive.getTop().getProperties(), "childB" );
        OCompoundProperty childAA( childA, "B" );
    }

    {
        std::vector< std::string > files;
        files.push_back( fileName );
        files.push_back( fileName2 );

        Alembic::AbcCoreFactory::IFactory factory;
        IArchive archive = factory.getArchive( files );

        // child, childA, childB
        ICompoundProperty root = archive.getTop().getProperties();
        TESTING_ASSERT( root.getNumProperties() == 3 );

        ICompoundProperty child( root, "child" );
        TESTING_ASSERT( child.getNumProperties() == 3 );

        TESTING_ASSERT( ICompoundProperty( child, "cool" ).valid() );
        TESTING_ASSERT(
            ICompoundProperty( child, "cool" ).getNumProperties() == 0 );

        TESTING_ASSERT( ICompoundProperty( child, "guy" ).valid() );
        TESTING_ASSERT(
            ICompoundProperty( child, "guy" ).getNumProperties() == 0 );

        ICompoundProperty childA( root, "childA" );
        TESTING_ASSERT( childA.getNumProperties() == 1 );
        TESTING_ASSERT( ICompoundProperty( childA, "A" ).valid() );
        TESTING_ASSERT(
            ICompoundProperty( childA, "A" ).getNumProperties() == 0 );

        ICompoundProperty childB( root, "childB" );
        TESTING_ASSERT( childB.getNumProperties() == 1 );
        TESTING_ASSERT( ICompoundProperty( childB, "B" ).valid() );
        TESTING_ASSERT(
            ICompoundProperty( childB, "B" ).getNumProperties() == 0 );
    }
}

//-*****************************************************************************
void pruneTest()
{
    std::string fileName = "propPrune1.abc";
    std::string fileName2 = "propPrune2.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName );
        OCompoundProperty child( archive.getTop().getProperties(), "child" );
        OCompoundProperty childCool( child, "cool" );
        OCompoundProperty childGuy( child, "guy" );

        OCompoundProperty childA( archive.getTop().getProperties(), "childA" );
        OCompoundProperty childAA( childA, "A" );

        OCompoundProperty childB( archive.getTop().getProperties(), "childB" );
        OCompoundProperty childBB( childB, "B" );
    }

    {
        MetaData md;
        Alembic::AbcCoreLayer::SetPrune( md, true );

        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName2 );
        OCompoundProperty child( archive.getTop().getProperties(), "child" );
        OCompoundProperty childGuy( child, "guy", md );

        OCompoundProperty childA( archive.getTop().getProperties(), "childA" );
        OCompoundProperty childAA( childA, "A", md );
        OCompoundProperty childAB( childA, "B", md );

        OCompoundProperty childB( archive.getTop().getProperties(),
                                  "childB", md );
    }

    {
        std::vector< std::string > files;
        files.push_back( fileName );
        files.push_back( fileName2 );

        Alembic::AbcCoreFactory::IFactory factory;
        IArchive archive = factory.getArchive( files );

        ICompoundProperty root = archive.getTop().getProperties();

        // child, childA, childB
        TESTING_ASSERT( root.getNumProperties() == 2 );

        ICompoundProperty child( root, "child" );
        TESTING_ASSERT( child.getNumProperties() == 1 );
        TESTING_ASSERT( ICompoundProperty( child, "cool" ).valid() );
        TESTING_ASSERT(
            ICompoundProperty( child, "cool" ).getNumProperties() == 0 );

        ICompoundProperty childA( root, "childA" );
        TESTING_ASSERT( childA.getNumProperties() == 0 );
    }
}

//-*****************************************************************************
void replaceTest()
{
    std::string fileName = "propReplace1.abc";
    std::string fileName2 = "propReplace2.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName );
        OCompoundProperty child( archive.getTop().getProperties(), "child" );

        OCompoundProperty childCool( child, "cool" );
        OCompoundProperty childCooler( childCool, "cooler" );

        OCompoundProperty childGuy( child, "guy" );
        OCompoundProperty childGuyA( childGuy, "A" );

        OCompoundProperty childA( archive.getTop().getProperties(), "childA" );
        OCompoundProperty childAA( childA, "A" );

        OCompoundProperty childB( archive.getTop().getProperties(), "childB" );
        OCompoundProperty childBB( childB, "B" );
    }

    {
        MetaData md;
        Alembic::AbcCoreLayer::SetReplace( md, true );

        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName2 );
        OCompoundProperty child( archive.getTop().getProperties(), "child" );
        OCompoundProperty childGuy( child, "guy", md );

        OCompoundProperty childCool( child, "cool", md );
        OCompoundProperty childCoolA( childCool, "A" );
        OCompoundProperty childCoolB( childCool, "B", md );

        OCompoundProperty childA( archive.getTop().getProperties(), "childA",
                                  md );

        OCompoundProperty childB( archive.getTop().getProperties(),
                                  "childB", md );
        OCompoundProperty childBA( childB, "A", md );
        OCompoundProperty childBC( childB, "C", md );
        OCompoundProperty childBCA( childBC, "A", md );
    }

    {
        std::vector< std::string > files;
        files.push_back( fileName );
        files.push_back( fileName2 );

        Alembic::AbcCoreFactory::IFactory factory;
        IArchive archive = factory.getArchive( files );

        ICompoundProperty root = archive.getTop().getProperties();

        // child, childA, childB
        TESTING_ASSERT( root.getNumProperties() == 3 );

        ICompoundProperty child( root, "child" );
        TESTING_ASSERT( child.getNumProperties() == 2 );
        TESTING_ASSERT( ICompoundProperty( child, "cool" ).valid() );
        TESTING_ASSERT( ICompoundProperty( child, "guy" ).valid() );
        TESTING_ASSERT(
            ICompoundProperty( child, "cool" ).getNumProperties() == 2 );
        TESTING_ASSERT(
            ICompoundProperty( child, "guy" ).getNumProperties() == 0 );

        ICompoundProperty childCool( child, "cool" );
        TESTING_ASSERT( ICompoundProperty( childCool, "A" ).valid() );
        TESTING_ASSERT(
            ICompoundProperty( childCool, "A" ).getNumProperties() == 0 );
        TESTING_ASSERT( ICompoundProperty( childCool, "B" ).valid() );
        TESTING_ASSERT(
            ICompoundProperty( childCool, "B" ).getNumProperties() == 0 );

        ICompoundProperty childA( root, "childA" );
        TESTING_ASSERT( childA.getNumProperties() == 0 );

        ICompoundProperty childB( root, "childB" );
        TESTING_ASSERT( childB.getNumProperties() == 2 );
        TESTING_ASSERT( ICompoundProperty( childB, "A" ).valid() );
        TESTING_ASSERT( ICompoundProperty( childB, "C" ).valid() );
        TESTING_ASSERT(
            ICompoundProperty( childB, "A" ).getNumProperties() == 0 );
        TESTING_ASSERT(
            ICompoundProperty( childB, "C" ).getNumProperties() == 1 );

        ICompoundProperty childBC( childB, "C" );
        TESTING_ASSERT( ICompoundProperty( childBC, "A" ).valid() );
        TESTING_ASSERT(
            ICompoundProperty( childBC, "A" ).getNumProperties() == 0 );
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
