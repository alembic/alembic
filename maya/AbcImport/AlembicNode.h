//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#ifndef ABCIMPORT_ALEMBIC_NODE_H_
#define ABCIMPORT_ALEMBIC_NODE_H_

#include "NodeIteratorVisitorHelper.h"

#include <maya/MDataHandle.h>
#include <maya/MDGContext.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>

#include <set>
#include <vector>
#include <string>

class AlembicNode : public MPxNode
{
public:

    AlembicNode() : mFileInitialized(0), mDebugOn(false)
    {
        mCurTime = DBL_MAX;

        // 0 mOutPropArrayAttr
        // 1 mOutTransOpArrayAttr
        // 2 mOutSubDArrayAttr
        // 3 mOutPolyArrayAttr
        // 4 mOutCameraArrayAttr
        // 5 mOutNurbsSurfaceArrayAttr
        // 6 mOutNurbsCurveGrpArrayAttr
        // 7 mOutParticlePosArrayAttr, mOutParticleIdArrayAttr
        // 8 mOutLocatorPosScaleArrayAttr
        mOutRead = std::vector<bool>(9, false);
    }

    virtual ~AlembicNode() {}

    // avoid calling createSceneVisitor twice by getting the
    // list of hdf reader pointers
    void setReaderPtrList(const WriterData & iData)
    {
        mData = iData;
    }

    static const MTypeId mMayaNodeId;

    // input attributes
    static MObject mTimeAttr;
    static MObject mAbcFileNameAttr;
    static MObject mAbcLayerFileNamesAttr;
    static MObject mSpeedAttr;
    static MObject mOffsetAttr;
    static MObject mCycleTypeAttr;
    static MObject mIncludeFilterAttr;
    static MObject mExcludeFilterAttr;

    // output attributes
    static MObject mOutPropArrayAttr;
    static MObject mOutSubDArrayAttr;
    static MObject mOutPolyArrayAttr;
    static MObject mOutCameraArrayAttr;
    static MObject mOutNurbsCurveGrpArrayAttr;
    static MObject mOutNurbsSurfaceArrayAttr;
    static MObject mOutTransOpArrayAttr;
    static MObject mOutLocatorPosScaleArrayAttr;

    // output informational attrs
    static MObject mStartFrameAttr;
    static MObject mEndFrameAttr;

    // override virtual methods from MPxNode
    virtual MStatus compute(const MPlug & plug, MDataBlock & dataBlock);

    // return a pointer to a new instance of the class
    // (derived from MPxNode) that implements the new node type
    static void* creator() { return (new AlembicNode()); }

    // override virtual methods from MPxNode
    virtual MStatus setDependentsDirty(const MPlug& plug, MPlugArray& plugArray);
    virtual bool isPassiveOutput(const MPlug & plug) const;
#if MAYA_API_VERSION >= 201600
    virtual SchedulingType schedulingType()const;
#endif

    // initialize all the attributes to default values
    static MStatus initialize();

    // override virtual methods from MPxNode, returns the list of
    // files to archive.
    virtual MStringArray getFilesToArchive(bool shortName,
                                           bool unresolvedName,
                                           bool markCouldBeImageSequence) const;
#if defined(MAYA_WANT_EXTERNALCONTENTTABLE)
    virtual void getExternalContent(MExternalContentInfoTable& table) const;
    virtual void setExternalContent(const MExternalContentLocationTable& table);
#endif

    void   setDebugMode(bool iDebugOn){ mDebugOn = iDebugOn; }
    void   setIncludeFilterString(const MString & iIncludeFilterString)
    {
        mIncludeFilterString = iIncludeFilterString;
    }
    void   setExcludeFilterString(const MString & iExcludeFilterString)
    {
        mExcludeFilterString = iExcludeFilterString;
    }

private:
    // compute the adjusted time from inputTime, speed and time offset.
    double computeAdjustedTime(const double inputTime,
                               const double speed,
                               const double timeOffset);

    // retime based on the playstyle: hold(use current time), loop and bound
    enum
    {
        PLAYTYPE_HOLD = 0,
        PLAYTYPE_LOOP,
        PLAYTYPE_REVERSE,
        PLAYTYPE_BOUNCE
    };
    double computeRetime(const double inputTime,
                         const double firstTime,
                         const double lastTime,
                         const short playStyle);
    double getFPS();

    // flag indicating if the input file should be opened again
    bool    mFileInitialized;

    // flag indicating either this is the first time a mesh plug is computed or
    // there's a topology change from last frame to this one
    bool    mSubDInitialized;
    bool    mPolyInitialized;

    double   mSequenceStartTime;
    double   mSequenceEndTime;
    double   mCurTime;

    bool    mDebugOn;

    // bool for each output plug, (the 2 transform plugs are lumped together,
    // when updating) this is to prevent rereading the same
    // frame when above or below the frame range
    std::vector<bool> mOutRead;

    bool    mConnect;
    bool    mCreateIfNotFound;
    bool    mRemoveIfNoUpdate;
    MString mConnectRootNodes;
    MString mIncludeFilterString;
    MString mExcludeFilterString;

    WriterData mData;
};

#endif  // ABCIMPORT_ALEMBIC_NODE_H_
