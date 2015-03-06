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

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

using namespace Alembic::AbcGeom;

//-*****************************************************************************
void xformOut()
{
    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), "matrixXform.abc" );
    OXform a( OObject( archive, kTop ), "a" );


    M44d mat;
    mat.makeIdentity();

    for ( size_t i = 0; i < 20; ++i )
    {
        XformSample asamp;

        mat.x[0][0] = (double)i;

        mat.x[2][1] = 2.0 * i;

        asamp.setMatrix( mat );

        a.getSchema().set( asamp );
    }

}

//-*****************************************************************************
void xformIn()
{
    IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), "matrixXform.abc" );

    M44d mat;

    mat.makeIdentity();

    IXform a( IObject( archive, kTop ), "a" );
    TESTING_ASSERT( a.getSchema().getNumOps() == 1 );
    TESTING_ASSERT( a.getSchema().getInheritsXforms() );
    for ( index_t i = 0; i < 20; ++i )
    {
        XformSample xs;
        a.getSchema().get( xs, Abc::ISampleSelector( i ) );

        mat.x[0][0] = (double)i;
        mat.x[2][1] = 2.0 * i;

        TESTING_ASSERT( xs.getNumOps() == 1 );
        TESTING_ASSERT( xs[0].isMatrixOp() );

        TESTING_ASSERT( xs.getMatrix() == mat );
    }

}

void rotateTest()
{
    std::string fileName = "rotateXformOp.h";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName );
        OXform a( OObject( archive, kTop ), "a" );
        OXform b( OObject( archive, kTop ), "b" );

        XformSample aSamp;
        aSamp.addOp( XformOp(kRotateOperation), V3d(0.0, 1.0, 0.0), 45.0 );
        aSamp.addOp( XformOp(kRotateOperation), V3d(0.0, 0.0, 1.0), 30.0 );
        aSamp.addOp( XformOp(kRotateOperation), V3d(1.0, 0.0, 0.0), 15.0 );
        a.getSchema().set( aSamp );

        XformSample bSamp;
        bSamp.addOp( XformOp(kRotateYOperation), 45.0 );
        bSamp.addOp( XformOp(kRotateZOperation), 30.0 );
        bSamp.addOp( XformOp(kRotateXOperation), 15.0 );
        b.getSchema().set( bSamp );
    }

    {
        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), fileName );
        IXform a( IObject( archive, kTop ), "a" );
        IXform b( IObject( archive, kTop ), "b" );

        M44d aMat = a.getSchema().getValue().getMatrix();
        M44d bMat = b.getSchema().getValue().getMatrix();
        TESTING_ASSERT( aMat == bMat );
    }
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    xformOut();
    xformIn();

    rotateTest();

    return 0;
}
