//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#ifndef AbcExport_AbcWriteJob_h
#define AbcExport_AbcWriteJob_h

#include "Foundation.h"

#include "MayaCameraWriter.h"
#include "MayaMeshWriter.h"
#include "MayaNurbsCurveWriter.h"
#include "MayaPointPrimitiveWriter.h"
#include "MayaTransformWriter.h"
#include "MayaLocatorWriter.h"
#include "MayaNurbsSurfaceWriter.h"

#include "MayaUtility.h"

typedef Alembic::Util::shared_ptr < MayaMeshWriter >
    MayaMeshWriterPtr;
typedef Alembic::Util::shared_ptr < MayaNurbsCurveWriter >
    MayaNurbsCurveWriterPtr;
typedef Alembic::Util::shared_ptr < MayaCameraWriter >
    MayaCameraWriterPtr;
typedef Alembic::Util::shared_ptr < MayaLocatorWriter >
    MayaLocatorWriterPtr;
typedef Alembic::Util::shared_ptr < MayaPointPrimitiveWriter >
    MayaPointPrimitiveWriterPtr;
typedef Alembic::Util::shared_ptr < MayaNurbsSurfaceWriter >
    MayaNurbsSurfaceWriterPtr;

struct AbcWriteJobStatistics
{
    AbcWriteJobStatistics()
    {
        mSubDStaticNum = 0;
        mSubDAnimNum = 0;
        mSubDStaticFaces = 0;
        mSubDAnimFaces = 0;
        mSubDStaticCVs = 0;
        mSubDAnimCVs = 0;

        mPolyStaticNum = 0;
        mPolyAnimNum = 0;
        mPolyStaticFaces = 0;
        mPolyAnimFaces = 0;
        mPolyStaticCVs = 0;
        mPolyAnimCVs = 0;

        mCurveStaticNum = 0;
        mCurveStaticCurves = 0;
        mCurveAnimNum = 0;
        mCurveAnimCurves = 0;
        mCurveStaticCVs = 0;
        mCurveAnimCVs = 0;

        mPointStaticNum = 0;
        mPointAnimNum = 0;
        mPointStaticCVs = 0;
        mPointAnimCVs = 0;

        mNurbsStaticNum = 0;
        mNurbsAnimNum = 0;
        mNurbsStaticCVs = 0;
        mNurbsAnimCVs = 0;

        mTransStaticNum = 0;
        mTransAnimNum = 0;

        mLocatorStaticNum = 0;
        mLocatorAnimNum = 0;

        mCameraStaticNum = 0;
        mCameraAnimNum = 0;
    }

    // for the statistic string
    unsigned int mSubDStaticNum;
    unsigned int mSubDAnimNum;
    unsigned int mSubDStaticFaces;
    unsigned int mSubDAnimFaces;
    unsigned int mSubDStaticCVs;
    unsigned int mSubDAnimCVs;

    unsigned int mPolyStaticNum;
    unsigned int mPolyAnimNum;
    unsigned int mPolyStaticFaces;
    unsigned int mPolyAnimFaces;
    unsigned int mPolyStaticCVs;
    unsigned int mPolyAnimCVs;

    unsigned int mCurveStaticNum;
    unsigned int mCurveStaticCurves;
    unsigned int mCurveAnimNum;
    unsigned int mCurveAnimCurves;
    unsigned int mCurveStaticCVs;
    unsigned int mCurveAnimCVs;

    unsigned int mPointStaticNum;
    unsigned int mPointAnimNum;
    unsigned int mPointStaticCVs;
    unsigned int mPointAnimCVs;

    unsigned int mNurbsStaticNum;
    unsigned int mNurbsAnimNum;
    unsigned int mNurbsStaticCVs;
    unsigned int mNurbsAnimCVs;

    unsigned int mTransStaticNum;
    unsigned int mTransAnimNum;

    unsigned int mLocatorStaticNum;
    unsigned int mLocatorAnimNum;

    unsigned int mCameraStaticNum;
    unsigned int mCameraAnimNum;
};

class AbcWriteJob
{
  public:

    AbcWriteJob(const char * iFileName, bool asOgawa,
        std::set<double> & iTransFrames,
        Alembic::AbcCoreAbstract::TimeSamplingPtr iTransTime,
        std::set<double> & iShapeFrames,
        Alembic::AbcCoreAbstract::TimeSamplingPtr iShapeTime,
        const JobArgs & iArgs);

    ~AbcWriteJob();

    // returns true if eval has been called on the last frame
    bool eval(double iFrame);

  private:

    void perFrameCallback(double iFrame);
    void postCallback(double iFrame);

    MBoundingBox getBoundingBox(double iFrame, const MMatrix & eMInvMat);
    void setup(double iFrame, MayaTransformWriterPtr iParent);

    // Currently Arnold and Renderman can not handle curve groups where the
    // degrees and closed status are different per curve.
    // return true if the degree and close state of each curve
    // in the group is the same
    bool checkCurveGrp();

    std::vector< MayaTransformWriterPtr > mTransList;
    std::vector< AttributesWriterPtr > mTransAttrList;

    std::vector< MayaCameraWriterPtr > mCameraList;
    std::vector< MayaMeshWriterPtr > mMeshList;
    std::vector< MayaNurbsCurveWriterPtr > mCurveList;
    std::vector< MayaNurbsSurfaceWriterPtr > mNurbsList;
    std::vector< MayaLocatorWriterPtr > mLocatorList;
    std::vector< MayaPointPrimitiveWriterPtr > mPointList;
    std::vector< AttributesWriterPtr > mShapeAttrList;

    // helper dag path map for bounding box calculation
    std::map< MDagPath, util::ShapeSet, util::cmpDag > mBBoxShapeMap;

    // helper dag path for recursive calculations
    MDagPath mCurDag;

    // the root world node of the scene
    Alembic::Abc::OArchive mRoot;

    std::string mFileName;
    bool mAsOgawa;

    MSelectionList mSList;
    std::set<double> mShapeFrames;
    Alembic::AbcCoreAbstract::TimeSamplingPtr mShapeTime;
    Alembic::Util::uint32_t mShapeTimeIndex;
    Alembic::Util::uint32_t mShapeSamples;

    std::set<double> mTransFrames;
    Alembic::AbcCoreAbstract::TimeSamplingPtr mTransTime;
    Alembic::Util::uint32_t mTransTimeIndex;
    Alembic::Util::uint32_t mTransSamples;

    // when eval is called and the time is the first frame
    // then we run the setup
    double mFirstFrame;

    // when eval is called and the time is the last frame
    // then we also call the post callback
    double mLastFrame;

    Alembic::Abc::OBox3dProperty mBoxProp;
    unsigned int mBoxIndex;

    AbcWriteJobStatistics mStats;
    JobArgs mArgs;
};

typedef Alembic::Util::shared_ptr < AbcWriteJob > AbcWriteJobPtr;

#endif  // AbcExport_AbcWriteJob_h
