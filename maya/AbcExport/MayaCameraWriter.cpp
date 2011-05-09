//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include "MayaCameraWriter.h"

MayaCameraWriter::MayaCameraWriter(MDagPath & iDag,
    Alembic::Abc::OObject & iParent, uint32_t iTimeIndex, bool iWriteVisibility,
    bool iForceStatic) :
    mIsAnimated(false),
    mDagPath(iDag),
    mUseRenderShutter(false),
    mShutterOpen(0.0),
    mShutterClose(0.0)
{
    MStatus status = MS::kSuccess;
    MFnCamera cam(iDag, &status);
    if (!status)
    {
        MGlobal::displayError( "MFnCamera() failed for MayaCameraWriter" );
    }

    Alembic::AbcGeom::OCamera obj(iParent, cam.name().asChar(), iTimeIndex);
    mSchema = obj.getSchema();


    MObject cameraObj = iDag.node();
    if (!iForceStatic && util::isAnimated(cameraObj))
        mIsAnimated = true;

    MObject renderObj;
    MSelectionList sel;
    sel.add("defaultRenderGlobals");

    if (!sel.isEmpty())
    {
        sel.getDependNode(0, renderObj);
        MFnDependencyNode dep(renderObj);
        MPlug plug = dep.findPlug("motionBlurUseShutter");
        if (plug.asBool())
        {
            MTime sec(1.0, MTime::kSeconds);
            double val = sec.as(MTime::uiUnit());

            mUseRenderShutter = true;
            plug = dep.findPlug("motionBlurShutterOpen");
            mShutterOpen = plug.asDouble() * val;
            plug = dep.findPlug("motionBlurShutterClose");
            mShutterClose = plug.asDouble() * val;
        }
    }

    Alembic::Abc::OCompoundProperty cp = mSchema.getArbGeomParams();

    mAttrs = AttributesWriterPtr(new AttributesWriter(cp, cam,
        iTimeIndex, iWriteVisibility, iForceStatic));
    write();
}

void MayaCameraWriter::write()
{
    Alembic::AbcGeom::CameraSample samp;
    MFnCamera mfnCamera(mDagPath);

    samp.setFocalLength(mfnCamera.focalLength());
    samp.setLensSqueezeRatio(mfnCamera.lensSqueezeRatio());
    samp.setHorizontalAperture(mfnCamera.horizontalFilmAperture() * 2.54);
    samp.setVerticalAperture(mfnCamera.verticalFilmAperture() * 2.54);
    samp.setHorizontalFilmOffset(mfnCamera.horizontalFilmOffset() * 2.54);
    samp.setVerticalFilmOffset(mfnCamera.verticalFilmOffset() * 2.54);
    double overscan = mfnCamera.overscan() - 1.0;
    samp.setOverScanLeft(overscan);
    samp.setOverScanRight(overscan);
    samp.setOverScanTop(overscan);
    samp.setOverScanBottom(overscan);
    samp.setNearClippingPlane(mfnCamera.nearClippingPlane());
    samp.setFarClippingPlane(mfnCamera.farClippingPlane());
    samp.setFStop(mfnCamera.fStop());
    samp.setFocusDistance(mfnCamera.focusDistance());

    // should this be based on the shutterAngle?  or the settings passed in?

    if (mUseRenderShutter)
    {
        samp.setShutterOpen(mShutterOpen);
        samp.setShutterClose(mShutterClose);
    }
    else
    {
        MTime sec(1.0, MTime::kSeconds);
        samp.setShutterOpen(0.0);
        samp.setShutterClose(
            Alembic::AbcGeom::RadiansToDegrees(mfnCamera.shutterAngle()) / 
            (360.0 * sec.as(MTime::uiUnit())) );
    }
    mSchema.set(samp);
}

bool MayaCameraWriter::isAnimated() const
{
    return  mIsAnimated || (mAttrs != NULL && mAttrs->isAnimated());
}
