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

#include <AlembicTakoSPIImport/CameraHelper.h>
#include <AlembicTakoSPIImport/Util.h>
#include <AlembicTakoSPIImport/NodeIteratorVisitorHelper.h>

namespace Atk = Alembic::TakoAbstract::TAKO_ABSTRACT_LIB_VERSION_NS;
namespace Alembic {

MStatus read(float iFrame, const TakoSPI::CameraReaderPtr & iNode,
    std::vector<double> & array)
{
    MStatus status;

    iNode->read(iFrame);

    array.reserve(26);
    array.clear();

    const Atk::CameraData & cam = iNode->getCameraData();
    array.push_back(cam.centerOfInterest);
    array.push_back(cam.focalLength);
    array.push_back(cam.lensSqueezeRatio);
    array.push_back(cam.cameraScale);

    // convert from cm back to inches
    array.push_back(cam.horizontalFilmAperture/2.54);
    array.push_back(cam.verticalFilmAperture/2.54);
    array.push_back(cam.horizontalFilmOffset/2.54);
    array.push_back(cam.verticalFilmOffset/2.54);
    array.push_back(cam.filmFitOffset/2.54);

    array.push_back(cam.overscan);
    array.push_back(cam.nearClipPlane);
    array.push_back(cam.farClipPlane);
    array.push_back(cam.preScale);
    array.push_back(cam.filmTranslateH);
    array.push_back(cam.filmTranslateV);
    array.push_back(cam.horizontalRollPivot);
    array.push_back(cam.verticalRollPivot);
    array.push_back(cam.filmRollValue);
    array.push_back(cam.postScale);
    array.push_back(cam.orthographicWidth);
    array.push_back(cam.fstop);
    array.push_back(cam.focusDistance);
    array.push_back(cam.shutterAngle);
    array.push_back(cam.tumblePivotX);
    array.push_back(cam.tumblePivotY);
    array.push_back(cam.tumblePivotZ);

    return status;
}

MStatus create(float iFrame, const TakoSPI::CameraReaderPtr & iNode,
    MObject & iParent, MObject & iObject,
    std::vector<std::string> & iSampledPropNameList)
{
    MStatus status;

    iNode->read(iFrame);

    // Given a parent transform, MFncamera::create() will still create an extra
    // transform node parenting the new cameraShape object
    // Some reparenting work needs to be done
    MFnCamera mFn;
    MObject temp = mFn.create(iParent, &status);
    if (status == MS::kSuccess)
    {
        mFn.setName(iNode->getName().c_str());
        MDagPath path;
        mFn.getPath(path);
        iObject = path.node();
        MFnDagNode fnDag(iParent, &status);
        if (status == MS::kSuccess)
        {
            // addChild works fine in Python test, but here it keeps
            // returning MS::kInvalidParameter error
            status = fnDag.addChild(iObject);
            if (status == MS::kSuccess)
            {
                MGlobal::deleteNode(temp);
            }
            else
            // in case the API failes to do the reparenting task as expected
            {
                 // reparent the cameraShape
                MString cmd("parent -add -shape ");
                cmd += path.partialPathName();
                cmd += " ";
                cmd += fnDag.partialPathName();
                cmd += ";\n";
                // delete the extra transform node
                MFnDagNode fnDag1(temp, &status);
                cmd += "delete ";
                cmd += fnDag1.partialPathName();
                cmd += ";";
                status = MGlobal::executeCommand(cmd, true);
                if (status != MS::kSuccess)
                {
                    MString theError("reparent with Mel command failed.");
                    theError += "MStatus = ";
                    theError += status.errorString();
                    MGlobal::displayError(theError);
                }
            }

            const Atk::CameraData & mCameraData = iNode->getCameraData();

            status = mFn.setHorizontalFilmAperture(
                mCameraData.horizontalFilmAperture/2.54);
            status = mFn.setVerticalFilmAperture(
                mCameraData.verticalFilmAperture/2.54);
            status = mFn.setHorizontalFilmOffset(
                mCameraData.horizontalFilmOffset/2.54);
            status = mFn.setVerticalFilmOffset(
                mCameraData.verticalFilmOffset/2.54);

            mFn.setFilmFitOffset(mCameraData.filmFitOffset/2.54);
            if (mCameraData.filmFit == TakoSPI::FILL)
                status = mFn.setFilmFit(MFnCamera::kFillFilmFit);
            else if (mCameraData.filmFit == TakoSPI::HORIZONTAL)
                status = mFn.setFilmFit(MFnCamera::kHorizontalFilmFit);
            else if (mCameraData.filmFit == TakoSPI::VERTICAL)
                status = mFn.setFilmFit(MFnCamera::kVerticalFilmFit);
            else if (mCameraData.filmFit == TakoSPI::OVERSCAN)
                status = mFn.setFilmFit(MFnCamera::kOverscanFilmFit);

            status = mFn.setOverscan(mCameraData.overscan);
            status = mFn.setHorizontalRollPivot(mCameraData.horizontalRollPivot);
            status = mFn.setVerticalRollPivot(mCameraData.verticalRollPivot);
            status = mFn.setFilmRollValue(mCameraData.filmRollValue);

            if (mCameraData.filmRollOrder == 0)
                status = mFn.setFilmRollOrder(MFnCamera::kRotateTranslate);
            else
                status = mFn.setFilmRollOrder(MFnCamera::kTranslateRotate);

            status = mFn.setPreScale(mCameraData.preScale);
            status = mFn.setPostScale(mCameraData.postScale);
            status = mFn.setFilmTranslateH(mCameraData.filmTranslateH);
            status = mFn.setFilmTranslateV(mCameraData.filmTranslateV);
            status = mFn.setFocalLength(mCameraData.focalLength);
            status = mFn.setLensSqueezeRatio(mCameraData.lensSqueezeRatio);
            status = mFn.setNearFarClippingPlanes(
                mCameraData.nearClipPlane, mCameraData.farClipPlane);
            status = mFn.setFStop(mCameraData.fstop);
            status = mFn.setFocusDistance(mCameraData.focusDistance);
            status = mFn.setShutterAngle(mCameraData.shutterAngle);
            status = mFn.setCenterOfInterest(mCameraData.centerOfInterest);
            status = mFn.setIsOrtho(mCameraData.orthographic);
            status = mFn.setUsePivotAsLocalSpace(mCameraData.tumblePivotLocal);
            status = mFn.setOrthoWidth(mCameraData.orthographicWidth);
            status = mFn.setCameraScale(mCameraData.cameraScale);
            MPoint tumblePivot(mCameraData.tumblePivotX,
                mCameraData.tumblePivotY, mCameraData.tumblePivotZ);
            status = mFn.setTumblePivot(tumblePivot);

            addProperties(iFrame, *iNode, iObject, iSampledPropNameList);
        }
    }

    return status;
}

} // End namespace Alembic

