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

#ifndef _AbcExport_AbcWriteJob_h_
#define _AbcExport_AbcWriteJob_h_

#include "Foundation.h"

#include "MayaTransformWriter.h"
#include "MayaMeshWriter.h"
#include "MayaPointPrimitiveWriter.h"

/*
#include "MayaNurbsSurfaceWriter.h"
#include "MayaNurbsCurveWriter.h"
#include "MayaCameraWriter.h"
#include "MayaLightWriter.h"
#include "MayaLocatorWriter.h"
*/

#include "MayaUtility.h"

typedef boost::shared_ptr < MayaMeshWriter >
    MayaMeshWriterPtr;
//typedef boost::shared_ptr < MayaNurbsSurfaceWriter >
//    MayaNurbsSurfaceWriterPtr;
//typedef boost::shared_ptr < MayaNurbsCurveWriter >
//    MayaNurbsCurveWriterPtr;
//typedef boost::shared_ptr < MayaCameraWriter >
//    MayaCameraWriterPtr;
//typedef boost::shared_ptr < MayaLightWriter >
//    MayaLightWriterPtr;
//typedef boost::shared_ptr < MayaLocatorWriter >
//    MayaLocatorWriterPtr;
typedef boost::shared_ptr < MayaPointPrimitiveWriter >
    MayaPointPrimitiveWriterPtr;

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

        mCameraStaticNum = 0;
        mCameraAnimNum = 0;

        mGenericNum = 0;
    }

    // for the statistic string
    size_t mSubDStaticNum;
    size_t mSubDAnimNum;
    size_t mSubDStaticFaces;
    size_t mSubDAnimFaces;
    size_t mSubDStaticCVs;
    size_t mSubDAnimCVs;

    size_t mPolyStaticNum;
    size_t mPolyAnimNum;
    size_t mPolyStaticFaces;
    size_t mPolyAnimFaces;
    size_t mPolyStaticCVs;
    size_t mPolyAnimCVs;

    size_t mCurveStaticNum;
    size_t mCurveStaticCurves;
    size_t mCurveAnimNum;
    size_t mCurveAnimCurves;
    size_t mCurveStaticCVs;
    size_t mCurveAnimCVs;

    size_t mPointStaticNum;
    size_t mPointAnimNum;
    size_t mPointStaticCVs;
    size_t mPointAnimCVs;

    size_t mNurbsStaticNum;
    size_t mNurbsAnimNum;
    size_t mNurbsStaticCVs;
    size_t mNurbsAnimCVs;

    size_t mTransStaticNum;
    size_t mTransAnimNum;

    size_t mCameraStaticNum;
    size_t mCameraAnimNum;

    size_t mGenericNum;
};

class AbcWriteJob
{
  public:

    AbcWriteJob(const util::ShapeSet & iDagPath,
        const char * iFileName,
        bool iUseSelectionList,
        bool iWorldSpace,
        bool iWriteVisibility,
        bool iWriteUVs,
        std::set<double> & iTransFrames,
        Alembic::AbcCoreAbstract::v1::TimeSamplingType & iTransTimeType,
        std::set<double> & iShapeFrames,
        Alembic::AbcCoreAbstract::v1::TimeSamplingType & iShapeTimeType,
        std::string & iMelPerFrameCallback,
        std::string & iMelPostCallback,
        std::string & iPythonPerFrameCallback,
        std::string & iPythonPostCallback,
        std::string & iPrefixFilter,
        std::set< std::string > & iAttribs);

    ~AbcWriteJob();

    // returns true if eval has been called on the last frame
    bool eval(double iFrame);

  private:
    typedef boost::variant<

    /*
        MayaNurbsSurfaceWriterPtr,
        MayaNurbsCurveWriterPtr,
        MayaCameraWriterPtr,
        MayaLightWriterPtr,
        MayaLocatorWriterPtr,
    */
        MayaMeshWriterPtr,
        MayaPointPrimitiveWriterPtr > MayaNodePtr;

    void perFrameCallback(double iFrame);
    void postCallback(double iFrame);

    void getBoundingBox(const MMatrix & eMInvMat);
    void setup(double iFrame, MayaTransformWriterPtr iParent);

    // Currently Arnold and Renderman can not handle curve groups where the
    // degrees and closed status are different per curve.
    // return true if the degree and close state of each curve
    // in the group is the same
    bool checkCurveGrp();

    std::vector< MayaTransformWriterPtr > mTransList;
    std::vector< AttributesWriterPtr > mTransAttrList;

    std::vector< MayaNodePtr > mShapeList;
    std::vector< AttributesWriterPtr > mShapeAttrList;

    // helper bounding box for recursive calculation
    MBoundingBox mCurBBox;

    // helper dag path for recursive calculations
    MDagPath mCurDag;

    // list of root transform dag paths
    util::ShapeSet mDagPath;

    // the root world node of the scene
    Alembic::Abc::OArchive mRoot;

    std::string mFileName;

    // whether or not we are writing nodes based on selection
    bool mUseSelectionList;

    // whether our root transforms (in mDagPath) should also have
    // all of it's ancestors
    bool mWorldSpace;

    // whether or not to write visibility in a Katana-esque way
    // if false then visibility state will not be written out
    bool mWriteVisibility;

    // whether or not to bake current uv set of polygons and subD meshs to file
    bool mWriteUVs;

    MSelectionList mSList;
    std::set<double> mShapeFrames;
    Alembic::AbcCoreAbstract::v1::TimeSamplingType mShapeTimeType;
    std::set<double> mTransFrames;
    Alembic::AbcCoreAbstract::v1::TimeSamplingType mTransTimeType;

    // convenience booleans used mainly during setup
    // indicates whether the shapes and transforms
    // only have 1 sample in their list
    bool mShapesStatic;
    bool mTransStatic;

    // when eval is called and the time is the first frame
    // then we run the setup
    double mFirstFrame;

    // when eval is called and the time is the last frame
    // then we also call the post callback
    double mLastFrame;

    Alembic::Abc::OBox3dProperty mBoxProp;
    unsigned int mBoxIndex;

    // for the callbacks
    std::string mMelPerFrameCallback;
    std::string mMelPostCallback;
    std::string mPythonPerFrameCallback;
    std::string mPythonPostCallback;

    // filtering for attributes
    std::string mFilter;
    std::set<std::string> mAttribs;

    AbcWriteJobStatistics mStats;
};

typedef boost::shared_ptr < AbcWriteJob > AbcWriteJobPtr;

#endif  // _AbcExport_AbcWriteJob_h_
