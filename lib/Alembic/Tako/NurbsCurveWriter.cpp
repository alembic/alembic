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

#include <Alembic/Tako/NurbsCurveWriter.h>
#include <Alembic/Tako/TransformWriter.h>
#include <Alembic/Tako/NodeTypeId.h>
#include <Alembic/Tako/PrivateUtil.h>

namespace Alembic {

using Tako::NurbsCurveWriter;

namespace {

enum CurveState
{
    CURVE_NOT_WRITTEN,
    CURVE_STATIC,
    CURVE_ANIMATED
};
}

class Tako::NurbsCurveWriter::PrivateData
{
    public:

        PrivateData()
        {
            mLastFrameId = -1;
            mWriteState = CURVE_NOT_WRITTEN;
        }

        hid_t mLastFrameId;
        CurveState mWriteState;
};

NurbsCurveWriter::NurbsCurveWriter(const char * iName,
    TransformWriter & iParent)
    : HDFWriterNode(iName, iParent.getHid()), mData(new PrivateData())
{
    int typeVal = NODE_TYPE_NURBSCURVE;
    H5LTset_attribute_int(mGroup, ".", "type", &typeVal, 1);

    int version = NURBSCURVE_INITIAL_VERSION;
    H5LTset_attribute_int(mGroup, ".", "vers", &version, 1);
}

NurbsCurveWriter::~NurbsCurveWriter()
{
    if (mData->mLastFrameId > -1)
        H5Gclose(mData->mLastFrameId);
}

void NurbsCurveWriter::write(float iFrame,
    const NurbsCurveGrp & curveGrp) const
{
    if (curveGrp.isValid() == false)
    {
        throw std::logic_error("curve group has invalid topology information");
    }

    std::string name = "static";
    if (iFrame != FLT_MAX)
    {
        if (mData->mWriteState == CURVE_STATIC)
            throw std::logic_error(
                "Static curves have been written so can't write animated!");

        name = floatToString(iFrame);

        // if this is our first animated write, set the sample hint
        if (mData->mWriteState == CURVE_NOT_WRITTEN)
        {
            char samp = 1;
            H5LTset_attribute_char(mGroup, ".", "samp", &samp, 1);
        }

        mData->mWriteState = CURVE_ANIMATED;
    }
    else if (mData->mWriteState == CURVE_ANIMATED)
    {
        throw std::logic_error(
            "Animated curves have been written so can't write static data!");
    }
    else
    {
        mData->mWriteState = CURVE_STATIC;
    }

    if (mData->mLastFrameId > -1)
        H5Gclose(mData->mLastFrameId);

    mData->mLastFrameId = H5Gcreate(mGroup, name.c_str(), H5P_DEFAULT,
        H5P_DEFAULT, H5P_DEFAULT);

    hsize_t dims[1];
    unsigned int numCurve = curveGrp.numCurve;
    if (numCurve == 0)
        return;

    dims[0] = 1;
    const int iPtr = numCurve;
    H5LTmake_dataset_int(mData->mLastFrameId, ".numCurve", 1, dims, &iPtr);

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

    unsigned int numCV = curveGrp.getNumCV();
    for (unsigned int index = 0; index < numCurve; index++)
    {
        data[index].numCV   = curveGrp.getNumCV(index);
        data[index].span    = curveGrp.getSpan(index);
        data[index].degree  = curveGrp.getDegree(index);
        data[index].form    = curveGrp.getForm(index);
        data[index].min     = curveGrp.getMin(index);
        data[index].max     = curveGrp.getMax(index);
    }

    // create the data space
    dims[0] = numCurve;

    hid_t space = H5Screate_simple(1, dims, NULL);

    // create the dataset
    hid_t dataset = H5Dcreate2(mData->mLastFrameId, ".miscinfo", miscinfo_tid,
        space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(dataset, miscinfo_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT,
        &data.front());

    H5Tclose(miscinfo_tid);
    H5Sclose(space);
    H5Dclose(dataset);

    dims[0] = 4*numCV;
    if (dims[0] == 0)
        return;
    const float * fPtr = &(curveGrp.cv[0]);
    H5LTmake_dataset_float(mData->mLastFrameId, ".cv", 1, dims, fPtr);

    if (curveGrp.constantWidth > 0)
    {
        dims[0] = 1;
        fPtr = &curveGrp.constantWidth;
        H5LTmake_dataset_float(mData->mLastFrameId, ".constantWidth", 1,
            dims, fPtr);
    }
    else if (curveGrp.width.size() > 0)
    {
        dims[0] = curveGrp.width.size();
        fPtr = &(curveGrp.width[0]);
        H5LTmake_dataset_float(mData->mLastFrameId, ".width", 1, dims, fPtr);
    }

    dims[0] = curveGrp.knot.size();
    fPtr = &(curveGrp.knot[0]);
    herr_t ret = H5LTmake_dataset_float(mData->mLastFrameId, ".knot", 1,
        dims, fPtr);
    if (ret < 0)
    {
        throw std::runtime_error("Error writing Nurbs Curves");
    }
}

// other info is reused from the previous write, right now this function is
// not used because it's not practical to know if the topology is the same
// for every curve
void NurbsCurveWriter::write(float iFrame,
    const std::vector<float> & cv) const
{
    if (mData->mLastFrameId <= -1)
        throw std::logic_error("Topology has not yet been written!");

    if (iFrame == FLT_MAX)
        throw std::logic_error("Can not write static geometry data!");

    hid_t gid = H5Gcreate(mGroup, floatToString(iFrame).c_str(), H5P_DEFAULT,
        H5P_DEFAULT, H5P_DEFAULT);

    hsize_t dims[1];
    dims[0] = cv.size();
    const float * fPtr = &cv[0];
    herr_t ret = H5LTmake_dataset_float(gid, ".cv", 1, dims, fPtr);
    if (ret < 0) {
        throw std::runtime_error("Error writing Nurbs Curves");
    }

    // make hard links for other information
    H5Lcreate_hard(mData->mLastFrameId, ".numCurve", gid, ".numCurve",
        H5P_DEFAULT, H5P_DEFAULT);

    H5Lcreate_hard(mData->mLastFrameId, ".miscinfo", gid, ".miscinfo",
        H5P_DEFAULT, H5P_DEFAULT);

    H5Lcreate_hard(mData->mLastFrameId, ".knot", gid, ".knot", H5P_DEFAULT,
        H5P_DEFAULT);

    if (H5Lexists(mData->mLastFrameId, ".width", H5P_DEFAULT))
    {
        H5Lcreate_hard(mData->mLastFrameId, ".width", gid, ".width",
            H5P_DEFAULT, H5P_DEFAULT);
    }
    else if (H5Lexists(mData->mLastFrameId, ".constantWidth", H5P_DEFAULT))
    {
        H5Lcreate_hard(mData->mLastFrameId, ".constantWidth", gid,
            ".constantWidth", H5P_DEFAULT, H5P_DEFAULT);
    }

    H5Gclose(gid);
}

} // End namespace Alembic
