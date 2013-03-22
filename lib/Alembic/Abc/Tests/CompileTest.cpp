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

#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/Abc/All.h>

using namespace Alembic::Abc;

// A bunch of minimal compile tests to make sure the templates compile

// Declare a test schema.
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "TestSchema_v1", "", ".test",
                                   TestSchemaInfo );

typedef OSchema<TestSchemaInfo> OTestSchema;

typedef OSchemaObject<OTestSchema> OTest;

typedef ISchema<TestSchemaInfo> ITestSchema;

typedef ISchemaObject<ITestSchema> ITest;

void testOSchemaObject( OObject &iParent )
{
    OTest tst( iParent, "childTestObject" );
    OTest tst2( tst.getPtr(), "foo",
                ErrorHandler::kQuietNoopPolicy );
    OTest tst3( tst2.getPtr(), "bar",
                kNoMatching );
    OTest tst4( tst3, "baz",
                ErrorHandler::kNoisyNoopPolicy,
                kStrictMatching );

    OTestSchema& schm = tst4.getSchema();
    schm.valid();
}

void testOSchema( OObject & iParent )
{
    OTestSchema tst( OCompoundProperty( iParent, kTop ) );
    OTestSchema tst2( tst.getPtr(), "foo",
                      ErrorHandler::kQuietNoopPolicy );
    OTestSchema tst3( tst2.getPtr(), "bar",
                      kNoMatching );
    OTestSchema tst4( tst3, "baz",
                      ErrorHandler::kNoisyNoopPolicy,
                      kStrictMatching );
}

void testISchemaObject( IObject & iParent )
{
    ITest tst( iParent, "childTestObject" );
    ITest tst2( tst.getPtr(), kWrapExisting,
                ErrorHandler::kQuietNoopPolicy );
    ITest tst3( tst2.getPtr(), kWrapExisting,
                      kNoMatching );
    ITest tst4( tst3, kWrapExisting,
                ErrorHandler::kNoisyNoopPolicy,
                kStrictMatching );

    ITestSchema& schm = tst4.getSchema();
    schm.valid();
}

void testISchema( IObject &iParent )
{
    ITestSchema tst( ICompoundProperty( iParent, kTop ) );
    ITestSchema tst2( tst.getPtr(), kWrapExisting,
                      ErrorHandler::kQuietNoopPolicy );
    ITestSchema tst3( tst2.getPtr(), kWrapExisting,
                      kNoMatching );
    ITestSchema tst4( tst3, kWrapExisting,
                      ErrorHandler::kNoisyNoopPolicy,
                      kStrictMatching );
}

void testOTypedScalarProperty( OObject &iObject )
{
    OBoolProperty boolProp( OCompoundProperty( iObject, kTop ),
                            "boolPropScalar" );
    OInt32Property intProp( OCompoundProperty( iObject, kTop ),
                          "intPropScalar" );
    OInt32Property intProp2( intProp.getPtr(), kWrapExisting );

    intProp.set( 5 );
    intProp2.set( 6 );

    boolProp.set( false );
}

void testOTypedArrayProperty( OObject &iObject )
{
    OBoolArrayProperty boolProp( OCompoundProperty( iObject, kTop ),
                                 "boolPropArray" );
    OInt32ArrayProperty intProp( OCompoundProperty( iObject, kTop ),
                               "intPropArray" );
    OInt32ArrayProperty intProp2( intProp.getPtr(), kWrapExisting );

    Alembic::Util::int32_t i[] = { 1, 2, 3, 4, 5 };
    intProp.set( Int32ArraySample( i, 5 ) );
    intProp2.set( Int32ArraySample( i, 5 ) );

    std::vector<bool_t> bools( 17 );
    boolProp.set( bools );
}

void testITypedScalarProperty( IObject &iObject )
{
    IBoolProperty boolProp( ICompoundProperty( iObject, kTop ),
                            "boolPropScalar" );
    IInt32Property intProp( ICompoundProperty( iObject, kTop ),
                          "intPropScalar" );
    IInt32Property intProp2( intProp.getPtr(), kWrapExisting );

    Alembic::Util::int32_t i;
    intProp.get( i );
    intProp2.get( i, 1 );

    Alembic::Util::bool_t b;
    boolProp.get( b );
}

void testITypedArrayProperty( IObject &iObject )
{
    IBoolArrayProperty boolProp( ICompoundProperty( iObject, kTop ),
                                 "boolPropArray" );
    IInt32ArrayProperty intProp( ICompoundProperty( iObject, kTop ),
                               "intPropArray" );
    IInt32ArrayProperty intProp2( intProp.getPtr(), kWrapExisting );

    Int32ArraySamplePtr iPtr;
    intProp.get( iPtr );
    intProp2.get( iPtr, 1 );

    BoolArraySamplePtr bPtr;
    boolProp.get( bPtr );
}

int main( int argc, char *argv[] )
{

    std::string archiveName = "compile_test.abc";
    {
        OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                          archiveName, ErrorHandler::kNoisyNoopPolicy );
        OObject archiveTop = archive.getTop();
        testOSchemaObject( archiveTop );
        OObject child( archiveTop, "otherChild" );
        testOSchema( child );
        testOTypedScalarProperty( child );
        testOTypedArrayProperty( child );
    }
    {
        IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), archiveName,
            ErrorHandler::kNoisyNoopPolicy );
        IObject archiveTop = archive.getTop();
        testISchemaObject( archiveTop );
        IObject child( archiveTop, "otherChild" );
        testISchema( child );
        testITypedScalarProperty( child );
        testITypedArrayProperty( child );
    }
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(),
                          archiveName, ErrorHandler::kNoisyNoopPolicy );
        OObject archiveTop = archive.getTop();
        testOSchemaObject( archiveTop );
        OObject child( archiveTop, "otherChild" );
        testOSchema( child );
        testOTypedScalarProperty( child );
        testOTypedArrayProperty( child );
    }
    {
        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), archiveName,
            ErrorHandler::kNoisyNoopPolicy );
        IObject archiveTop = archive.getTop();
        testISchemaObject( archiveTop );
        IObject child( archiveTop, "otherChild" );
        testISchema( child );
        testITypedScalarProperty( child );
        testITypedArrayProperty( child );
    }
    return 0;
}
