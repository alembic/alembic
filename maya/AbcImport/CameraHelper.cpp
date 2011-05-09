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

#include <maya/MFnCamera.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MTime.h>
#include <maya/MVector.h>
#include <maya/MDagModifier.h>

#include "util.h"
#include "CameraHelper.h"
#include "NodeIteratorVisitorHelper.h"

void read(double iFrame, Alembic::AbcGeom::ICamera & iCamera,
    std::vector<double> & oArray)
{
    Alembic::AbcGeom::ICameraSchema schema = iCamera.getSchema();

    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame,
        schema.getTimeSampling(), schema.getNumSamples(), index, ceilIndex);

    if (alpha != 0.0)
    {
        Alembic::AbcGeom::CameraSample samp, ceilSamp;
        schema.get(samp, index);
        schema.get(ceilSamp, ceilIndex);

        oArray.push_back( simpleLerp<double>(alpha, samp.getFocalLength(),
            ceilSamp.getFocalLength()) );
        oArray.push_back( simpleLerp<double>(alpha, samp.getLensSqueezeRatio(),
            ceilSamp.getLensSqueezeRatio()) );
        oArray.push_back(simpleLerp<double>(alpha,
            samp.getHorizontalAperture()/2.54,
            ceilSamp.getHorizontalAperture()/2.54));
        oArray.push_back(simpleLerp<double>(alpha,
            samp.getVerticalAperture()/2.54,
            ceilSamp.getVerticalAperture()/2.54));
        oArray.push_back(simpleLerp<double>(alpha,
            samp.getHorizontalFilmOffset()/2.54,
            ceilSamp.getHorizontalFilmOffset()/2.54));
        oArray.push_back(simpleLerp<double>(alpha,
            samp.getVerticalFilmOffset()/2.54,
            ceilSamp.getVerticalFilmOffset()/2.54));

        if (samp.getOverScanLeft() == samp.getOverScanRight() &&
            samp.getOverScanTop() == samp.getOverScanBottom() &&
            samp.getOverScanLeft() == samp.getOverScanTop() &&
            ceilSamp.getOverScanLeft() == ceilSamp.getOverScanRight() &&
            ceilSamp.getOverScanTop() == ceilSamp.getOverScanBottom() &&
            ceilSamp.getOverScanLeft() == ceilSamp.getOverScanTop())
        {
            oArray.push_back(simpleLerp<double>(alpha,
                samp.getOverScanLeft() + 1.0,
                ceilSamp.getOverScanLeft() + 1.0));
        }
        else
        {
            oArray.push_back(1.0);
        }


        oArray.push_back( simpleLerp<double>(alpha,
            samp.getNearClippingPlane(),
            ceilSamp.getNearClippingPlane()) );
        oArray.push_back( simpleLerp<double>(alpha,
            samp.getFarClippingPlane(),
            ceilSamp.getFarClippingPlane()) );

        oArray.push_back( simpleLerp<double>(alpha, samp.getFStop(),
            ceilSamp.getFStop()) );
        oArray.push_back( simpleLerp<double>(alpha, samp.getFocusDistance(),
            ceilSamp.getFocusDistance()) );

        double shutterClose = simpleLerp<double>(alpha, samp.getShutterClose(),
            ceilSamp.getShutterClose());
        double shutterOpen = simpleLerp<double>(alpha, samp.getShutterOpen(),
            ceilSamp.getShutterOpen());
        MTime sec(1.0, MTime::kSeconds);
        oArray.push_back(Alembic::AbcGeom::DegreesToRadians(
            360.0 * (shutterClose - shutterOpen) /
            sec.as(MTime::uiUnit()) ));
    }
    else
    {
        Alembic::AbcGeom::CameraSample samp;
        schema.get(samp, index);
        oArray.push_back(samp.getFocalLength());
        oArray.push_back(samp.getLensSqueezeRatio());
        oArray.push_back(samp.getHorizontalAperture()/2.54);
        oArray.push_back(samp.getVerticalAperture()/2.54);
        oArray.push_back(samp.getHorizontalFilmOffset()/2.54);
        oArray.push_back(samp.getVerticalFilmOffset()/2.54);

        if (samp.getOverScanLeft() == samp.getOverScanRight() &&
            samp.getOverScanTop() == samp.getOverScanBottom() &&
            samp.getOverScanLeft() == samp.getOverScanTop())
        {
            oArray.push_back(samp.getOverScanLeft() + 1.0);
        }
        else
        {
            oArray.push_back(1.0);
        }


        oArray.push_back(samp.getNearClippingPlane());
        oArray.push_back(samp.getFarClippingPlane());

        oArray.push_back(samp.getFStop());
        oArray.push_back(samp.getFocusDistance());

        MTime sec(1.0, MTime::kSeconds);
        oArray.push_back(Alembic::AbcGeom::DegreesToRadians(
            360.0 * (samp.getShutterClose()-samp.getShutterOpen()) /
            sec.as(MTime::uiUnit()) ));

        // prescale, film translate H, V, roll pivot H,V, film roll value
        // post scale might be in the 3x3
    }


}

MObject create(Alembic::AbcGeom::ICamera & iNode, MObject & iParent)
{
    Alembic::AbcGeom::ICameraSchema schema = iNode.getSchema();
    MString name(iNode.getName().c_str());

    MFnCamera fnCamera;
    MObject obj = fnCamera.create(iParent);
    fnCamera.setName(name);

    if (schema.getNumSamples() == 1)
    {
        Alembic::AbcGeom::CameraSample samp;
        iNode.getSchema().get(samp);

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
/*
    MFnDagNode fnDag(iParent);

    // reparent the cameraShape
    MString cmd("parent -add -shape ");
    cmd += path.partialPathName();
    cmd += " ";
    cmd += fnDag.partialPathName();

    cmd += ";\n";
    // delete the extra transform node
    MFnDagNode fnDag1(obj);
    cmd += "delete ";
    cmd += fnDag1.partialPathName();
    cmd += ";";
    MGlobal::executeCommand(cmd, true);
*/

    return camObj;
}
