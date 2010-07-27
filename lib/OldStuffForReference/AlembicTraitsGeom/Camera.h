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

#ifndef _AlembicTraitsGeom_Camera_h_
#define _AlembicTraitsGeom_Camera_h_

#include <AlembicTraitsGeom/Foundation.h>
#include <AlembicTraitsGeom/Xform.h>

namespace AlembicTraitsGeom {

//-*****************************************************************************
// We want to specify a basic camera that contains all the necessary
// information required to create a CG camera that correctly reproduces
// a projection, along with depth of field & stereo information.
// We don't want to attempt to store every field that might reasonably be
// associated with a camera - though those can obviously be added as additional
// properties as needed.
//
// We'll basically copy the Renderman Camera specification.
// TODO: Add Stereo Parameters
//
// int   xResolution()       : NON-ANIMATEABLE
//                             number of pixels corresponding to the requested
//                             crop window in the requested projection.
//                
// float pixelAspectRatio()  : OPTIONAL, NON-ANIMATEABLE
//                             ratio of pixel width to pixel height.
// 
// box2f cropWindow()        : OPTIONAL, NON-ANIMATEABLE
//                             region of the raster that is actually rendered.
//                             Use me to achieve oversized renders!
//
// box2f screenWindow()      : NON-ANIMATEABLE
//                             region of the screen (plane after projection)
//                             to be considered as the raster plane.
//                             This implicitly contains raster aspect ratio.
//
// char  projection()        : NON-ANIMATEABLE
//                             (enum, kPerspectiveProjection,
//                                    kOrthographicProjection)
// 
// float xFov()              : OPTIONAL, ANIMATEABLE
//                             if perspective, the field of view, in DEGREES,
//                             of the full angle from -1 screen to +1 screen.
//
// float orthographicWidth() : OPTIONAL, ANIMATEABLE
//                             if orthographic, the width of the projection.
//                
// m44d  xform()             : ANIMATEABLE
//                             camera local space (right handed) to world space.
//                             This differs from prman, which assumes default
//                             camera to be left-handed.
//
// v2f   clip()              : ANIMATEABLE
//                             (near,far) - positive values.
//
// v3f   depthOfField()      : OPTIONAL, ANIMATEABLE
//                             (fstop, focalLength focalDistance)
//
// v2f   shutter()           : OPTIONAL, ANIMATEABLE
//                             (open,close) - 
//-*****************************************************************************

//-*****************************************************************************
// Enums
enum CameraProjectionType
{
    kPerspectiveProjection,
    kOrthographicProjection
};

//-*****************************************************************************
// String Traits - for string literals that differentiate types.
ALEMBIC_STRING_TRAIT( SimpleCameraProtocolStrait,
                      "AlembicSimpleCamera_v0001" );

ALEMBIC_STRING_TRAIT( XResolutionStrait, "camera|xResolution" );
ALEMBIC_STRING_TRAIT( PixelAspectRatioStrait, "camera|pixelAspectRatio" );
ALEMBIC_STRING_TRAIT( CropWindowStrait, "camera|cropWindow" );
ALEMBIC_STRING_TRAIT( ScreenWindowStrait, "camera|screenWindow" );
ALEMBIC_STRING_TRAIT( ProjectionStrait, "camera|projection" );
ALEMBIC_STRING_TRAIT( FovStrait, "camera|fov" );
ALEMBIC_STRING_TRAIT( OrthographicWidthStrait, "camera|orthographicWidth" );
ALEMBIC_STRING_TRAIT( ClippingStrait, "camera|clipping" );
ALEMBIC_STRING_TRAIT( DepthOfFieldStrait, "camera|depthOfField" );
ALEMBIC_STRING_TRAIT( ShutterStrait, "camera|shutter" );

//-*****************************************************************************
// Property Traits - creating a type that associates datatype, name,
// and optional/animateable status.

ALEMBIC_NONANIM_PROPERTY_TRAIT(         XResolutionTrait,
                                        UintProperty,
                                        XResolutionStrait );

ALEMBIC_OPT_NONANIM_PROPERTY_TRAIT(     PixelAspectRatioTrait,
                                        FloatProperty,
                                        PixelAspectRatioStrait );

ALEMBIC_OPT_NONANIM_PROPERTY_TRAIT(     CropWindowTrait,
                                        Box2fProperty,
                                        CropWindowStrait );

ALEMBIC_NONANIM_PROPERTY_TRAIT(         ScreenWindowTrait,
                                        Box2fProperty,
                                        ScreenWindowStrait );

ALEMBIC_NONANIM_PROPERTY_TRAIT(         ProjectionTrait,
                                        CharProperty,
                                        ProjectionStrait );

ALEMBIC_OPT_NONANIM_PROPERTY_TRAIT(     FovTrait,
                                        FloatProperty,
                                        FovStrait );

ALEMBIC_OPT_NONANIM_PROPERTY_TRAIT(     OrthographicWidthTrait,
                                        FloatProperty,
                                        OrthographicWidthStrait );

ALEMBIC_PROPERTY_TRAIT(                 ClippingTrait,
                                        V2fProperty,
                                        ClippingStrait );

ALEMBIC_OPT_PROPERTY_TRAIT(             DepthOfFieldTrait,
                                        V3fProperty,
                                        DepthOfFieldStrait );

ALEMBIC_OPT_PROPERTY_TRAIT(             ShutterTrait,
                                        V2fProperty,
                                        ShutterStrait );

//-*****************************************************************************
// Amalgams - combining traits.

// Traits that are primarily concerned with the raster are combined.
ALEMBIC_AMALGAM_TRAIT3( CameraRasterTrait,
                        XResolutionTrait, xResolution,
                        PixelAspectRatioTrait, pixelAspectRatio,
                        CropWindowTrait, cropWindow );

// Traits that are primarily concerned with the projection are combined.
ALEMBIC_AMALGAM_TRAIT5( CameraProjectionTrait,
                        ProjectionTrait, type,
                        FovTrait, fov,
                        OrthographicWidthTrait, orthographicWidth,
                        ScreenWindowTrait, screenWindow,
                        ClippingTrait, clipping );

// Combine camera stuff into Camera Trait.
ALEMBIC_AMALGAM_TRAIT6( SimpleCameraTrait,
                        // Even though the camera occupies no space itself,
                        // It could have geometric children, so we give it
                        // bounds to encompass them.
                        BoundsLocalTrait, bounds,
                        XformLocalTrait, xform,
                        CameraRasterTrait, raster,
                        CameraProjectionTrait, projection,
                        DepthOfFieldTrait, depthOfField,
                        ShutterTrait, shutter );

//-*****************************************************************************
// Trait objects
ALEMBIC_TRAIT_OBJECT( SimpleCamera, SimpleCameraTrait,
                      SimpleCameraProtocolStrait );

//-*****************************************************************************
// OUTPUT USAGE
//
// OSimpleCamera camera( ... );
// camera->bounds().set( ... );
// camera->xform().set( ... );
// 
// camera->raster().xResolution().set( ... );
// camera->raster().pixelAspectRatio().addOptional();
//     camera->raster().pixelAspectRatio()->set( ... );
// camera->raster().cropWindow().addOptional();
//     camera->raster().cropWindow()->set( ... );
//
// camera->projection().type().set( ( char )kPerspectiveProjection );
// camera->projection().fov().addOptional( ... );
//     camera->projection().fov()->set( ... );
// camera->projection().orthographicWidth().addOptional( ... );
//     camera->orthographicWidth().orthographicWidth()->set( ... );
// camera->projection().screenWindow().set( ... );
// camera->projection().clipping().set( ... );
//
// camera->depthOfField().addOptional( ... );
//     camera->depthOfField()->set( ... );
// camera->shutter().addOptional( ... );
//     camera->shutter()->set( ... );
//-*****************************************************************************


} // End namespace AlembicTraitsGeom

#endif
