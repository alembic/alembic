//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include <Alembic/TraitsGeom/TraitsGeomAll.h>
#include <Alembic/TraitsGeom/Test.h>
#include <Alembic/Asset/AssetAll.h>
#include <ImathBoxAlgo.h>
#include <ImathMatrixAlgo.h>

namespace Abc = Alembic::Asset;

namespace Alembic {
namespace TraitsGeom {

//-*****************************************************************************
void meshOut()
{
    Abc::OAsset asset( "meshOut1.abc", Abc::kThrowException );
    Abc::float64_t verts[] = { -1.0, -1.0, -1.0,
                               1.0, -1.0, -1.0,
                               -1.0, 1.0, -1.0,
                               1.0, 1.0, -1.0,
                               -1.0, -1.0, 1.0,
                               1.0, -1.0, 1.0,
                               -1.0, 1.0, 1.0,
                               1.0, 1.0, 1.0 };
    size_t numVerts = 8;

    Abc::int32_t counts[] = { 4, 4, 4, 4, 4, 4 };
    size_t numFaces = 6;

    Abc::int32_t indices[] = {
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
    size_t numIndices = 24;

    Abc::float32_t normals[] = {
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
    size_t numNormals = numIndices;

    Abc::float32_t uvs[] = {
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
    size_t numUVs = numIndices;

    // Mesh has animated xform but not verts.
    OSimplePolyMesh pmesh( asset, "meshy",
                           Abc::kThrowException );
    pmesh->normals().addOptional( Abc::kThrowException );
    pmesh->uvs().addOptional( Abc::kThrowException );

    // Set rest.
    Abc::Box3d localB( Abc::V3d( -1.0, -1.0, -1.0 ),
                       Abc::V3d( 1.0, 1.0, 1.0 ) );
    Abc::M44d xform;
    xform.makeIdentity();
    pmesh->xform().set( xform );
    pmesh->bounds().set( Imath::transform( localB, xform ) );
    pmesh->mesh().positions().set( ( const Abc::V3d * )verts, numVerts );
    pmesh->mesh().faceCounts().set( counts, numFaces );
    pmesh->mesh().vertexIndices().set( indices, numIndices );
    pmesh->normals()->set( ( const Abc::V3f * )normals, numNormals );
    pmesh->uvs()->set( ( const Abc::V2f * )uvs, numUVs );

    // Set anim xforms.
    Abc::TimeSamplingInfo tsamp( Abc::kUniformTimeSampling,
                                 // First sample seconds
                                 0.0,
                                 // First sample interval
                                 1.0 / 24.0 );
    pmesh->xform().makeAnimated( tsamp );
    pmesh->bounds().makeAnimated( tsamp );
    for ( size_t samp = 0; samp < 48; ++samp )
    {
        Abc::float64_t ytrans = 0.125 * ( Abc::float64_t )samp;
        xform.setTranslation( Abc::V3d( 0.0, ytrans, 0.0 ) );
        pmesh->xform().setAnim( samp, xform );
        pmesh->bounds().setAnim( samp,
                                 Imath::transform( localB, xform ) );
    }

    // All done.
    std::cout << "Wrote meshOut1.abc" << std::endl;
}

//-*****************************************************************************
void meshIn()
{
    Abc::IAsset asset( "meshOut1.abc" );
    std::cout << "Read asset meshOut1.abc" << std::endl;

    ISimplePolyMesh pmesh( asset, "meshy" );
    std::cout << "Read simple poly mesh meshy." << std::endl;

    // See if it has normals.
    if ( pmesh->normals() )
    {
        std::cout << "meshy has normals" << std::endl;
    }

    // See if it has uvs.
    if ( pmesh->uvs() )
    {
        std::cout << "meshy has uvs" << std::endl;
    }

    // Get indices
    Abc::IIntSample indices = pmesh->mesh().vertexIndices().get();
    size_t numIndices = indices.size();
    std::cout << "Num indices: " << numIndices << std::endl
              << "Indices: ";
    for ( size_t i = 0; i < numIndices; ++i )
    {
        std::cout << indices[i] << ", ";
    }
    std::cout << std::endl;
}

//-*****************************************************************************
void subdOut()
{
    Abc::OAsset asset( "subdOut1.abc", Abc::kThrowException );

    Abc::float64_t verts[] = { -1.0, -1.0, -1.0,
                               1.0, -1.0, -1.0,
                               -1.0, 1.0, -1.0,
                               1.0, 1.0, -1.0,
                               -1.0, -1.0, 1.0,
                               1.0, -1.0, 1.0,
                               -1.0, 1.0, 1.0,
                               1.0, 1.0, 1.0 };
    size_t numVerts = 8;

    Abc::int32_t counts[] = { 4, 4, 4, 4, 4, 4 };
    size_t numFaces = 6;

    Abc::int32_t indices[] = {
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
    size_t numIndices = 24;

    Abc::float32_t uvs[] = {
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
    size_t numUVs = numIndices;

    Abc::int32_t holes[] = { 0, 2, 4 };
    size_t numHoles = 3;

    Abc::int32_t corners[] = { 1, 3, 5, 7 };
    size_t numCorners = 4;

    // Subd has animated xform but not verts.
    OSimpleSubd psubd( asset, "subdy",
                       Abc::kThrowException );
    psubd->uvs().addOptional( Abc::kThrowException );
    psubd->subd().holes().addOptional( Abc::kThrowException );
    psubd->subd().corners().addOptional( Abc::kThrowException );

    // Set rest.
    Abc::Box3d localB( Abc::V3d( -1.0, -1.0, -1.0 ),
                       Abc::V3d( 1.0, 1.0, 1.0 ) );
    Abc::M44d xform;
    xform.makeIdentity();
    psubd->xform().set( xform );
    psubd->bounds().set( Imath::transform( localB, xform ) );
    psubd->mesh().positions().set( ( const Abc::V3d * )verts, numVerts );
    psubd->mesh().faceCounts().set( counts, numFaces );
    psubd->mesh().vertexIndices().set( indices, numIndices );
    psubd->subd().scheme().set( ( char )kCatmullClarkSubd );
    psubd->subd().boundary().set( ( char )kEdgesCornersBoundaryInterp );
    psubd->subd().holes()->set( holes, numHoles );
    psubd->subd().corners()->set( corners, numCorners );
    psubd->uvs()->set( ( const Abc::V2f * )uvs, numUVs );

    // Set anim xforms.
    Abc::TimeSamplingInfo tsamp( Abc::kUniformTimeSampling,

                                 // First sample seconds
                                 0.0,

                                 // First sample interval
                                 1.0 / 24.0 );
    psubd->xform().makeAnimated( tsamp );
    psubd->bounds().makeAnimated( tsamp );
    for ( size_t samp = 0; samp < 48; ++samp )
    {
        Abc::float64_t ytrans = 0.125 * ( Abc::float64_t )samp;
        xform.setTranslation( Abc::V3d( 0.0, ytrans, 0.0 ) );
        psubd->xform().setAnim( samp, xform );
        psubd->bounds().setAnim( samp,
                                 Imath::transform( localB, xform ) );
    }

    // All done.
    std::cout << "Wrote subdOut1.abc" << std::endl;
}

//-*****************************************************************************
void subdIn()
{
    Abc::IAsset asset( "subdOut1.abc" );
    std::cout << "Read asset subdOut1.abc" << std::endl;

    ISimpleSubd psubd( asset, "subdy" );
    std::cout << "Read simple subd subdy." << std::endl;


    // See if it has uvs.
    if ( psubd->uvs() )
    {
        std::cout << "subdy has uvs" << std::endl;
    }

    // See if it has holes
    if ( psubd->subd().holes() )
    {
        std::cout << "Subd has holes" << std::endl;
    }

    // See if it has corners
    if ( psubd->subd().corners() )
    {
        std::cout << "Subd has corners" << std::endl;
    }

    // See if it has creases
    if ( psubd->subd().creases() )
    {
        std::cout << "Subd has creases" << std::endl;
    }
    else
    {
        std::cout << "Subd does not have creases." << std::endl;
    }

    // Get scheme
    if ( psubd->subd().scheme().get() == kCatmullClarkSubd )
    {
        std::cout << "CATMULL CLARK SUBD!" << std::endl;
    }
    else
    {
        std::cout << "LOOP SUBD!" << std::endl;
    }

    SubdBoundaryInterpolation binterp =
        ( SubdBoundaryInterpolation )
        psubd->subd().boundary().get();
    switch ( binterp )
    {
    case kNoBoundaryInterp:
        std::cout << "No Boundary Interp" << std::endl;
        break;

    case kEdgesCornersBoundaryInterp:
        std::cout << "Edges Corners Boundary Interp" << std::endl;
        break;

    case kEdgesBoundaryInterp:
        std::cout << "Edges Boundary Interp" << std::endl;
        break;
    default:
        std::cout << "Unknown Boundary Interp" << std::endl;
        break;
    };

    // Get indices
    Abc::IIntSample indices = psubd->mesh().vertexIndices().get();
    size_t numIndices = indices.size();
    std::cout << "Num indices: " << numIndices << std::endl
              << "Indices: ";
    for ( size_t i = 0; i < numIndices; ++i )
    {
        std::cout << indices[i] << ", ";
    }
    std::cout << std::endl;
}

//-*****************************************************************************
void cameraOut()
{
    Abc::OAsset asset( "cameraOut1.abc", Abc::kThrowException );

    // Camera has animated xform.
    OSimpleCamera camera( asset, "camera",
                          Abc::kThrowException );

    // Add the optional properties we want.
    camera->projection().fov().addOptional( Abc::kThrowException );
    camera->shutter().addOptional( Abc::kThrowException );

    // Set the non-animated stuff.
    Abc::Box3d boundsLocal;
    boundsLocal.makeEmpty();
    camera->bounds().set( boundsLocal );

    Abc::M44d xformLocal;
    xformLocal.setTranslation( Abc::V3d( 0.0, 0.0, 25.0 ) );
    camera->xform().set( xformLocal );

    camera->raster().xResolution().set( 1024 );
    camera->projection().type().set( ( char )kPerspectiveProjection );
    camera->projection().fov()->set( 75.0f );
    camera->projection().
        screenWindow().set( Abc::Box2f(
                                Abc::V2f( -1.85f, -1.0f ),
                                Abc::V2f( 1.85f, 1.0f ) ) );
    camera->projection().clipping().set( Abc::V2f( 1.0e-5f,
                                                   1.0e8f ) );

    // Shutter is in seconds.
    // Use a 144 degree shutter angle, which is pretty wide.
    camera->shutter()->set( Abc::V2f( -0.5f * ( 144.0f / 360.0f ),
                                      0.5f * ( 144.0f / 360.0f ) ) );

    // Set anim xforms.
    Abc::TimeSamplingInfo tsamp( Abc::kUniformTimeSampling,

                                 // First sample seconds
                                 0.0,

                                 // First sample interval
                                 1.0 / 24.0 );
    camera->xform().makeAnimated( tsamp );

    // Set a bunch of samples.
    Abc::V3d target( 0.0, 0.0, 0.0 );
    for ( size_t samp = 0; samp < 48; ++samp )
    {
        Abc::float64_t ytrans = 0.35 * ( Abc::float64_t )samp;
        Abc::V3d eye( 25.0, ytrans, 15.0 );

        Abc::M44d xform =
            Imath::alignZAxisWithTargetDir( eye - target,
                                            Abc::V3d( 0.0, 1.0, 0.0 ) );
        xform.translate( eye );
        camera->xform().setAnim( samp, xform );
    }

    // All done.
    std::cout << "Wrote cameraOut1.abc" << std::endl;
}

//-*****************************************************************************
void cameraIn( void )
{
    Abc::IAsset asset( "cameraOut1.abc" );
    std::cout << "Read asset cameraOut1.abc" << std::endl;

    ISimpleCamera camera( asset, "camera" );
    std::cout << "Read simple camera" << std::endl;

    // X resolution
    std::cout << "Camera Raster X-Resolution: "
              << camera->raster().xResolution().get()
              << std::endl;

    // See if it has pixel aspect ratio
    if ( camera->raster().pixelAspectRatio() )
    {
        std::cout << "Camera has optional pixelAspectRatio: "
                  << camera->raster().pixelAspectRatio().get()
                  << std::endl;
    }
    else
    {
        std::cout << "No pixel aspect ratio." << std::endl;
    }

    // See if it has crop window.
    if ( camera->raster().cropWindow() )
    {
        Abc::Box2f cropWindow = camera->raster().cropWindow().get();
        std::cout << "Camera has optional crop window: "
                  << cropWindow.min << " to " << cropWindow.max
                  << std::endl;
    }
    else
    {
        std::cout << "Camera does not have crop window."
                  << std::endl;
    }

    // Projection type
    CameraProjectionType ptype = ( CameraProjectionType )
        camera->projection().type().get();
    if ( ptype == kPerspectiveProjection )
    {
        std::cout << "Camera is perspective!" << std::endl;
        if ( camera->projection().fov() )
        {
            std::cout << "Camera has fov: "
                      << camera->projection().fov().get()
                      << std::endl;
        }
        else
        {
            std::cout << "Camera has unspecified fov."
                      << std::endl;
        }
    }
    else
    {
        std::cout << "Camera is orthographic!" << std::endl;
        if ( camera->projection().orthographicWidth() )
        {
            std::cout << "Camera has orthographic width: "
                      << camera->projection().orthographicWidth().get()
                      << std::endl;
        }
        else
        {
            std::cout << "Camera has unspecified orthographic width."
                      << std::endl;
        }
    }

    // Get screen window.
    Abc::Box2f screenWindow = camera->projection().screenWindow().get();
    std::cout << "Screen Window: "
              << screenWindow.min << " to " << screenWindow.max
              << std::endl;

    // Get clipping.
    std::cout << "Clipping: "
              << camera->projection().clipping().get() << std::endl;

    // See if it has depth of field
    if ( camera->depthOfField() )
    {
        std::cout << "Camera has depth of field: "
                  << camera->depthOfField().get() << std::endl;
    }
    else
    {
        std::cout << "Camera does not have depth of field."
                  << std::endl;
    }

    // See if it has shutter
    if ( camera->shutter() )
    {
        std::cout << "Camera has shutter: " << camera->shutter().get()
                  << std::endl;
    }
    else
    {
        std::cout << "Camera does not have shutter."
                  << std::endl;
    }

    // Get xform.
    if ( camera->xform().isAnimated() )
    {
        std::cout << "Camera has animated xform." << std::endl;
        const Abc::ITimeSampling *tsamp = camera->xform().timeSampling();
        assert( tsamp != NULL );
        size_t numSamps = tsamp->numSamples();
        for ( size_t samp = 0; samp < numSamps; ++samp )
        {
            Abc::M44d xformAnim = camera->xform().getAnim( samp );
            Abc::seconds_t sampSecs = tsamp->sampleSeconds( samp );
            std::cout << "Sample: " << samp
                      << " Seconds: " << sampSecs
                      << " Xform: " << xformAnim << std::endl;
        }
    }
    else
    {
        std::cout << "Camera xform is not animated." << std::endl;
    }
}

//-*****************************************************************************
void PolyMeshTest( void )
{
    meshOut();
    meshIn();
}

//-*****************************************************************************
void SubdTest( void )
{
    subdOut();
    subdIn();
}

//-*****************************************************************************
void CameraTest( void )
{
    cameraOut();
    cameraIn();
}

} // End namespace TraitsGeom
} // End namespace Alembic


