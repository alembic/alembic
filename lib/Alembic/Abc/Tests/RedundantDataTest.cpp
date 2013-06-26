//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <set>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

namespace Abc = Alembic::Abc;
using namespace Abc;

namespace AbcF = Alembic::AbcCoreFactory;

//-*****************************************************************************
static std::vector<Alembic::Util::int32_t> intArraySamp;
static std::set<std::string> PATHS;

static const Alembic::Util::int32_t NUM_TOP_CHILDREN = 100;
static const Alembic::Util::int32_t DEPTH = 499;

static const Alembic::Util::int32_t HIGHVAL = 200;

typedef std::pair< std::set<std::string>::iterator, bool > PATH_PAIR;

//-*****************************************************************************
void makeIntArraySamp()
{
    for ( Alembic::Util::int32_t i = 0 ; i < HIGHVAL ; i++ )
    {
        intArraySamp.push_back( i );
    }
}

//-*****************************************************************************
void makeDeepHierarchy( OObject parent, const int level )
{
    if ( level > DEPTH )
    {
        OCompoundProperty p = parent.getProperties();
        OInt32ArrayProperty iap( p, "intArrayProp", 0 );
        iap.set( intArraySamp );
        return;
    }

    std::ostringstream strm;
    strm << level;
    std::string levelName = strm.str();
    makeDeepHierarchy( OObject( parent, levelName ), level + 1 );
}

//-*****************************************************************************
void readDeepHierarchy( IObject parent, const int level, const IObject& orig )
{
    if ( level > DEPTH )
    {
        ICompoundProperty p = parent.getProperties();
        IInt32ArrayProperty iap( p, "intArrayProp" );
        std::string fullName = const_cast<std::string&>(
            iap.getObject().getFullName() );

        PATH_PAIR ret = PATHS.insert( fullName );

        Int32ArraySamplePtr sampPtr = iap.getValue();

        TESTING_ASSERT( sampPtr->get()[5] == 5 );
        TESTING_ASSERT( sampPtr->get()[99] == 99 );
        TESTING_ASSERT( sampPtr->size() == ( size_t ) HIGHVAL );

        TESTING_ASSERT( ret.second );

        TESTING_ASSERT( fullName == parent.getFullName() );

        // walk back up the tree until you find the first child object
        // under the top object, and check that it's the one we started
        // with.
        IObject _p = parent;
        while ( _p.getParent().getParent() )
        {
            _p = _p.getParent();
        }

        TESTING_ASSERT( _p.getFullName() == orig.getFullName() );

        return;
    }

    std::ostringstream strm;
    strm << level;
    std::string levelName = strm.str();
    readDeepHierarchy( IObject( parent, levelName ), level + 1, orig );
}

//-*****************************************************************************
void simpleTestOut( const std::string &iArchiveName, bool useOgawa )
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
    OObject archiveTop( archive, kTop );

    // create 100 top-level children
    for ( int i = 0 ; i < NUM_TOP_CHILDREN ; i++ )
    {
        std::ostringstream strm;
        strm << i;
        std::string name = strm.str();
        OObject obj( archiveTop, name );
        makeDeepHierarchy( obj, 0 );
    }
}

//-*****************************************************************************
void simpleTestIn( const std::string &iArchiveName )
{
    AbcF::IFactory factory;
    factory.setPolicy(  ErrorHandler::kThrowPolicy );
    AbcF::IFactory::CoreType coreType;
    IArchive archive = factory.getArchive(iArchiveName, coreType);

    IObject archiveTop = archive.getTop();

    TESTING_ASSERT( archiveTop.getNumChildren() == ( size_t )NUM_TOP_CHILDREN );

    for ( int i = 0 ; i < NUM_TOP_CHILDREN ; i++ )
    {
        std::ostringstream strm;
        strm << i;
        std::string cname = strm.str();
        IObject obj( archiveTop, cname );
        readDeepHierarchy( obj, 0, obj  );
    }

    TESTING_ASSERT( PATHS.size() == ( size_t ) NUM_TOP_CHILDREN );
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    const std::string arkive( "redundantDataTest.abc" );

    // make the int array sample values
    makeIntArraySamp();

    bool useOgawa = true;
    simpleTestOut( arkive, useOgawa );
    simpleTestIn( arkive );

    PATHS.clear();

    useOgawa = false;
    simpleTestOut( arkive, useOgawa );
    simpleTestIn( arkive );

    return 0;
}
