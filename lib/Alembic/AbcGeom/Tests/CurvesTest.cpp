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
#include <Alembic/AbcGeom/Tests/CurvesData.h>

#include "Assert.h"

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
void Example1_CurvesOut()
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
        "curves1.abc" );

    // Create a Curves class.
    OCurves myCurves(   OObject( archive, kTop ),
                        "reallly_long_curves_name");

    OCurvesSchema &curves = myCurves.getSchema();

    // UVs and Normals use GeomParams, which can be written or read
    // as indexed or not, as you'd like.

    /*
    OV2fGeomParam::Sample uvsamp( V2fArraySample( (const V2f *)g_uvs,
                                                  g_numUVs ),
                                  kFacevaryingScope );
    // indexed normals
    ON3fGeomParam::Sample nsamp( N3fArraySample( (const N3f *)g_normals,
                                                 g_numNormals ),
                                                    kFacevaryingScope );
    */

    V2fArraySample widthSample( V2fArraySample( (const V2f *)g_widths,
                                2));

    V2fArraySample uvSample( V2fArraySample( (const V2f *)g_uvs,
                                12));

    std::cout << "original size " << widthSample.size() << std::endl;
    std::cout << "uz original size " << uvSample.size() << std::endl;

    // Set a curve sample.
    // We're creating the sample inline here,
    // but we could create a static sample and leave it around,
    // only modifying the parts that have changed.
    std::cout << "creating sample" << std::endl;
    OCurvesSchema::Sample curves_sample(
        V3fArraySample( ( const V3f * ) g_verts, g_totalVerts ),
        "cubic",
        Int32ArraySample( ( const int * ) g_numVerts, g_numCurves),
        "nonperiodic",
        widthSample,
        uvSample
        );

    std::cout << "setting sample" << std::endl;

    // Set the sample.
    curves.set( curves_sample );

    std::cout << "done settings sample" << std::endl;

    // Alembic objects close themselves automatically when they go out
    // of scope. So - we don't have to do anything to finish
    // them off!
    std::cout << "Writing: " << archive.getName() << std::endl;
}


void Example1_CurvesIn()
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), "curves1.abc" );
    std::cout << "Reading: " << archive.getName() << std::endl;

    std::cout <<"constructing curves" << std::endl;
    ICurves myCurves( IObject( archive, kTop) , "reallly_long_curves_name");
    std::cout << "getting schema" << std::endl;
    ICurvesSchema &curves = myCurves.getSchema();
    std::cout << "done getting schema" << std::endl;

    // get the samples from the curves
    ICurvesSchema::Sample curvesSample;
    curves.get( curvesSample );

    // test the bounding box
    TESTING_ASSERT( curvesSample.getSelfBounds().min == V3d( -1.0, -1.0, -1.0 ) );
    std::cout << "bounds max " << curvesSample.getSelfBounds().max << std::endl;
    TESTING_ASSERT( curvesSample.getSelfBounds().max == V3d( 1.0, 1.0, 1.0 ) );

    std::cout << curves.getUBasis() << std::endl;
    std::cout << curves.getVBasis() << std::endl;

    // test other attributes
    //TESTING_ASSERT( curvesSample.getPositions() -> size() == 12);

    //TESTING_ASSERT( curvesSample.getWidths() -> size() == 2);


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

    std::cout << "writing curves" << std::endl;

    // Curves Out
    Example1_CurvesOut();

    std::cout << "wrote curves" << std::endl;

    Example1_CurvesIn();

    //Time_Sampled_Mesh_Test0();
    return 0;
}
