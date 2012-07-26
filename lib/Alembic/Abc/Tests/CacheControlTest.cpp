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
#include <Alembic/AbcCoreHDF5/All.h>
#include <iostream>

namespace Abc = Alembic::Abc;
using namespace Abc;

//-*****************************************************************************
// This test is mostly a compile test at this time - I want to demonstrate
// the different ways of expressing the cache to the IArchives
void cacheControlTest( const std::string &iArchiveBaseName )
{
    for ( size_t i = 0 ; i < 3 ; i++ )
    {
        std::string iArchiveName = iArchiveBaseName;
        char a = 'a' + i;
        iArchiveName += a ;
        iArchiveName.append( ".abc" );

        OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                          iArchiveName );

        // all child Objects in an Archive are actually children of the single
        // top Object in an Archive
        OObject topobj = archive.getTop();

        // 0th archive child
        OObject ac0( topobj, "ac0" );
        // 0th archive child child
        OObject acc0( ac0, "acc0" );
    }

    for ( size_t i = 0 ; i < 3 ; i++ )
    {
        std::string iArchiveName = iArchiveBaseName;
        char a = 'a' + i;
        iArchiveName += a ;
        iArchiveName.append( ".abc" );

        if ( i == 0 )
        {
            // Explicitly specify the implementation and the file name.
            // A cache is created automatically.
            IArchive archive1( Alembic::AbcCoreHDF5::ReadArchive(),
                               iArchiveName );
        }

        if ( i == 1 )
        {
            // Create an implementation with a NULL cache.
            IArchive archive2( Alembic::AbcCoreHDF5::ReadArchive(),
                               iArchiveName,
                               ErrorHandler::kThrowPolicy,
                               ReadArraySampleCachePtr() );
        }

        if ( i == 2 )
        {
            // Create an implementation with a cache from elsewhere
            ReadArraySampleCachePtr externalCache =
                Alembic::AbcCoreHDF5::CreateCache();
            IArchive archive3( Alembic::AbcCoreHDF5::ReadArchive(),
                               iArchiveName,
                               ErrorHandler::kThrowPolicy,
                               externalCache );
        }
    }
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    cacheControlTest( "oooooooh_ca-aache_controoo-ool_oooh_oh" );
    return 0;
}
