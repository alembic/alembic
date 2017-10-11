//-*****************************************************************************
//
// Copyright (c) 2009-2014,
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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// We include some global mesh data to test with from an external source
// to keep this example code clean.
#include <Alembic/AbcGeom/Tests/MeshData.h>

//-*****************************************************************************
using namespace Alembic::AbcGeom; // Contains Abc, AbcCoreAbstract

void Example1_MeshOut()
{
    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), "subD1.abc" );

    // Create a SubD class.
    OSubD meshyObj( OObject( archive, kTop ), "subd" );
    OSubDSchema &mesh = meshyObj.getSchema();

    // Set a mesh sample.
    // We're creating the sample inline here,
    // but we could create a static sample and leave it around,
    // only modifying the parts that have changed.
    OSubDSchema::Sample mesh_samp(
        V3fArraySample( ( const V3f * )g_verts, g_numVerts ),
        Int32ArraySample( g_indices, g_numIndices ),
        Int32ArraySample( g_counts, g_numCounts ) );

    std::vector<Alembic::Util::int32_t> creases;
    std::vector<Alembic::Util::int32_t> corners;
    std::vector<Alembic::Util::int32_t> creaseLengths;
    std::vector<float32_t> creaseSharpnesses;
    std::vector<float32_t> cornerSharpnesses;
    std::vector<Alembic::Util::int32_t> holes;
    holes.push_back(0);
    holes.push_back(5);

    for ( size_t i = 0 ; i < 24 ; i++ )
    {
        creases.push_back( g_indices[i] );
        corners.push_back( g_indices[i] );
        cornerSharpnesses.push_back( 10.0 );
    }

    for ( size_t i = 0 ; i < 6 ; i++ )
    {
        creaseLengths.push_back( 4 );
        creaseSharpnesses.push_back( 0.5 );
    }

    mesh_samp.setCreases( creases, creaseLengths, creaseSharpnesses );
    mesh_samp.setCorners( corners, cornerSharpnesses );
    mesh_samp.setHoles( holes );

    // velocities
    mesh_samp.setVelocities( V3fArraySample( ( const V3f * )g_veloc,
                                            g_numVerts ) );
    // UVs
    OV2fGeomParam::Sample uvsamp( V2fArraySample( (const V2f *)g_uvs,
                                                  g_numUVs ),
                                  kFacevaryingScope );

    mesh_samp.setUVs( uvsamp );

    mesh.setUVSourceName("cool");

    // Set the sample.
    mesh.set( mesh_samp );

    // change one of the schema's parameter's
    mesh_samp.setInterpolateBoundary( 1 );
    mesh.set( mesh_samp );

    // test that the integer property doesn't latch to non-zero
    mesh_samp.setInterpolateBoundary( 0 );
    mesh.set( mesh_samp );

    C3f color_val( 1.0, 0.0, 0.0 );

    OCompoundProperty arbParams = mesh.getArbGeomParams();
    C3fArraySample val_samp( &color_val, 1 );

    OC3fGeomParam color( arbParams, "color", false, kConstantScope, 1 );
    OC3fGeomParam::Sample color_samp( val_samp, kConstantScope );

    // write red
    color.set( color_samp );

    // now purple
    color_val.z = 1.0;
    color.set( color_samp );

    OC3fGeomParam colorIndexed( arbParams, "colori", true,
        kFacevaryingScope, 1);

    std::vector< C3f > color_vals;
    color_vals.push_back( C3f( 0.0, 1.0, 1.0 ) );
    color_vals.push_back( C3f( 1.0, 0.0, 1.0 ) );
    color_vals.push_back( C3f( 1.0, 1.0, 0.0 ) );
    Alembic::Util::uint32_t indices[24] = { 2, 2, 1, 1, 0, 0, 1, 2,
        1, 1, 1, 0, 0, 0, 2, 2, 0, 0, 0, 2, 2, 2, 1, 1 };

    C3fArraySample cval_samp( &(color_vals.front()), color_vals.size() );
    UInt32ArraySample cind_samp( indices, 24 );
    color_samp.setVals( cval_samp );
    color_samp.setIndices( cind_samp );
    color_samp.setScope( kFacevaryingScope );
    colorIndexed.set( color_samp );

    std::cout << "Writing: " << archive.getName() << std::endl;
}

//-*****************************************************************************
void Example1_MeshIn()
{
    IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), "subD1.abc" );
    std::cout << "Reading: " << archive.getName() << std::endl;


    IGeomBaseObject geomBase( IObject( archive, kTop ), "subd" );
    TESTING_ASSERT( geomBase.getSchema().getSelfBoundsProperty().valid() );

    ISubD meshyObj( IObject( archive, kTop ), "subd",
        ErrorHandler::kNoisyNoopPolicy );

    ISubDSchema &mesh = meshyObj.getSchema();

    TESTING_ASSERT( mesh.getErrorHandlerPolicy() ==
                    ErrorHandler::kNoisyNoopPolicy );

    TESTING_ASSERT(
        mesh.getUVsParam().getValueProperty().getErrorHandlerPolicy() ==
        ErrorHandler::kNoisyNoopPolicy );

    TESTING_ASSERT(
        mesh.getInterpolateBoundaryProperty().getErrorHandlerPolicy() ==
        ErrorHandler::kNoisyNoopPolicy );

    TESTING_ASSERT( 3 == mesh.getNumSamples() );

    // UVs
    IV2fGeomParam uv = mesh.getUVsParam();
    TESTING_ASSERT( ! uv.isIndexed() );
    TESTING_ASSERT( GetSourceName( uv.getMetaData() ) == "cool" );
    TESTING_ASSERT( isUV( uv.getHeader() ) );

    // we can fake like the UVs are indexed
    IV2fGeomParam::Sample uvsamp = uv.getIndexedValue();
    TESTING_ASSERT( (*(uvsamp.getIndices()))[1] == 1 );
    V2f uv2 = (*(uvsamp.getVals()))[2];
    TESTING_ASSERT( uv2 == V2f( 1.0f, 1.0f ) );
    std::cout << "2th UV: " << uv2 << std::endl;


    // get the 1th sample by value
    ISubDSchema::Sample samp1 = mesh.getValue( 1 );
    IGeomBase::Sample baseSamp = geomBase.getSchema().getValue( 1 );

    std::cout << "bounds: " << samp1.getSelfBounds().min << ", "
              << samp1.getSelfBounds().max << std::endl;

    TESTING_ASSERT( samp1.getSelfBounds().min == V3d( -1.0, -1.0, -1.0 ) );

    TESTING_ASSERT( samp1.getSelfBounds().max == V3d( 1.0, 1.0, 1.0 ) );

    TESTING_ASSERT( baseSamp.getSelfBounds().min == V3d( -1.0, -1.0, -1.0 ) );

    TESTING_ASSERT( baseSamp.getSelfBounds().max == V3d( 1.0, 1.0, 1.0 ) );

    for ( size_t i = 0 ; i < samp1.getCreaseSharpnesses()->size() ; ++i )
    {
        std::cout << "crease sharpness[" << i << "]: "
                  << (*(samp1.getCreaseSharpnesses()))[i] << std::endl;
        TESTING_ASSERT( 0.5 == (*(samp1.getCreaseSharpnesses()))[i] );
    }

    for ( size_t i = 0 ; i < samp1.getCornerSharpnesses()->size() ; ++i )
    {
        std::cout << "corner sharpness[" << i << "]: "
                  << (*(samp1.getCornerSharpnesses()))[i] << std::endl;
        TESTING_ASSERT( 10.0 == (*(samp1.getCornerSharpnesses()))[i] );
    }

    for ( size_t i = 0 ; i < samp1.getVelocities()->size() ; ++i )
    {
        V3f veloc( g_veloc[i*3], g_veloc[i*3+1], g_veloc[i*3+2] );
        std::cout << "velocities[" << i << "]: "
        << (*(samp1.getVelocities()))[i] << std::endl;
        TESTING_ASSERT( veloc == (*(samp1.getVelocities()))[i] );
    }

    TESTING_ASSERT( samp1.getHoles()->size() == 2 );
    TESTING_ASSERT( (*(samp1.getHoles()))[0] == 0 );
    TESTING_ASSERT( (*(samp1.getHoles()))[1] == 5 );

    // test the second sample has '1' as the interpolate boundary value
    TESTING_ASSERT( 1 == samp1.getInterpolateBoundary() );

    std::cout << "Interpolate boundary at 1th sample: "
              << samp1.getInterpolateBoundary() << std::endl;

    // get the twoth sample by reference
    ISubDSchema::Sample samp2;
    mesh.get( samp2, 2 );

    TESTING_ASSERT( samp2.getSelfBounds().min == V3d( -1.0, -1.0, -1.0 ) );

    TESTING_ASSERT( samp2.getSelfBounds().max == V3d( 1.0, 1.0, 1.0 ) );

    TESTING_ASSERT( 0 == samp2.getInterpolateBoundary() );

    std::cout << "Interpolate boundary at 2th sample: "
              << samp2.getInterpolateBoundary() << std::endl;

    std::cout << "Mesh num vertices: "
              << samp2.getPositions()->size() << std::endl;

    std::cout << "0th vertex from the mesh sample: "
              << (*(samp2.getPositions()))[0] << std::endl;

    std::cout << "0th vertex from the mesh sample with get method: "
              << samp2.getPositions()->get()[0] << std::endl;

    ICompoundProperty arbattrs = mesh.getArbGeomParams();

    // This better exist since we wrote custom attr called color to it
    TESTING_ASSERT( arbattrs );

    for (int i = 0; i < 2; ++ i)
    {
        PropertyHeader p = arbattrs.getPropertyHeader(i);

        TESTING_ASSERT( IC3fGeomParam::matches( p ) );
        TESTING_ASSERT( OC3fGeomParam::matches( p ) );
        TESTING_ASSERT( ! IC3cGeomParam::matches( p ) );
        TESTING_ASSERT( ! OC3cGeomParam::matches( p ) );
        TESTING_ASSERT( ! IInt32GeomParam::matches( p ) );
        TESTING_ASSERT( ! IFloatGeomParam::matches( p ) );
        TESTING_ASSERT( ! IDoubleGeomParam::matches( p ) );
        TESTING_ASSERT( ! IV3iGeomParam::matches( p ) );
        TESTING_ASSERT( ! IV3fGeomParam::matches( p ) );

        if ( p.getName() == "color" )
        {
            IC3fGeomParam color(arbattrs, "color");
            TESTING_ASSERT( color.getValueProperty().isScalarLike() );

            IC3fGeomParam::Sample cSamp0, cSamp1;
            color.getExpanded(cSamp0, 0);
            color.getExpanded(cSamp1, 1);
            TESTING_ASSERT( (*(cSamp0.getVals()))[0] == C3f( 1.0, 0.0, 0.0 ) );
            TESTING_ASSERT( (*(cSamp1.getVals()))[0] == C3f( 1.0, 0.0, 1.0 ) );
        }
        else if ( p.getName() == "colori" )
        {
            IC3fGeomParam color(arbattrs, "colori");
            TESTING_ASSERT( !color.getValueProperty().isScalarLike() );

            IC3fGeomParam::Sample cSamp;
            color.getIndexed( cSamp );
            TESTING_ASSERT( cSamp.getScope() == kFacevaryingScope );
            TESTING_ASSERT( cSamp.getVals()->size() == 3 );
            TESTING_ASSERT( (*cSamp.getVals())[0] == C3f( 0.0, 1.0, 1.0 ) );
            TESTING_ASSERT( (*cSamp.getVals())[1] == C3f( 1.0, 0.0, 1.0 ) );
            TESTING_ASSERT( (*cSamp.getVals())[2] == C3f( 1.0, 1.0, 0.0 ) );

            Alembic::Util::uint32_t indices[24] = { 2, 2, 1, 1, 0, 0, 1, 2,
                1, 1, 1, 0, 0, 0, 2, 2, 0, 0, 0, 2, 2, 2, 1, 1};

            for (int j = 0; j < 24; ++j)
            {
                TESTING_ASSERT( (*cSamp.getIndices())[j] == indices[j] );
            }
        }
    }
}

//-*****************************************************************************
void optPropTest()
{
    std::string name = "subdVelocTest.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), name );
        OSubD meshyObj( OObject( archive, kTop ), "subd" );
        OSubDSchema &mesh = meshyObj.getSchema();

        // copy because we'll be changing these values
        std::vector< V3f > verts( g_numVerts );
        for ( size_t i = 0; i < g_numVerts; ++i )
        {
            verts[i] = V3f( g_verts[3*i], g_verts[3*i+1], g_verts[3*i+2] );
        }

        OSubDSchema::Sample mesh_samp(
            V3fArraySample( verts ),
            Int32ArraySample( g_indices, g_numIndices ),
            Int32ArraySample( g_counts, g_numCounts ) );

        for ( size_t i = 0; i < 2; ++i )
        {
            mesh.set( mesh_samp );
            for ( size_t j = 0; j < g_numVerts; ++j )
            {
                verts[j] *= 2;
            }
        }

        OV2fGeomParam::Sample uvsamp( V2fArraySample( (const V2f *)g_uvs,
                                      g_numUVs ), kFacevaryingScope );
        mesh_samp.setUVs( uvsamp );

        mesh_samp.setVelocities( V3fArraySample( ( const V3f * )g_veloc,
                                               g_numVerts ) );

        mesh.set( mesh_samp );

        mesh_samp.setUVs( OV2fGeomParam::Sample() );
        mesh_samp.setVelocities( V3fArraySample() );
        mesh.set( mesh_samp );


        mesh_samp.setVelocities( V3fArraySample( ( const V3f * )g_veloc,
                                               g_numVerts ) );
        mesh_samp.setUVs( uvsamp );

        for ( size_t i = 0; i < 2; ++i )
        {
            mesh.set( mesh_samp );
            for ( size_t j = 0; j < g_numVerts; ++j )
            {
                verts[j] *= 2;
            }
        }

        mesh_samp.setUVs( OV2fGeomParam::Sample() );
        mesh_samp.setVelocities( V3fArraySample() );
        mesh.set( mesh_samp );
    }

    {
        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), name );

        ISubD meshyObj( IObject( archive, kTop ), "subd" );
        ISubDSchema &mesh = meshyObj.getSchema();
        TESTING_ASSERT( 7 == mesh.getNumSamples() );
        TESTING_ASSERT( 7 == mesh.getVelocitiesProperty().getNumSamples() );
        TESTING_ASSERT( 7 == mesh.getUVsParam().getNumSamples() );
        TESTING_ASSERT(
            GetSourceName( mesh.getUVsParam().getMetaData() ) == "" );
        TESTING_ASSERT( isUV( mesh.getUVsParam().getHeader() ) );

        TESTING_ASSERT( !mesh.getCreaseIndicesProperty() );
        TESTING_ASSERT( !mesh.getCreaseLengthsProperty() );
        TESTING_ASSERT( !mesh.getCreaseSharpnessesProperty() );
        TESTING_ASSERT( !mesh.getCornerIndicesProperty() );
        TESTING_ASSERT( !mesh.getCornerSharpnessesProperty() );
        TESTING_ASSERT( !mesh.getHolesProperty() );

        TESTING_ASSERT( !mesh.getFaceVaryingInterpolateBoundaryProperty() );
        TESTING_ASSERT( !mesh.getFaceVaryingPropagateCornersProperty() );
        TESTING_ASSERT( !mesh.getInterpolateBoundaryProperty() );
        TESTING_ASSERT( !mesh.getSubdivisionSchemeProperty() );

    }
}

//-*****************************************************************************
void sparseTest()
{
    std::string name = "sparseSubDTest.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), name );
        OSubD meshUVsObj( OObject( archive, kTop ), "meshUVs", kSparse );

        // only set UVs
        OSubDSchema::Sample meshSamp;
        OV2fGeomParam::Sample uvSample( V2fArraySample( (const V2f *)g_uvs,
            g_numUVs ), kFacevaryingScope );
        meshSamp.setUVs( uvSample );
        meshUVsObj.getSchema().set( meshSamp );

        // only set pts
        OSubD meshPosObj( OObject( archive, kTop ), "meshPositions", kSparse );
        OSubDSchema::Sample meshSamp2;
        meshSamp2.setPositions(
            V3fArraySample( ( const V3f * )g_verts, g_numVerts ) );
        meshPosObj.getSchema().set( meshSamp2 );
    }

    {
        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), name );

        IObject meshUVsObj( IObject( archive, kTop ), "meshUVs" );

        // This should NOT match
        TESTING_ASSERT( !ISubDSchema::matches( meshUVsObj.getMetaData() ) );
        ICompoundProperty geomProp( meshUVsObj.getProperties(), ".geom" );

        // This shouldn't match either
        TESTING_ASSERT( !ISubDSchema::matches( geomProp.getMetaData() ) );

        // and we should ONLY have UVs
        TESTING_ASSERT( geomProp.getNumProperties() == 1 &&
            geomProp.getPropertyHeader("uv") != NULL );

        IArrayProperty uvsProp( geomProp, "uv" );
        TESTING_ASSERT( uvsProp.getNumSamples() == 1 );

        IObject meshPtsObj( IObject( archive, kTop ), "meshPositions" );

        // This should NOT match
        TESTING_ASSERT( !ISubDSchema::matches( meshPtsObj.getMetaData() ) );
        geomProp = ICompoundProperty( meshPtsObj.getProperties(), ".geom" );

        // This shouldn't match either
        TESTING_ASSERT( !ISubDSchema::matches( geomProp.getMetaData() ) );
        TESTING_ASSERT( geomProp.getNumProperties() == 2 &&
            geomProp.getPropertyHeader("P") != NULL &&
            geomProp.getPropertyHeader(".selfBnds") != NULL );
        IArrayProperty ptsProp( geomProp, "P" );
        TESTING_ASSERT( ptsProp.getNumSamples() == 1 );
        IScalarProperty selfBndsProp( geomProp, ".selfBnds" );
        TESTING_ASSERT( selfBndsProp.getNumSamples() == 1 );
    }
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    Example1_MeshOut();
    Example1_MeshIn();

    optPropTest();

    sparseTest();

    return 0;
}
