//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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
#include <Alembic/AbcCoreOgawa/All.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

#include <memory.h>

using namespace Alembic::AbcGeom;

using Alembic::AbcCoreAbstract::chrono_t;
using Alembic::AbcCoreAbstract::index_t;

//-*****************************************************************************
Alembic::Util::shared_ptr< Alembic::Abc::OObject >
makeXform( Alembic::Abc::OObject & parent )
{
    Alembic::Util::shared_ptr< Alembic::AbcGeom::OXform > xformObjPtr
        ( new Alembic::AbcGeom::OXform( parent, "myXform" ) );

    // add a couple of ops
    XformOp transop( kTranslateOperation, kTranslateHint );
    XformOp scaleop( kScaleOperation, kScaleHint );

    XformSample samp;
    samp.addOp( transop, V3d( 1.0, 2.0, 3.0 ) );
    samp.addOp( scaleop, V3d( 2.0, 4.0, 6.0 ) );

    Alembic::AbcGeom::OXformSchema &schema = xformObjPtr->getSchema();

    schema.set( samp );

    return xformObjPtr;
}

//-*****************************************************************************
Alembic::Util::shared_ptr< Alembic::Abc::OObject >
subdCube( Alembic::Abc::OObject & parent )
{
    Alembic::Util::shared_ptr< Alembic::AbcGeom::OSubD > subdObjPtr
        ( new Alembic::AbcGeom::OSubD( parent, "mySubD" ) );

    Alembic::AbcGeom::OSubDSchema &schema = subdObjPtr->getSchema();

    std::vector<V3f> verts( 8 );
    std::vector<Alembic::Util::int32_t> indices( 8 );

    Alembic::AbcGeom::OSubDSchema::Sample sample( verts, indices, indices );

    schema.set( sample );

    return subdObjPtr;
}

//-*****************************************************************************
void OWrapExisting()
{
    Alembic::Abc::OArchive archive(
        Alembic::AbcCoreOgawa::WriteArchive(),
        "playground_owrap.abc"
                                  );

    Alembic::Abc::OObject archiveTop = archive.getTop();

    Alembic::Util::shared_ptr< Alembic::Abc::OObject > objAPtr =
        makeXform( archiveTop );

    Alembic::Util::shared_ptr< Alembic::Abc::OObject > objBPtr =
        subdCube( *objAPtr );

    //
    // NOW THE FUN BEGINS
    //
    TESTING_ASSERT( Alembic::AbcGeom::OSubD::matches( objBPtr->getHeader() ) );
    {
        Alembic::Util::shared_ptr< Alembic::AbcGeom::OSubD > subdObjPtr =
            Alembic::Util::dynamic_pointer_cast< Alembic::AbcGeom::OSubD >
                ( objBPtr );
        Alembic::AbcGeom::OSubD subdObj = *subdObjPtr;

        std::cout << "wrapped-existing subd has "
                  << subdObj.getSchema().getNumSamples() << " num samples."
                  << std::endl;


        std::vector<V3f> verts( 8, V3f(2.0, 2.0, 2.0 ) );
        Alembic::AbcGeom::OSubDSchema::Sample sample;
        sample.setPositions( Alembic::Abc::V3fArraySample( &(verts[0]),
                                                           verts.size() ) );
        subdObj.getSchema().set( sample );
        TESTING_ASSERT( subdObj.getSchema().getNumSamples() == 2 );
    }

    TESTING_ASSERT( Alembic::AbcGeom::OXform::matches( objAPtr->getHeader() ) );
    {
        XformOp transop( kTranslateOperation, kTranslateHint );
        XformOp scaleop( kScaleOperation, kScaleHint );

        XformSample samp;
        samp.addOp( transop, V3d( 4.0, 5.0, 6.0 ) );
        samp.addOp( scaleop, V3d( 8.0, 10.0, 12.0 ) );

        Alembic::Util::shared_ptr< Alembic::AbcGeom::OXform > xformObjPtr =
            Alembic::Util::dynamic_pointer_cast< Alembic::AbcGeom::OXform >
                ( objAPtr );
        Alembic::AbcGeom::OXform xformObj = *xformObjPtr;
        xformObj.getSchema().set( samp );
        TESTING_ASSERT( xformObj.getSchema().getNumSamples() == 2 );
    }
}

//-*****************************************************************************
void PolyMorphicAbstractPtrs()
{
    std::string name( "playground_pmap.abc" );
    {
        Alembic::Abc::OArchive archive(
            Alembic::AbcCoreOgawa::WriteArchive(),
            name
                                      );

        Alembic::Abc::OObject archiveTop = archive.getTop();

        AbcA::BasePropertyWriterPtr pwPtr;

        AbcA::ScalarPropertyWriterPtr swPtr;
        AbcA::ArrayPropertyWriterPtr awPtr;
        AbcA::ScalarPropertyWriterPtr aswPtr;

        AbcA::DataType dt( Alembic::Util::kUint32POD, 1 );

        AbcA::DataType arrayDT( kUint32POD, 254 );

        swPtr = archiveTop.getProperties().getPtr()->createScalarProperty(
            "scalarprop", AbcA::MetaData(), dt, 0 );

        aswPtr = archiveTop.getProperties().getPtr()->createScalarProperty(
            "arrayscalarprop", AbcA::MetaData(), arrayDT, 0 );

        awPtr = archiveTop.getProperties().getPtr()->createArrayProperty(
            "arrayprop", AbcA::MetaData(), dt, 0 );

        Alembic::Util::uint32_t sval = 2;
        std::vector<Alembic::Util::uint32_t> aval( 5, 3 );
        std::vector<Alembic::Util::uint32_t> saval( 254, 2 );

        // use base type as scalar prop writer
        pwPtr = swPtr;
        pwPtr->asScalarPtr()->setSample( &sval );

        // use base type as array prop writer
        pwPtr = awPtr;
        pwPtr->asArrayPtr()->setSample( AbcA::ArraySample( &(aval.front()), dt,
                                                           Dimensions( 5 ) ) );

        // use base type as "array scalar prop"
        pwPtr = aswPtr;
        pwPtr->asScalarPtr()->setSample( &(saval.front()) );
    }

    // now read in
    {
        ReadArraySampleCachePtr cachePtr;

        if ( cachePtr ) { std::cout << "cachePtr is valid" << std::endl; }
        if ( cachePtr != NULL ) { std::cout << "cachePtr is not NULL" << std::endl; }
        else { std::cout << "cachePtr IS NULL" << std::endl; }
        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), name,
                          ErrorHandler::kThrowPolicy,
                          cachePtr );

        IUInt32ArrayProperty p( archive.getTop().getProperties(),
                                "arrayprop" );

        UInt32ArraySamplePtr samp = p.getValue();

        std::cout << "*samp[0]: " << (*samp)[0] << std::endl;
    }



}

//-*****************************************************************************
void StupidData()
{
    std::string archiveName( "stupiddata.abc" );

    // out
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(),
                          archiveName );

        OObject myobject( archive.getTop(), "myobject" );

        // a compound property to use as the parent for data-containing properties
        OCompoundProperty props = myobject.getProperties();

        OInt32ArrayProperty intArrayProp( props, "intArrayProp" );

        for ( Alembic::Util::int32_t i = 0 ; i < 10 ; ++i )
        {
            std::vector<Alembic::Util::int32_t> v( i, i );
            intArrayProp.set( v );
            TESTING_ASSERT( intArrayProp.getNumSamples() ==
                            ( size_t ) ( i + 1 ) );
        }
        std::cout << std::endl << "Writing " << archiveName << std::endl;
    }

    // in
    {
        std::cout << std::endl << "Reading " << archiveName << std::endl;

        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(),
                          archiveName );

        IObject myobject( archive.getTop(), "myobject" );

        // a compound property to use as the parent for data-containing properties
        ICompoundProperty props = myobject.getProperties();

        IInt32ArrayProperty intArrayProp( props, "intArrayProp" );

        TESTING_ASSERT( intArrayProp.getNumSamples() == 10 );

        // try to create a non-existent property
        IFloatArrayProperty nonProp( props, "doesntexist",
                                     ErrorHandler::kQuietNoopPolicy );

        if ( nonProp )
        { std::cout << "this should not be printed" << std::endl; }
        else
        { std::cout << "non-existent Property doesn't exist." << std::endl; }

        for ( size_t i = 0 ; i < 10 ; ++i )
        {
            std::vector<Alembic::Util::int32_t> v( i, i );
            Int32ArraySamplePtr samp = intArrayProp.getValue( i );
            size_t numpoints = samp->size();

            TESTING_ASSERT( numpoints == i );

            std::cout << "sample " << i << ": ";

            for ( size_t j = 0 ; j < numpoints ; ++j )
            {
                std::cout << (*samp)[j];
                if ( numpoints > 0 && j < numpoints - 1 )
                { std::cout << ", "; }
            }
            std::cout << std::endl;
        }
    }

}

//-*****************************************************************************
int main( int, char** )
{
    OWrapExisting();
    PolyMorphicAbstractPtrs();
    StupidData();
    return 0;
}
