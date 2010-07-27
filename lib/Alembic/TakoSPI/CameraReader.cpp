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

#include <Alembic/TakoSPI/CameraReader.h>
#include <Alembic/TakoSPI/TransformReader.h>
#include <Alembic/TakoSPI/PrivateUtil.h>

namespace Alembic {

namespace TakoSPI {

class CameraReaderIMPL::PrivateData
{
    public:
        PrivateData()
        {
            mStaticRead = false;
            mIsSampled = false;
            mVersion = -1;
        }

        bool mIsSampled;
        bool mStaticRead;

        int  mVersion;
};

CameraReaderIMPL::CameraReaderIMPL(const std::string & iName,
                                   TransformReaderIMPL & iParent)
  : ReaderGlue<TakoAbstractV1::CameraReader>(iName, iParent.getHid()),
    mData(new PrivateData())
{
    if (H5Aexists(getGroup(), "vers") == 1)
    {
        hid_t aid = H5Aopen(getGroup(), "vers", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &mData->mVersion);
        H5Aclose(aid);
    }

    if (H5Aexists(getGroup(), "samp") == 1)
    {
        char samp = 0;
        hid_t aid = H5Aopen(getGroup(), "samp", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_CHAR, &samp);
        H5Aclose(aid);
        mData->mIsSampled = (samp != 0);
    }
}

CameraReaderIMPL::~CameraReaderIMPL() {}

void CameraReaderIMPL::getFrames(std::set<float>& ioFrames) const
{
    H5Literate(getGroup(), H5_INDEX_NAME, H5_ITER_INC, NULL, GetFrames,
        &ioFrames);
}

bool CameraReaderIMPL::hasFrames() const
{
    return mData->mIsSampled;
}

unsigned int CameraReaderIMPL::read(float iFrame)
{
    std::string name = "static";

    if (mData->mIsSampled)
    {
        name = floatToString(iFrame);
    }
    else if (mData->mStaticRead)
    {
        // the camera has been read once, don't read again
        return CAMERA_STATIC;
    }

    hid_t gid = H5Gopen2(getGroup(), name.c_str(), H5P_DEFAULT);

    if (gid < 0)
        return READ_ERROR;

    hid_t did = H5Dopen2(gid, ".camera", H5P_DEFAULT);

    HDFCameraCompound data;

    hsize_t dims[1];
    dims[0] = sizeof(data.doubleData)/sizeof(data.doubleData[0]);
    hid_t d_tid = H5Tarray_create(H5T_NATIVE_DOUBLE, 1, dims);

    // create the custom data type for reading the camera from hdf
    hid_t cam_tid = H5Tcreate(H5T_COMPOUND, sizeof(HDFCameraCompound));

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

    dims[0] = 1;
    // for backward compatibility
    herr_t ret = H5Dread(did, cam_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data);

    // reading hdf file written by SG V1
    if (ret < 0)
    {
        HDFCameraCompound_V1 data_V1;

        // create the custom data type for reading the camera from hdf
        cam_tid = H5Tcreate(H5T_COMPOUND, sizeof(HDFCameraCompound_V1));

        H5Tinsert(cam_tid, "filmFit", HOFFSET(HDFCameraCompound_V1, filmFit),
            H5T_NATIVE_UINT);

        H5Tinsert(cam_tid, "doubleData", HOFFSET(HDFCameraCompound_V1,
            doubleData), d_tid);

        H5Tinsert(cam_tid, "orthographic", HOFFSET(HDFCameraCompound_V1,
            orthographic), H5T_NATIVE_CHAR);

        H5Tinsert(cam_tid, "filmRollOrder", HOFFSET(HDFCameraCompound_V1,
            filmRollOrder), H5T_NATIVE_CHAR);

        H5Dread(did, cam_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data_V1);

        mCameraData.filmFit = data_V1.filmFit;
        mCameraData.centerOfInterest        = data_V1.doubleData[ 0];
        mCameraData.focalLength             = data_V1.doubleData[ 1];
        mCameraData.lensSqueezeRatio        = data_V1.doubleData[ 2];
        mCameraData.cameraScale             = data_V1.doubleData[ 3];
        mCameraData.horizontalFilmAperture  = data_V1.doubleData[ 4];
        mCameraData.verticalFilmAperture    = data_V1.doubleData[ 5];
        mCameraData.horizontalFilmOffset    = data_V1.doubleData[ 6];
        mCameraData.verticalFilmOffset      = data_V1.doubleData[ 7];
        mCameraData.filmFitOffset           = data_V1.doubleData[ 8];
        mCameraData.overscan                = data_V1.doubleData[ 9];
        mCameraData.nearClipPlane           = data_V1.doubleData[10];
        mCameraData.farClipPlane            = data_V1.doubleData[11];
        mCameraData.preScale                = data_V1.doubleData[12];
        mCameraData.filmTranslateH          = data_V1.doubleData[13];
        mCameraData.filmTranslateV          = data_V1.doubleData[14];
        mCameraData.horizontalRollPivot     = data_V1.doubleData[15];
        mCameraData.verticalRollPivot       = data_V1.doubleData[16];
        mCameraData.filmRollValue           = data_V1.doubleData[17];
        mCameraData.postScale               = data_V1.doubleData[18];
        mCameraData.orthographicWidth       = data_V1.doubleData[19];
        mCameraData.fstop                   = data_V1.doubleData[20];
        mCameraData.focusDistance           = data_V1.doubleData[21];
        mCameraData.shutterAngle            = data_V1.doubleData[22];
        mCameraData.tumblePivotX            = data_V1.doubleData[23];
        mCameraData.tumblePivotY            = data_V1.doubleData[24];
        mCameraData.tumblePivotZ            = data_V1.doubleData[25];
        mCameraData.orthographic            = data_V1.orthographic;
        mCameraData.filmRollOrder           = data_V1.filmRollOrder;
    }
    else
    {
        mCameraData.filmFit = data.filmFit;
        mCameraData.centerOfInterest        = data.doubleData[ 0];
        mCameraData.focalLength             = data.doubleData[ 1];
        mCameraData.lensSqueezeRatio        = data.doubleData[ 2];
        mCameraData.cameraScale             = data.doubleData[ 3];
        mCameraData.horizontalFilmAperture  = data.doubleData[ 4];
        mCameraData.verticalFilmAperture    = data.doubleData[ 5];
        mCameraData.horizontalFilmOffset    = data.doubleData[ 6];
        mCameraData.verticalFilmOffset      = data.doubleData[ 7];
        mCameraData.filmFitOffset           = data.doubleData[ 8];
        mCameraData.overscan                = data.doubleData[ 9];
        mCameraData.nearClipPlane           = data.doubleData[10];
        mCameraData.farClipPlane            = data.doubleData[11];
        mCameraData.preScale                = data.doubleData[12];
        mCameraData.filmTranslateH          = data.doubleData[13];
        mCameraData.filmTranslateV          = data.doubleData[14];
        mCameraData.horizontalRollPivot     = data.doubleData[15];
        mCameraData.verticalRollPivot       = data.doubleData[16];
        mCameraData.filmRollValue           = data.doubleData[17];
        mCameraData.postScale               = data.doubleData[18];
        mCameraData.orthographicWidth       = data.doubleData[19];
        mCameraData.fstop                   = data.doubleData[20];
        mCameraData.focusDistance           = data.doubleData[21];
        mCameraData.shutterAngle            = data.doubleData[22];
        mCameraData.tumblePivotX            = data.doubleData[23];
        mCameraData.tumblePivotY            = data.doubleData[24];
        mCameraData.tumblePivotZ            = data.doubleData[25];
        mCameraData.orthographic            = data.orthographic;
        mCameraData.tumblePivotLocal        = data.tumblePivotLocal;
        mCameraData.filmRollOrder           = data.filmRollOrder;
    }

    H5Tclose(d_tid);
    H5Dclose(did);
    H5Gclose(gid);

    // if the data is NOT sampled
    if (!mData->mIsSampled)
    {
        mData->mStaticRead = true;
        return CAMERA_STATIC;
    }
    else
    {
        return CAMERA_ANIMATED;
    }
}

} // End namespace TakoSPI

} // End namespace Alembic

