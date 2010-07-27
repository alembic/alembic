//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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
// libraries: AlembicAsset, which provides the basic Alembic Abstractions,
// and AlembicTraitsGeom, which implements specific Geometric primitives
// on top of AlembicAsset. There is a middle library, AlembicTraits, upon which
// AlembicTraitsGeom is built, but we don't use that specifically.
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
//-*****************************************************************************
// INCLUDES
//
// Each Library includes the entirety of itself in a self-named
// file. So, you can typically just do include lines like the following.
//-*****************************************************************************
//-*****************************************************************************

// Alembic Includes
// Technically, AlembicAsset is included by AlembicTraitsGeom, but I feel
// that this is usefully declarative.
#include <AlembicAsset/AlembicAsset.h>
#include <AlembicTraitsGeom/AlembicTraitsGeom.h>

// Other includes
// Alembic renames Imath types to its own namespace, but they're still
// derived from Imath, so we can use Imath's rich set of functions to
// manipulate those types.
#include <ImathBoxAlgo.h>
#include <ImathMatrixAlgo.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//-*****************************************************************************
//-*****************************************************************************
// NAMESPACES
//
// Most of Alembic's declarations live in the AlembicAsset namespace, which
// we typically alias to "Abc". The specific geometric primitives like
// SimplePolyMesh and SimpleParticles live in the namespace AlembicTraitsGeom,
// which we typically alias to "Atg".
//-*****************************************************************************
//-*****************************************************************************

namespace Abc = AlembicAsset;
namespace Atg = AlembicTraitsGeom;

//-*****************************************************************************
//-*****************************************************************************
// GLOBALS
// 
// To simplify the actual Alembic Usage code below, I'm going to declare
// a bunch of data arrays in the global scope here. These are just storage
// of vertices and indices in the "usual" way.  I'm making use of Alembic's
// specific data types (declared in AlembicAsset/Base/PlainOldDataType.h,
// as well as AlembicAsset/Base/FoundationBase.h)
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// 8 vertices of a unit cube
static const size_t g_numVerts = 8;
static const Abc::float64_t g_verts[] = { -1.0, -1.0, -1.0,
                                          1.0, -1.0, -1.0,
                                          -1.0, 1.0, -1.0,
                                          1.0, 1.0, -1.0,
                                          -1.0, -1.0, 1.0,
                                          1.0, -1.0, 1.0,
                                          -1.0, 1.0, 1.0,
                                          1.0, 1.0, 1.0 };

//-*****************************************************************************
// "Face Counts" - number of vertex indices per face.
static const size_t g_numFaces = 6;
static const Abc::int32_t g_counts[] = { 4, 4, 4, 4, 4, 4 };

//-*****************************************************************************
// Indices of the cube vertices
static const size_t g_numIndices = 24;
static const Abc::int32_t g_indices[] = {
    // LEFT
    0, 4, 6, 2,
    // RIGHT
    5, 1, 3, 7,
    // BOTTOM
    0, 1, 5, 4,
    // TOP,
    6, 7, 3, 2,
    // BACK
    1, 0, 2, 3,
    // FRONT
    4, 5, 7, 6 };

//-*****************************************************************************
// Normals of the cube vertices.
// Normals in AlembicTraitsGeom's SimplePolyMesh class are per-vertex, per-face,
// without indices. This is basically the storage that matches renderman's
// "facevarying" detail type.
static const size_t g_numNormals = g_numIndices;
static const Abc::float32_t g_normals[] = {
    // LEFT
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    // RIGHT
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    // BOTTOM
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    // TOP
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    // BACK
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    // FRONT
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f };

//-*****************************************************************************
// UVs of the cube vertices.
// UVS, like Normals, are per-vertex, per-face, without indices.
static const size_t g_numUVs = g_numIndices;
static const Abc::float32_t g_uvs[] = {
    // LEFT
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    // RIGHT
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    // BOTTOM
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    // TOP
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    // BACK
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    // FRONT
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f };

//-*****************************************************************************
//-*****************************************************************************
// WRITING OUT AN ANIMATED MESH
//
// Here we'll create an "Asset", which is Alembic's term for the actual
// file on disk containing all of the scene geometry. The Asset will contain
// just a single PolyMesh, which we will animate the transformation of.
//-*****************************************************************************
//-*****************************************************************************
void Example1_MeshOut()
{
    // Create an OAsset.
    // Like std::iostreams, we have a completely separate-but-parallel class
    // hierarchy for output and for input (OAsset, IAsset, and so on). This
    // maintains the important abstraction that AlembicAssets are for storage,
    // representation, and archival.
    Abc::OAsset asset(
        // The file name.
        // Because we're an OAsset, this is creating (or clobbering)
        // the asset with this filename.
        "meshOut1.abc",

        // By default, all Alembic classes will fail silently, and quite
        // happily continue working, though all operations will become
        // no-ops. This, again, is like std::iostreams. All Alembic objects
        // can be tested by calling their "valid()" function or by simply
        // testing like a boolean: if ( asset ).
        // However, if we want operations that might fail to throw an
        // exception, we pass the Abc::kThrowException flag into the
        // constructor.
        Abc::kThrowException );

    // Create a SimplePolyMesh class. This geometric type is declared
    // in AlembicTraitsGeom/PolyMesh.h.
    Atg::OSimplePolyMesh pmesh(

        // The first argument for creating any object (except the
        // top level asset) is a parent object. In this case, we're
        // placing the mesh in the top level asset. However, if we
        // had more objects, we could use them as parents of each other,
        // creating a hierarchy.
        asset,

        // The second argument is the name of the object. Names must
        // be unique within the scope of the parent object. If a
        // duplicate name is produced, the construction would fail,
        // either producing a no-op object or throwing an exception
        // if requested.
        "meshy",

        // Throw if something goes wrong!
        Abc::kThrowException );

    // Alembic objects consist of "traits", which are in turn composed
    // (amalgamated) from other traits. A "trait" is just a nugget of
    // object-behavior. In this case, SimplePolyMesh has a trait called
    // SimplePolyMeshTrait, which in turn has an XformLocalTrait that
    // creates a property that is named "xform|local" in the file and is
    // accessed via object->xform(). These traits are basically just
    // member functions wrapped around member data.
    // One of the 'features' that Traits offer for Object Specification
    // is the ability to declare some properties as optional. In this
    // case, both Normals and Uvs are optional, so we declare that
    // we'd like them to be present.
    //
    // We could do this the long way:
    // pmesh.trait().normals()
    // but since all AlembicTraitsGeom objects have one and only one
    // trait, we overload the (*pmesh) and pmesh-> operators to mean
    // pmesh.trait().
    pmesh->normals().addOptional( Abc::kThrowException );

    // Do it the other way, just to show
    pmesh.trait().uvs().addOptional( Abc::kThrowException );

    // Set the "rest pose"
    // Alembic objects have a built-in notion of a "rest pose",
    // which is read and written separately from animation samples
    // at any given time. If an object does not have its rest pose set,
    // it will attempt to set itself a default value, but warnings will
    // ensue.
    // These types are just renamed Imath types - please see
    // ImathBox.h and ImathMatrix.h for documentation.
    Abc::Box3d localB( Abc::V3d( -1.0, -1.0, -1.0 ),
                       Abc::V3d( 1.0, 1.0, 1.0 ) );
    Abc::M44d xform;
    xform.makeIdentity();
    pmesh->xform().set( xform );
    pmesh->bounds().set( Imath::transform( localB, xform ) );

    // Use those global values to set these arrays.
    pmesh->mesh().positions().set( ( const Abc::V3d * )g_verts, g_numVerts );
    pmesh->mesh().faceCounts().set( g_counts, g_numFaces );
    pmesh->mesh().vertexIndices().set( g_indices, g_numIndices );

    // Note that with normals and UVs, I have to use the pointer
    // notation, because they were optional and have the ability to
    // not exist. I'm not a big fan of this easily forgettable
    // hack, and am working on ways to eliminate it! But for now:
    pmesh->normals()->set( ( const Abc::V3f * )g_normals, g_numNormals );
    pmesh->uvs()->set( ( const Abc::V2f * )g_uvs, g_numUVs );

    // Animation! We declare properties to be animated one-by-one,
    // by passing them an instance of something called TimeSmaplingInfo.
    // The TimeSamplingInfo contains the time sampling type
    // (for now, uniform and varying are the only choices), as well
    // as the time, in seconds, of the first sample. If the time sampling
    // is uniform, a third argument specifies the time, in seconds, between
    // uniform samples.
    Abc::TimeSamplingInfo tsamp( Abc::kUniformTimeSampling,
                                 // First sample seconds
                                 0.0,
                                 // First sample interval
                                 1.0 / 24.0 );

    // Declare that we'd like to animate the transform and the bounds.
    // Alembic does not try to be "too smart", so even though we
    // understand that if the xform is animated, so too should be the
    // bounds - we don't enforce that in the API.
    // We could also animate the positions and normals, but not the
    // face counts, nor the vertex indices, as they have been declared
    // non-animating in AlembicTraitsGeom. Note that each and every
    // attribute can be animated with separate time sampling - so you
    // could happily create a helicopter rotor that had ten xform
    // samples per frame, while the rest of the asset had only one
    // sample per frame.
    //
    // Also! Animated properties that end up having non-changing
    // values across their entire sample range will automatically
    // collapse themselves to non-animated at storage close time.
    pmesh->xform().makeAnimated( tsamp );
    pmesh->bounds().makeAnimated( tsamp );

    // Loop over the time samples. Because both the xform and
    // bounds were animated with the same time-sampling, we can
    // set timesamples for both using the same sample index 'samp'.
    // If this were not the case, we could set the animated samples
    // using Abc::seconds_t, or even Abc::Time instances.
    for ( size_t samp = 0; samp < 48; ++samp )
    {
        Abc::float64_t ytrans = 0.125 * ( Abc::float64_t )samp;
        xform.setTranslation( Abc::V3d( 0.0, ytrans, 0.0 ) );
        pmesh->xform().setAnim( samp, xform );

        // This transforming boxes by xforms is declared in
        // ImathBoxAlgo.h
        pmesh->bounds().setAnim( samp,
                                 Imath::transform( localB, xform ) );
    }

    // Alembic objects close themselves automatically when they go out
    // of scope. So - we don't have to do anything to finish
    // them off!
    std::cout << "Wrote meshOut1.abc" << std::endl;
}

//-*****************************************************************************
//-*****************************************************************************
// READING IN AN ANIMATED MESH
//
// Here we'll do basically a similar set of operations as we did to write
// the Asset with the mesh out. We're going to assume that the file we're
// loading definitely has a mesh named as we wrote it out. There are other
// ways of introspecting an Asset or an Object, but we'll skip that for
// now.
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
void Example1_MeshIn()
{
    // Create an IAsset.
    // Just like OAsset! Again, using the std::iostreams metaphor.
    Abc::IAsset asset(
        // Filename
        "meshOut1.abc",

        // Specify that we wish exceptions instead of silent failure.
        Abc::kThrowException );
    std::cout << "Read asset meshOut1.abc" << std::endl;

    // Create an ISimplePolyMesh - this will look for an object in
    // the file, parented directly under the asset, with the name
    // "meshy", and the appropriate type for ISimplePolyMesh. If it
    // cannot find it, we specify that we want an exception thrown.
    Atg::ISimplePolyMesh pmesh(
        
        // Parent object
        asset,
        
        // This object's name.
        "meshy",

        // Throw if it cannot be found.
        Abc::kThrowException );
    std::cout << "Read simple poly mesh meshy." << std::endl;

    // See if it has normals.
    // All alembic objects and all their properties and traits
    // can always be treated as booleans. This is done using the
    // "unspecified_bool_type" method, so no accidental arithmetic operations
    // will occur.
    if ( pmesh->normals() )
    {
        std::cout << "meshy has normals" << std::endl;
    }

    // See if it has uvs.
    if ( pmesh->uvs() )
    {
        std::cout << "meshy has uvs" << std::endl;
    }

    // Get indices at the rest pose.
    // Here we get a "Sample" object, which is a typed array of
    // data. This is only for properties which are "ArrayProperties",
    // as the indices (and faceCounts, vertices, normals, uvs) of the
    // mesh are. This Sample object can be compared as a bool as well.
    Abc::IIntSample indices = pmesh->mesh().vertexIndices().get();
    size_t numIndices = indices.size();
    std::cout << "Num indices: " << numIndices << std::endl
              << "Indices: ";
    for ( size_t i = 0; i < numIndices; ++i )
    {
        std::cout << indices[i] << ", ";
    }
    std::cout << std::endl;

    // Let's just get the animated transforms, just to see how
    // that's done. ITimeSampling is a little more robust that
    // the TimeSamplingInfo (which could have been called OTimeSampling)
    // Here I'm kinda cheating in that I do already know that xform has
    // been animated.
    if ( pmesh->xform().isAnimated() )
    {
        // Get the Time Sampling class. Defined in
        // AlembicAsset/Base/ITimeSampling.h, this class provides
        // information about the time sampling type and functions to
        // convert between sample indices and sample times.
        const Abc::ITimeSampling *timeSampling =
            pmesh->xform().timeSampling();
        // a NULL time sampling on an animated object can't happen.
        assert( timeSampling != NULL );

        // Loop over the anim samples.
        size_t numSamples = timeSampling->numSamples();
        for ( size_t samp = 0; samp < numSamples; ++samp )
        {
            // The Time Sampling will tell us what the seconds of
            // a particular sample is. The IProperty class also
            // does this, so we could ask pmesh->xform() directly.
            Abc::seconds_t sampSecs = timeSampling->sampleSeconds( samp );

            // We are using the sample index here, but we could use time
            // or seconds.
            Abc::M44d sampXform = pmesh->xform().getAnim( samp );

            // Let's just ask for animation from another property,
            // whether it is animated or not. In this case, bounds are
            // animated, but even if they weren't, this would work.
            // Non-animated properties will simply return their rest pose.
            // When asking for animation samples with a time, you must
            // specify 'lowerBound' (highest sample with time
            // not-greater-than), or 'upperBound' (lowest sample with
            // time not-less-than)
            Abc::Box3d sampBounds =
                pmesh->bounds().getAnimLowerBound( sampSecs );

            std::cout << "Sample: " << samp
                      << " at time: " << sampSecs
                      << " has xform: " << sampXform << std::endl
                      << " and bounds: " << sampBounds.min
                      << " to " << sampBounds.max << std::endl;
        }
    }

    // Like the Alembic Output interface, we just let objects go out
    // of scope and they're groovy.
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MAIN FUNCTION!
// The only important thing here is to remember to do Abc::Init.
// I'm not going to bother with exceptions, since I have no actions I
// could do to deal with them. If something goes wrong, it will cheerfully
// crash and print the exception information.
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
int main( int argc, char *argv[] )
{
    // Initialize Alembic. Must be called before _ANY_ Alembic functions
    // may be used.
    Abc::Init();

    // Mesh out
    Example1_MeshOut();

    // And back in.
    Example1_MeshIn();

    return 0;
}
