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

#ifndef _AlembicTakoImport_NodeIteratorVisitorHelper_h_
#define _AlembicTakoImport_NodeIteratorVisitorHelper_h_

#include <AlembicTakoImport/Foundation.h>

namespace Alembic {

/// static visitor that adds attribute of type Property to a DG node
class AddAttrVisitor : public boost::static_visitor<>
{
public:

    AddAttrVisitor(std::string iAttrName, MObject & iParent,
        Tako::ArbAttrInfo & iInfo);

    void operator()(const int8_t  iProp);
    void operator()(const int16_t iProp);
    void operator()(const int32_t iProp);
    void operator()(const int64_t iProp);
    void operator()(const float   iProp);
    void operator()(const double  iProp);
    void operator()(const std::string & iProp);
    void operator()(const std::vector<int16_t> & iProp);
    void operator()(const std::vector<int32_t> & iProp);
    void operator()(const std::vector<int64_t> & iProp);
    void operator()(const std::vector<float> & iProp);
    void operator()(const std::vector<double> & iProp);
    void operator()(const std::vector<std::string> & iProp);

private:
    void addString(const std::string & iProp);
    void addArbAttrAndScope();

    std::string mAttrName;

    Tako::ScopeType   mScope;
    Tako::ArbAttrType mInputType;
    Tako::ArbAttrType mOutputType;

    // the object the attributes will be attached to
    MObject mParent;
};


void addProperties(float iFrame, Tako::HDFReaderNode & iNode,
    MObject iObject, std::vector<std::string> & iSampledPropNameList);

//=============================================================================
//
// Polygon mesh is treated differently when adding and updating props because
// the optional uv data is written as an arbitrary attr of the mesh node.
//
void addPolyProperties(float iFrame,
    const Tako::PolyMeshReaderPtr & iNode, MObject iObject,
    std::vector<std::string> & iSampledPropNameList);

//
class getPolyUVsVisitor : public boost::static_visitor<>
{
public:

    void operator()(const int8_t  iProp) {}
    void operator()(const int16_t iProp) {}
    void operator()(const int32_t iProp) {}
    void operator()(const int64_t iProp) {}
    void operator()(const float   iProp) {}
    void operator()(const double  iProp) {}
    void operator()(const std::string & iProp) {}
    void operator()(const std::vector<int16_t> & iProp) {}
    void operator()(const std::vector<int32_t> & iProp) {}
    void operator()(const std::vector<int64_t> & iProp) {}
    void operator()(const std::vector<float> & iProp) { mFloatVec = iProp;}
    void operator()(const std::vector<double> & iProp) {}
    void operator()(std::vector<std::string> &  iProp) {}

    std::vector<float>  mFloatVec;
};  // class getPolyUVsVisitor



//=============================================================================


//
// This class is used when sampled properties or sampled transform operations
// exist, to keep the list of names of sampled channels associated with a
// particular MObject (mObject)
//
class SampledPair
{
public:
    SampledPair(MObject & iObject, std::vector<std::string> & sN):
        mObject(iObject), sampleNameList(sN)
    {
    }

    MObject & getObject()
    {
        return mObject;
    }

    unsigned int sampledChannelSize()
    {
        return sampleNameList.size();
    }

    std::string getSampleElement(unsigned int index)
    {
        if (index < sampleNameList.size())
            return sampleNameList[index];
        else
            return 0;
    }

private:
    MObject mObject;
    std::vector<std::string> sampleNameList;
};  // SampledPair


// ============================================================================

// reads sampled props and gives access to the sampled property map begin and
// end iterators
class UpdatePropVisitor : public boost::static_visitor< >
{
public:

    UpdatePropVisitor(double iFrame) : mFrame(iFrame) {}

    template<typename NodeType>
    void operator()(NodeType & iNode)
    {
        iNode->readProperties(mFrame);
        mSampBegin = iNode->beginSampledProperties();
        mSampEnd = iNode->endSampledProperties();
    }

    Tako::PropertyMap::const_iterator begin()
    {
        return mSampBegin;
    };

    Tako::PropertyMap::const_iterator end()
    {
        return mSampEnd;
    };

private:

    MObject mObject;
    double  mFrame;
    Tako::PropertyMap::const_iterator mSampBegin;
    Tako::PropertyMap::const_iterator mSampEnd;
};  // UpdatePropVisitor

// ============================================================================


class ReadPropAttrVisitor :  public boost::static_visitor< MStatus >
{
public:

    ReadPropAttrVisitor(double iFrame) : mFrame(iFrame) {}

    void setArbAttrInfo(const Tako::ArbAttrInfo & iArbAttrInfo);
    void setHandle(MDataHandle & iHandle);
    void setPlug(MPlug & iPlug);

    MStatus operator()(const int8_t  & iProp);
    MStatus operator()(const int16_t & iProp);
    MStatus operator()(const int32_t & iProp);
    MStatus operator()(const int64_t & iProp);
    MStatus operator()(const float   & iProp);
    MStatus operator()(const double  & iProp);
    MStatus operator()(const std::vector<int16_t> & iProp);
    MStatus operator()(const std::vector<int32_t> & iProp);
    MStatus operator()(const std::vector<int64_t> & iProp);
    MStatus operator()(const std::vector<float>   & iProp);
    MStatus operator()(const std::vector<double>  & iProp);

    MStatus operator()(const std::string & iProp);
    MStatus operator()(const std::vector<std::string> & iProp);

private:
    double  mFrame;
    Tako::ArbAttrInfo mArbAttrInfo;

    unsigned int mMode;  // 0, set datahandle; 1, set plug

    MDataHandle mHandle;
    MPlug mPlug;
};  // class ReadPropAttrVisitor


// A data class for cleaner code when copying a group of info between functions
class WriterData
{
public:
    WriterData();
    WriterData(const WriterData & rhs);
    WriterData & operator=(const WriterData & rhs);

    std::vector<MObject>        mSubDObjList;
    std::vector<MObject>        mPolyObjList;
    std::vector<MObject>        mCameraObjList;
    std::vector<MObject>        mParticleObjList;
    std::vector<SampledPair>  mTransOpList;
    std::vector<MObject>        mNurbsSurfaceObjList;
    std::vector<MObject>        mNurbsCurveObjList;
    std::vector<SampledPair>  mPropList;

    std::vector<Tako::SubDReaderPtr>
                                mSubDNodePtrList;
    std::vector<Tako::PolyMeshReaderPtr>
                                mPolyNodePtrList;
    std::vector<Tako::CameraReaderPtr>
                                mCameraNodePtrList;
    std::vector<Tako::PointPrimitiveReaderPtr>
                                mParticleNodePtrList;
    std::vector<Tako::TransformReaderPtr>
                                mTransOpNodePtrList;
    std::vector<Tako::NurbsSurfaceReaderPtr>
                                mNurbsSurfaceNodePtrList;
    std::vector<Tako::NurbsCurveReaderPtr>
                                mNurbsCurveNodePtrList;
    std::vector<Tako::ChildNodePtr>
                                mPropNodePtrList;

    std::vector<bool>           mIsSampledTransOpAngle;
    std::vector<bool>           mIsComplexTrans;
    std::vector<unsigned int>   mNurbsCurveNumCurveList;
    std::vector<float>          mParticlePos;
    std::vector<double>         mParticleIds;
};  // WriterData

class ArgData
{
public:
    ArgData(MString iFileName = "",
        double  iSequenceStartFrame = FLT_MAX,
        double  iSequenceEndFrame = FLT_MAX,
        bool    iDebugOn = false,
        MObject iReparentObj = MObject::kNullObj,
        bool    iConnect = false,
        MString iConnectRootNodes = MString("/"),
        bool    iCreateIfNotFound = false,
        bool    iRemoveIfNoUpdate = false,
        bool    iLoadUVs = false,
        bool    iLoadNormals = false);
    ArgData(const ArgData & rhs);
    ArgData & operator=(const ArgData & rhs);

    MString     mFileName;
    double      mSequenceStartFrame;
    double      mSequenceEndFrame;
    bool        mDebugOn;

    MObject     mReparentObj;

    // optional information for the "connect" flag
    bool        mConnect;
    MString     mConnectRootNodes;
    bool        mCreateIfNotFound;
    bool        mRemoveIfNoUpdate;

    // optional information for loading normals and(or) uvs
    bool        mLoadUVs;
    bool        mLoadNormals;

    WriterData  mData;
};  // ArgData

// traverse and create the corresponding maya hierarchy
MString createScene(ArgData & iArgData);

// Called in createScene only if there are sampled data in the scene.
// This function sets up the connections to AlembicTakoCacheNode
MString connectAttr(ArgData & iArgData);

} // End namespace Alembic

#endif  // _AlembicTakoImport_NodeIteratorVisitorHelper_h_
