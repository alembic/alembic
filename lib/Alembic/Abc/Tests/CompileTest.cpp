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

#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/Abc/All.h>

#ifdef ALEMBIC_WITH_HDF5
#include <Alembic/AbcCoreHDF5/All.h>
#endif

using namespace Alembic::Abc;

// A bunch of minimal compile tests to make sure the templates compile

// Declare a test schema.
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "TestSchema_v1", "", ".test", false,
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
    OTestSchema tst( OCompoundProperty( iParent, kTop ),
                     OTestSchema::getDefaultSchemaName() );
    OTestSchema tst2( tst, "foo", ErrorHandler::kQuietNoopPolicy );
    OTestSchema tst3( tst2, "bar", kNoMatching );
    OTestSchema tst4( tst3, "baz",
                      ErrorHandler::kNoisyNoopPolicy,
                      kStrictMatching );
}

void testISchemaObject( IObject & iParent )
{
    ITest tst( iParent, "childTestObject" );
    ITest tst2( tst, ErrorHandler::kQuietNoopPolicy );
    ITest tst3( tst2, kNoMatching );
    ITest tst4( tst3, kWrapExisting,
                ErrorHandler::kNoisyNoopPolicy,
                kStrictMatching );

    ITestSchema& schm = tst4.getSchema();
    schm.valid();
}

void testISchema( IObject &iParent )
{
    ITestSchema tst( ICompoundProperty( iParent, kTop ) );
    std::cout << "isNull? " << int( tst.getPtr() == NULL ) << std::endl;
    ITestSchema tst2( tst.getPtr(), ErrorHandler::kQuietNoopPolicy );
    std::cout << "isNull? " << int( tst2.getPtr() == NULL ) << std::endl;
    ITestSchema tst3( tst2.getPtr(), kNoMatching );
    ITestSchema tst4( tst3,
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

void testInstancedSchema()
{
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(),
                          "instancedTest.abc" );

        OObject archiveTop = archive.getTop();
        OObject a( archiveTop, "a" );
        OTest aa( a, "a" );

        OObject b( archiveTop, "b" );
        b.addChildInstance( aa, "bb" );
    }

    {
        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(),
                          "instancedTest.abc" );
        IObject archiveTop = archive.getTop();
        IObject b( archiveTop, "b" );
        IObject bb( b, "bb" );
        ITest bb1( b, "bb" );
        ITest bb2( bb );
        ABCA_ASSERT( bb.getName() == bb1.getName(),
            "Instanced bb and bb1 names don't match." );
        ABCA_ASSERT( bb.getName() == bb2.getName(),
            "Instanced bb and bb1 names don't match." );
        ABCA_ASSERT( bb.getFullName() == "/b/bb", "Bad full name for bb" );
        ABCA_ASSERT( bb1.getFullName() == "/b/bb", "Bad full name for bb1" );
        ABCA_ASSERT( bb2.getFullName() == "/b/bb", "Bad full name for bb2" );
    }
}

int main( int argc, char *argv[] )
{

    std::string archiveName = "compile_test.abc";
#ifdef ALEMBIC_WITH_HDF5
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
#endif

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

    testInstancedSchema();
    return 0;
}
