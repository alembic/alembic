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

#include <Alembic/TakoSPI/NurbsSurfaceReader.h>
#include <Alembic/TakoSPI/TransformReader.h>
#include <Alembic/TakoSPI/PrivateUtil.h>

namespace Alembic {

namespace TakoSPI {

namespace {

unsigned int readGeometry(hid_t iGroup, haddr_t & ioAddr,
    std::vector<float> & ioCV,
    std::vector<double> & ioKnotsInU,
    std::vector<double> & ioKnotsInV,
    NurbsSurfaceMiscInfo & mMiscInfo)
{
    if (iGroup == -1)
        return NurbsSurfaceReaderIMPL::READ_ERROR;

    // read the cv
    hid_t did = H5Dopen2(iGroup, ".cv", H5P_DEFAULT);
    hid_t sid = H5Dget_space(did);
    hid_t tid = H5Dget_type(did);

    hsize_t dims[1];
    H5Sget_simple_extent_dims(sid, dims, NULL);
    ioCV.resize(dims[0]);
    float * fPtr = &ioCV[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    // now read the rest of the information, only if neccessary
    H5L_info_t info;
    H5Lget_info(iGroup, ".miscinfo", &info, H5P_DEFAULT);
    if (info.type == H5L_TYPE_HARD && info.u.address == ioAddr)
    {
        return NurbsSurfaceReaderIMPL::GEOMETRY_HOMOGENOUS;
    }

    did = H5Dopen2(iGroup, ".miscinfo", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, &mMiscInfo);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    did = H5Dopen2(iGroup, ".knotsU", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    unsigned int knotSize = dims[0];
    ioKnotsInU.resize(knotSize);

    // check the data type, the renderers use float knots, but Maya is
    // overly strict with its Nurbs tolerance, so double knots is an
    // attempt to appease it
    bool isKnotFloat = (H5Tget_size(tid) == H5Tget_size(H5T_NATIVE_FLOAT));
    if (isKnotFloat)
    {
        std::vector<float> knotU;
        knotU.resize(knotSize);
        float *fPtr = &knotU[0];
        H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
        for (size_t i = 0; i < knotSize; i++)
            ioKnotsInU[i] = knotU[i];
    }
    else
    {
        double * dPtr = &ioKnotsInU[0];
        H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, dPtr);
    }
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    did = H5Dopen2(iGroup, ".knotsV", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    knotSize = dims[0];
    ioKnotsInV.resize(knotSize);
    if (isKnotFloat)
    {
        std::vector<float> knotV;
        knotV.resize(knotSize);
        float *fPtr = &knotV[0];
        H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
        for (size_t i = 0; i < knotSize; i++)
            ioKnotsInV[i] = knotV[i];
    }
    else
    {
        double * dPtr = &ioKnotsInV[0];
        H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, dPtr);
    }
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    unsigned int rtype = NurbsSurfaceReaderIMPL::GEOMETRY_HETEROGENOUS;

     if (ioAddr == 0)
        rtype = NurbsSurfaceReaderIMPL::GEOMETRY_HOMOGENOUS;

    // update ioAddr
    if (info.type == H5L_TYPE_HARD)
        ioAddr = info.u.address;

    return rtype;
}


unsigned int readTrimCurve(hid_t iGroup, haddr_t & ioAddr,
    std::vector<NurbsCurveGrp> & tc )
{
    if (iGroup < 0 || H5Lexists(iGroup, ".TCcv", H5P_DEFAULT) != 1)
        return 0;

    hsize_t dims[1];

    hid_t did = H5Dopen2(iGroup, ".TCcv", H5P_DEFAULT);
    hid_t sid = H5Dget_space(did);
    hid_t tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    std::vector<float> cvVec;
    cvVec.resize(dims[0]);
    float * fPtr = &cvVec[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);


    did = H5Dopen2(iGroup, ".TCnumCV", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    std::vector<int> numcvVec(dims[0]);
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, &numcvVec.front());
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    // now read the rest of the information, only if neccessary
    H5L_info_t info;
    H5Lget_info(iGroup, ".TCmiscinfo", &info, H5P_DEFAULT);
    if (info.type == H5L_TYPE_HARD && info.u.address == ioAddr)
    {
        size_t numGrp = tc.size();
        std::vector<float>::iterator sIter = cvVec.begin();
        std::vector<float>::iterator eIter = cvVec.begin();
        for (size_t i = 0; i < numGrp; i++)
        {
            tc[i].cv.resize(numcvVec[i]*4);
            eIter += numcvVec[i]*4;
            std::copy(sIter, eIter, tc[i].cv.begin());
            sIter = eIter;
        }
        return NurbsSurfaceReaderIMPL::TRIM_CURVE_HOMOGENOUS;
    }

    did = H5Dopen2(iGroup, ".TCnumCurve", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    unsigned int numGrp = dims[0];
    tc.resize(numGrp);

    std::vector<unsigned int> numCurveVec;
    numCurveVec.resize(numGrp);
    unsigned int * uiPtr = &numCurveVec[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, uiPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    unsigned int sizeMiscInfo = 0;
    for (size_t i = 0; i < numGrp; i++)
        sizeMiscInfo += numCurveVec[i];

    // create the custum data type for reading from hdf
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

    did = H5Dopen2(iGroup, ".TCmiscinfo", H5P_DEFAULT);
    std::vector<NurbsCurveMiscInfo> miscInfoVec;
    miscInfoVec.resize(sizeMiscInfo);
    NurbsCurveMiscInfo *ptr = &miscInfoVec[0];
    H5Dread(did, miscinfo_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, ptr);
    H5Dclose(did);

    did = H5Dopen2(iGroup, ".TCknot", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    std::vector<float> knotVec;
    knotVec.resize(dims[0]);
    fPtr = &knotVec[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    // unpack information back to curve groups
    std::vector<float>::iterator cvSIter = cvVec.begin();
    std::vector<float>::iterator cvEIter = cvVec.begin();

    std::vector<NurbsCurveMiscInfo>::iterator miscinfoSIter =
        miscInfoVec.begin();

    std::vector<NurbsCurveMiscInfo>::iterator miscinfoEIter =
        miscInfoVec.begin();

    std::vector<float>::iterator knotSIter = knotVec.begin();
    std::vector<float>::iterator knotEIter = knotVec.begin();

    for (size_t i = 0; i < numGrp; i++)
    {
        tc[i].numCurve = numCurveVec[i];

        tc[i].cv.resize(numcvVec[i]*4);
        cvEIter += numcvVec[i]*4;

        std::copy(cvSIter, cvEIter, tc[i].cv.begin());
        cvSIter = cvEIter;

        tc[i].miscInfo.resize(numCurveVec[i]);
        miscinfoEIter += numCurveVec[i];

        std::copy(miscinfoSIter, miscinfoEIter,
                  tc[i].miscInfo.begin());

        miscinfoSIter = miscinfoEIter;

        unsigned int sumDegree = 0;
        unsigned int sumSpan = 0;
        for (size_t index = 0; index < numCurveVec[i]; index++)
        {
            sumDegree += tc[i].miscInfo[index].degree;
            sumSpan += tc[i].miscInfo[index].span;
        }

        tc[i].knot.resize(sumSpan + 2*sumDegree + numCurveVec[i]);
        knotEIter += sumSpan + 2*sumDegree + numCurveVec[i];
        std::copy(knotSIter, knotEIter, tc[i].knot.begin());
        knotSIter = knotEIter;
    }

    unsigned int rtype = NurbsSurfaceReaderIMPL::TRIM_CURVE_HETEROGENOUS;

    // nothing has been previously loaded so homogenous
    if (ioAddr == 0)
        rtype = NurbsSurfaceReaderIMPL::TRIM_CURVE_HOMOGENOUS;

    if (info.type == H5L_TYPE_HARD)
        ioAddr = info.u.address;

    return rtype;
}
}

class NurbsSurfaceReaderIMPL::PrivateData
{
    public:
        PrivateData()
        {
            mGeometryAddr = 0;
            mTrimCurveAddr = 0;
            mStaticHid = -1;
            mVersion = -1;
            mGeoHint = 1;
            mTrimHint = 0;
        }

        haddr_t mGeometryAddr;
        haddr_t mTrimCurveAddr;
        hid_t mStaticHid;

        // 0 doesnt exist, 1 static, 2 animated
        char mGeoHint;
        char mTrimHint;

        int mVersion;
};

NurbsSurfaceReaderIMPL::NurbsSurfaceReaderIMPL(
    const std::string & iName,
    TransformReaderIMPL & iParent)
  : ReaderGlue<TakoAbstractV1::NurbsSurfaceReader>(iName, iParent.getHid()),
    mData(new PrivateData())
{
    if (H5Lexists(getGroup(), "static", H5P_DEFAULT) == 1)
    {
        mData->mStaticHid = H5Gopen2(getGroup(), "static", H5P_DEFAULT);
    }

    if (H5Aexists(getGroup(), "vers") == 1)
    {
        hid_t aid = H5Aopen(getGroup(), "vers", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &mData->mVersion);
        H5Aclose(aid);
    }

    // the geometry must exist, but if the attr doesnt exist then we assume
    // it is static
    if (H5Aexists(getGroup(), "geo") == 1)
    {
        hid_t aid = H5Aopen(getGroup(), "geo", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_CHAR, &mData->mGeoHint);
        H5Aclose(aid);
    }

    if (H5Aexists(getGroup(), "trim") == 1)
    {
        hid_t aid = H5Aopen(getGroup(), "trim", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_CHAR, &mData->mTrimHint);
        H5Aclose(aid);
    }
}

NurbsSurfaceReaderIMPL::~NurbsSurfaceReaderIMPL()
{
    if (mData->mStaticHid > -1)
        H5Gclose(mData->mStaticHid);
}

void NurbsSurfaceReaderIMPL::getFrames(std::set<float>& ioFrames) const
{
    H5Literate(getGroup(), H5_INDEX_NAME, H5_ITER_INC, NULL, GetFrames,
        &ioFrames);
}

bool NurbsSurfaceReaderIMPL::hasFrames() const
{
    return (mData->mGeoHint == 2) || (mData->mTrimHint == 2);
}

unsigned int NurbsSurfaceReaderIMPL::read(float iFrame)
{
    hid_t frameHid = -1;

    std::string name = "static";

    // dont bother looking if we dont have any frames
    if (hasFrames())
    {
        frameHid = H5Gopen2(getGroup(), floatToString(iFrame).c_str(),
            H5P_DEFAULT);
    }

    unsigned int geoType = NurbsSurfaceReaderIMPL::GEOMETRY_STATIC;

    switch (mData->mGeoHint)
    {
        case 2:
        {
            geoType = readGeometry(frameHid, mData->mGeometryAddr,
                                   mCV,
                                   mKnotsInU,
                                   mKnotsInV,
                                   mMiscInfo);
        }
        break;

        // since the geometry must exist any other case is static
        default:
        {
            // we haven't read the static geometry data once
            if (mCV.empty())
            {
                // only pay attention to the return type for a read error
                geoType = readGeometry(
                    mData->mStaticHid,
                    mData->mGeometryAddr,
                    mCV,
                    mKnotsInU,
                    mKnotsInV,
                    mMiscInfo);

                // if its not a read error, then we remap it to static
                if (geoType != NurbsSurfaceReaderIMPL::READ_ERROR)
                    geoType = NurbsSurfaceReaderIMPL::GEOMETRY_STATIC;
            }
        }
        break;
    }

    unsigned int trimCurveType = 0;

    switch (mData->mTrimHint)
    {
        case 1:
        {
            trimCurveType = NurbsSurfaceReaderIMPL::TRIM_CURVE_STATIC;

            // if we have read the trim curve already
            if (mTrimCurve.empty())
            {
                readTrimCurve(mData->mStaticHid, mData->mTrimCurveAddr,
                    mTrimCurve);
            }
        }
        break;

        case 2:
        {
            trimCurveType = readTrimCurve(frameHid, mData->mTrimCurveAddr,
                mTrimCurve);
        }
        break;

        // trim curve hint says it doesn't exist
        default:
        break;
    }

    if (frameHid > -1)
        H5Gclose(frameHid);

    return geoType | trimCurveType;
}

} // End namespace TakoSPI

} // End namespace Alembic
