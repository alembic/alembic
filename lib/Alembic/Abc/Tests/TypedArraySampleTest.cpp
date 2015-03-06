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
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

#include <ImathMath.h>

#include <limits>

namespace Abc = Alembic::Abc;
using namespace Abc;


//-*****************************************************************************
void simpleTestOut( const std::string &iArchiveName )
{
    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(),
                      iArchiveName );
    OObject archiveTop( archive, kTop );

    OObject c0( archiveTop, "c0" );
    OCompoundProperty c0Props = c0.getProperties();


    OInt32ArrayProperty i32ap( c0Props, "i32ap" );

    Alembic::Util::shared_ptr<std::vector<int32_t> > vptr( new std::vector<int32_t>( 50, 4 ) );

    Int32ArraySample samp( *vptr );

    i32ap.set( samp );
}

//-*****************************************************************************
void simpleTestIn( const std::string &iArchiveName )
{
    IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(),
                      iArchiveName, ErrorHandler::kThrowPolicy );

    IObject archiveTop = archive.getTop();

    IObject c0( archiveTop, "c0" );
    ICompoundProperty c0Props = c0.getProperties();

    IInt32ArrayProperty i32ap( c0Props, "i32ap" );

    Int32ArraySamplePtr sptr = i32ap.getValue();

    for ( int32_t i = 0 ; i < 50 ; ++i )
    {
        TESTING_ASSERT( 4 == (*sptr)[i] );
        //std::cout << (*sptr)[i] << std::endl;
    }

}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    const std::string arkive( "typedArraySample.abc" );

    simpleTestOut( arkive );
    simpleTestIn( arkive );

    return 0;
}
