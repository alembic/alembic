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

#include <AlembicTakoImport/MeshHelper.h>
#include <AlembicTakoImport/Util.h>
#include <AlembicTakoImport/LightHelper.h>
#include <AlembicTakoImport/LocatorHelper.h>
#include <AlembicTakoImport/CameraHelper.h>
#include <AlembicTakoImport/PointPrimitiveHelper.h>
#include <AlembicTakoImport/NurbsSurfaceHelper.h>
#include <AlembicTakoImport/NurbsCurveHelper.h>
#include <AlembicTakoImport/TransformHelper.h>
#include <AlembicTakoImport/CreateSceneVisitor.h>

namespace Alembic {

using Tako::ChildNodePtr;

namespace
{
    void removeDagNode(bool removeIfNotUpdate, MDagPath & dagPath)
    {
        if (removeIfNotUpdate)
        {
            MStatus status = deleteDagNode(dagPath);
            if ( status != MS::kSuccess )
            {
                MString theError = dagPath.partialPathName();
                theError += MString(" removal not successful");
                printError(theError);
            }
        }
    }  // removeDagNode

    // a convenience function for getting the name of a node if the node type
    // is Tako::ChildNodePtr
    class GetNameVisitor : public boost::static_visitor< std::string >
    {
      public:

        template<typename NodeType>
        std::string operator()(NodeType & iNode)
        {
            return iNode->getName();
        }
    };  // GetNameVisitor
}


CreateSceneVisitor::CreateSceneVisitor(double iFrame,
    const MObject & iParent, bool iNotCreate) :
    mFrame(iFrame), mParent(iParent), mNotCreate(iNotCreate)
{
}

CreateSceneVisitor::~CreateSceneVisitor()
{
}

void CreateSceneVisitor::setLoadUVNormalState(bool iLoadUVs, bool iLoadNormals)
{
    mLoadUVs = iLoadUVs;
    mLoadNormals = iLoadNormals;
}

void CreateSceneVisitor::setConnectArgs(
    bool iConnect, MString iConnectRootNodes,
    bool iCreateIfNotFound, bool iRemoveIfNoUpdate)
{
    mConnect = iConnect;
    mCreateIfNotFound = iCreateIfNotFound;
    mRemoveIfNoUpdate = iRemoveIfNoUpdate;
    mCurrentConnectAction = NONE;
    mConnectRootNodes.clear();
    mConnectUpdateNodes.clear();

    // parse the input string to extract the nodes that need (re)connection
    if ( iConnectRootNodes != MString("/") )
    {
        MStringArray theArray;
        if (iConnectRootNodes.split(' ', theArray) == MS::kSuccess)
        {
            unsigned int len = theArray.length();
            for (unsigned int i = 0; i < len; i++)
            {
                MString name = theArray[i];
                // the name could be either a partial path or a full path
                MDagPath dagPath;
                if ( getDagPathByName( name, dagPath ) == MS::kSuccess )
                {
                    name = dagPath.partialPathName();
                }

                mConnectRootNodes.insert(name.asChar());
            }
        }
    }
}

void CreateSceneVisitor::getData(WriterData & oData)
{
    oData = mData;
}

bool CreateSceneVisitor::hasSampledData()
{
    unsigned int subDSize = mData.mSubDNodePtrList.size();
    unsigned int polySize = mData.mPolyNodePtrList.size();
    unsigned int cameraSize = mData.mCameraNodePtrList.size();
    // Currently there's no support for bringing in particle system simulation
    // unsigned int particleSize = mData.mParticleNodePtrList.size();
    unsigned int transopSize = mData.mTransOpNodePtrList.size();
    unsigned int nSurfaceSize  = mData.mNurbsSurfaceNodePtrList.size();
    unsigned int nCurveSize  = mData.mNurbsCurveNodePtrList.size();
    unsigned int propSize = mData.mPropNodePtrList.size();

    return ( subDSize > 0 || polySize > 0 || nSurfaceSize > 0 || nCurveSize > 0
            || transopSize > 0 || cameraSize > 0  // || particleSize > 0
            || propSize > 0);
}

void CreateSceneVisitor::applyShaderSelection()
{
    std::map < MObject, MSelectionList, ltMObj >::iterator i =
        mShaderMeshMap.begin();

    std::map < MObject, MSelectionList, ltMObj >::iterator end =
        mShaderMeshMap.end();

    for (; i != end; ++i)
    {
        MFnSet curSet(i->first);
        curSet.addMembers(i->second);
    }
    mShaderMeshMap.clear();
}

void CreateSceneVisitor::checkShaderSelection(MFnMesh & iMesh,
    unsigned int iInst)
{
    MObjectArray sets;
    MObjectArray comps;

    iMesh.getConnectedSetsAndMembers(iInst, sets, comps, false);
    unsigned int setsLength = sets.length();
    for (unsigned int i = 0; i < setsLength; ++i)
    {
        MObject & curSetObj = sets[i];
        if (mShaderMeshMap.find(curSetObj) == mShaderMeshMap.end())
        {
            MFnSet curSet(curSetObj);
            MSelectionList & curSel = mShaderMeshMap[curSetObj];
            curSet.getMembers(curSel, true);

            // clear before hand so that when we add the selection to the
            // set later, it will take.  This is to get around a problem where
            // shaders are assigned per face dont shade correctly
            curSet.clear();
        }
    }
}

MStatus CreateSceneVisitor::operator()(const Tako::GenericNodeReaderPtr& iNode)
{
    MStatus status = MS::kSuccess;
    MObject genericObj = MObject::kNullObj;

    if (mNotCreate == 1)
    {
        if (iNode->hasPropertyFrames())
            mData.mPropNodePtrList.push_back(iNode);
        return status;
    }

    std::vector<std::string> mPropNameList;
    if ( iNode->mUserType == "light" )
    {
        status =
            createLight(mFrame, iNode, mParent, genericObj, mPropNameList);
    }
    else if ( iNode->mUserType == "locator" )
    {
        status =
            createLocator(mFrame, iNode, mParent, genericObj, mPropNameList);
    }
    else
    {
        MString theError(iNode->mUserType.c_str());
        theError += MString(" is not a supported generic node type");
        printError(theError);
    }

    if ( iNode->hasPropertyFrames() )
    {
        SampledPair mSampledPair(genericObj, mPropNameList);
        mData.mPropList.push_back(mSampledPair);
        mData.mPropNodePtrList.push_back(iNode);
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(
    const Tako::PointPrimitiveReaderPtr& iNode)
{
    MStatus status = MS::kSuccess;
    MObject particleObj = MObject::kNullObj;

    if (mNotCreate == 1)
    {
        if (iNode->hasFrames())
            mData.mParticleNodePtrList.push_back(iNode);
        if (iNode->hasPropertyFrames())
            mData.mPropNodePtrList.push_back(iNode);
        return status;
    }

    std::vector<std::string> mPropNameList;
    status = create(mFrame, iNode, mParent, particleObj, mPropNameList);
    if (iNode->hasFrames())
    {
        mData.mParticleObjList.push_back(particleObj);
        mData.mParticleNodePtrList.push_back(iNode);
    }
    if (iNode->hasPropertyFrames())
    {
        SampledPair mSampledPair(particleObj, mPropNameList);
        mData.mPropList.push_back(mSampledPair);
        mData.mPropNodePtrList.push_back(iNode);
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(const Tako::NurbsCurveReaderPtr& iNode)
{
    MStatus status = MS::kSuccess;
    std::vector< MObject > curveObjects;

    if (mNotCreate == 1)
    {
        if (iNode->hasFrames())
            mData.mNurbsCurveNodePtrList.push_back(iNode);
        if (iNode->hasPropertyFrames())
            mData.mPropNodePtrList.push_back(iNode);

        return status;
    }

    std::vector<std::string> mPropNameList;

    if ( mConnect == true && mCurrentConnectAction == CONNECT )
    {
        MObject currentObject = mCurrentDagNode.node();

        // check that the data types are compatible:
        // the original object could be a curve shape object which is
        // compatible with MFn::kNurbsCurve, or a transform node
        // which indicating it is a curve group, and in which case currently
        // there's no support for connect operation
        //
        if ( currentObject.hasFn(MFn::kNurbsCurve)
            || currentObject.hasFn(MFn::kTransform))
        {
            mParent = mCurrentDagNode.transform(&status);
            MObject obj = create(
                mFrame, iNode, mParent, curveObjects, mPropNameList, mConnect);

            if (iNode->hasFrames())
            {
                mData.mNurbsCurveNodePtrList.push_back(iNode);
                unsigned int numCurves = curveObjects.size();
                mData.mNurbsCurveNumCurveList.push_back(numCurves);
                for (unsigned int i = 0; i < numCurves; i++)
                    mData.mNurbsCurveObjList.push_back(curveObjects[i]);
            }
            if (iNode->hasPropertyFrames())
            {
                SampledPair mSampledPair(obj, mPropNameList);
                mData.mPropList.push_back(mSampledPair);
                mData.mPropNodePtrList.push_back(iNode);
            }
        }
        else
        {
            MString theError("No connection done for node \"");
            theError += MString(iNode->getName());
            theError += MString("\" with ");
            theError += mCurrentDagNode.fullPathName();
            printError(theError);
            return status;
        }
        return status;
    }

    MObject obj = create(mFrame, iNode, mParent, curveObjects, mPropNameList);

    if (status == MS::kSuccess)
    {
        if (iNode->hasFrames())
        {
            mData.mNurbsCurveNodePtrList.push_back(iNode);
            unsigned int numCurves = curveObjects.size();
            mData.mNurbsCurveNumCurveList.push_back(numCurves);
            for (unsigned int i = 0; i < numCurves; i++)
                mData.mNurbsCurveObjList.push_back(curveObjects[i]);
        }

        if (iNode->hasPropertyFrames())
        {
            SampledPair mSampledPair(obj, mPropNameList);
            mData.mPropList.push_back(mSampledPair);
            mData.mPropNodePtrList.push_back(iNode);
        }
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(const Tako::CameraReaderPtr& iNode)
{
    MStatus status = MS::kSuccess;
    MObject camObj = MObject::kNullObj;

    if (mNotCreate == 1)
    {
        if (iNode->hasFrames())
            mData.mCameraNodePtrList.push_back(iNode);
        if (iNode->hasPropertyFrames())
            mData.mPropNodePtrList.push_back(iNode);
        return status;
    }

    std::vector<std::string> mPropNameList;
    status = create(mFrame, iNode, mParent, camObj, mPropNameList);
    if (iNode->hasFrames())
    {
        mData.mCameraObjList.push_back(camObj);
        mData.mCameraNodePtrList.push_back(iNode);
    }
    if (iNode->hasPropertyFrames())
    {
        SampledPair mSampledPair(camObj, mPropNameList);
        mData.mPropList.push_back(mSampledPair);
        mData.mPropNodePtrList.push_back(iNode);
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(
    const Tako::NurbsSurfaceReaderPtr& iNode)
{
    MStatus status = MS::kSuccess;
    MObject surfaceObj = MObject::kNullObj;

    if (mNotCreate == 1)
    {
        if (iNode->hasFrames())
            mData.mNurbsSurfaceNodePtrList.push_back(iNode);
        if (iNode->hasPropertyFrames())
            mData.mPropNodePtrList.push_back(iNode);

        return status;
    }

    if ( mConnect == true && mCurrentConnectAction == CONNECT )
    {
        surfaceObj = mCurrentDagNode.node();
        MFnNurbsSurface mFn(surfaceObj, &status);
        if ( status == MS::kSuccess )
        {
            std::vector<std::string> mPropNameList;
            mParent = mCurrentDagNode.transform(&status);

            status = create(mFrame, iNode, mParent, surfaceObj, mPropNameList, true);

            if (iNode->hasFrames())
            {
                mData.mNurbsSurfaceObjList.push_back(surfaceObj);
                mData.mNurbsSurfaceNodePtrList.push_back(iNode);
            }

            if (iNode->hasPropertyFrames())
            {
                SampledPair mSampledPair(surfaceObj, mPropNameList);
                mData.mPropList.push_back(mSampledPair);
                mData.mPropNodePtrList.push_back(iNode);
            }
        }
        else
        {
            MString theError("No connection done for node \"");
            theError += MString(iNode->getName());
            theError += MString("\" with ");
            theError += mCurrentDagNode.fullPathName();
            printError(theError);
            return status;
        }
        return status;
    }

    std::vector<std::string> mPropNameList;
    status = create(mFrame, iNode, mParent, surfaceObj, mPropNameList);

    if (iNode->hasFrames())
    {
        mData.mNurbsSurfaceObjList.push_back(surfaceObj);
        mData.mNurbsSurfaceNodePtrList.push_back(iNode);
    }

    if (iNode->hasPropertyFrames())
    {
        SampledPair mSampledPair(surfaceObj, mPropNameList);
        mData.mPropList.push_back(mSampledPair);
        mData.mPropNodePtrList.push_back(iNode);
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(const Tako::SubDReaderPtr& iNode)
{
    MStatus status = MS::kSuccess;
    MObject subDObj = MObject::kNullObj;

    if (mNotCreate == 1)
    {
        if (iNode->hasFrames())
            mData.mSubDNodePtrList.push_back(iNode);
        if (iNode->hasPropertyFrames())
            mData.mPropNodePtrList.push_back(iNode);

        return status;
    }

    if ( mConnect == true && mCurrentConnectAction == CONNECT )
    {
        subDObj = mCurrentDagNode.node();
        MFnMesh mFn(subDObj, &status);

        // check that the data types are compatible
        if ( status == MS::kSuccess )
        {
            std::vector<std::string> mPropNameList;
            mParent = mCurrentDagNode.transform(&status);

            if (iNode->hasFrames())
            {
                status = createSubD(mFrame, iNode, mParent, subDObj, mPropNameList, 1);
                mData.mSubDObjList.push_back(subDObj);
                mData.mSubDNodePtrList.push_back(iNode);
            }
            else
            {
                status = createSubD(mFrame, iNode, mParent, subDObj, mPropNameList, 1,
                    mLoadUVs, mLoadNormals);
            }

            if (iNode->hasPropertyFrames())
            {
                SampledPair mSampledPair(subDObj, mPropNameList);
                mData.mPropList.push_back(mSampledPair);
                mData.mPropNodePtrList.push_back(iNode);
            }

            checkShaderSelection(mFn, mCurrentDagNode.instanceNumber());
        }
        else
        {
            MString theError("No connection done for node \"");
            theError += MString(iNode->getName());
            theError += MString("\" with ");
            theError += mCurrentDagNode.fullPathName();
            printError(theError);
            return status;
        }
        return status;
    }

    std::vector<std::string> mPropNameList;

    if (iNode->hasFrames())
    {
        status = createSubD(mFrame, iNode, mParent, subDObj, mPropNameList);
        mData.mSubDObjList.push_back(subDObj);
        mData.mSubDNodePtrList.push_back(iNode);
    }
    else
    {
        status = createSubD(mFrame, iNode, mParent, subDObj, mPropNameList, 0,
            mLoadUVs, mLoadNormals);
    }
    if (iNode->hasPropertyFrames())
    {
        SampledPair mSampledPair(subDObj, mPropNameList);
        mData.mPropList.push_back(mSampledPair);
        mData.mPropNodePtrList.push_back(iNode);
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(const Tako::PolyMeshReaderPtr& iNode)
{
    MStatus status = MS::kSuccess;
    MObject polyObj = MObject::kNullObj;

    if (mNotCreate == 1)
    {
        if (iNode->hasFrames())
            mData.mPolyNodePtrList.push_back(iNode);
        if (iNode->hasPropertyFrames())
            mData.mPropNodePtrList.push_back(iNode);

        return status;
    }

    if ( mConnect == true && mCurrentConnectAction == CONNECT )
    {
        polyObj = mCurrentDagNode.node();
        MFnMesh mFn(polyObj, &status);

        // check that the data types are compatible
        if ( status == MS::kSuccess )
        {
            std::vector<std::string> mPropNameList;
            mParent = mCurrentDagNode.transform(&status);

            if (iNode->hasFrames())
            {
                status = createPoly(mFrame, iNode, mParent, polyObj, mPropNameList, 1);
                mData.mPolyObjList.push_back(polyObj);
                mData.mPolyNodePtrList.push_back(iNode);
            }
            else
            {
                status = createPoly(mFrame, iNode, mParent, polyObj, mPropNameList, 1,
                    mLoadUVs, mLoadNormals);
            }

            if (iNode->hasPropertyFrames())
            {
                SampledPair mSampledPair(polyObj, mPropNameList);
                mData.mPropList.push_back(mSampledPair);
                mData.mPropNodePtrList.push_back(iNode);
            }

            checkShaderSelection(mFn, mCurrentDagNode.instanceNumber());
        }
        else
        {
            MString theError("No connection done for node \"");
            theError += MString(iNode->getName());
            theError += MString("\" with ");
            theError += mCurrentDagNode.fullPathName();
            printError(theError);
            return status;
        }
        return status;
    }

    std::vector<std::string> mPropNameList;

    if (iNode->hasFrames())
    {
        status = createPoly(mFrame, iNode, mParent, polyObj, mPropNameList);
        mData.mPolyObjList.push_back(polyObj);
        mData.mPolyNodePtrList.push_back(iNode);
    }
    else
    {
        status = createPoly(mFrame, iNode, mParent, polyObj, mPropNameList, 0,
            mLoadUVs, mLoadNormals);
    }
    if (iNode->hasPropertyFrames())
    {
        SampledPair mSampledPair(polyObj, mPropNameList);
        mData.mPropList.push_back(mSampledPair);
        mData.mPropNodePtrList.push_back(iNode);
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(const Tako::TransformReaderPtr& iNode)
{
    MStatus status = MS::kSuccess;
    MObject transObj;

     // root of file, no creation of DG node
    if (MString(iNode->getName()) == MString())
    {
        mData.mIsSampledTransOpAngle.clear();
        MObject saveParent = mParent;

        size_t numChildren = iNode->numChildren();
        if (numChildren == 0)   return status;

        if (!mConnect)  // simply scene creation mode
        {
            for (size_t i = 0; i < numChildren; i++)
            {
                ChildNodePtr child = iNode->getChild(i);
                boost::apply_visitor(*this, child);
                mParent = saveParent;
            }
        }
        else  // connect flag set
        {
            if (!mNotCreate)
            {
                std::set<std::string> connectCurNodesInFile;
                std::set<std::string> connectCurNodesInBoth;
                std::set<std::string> connectCurNodesToBeCreated;

                bool connectWorld = (mConnectRootNodes.size() == 0);
                std::set<std::string>::iterator fileEnd =
                    connectCurNodesInFile.end();
                for (size_t i = 0; i < numChildren; i++)
                {
                    ChildNodePtr child = iNode->getChild(i);
                    GetNameVisitor nameVisitor;
                    std::string name =
                        boost::apply_visitor(nameVisitor, child);
                    connectCurNodesInFile.insert(name);
                    // see if this AlembicTakoCacheNode is part of the input
                    if (connectWorld || (!connectWorld &&
                      mConnectRootNodes.find(name) != mConnectRootNodes.end()))
                    {
                        // Find out if this node exists in the current scene
                        MDagPath dagPath;
                        bool existInScene =
                            getDagPathByName(MString(name.c_str()), dagPath)
                                == MS::kSuccess;
                        if ( existInScene )
                        {
                            connectCurNodesInBoth.insert(name);
                            mConnectUpdateNodes.insert(name);
                            mCurrentDagNode = dagPath;
                            mCurrentConnectAction = CONNECT;
                            name = dagPath.partialPathName().asChar();
                            boost::apply_visitor(*this, child);
                            mParent = saveParent;
                        }
                        else
                        {
                            connectCurNodesToBeCreated.insert(name);
                            if ( mCreateIfNotFound )
                            {
                                mConnectUpdateNodes.insert(name);
                                mCurrentConnectAction = CREATE;
                                boost::apply_visitor(*this, child);
                                mParent = saveParent;
                            }
                            else
                            {
                                printWarning("-createIfNotFound flag not set, do nothing");
                            }
                        }
                    }
                }  // for-loop

                if ( connectWorld )
                {
                    mConnectRootNodes = connectCurNodesInFile;
                }
                else if (mConnectRootNodes.size() >
                    (connectCurNodesInBoth.size()
                    + connectCurNodesToBeCreated.size()))
                {
                    std::set<std::string>::iterator iter =
                        mConnectRootNodes.begin();
                    const std::set<std::string>::iterator fileEndIter =
                        connectCurNodesInFile.end();
                    MDagPath dagPath;
                    for ( ; iter != mConnectRootNodes.end(); iter++)
                    {
                        std::string name = *iter;
                        bool existInFile =
                            (connectCurNodesInFile.find(name) != fileEndIter);
                        bool existInScene =
                            (getDagPathByName(MString(name.c_str()), dagPath)
                                == MS::kSuccess);
                        if ( existInScene && !existInFile )
                            removeDagNode(mRemoveIfNoUpdate, dagPath);
                        else if (!existInScene && !existInFile)
                        {
                            MString theWarning(name.c_str());
                            theWarning += " exists neither in file nor in the scene";
                            printWarning(theWarning);
                        }
                    }
                }
            }
            else  // mNotCreate == 1
            {
                for (size_t i = 0; i < numChildren; i++)
                {
                    ChildNodePtr child = iNode->getChild(i);
                    GetNameVisitor nameVisitor;
                    std::string name = boost::apply_visitor(nameVisitor, child);
                    if ( mConnectUpdateNodes.find( name )
                        != mConnectUpdateNodes.end() )
                    {
                        getDagPathByName(name.c_str(), mCurrentDagNode);
                        boost::apply_visitor(*this, child);
                    }
                }
            }
        }
    }
    else
    {
        if (mNotCreate == 1)
        {
            if (iNode->hasFrames())
            {
                mData.mTransOpNodePtrList.push_back(iNode);
                mData.mIsComplexTrans.push_back(isComplex(mFrame, iNode));
            }
            if (iNode->hasPropertyFrames())
                mData.mPropNodePtrList.push_back(iNode);

            size_t numChildren = iNode->numChildren();
            if ( !mConnect )
            {
                for (size_t childIndex = 0;
                    childIndex < numChildren; childIndex++)
                {
                    ChildNodePtr child = iNode->getChild(childIndex);
                    boost::apply_visitor(*this, child);
                }
            }
            else
            // if mConnect is on, selectively traverse the hierarchy
            // (possibly for a second time)
            {
                std::set<std::string> childNodesInFile;
                MObject saveParent = transObj;
                MDagPath saveDag = mCurrentDagNode;
                for (size_t childIndex = 0;
                    childIndex < numChildren; childIndex++)
                {
                    ChildNodePtr child = iNode->getChild(childIndex);
                    mParent = saveParent;
                    GetNameVisitor nameVisitor;
                    std::string childName =
                        boost::apply_visitor(nameVisitor, child);
                    MString name = saveDag.fullPathName();
                    name += "|";
                    name += childName.c_str();
                    if (getDagPathByName(name, mCurrentDagNode) == MS::kSuccess)
                        boost::apply_visitor(*this, child);
                }
            }

            return status;
        }

        if ( mConnect == 1 && mCurrentConnectAction == CONNECT )
        {
            transObj = mCurrentDagNode.node();
            if (transObj.hasFn(MFn::kTransform))
            {
                std::vector<std::string> mTransopNameList;
                std::vector<std::string> mPropNameList;
                bool isComplex = false;
                status = create(mFrame, iNode, mParent, transObj,
                    mPropNameList, mTransopNameList, isComplex, true);

                unsigned int size = mTransopNameList.size();
                for (unsigned int i = 0; i < size; i++)
                {
                    if (mTransopNameList[i].find("rotate") != std::string::npos
                        && mTransopNameList[i].find("rotatePivot")
                            == std::string::npos)
                        mData.mIsSampledTransOpAngle.push_back(true);
                    else
                        mData.mIsSampledTransOpAngle.push_back(false);
                }

                if (iNode->hasFrames())
                {
                    SampledPair mSampledPair(transObj, mTransopNameList);
                    mData.mTransOpList.push_back(mSampledPair);
                    mData.mTransOpNodePtrList.push_back(iNode);
                    mData.mIsComplexTrans.push_back(isComplex);
                }
                if (iNode->hasPropertyFrames())
                {
                    SampledPair mSampledPair(transObj, mPropNameList);
                    mData.mPropList.push_back(mSampledPair);
                    mData.mPropNodePtrList.push_back(iNode);
                }

                // go down the current DAG's children and current
                // AlembicTakoCacheNode's  children
                size_t numChildren = iNode->numChildren();
                std::set<std::string> childNodesInFile;
                MObject saveParent = transObj;
                MDagPath saveDag = mCurrentDagNode;
                for (size_t childIndex = 0;
                    childIndex < numChildren; childIndex++)
                {
                    ChildNodePtr child = iNode->getChild(childIndex);
                    mParent = saveParent;
                    GetNameVisitor nameVisitor;
                    std::string childName =
                        boost::apply_visitor(nameVisitor, child);
                    childNodesInFile.insert(childName);
                    MString name = saveDag.fullPathName();
                    name += "|";
                    name += childName.c_str();
                    MDagPath dagPath;
                    if (getDagPathByName(name, dagPath) == MS::kSuccess)
                    {
                        mCurrentDagNode = dagPath;
                        boost::apply_visitor(*this, child);
                        mCurrentDagNode = saveDag;
                    }
                    else if (mCreateIfNotFound)  // create if necessary
                    {
                        mCurrentConnectAction = CREATE;
                        boost::apply_visitor(*this, child);
                        mCurrentConnectAction = CONNECT;
                    }
                }

                // There might be children under the current DAG node that
                // don't exist in the file.
                // remove if the -removeIfNoUpdate flag is set
                if ( mRemoveIfNoUpdate )
                {
                    unsigned int numChild = mCurrentDagNode.childCount();
                    std::vector<MDagPath> dagToBeRemoved;
                    for ( unsigned int i = 0; i < numChild; i++ )
                    {
                        MObject child = mCurrentDagNode.child(i);
                        MFnTransform fn(child, &status);
                        if ( status == MS::kSuccess )
                        {
                            std::string childName = fn.fullPathName().asChar();
                            size_t found = childName.rfind("|");
                            if (found != std::string::npos)
                            {
                                childName = childName.substr(
                                    found+1, childName.length() - found);
                                if (childNodesInFile.find(childName)
                                    == childNodesInFile.end())
                                {
                                    MDagPath dagPath;
                                    getDagPathByName(
                                        fn.fullPathName(), dagPath);
                                    dagToBeRemoved.push_back(dagPath);
                                }
                            }
                        }
                    }
                    if (dagToBeRemoved.size() > 0)
                    {
                        unsigned int dagSize = dagToBeRemoved.size();
                        for ( unsigned int i = 0; i < dagSize; i++ )
                            removeDagNode(mRemoveIfNoUpdate, dagToBeRemoved[i]);
                    }
                }
            }
            else
            {
                MString theError = mCurrentDagNode.partialPathName();
                theError += MString(" is not compatible as a transform node. ");
                theError += MString("Connection failed.");
                printError(theError);
            }
        }
        if ( !mConnect || mCurrentConnectAction == CREATE )
        {
            // create transform node
            std::vector<std::string> mTransopNameList;
            std::vector<std::string> mPropNameList;

            bool isComplex = false;
            status = create(mFrame, iNode, mParent, transObj,
                  mPropNameList, mTransopNameList, isComplex);

            unsigned int size = mTransopNameList.size();
            for (unsigned int i = 0; i < size; i++)
            {
                if (mTransopNameList[i].find("rotate") != std::string::npos &&
                   mTransopNameList[i].find("rotatePivot") == std::string::npos)
                    mData.mIsSampledTransOpAngle.push_back(true);
                else
                    mData.mIsSampledTransOpAngle.push_back(false);
            }

            if (iNode->hasFrames())
            {
                SampledPair mSampledPair(transObj, mTransopNameList);
                mData.mTransOpList.push_back(mSampledPair);
                mData.mTransOpNodePtrList.push_back(iNode);
                mData.mIsComplexTrans.push_back(isComplex);
            }
            if (iNode->hasPropertyFrames())
            {
                SampledPair mSampledPair(transObj, mPropNameList);
                mData.mPropList.push_back(mSampledPair);
                mData.mPropNodePtrList.push_back(iNode);
            }

            size_t numChildren = iNode->numChildren();
            MObject saveParent = transObj;
            for (size_t childIndex = 0; childIndex < numChildren; childIndex++)
            {
                ChildNodePtr child = iNode->getChild(childIndex);
                mParent = saveParent;
                boost::apply_visitor(*this, child);
            }
       }
    }

    return status;
}

} // End namespace Alembic

