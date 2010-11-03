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

using namespace Alembic::AbcGeom;

//-*****************************************************************************
void xformOut()
{
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                      "simpleXform1.abc" );    
    OSimpleXform general( OObject( archive, kTop ), "general" );
    OSimpleXform tsao( general, "tsao" );
    OSimpleXform chicken( tsao, "chicken" );
    OSimpleXform is( chicken, "is" );
    OSimpleXform delicious( is, "delicious" );
    OSimpleXform when( delicious, "when" );
    OSimpleXform prepared( when, "prepared" );
    OSimpleXform properly( prepared, "properly" );


    SimpleXformSample sample;

    sample.makeIdentity();
    sample.setTranslation( V3d( -2.0, -3.0, 1.5 ) );
    general.getSchema().set( sample );

    sample.makeIdentity();
    sample.setXYZRotation( V3d( -1.25, 0.0, .1 ) );
    tsao.getSchema().set( sample );

    sample.makeIdentity();
    sample.setScale( V3d( 0.9 ) );
    chicken.getSchema().set( sample );

    sample.makeIdentity();
    sample.setTranslation( V3d( -100.0 ) );
    is.getSchema().set( sample );

    sample.makeIdentity();
    sample.setXYZRotation( V3d( -5.0, 1.1, 2.3 ) );
    delicious.getSchema().set( sample );

    sample.makeIdentity();
    sample.setScale( V3d( 1.1, 1.2, 1.3 ) );
    when.getSchema().set( sample );

    sample.makeIdentity();
    sample.setTranslation( V3d( -1.0, -2.0, 1.7 ) );
    prepared.getSchema().set( sample );

    sample.makeIdentity();
    sample.setScale( V3d( 2.0, 3.0, 4.0 ) );
    properly.getSchema().set( sample );

    std::cout << "Out with: " << archive.getName() << std::endl;
}

//-*****************************************************************************
void xformIn()
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                      "simpleXform1.abc" );
    ISimpleXform general( IObject( archive, kTop ), "general" );
    ISimpleXform tsao( general, "tsao" );
    ISimpleXform chicken( tsao, "chicken" );
    ISimpleXform is( chicken, "is" );
    ISimpleXform delicious( is, "delicious" );
    ISimpleXform when( delicious, "when" );
    ISimpleXform prepared( when, "prepared" );
    ISimpleXform properly( prepared, "properly" );

    SimpleXformSample sample;
    
    general.getSchema().get( sample );
    std::cout << "General: " << sample << std::endl;

    tsao.getSchema().get( sample );
    std::cout << "Tsao: " << sample << std::endl;

    chicken.getSchema().get( sample );
    std::cout << "Chicken: " << sample << std::endl;

    is.getSchema().get( sample );
    std::cout << "Is: " << sample << std::endl;

    delicious.getSchema().get( sample );
    std::cout << "Delicious: " << sample << std::endl;

    when.getSchema().get( sample );
    std::cout << "When: " << sample << std::endl;

    prepared.getSchema().get( sample );
    std::cout << "Prepared: " << sample << std::endl;

    properly.getSchema().get( sample );
    std::cout << "Properly: " << sample << std::endl;
    
    std::cout << "Done with: " << archive.getName() << std::endl;
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    xformOut();
    xformIn();

    return 0;
}
