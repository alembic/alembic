//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

namespace Abc = Alembic::Abc;
using namespace Abc;

namespace AbcF = Alembic::AbcCoreFactory;

using Alembic::AbcCoreAbstract::chrono_t;
using Alembic::AbcCoreAbstract::index_t;
using Alembic::Util::uint32_t;
using Alembic::Util::float32_t;

//-*****************************************************************************
void simpleTestOut( const std::string& iArchiveName, bool useOgawa )
{
    OArchive archive;
    if (useOgawa)
    {
        archive = OArchive( Alembic::AbcCoreOgawa::WriteArchive(),
            iArchiveName, ErrorHandler::kThrowPolicy );
    }
    else
    {
        archive = OArchive( Alembic::AbcCoreHDF5::WriteArchive(),
            iArchiveName, ErrorHandler::kThrowPolicy );
    }

    /*
               x1
           /   |   \
         x2    x3   x2a (x2a is an instance targeting x2)
          |    |
         x4    x5    (x5 is an instance targeting x4)
        / |
      g1  g2
          |
          g5
    */

    // all child Objects in an Archive are actually children of the single
    // top Object in an Archive
    OObject topobj = archive.getTop();

    OObject x1( topobj, "x1" );
    OObject x2( x1, "x2" );
    OObject x3( x1, "x3" );

    OObject x4( x2, "x4" );
    OObject g1( x4, "g1" );
    OObject g2( x4, "g2" );

    OObject g5( g2, "g5" );

    // x5 is an instance targeting x4
    TESTING_ASSERT( x3.addChildInstance( x4, "x5" ) );

    // proxies can't point at ancestors (would create a cycle)
    TESTING_ASSERT( !x2.addChildInstance( x2, "x" ) );
    TESTING_ASSERT( !x2.addChildInstance( x1, "x" ) );

    // x2a is an instance targeting x2
    TESTING_ASSERT( x1.addChildInstance( x2, "x2a" ) );

    const Alembic::AbcCoreAbstract::ObjectHeader& x5h = x3.getChildHeader(0);
    TESTING_ASSERT( x5h.getFullName() == std::string("/x1/x3/x5") );

    const Alembic::AbcCoreAbstract::MetaData& md = x5h.getMetaData();
    TESTING_ASSERT( md.get("isInstance") == std::string("1") );

    const Alembic::AbcCoreAbstract::ObjectHeader& x2ah = x1.getChildHeader(2);
    TESTING_ASSERT( x2ah.getFullName() == std::string("/x1/x2a") );

    const Alembic::AbcCoreAbstract::MetaData& md2 = x2ah.getMetaData();
    TESTING_ASSERT( md2.get("isInstance") == std::string("1") );
}

//-*****************************************************************************
void simpleTestIn( const std::string& iArchiveName )
{
    AbcF::IFactory factory;
    factory.setPolicy( ErrorHandler::kThrowPolicy );

    AbcF::IFactory::CoreType coreType;
    IArchive archive = factory.getArchive( iArchiveName, coreType );

    /*
               x1
           /   |   \
         x2    x3   x2a (x2a is an instance targeting x2)
          |    |
         x4    x5    (x5 is an instance targeting x4)
        / |
      g1  g2
          |
          g5
    */

    // an archive has a single top object which contains all its children
    IObject topObject = archive.getTop();

    IObject x1( topObject, "x1" );
    TESTING_ASSERT( x1 != 0 );

    //
    // Verify the target path
    IObject x2( x1, "x2" );
    TESTING_ASSERT( x2.valid() );
    TESTING_ASSERT( !x2.isInstanceDescendant() );

    IObject x4( x2, "x4" );
    TESTING_ASSERT( x4.valid() );
    TESTING_ASSERT( !x4.isInstanceDescendant() );

    int numChildren = x4.getNumChildren();
    TESTING_ASSERT( numChildren == 2 );
    TESTING_ASSERT( x4.getParent().getFullName() == x2.getFullName() );

    IObject g1( x4.getChild(0) );
    TESTING_ASSERT( g1 != 0 );
    TESTING_ASSERT( g1.getName() == "g1" );
    TESTING_ASSERT( !g1.isInstanceDescendant() );
    TESTING_ASSERT( g1.getParent() != 0 );
    TESTING_ASSERT( g1.getParent().getFullName() == x4.getFullName() );

    IObject g2( x4.getChild(1) );
    TESTING_ASSERT( g2 != 0 );
    TESTING_ASSERT( g2.getName() == "g2" );
    TESTING_ASSERT( !g2.isInstanceDescendant() );
    TESTING_ASSERT( g2.getParent() != 0 );
    TESTING_ASSERT( g2.getParent().getFullName() == x4.getFullName() );

    IObject g5( g2.getChild(0) );
    TESTING_ASSERT( g5 != 0 );
    TESTING_ASSERT( g5.getName() == "g5" );
    TESTING_ASSERT( !g5.isInstanceDescendant() );
    TESTING_ASSERT( g5.getParent() != 0 );
    TESTING_ASSERT( g5.getParent().getFullName() == g2.getFullName() );

    //
    // Verify the instance path
    IObject x3( x1, "x3" );
    TESTING_ASSERT( x3 != 0 );

    IObject x5( x3, "x5" );
    TESTING_ASSERT( x5 != 0 );

    TESTING_ASSERT( x5.isInstanceDescendant() );
    TESTING_ASSERT( x5.isInstanceRoot() );
    TESTING_ASSERT( x5.instanceSourcePath() == x4.getFullName() );

    numChildren = x5.getNumChildren();
    TESTING_ASSERT( numChildren == 2 );
    TESTING_ASSERT( x5.getParent().getFullName() == x3.getFullName() );

    IObject g1p( x5.getChild(0) );
    TESTING_ASSERT( g1p != 0 );
    TESTING_ASSERT( g1p.getName() == "g1" );
    TESTING_ASSERT( g1p.isInstanceDescendant() );
    TESTING_ASSERT( !g1p.isInstanceRoot() );
    TESTING_ASSERT( g1p.getParent() != 0 );
    TESTING_ASSERT( g1p.getParent().getFullName() == x5.getFullName() );

    IObject g2p( x5.getChild(1) );
    TESTING_ASSERT( g2p != 0 );
    TESTING_ASSERT( g2p.getName() == "g2" );
    TESTING_ASSERT( g2p.isInstanceDescendant() );
    TESTING_ASSERT( !g2p.isInstanceRoot() );
    TESTING_ASSERT( g2p.getParent() != 0 );
    TESTING_ASSERT( g2p.getParent().getFullName() == x5.getFullName() );

    IObject g5p( g2p.getChild(0) );
    TESTING_ASSERT( g5p != 0 );
    TESTING_ASSERT( g5p.getName() == "g5" );
    TESTING_ASSERT( g5p.isInstanceDescendant() );
    TESTING_ASSERT( !g5p.isInstanceRoot() );
    TESTING_ASSERT( g5p.getParent() != 0 );
    TESTING_ASSERT( g5p.getParent().getFullName() == g2p.getFullName() );

    // test x2a
    IObject x2a( x1, "x2a" );
    TESTING_ASSERT( x2a.valid() );
    TESTING_ASSERT( x2a.isInstanceDescendant() );
    TESTING_ASSERT( x2a.isInstanceRoot() );
    TESTING_ASSERT( x2a.instanceSourcePath() == x2.getFullName() );
    TESTING_ASSERT( x2a.getNumChildren() == 1 );

    IObject x2aParent = x2a.getParent();
    TESTING_ASSERT( x2aParent.getFullName() == "/x1" );
    TESTING_ASSERT( !x2aParent.isInstanceDescendant() );
}

//-*****************************************************************************
void diabolicalInstance( const std::string& iArchiveName, bool useOgawa )
{
    /*
               a0  b0 (points to a0)
              /  |
            a1   b1 (points to a1)
           /  |
          a2  b2 (points to b2)
    */

{
    OArchive archive;
    if (useOgawa)
    {
        archive = OArchive( Alembic::AbcCoreOgawa::WriteArchive(),
            iArchiveName, ErrorHandler::kThrowPolicy );
    }
    else
    {
        archive = OArchive( Alembic::AbcCoreHDF5::WriteArchive(),
            iArchiveName, ErrorHandler::kThrowPolicy );
    }

    OObject topobj = archive.getTop();

    OObject a0( topobj, "a0" );
    TESTING_ASSERT( topobj.addChildInstance( a0, "b0" ) );

    OObject a1( a0, "a1" );
    TESTING_ASSERT( a0.addChildInstance( a1, "b1" ) );

    OObject a2( a1, "a2" );
    TESTING_ASSERT( a1.addChildInstance( a2, "b2" ) );
}

{
    AbcF::IFactory factory;

    IArchive archive = factory.getArchive( iArchiveName );
    IObject topObject = archive.getTop();

    IObject a0( topObject.getChild(0) );
    TESTING_ASSERT( !a0.isInstanceDescendant() );
    TESTING_ASSERT( a0.getFullName() == "/a0" );
    TESTING_ASSERT( !a0.getParent().isInstanceDescendant() );
    TESTING_ASSERT( a0.getParent().getFullName() == "/" );

    IObject b0( topObject.getChild(1) );
    TESTING_ASSERT( b0.isInstanceDescendant() );
    TESTING_ASSERT( b0.getName() == "b0" );
    TESTING_ASSERT( b0.getFullName() == "/b0" );
    TESTING_ASSERT( !b0.getParent().isInstanceDescendant() );
    TESTING_ASSERT( b0.getParent().getFullName() == "/" );

    IObject a0a1( a0.getChild(0) );
    TESTING_ASSERT( !a0a1.isInstanceDescendant() );
    TESTING_ASSERT( a0a1.getName() == "a1" );
    TESTING_ASSERT( a0a1.getFullName() == "/a0/a1" );
    TESTING_ASSERT( !a0a1.getParent().isInstanceDescendant() );
    TESTING_ASSERT( a0a1.getParent().getName() == "a0" );
    TESTING_ASSERT( a0a1.getParent().getFullName() == "/a0" );

    IObject a0b1( a0.getChild(1) );
    TESTING_ASSERT( a0b1.isInstanceDescendant() );
    TESTING_ASSERT( a0b1.getName() == "b1" );
    TESTING_ASSERT( a0b1.getFullName() == "/a0/b1" );
    TESTING_ASSERT( !a0b1.getParent().isInstanceDescendant() );
    TESTING_ASSERT( a0b1.getParent().getName() == "a0" );
    TESTING_ASSERT( a0b1.getParent().getFullName() == "/a0" );

    IObject b0a1( b0.getChild(0) );
    TESTING_ASSERT( b0a1.isInstanceDescendant() );
    TESTING_ASSERT( b0a1.getParent().isInstanceDescendant() );

    IObject b0b1( b0.getChild(1) );
    TESTING_ASSERT( b0b1.isInstanceDescendant() );
    TESTING_ASSERT( b0b1.getName() == "b1" );
    TESTING_ASSERT( b0b1.getFullName() == "/b0/b1" );
    TESTING_ASSERT( b0b1.getParent().isInstanceDescendant() );
    TESTING_ASSERT( b0b1.getParent().getName() == "b0" );
    TESTING_ASSERT( b0b1.getParent().getFullName() == "/b0" );

    IObject a0a1a2( a0a1.getChild(0) );
    TESTING_ASSERT( !a0a1a2.isInstanceDescendant() );
    TESTING_ASSERT( a0a1a2.getName() == "a2" );
    TESTING_ASSERT( a0a1a2.getFullName() == "/a0/a1/a2" );
    TESTING_ASSERT( !a0a1a2.getParent().isInstanceDescendant() );
    TESTING_ASSERT( a0a1a2.getParent().getName() == "a1" );
    TESTING_ASSERT( a0a1a2.getParent().getFullName() == "/a0/a1" );
    TESTING_ASSERT( !a0a1a2.getParent().getParent().isInstanceDescendant() );
    TESTING_ASSERT( a0a1a2.getParent().getParent().getName() == "a0" );
    TESTING_ASSERT( a0a1a2.getParent().getParent().getFullName() == "/a0" );

    IObject a0a1b2( a0a1.getChild(1) );
    TESTING_ASSERT( a0a1b2.isInstanceDescendant() );
    TESTING_ASSERT( a0a1b2.getName() == "b2" );
    TESTING_ASSERT( a0a1b2.getFullName() == "/a0/a1/b2" );
    TESTING_ASSERT( !a0a1b2.getParent().isInstanceDescendant() );
    TESTING_ASSERT( a0a1b2.getParent().getName() == "a1" );
    TESTING_ASSERT( a0a1b2.getParent().getFullName() == "/a0/a1" );
    TESTING_ASSERT( !a0a1b2.getParent().getParent().isInstanceDescendant() );
    TESTING_ASSERT( a0a1b2.getParent().getParent().getName() == "a0" );
    TESTING_ASSERT( a0a1b2.getParent().getParent().getFullName() == "/a0" );

    IObject a0b1a2( a0b1.getChild(0) );
    TESTING_ASSERT( a0b1a2.isInstanceDescendant() );
    TESTING_ASSERT( a0b1a2.getName() == "a2" );
    TESTING_ASSERT( a0b1a2.getFullName() == "/a0/b1/a2" );
    TESTING_ASSERT( a0b1a2.getParent().isInstanceDescendant() );
    TESTING_ASSERT( a0b1a2.getParent().getName() == "b1" );
    TESTING_ASSERT( a0b1a2.getParent().getFullName() == "/a0/b1" );
    TESTING_ASSERT( !a0b1a2.getParent().getParent().isInstanceDescendant() );
    TESTING_ASSERT( a0b1a2.getParent().getParent().getName() == "a0" );
    TESTING_ASSERT( a0b1a2.getParent().getParent().getFullName() == "/a0" );

    IObject a0b1b2( a0b1.getChild(1) );
    TESTING_ASSERT( a0b1b2.isInstanceDescendant() );
    TESTING_ASSERT( a0b1b2.getName() == "b2" );
    TESTING_ASSERT( a0b1b2.getFullName() == "/a0/b1/b2" );
    TESTING_ASSERT( a0b1b2.getParent().isInstanceDescendant() );
    TESTING_ASSERT( a0b1b2.getParent().getName() == "b1" );
    TESTING_ASSERT( a0b1b2.getParent().getFullName() == "/a0/b1" );
    TESTING_ASSERT( !a0b1b2.getParent().getParent().isInstanceDescendant() );
    TESTING_ASSERT( a0b1b2.getParent().getParent().getName() == "a0" );
    TESTING_ASSERT( a0b1b2.getParent().getParent().getFullName() == "/a0" );

    IObject b0a1a2( b0a1.getChild(0) );
    TESTING_ASSERT( b0a1a2.isInstanceDescendant() );
    TESTING_ASSERT( b0a1a2.getName() == "a2" );
    TESTING_ASSERT( b0a1a2.getFullName() == "/b0/a1/a2" );
    TESTING_ASSERT( b0a1a2.getParent().isInstanceDescendant() );
    TESTING_ASSERT( b0a1a2.getParent().getName() == "a1" );
    TESTING_ASSERT( b0a1a2.getParent().getFullName() == "/b0/a1" );
    TESTING_ASSERT( b0a1a2.getParent().getParent().isInstanceDescendant() );
    TESTING_ASSERT( b0a1a2.getParent().getParent().getName() == "b0" );
    TESTING_ASSERT( b0a1a2.getParent().getParent().getFullName() == "/b0" );

    IObject b0a1b2( b0a1.getChild(1) );
    TESTING_ASSERT( b0a1b2.isInstanceDescendant() );
    TESTING_ASSERT( b0a1b2.getName() == "b2" );
    TESTING_ASSERT( b0a1b2.getFullName() == "/b0/a1/b2" );
    TESTING_ASSERT( b0a1b2.getParent().isInstanceDescendant() );
    TESTING_ASSERT( b0a1b2.getParent().getName() == "a1" );
    TESTING_ASSERT( b0a1b2.getParent().getFullName() == "/b0/a1" );
    TESTING_ASSERT( b0a1b2.getParent().getParent().isInstanceDescendant() );
    TESTING_ASSERT( b0a1b2.getParent().getParent().getName() == "b0" );
    TESTING_ASSERT( b0a1b2.getParent().getParent().getFullName() == "/b0" );

    IObject b0b1a2( b0b1.getChild(0) );
    TESTING_ASSERT( b0b1a2.isInstanceDescendant() );
    TESTING_ASSERT( b0b1a2.getName() == "a2" );
    TESTING_ASSERT( b0b1a2.getFullName() == "/b0/b1/a2" );
    TESTING_ASSERT( b0b1a2.getParent().isInstanceDescendant() );
    TESTING_ASSERT( b0b1a2.getParent().getName() == "b1" );
    TESTING_ASSERT( b0b1a2.getParent().getFullName() == "/b0/b1" );
    TESTING_ASSERT( b0b1a2.getParent().getParent().isInstanceDescendant() );
    TESTING_ASSERT( b0b1a2.getParent().getParent().getName() == "b0" );
    TESTING_ASSERT( b0b1a2.getParent().getParent().getFullName() == "/b0" );

    IObject b0b1b2( b0b1.getChild(1) );
    TESTING_ASSERT( b0b1b2.isInstanceDescendant() );
    TESTING_ASSERT( b0b1b2.getName() == "b2" );
    TESTING_ASSERT( b0b1b2.getFullName() == "/b0/b1/b2" );
    TESTING_ASSERT( b0b1b2.getParent().isInstanceDescendant() );
    TESTING_ASSERT( b0b1b2.getParent().getName() == "b1" );
    TESTING_ASSERT( b0b1b2.getParent().getFullName() == "/b0/b1" );
    TESTING_ASSERT( b0b1b2.getParent().getParent().isInstanceDescendant() );
    TESTING_ASSERT( b0b1b2.getParent().getParent().getName() == "b0" );
    TESTING_ASSERT( b0b1b2.getParent().getParent().getFullName() == "/b0" );
}

}

//-*****************************************************************************
int main( int argc, char* argv[] )
{
    const std::string oarkhive( "instancetest_ogawa.abc" );
    const std::string harkhive( "insatncetest_hdf5.abc" );

    const std::string oarkhive2( "diabolical.instancet_ogawa.abc" );
    const std::string harkhive2( "diabolical.instancet_hdf5.abc" );

    bool useOgawa = true;
    simpleTestOut( oarkhive, useOgawa );
    simpleTestIn( oarkhive );
    diabolicalInstance( oarkhive2, useOgawa );

    useOgawa = false;
    simpleTestOut( harkhive, useOgawa );
    simpleTestIn( harkhive );
    diabolicalInstance( harkhive2, useOgawa );

    return 0;
}
