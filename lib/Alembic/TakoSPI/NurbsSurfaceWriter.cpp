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

#include <Alembic/TakoSPI/NurbsSurfaceWriter.h>
#include <Alembic/TakoSPI/TransformWriter.h>
#include <Alembic/TakoSPI/NodeTypeId.h>
#include <Alembic/TakoSPI/PrivateUtil.h>

namespace Alembic {

namespace TakoSPI {

namespace {

void verifyLists(unsigned char degreeU, unsigned char degreeV,
    unsigned int spanU, unsigned int spanV,
    unsigned int numKnotsU, unsigned int numKnotsV, index_t numVals)
{
    // make sure the size of CV points is correct
    // numCVsInU = degreeU + spansInU
    // numCVsInV = degreeV + spansInV
    unsigned int numCVsInU = degreeU + spanU;
    unsigned int numCVsInV = degreeV + spanV;

    // make sure the size of knot vectors is correct
    unsigned int numKnotsInU = numCVsInU + degreeU + 1;
    unsigned int numKnotsInV = numCVsInV + degreeV + 1;
    if (numKnotsU != numKnotsInU)
        throw std::logic_error("Wrong size for knots in U");
    if (numKnotsV != numKnotsInV)
        throw std::logic_error("Wrong size for knots in V");

    if (numVals != 4*numCVsInU*numCVsInV)
        throw std::logic_error("Wrong size for control vertices");
}

enum SurfaceState
{
    SURFACE_NOT_WRITTEN,
    SURFACE_STATIC,
    SURFACE_ANIMATED,
    TRIM_CURVE_NOT_WRITTEN,
    TRIM_CURVE_STATIC,
    TRIM_CURVE_ANIMATED
};
}

class NurbsSurfaceWriterIMPL::PrivateData
{
    public:
        PrivateData()
        {
            mNumVals = 0;

            mSurfaceLastFrameId = -1;
            mSurfaceWriteState = SURFACE_NOT_WRITTEN;

            mTCNumVals = 0;
            mTrimCurveLastFrameId = -1;
            mTrimCurveWriteState = TRIM_CURVE_NOT_WRITTEN;

            mCurrentFrame = FLT_MAX;
            mStartCalled = false;
        }

        index_t mNumVals;

        hid_t mSurfaceLastFrameId;
        SurfaceState mSurfaceWriteState;

        hid_t mTrimCurveLastFrameId;
        SurfaceState mTrimCurveWriteState;
        index_t mTCNumVals;

        float mCurrentFrame;
        bool mStartCalled;
};

NurbsSurfaceWriterIMPL::NurbsSurfaceWriterIMPL(
    const std::string & iName,
    TransformWriterIMPL & iParent)
  : WriterGlue<TakoAbstractV1::NurbsSurfaceWriter>(iName, iParent.getHid()),
    mData(new PrivateData())
{
    int typeVal = NODE_TYPE_NURBSSURFACE;
    H5LTset_attribute_int(getGroup(), ".", "type", &typeVal, 1);

    int version = NURBSSURFACE_INITIAL_VERSION;
    H5LTset_attribute_int(getGroup(), ".", "vers", &version, 1);
}

NurbsSurfaceWriterIMPL::~NurbsSurfaceWriterIMPL()
{
    if (mData->mSurfaceLastFrameId > -1)
        H5Gclose(mData->mSurfaceLastFrameId);

    if (mData->mTrimCurveLastFrameId > -1)
        H5Gclose(mData->mTrimCurveLastFrameId);
}

void NurbsSurfaceWriterIMPL::start(float iFrame)
{
    if (mData->mStartCalled)
        throw std::logic_error("start already called, please call end first.");

    mData->mStartCalled = true;
    mData->mCurrentFrame = iFrame;
}

void NurbsSurfaceWriterIMPL::end()
{
    if (!mData->mStartCalled)
        throw std::logic_error("end can not be called before start.");

    mData->mStartCalled = false;
}

void NurbsSurfaceWriterIMPL::write(const std::vector<float> & iCV,
    const std::vector<double> & iKnotsInU,
    const std::vector<double> & iKnotsInV,
    const NurbsSurfaceMiscInfo & miscInfo)
{
    if (!mData->mStartCalled)
        throw std::logic_error("start must be called before writing geometry.");

    std::string name = "static";
    if (mData->mCurrentFrame != FLT_MAX)
    {
        if (mData->mSurfaceWriteState == SURFACE_STATIC)
        {
            throw std::logic_error(
                "Static data has been written so can't write animated!");
        }

        name = floatToString(mData->mCurrentFrame);

        // write the geo animated hint only if it is animated, and this is
        // our first animated write
        if (mData->mSurfaceWriteState == SURFACE_NOT_WRITTEN)
        {
            char val = 2;
            H5LTset_attribute_char(getGroup(), ".", "geo", &val, 1);
        }

        mData->mSurfaceWriteState = SURFACE_ANIMATED;
    }
    else if (mData->mSurfaceWriteState == SURFACE_ANIMATED)
    {
        throw std::logic_error(
            "Animated data has been written so can't write static data!");
    }
    else
    {
        mData->mSurfaceWriteState = SURFACE_STATIC;
    }

    index_t numVals = iCV.size();
    mData->mNumVals = numVals;
    verifyLists(miscInfo.degreeU, miscInfo.degreeV, miscInfo.spanU,
        miscInfo.spanV, iKnotsInU.size(), iKnotsInV.size(), numVals);

    if (mData->mSurfaceLastFrameId > -1)
        H5Gclose(mData->mSurfaceLastFrameId);

    mData->mSurfaceLastFrameId = H5Gcreate(getGroup(), name.c_str(), H5P_DEFAULT,
        H5P_DEFAULT, H5P_DEFAULT);

    hsize_t dims[1];
    dims[0] = numVals;

    const float * fPtr = &(iCV[0]);
    H5LTmake_dataset_float(mData->mSurfaceLastFrameId, ".cv", 1, dims, fPtr);

    // create the custum data type for writing into hdf
    hid_t miscinfo_tid = H5Tcreate(H5T_COMPOUND, sizeof(NurbsSurfaceMiscInfo));

    H5Tinsert(miscinfo_tid, "spanU", HOFFSET(NurbsSurfaceMiscInfo, spanU),
        H5T_NATIVE_UINT);

    H5Tinsert(miscinfo_tid, "spanV", HOFFSET(NurbsSurfaceMiscInfo, spanV),
        H5T_NATIVE_UINT);

    H5Tinsert(miscinfo_tid, "degreeU", HOFFSET(NurbsSurfaceMiscInfo, degreeU),
        H5T_NATIVE_UCHAR);

    H5Tinsert(miscinfo_tid, "degreeV", HOFFSET(NurbsSurfaceMiscInfo, degreeV),
        H5T_NATIVE_UCHAR);

    H5Tinsert(miscinfo_tid, "formU", HOFFSET(NurbsSurfaceMiscInfo, formU),
        H5T_NATIVE_UCHAR);

    H5Tinsert(miscinfo_tid, "formV", HOFFSET(NurbsSurfaceMiscInfo, formV),
        H5T_NATIVE_UCHAR);

    H5Tinsert(miscinfo_tid, "minU", HOFFSET(NurbsSurfaceMiscInfo, minU),
        H5T_NATIVE_FLOAT);

    H5Tinsert(miscinfo_tid, "maxU", HOFFSET(NurbsSurfaceMiscInfo, maxU),
        H5T_NATIVE_FLOAT);

    H5Tinsert(miscinfo_tid, "minV", HOFFSET(NurbsSurfaceMiscInfo, minV),
        H5T_NATIVE_FLOAT);

    H5Tinsert(miscinfo_tid, "maxV", HOFFSET(NurbsSurfaceMiscInfo, maxV),
        H5T_NATIVE_FLOAT);

    // create the data space
    dims[0] = 1;
    hid_t space = H5Screate_simple(1, dims, NULL);

    // create the dataset
    hid_t dataset = H5Dcreate2(mData->mSurfaceLastFrameId, ".miscinfo",
        miscinfo_tid, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    H5Dwrite(dataset, miscinfo_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, &miscInfo);

    H5Tclose(miscinfo_tid);
    H5Sclose(space);
    H5Dclose(dataset);

    dims[0] = iKnotsInU.size();
    const double * knotsUPtr = &(iKnotsInU[0]);
    H5LTmake_dataset_double(
        mData->mSurfaceLastFrameId, ".knotsU", 1, dims, knotsUPtr);

    dims[0] = iKnotsInV.size();
    const double * knotsVPtr = &(iKnotsInV[0]);
    herr_t ret = H5LTmake_dataset_double(
        mData->mSurfaceLastFrameId, ".knotsV", 1, dims, knotsVPtr);

    if (ret < 0)
    {
        throw std::runtime_error("Error writing Nurbs Surface");
    }
}

// other info is reused from the previous write
void NurbsSurfaceWriterIMPL::write(const std::vector<float> & iCV) 
{
    if (!mData->mStartCalled)
        throw std::logic_error("start must be called before writing geometry.");

    if (mData->mSurfaceLastFrameId <= -1)
        throw std::logic_error("Topology has not yet been written!");

    if (mData->mCurrentFrame == FLT_MAX)
        throw std::logic_error("Can not write static geometry data!");

    index_t numVals = iCV.size();
    if (numVals != mData->mNumVals)
        throw std::logic_error("Point counts have changed!");

    hid_t gid = H5Gcreate(getGroup(), floatToString(mData->mCurrentFrame).c_str(),
        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    hsize_t dims[1];
    dims[0] = numVals;

    const float * fPtr = &(iCV[0]);
    herr_t ret = H5LTmake_dataset_float(gid, ".cv", 1, dims, fPtr);
    if (ret < 0)
    {
        throw std::runtime_error("Error writing Nurbs Surface cvs");
    }

    // make hard links for other information
    H5Lcreate_hard(mData->mSurfaceLastFrameId, ".miscinfo", gid, ".miscinfo",
        H5P_DEFAULT, H5P_DEFAULT);

    H5Lcreate_hard(mData->mSurfaceLastFrameId, ".knotsU", gid, ".knotsU",
        H5P_DEFAULT, H5P_DEFAULT);

    H5Lcreate_hard(mData->mSurfaceLastFrameId, ".knotsV", gid, ".knotsV",
        H5P_DEFAULT, H5P_DEFAULT);

    H5Gclose(gid);
}

void NurbsSurfaceWriterIMPL::writeTrimCurve( const std::vector<NurbsCurveGrp> & tc )
{
    if (!mData->mStartCalled)
    {
        throw std::logic_error(
            "start must be called before writing trim curves.");
    }

    if (mData->mSurfaceLastFrameId == -1)
    {
        throw std::logic_error(
            "Nurbs surface must be written before trim curves.");
    }

    std::string name = "static";
    if (mData->mCurrentFrame != FLT_MAX)
    {
        if (mData->mTrimCurveWriteState == TRIM_CURVE_STATIC)
        {
            throw std::logic_error(
                "Static data has been written so can't write animated trim!");
        }

        name = floatToString(mData->mCurrentFrame);

        mData->mTrimCurveWriteState = TRIM_CURVE_ANIMATED;
    }
    else if (mData->mTrimCurveWriteState == TRIM_CURVE_ANIMATED)
    {
        throw std::logic_error(
            "Animated data has been written so can't write static trim!");
    }
    else
    {
        mData->mTrimCurveWriteState = TRIM_CURVE_STATIC;
    }

    if (mData->mTrimCurveLastFrameId > -1)
    {
        H5Gclose(mData->mTrimCurveLastFrameId);
    }
    else
    {
        // first time writing the trim, write the hint attribute
        char val = 1;
        if (mData->mCurrentFrame != FLT_MAX)
        {
            val = 2;
        }
        H5LTset_attribute_char(getGroup(), ".", "trim", &val, 1);
    }

    if (H5Lexists(getGroup(), name.c_str(), H5P_DEFAULT) == 0)
    {
        mData->mTrimCurveLastFrameId = H5Gcreate(getGroup(), name.c_str(),
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    else
    {
        mData->mTrimCurveLastFrameId = H5Gopen(getGroup(), name.c_str(),
            H5P_DEFAULT);
    }

    hsize_t dims[1];

    unsigned int numGrp = tc.size();

    dims[0] = numGrp;
    if (numGrp == 0)
        return;

    std::vector<int> numCurveVec(numGrp);
    std::vector<int> numcvVec(numGrp);
    unsigned int numCurve = 0;
    unsigned int numCV = 0;
    std::vector<float> cv;
    std::vector<float> knot;
    for (size_t i = 0; i < numGrp; i++)
    {
        if (tc[i].isValid() == false)
        {
            throw std::logic_error(
                "Curve group has invalid topology information");
        }

        numCurveVec[i] = tc[i].numCurve;
        numCurve += tc[i].numCurve;
        numCV += tc[i].getNumCV();
        numcvVec[i] = tc[i].getNumCV();
        std::copy(tc[i].cv.begin(),
                  tc[i].cv.end(), std::back_inserter(cv));
        std::copy(tc[i].knot.begin(),
                  tc[i].knot.end(),
            std::back_inserter(knot));
    }

    mData->mTCNumVals = numCV*4;
    H5LTmake_dataset_int(mData->mTrimCurveLastFrameId, ".TCnumCurve", 1,
        dims, &numCurveVec.front());

    // numCV is not redundant information, since it is needed to unpack
    // trim curve cv vector when reading
    H5LTmake_dataset_int(mData->mTrimCurveLastFrameId, ".TCnumCV", 1, dims,
        &numcvVec.front());

    // create the custum data type for writing into hdf
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

    std::vector<NurbsCurveMiscInfo> data(numCurve);
    unsigned int cnt = 0;
    for (size_t i = 0; i < numGrp; i++)
    {
        unsigned int nCurve = tc[i].numCurve;
        for (size_t index = 0; index < nCurve; index++)
        {
            data[cnt].numCV   = tc[i].getNumCV(index);
            data[cnt].span    = tc[i].getSpan(index);
            data[cnt].degree  = tc[i].getDegree(index);
            data[cnt].form    = tc[i].getForm(index);
            data[cnt].min     = tc[i].getMin(index);
            data[cnt].max     = tc[i].getMax(index);
            cnt++;
        }
    }

    // create the data space
    dims[0] = numCurve;
    hid_t space = H5Screate_simple(1, dims, NULL);
    // create the dataset
    hid_t dataset = H5Dcreate2(mData->mTrimCurveLastFrameId, ".TCmiscinfo",
        miscinfo_tid, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    H5Dwrite(dataset, miscinfo_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT,
        &data.front());

    H5Tclose(miscinfo_tid);
    H5Sclose(space);
    H5Dclose(dataset);

    dims[0] = cv.size();
    const float * fPtr = &(cv[0]);
    H5LTmake_dataset_float(mData->mTrimCurveLastFrameId, ".TCcv", 1, dims,
        fPtr);

    dims[0] = knot.size();
    fPtr = &(knot[0]);
    herr_t ret = H5LTmake_dataset_float(mData->mTrimCurveLastFrameId,
        ".TCknot", 1, dims, fPtr);
    if (ret < 0)
    {
        throw std::runtime_error("Error writing Nurbs Surface trim curves");
    }
}


// other info is reused from the previous write
void NurbsSurfaceWriterIMPL::writeTrimCurveCV(
    const std::vector<NurbsCurveGrp> & tc) 
{
    if (!mData->mStartCalled)
    {
        throw std::logic_error(
            "start must be called before writing trim curve cvs.");
    }

    if (mData->mTrimCurveLastFrameId <= -1)
        throw std::logic_error("Topology has not yet been written!");

    if (mData->mCurrentFrame == FLT_MAX)
        throw std::logic_error("Can not write static trim curve data!");

    hsize_t dims[1];

    unsigned int numGrp = tc.size();
    std::vector<float> cv;
    for (size_t i = 0; i < numGrp; i++)
    {
        if (tc[i].isValid() == false)
        {
            throw std::logic_error(
                "curve group has invalid topology information");
        }
        std::copy(tc[i].cv.begin(),
                  tc[i].cv.end(), std::back_inserter(cv));
    }
    dims[0] = cv.size();
    if (cv.size() != mData->mTCNumVals)
        throw std::logic_error("Point counts have changed!");

    std::string name = floatToString(mData->mCurrentFrame);

    hid_t gid;
    if (H5Lexists(getGroup(), name.c_str(), H5P_DEFAULT) == 0)
        gid = H5Gcreate(getGroup(), name.c_str(), H5P_DEFAULT,
            H5P_DEFAULT, H5P_DEFAULT);
    else
        gid = H5Gopen(getGroup(), name.c_str(), H5P_DEFAULT);

    const float * fPtr = &(cv[0]);
    herr_t ret = H5LTmake_dataset_float(gid, ".TCcv", 1, dims, fPtr);
    if (ret < 0)
    {
        throw std::runtime_error("Error writing Nurbs Surface trim curve cvs");
    }

    // make hard links for other information
    H5Lcreate_hard(mData->mTrimCurveLastFrameId, ".TCnumCurve", gid,
        ".TCnumCurve", H5P_DEFAULT, H5P_DEFAULT);

    H5Lcreate_hard(mData->mTrimCurveLastFrameId, ".TCmiscinfo", gid,
        ".TCmiscinfo", H5P_DEFAULT, H5P_DEFAULT);

    H5Lcreate_hard(mData->mTrimCurveLastFrameId, ".TCknot", gid,
        ".TCknot", H5P_DEFAULT, H5P_DEFAULT);

    H5Lcreate_hard(mData->mTrimCurveLastFrameId, ".TCnumCV", gid,
        ".TCnumCV", H5P_DEFAULT, H5P_DEFAULT);

    H5Gclose(gid);
}

} // End namespace TakoSPI

} // End namespace Alembic
