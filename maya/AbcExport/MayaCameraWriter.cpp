//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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
    Alembic::Abc::OObject & iParent, Alembic::Util::uint32_t iTimeIndex,
    const JobArgs & iArgs) :
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

    MString name = cam.name();
    name = util::stripNamespaces(name, iArgs.stripNamespace);

    Alembic::AbcGeom::OCamera obj(iParent, name.asChar(), iTimeIndex);
    mSchema = obj.getSchema();

    MObject cameraObj = iDag.node();
    if (iTimeIndex != 0 && util::isAnimated(cameraObj))
    {
        mIsAnimated = true;
    }
    else
    {
        iTimeIndex = 0;
    }

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
            mShutterOpen = plug.asDouble() / val;
            plug = dep.findPlug("motionBlurShutterClose");
            mShutterClose = plug.asDouble() / val;
        }
    }

    Alembic::Abc::OCompoundProperty cp;
    Alembic::Abc::OCompoundProperty up;
    if (AttributesWriter::hasAnyAttr(cam, iArgs))
    {
        cp = mSchema.getArbGeomParams();
        up = mSchema.getUserProperties();
    }

    mAttrs = AttributesWriterPtr(new AttributesWriter(cp, up, obj, cam,
        iTimeIndex, iArgs, true));

    if (!mIsAnimated || iArgs.setFirstAnimShape)
    {
        write();
    }
}

void MayaCameraWriter::write()
{
    MFnCamera mfnCamera(mDagPath);

    mSamp.setFocalLength(mfnCamera.focalLength());
    mSamp.setLensSqueezeRatio(mfnCamera.lensSqueezeRatio());
    mSamp.setHorizontalAperture(mfnCamera.horizontalFilmAperture() * 2.54);
    mSamp.setVerticalAperture(mfnCamera.verticalFilmAperture() * 2.54);
    mSamp.setHorizontalFilmOffset(mfnCamera.horizontalFilmOffset() * 2.54);
    mSamp.setVerticalFilmOffset(mfnCamera.verticalFilmOffset() * 2.54);
    double overscan = mfnCamera.overscan() - 1.0;
    mSamp.setOverScanLeft(overscan);
    mSamp.setOverScanRight(overscan);
    mSamp.setOverScanTop(overscan);
    mSamp.setOverScanBottom(overscan);
    mSamp.setNearClippingPlane(mfnCamera.nearClippingPlane());
    mSamp.setFarClippingPlane(mfnCamera.farClippingPlane());
    mSamp.setFStop(mfnCamera.fStop());
    mSamp.setFocusDistance(mfnCamera.focusDistance());

    // should this be based on the shutterAngle?  or the settings passed in?

    if (mUseRenderShutter)
    {
        mSamp.setShutterOpen(mShutterOpen);
        mSamp.setShutterClose(mShutterClose);
    }
    else
    {
        MTime sec(1.0, MTime::kSeconds);
        mSamp.setShutterOpen(0.0);
        mSamp.setShutterClose(
            Alembic::AbcGeom::RadiansToDegrees(mfnCamera.shutterAngle()) /
            ( 360.0 * sec.as(MTime::uiUnit()) ));
    }

    // build up the film fit and post projection matrix
    if (mSchema.getNumSamples() == 0)
    {
        // film fit first
        std::string filmFitName = "filmFit";
        mFilmFit = mfnCamera.filmFit();
        switch (mFilmFit)
        {
            case MFnCamera::kFillFilmFit:
            {
                Alembic::AbcGeom::FilmBackXformOp fit(
                    Alembic::AbcGeom::kScaleFilmBackOperation, "filmFitFill");
                mSamp.addOp(fit);
            }
            break;

            case MFnCamera::kHorizontalFilmFit:
            {
                Alembic::AbcGeom::FilmBackXformOp fit(
                    Alembic::AbcGeom::kScaleFilmBackOperation, "filmFitHorz");
                mSamp.addOp(fit);

                Alembic::AbcGeom::FilmBackXformOp offset(
                    Alembic::AbcGeom::kTranslateFilmBackOperation,
                    "filmFitOffs");
                mSamp.addOp(offset);
            }
            break;

            case MFnCamera::kVerticalFilmFit:
            {
                Alembic::AbcGeom::FilmBackXformOp fit(
                    Alembic::AbcGeom::kScaleFilmBackOperation, "filmFitVert");
                mSamp.addOp(fit);

                Alembic::AbcGeom::FilmBackXformOp offset(
                    Alembic::AbcGeom::kTranslateFilmBackOperation,
                    "filmFitOffs");
                mSamp.addOp(offset);
            }
            break;

            case MFnCamera::kOverscanFilmFit:
            {
                Alembic::AbcGeom::FilmBackXformOp fit(
                    Alembic::AbcGeom::kScaleFilmBackOperation, "filmFitOver");
                mSamp.addOp(fit);
            }
            break;

            default:
            break;
        }

        Alembic::AbcGeom::FilmBackXformOp preScale(
            Alembic::AbcGeom::kScaleFilmBackOperation, "preScale");
        mSamp.addOp(preScale);

        Alembic::AbcGeom::FilmBackXformOp filmTranslate(
            Alembic::AbcGeom::kTranslateFilmBackOperation, "filmTranslate");
        mSamp.addOp(filmTranslate);

        // skip film roll for now

        Alembic::AbcGeom::FilmBackXformOp postScale(
            Alembic::AbcGeom::kScaleFilmBackOperation, "postScale");
        mSamp.addOp(postScale);

        Alembic::AbcGeom::FilmBackXformOp cameraScale(
            Alembic::AbcGeom::kScaleFilmBackOperation, "cameraScale");
        mSamp.addOp(cameraScale);

    }

    std::size_t filmBackIndex = 0;

    switch (mFilmFit)
    {
        case MFnCamera::kFillFilmFit:
        {
            if (mSamp.getLensSqueezeRatio() > 1.0)
            {
                mSamp[0].setChannelValue(0, 1.0/mSamp.getLensSqueezeRatio());
                mSamp[0].setChannelValue(1, 1.0);
            }
            else
            {
                mSamp[0].setChannelValue(0, 1.0);
                mSamp[0].setChannelValue(1, mSamp.getLensSqueezeRatio());
            }
            filmBackIndex = 1;
        }
        break;

        case MFnCamera::kHorizontalFilmFit:
        {
            if (mSamp.getLensSqueezeRatio() > 1.0)
            {
                mSamp[0].setChannelValue(0, 1.0);
                mSamp[0].setChannelValue(1, mSamp.getLensSqueezeRatio());
                mSamp[1].setChannelValue(0, 0.0);
                mSamp[1].setChannelValue(1, 2.0 *
                    mfnCamera.filmFitOffset()/
                    mfnCamera.horizontalFilmAperture() );
            }
            else
            {
                mSamp[0].setChannelValue(0, 1.0);
                mSamp[0].setChannelValue(1, 1.0);
                mSamp[1].setChannelValue(0, 0.0);
                mSamp[1].setChannelValue(1, 0.0);
            }
            filmBackIndex = 2;
        }
        break;

        case MFnCamera::kVerticalFilmFit:
        {
            if (1.0/mSamp.getLensSqueezeRatio() > 1.0)
            {
                mSamp[0].setChannelValue(0, 1.0/mSamp.getLensSqueezeRatio());
                mSamp[0].setChannelValue(1, 1.0);
                mSamp[1].setChannelValue(0, 2.0 *
                    mfnCamera.filmFitOffset() /
                    mfnCamera.horizontalFilmAperture() );
                mSamp[1].setChannelValue(1, 0.0);
            }
            else
            {
                mSamp[0].setChannelValue(0, 1.0);
                mSamp[0].setChannelValue(1, 1.0);
                mSamp[1].setChannelValue(0, 0.0);
                mSamp[1].setChannelValue(1, 0.0);
            }
            filmBackIndex = 2;
        }
        break;

        case MFnCamera::kOverscanFilmFit:
        {
            if (mSamp.getLensSqueezeRatio() < 1.0)
            {
                mSamp[0].setChannelValue(0, 1.0);
                mSamp[0].setChannelValue(1, mSamp.getLensSqueezeRatio());
            }
            else
            {
                mSamp[0].setChannelValue(0, 1.0/mSamp.getLensSqueezeRatio());
                mSamp[0].setChannelValue(1, 1.0);
            }
            filmBackIndex = 1;
        }
        break;

        default:
        break;
    }

    mSamp[filmBackIndex].setChannelValue(0, 1.0/mfnCamera.preScale());
    mSamp[filmBackIndex].setChannelValue(1, 1.0/mfnCamera.preScale());

    mSamp[filmBackIndex+1].setChannelValue(0, mfnCamera.filmTranslateH());
    mSamp[filmBackIndex+1].setChannelValue(1, mfnCamera.filmTranslateV());

    mSamp[filmBackIndex+2].setChannelValue(0, 1.0/mfnCamera.postScale());
    mSamp[filmBackIndex+2].setChannelValue(1, 1.0/mfnCamera.postScale());

    mSamp[filmBackIndex+3].setChannelValue(0, mfnCamera.cameraScale());
    mSamp[filmBackIndex+3].setChannelValue(1, mfnCamera.cameraScale());

    mSchema.set(mSamp);
}

bool MayaCameraWriter::isAnimated() const
{
    return  mIsAnimated || (mAttrs != NULL && mAttrs->isAnimated());
}
