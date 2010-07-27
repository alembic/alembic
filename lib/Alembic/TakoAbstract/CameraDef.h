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

#ifndef _Alembic_TakoAbstract_CameraDef_h_
#define _Alembic_TakoAbstract_CameraDef_h_

#include <Alembic/TakoAbstract/Foundation.h>

namespace Alembic {

namespace TakoAbstract {

namespace TAKO_ABSTRACT_LIB_VERSION_NS {

//-*****************************************************************************
enum FilmFitType
{
    FILL = 0,
    HORIZONTAL,
    VERTICAL,
    OVERSCAN
};

//-*****************************************************************************
struct CameraData
{
    CameraData()
      : orthographic(0),
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

    bool orthographic;
    double centerOfInterest;
    double focalLength;
    double lensSqueezeRatio;
    double cameraScale;
    double horizontalFilmAperture;
    double verticalFilmAperture;
    double horizontalFilmOffset;
    double verticalFilmOffset;
    FilmFitType filmFit;
    double filmFitOffset;
    double overscan;
    double nearClipPlane;
    double farClipPlane;
    double preScale;
    double filmTranslateH;
    double filmTranslateV;
    double horizontalRollPivot;
    double verticalRollPivot;
    double filmRollValue;
    bool filmRollOrder;  // 0 kRotateTranslate, 1 kTranslateRotate
    double postScale;
    double orthographicWidth;
    double fstop;
    double focusDistance;
    double shutterAngle;
    double tumblePivotX;
    double tumblePivotY;
    double tumblePivotZ;
    bool tumblePivotLocal;
};

} // End namespace TAKO_ABSTRACT_LIB_VERSION_NS

} // End namespace TakoAbstract

} // End namespace Alembic

#endif  // _Alembic_TakoAbstract_CameraDef_h_
