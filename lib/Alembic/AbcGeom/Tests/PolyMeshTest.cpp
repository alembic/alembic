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

#include "Assert.h"

//-*****************************************************************************
//-*****************************************************************************
// NAMESPACES
//
// Each library has a namespace which is the same as the name of the library.
// We shorten those here for brevity.
//-*****************************************************************************
//-*****************************************************************************

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

    // UVs and Normals use GeomParams, which can be written or read
    // as indexed or not, as you'd like.
    OV2fGeomParam::Sample uvsamp( V2fArraySample( (const V2f *)g_uvs,
                                                  g_numUVs ),
                                  kFacevaryingScope );
    // indexed normals
    ON3fGeomParam::Sample nsamp( N3fArraySample( (const N3f *)g_normals,
                                                 g_numNormals ),
                                 UInt32ArraySample( g_uindices, g_numIndices),
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

    // Set the sample.
    mesh.set( mesh_samp );

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

    IPolyMesh meshyObj( IObject( archive, kTop ), "meshy" );
    IPolyMeshSchema &mesh = meshyObj.getSchema();
    IN3fGeomParam N = mesh.getNormals();
    IV2fGeomParam uv = mesh.getUVs();

    TESTING_ASSERT( N.isIndexed() );

    TESTING_ASSERT( ! uv.isIndexed() );

    IPolyMeshSchema::Sample mesh_samp;
    mesh.get( mesh_samp );

    // getExpandedValue() takes an optional ISampleSelector;
    // getVals() returns a TypedArraySamplePtr
    N3fArraySamplePtr nsp = N.getExpandedValue().getVals();

    N3f n0 = (*nsp)[0];

    TESTING_ASSERT( n0 == N3f( -1.0f, 0.0f, 0.0f ) );
    std::cout << "0th normal: " << n0 << std::endl;

    IV2fGeomParam::Sample uvsamp = uv.getIndexedValue();

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

#if 0

//-*****************************************************************************
void Time_Sampled_Mesh_Test0_Writer()
{
    using namespace Ago;

    OArchive archive( "time_samples_mesh0.abc" );
    std::cout << "Writing: " << archive.getName() << std::endl;

    OPolyMesh mesh( archive, "meshy" );
    OSampleSelector oSS;
    for (int iSample=0; iSample<10; iSample++)
    {
        OPolyMeshTrait::Sample mesh_samp(

            V3fArraySample( ( const V3f * )g_verts, g_numVerts ),
            Int32ArraySample( g_indices, g_numIndices ),
            Int32ArraySample( g_starts, g_numStarts ),
            V3fArraySample( ( const V3f * )g_normals, g_numNormals ),
            V2fArraySample( ( const V2f * )g_uvs, g_numUVs )

            );
        mesh.set( mesh_samp, OSampleSelector( iSample) );
        std::cout << "  ..wrote sample " << iSample << std::endl;
    }

    OPolyMeshTrait::Sample mesh_samp;
    mesh.set( mesh_samp );
}

void Time_Sampled_Mesh_Test0_Reader()
{
    using namespace Ago;

    IArchive archive( "time_samples_mesh0.abc" );
    std::cout << "Reading: " << archive.getName() << std::endl;


    IPolyMesh mesh( archive, "meshy" );

    std::cout << "mesh has " << mesh->getNumChildren() << " children"
              << std::endl;

    // Find out information about the positions
    ICompoundProperty comp( mesh, ".geom" );
    IV3fArrayProperty posPtr( comp, "P" );
    std::cout << "mesh positions has " << posPtr->getNumSamples() << " samples"
              << std::endl;

    for (int ii=0; ii<posPtr->getNumSamples(); ii++)
    {
        std::cout << " sample: " << ii << "  v[0] = ";
        V3fArraySamplePtr posSamp;
        posPtr.get( posSamp, (size_t) ii );
        std::cout << (*posSamp)[0] << std::endl;
        // is the same as:
        //   std::cout << posSamp->get()[0] << std::endl;
    }




    IPolyMeshTrait::Sample mesh_samp;
    mesh.get( mesh_samp );

    std::cout << "Mesh num vertices: "
              << mesh_samp.getPositions()->size() << std::endl;

    //std::cout << "Mesh num vertex samples: "
    //          << mesh_samp.getPositions()->getNumSamples() << std::endl;


}

void Time_Sampled_Mesh_Test0()
{
    Time_Sampled_Mesh_Test0_Writer();
    Time_Sampled_Mesh_Test0_Reader();
}

#endif

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

    //Time_Sampled_Mesh_Test0();
    return 0;
}
