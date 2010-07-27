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

#include <Alembic/Tako/SubDReader.h>
#include <Alembic/Tako/TransformReader.h>
#include <Alembic/Tako/PrivateUtil.h>

namespace Alembic {

using Tako::SubDReader;

namespace {

unsigned int readGeometry(hid_t iGroup, haddr_t & ioAddr,
    std::vector<float> & ioPoints, std::vector<Tako::index_t> & ioFacePoints,
    std::vector<Tako::index_t> & ioFaceList)
{
    if (iGroup < 0)
        return SubDReader::READ_ERROR;

    // read the points
    hid_t did = H5Dopen2(iGroup, ".points", H5P_DEFAULT);
    hid_t sid = H5Dget_space(did);
    hid_t tid = H5Dget_type(did);

    hsize_t dims[1];
    H5Sget_simple_extent_dims(sid, dims, NULL);
    ioPoints.resize(dims[0]);
    float * fPtr = &ioPoints[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    // now read the face points and face list, only if neccessary
    H5L_info_t info;
    H5Lget_info(iGroup, ".facePoints", &info, H5P_DEFAULT);

    // we don't need to read the face points
    if (info.type == H5L_TYPE_HARD && info.u.address == ioAddr)
    {
        return SubDReader::GEOMETRY_HOMOGENOUS;
    }

    did = H5Dopen2(iGroup, ".facePoints", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    ioFacePoints.resize(dims[0]);
    Tako::index_t * iPtr = &ioFacePoints[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, iPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);


    did = H5Dopen2(iGroup, ".faceList", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    ioFaceList.resize(dims[0]);
    iPtr = &ioFaceList[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, iPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    unsigned int rtype = SubDReader::GEOMETRY_HETEROGENOUS;

    // hasn't been loaded yet so homogenous
    if (ioAddr == 0)
        rtype = SubDReader::GEOMETRY_HOMOGENOUS;

    if (info.type == H5L_TYPE_HARD)
        ioAddr = info.u.address;

    return rtype;
}

// very similiar to the above function, but the group names it needs to read
// and the return value is very different. The data may not even exist.
unsigned int readCreases(hid_t iGroup, haddr_t & ioAddr,
    std::vector<float> & ioSharpness,
    std::vector<Tako::index_t> & ioIndices,
    std::vector<Tako::index_t> & ioLengths)
{
    // crease data doesnt exist
    if (iGroup < 0 || H5Lexists(iGroup, ".creaseSharpness", H5P_DEFAULT) != 1)
        return 0;

    // read the crease sharpness
    hid_t did = H5Dopen2(iGroup, ".creaseSharpness", H5P_DEFAULT);
    hid_t sid = H5Dget_space(did);
    hid_t tid = H5Dget_type(did);

    hsize_t dims[1];
    H5Sget_simple_extent_dims(sid, dims, NULL);
    ioSharpness.resize(dims[0]);
    float * fPtr = &ioSharpness[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    // now read the crease indices and lengths, only if neccessary
    H5L_info_t info;
    H5Lget_info(iGroup, ".creaseIndices", &info, H5P_DEFAULT);

    // we don't need to read the crease topology
    if (info.type == H5L_TYPE_HARD && info.u.address == ioAddr)
    {
        return SubDReader::CREASES_HOMOGENOUS;
    }

    did = H5Dopen2(iGroup, ".creaseIndices", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    ioIndices.resize(dims[0]);
    Tako::index_t * iPtr = &ioIndices[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, iPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);


    did = H5Dopen2(iGroup, ".creaseLengths", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    ioLengths.resize(dims[0]);
    iPtr = &ioLengths[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, iPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    unsigned int rtype = SubDReader::CREASES_HETEROGENOUS;

    // nothing has been loaded previously so homogenous
    if (ioAddr == 0)
        rtype = SubDReader::CREASES_HOMOGENOUS;

    if (info.type == H5L_TYPE_HARD)
        ioAddr = info.u.address;

    return rtype;
}

// very similiar to the creases function, but reads less data (no lengths)
unsigned int readCorners(hid_t iGroup, haddr_t & ioAddr,
    std::vector<float> & ioSharpness,
    std::vector<Tako::index_t> & ioIndices)
{
    // corner data doesnt exist
    if (iGroup < 0 || H5Lexists(iGroup, ".cornerSharpness", H5P_DEFAULT) != 1)
        return 0;

    // read the corner sharpness
    hid_t did = H5Dopen2(iGroup, ".cornerSharpness", H5P_DEFAULT);
    hid_t sid = H5Dget_space(did);
    hid_t tid = H5Dget_type(did);

    hsize_t dims[1];
    H5Sget_simple_extent_dims(sid, dims, NULL);
    ioSharpness.resize(dims[0]);
    float * fPtr = &ioSharpness[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    // now read the corner indices, only if neccessary
    H5L_info_t info;
    H5Lget_info(iGroup, ".cornerIndices", &info, H5P_DEFAULT);

    // we don't need to read the corner topology
    if (info.type == H5L_TYPE_HARD && info.u.address == ioAddr)
    {
        return SubDReader::CORNERS_HOMOGENOUS;
    }

    did = H5Dopen2(iGroup, ".cornerIndices", H5P_DEFAULT);
    sid = H5Dget_space(did);
    tid = H5Dget_type(did);
    H5Sget_simple_extent_dims(sid, dims, NULL);
    ioIndices.resize(dims[0]);
    Tako::index_t * iPtr = &ioIndices[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, iPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    unsigned int rtype = SubDReader::CORNERS_HETEROGENOUS;

    // nothing has been loaded previously so homogenous
    if (ioAddr == 0)
        rtype = SubDReader::CORNERS_HOMOGENOUS;

    if (info.type == H5L_TYPE_HARD)
        ioAddr = info.u.address;

    return rtype;
}

// very similiar to the corners function, but reads less data (just indices)
unsigned int readHolePolyIndices(hid_t iGroup,
    std::vector<Tako::index_t> & ioIndices)
{
    // hole poly data doesnt exist for this group
    if (iGroup < 0 || H5Lexists(iGroup, ".holePolyIndices", H5P_DEFAULT) != 1)
        return 0;

    // read the hole poly indices
    hid_t did = H5Dopen2(iGroup, ".holePolyIndices", H5P_DEFAULT);
    hid_t sid = H5Dget_space(did);
    hid_t tid = H5Dget_type(did);

    hsize_t dims[1];
    H5Sget_simple_extent_dims(sid, dims, NULL);
    ioIndices.resize(dims[0]);
    Tako::index_t * iPtr = &ioIndices[0];
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, iPtr);
    H5Tclose(tid);
    H5Sclose(sid);
    H5Dclose(did);

    // since hole poly indices mark the geometry faces as invisible, it
    // doesn't make sense to have a homogenous variable, it either changes
    // the topology or is static (since it's just an index array).

    // The reader function makes the static determination so we can just
    // return heterogenous.

    return SubDReader::HOLEPOLY_HETEROGENOUS;
}
}

class Tako::SubDReader::PrivateData
{
    public:
        PrivateData()
        {
            mGeometryAddr = 0;
            mCornersAddr = 0;
            mCreasesAddr = 0;
            mStaticHid = -1;

            mGeoHint = 1;
            mCornerHint = 0;
            mCreaseHint = 0;
            mHoleHint = 0;

            mFaceVaryingInterpolateBoundary = -1;
            mFaceVaryingPropagateCorners = -1;
            mInterpolateBoundary = -1;

            mVersion = -1;
        }

        haddr_t mGeometryAddr;
        haddr_t mCornersAddr;
        haddr_t mCreasesAddr;
        hid_t mStaticHid;

        // 0 doesnt exist, 1 static, 2 animated
        char mCornerHint;
        char mCreaseHint;
        char mGeoHint;
        char mHoleHint;

        int mFaceVaryingInterpolateBoundary;
        int mFaceVaryingPropagateCorners;
        int mInterpolateBoundary;

        int mVersion;
};

SubDReader::SubDReader(const char * iName, TransformReader & iParent)
: HDFReaderNode(iName, iParent.getHid()), mData(new PrivateData())
{
    if (H5Lexists(mGroup, "static", H5P_DEFAULT) == 1)
    {
        mData->mStaticHid = H5Gopen2(mGroup, "static", H5P_DEFAULT);
    }

    if (H5Aexists(mGroup, "faceVaryingInterpolateBoundary") == 1)
    {
        hid_t aid = H5Aopen(mGroup, "faceVaryingInterpolateBoundary",
            H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &mData->mFaceVaryingInterpolateBoundary);
        H5Aclose(aid);
    }


    if (H5Aexists(mGroup, "faceVaryingPropagateCorners") == 1)
    {
        hid_t aid = H5Aopen(mGroup, "faceVaryingPropagateCorners",
            H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &mData->mFaceVaryingPropagateCorners);
        H5Aclose(aid);
    }

    if (H5Aexists(mGroup, "interpolateBoundary") == 1)
    {
        hid_t aid = H5Aopen(mGroup, "interpolateBoundary", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &mData->mInterpolateBoundary);
        H5Aclose(aid);
    }

    if (H5Aexists(mGroup, "vers") == 1)
    {
        hid_t aid = H5Aopen(mGroup, "vers", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &mData->mVersion);
        H5Aclose(aid);
    }

    // the geometry must exist, but if the attr doesnt exist then we assume
    // it is static
    if (H5Aexists(mGroup, "geo") == 1)
    {
        hid_t aid = H5Aopen(mGroup, "geo", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_CHAR, &mData->mGeoHint);
        H5Aclose(aid);
    }

    if (H5Aexists(mGroup, "corner") == 1)
    {
        hid_t aid = H5Aopen(mGroup, "corner", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_CHAR, &mData->mCornerHint);
        H5Aclose(aid);
    }

    if (H5Aexists(mGroup, "crease") == 1)
    {
        hid_t aid = H5Aopen(mGroup, "crease", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_CHAR, &mData->mCreaseHint);
        H5Aclose(aid);
    }

    if (H5Aexists(mGroup, "hole") == 1)
    {
        hid_t aid = H5Aopen(mGroup, "hole", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_CHAR, &mData->mHoleHint);
        H5Aclose(aid);
    }
}

SubDReader::~SubDReader()
{
    if (mData->mStaticHid > -1)
        H5Gclose(mData->mStaticHid);
}

int SubDReader::getFaceVaryingInterpolateBoundary()
{
    return mData->mFaceVaryingInterpolateBoundary;
}

int SubDReader::getFaceVaryingPropagateCorners()
{
    return mData->mFaceVaryingPropagateCorners;
}

int SubDReader::getInterpolateBoundary()
{
    return mData->mInterpolateBoundary;
}


void SubDReader::getFrames(std::set<float>& ioFrames) const
{
    H5Literate(mGroup, H5_INDEX_NAME, H5_ITER_INC, NULL, GetFrames,
        &ioFrames);
}

bool SubDReader::hasFrames() const
{
    return (mData->mGeoHint == 2 || mData->mCornerHint == 2 ||
        mData->mCreaseHint == 2 || mData->mHoleHint == 2);
}

// returns status of what is changing
unsigned int SubDReader::read(float iFrame)
{
    hid_t frameHid = -1;

    // dont bother looking if we dont even have any frames
    if (hasFrames())
    {
        frameHid = H5Gopen2(mGroup, floatToString(iFrame).c_str(),
            H5P_DEFAULT);
    }

    unsigned int geoType = SubDReader::GEOMETRY_STATIC;
    switch (mData->mGeoHint)
    {
        // animated case
        case 2:
        {
            geoType = readGeometry(frameHid, mData->mGeometryAddr, mPoints,
                mFacePoints, mFaceList);
        }
        break;

        // since geometry must exist any other case is static
        default:
        {
            // if we haven't already read the geometry data
            if (mPoints.empty())
            {
                // we only pay attention to the return type in case we have
                // a read error
                geoType = readGeometry(mData->mStaticHid, mData->mGeometryAddr,
                    mPoints, mFacePoints, mFaceList);

                // if its not a read error, then we remap it to static
                if (geoType != SubDReader::READ_ERROR)
                    geoType = SubDReader::GEOMETRY_STATIC;
            }
        }
        break;
    }

    unsigned int creasesType = 0;

    switch (mData->mCreaseHint)
    {
        case 1:
        {
            creasesType = SubDReader::CREASES_STATIC;

            // if we haven't already read crease data
            if (mCreaseSharpness.empty())
            {
                // ignore the return value since the hint says we are static
                readCreases(mData->mStaticHid,
                    mData->mCreasesAddr, mCreaseSharpness, mCreaseIndices,
                    mCreaseLengths);
            }
        }
        break;
        case 2:
        {
            creasesType = readCreases(frameHid,
                mData->mCreasesAddr, mCreaseSharpness, mCreaseIndices,
                mCreaseLengths);
        }
        break;

        // creasesType is already 0
        default:
        break;
    }

    unsigned int cornersType = 0;
    switch (mData->mCornerHint) {
        // static case
        case 1:
        {
            cornersType = SubDReader::CORNERS_STATIC;

            // if we haven't yet read the corners static data
            if (mCornerSharpness.empty())
            {
                // ignore the return value since the hint says we are static
                readCorners(mData->mStaticHid, mData->mCornersAddr,
                    mCornerSharpness, mCornerIndices);
            }
        }
        break;

        case 2:
        {
            cornersType = readCorners(frameHid, mData->mCornersAddr,
                mCornerSharpness, mCornerIndices);
        }
        break;

        // the cornersType was already initialized to 0
        default:
        break;
    }

    unsigned int holePolyType = 0;
    switch (mData->mHoleHint)
    {
        case 1:
        {
            holePolyType = SubDReader::HOLEPOLY_STATIC;

            // if we havent yet read the static hole data
            if (mHolePolyIndices.empty())
            {
                // dont worry about return type since we got the static hint
                readHolePolyIndices(mData->mStaticHid, mHolePolyIndices);
            }
        }
        break;

        case 2:
        {
            holePolyType = readHolePolyIndices(frameHid, mHolePolyIndices);
        }
        break;

        default:
        break;
    }

    if (frameHid > -1)
        H5Gclose(frameHid);

    return geoType | creasesType | cornersType | holePolyType;
}

} // End namespace Alembic

