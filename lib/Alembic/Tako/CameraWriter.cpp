//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include <Alembic/Tako/CameraWriter.h>
#include <Alembic/Tako/TransformWriter.h>
#include <Alembic/Tako/NodeTypeId.h>
#include <Alembic/Tako/PrivateUtil.h>

namespace Alembic {

using Tako::TransformWriter;
using Tako::CameraWriter;

namespace {

enum CameraState
{
    CAMERA_NOT_WRITTEN,
    CAMERA_STATIC,
    CAMERA_ANIMATED
};
}


class Tako::CameraWriter::PrivateData
{
    public:

    PrivateData()
    {
        mLastFrameId = -1;
        mWriteState = CAMERA_NOT_WRITTEN;
    }

    hid_t mLastFrameId;
    CameraState mWriteState;
};

CameraWriter::CameraWriter(const char * iName, TransformWriter & iParent)
    : HDFWriterNode(iName, iParent.getHid()), mData(new PrivateData())
{
    int typeVal = NODE_TYPE_CAMERA;
    H5LTset_attribute_int(mGroup, ".", "type", &typeVal, 1);

    int version = CAMERA_INITIAL_VERSION;
    H5LTset_attribute_int(mGroup, ".", "vers", &version, 1);
}

CameraWriter::~CameraWriter()
{
    if (mData->mLastFrameId > -1)
        H5Gclose(mData->mLastFrameId);
}

void CameraWriter::write(float iFrame, const CameraData & cam) const
{
    std::string name = "static";
    if (iFrame != FLT_MAX)
    {
        if (mData->mWriteState == CAMERA_STATIC)
        {
            throw std::logic_error(
                "Static camera has been written so can't write animated!");
        }

        name = floatToString(iFrame);

        // write the sample hint if this is our first time writing animated
        // data on this node
        if (mData->mWriteState == CAMERA_NOT_WRITTEN)
        {
            char val = 1;
            H5LTset_attribute_char(mGroup, ".", "samp", &val, 1);
        }

        mData->mWriteState = CAMERA_ANIMATED;
    }
    else if (mData->mWriteState == CAMERA_ANIMATED)
    {
        throw std::logic_error(
            "Animated camera has been written, can' write static!");
    }
    else
    {
        mData->mWriteState = CAMERA_STATIC;
    }

    if (mData->mLastFrameId > -1)
    {
        H5Gclose(mData->mLastFrameId);
    }

    mData->mLastFrameId = H5Gcreate(mGroup, name.c_str(), H5P_DEFAULT,
                H5P_DEFAULT, H5P_DEFAULT);

    HDFCameraCompound data;

    hsize_t dims[1];
    dims[0] = sizeof(data.doubleData)/sizeof(data.doubleData[0]);
    hid_t d_tid = H5Tarray_create(H5T_NATIVE_DOUBLE, 1, dims);

    // create the custom data type for writing into hdf
    hid_t cam_tid = H5Tcreate(H5T_COMPOUND, sizeof(data));
    H5Tinsert(cam_tid, "filmFit", HOFFSET(HDFCameraCompound, filmFit),
        H5T_NATIVE_UINT);

    H5Tinsert(cam_tid, "doubleData", HOFFSET(HDFCameraCompound,
        doubleData), d_tid);

    H5Tinsert(cam_tid, "orthographic", HOFFSET(HDFCameraCompound,
        orthographic), H5T_NATIVE_CHAR);

    H5Tinsert(cam_tid, "tumblePivotLocal", HOFFSET(HDFCameraCompound,
        tumblePivotLocal), H5T_NATIVE_CHAR);

    H5Tinsert(cam_tid, "filmRollOrder", HOFFSET(HDFCameraCompound,
        filmRollOrder), H5T_NATIVE_CHAR);

    // fill the data
    data.filmFit = cam.filmFit;
    data.doubleData[ 0] = cam.centerOfInterest;
    data.doubleData[ 1] = cam.focalLength;
    data.doubleData[ 2] = cam.lensSqueezeRatio;
    data.doubleData[ 3] = cam.cameraScale;
    data.doubleData[ 4] = cam.horizontalFilmAperture;
    data.doubleData[ 5] = cam.verticalFilmAperture;
    data.doubleData[ 6] = cam.horizontalFilmOffset;
    data.doubleData[ 7] = cam.verticalFilmOffset;
    data.doubleData[ 8] = cam.filmFitOffset;
    data.doubleData[ 9] = cam.overscan;
    data.doubleData[10] = cam.nearClipPlane;
    data.doubleData[11] = cam.farClipPlane;
    data.doubleData[12] = cam.preScale;
    data.doubleData[13] = cam.filmTranslateH;
    data.doubleData[14] = cam.filmTranslateV;
    data.doubleData[15] = cam.horizontalRollPivot;
    data.doubleData[16] = cam.verticalRollPivot;
    data.doubleData[17] = cam.filmRollValue;
    data.doubleData[18] = cam.postScale;
    data.doubleData[19] = cam.orthographicWidth;
    data.doubleData[20] = cam.fstop;
    data.doubleData[21] = cam.focusDistance;
    data.doubleData[22] = cam.shutterAngle;
    data.doubleData[23] = cam.tumblePivotX;
    data.doubleData[24] = cam.tumblePivotY;
    data.doubleData[25] = cam.tumblePivotZ;
    data.orthographic   = cam.orthographic;
    data.tumblePivotLocal = cam.tumblePivotLocal;
    data.filmRollOrder  = cam.filmRollOrder;

    dims[0] = 1;
    hid_t space = H5Screate_simple(1, dims, NULL);

    // create the dataset
    hid_t dataset = H5Dcreate2(mData->mLastFrameId, ".camera", cam_tid,
        space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    herr_t ret = H5Dwrite(dataset, cam_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT,
        &data);

    if (ret < 0)
    {
        throw std::runtime_error("Error writing camera");
    }

    H5Tclose(d_tid);
    H5Tclose(cam_tid);
    H5Sclose(space);
    H5Dclose(dataset);
}

} // End namespace Alembic

