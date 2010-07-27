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

#include <Alembic/Tako/Tests/CameraTest.h>

#include <Alembic/Tako/CameraReader.h>
#include <Alembic/Tako/CameraWriter.h>
#include <Alembic/Tako/CameraDef.h>
#include <Alembic/Tako/TransformOperation.h>
#include <Alembic/Tako/TransformReader.h>
#include <Alembic/Tako/TransformWriter.h>

namespace {

// CPPUNIT_TEST_SUITE_REGISTRATION(CameraTest);
}

using namespace Alembic::Tako;


void CameraTest::testCameraSimple(void)
{
    CameraData cam;
    std::string name("defaultcamera");

    hid_t fid = -1;
    // write static camera
    {
        TransformWriter root("/tmp/testCamera.hdf");
        fid = H5Iget_file_id(root.getHid());

        TransformWriter tw("test", root);
        Value tx, ty, tz;
        tx = 1.0;
        ty = 2.0;
        tz = 3.0;
        TransformOperationVariant t(Translate(Translate::cTranslate, tx, ty, tz));
        tw.push(t);
        tw.writeTransformStack(0.0, true);

        CameraWriter cw(name.c_str(), tw);
        cw.write(FLT_MAX, cam);
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    // read static camera
    {
        TransformReader root("/tmp/testCamera.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(root.getChild(0));
        TESTING_ASSERT(str == testPtr->getName());

        CameraReaderPtr ptr = boost::get< CameraReaderPtr >(testPtr->getChild(0));
        TESTING_ASSERT(name == ptr->getName());

        unsigned int ttype = ptr->read(0.0);
        TESTING_ASSERT(ttype == CameraReader::CAMERA_STATIC);

        TESTING_ASSERT(ptr->mCameraData.orthographic == cam.orthographic);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotLocal == cam.tumblePivotLocal);
        TESTING_ASSERT(ptr->mCameraData.centerOfInterest == cam.centerOfInterest);
        TESTING_ASSERT(ptr->mCameraData.focalLength == cam.focalLength);
        TESTING_ASSERT(ptr->mCameraData.lensSqueezeRatio == cam.lensSqueezeRatio);
        TESTING_ASSERT(ptr->mCameraData.cameraScale == cam.cameraScale);
        TESTING_ASSERT(ptr->mCameraData.horizontalFilmAperture == cam.horizontalFilmAperture);
        TESTING_ASSERT(ptr->mCameraData.verticalFilmAperture == cam.verticalFilmAperture);
        TESTING_ASSERT(ptr->mCameraData.horizontalFilmOffset == cam.horizontalFilmOffset);
        TESTING_ASSERT(ptr->mCameraData.verticalFilmOffset == cam.verticalFilmOffset);
        TESTING_ASSERT(ptr->mCameraData.filmFit == cam.filmFit);
        TESTING_ASSERT(ptr->mCameraData.filmFitOffset == cam.filmFitOffset);
        TESTING_ASSERT(ptr->mCameraData.overscan == cam.overscan);
        TESTING_ASSERT(ptr->mCameraData.nearClipPlane == cam.nearClipPlane);
        TESTING_ASSERT(ptr->mCameraData.farClipPlane == cam.farClipPlane);
        TESTING_ASSERT(ptr->mCameraData.preScale == cam.preScale);
        TESTING_ASSERT(ptr->mCameraData.filmTranslateH == cam.filmTranslateH);
        TESTING_ASSERT(ptr->mCameraData.filmTranslateV == cam.filmTranslateV);
        TESTING_ASSERT(ptr->mCameraData.horizontalRollPivot == cam.horizontalRollPivot);
        TESTING_ASSERT(ptr->mCameraData.verticalRollPivot == cam.verticalRollPivot);
        TESTING_ASSERT(ptr->mCameraData.filmRollValue == cam.filmRollValue);
        TESTING_ASSERT(ptr->mCameraData.filmRollOrder == cam.filmRollOrder);
        TESTING_ASSERT(ptr->mCameraData.postScale == cam.postScale);
        TESTING_ASSERT(ptr->mCameraData.orthographicWidth == cam.orthographicWidth);
        TESTING_ASSERT(ptr->mCameraData.fstop == cam.fstop);
        TESTING_ASSERT(ptr->mCameraData.focusDistance == cam.focusDistance);
        TESTING_ASSERT(ptr->mCameraData.shutterAngle == cam.shutterAngle);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotX == cam.tumblePivotX);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotY == cam.tumblePivotY);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotZ == cam.tumblePivotZ);

        ttype = ptr->read(150.0);
        TESTING_ASSERT(ttype == (CameraReader::CAMERA_STATIC));

        std::set<float> frames;
        ptr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 0);
        TESTING_ASSERT(!ptr->hasFrames());
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);
}


void CameraTest::testCameraAnimated()
{
    CameraData cam, cam1;
    std::string name("defaultcamera");

    hid_t fid = -1;

    // write keyed camera
    {
        TransformWriter root("/tmp/testCameraAnim.hdf");
        fid = H5Iget_file_id(root.getHid());

        TransformWriter tw("test", root);
        Value tx, ty, tz;
        tx = 1.0;
        ty = 2.0;
        tz = 3.0;
        TransformOperationVariant t(Translate(Translate::cTranslate, tx, ty, tz));
        tw.push(t);
        tw.writeTransformStack(0.0, true);

        CameraWriter cw(name.c_str(), tw);
        cw.write(0.0, cam);

        cam1.centerOfInterest = 5.5;
        cam1.focalLength = 32.0;
        cam1.lensSqueezeRatio = 1.1;
        cam1.horizontalFilmAperture = 3.5;
        cam1.verticalFilmAperture = 2.5;
        cam1.focusDistance = 4.9;
        cam1.shutterAngle = 143;
        cw.write(1.0, cam1);

        cw.write(2.0, cam);

        // write the frame range
        std::vector<float> frames;
        frames.push_back(0.0);
        frames.push_back(1.0);
        frames.push_back(2.0);
        PropertyPair p;
        p.first = frames;
        root.setNonSampledProperty("frameRange", p);
        root.writeProperties(0.0);

    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    // read keyed camera
    {
        TransformReader root("/tmp/testCameraAnim.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr =
            boost::get< TransformReaderPtr >(root.getChild(0));
        TESTING_ASSERT(str == testPtr->getName());

        CameraReaderPtr ptr =
            boost::get< CameraReaderPtr >(testPtr->getChild(0));
        TESTING_ASSERT(name == ptr->getName());

        unsigned int ttype = ptr->read(0.0);
        TESTING_ASSERT(ttype == CameraReader::CAMERA_ANIMATED);

        TESTING_ASSERT(ptr->mCameraData.orthographic == cam.orthographic);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotLocal == cam.tumblePivotLocal);
        TESTING_ASSERT(ptr->mCameraData.centerOfInterest == cam.centerOfInterest);
        TESTING_ASSERT(ptr->mCameraData.focalLength == cam.focalLength);
        TESTING_ASSERT(ptr->mCameraData.lensSqueezeRatio == cam.lensSqueezeRatio);
        TESTING_ASSERT(ptr->mCameraData.cameraScale == cam.cameraScale);
        TESTING_ASSERT(ptr->mCameraData.horizontalFilmAperture == cam.horizontalFilmAperture);
        TESTING_ASSERT(ptr->mCameraData.verticalFilmAperture == cam.verticalFilmAperture);
        TESTING_ASSERT(ptr->mCameraData.horizontalFilmOffset == cam.horizontalFilmOffset);
        TESTING_ASSERT(ptr->mCameraData.verticalFilmOffset == cam.verticalFilmOffset);
        TESTING_ASSERT(ptr->mCameraData.filmFit == cam.filmFit);
        TESTING_ASSERT(ptr->mCameraData.filmFitOffset == cam.filmFitOffset);
        TESTING_ASSERT(ptr->mCameraData.overscan == cam.overscan);
        TESTING_ASSERT(ptr->mCameraData.nearClipPlane == cam.nearClipPlane);
        TESTING_ASSERT(ptr->mCameraData.farClipPlane == cam.farClipPlane);
        TESTING_ASSERT(ptr->mCameraData.preScale == cam.preScale);
        TESTING_ASSERT(ptr->mCameraData.filmTranslateH == cam.filmTranslateH);
        TESTING_ASSERT(ptr->mCameraData.filmTranslateV == cam.filmTranslateV);
        TESTING_ASSERT(ptr->mCameraData.horizontalRollPivot == cam.horizontalRollPivot);
        TESTING_ASSERT(ptr->mCameraData.verticalRollPivot == cam.verticalRollPivot);
        TESTING_ASSERT(ptr->mCameraData.filmRollValue == cam.filmRollValue);
        TESTING_ASSERT(ptr->mCameraData.filmRollOrder == cam.filmRollOrder);
        TESTING_ASSERT(ptr->mCameraData.postScale == cam.postScale);
        TESTING_ASSERT(ptr->mCameraData.orthographicWidth == cam.orthographicWidth);
        TESTING_ASSERT(ptr->mCameraData.fstop == cam.fstop);
        TESTING_ASSERT(ptr->mCameraData.focusDistance == cam.focusDistance);
        TESTING_ASSERT(ptr->mCameraData.shutterAngle == cam.shutterAngle);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotX == cam.tumblePivotX);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotY == cam.tumblePivotY);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotZ == cam.tumblePivotZ);

        ttype = ptr->read(1.0);
        TESTING_ASSERT(ttype == (CameraReader::CAMERA_ANIMATED));

        TESTING_ASSERT(ptr->mCameraData.orthographic == cam1.orthographic);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotLocal == cam.tumblePivotLocal);
        TESTING_ASSERT(ptr->mCameraData.centerOfInterest == cam1.centerOfInterest);
        TESTING_ASSERT(ptr->mCameraData.focalLength == cam1.focalLength);
        TESTING_ASSERT(ptr->mCameraData.lensSqueezeRatio == cam1.lensSqueezeRatio);
        TESTING_ASSERT(ptr->mCameraData.cameraScale == cam1.cameraScale);
        TESTING_ASSERT(ptr->mCameraData.horizontalFilmAperture == cam1.horizontalFilmAperture);
        TESTING_ASSERT(ptr->mCameraData.verticalFilmAperture == cam1.verticalFilmAperture);
        TESTING_ASSERT(ptr->mCameraData.horizontalFilmOffset == cam1.horizontalFilmOffset);
        TESTING_ASSERT(ptr->mCameraData.verticalFilmOffset == cam1.verticalFilmOffset);
        TESTING_ASSERT(ptr->mCameraData.filmFit == cam1.filmFit);
        TESTING_ASSERT(ptr->mCameraData.filmFitOffset == cam1.filmFitOffset);
        TESTING_ASSERT(ptr->mCameraData.overscan == cam1.overscan);
        TESTING_ASSERT(ptr->mCameraData.nearClipPlane == cam1.nearClipPlane);
        TESTING_ASSERT(ptr->mCameraData.farClipPlane == cam1.farClipPlane);
        TESTING_ASSERT(ptr->mCameraData.preScale == cam1.preScale);
        TESTING_ASSERT(ptr->mCameraData.filmTranslateH == cam1.filmTranslateH);
        TESTING_ASSERT(ptr->mCameraData.filmTranslateV == cam1.filmTranslateV);
        TESTING_ASSERT(ptr->mCameraData.horizontalRollPivot == cam1.horizontalRollPivot);
        TESTING_ASSERT(ptr->mCameraData.verticalRollPivot == cam1.verticalRollPivot);
        TESTING_ASSERT(ptr->mCameraData.filmRollValue == cam1.filmRollValue);
        TESTING_ASSERT(ptr->mCameraData.filmRollOrder == cam1.filmRollOrder);
        TESTING_ASSERT(ptr->mCameraData.postScale == cam1.postScale);
        TESTING_ASSERT(ptr->mCameraData.orthographicWidth == cam1.orthographicWidth);
        TESTING_ASSERT(ptr->mCameraData.fstop == cam1.fstop);
        TESTING_ASSERT(ptr->mCameraData.focusDistance == cam1.focusDistance);
        TESTING_ASSERT(ptr->mCameraData.shutterAngle == cam1.shutterAngle);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotX == cam1.tumblePivotX);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotY == cam1.tumblePivotY);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotZ == cam1.tumblePivotZ);

        ttype = ptr->read(2.0);
        TESTING_ASSERT(ttype == CameraReader::CAMERA_ANIMATED);

        TESTING_ASSERT(ptr->mCameraData.orthographic == cam.orthographic);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotLocal == cam.tumblePivotLocal);
        TESTING_ASSERT(ptr->mCameraData.centerOfInterest == cam.centerOfInterest);
        TESTING_ASSERT(ptr->mCameraData.focalLength == cam.focalLength);
        TESTING_ASSERT(ptr->mCameraData.lensSqueezeRatio == cam.lensSqueezeRatio);
        TESTING_ASSERT(ptr->mCameraData.cameraScale == cam.cameraScale);
        TESTING_ASSERT(ptr->mCameraData.horizontalFilmAperture == cam.horizontalFilmAperture);
        TESTING_ASSERT(ptr->mCameraData.verticalFilmAperture == cam.verticalFilmAperture);
        TESTING_ASSERT(ptr->mCameraData.horizontalFilmOffset == cam.horizontalFilmOffset);
        TESTING_ASSERT(ptr->mCameraData.verticalFilmOffset == cam.verticalFilmOffset);
        TESTING_ASSERT(ptr->mCameraData.filmFit == cam.filmFit);
        TESTING_ASSERT(ptr->mCameraData.filmFitOffset == cam.filmFitOffset);
        TESTING_ASSERT(ptr->mCameraData.overscan == cam.overscan);
        TESTING_ASSERT(ptr->mCameraData.nearClipPlane == cam.nearClipPlane);
        TESTING_ASSERT(ptr->mCameraData.farClipPlane == cam.farClipPlane);
        TESTING_ASSERT(ptr->mCameraData.preScale == cam.preScale);
        TESTING_ASSERT(ptr->mCameraData.filmTranslateH == cam.filmTranslateH);
        TESTING_ASSERT(ptr->mCameraData.filmTranslateV == cam.filmTranslateV);
        TESTING_ASSERT(ptr->mCameraData.horizontalRollPivot == cam.horizontalRollPivot);
        TESTING_ASSERT(ptr->mCameraData.verticalRollPivot == cam.verticalRollPivot);
        TESTING_ASSERT(ptr->mCameraData.filmRollValue == cam.filmRollValue);
        TESTING_ASSERT(ptr->mCameraData.filmRollOrder == cam.filmRollOrder);
        TESTING_ASSERT(ptr->mCameraData.postScale == cam.postScale);
        TESTING_ASSERT(ptr->mCameraData.orthographicWidth == cam.orthographicWidth);
        TESTING_ASSERT(ptr->mCameraData.fstop == cam.fstop);
        TESTING_ASSERT(ptr->mCameraData.focusDistance == cam.focusDistance);
        TESTING_ASSERT(ptr->mCameraData.shutterAngle == cam.shutterAngle);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotX == cam.tumblePivotX);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotY == cam.tumblePivotY);
        TESTING_ASSERT(ptr->mCameraData.tumblePivotZ == cam.tumblePivotZ);

        std::set<float> frames;
        ptr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 3);
        TESTING_ASSERT(ptr->hasFrames());

        TESTING_ASSERT(frames.find(0.0) != frames.end());
        TESTING_ASSERT(frames.find(1.0) != frames.end());
        TESTING_ASSERT(frames.find(2.0) != frames.end());
        TESTING_ASSERT(frames.find(3.0) == frames.end());
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);
}

