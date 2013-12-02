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

//-*****************************************************************************
//-*****************************************************************************
// EXAMPLE1 - INTRODUCTION
//
// Hello Alembic User! This is the first Example Usage file, and so we'll
// start by targeting the thing you'd most often want to do - write and read
// animated, geometric primitives. To do this, we will be using two main
// libraries: Alembic::Abc, which provides the basic Alembic Abstractions,
// and Alembic::AbcGeom, which implements specific Geometric primitives
// on top of Alembic::Abc.
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
//-*****************************************************************************
// INCLUDES
//
// Each Library includes the entirety of its public self in a file named "All.h"
// file. So, you can typically just do include lines like the following.
//-*****************************************************************************
//-*****************************************************************************

// Alembic Includes
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

// Other includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// We include some global mesh data to test with from an external source
// to keep this example code clean.
#include <Alembic/AbcGeom/Tests/MeshData.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

//-*****************************************************************************
//-*****************************************************************************
// NAMESPACES
//
// Each library has a namespace which is the same as the name of the library.
// We shorten those here for brevity.
//-*****************************************************************************
//-*****************************************************************************

using namespace std;
using namespace Alembic::AbcGeom; // Contains Abc, AbcCoreAbstract

//-*****************************************************************************
//-*****************************************************************************
// WRITING OUT AN ANIMATED MESH
//
// Here we'll create an "Archive", which is Alembic's term for the actual
// file on disk containing all of the scene geometry. The Archive will contain
// a single animated Transform with a single static PolyMesh as its child.
//-*****************************************************************************
//-*****************************************************************************
void Example1_MeshOut()
{
    // Create an OArchive.
    // Like std::iostreams, we have a completely separate-but-parallel class
    // hierarchy for output and for input (OArchive, IArchive, and so on). This
    // maintains the important abstraction that Alembic is for storage,
    // representation, and archival. (as opposed to being a dynamic scene
    // manipulation framework).
    OArchive archive(

        // The hard link to the implementation.
        Alembic::AbcCoreHDF5::WriteArchive(),

        // The file name.
        // Because we're an OArchive, this is creating (or clobbering)
        // the archive with this filename.
        "polyMesh1.abc" );

    // Create a PolyMesh class.
    OPolyMesh meshyObj( OObject( archive, kTop ), "meshy" );
    OPolyMeshSchema &mesh = meshyObj.getSchema();

    // some apps can arbitrarily name their primary UVs, this function allows
    // you to do that, and must be done before the first time you set UVs
    // on the schema
    mesh.setUVSourceName("test");

    // UVs and Normals use GeomParams, which can be written or read
    // as indexed or not, as you'd like.
    OV2fGeomParam::Sample uvsamp( V2fArraySample( (const V2f *)g_uvs,
                                                  g_numUVs ),
                                  kFacevaryingScope );
    // indexed normals
    ON3fGeomParam::Sample nsamp( N3fArraySample( (const N3f *)g_normals,
                                                 g_numNormals ),
                                 kFacevaryingScope );

    // Set a mesh sample.
    // We're creating the sample inline here,
    // but we could create a static sample and leave it around,
    // only modifying the parts that have changed.
    OPolyMeshSchema::Sample mesh_samp(
        V3fArraySample( ( const V3f * )g_verts, g_numVerts ),
        Int32ArraySample( g_indices, g_numIndices ),
        Int32ArraySample( g_counts, g_numCounts ),
        uvsamp, nsamp );

    // not actually the right data; just making it up
    Box3d cbox;
    cbox.extendBy( V3d( 1.0, -1.0, 0.0 ) );
    cbox.extendBy( V3d( -1.0, 1.0, 3.0 ) );

    // Set the sample twice
    mesh.set( mesh_samp );
    mesh.set( mesh_samp );

    // do it twice to make sure getChildBoundsProperty works correctly
    mesh.getChildBoundsProperty().set( cbox );
    mesh.getChildBoundsProperty().set( cbox );

    // Alembic objects close themselves automatically when they go out
    // of scope. So - we don't have to do anything to finish
    // them off!
    std::cout << "Writing: " << archive.getName() << std::endl;
}

//-*****************************************************************************
void Example1_MeshIn()
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), "polyMesh1.abc" );
    std::cout << "Reading: " << archive.getName() << std::endl;

    IGeomBaseObject geomBase( IObject( archive, kTop ), "meshy" );
    TESTING_ASSERT( geomBase.getSchema().getSelfBoundsProperty().valid() );

    IPolyMesh meshyObj( IObject( archive, kTop ), "meshy" );
    IPolyMeshSchema &mesh = meshyObj.getSchema();
    IN3fGeomParam N = mesh.getNormalsParam();
    IV2fGeomParam uv = mesh.getUVsParam();

    TESTING_ASSERT( ! N.isIndexed() );

    TESTING_ASSERT( ! uv.isIndexed() );

    IPolyMeshSchema::Sample mesh_samp;
    mesh.get( mesh_samp );
    IGeomBase::Sample baseSamp;
    geomBase.getSchema().get( baseSamp );

    TESTING_ASSERT( mesh_samp.getSelfBounds().min == V3d( -1.0, -1.0, -1.0 ) );

    TESTING_ASSERT( mesh_samp.getSelfBounds().max == V3d( 1.0, 1.0, 1.0 ) );

    TESTING_ASSERT( baseSamp.getSelfBounds().min == V3d( -1.0, -1.0, -1.0 ) );

    TESTING_ASSERT( baseSamp.getSelfBounds().max == V3d( 1.0, 1.0, 1.0 ) );

    ICompoundProperty arbattrs = mesh.getArbGeomParams();

    // we didn't set any on write, so on read, it should be an invalid container
    TESTING_ASSERT( ! arbattrs );

    // getExpandedValue() takes an optional ISampleSelector;
    // getVals() returns a TypedArraySamplePtr
    N3fArraySamplePtr nsp = N.getExpandedValue().getVals();

    TESTING_ASSERT( N.isConstant() );
    TESTING_ASSERT( uv.isConstant() );

    TESTING_ASSERT( IsGeomParam( N.getHeader() ) );

    TESTING_ASSERT( GetSourceName( uv.getMetaData() ) == "test" );
    TESTING_ASSERT( isUV( uv.getHeader() ) );

    N3f n0 = (*nsp)[0];

    for ( size_t i = 0 ; i < nsp->size() ; ++i )
    {
        std::cout << i << "th normal: " << (*nsp)[i] << std::endl;
    }

    TESTING_ASSERT( n0 == N3f( -1.0f, 0.0f, 0.0f ) );
    std::cout << "0th normal: " << n0 << std::endl;

    IV2fGeomParam::Sample uvsamp;
    TESTING_ASSERT( !uvsamp.valid() );
    uvsamp = uv.getIndexedValue();
    TESTING_ASSERT( uvsamp.valid() );

    TESTING_ASSERT( (*(uvsamp.getIndices()))[1] == 1 );
    V2f uv2 = (*(uvsamp.getVals()))[2];
    TESTING_ASSERT( uv2 == V2f( 1.0f, 1.0f ) );
    std::cout << "2th UV: " << uv2 << std::endl;

    std::cout << "Mesh num vertices: "
              << mesh_samp.getPositions()->size() << std::endl;

    std::cout << "0th vertex from the mesh sample: "
              << (*(mesh_samp.getPositions()))[0] << std::endl;

    std::cout << "0th vertex from the mesh sample with get method: "
              << mesh_samp.getPositions()->get()[0] << std::endl;
}

//-*****************************************************************************
void meshUnderXformOut( const std::string &iName )
{
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(), iName );

    TimeSamplingPtr ts( new TimeSampling( 1.0 / 24.0, 0.0 ) );

    OXform xfobj( archive.getTop(), "xf", ts );

    OPolyMesh meshobj( xfobj, "mesh", ts );

    OPolyMeshSchema::Sample mesh_samp(
        V3fArraySample( ( const V3f * )g_verts, g_numVerts ),
        Int32ArraySample( g_indices, g_numIndices ),
        Int32ArraySample( g_counts, g_numCounts ) );

    XformSample xf_samp;
    XformOp rotOp( kRotateYOperation );

    Box3d childBounds;
    childBounds.makeEmpty();
    childBounds.extendBy( V3d( 1.0, 1.0, 1.0 ) );
    childBounds.extendBy( V3d( -1.0, -1.0, -1.0 ) );

    xfobj.getSchema().getChildBoundsProperty().set( childBounds );

    double rotation = 0.0;

    for ( std::size_t i = 0 ; i < 100 ; ++i )
    {
        xf_samp.addOp( rotOp, rotation );
        xfobj.getSchema().set( xf_samp );
        meshobj.getSchema().set( mesh_samp );
        rotation += 30.0;
    }
}

//-*****************************************************************************
void optPropTest()
{
    std::string name = "meshOptPropTest.abc";
    {
        OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(), name );
        OPolyMesh meshyObj( OObject( archive, kTop ), "mesh" );
        OPolyMeshSchema &mesh = meshyObj.getSchema();

        // copy because we'll be changing these values
        std::vector< V3f > verts( g_numVerts );
        for ( size_t i = 0; i < g_numVerts; ++i )
        {
            verts[i] = V3f( g_verts[3*i], g_verts[3*i+1], g_verts[3*i+2] );
        }

        OPolyMeshSchema::Sample mesh_samp(
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

        ON3fGeomParam::Sample nsamp( N3fArraySample( (const N3f *)g_normals,
            g_numNormals ), kFacevaryingScope );
        mesh_samp.setNormals( nsamp );

        OV2fGeomParam::Sample uvsamp( V2fArraySample( (const V2f *)g_uvs,
                                      g_numUVs ), kFacevaryingScope );
        mesh_samp.setUVs( uvsamp );
        mesh_samp.setVelocities( V3fArraySample( ( const V3f * )g_veloc,
                                               g_numVerts ) );
        mesh.set( mesh_samp );

        mesh_samp.setNormals( ON3fGeomParam::Sample() );
        mesh_samp.setUVs( OV2fGeomParam::Sample() );
        mesh_samp.setVelocities( V3fArraySample() );
        mesh.set( mesh_samp );


        mesh_samp.setVelocities( V3fArraySample( ( const V3f * )g_veloc,
                                               g_numVerts ) );
        mesh_samp.setUVs( uvsamp );
        mesh_samp.setNormals( nsamp );

        for ( size_t i = 0; i < 2; ++i )
        {
            mesh.set( mesh_samp );
            for ( size_t j = 0; j < g_numVerts; ++j )
            {
                verts[j] *= 2;
            }
        }

        mesh_samp.setUVs( OV2fGeomParam::Sample() );
        mesh_samp.setNormals( ON3fGeomParam::Sample() );
        mesh_samp.setVelocities( V3fArraySample() );
        mesh.set( mesh_samp );
    }

    {
        IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), name );

        IPolyMesh meshyObj( IObject( archive, kTop ), "mesh" );
        IPolyMeshSchema &mesh = meshyObj.getSchema();
        TESTING_ASSERT( 7 == mesh.getNumSamples() );
        TESTING_ASSERT( 7 == mesh.getVelocitiesProperty().getNumSamples() );
        TESTING_ASSERT( 7 == mesh.getUVsParam().getNumSamples() );
        TESTING_ASSERT( 7 == mesh.getNormalsParam().getNumSamples() );
        TESTING_ASSERT(
            GetSourceName( mesh.getUVsParam().getMetaData() ) == "" );
        TESTING_ASSERT( isUV( mesh.getUVsParam().getHeader() ) );
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MAIN FUNCTION!
// I'm not going to bother with exceptions, since I have no actions I
// could do to deal with them. If something goes wrong, it will cheerfully
// crash and print the exception information.
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
int main( int argc, char *argv[] )
{
    // Mesh out
    Example1_MeshOut();
    Example1_MeshIn();

    meshUnderXformOut( "animatedXformedMesh.abc" );

    optPropTest();
    return 0;
}
