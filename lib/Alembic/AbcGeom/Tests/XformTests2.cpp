//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/Abc/Tests/Assert.h>

#include <ImathMath.h>

#include <limits>

static const double VAL_EPSILON = std::numeric_limits<double>::epsilon() \
    * 1024.0;

bool almostEqual( const double &a, const double &b,
                  const double &epsilon = VAL_EPSILON )
{
    return Imath::equalWithAbsError( a, b, epsilon );
}

using namespace Alembic::AbcGeom;

//-*****************************************************************************
void xformOut()
{
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(), "matrixXform.abc" );
    OXform a( OObject( archive, kTop ), "a" );


    M44d mat;
    mat.makeIdentity();

    for ( size_t i = 0; i < 20; ++i )
    {
        XformSample asamp;

        mat.x[0][0] = (double)i;

        mat.x[2][1] = 2.0 * i;

        asamp.setMatrix( mat );

        a.getSchema().set( asamp, OSampleSelector( i ) );
    }

}

//-*****************************************************************************
void xformIn()
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), "matrixXform.abc" );

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


//-*****************************************************************************
int main( int argc, char *argv[] )
{
    xformOut();
    xformIn();

    return 0;
}
