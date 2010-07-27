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

#include <Alembic/TakoSPI/NurbsCurveReader.h>
#include <Alembic/TakoSPI/TransformReader.h>
#include <Alembic/TakoSPI/PrivateUtil.h>

namespace Alembic {

namespace TakoSPI {

class NurbsCurveReaderIMPL::PrivateData
{
    public:
        PrivateData()
        {
            mGeometryAddr = 0;
            mVersion = -1;
            mIsSampled = false;
        }

        haddr_t mGeometryAddr;
        int mVersion;
        bool mIsSampled;
};

NurbsCurveReaderIMPL::NurbsCurveReaderIMPL(
    const std::string & iName,
    TransformReaderIMPL & iParent)
  : ReaderGlue<TakoAbstractV1::NurbsCurveReader>(iName, iParent.getHid()),
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

NurbsCurveReaderIMPL::~NurbsCurveReaderIMPL() {}

void NurbsCurveReaderIMPL::getFrames(std::set<float>& ioFrames) const
{
    H5Literate(getGroup(), H5_INDEX_NAME, H5_ITER_INC, NULL, GetFrames,
        &ioFrames);
}

bool NurbsCurveReaderIMPL::hasFrames() const
{
    return mData->mIsSampled;
}

unsigned int NurbsCurveReaderIMPL::read(float iFrame)
{
    std::string name = "static";

    if (mData->mIsSampled)
    {
        name = floatToString(iFrame);
    }

    // if we have already read the curve data for static then we don't
    // need to do it again
    if (!mData->mIsSampled && !mCurveGrp.cv.empty())
        return NurbsCurveReaderIMPL::GEOMETRY_STATIC;

    hid_t gid = H5Gopen2(getGroup(), name.c_str(), H5P_DEFAULT);

    if (gid < 0)
        return NurbsCurveReaderIMPL::READ_ERROR;

    // read the cv
    hid_t did = H5Dopen2(gid, ".cv", H5P_DEFAULT);
    hid_t sid = H5Dget_space(did);
    hid_t tid = H5Dget_type(did);

    hsize_t dims[1];
    H5Sget_simple_extent_dims(sid, dims, NULL);
    mCurveGrp.cv.resize(dims[0]);
    float * fPtr = &(mCurveGrp.cv[0]);
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    did = H5Dopen2(gid, ".numCurve", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    unsigned int * iPtr = &mCurveGrp.numCurve;
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, iPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    // now read the rest of the information, only if neccessary
    H5L_info_t info;
    H5Lget_info(gid, ".miscinfo", &info, H5P_DEFAULT);
    if (info.type == H5L_TYPE_HARD && info.u.address == mData->mGeometryAddr)
    {
        H5Gclose(gid);
        return NurbsCurveReaderIMPL::GEOMETRY_ANIMATED;
    }

    // create the custom data type for reading from hdf
    hid_t miscinfo_tid = H5Tcreate(H5T_COMPOUND, sizeof(NurbsCurveMiscInfo));
    H5Tinsert(miscinfo_tid, "numCV", HOFFSET(NurbsCurveMiscInfo, numCV),
        H5T_NATIVE_UINT);

    H5Tinsert(miscinfo_tid, "span", HOFFSET(NurbsCurveMiscInfo, span),
        H5T_NATIVE_UINT);

    H5Tinsert(miscinfo_tid, "degree", HOFFSET(NurbsCurveMiscInfo, degree),
        H5T_NATIVE_UCHAR);

    H5Tinsert(miscinfo_tid, "form", HOFFSET(NurbsCurveMiscInfo, form),
        H5T_NATIVE_UCHAR);

    H5Tinsert(miscinfo_tid, "min", HOFFSET(NurbsCurveMiscInfo, min),
        H5T_NATIVE_FLOAT);

    H5Tinsert(miscinfo_tid, "max", HOFFSET(NurbsCurveMiscInfo, max),
        H5T_NATIVE_FLOAT);

    did = H5Dopen2(gid, ".miscinfo", H5P_DEFAULT);
    mCurveGrp.miscInfo.resize(mCurveGrp.numCurve);
    NurbsCurveMiscInfo * ptr = &(mCurveGrp.miscInfo[0]);
    H5Dread(did, miscinfo_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, ptr);
    H5Dclose(did);

    did = H5Dopen2(gid, ".knot", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    mCurveGrp.knot.resize(dims[0]);
    fPtr = &(mCurveGrp.knot[0]);
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    if (H5Lexists(gid, ".width", H5P_DEFAULT))
    {
        did = H5Dopen2(gid, ".width", H5P_DEFAULT);
        sid = H5Dget_space(did);
        tid = H5Dget_type(did);
        H5Sget_simple_extent_dims(sid, dims, NULL);
        mCurveGrp.width.resize(dims[0]);
        fPtr = &(mCurveGrp.width[0]);
        H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
        H5Tclose(tid);
        H5Sclose(sid);
        H5Dclose(did);
    }
    else if (H5Lexists(gid, ".constantWidth", H5P_DEFAULT))
    {
        did = H5Dopen2(gid, ".constantWidth", H5P_DEFAULT);
        sid = H5Dget_space(did);
        tid = H5Dget_type(did);
        fPtr = &mCurveGrp.constantWidth;
        H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
        H5Tclose(tid);
        H5Sclose(sid);
        H5Dclose(did);
    }

    if (info.type == H5L_TYPE_HARD)
        mData->mGeometryAddr = info.u.address;

    if (gid > -1)
        H5Gclose(gid);

    unsigned int rtype = NurbsCurveReaderIMPL::GEOMETRY_ANIMATED;

    if (!mData->mIsSampled)
        rtype = NurbsCurveReaderIMPL::GEOMETRY_STATIC;

    return rtype;
}

} // End namespace TakoSPI

} // End namespace Alembic
