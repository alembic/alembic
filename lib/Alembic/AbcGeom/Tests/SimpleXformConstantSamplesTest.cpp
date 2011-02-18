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

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include "Assert.h"

#include <iostream>

using namespace Alembic::AbcGeom;

V3d g_trans( 2.0, 4.0, 8.0 );

//-*****************************************************************************
void writeArchive( const std::string &iArchive )
{
    OArchive out( Alembic::AbcCoreHDF5::WriteArchive(),
                  iArchive );

    TimeSamplingType tst( 1.0 / 96.0 );

    OSimpleXform foo( out.getTop(), "foo", tst );

    SimpleXformSample s;
    s.makeIdentity();

    // set the same thing ten times
    for ( size_t i = 0 ; i < 10 ; ++i )
    {
        s.setTranslation( g_trans );
        foo.getSchema().set( s, i );
        s.makeIdentity();
    }
}
//-*****************************************************************************
void readArchive( const std::string &iArchive )
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), iArchive );

    IObject top = archive.getTop();

    ISimpleXform x( archive.getTop(), "foo" );

    std::cout << x.getName() << " has " << x.getSchema().getNumSamples()
              << " samples." << std::endl;

    TESTING_ASSERT( x.getSchema().getNumSamples() == 1 );

    TESTING_ASSERT( x.getSchema().getValue( 0 ).getTranslation() == g_trans );
}

//-*****************************************************************************
int main(int argc, char **argv)
{
    const std::string &iArchive( "constantSimpleXformSamples.abc" );
    writeArchive( iArchive );
    readArchive( iArchive );

    return 0;
}
