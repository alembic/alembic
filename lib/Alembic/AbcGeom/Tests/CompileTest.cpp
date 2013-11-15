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

#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcGeom/All.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

using namespace Alembic::Abc;
using namespace Alembic::AbcGeom;

// A bunch of minimal compile tests to make sure the templates compile


void testOGeomParam( Abc::OCompoundProperty &iParent )
{
    OV2fGeomParam uvs( iParent, "uv", false, kVertexScope, 1 );

    AbcA::MetaData md;
    SetIsUV( md, false );
    OV2fGeomParam notuvs( iParent, "notuv", false, kVertexScope, 1, md);
    OV2fGeomParam fakeuvs( iParent, "fakeuv", false, kConstantScope, 1);

    std::vector<V2f> vec;

    vec.push_back( V2f( 1.0f, 2.0f ) );

    V2fArraySample val( vec );

    OV2fGeomParam::Sample samp( val, kUnknownScope );

    uvs.set( samp );
    notuvs.set( samp );
    fakeuvs.set( samp );
}

void testIGeomParam( Abc::ICompoundProperty &iParent )
{
    IV2fGeomParam uvs( iParent, "uv" );
    IV2fGeomParam notuvs( iParent, "notuv" );
    IV2fGeomParam fakeuvs( iParent, "fakeuv" );

    // not really UVs since it's not of the correct scope
    TESTING_ASSERT( isUV( uvs.getHeader() ) );

    // not UVs since we specifically said it wasn't, even though it has the
    // correct scope
    TESTING_ASSERT( !isUV( notuvs.getHeader() ) );

    // not UVs because it is the incorrect scope
    TESTING_ASSERT( !isUV( fakeuvs.getHeader() ) );

    const AbcA::DataType &dt = uvs.getDataType();
    dt.getExtent();
}

int main( int argc, char *argv[] )
{

    std::string archiveName = "compile_test.abc";
    {
        OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                          archiveName, ErrorHandler::kNoisyNoopPolicy );
        OObject archiveTop = archive.getTop();
        OObject child( archiveTop, "otherChild" );
        OCompoundProperty topProp = child.getProperties();
        testOGeomParam( topProp );
    }
    {
        IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), archiveName,
            ErrorHandler::kNoisyNoopPolicy );
        IObject archiveTop = archive.getTop();
        IObject child( archiveTop, "otherChild" );
        ICompoundProperty topProp = child.getProperties();
        testIGeomParam( topProp );
    }

    return 0;
}
