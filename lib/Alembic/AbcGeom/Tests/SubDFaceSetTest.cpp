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
#include <Alembic/AbcCoreHDF5/All.h>

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
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(), "facesetSubD1.abc" );

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

    for ( size_t i = 0 ; i < 24 ; i++ )
    {
        creases.push_back( g_indices[i] );
        corners.push_back( g_indices[i] );
        cornerSharpnesses.push_back( 1.0e38 );
    }

    for ( size_t i = 0 ; i < 6 ; i++ )
    {
        creaseLengths.push_back( 4 );
        creaseSharpnesses.push_back( 1.0e38 );
    }

    mesh_samp.setCreases( creases, creaseLengths, creaseSharpnesses );
    mesh_samp.setCorners( corners, cornerSharpnesses );

    // UVs
    OV2fGeomParam::Sample uvsamp( V2fArraySample( (const V2f *)g_uvs,
                                                  g_numUVs ),
                                  kFacevaryingScope );

    mesh_samp.setUVs( uvsamp );


    // Set the sample.
    mesh.set( mesh_samp );

    // change one of the schema's parameter's
    mesh_samp.setInterpolateBoundary( 1 );
    mesh.set( mesh_samp );

    // test that the integer property doesn't latch to non-zero
    mesh_samp.setInterpolateBoundary( 0 );
    mesh.set( mesh_samp );

    // ***** FaceSet testing
    std::vector <std::string> faceSetNames;
    OFaceSet my_face_set_obj = mesh.createFaceSet ("testing_faceset");
    mesh.getFaceSetNames (faceSetNames);
    TESTING_ASSERT( faceSetNames.size () == 1 );
    OFaceSetSchema my_face_set = my_face_set_obj.getSchema ();
    std::cout << " - created our FaceSet called : " << my_face_set_obj.getName() << std::endl;
    std::vector<Alembic::Util::int32_t> face_nums;
    // Our FaceSet is composed of faces 1-3
    face_nums.push_back (1);
    face_nums.push_back (2);
    face_nums.push_back (3);
    OFaceSetSchema::Sample my_face_set_samp ( face_nums );
    // faceset is visible, doesn't change.
    my_face_set.set ( my_face_set_samp );
    my_face_set.setFaceExclusivity ( kFaceSetExclusive );
    TESTING_ASSERT (my_face_set.getFaceExclusivity () == kFaceSetExclusive );
    // Test that we've computed selfBounds correctly.
    Box3d  face_set_bounds = my_face_set_samp.getSelfBounds ();
    // std::cout << " -                 has bounds : " << face_set_bounds << std::endl;

    OObject parentOfFaceSet = my_face_set_obj.getParent ();
    OObject grandParent = parentOfFaceSet.getParent ();
    /* NYI see issue 150
    // Both construct by name and kWrapExisiting fail with
    // ERROR: EXCEPTION:
    // Already have an Object named: subd
    // OSubD faceSetMesh( grandParent, "subd");
    //OSubD faceSetMesh(  parentOfFaceSet, kWrapExisting );
    */
    // OSubDSchema &faceSetMeshSchema = faceSetMesh.getSchema();


    std::cout << "Writing: " << archive.getName() << std::endl;
}

//-*****************************************************************************
void Example1_MeshIn()
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), "facesetSubD1.abc" );
    std::cout << "Reading: " << archive.getName() << std::endl;

    ISubD meshyObj( IObject( archive, kTop ), "subd" );
    ISubDSchema &mesh = meshyObj.getSchema();

    TESTING_ASSERT( 3 == mesh.getNumSamples() );

    // ***** FaceSet testing
    TESTING_ASSERT( mesh.hasFaceSet ("testing_faceset" ) );
    std::vector <std::string> faceSetNames;
    mesh.getFaceSetNames (faceSetNames);
    TESTING_ASSERT( faceSetNames.size () == 1 );
    std::vector <std::string>::iterator nameIter;
    for (nameIter = faceSetNames.begin (); nameIter != faceSetNames.end ();
        ++nameIter)
    {
       std::cout << " obj subd has FaceSet - " << *nameIter << std::endl;
    }
    TESTING_ASSERT( faceSetNames [0] == "testing_faceset" );
    IFaceSet faceSetObj = mesh.getFaceSet( "testing_faceset" );
    IFaceSetSchema faceSet = faceSetObj.getSchema();
    TESTING_ASSERT (faceSet.getFaceExclusivity () == kFaceSetExclusive );
    IFaceSetSchema::Sample faceSetSamp0;
    faceSet.get( faceSetSamp0 );
    Int32ArraySample faces = *(faceSetSamp0.getFaces ());
    TESTING_ASSERT( faces [0] == 1 &&
                    faces [1] == 2 &&
                    faces [2] == 3);
    // end of FaceSet testing

    // UVs
    IV2fGeomParam uv = mesh.getUVsParam();
    TESTING_ASSERT( ! uv.isIndexed() );

    // we can fake like the UVs are indexed
    IV2fGeomParam::Sample uvsamp = uv.getIndexedValue();
    TESTING_ASSERT( (*(uvsamp.getIndices()))[1] == 1 );
    V2f uv2 = (*(uvsamp.getVals()))[2];
    TESTING_ASSERT( uv2 == V2f( 1.0f, 1.0f ) );
    std::cout << "2th UV: " << uv2 << std::endl;


    // get the 1th sample by value
    ISubDSchema::Sample samp1 = mesh.getValue( 1 );

    TESTING_ASSERT( samp1.getSelfBounds().min == V3d( -1.0, -1.0, -1.0 ) );

    TESTING_ASSERT( samp1.getSelfBounds().max == V3d( 1.0, 1.0, 1.0 ) );


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
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    Example1_MeshOut();
    Example1_MeshIn();

    return 0;
}
