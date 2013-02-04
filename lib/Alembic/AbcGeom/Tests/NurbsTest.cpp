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

#include <Alembic/AbcGeom/Tests/NurbsData.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

using namespace std;
using namespace Alembic::AbcGeom;

void Example1_NurbsOut()
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
        "nurbs1.abc" );

    ONuPatch myNurbs( OObject( archive, kTop ),
                      "nurbs_surface" );

    ONuPatchSchema &myNurbsSchema = myNurbs.getSchema();

    V3fArraySample pSample( (const V3f *)g_P, g_nP );
    FloatArraySample uKnotSample( (const float32_t *)g_uKnot, 8 );
    FloatArraySample vKnotSample( (const float *)g_vKnot, 8 );

    ONuPatchSchema::Sample nurbsSample(
        pSample,
        g_nu,
        g_nv,
        g_uOrder,
        g_vOrder,
        uKnotSample,
        vKnotSample
        );

    // set the trim curve
    nurbsSample.setTrimCurve(   g_trim_nLoops,
                                Int32ArraySample( (const Alembic::Util::int32_t *) &g_trim_nCurves, 1),
                                Int32ArraySample( (const Alembic::Util::int32_t *) &g_trim_n, 1),
                                Int32ArraySample( (const Alembic::Util::int32_t *) &g_trim_order, 1),
                                FloatArraySample( (const float32_t *) &g_trim_knot, 12),
                                FloatArraySample( (const float32_t *) &g_trim_min, 1),
                                FloatArraySample( (const float32_t *) &g_trim_max, 1),
                                FloatArraySample( (const float32_t *) &g_trim_u, 9),
                                FloatArraySample( (const float32_t *) &g_trim_v, 9),
                                FloatArraySample( (const float32_t *) &g_trim_w, 9));

    // Set the sample.
    myNurbsSchema.set( nurbsSample );

    // Alembic objects close themselves automatically when they go out
    // of scope. So - we don't have to do anything to finish
    // them off!
    std::cout << "Writing: " << archive.getName() << std::endl;
}

void Example2_NurbsOut()
{
    // same as example 1 but without the trim curves
    OArchive archive(
        Alembic::AbcCoreHDF5::WriteArchive(),
        "nurbs2.abc" );

    ONuPatch myNurbs(   OObject( archive, kTop ),
                        "nurbs_surface_noTrim");

    ONuPatchSchema &myNurbsSchema = myNurbs.getSchema();

    V3fArraySample pSample( (const V3f *)g_P, g_nP );
    FloatArraySample uKnotSample( (const float32_t *)g_uKnot, 8 );
    FloatArraySample vKnotSample( (const float32_t *)g_vKnot, 8 );

    ONuPatchSchema::Sample nurbsSample(
        pSample,
        g_nu,
        g_nv,
        g_uOrder,
        g_vOrder,
        uKnotSample,
        vKnotSample
        );

    // Set the sample.
    myNurbsSchema.set( nurbsSample );
}

void Example3_NurbsOut()
{
    // same as example 1 but without the trim curves
    OArchive archive(
        Alembic::AbcCoreHDF5::WriteArchive(),
        "nurbs3.abc" );

    ONuPatch myNurbs(   OObject( archive, kTop ),
                        "nurbs_surface_withW");

    ONuPatchSchema &myNurbsSchema = myNurbs.getSchema();

    V3fArraySample pSample( (const V3f *)g_P, g_nP );
    FloatArraySample uKnotSample( (const float32_t *)g_uKnot, 8 );
    FloatArraySample vKnotSample( (const float32_t *)g_vKnot, 8 );
    FloatArraySample pwSample( (const float32_t *)g_Pw, g_nP );

    ONuPatchSchema::Sample nurbsSample(
        pSample,
        g_nu,
        g_nv,
        g_uOrder,
        g_vOrder,
        uKnotSample,
        vKnotSample
        );

    nurbsSample.setPositionWeights(pwSample);

    // Set the sample.
    myNurbsSchema.set( nurbsSample );
}

void Example1_NurbsIn()
{
    std::cout << "loading archive" << std::endl;
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), "nurbs1.abc" );

    std::cout << "making INuPatch object" << std::endl;
    INuPatch myNurbs( IObject( archive, kTop) , "nurbs_surface");

    std::cout << "getting INuPatch schema" << std::endl;
    INuPatchSchema &nurbsSchema = myNurbs.getSchema();

    // get the samples from the curves
    std::cout << "getting INuPatch sample" << std::endl;
    INuPatchSchema::Sample nurbsSample;
    nurbsSchema.get( nurbsSample );

    // test the bounding box

    std::cout << nurbsSample.getSelfBounds().min << std::endl;
    std::cout << nurbsSample.getSelfBounds().max << std::endl;

    TESTING_ASSERT( nurbsSample.getSelfBounds().min == V3d( 0.0, 0.0, -3.0 ) );
    TESTING_ASSERT( nurbsSample.getSelfBounds().max == V3d( 3.0, 3.0, 3.0 ) );

    TESTING_ASSERT( nurbsSample.getTrimNumLoops() == 1 );
    TESTING_ASSERT( nurbsSample.getTrimOrders() -> size() == 1 );
    TESTING_ASSERT( nurbsSample.hasTrimCurve() == true );
    TESTING_ASSERT( nurbsSchema.isConstant() == true );
}

void Example2_NurbsIn()
{
    std::cout << "loading archive" << std::endl;
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), "nurbs2.abc" );

    std::cout << "making INuPatch object" << std::endl;
    INuPatch myNurbs( IObject( archive, kTop) , "nurbs_surface_noTrim");

    std::cout << "getting INuPatch schema" << std::endl;
    INuPatchSchema &nurbsSchema = myNurbs.getSchema();

    // get the samples from the curves
    std::cout << "getting INuPatch sample" << std::endl;
    INuPatchSchema::Sample nurbsSample;
    nurbsSchema.get( nurbsSample );

    // test the bounding box

    std::cout << nurbsSample.getSelfBounds().min << std::endl;
    std::cout << nurbsSample.getSelfBounds().max << std::endl;

    TESTING_ASSERT( nurbsSample.getSelfBounds().min == V3d( 0.0, 0.0, -3.0 ) );
    TESTING_ASSERT( nurbsSample.getSelfBounds().max == V3d( 3.0, 3.0, 3.0 ) );

    std::cout << "Number of trim curves: " << nurbsSample.getTrimNumLoops() << std::endl;
    TESTING_ASSERT( nurbsSample.getTrimNumLoops() == 0 );
    TESTING_ASSERT( nurbsSample.hasTrimCurve() == false );
    TESTING_ASSERT( !nurbsSample.getPositionWeights() );
    TESTING_ASSERT( nurbsSchema.isConstant() == true );
}

void Example3_NurbsIn()
{
    std::cout << "loading archive" << std::endl;
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), "nurbs3.abc" );

    std::cout << "making INuPatch object" << std::endl;
    INuPatch myNurbs( IObject( archive, kTop) , "nurbs_surface_withW");

    std::cout << "getting INuPatch schema" << std::endl;
    INuPatchSchema &nurbsSchema = myNurbs.getSchema();

    // get the samples from the curves
    std::cout << "getting INuPatch sample" << std::endl;
    INuPatchSchema::Sample nurbsSample;
    nurbsSchema.get( nurbsSample );

    // test the bounding box

    std::cout << nurbsSample.getSelfBounds().min << std::endl;
    std::cout << nurbsSample.getSelfBounds().max << std::endl;

    TESTING_ASSERT( nurbsSample.getSelfBounds().min == V3d( 0.0, 0.0, -3.0 ) );
    TESTING_ASSERT( nurbsSample.getSelfBounds().max == V3d( 3.0, 3.0, 3.0 ) );

    std::cout << "Number of trim curves: " << nurbsSample.getTrimNumLoops() << std::endl;
    TESTING_ASSERT( nurbsSample.getTrimNumLoops() == 0 );
    TESTING_ASSERT( nurbsSample.hasTrimCurve() == false );
    TESTING_ASSERT( nurbsSample.getPositionWeights()->size() ==
                    ( size_t ) g_nP );
    TESTING_ASSERT( nurbsSchema.isConstant() == true );
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

    std::cout << "writing nurbs" << std::endl;

    // Nurbs Out
    Example1_NurbsOut();
    std::cout << "done writing nurbs" << std::endl;

    std::cout << "reading nurbs" << std::endl;
    Example1_NurbsIn();
    std::cout << "done reading nurbs" << std::endl;

    Example2_NurbsOut();
    std::cout << "done writing nurbs 2" << std::endl;

    std::cout << "reading nurbs 2" << std::endl;
    Example2_NurbsIn();
    std::cout << "done reading nurbs 2" << std::endl;

    Example3_NurbsOut();
    std::cout << "done writing nurbs 3" << std::endl;

    std::cout << "reading nurbs 3" << std::endl;
    Example3_NurbsIn();
    std::cout << "done reading nurbs 3" << std::endl;

    return 0;
}
