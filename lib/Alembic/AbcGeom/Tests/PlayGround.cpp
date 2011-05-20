//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

using namespace Alembic::AbcGeom;

using Alembic::AbcCoreAbstract::chrono_t;
using Alembic::AbcCoreAbstract::index_t;
using Alembic::Util::uint32_t;
using Alembic::Util::float32_t;
using Alembic::Util::int32_t;


//-*****************************************************************************
Alembic::Abc::OObject subdCube( Alembic::Abc::OObject parent )
{
    Alembic::AbcGeom::OSubD subdObj( parent, "mySubD",
                                     Alembic::Abc::ErrorHandler::kNoisyNoopPolicy );
    Alembic::AbcGeom::OSubDSchema &schema = subdObj.getSchema();

    std::vector<V3f> verts( 8 );
    std::vector<int32_t> indices( 8 );

    Alembic::AbcGeom::OSubDSchema::Sample sample( verts, indices, indices );

    schema.set( sample );

    return subdObj;
}

//-*****************************************************************************
void OWrapExisting()
{
    Alembic::Abc::OArchive archive(
        Alembic::AbcCoreHDF5::WriteArchive(),
        "playground_owrap.abc"
                                  );
    Alembic::Abc::OObject archiveTop = archive.getTop();

    Alembic::Abc::OObject obj = subdCube( archiveTop );

    //
    // NOW THE FUN BEGINS
    //

    if( Alembic::AbcGeom::OSubD::matches( obj.getHeader() ) )
    {
        Alembic::AbcGeom::OSubD subdObj( obj, Alembic::Abc::kWrapExisting );

        std::cout << "wrapped-existing subd has "
                  << subdObj.getSchema().getNumSamples() << " num samples."
                  << std::endl;


        //Alembic::AbcGeom::OSubDSchema::Sample sample;
        //sample.setPositions( Alembic::Abc::V3fArraySample( ( const Alembic::Abc::V3f* )g_verts,
        //                                                   g_numVerts ) );
        //schema().set( sample, Alembic::Abc::OSampleSelector( 1 ) );
    }
}

//-*****************************************************************************
void PolyMorphicAbstractPtrs()
{
    std::string name( "playground_pmap.abc" );
    {
        Alembic::Abc::OArchive archive(
            Alembic::AbcCoreHDF5::WriteArchive(),
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

        uint32_t sval = 2;
        std::vector<uint32_t> aval( 5, 3 );
        std::vector<uint32_t> saval( 254, 2 );

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
        IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), name,
                          ErrorHandler::kThrowPolicy,
                          cachePtr );

        IUInt32ArrayProperty p( archive.getTop().getProperties(),
                                "arrayprop" );

        UInt32ArraySamplePtr samp = p.getValue();

        std::cout << "*samp[0]: " << (*samp)[0] << std::endl;
    }



}

//-*****************************************************************************
int main( int, char** )
{
    //OWrapExisting();
    PolyMorphicAbstractPtrs();
    return 0;
}
