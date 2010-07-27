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

#ifndef _Alembic_Tako_CameraDef_h_
#define _Alembic_Tako_CameraDef_h_

#include <Alembic/Tako/Foundation.h>

namespace Alembic {

// In the camera's local space, the camera is positioned at (0, 0, 0),
// its center of interest at (0, 0, -1), which implies that the view-direction
// is pointing in the direction of the negative z-axis, and its up-direction
// along the positive Y axis.
//
// The lookAt and up vectors are changed by it's parent transform node

namespace Tako {

namespace TAKO_LIB_VERSION_NS {

/**
\brief Used to describe the relationship between the digital image and the
film back.
*/
typedef enum
{
    /**
    Indicates that whichever fit, horizontal or vertical, makes the digital
    image larger than the film back is applied.
    */
    FILL = 0,

    /**
    The digital image fits the film back along the horizontal direction so
    that the width is matched.  The height does not have to match.
    */
    HORIZONTAL,

    /**
    The digital image fits the film back along the vertical direction so
    that the height is matched.  The width does not have to match.
    */
    VERTICAL,

    /**
    The film gate is over scanned.
    */
    OVERSCAN
} FilmFitType;

/**
\brief Data container for all of the camera information.
*/
class CameraData
{
    public:

    /** Constructor which sets up the camera with some simple defaults.*/
    CameraData() :
        orthographic(0),
        centerOfInterest(5.0),
        focalLength(35.0),
        lensSqueezeRatio(1.0),
        cameraScale(1.0),
        horizontalFilmAperture(3.6),
        verticalFilmAperture(2.4),
        horizontalFilmOffset(0.0),
        verticalFilmOffset(0.0),
        filmFit(HORIZONTAL),
        filmFitOffset(0.0),
        overscan(1.0),
        nearClipPlane(0.01),
        farClipPlane(1000.0),
        preScale(1.0),
        filmTranslateH(0.0),        filmTranslateV(0.0),
        horizontalRollPivot(0.0),   verticalRollPivot(0.0),
        filmRollValue(0.0),
        filmRollOrder(0),
        postScale(1.0),
        orthographicWidth(10.0),
        fstop(5.6),
        focusDistance(5.0),
        shutterAngle(3.141592654),
        tumblePivotX(0.0), tumblePivotY(0.0), tumblePivotZ(0.0),
        tumblePivotLocal(0)
        {}

    /** Destructor */
    ~CameraData() {}

    /** If this value is true then we have an orthographic camera, otherwise
    we have a perspective camera.*/
    bool orthographic;

    // Camera Properties

    /**
    The center of interest (positive) distance along the view direction.
    */
    double centerOfInterest;

    // Lens Properties

    /**
    The distance between the lens and the film plane in millimeters.
    (when the focal distance is "infinite") Note: focal length is specified
    in millimeters here because of photographic convention.  For more
    information consult your local library or
    http://en.wikipedia.org/wiki/Focal_length
    */
    double focalLength;

    /**
    The width/height lens aspect ratio.
    */
    double lensSqueezeRatio;

    /**
    Camera Scale value.
    */
    double cameraScale;

    // Film Back Properties

    /**
    Horizontal size of the camera aperture in centimeters.
    */
    double horizontalFilmAperture;

    /**
    Vertical size of the camera aperture in centimeters.
    */
    double verticalFilmAperture;

    /**
    Horizontal offset distance of the film in centimeters.
    */
    double horizontalFilmOffset;

    /**
    Vertical offset distance of the film in centimeters.
    */
    double verticalFilmOffset;

    /**
    Specifies how the digital image is matched to the film back.
    */
    FilmFitType filmFit;

    /**
    An offset to line up the image with parts of the film back in centimeters.
    */
    double filmFitOffset;

    /**
    Scaling value that represents extra area around the image.
    */
    double overscan;

    // Clipping Planes

    /**
    Distance to the near clipping plane in centimeters
    */
    double nearClipPlane;

    /**
    Distance to the far clipping plane in centimeters
    */
    double farClipPlane;

    /**
    The post projection matrix's pre-scale value.
    */
    double preScale;

    /**
    The horizontal film translation for the normalized viewport.
    */
    double filmTranslateH;

    /**
    The vertical film translation for the normalized viewport.
    */
    double filmTranslateV;

    /**
    Unknown
    */
    double horizontalRollPivot;

    /**
    Unknown
    */
    double verticalRollPivot;

    /**
    Film back roll angle in radians
    */
    double filmRollValue;

    /**
    If true the film back translation is applied before the film back rotation.
    If false the film back rotation is applied first, then the film back
    translation.
    */
    bool filmRollOrder;  // 0 kRotateTranslate, 1 kTranslateRotate

    /**
    The post projection matrix's post-scale value.
    */
    double postScale;

    /**
    The orthographic width of the camera in centimeters.
    */
    double orthographicWidth;

    /**
    Optical property of the lens.  Focal length divided by "effective" lens
    diameter.
    */
    double fstop;

    /**
    Distance from the camera to the object being focused on in centimeters.
    */
    double focusDistance;

    // Not sure if needed

    /**
    The angle of the shutter in radians  which controls the proportion
    of the time that the film is exposed to light for each frame.
    */
    double shutterAngle;

    // need to add in next version of the API determines if the tumble pivot
    // point is in local space or world space see usePivotAsLocalSpace
    // bool tumblePivotLocal

    /**
    X component from the tumble pivot point???
    */
    double tumblePivotX;

    /**
    Y component from the tumble pivot point???
    */
    double tumblePivotY;

    /**
    Z component from the tumble pivot point???
    */
    double tumblePivotZ;

    /**
    Whether the tumble pivot is done in local space???
    */
    bool tumblePivotLocal;
};
}

using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_CameraDef_h_
