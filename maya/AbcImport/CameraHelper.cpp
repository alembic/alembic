//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include "util.h"
#include "CameraHelper.h"
#include "NodeIteratorVisitorHelper.h"

#include <maya/MFnCamera.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MTime.h>
#include <maya/MVector.h>
#include <maya/MDagModifier.h>

void read(double iFrame, Alembic::AbcGeom::ICamera & iCamera,
    std::vector<double> & oArray)
{
    oArray.resize(18);

    // set some optional scale values
    oArray[13] = 1.0;
    oArray[16] = 1.0;
    oArray[17] = 1.0;

    Alembic::AbcGeom::ICameraSchema schema = iCamera.getSchema();

    Alembic::AbcCoreAbstract::index_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame,
        schema.getTimeSampling(), schema.getNumSamples(), index, ceilIndex);

    if (alpha != 0.0)
    {
        Alembic::AbcGeom::CameraSample samp, ceilSamp;
        schema.get(samp, index);
        schema.get(ceilSamp, ceilIndex);

        oArray[0] = simpleLerp<double>(alpha, samp.getFocalLength(),
            ceilSamp.getFocalLength());
        oArray[1] = simpleLerp<double>(alpha, samp.getLensSqueezeRatio(),
            ceilSamp.getLensSqueezeRatio());
        oArray[2] = simpleLerp<double>(alpha, samp.getHorizontalAperture(),
            ceilSamp.getHorizontalAperture()) / 2.54;
        oArray[3] = simpleLerp<double>(alpha, samp.getVerticalAperture(),
            ceilSamp.getVerticalAperture()) / 2.54;
        oArray[4] = simpleLerp<double>(alpha,
            samp.getHorizontalFilmOffset(),
            ceilSamp.getHorizontalFilmOffset()) / 2.54;
        oArray[5] = simpleLerp<double>(alpha,
            samp.getVerticalFilmOffset(),
            ceilSamp.getVerticalFilmOffset()) / 2.54;

        if (samp.getOverScanLeft() == samp.getOverScanRight() &&
            samp.getOverScanTop() == samp.getOverScanBottom() &&
            samp.getOverScanLeft() == samp.getOverScanTop() &&
            ceilSamp.getOverScanLeft() == ceilSamp.getOverScanRight() &&
            ceilSamp.getOverScanTop() == ceilSamp.getOverScanBottom() &&
            ceilSamp.getOverScanLeft() == ceilSamp.getOverScanTop())
        {
            oArray[6] = simpleLerp<double>(alpha,
                samp.getOverScanLeft() + 1.0,
                ceilSamp.getOverScanLeft() + 1.0);
        }
        else
        {
            oArray[6] = 1.0;
        }

        oArray[7] = simpleLerp<double>(alpha, samp.getNearClippingPlane(),
            ceilSamp.getNearClippingPlane());

        oArray[8] = simpleLerp<double>(alpha, samp.getFarClippingPlane(),
            ceilSamp.getFarClippingPlane());

        oArray[9] = simpleLerp<double>(alpha, samp.getFStop(),
            ceilSamp.getFStop());

        oArray[10] = simpleLerp<double>(alpha, samp.getFocusDistance(),
            ceilSamp.getFocusDistance());

        double shutterClose = simpleLerp<double>(alpha, samp.getShutterClose(),
            ceilSamp.getShutterClose());
        double shutterOpen = simpleLerp<double>(alpha, samp.getShutterOpen(),
            ceilSamp.getShutterOpen());
        MTime sec(1.0, MTime::kSeconds);
        oArray[11] =
            360.0 * (shutterClose - shutterOpen) * sec.as(MTime::uiUnit());

        std::size_t numOps = samp.getNumOps();
        for (std::size_t i = 0; i < numOps; ++i)
        {
            Alembic::AbcGeom::FilmBackXformOp & op = samp[i];
            Alembic::AbcGeom::FilmBackXformOp & ceilOp = ceilSamp[i];
            if (op.getHint() == "filmFitOffs")
            {
                double val = op.getChannelValue(0) *
                    samp.getHorizontalAperture();

                double ceilVal = ceilOp.getChannelValue(0) *
                    ceilSamp.getHorizontalAperture();

                if (val != 0.0)
                {
                    // chanValue(0) * 0.5 * horiz aper / 2.54
                    oArray[12] = simpleLerp<double>(alpha, val, ceilVal) / 5.08;
                }
                else
                {
                    val = op.getChannelValue(1) * samp.getHorizontalAperture();

                    ceilVal = ceilOp.getChannelValue(1) *
                        ceilSamp.getHorizontalAperture();

                    // chanValue(1)* 0.5 * horiz aper / 2.54
                    oArray[12] = simpleLerp<double>(alpha, val, ceilVal) / 5.08;
                }
            }
            else if (op.getHint() == "preScale")
            {
                oArray[13] = 1.0 / simpleLerp<double>(alpha,
                    op.getChannelValue(0), ceilOp.getChannelValue(0));
            }
            else if (op.getHint() == "filmTranslate")
            {
                oArray[14] = simpleLerp<double>(alpha,
                    op.getChannelValue(0), ceilOp.getChannelValue(0));

                oArray[15] = simpleLerp<double>(alpha,
                    op.getChannelValue(1), ceilOp.getChannelValue(1));
            }
            else if (op.getHint() == "postScale")
            {
                oArray[16] = 1.0 / simpleLerp<double>(alpha,
                    op.getChannelValue(0), ceilOp.getChannelValue(0));
            }
            else if (op.getHint() == "cameraScale")
            {
                oArray[17] = simpleLerp<double>(alpha,
                    op.getChannelValue(0), ceilOp.getChannelValue(0));
            }
        }
    }
    else
    {
        Alembic::AbcGeom::CameraSample samp;
        schema.get(samp, index);
        oArray[0] = samp.getFocalLength();
        oArray[1] = samp.getLensSqueezeRatio();
        oArray[2] = samp.getHorizontalAperture()/2.54;
        oArray[3] = samp.getVerticalAperture()/2.54;
        oArray[4] = samp.getHorizontalFilmOffset()/2.54;
        oArray[5] = samp.getVerticalFilmOffset()/2.54;

        if (samp.getOverScanLeft() == samp.getOverScanRight() &&
            samp.getOverScanTop() == samp.getOverScanBottom() &&
            samp.getOverScanLeft() == samp.getOverScanTop())
        {
            oArray[6] = samp.getOverScanLeft() + 1.0;
        }
        else
        {
            oArray[6] = 1.0;
        }

        oArray[7] = samp.getNearClippingPlane();
        oArray[8] = samp.getFarClippingPlane();

        oArray[9] = samp.getFStop();
        oArray[10] = samp.getFocusDistance();

        MTime sec(1.0, MTime::kSeconds);
        oArray[11] = 360.0 * (samp.getShutterClose()-samp.getShutterOpen()) *
            sec.as(MTime::uiUnit());

        // prescale, film translate H, V, roll pivot H,V, film roll value
        // post scale might be in the 3x3
        std::size_t numOps = samp.getNumOps();
        for (std::size_t i = 0; i < numOps; ++i)
        {
            Alembic::AbcGeom::FilmBackXformOp & op = samp[i];
            if (op.getHint() == "filmFitOffs")
            {
                if (op.getChannelValue(0) != 0.0)
                {
                    oArray[12] = op.getChannelValue(0) *
                        samp.getHorizontalAperture() / 5.08;
                }
                else
                {
                    oArray[12] = op.getChannelValue(1) *
                        samp.getHorizontalAperture() / 5.08;
                }
            }
            else if (op.getHint() == "preScale")
            {
                oArray[13] = 1.0 / op.getChannelValue(0);
            }
            else if (op.getHint() == "filmTranslate")
            {
                oArray[14] = op.getChannelValue(0);

                oArray[15] = op.getChannelValue(1);
            }
            else if (op.getHint() == "postScale")
            {
                oArray[16] = 1.0 / op.getChannelValue(0);
            }
            else if (op.getHint() == "cameraScale")
            {
                oArray[17] = op.getChannelValue(0);
            }
        }
    }
}

MObject create(Alembic::AbcGeom::ICamera & iNode, MObject & iParent)
{
    Alembic::AbcGeom::ICameraSchema schema = iNode.getSchema();
    MString name(iNode.getName().c_str());

    MFnCamera fnCamera;
    MObject obj = fnCamera.create(iParent);
    fnCamera.setName(name);

    // we need to read this to determine the film fit
    Alembic::AbcGeom::CameraSample samp;
    iNode.getSchema().get(samp);

    std::size_t numOps = samp.getNumOps();
    if (numOps > 0)
    {
        std::string hint = samp[0].getHint();
        if (hint == "filmFitFill")
        {
            fnCamera.setFilmFit(MFnCamera::kFillFilmFit);
        }
        else if (hint == "filmFitHorz")
        {
            fnCamera.setFilmFit(MFnCamera::kHorizontalFilmFit);
        }
        else if (hint == "filmFitVert")
        {
            fnCamera.setFilmFit(MFnCamera::kVerticalFilmFit);
        }
        else if (hint == "filmFitOver")
        {
            fnCamera.setFilmFit(MFnCamera::kOverscanFilmFit);
        }
    }

    if (schema.isConstant())
    {

        // no center of interest
        fnCamera.setFocalLength(samp.getFocalLength());
        fnCamera.setLensSqueezeRatio(samp.getLensSqueezeRatio());
        // camera scale might be in the 3x3

        // weirdo attrs that are in inches
        fnCamera.setHorizontalFilmAperture(samp.getHorizontalAperture()/2.54);
        fnCamera.setVerticalFilmAperture(samp.getVerticalAperture()/2.54);
        fnCamera.setHorizontalFilmOffset(samp.getHorizontalFilmOffset()/2.54);
        fnCamera.setVerticalFilmOffset(samp.getVerticalFilmOffset()/2.54);

        // film fit offset might be in the 3x3

        if (samp.getOverScanLeft() == samp.getOverScanRight() &&
            samp.getOverScanTop() == samp.getOverScanBottom() &&
            samp.getOverScanLeft() == samp.getOverScanTop())
        {
            fnCamera.setOverscan(samp.getOverScanLeft() + 1.0);
        }
        else
        {
            MString warn = iNode.getName().c_str();
            warn += " has unsupported overscan values.";
            MGlobal::displayWarning(warn);
        }

        fnCamera.setNearClippingPlane(samp.getNearClippingPlane());
        fnCamera.setFarClippingPlane(samp.getFarClippingPlane());

        // prescale, film translate H, V, roll pivot H,V, film roll value
        // post scale might be in the 3x3

        fnCamera.setFStop(samp.getFStop());
        fnCamera.setFocusDistance(samp.getFocusDistance());

        MTime sec(1.0, MTime::kSeconds);
        fnCamera.setShutterAngle(Alembic::AbcGeom::DegreesToRadians(
            360.0 * (samp.getShutterClose()-samp.getShutterOpen()) *
            sec.as(MTime::uiUnit()) ));

        for (std::size_t i = 0; i < numOps; ++i)
        {
            Alembic::AbcGeom::FilmBackXformOp & op = samp[i];
            if (op.getHint() == "filmFitOffs")
            {
                double val = op.getChannelValue(0) *
                    samp.getHorizontalAperture() / 5.08;

                if (val != 0.0)
                {
                    fnCamera.setFilmFitOffset(val);
                }
                else
                {
                    fnCamera.setFilmFitOffset(op.getChannelValue(1) *
                        samp.getHorizontalAperture() / 5.08);
                }
            }
            else if (op.getHint() == "preScale")
            {
                fnCamera.setPreScale(1.0/op.getChannelValue(0));
            }
            else if (op.getHint() == "filmTranslate")
            {
                fnCamera.setFilmTranslateH(op.getChannelValue(0));
                fnCamera.setFilmTranslateV(op.getChannelValue(1));
            }
            else if (op.getHint() == "postScale")
            {
                fnCamera.setPostScale(1.0/op.getChannelValue(0));
            }
            else if (op.getHint() == "cameraScale")
            {
                fnCamera.setCameraScale(op.getChannelValue(0));
            }
        }
    }

    // extra transform node is unfortuneatly automatically created above the
    // camera, let's do some reparenting and delete that extra transform
    MDagPath path;
    fnCamera.getPath(path);
    MObject camObj = path.node();

    MDagModifier dagMod;
    dagMod.reparentNode(camObj, iParent);
    dagMod.doIt();
    dagMod.deleteNode(obj);
    dagMod.doIt();

    return camObj;
}
