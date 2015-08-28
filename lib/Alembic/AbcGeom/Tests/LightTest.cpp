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

using namespace Alembic::AbcGeom; // Contains Abc, AbcCoreAbstract

//-*****************************************************************************
void lightTest()
{
    std::string fileName = "light1.abc";

    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName );
        OLight emptyLightObj( OObject( archive, kTop ), "emptyLight" );
        OLight lightObj( OObject( archive, kTop ), "myLight" );

        CameraSample samp;
        lightObj.getSchema().setCameraSample( samp );
        samp = CameraSample( -0.35, 0.75, 0.1, 0.5 );
        samp.setChildBounds( Abc::Box3d(
            Abc::V3d( 0.0, 0.1, 0.2), Abc::V3d( 0.3, 0.4, 0.5 ) ) );
        lightObj.getSchema().setCameraSample( samp );

        Abc::OCompoundProperty arb = lightObj.getSchema().getArbGeomParams();
        OFloatGeomParam param(arb, "test", false,
            Alembic::AbcGeom::kConstantScope, 1);
        Abc::OCompoundProperty user = lightObj.getSchema().getUserProperties();
        OFloatProperty(user, "test");
    }

    {
        CameraSample samp;
        double top, bottom, left, right;

        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), fileName );
        ILight emptyLightObj( IObject( archive, kTop ), "emptyLight" );
        ILight lightObj( IObject( archive, kTop ), "myLight" );

        TESTING_ASSERT( ! emptyLightObj.getSchema().getArbGeomParams() );
        TESTING_ASSERT( ! emptyLightObj.getSchema().getUserProperties() );
        TESTING_ASSERT( ! emptyLightObj.getSchema().getCameraSchema().valid() );
        TESTING_ASSERT(
            lightObj.getSchema().getArbGeomParams().getNumProperties() == 1 );
        TESTING_ASSERT(
            lightObj.getSchema().getUserProperties().getNumProperties() == 1 );

        lightObj.getSchema().getCameraSchema().get( samp, 0 );
        samp.getScreenWindow( top, bottom, left, right );
        TESTING_ASSERT( almostEqual( top, 0.666666666666667 ) );
        TESTING_ASSERT( almostEqual( bottom, -0.666666666666667 ) );
        TESTING_ASSERT( almostEqual( left, -1.0 ) );
        TESTING_ASSERT( almostEqual( right, 1.0 ) );

        lightObj.getSchema().getCameraSchema().get( samp, 1 );
        samp.getScreenWindow( top, bottom, left, right );
        TESTING_ASSERT( almostEqual( top, -0.35 ) );
        TESTING_ASSERT( almostEqual( bottom, 0.75 ) );
        TESTING_ASSERT( almostEqual( left, 0.1 ) );
        TESTING_ASSERT( almostEqual( right, 0.5 ) );

        TESTING_ASSERT(
            ! lightObj.getSchema().getCameraSchema().getChildBoundsProperty() );
    }
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    lightTest();
    return 0;
}
