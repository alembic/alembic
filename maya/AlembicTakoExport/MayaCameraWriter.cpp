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

#include <AlembicTakoExport/MayaCameraWriter.h>
#include <AlembicTakoExport/MayaUtility.h>

namespace Alembic {

namespace {

void fillCameraData(MFnCamera & mfnCamera, Tako::CameraData & camera)
{
    // fill in the camera data
    camera.orthographic = mfnCamera.isOrtho();
    camera.tumblePivotLocal = mfnCamera.usePivotAsLocalSpace();
    camera.centerOfInterest = mfnCamera.centerOfInterest();
    camera.focalLength = mfnCamera.focalLength();
    camera.lensSqueezeRatio = mfnCamera.lensSqueezeRatio();
    camera.cameraScale = mfnCamera.cameraScale();
    camera.horizontalFilmAperture = mfnCamera.horizontalFilmAperture() * 2.54;
    camera.verticalFilmAperture = mfnCamera.verticalFilmAperture() * 2.54;
    camera.horizontalFilmOffset = mfnCamera.horizontalFilmOffset() * 2.54;
    camera.verticalFilmOffset = mfnCamera.verticalFilmOffset() * 2.54;
    MFnCamera::FilmFit f = mfnCamera.filmFit();
    if (f == MFnCamera::kFillFilmFit)
        camera.filmFit = Tako::FILL;
    else if (f == MFnCamera::kHorizontalFilmFit)
        camera.filmFit = Tako::HORIZONTAL;
    else if (f == MFnCamera::kVerticalFilmFit)
        camera.filmFit = Tako::VERTICAL;
    else if (f == MFnCamera::kOverscanFilmFit)
        camera.filmFit = Tako::OVERSCAN;
    camera.filmFitOffset = mfnCamera.filmFitOffset() * 2.54;
    camera.overscan = mfnCamera.overscan();
    camera.nearClipPlane = mfnCamera.nearClippingPlane();
    camera.farClipPlane = mfnCamera.farClippingPlane();
    camera.preScale = mfnCamera.preScale();
    camera.filmTranslateH = mfnCamera.filmTranslateH();
    camera.filmTranslateV = mfnCamera.filmTranslateV();
    camera.horizontalRollPivot = mfnCamera.horizontalRollPivot();
    camera.verticalRollPivot = mfnCamera.verticalRollPivot();
    camera.filmRollValue = mfnCamera.filmRollValue();
    if (mfnCamera.filmRollOrder() == MFnCamera::kRotateTranslate)
        camera.filmRollOrder = 0;
    else
        camera.filmRollOrder = 1;
    camera.postScale = mfnCamera.postScale();

    // Mayas docs said the ortho width was in inches, but tests indicate that
    // it is in centimeters
    camera.orthographicWidth = mfnCamera.orthoWidth();
    camera.fstop = mfnCamera.fStop();
    camera.focusDistance = mfnCamera.focusDistance();
    camera.shutterAngle = mfnCamera.shutterAngle();

    MPoint tublePivot = mfnCamera.tumblePivot();
    camera.tumblePivotX = tublePivot.x;
    camera.tumblePivotY = tublePivot.y;
    camera.tumblePivotZ = tublePivot.z;
}
}

MayaCameraWriter::MayaCameraWriter(
    float iFrame, MDagPath & iDag, TransformWriterPtr iParent,
    bool iWriteVisibility) :
    mIsAnimated(false), mDagPath(iDag)
{
    MStatus stat;
    MFnCamera mfnCamera(mDagPath);
    mWriter = CameraWriterPtr(new Tako::CameraWriter(mfnCamera.name().asChar(),
        *iParent));
    mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, *mWriter,
        mfnCamera, iWriteVisibility));

    MObject cameraObj = iDag.node();
    if (util::isAnimated(cameraObj))
        mIsAnimated = true;
    else
        iFrame = FLT_MAX;

    Tako::CameraData camera;
    fillCameraData(mfnCamera, camera);
    mWriter->write(iFrame, camera);
}

void MayaCameraWriter::write(float iFrame)
{
    Tako::CameraData camera;
    MFnCamera mfnCamera(mDagPath);
    fillCameraData(mfnCamera, camera);
    mWriter->write(iFrame, camera);

    mAttrs->write(iFrame);
}


bool MayaCameraWriter::isAnimated() const
{
    return  mIsAnimated || (mAttrs != NULL && mAttrs->isAnimated());
}

} // End namespace Alembic

