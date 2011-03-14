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

#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MPlug.h>
#include <maya/MDGModifier.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnStringData.h>
#include <maya/MFnTransform.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnSet.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "util.h"
#include "MeshHelper.h"
#include "PointHelper.h"
#include "XformHelper.h"
#include "CreateSceneHelper.h"

namespace
{
    void removeDagNode(MDagPath & dagPath)
    {

        MStatus status = deleteDagNode(dagPath);
        if ( status != MS::kSuccess )
        {
            MString theError = dagPath.partialPathName();
            theError += MString(" removal not successful");
            printError(theError);
        }
    }

}


CreateSceneVisitor::CreateSceneVisitor(double iFrame,
        const MObject & iParent, Action iAction,
        MString iRootNodes) :
    mFrame(iFrame), mParent(iParent), mAction(iAction)
{
    // parse the input string to extract the nodes that need (re)connection
    if ( iRootNodes != MString("/") )
    {
        MStringArray theArray;
        if (iRootNodes.split(' ', theArray) == MS::kSuccess)
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

                mRootNodes.insert(name.asChar());
            }
        }
    }
}

CreateSceneVisitor::~CreateSceneVisitor()
{
}

void CreateSceneVisitor::getData(WriterData & oData)
{
    oData = mData;
}

bool CreateSceneVisitor::hasSampledData()
{
    unsigned int subDSize = mData.mSubDList.size();
    unsigned int polySize = mData.mPolyMeshList.size();
    unsigned int cameraSize = 0; //mData.mCameraList.size();
    // Currently there's no support for bringing in particle system simulation
    // unsigned int particleSize = mData.mParticleList.size();
    unsigned int transopSize = mData.mXformList.size();
    unsigned int nSurfaceSize  = 0; //mData.mNurbsList.size();
    unsigned int nCurveSize  = 0; //mData.mCurveList.size();
    unsigned int propSize = 0; //mData.mPropList.size();

    return ( subDSize > 0 || polySize > 0 || nSurfaceSize > 0 || nCurveSize > 0
            || transopSize > 0 || cameraSize > 0  // || particleSize > 0
            || propSize > 0);
}

// re-add the selection back to the sets
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

// remembers what sets a mesh was part of, gets those sets as a selection
// and then clears the sets for reassignment later  this is only used when
// hooking up an HDF node to a previous hierarchy (swapping)
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

void CreateSceneVisitor::visit(Alembic::Abc::IObject & iObj)
{
    if ( Alembic::AbcGeom::IXform::matches(iObj.getHeader()) )
    {
        Alembic::AbcGeom::IXform xform(iObj, Alembic::Abc::kWrapExisting);
        (*this)(xform);
    }
    else if ( Alembic::AbcGeom::ISubD::matches(iObj.getHeader()) )
    {
        Alembic::AbcGeom::ISubD mesh(iObj, Alembic::Abc::kWrapExisting);
        (*this)(mesh);
    }
    else if ( Alembic::AbcGeom::IPolyMesh::matches(iObj.getHeader()) )
    {
        Alembic::AbcGeom::IPolyMesh mesh(iObj, Alembic::Abc::kWrapExisting);
        (*this)(mesh);
    }
    else if ( Alembic::AbcGeom::IPoints::matches(iObj.getHeader()) )
    {
        Alembic::AbcGeom::IPoints pts(iObj, Alembic::Abc::kWrapExisting);
        (*this)(pts);
    }
    else
    {
        MString theWarning(iObj.getName().c_str());
        theWarning += " is an unsupported schema, skipping: ";
        theWarning += iObj.getMetaData().get("schema").c_str();
        printWarning(theWarning);
    }
}

 // root of file, no creation of DG node
MStatus CreateSceneVisitor::walk(Alembic::Abc::IArchive & iRoot)
{
    MStatus status = MS::kSuccess;

    mData.mIsSampledXformOpAngle.clear();
    MObject saveParent = mParent;

    Alembic::Abc::IObject top = iRoot.getTop();
    size_t numChildren = top.getNumChildren();

    if (numChildren == 0) return status;

    if (mAction == NONE)  // simple scene creation mode
    {
        for (size_t i = 0; i < numChildren; i++)
        {
            Alembic::Abc::IObject child = top.getChild(i);
            this->visit(child);
            mParent = saveParent;
        }
        return status;
    }

    // doing connections
    std::set<std::string> connectUpdateNodes;
    if (mAction == CREATE || mAction == CREATE_REMOVE)
    {
        std::set<std::string> connectCurNodesInFile;

        bool connectWorld = (mRootNodes.size() == 0);
        std::set<std::string>::iterator fileEnd =
            connectCurNodesInFile.end();
        for (size_t i = 0; i < numChildren; i++)
        {
            Alembic::Abc::IObject obj = top.getChild(i);
            std::string name = obj.getName();
            connectCurNodesInFile.insert(name);

            // see if this name is part of the input to AlembicNode 
            if (connectWorld || (!connectWorld &&
              mRootNodes.find(name) != mRootNodes.end()))
            {
                // Find out if this node exists in the current scene
                MDagPath dagPath;

                if (getDagPathByName(MString(name.c_str()), dagPath) ==
                    MS::kSuccess)
                {
                    connectUpdateNodes.insert(name);
                    mCurrentDagNode = dagPath;
                    name = dagPath.partialPathName().asChar();
                    this->visit(obj);
                    mParent = saveParent;
                }
                else
                {
                    connectUpdateNodes.insert(name);
                    this->visit(obj);
                    mParent = saveParent;

                }
            }
        }  // for-loop

        if ( connectWorld )
        {
            mRootNodes = connectCurNodesInFile;
        }
        else if (mRootNodes.size() > connectUpdateNodes.size())
        {
            std::set<std::string>::iterator iter =
                mRootNodes.begin();
            const std::set<std::string>::iterator fileEndIter =
                connectCurNodesInFile.end();
            MDagPath dagPath;
            for ( ; iter != mRootNodes.end(); iter++)
            {
                std::string name = *iter;
                bool existInFile =
                    (connectCurNodesInFile.find(name) != fileEndIter);
                bool existInScene =
                    (getDagPathByName(MString(name.c_str()), dagPath)
                        == MS::kSuccess);
                if ( existInScene && !existInFile )
                    removeDagNode(dagPath);
                else if (!existInScene && !existInFile)
                {
                    MString theWarning(name.c_str());
                    theWarning += 
                        " exists neither in file nor in the scene";
                    printWarning(theWarning);
                }
            }
        }
    }
    else  // mAction == CONNECT
    {
        for (size_t i = 0; i < numChildren; i++)
        {
            Alembic::Abc::IObject child = top.getChild(i);
            std::string name = child.getName();
            if ( connectUpdateNodes.find( name )
                != connectUpdateNodes.end() )
            {
                getDagPathByName(name.c_str(), mCurrentDagNode);
                this->visit(child);
            }
        }
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(Alembic::AbcGeom::IPoints& iNode)
{
    MStatus status = MS::kSuccess;
    MObject particleObj = MObject::kNullObj;

    if (iNode.getSchema().getNumSamples() > 1)
        mData.mPointsList.push_back(iNode);

    // review other arbitrary attributes and add it to the lists

    if (mAction == CREATE || mAction == CREATE_REMOVE)
    {
        std::vector<std::string> propNameList;
        status = create(mFrame, iNode, mParent, particleObj, propNameList);
        if (iNode.getSchema().getNumSamples() > 1)
        {
            mData.mPointsObjList.push_back(particleObj);
        }

        if (propNameList.size() > 0)
        {
            SampledPair mSampledPair(particleObj, propNameList);
            //mData.mPropList.push_back(mSampledPair);
            //mData.mPropNodePtrList.push_back(iNode);
        }
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(Alembic::AbcGeom::ISubD& iNode)
{
    MStatus status = MS::kSuccess;
    MObject subDObj = MObject::kNullObj;

    size_t numSamples = iNode.getSchema().getNumSamples();

    // add animated SubDs to the list
    if (numSamples > 1)
    {
        mData.mSubDList.push_back(iNode);
    }

    // review other arbitrary attributes and add it to the lists

    std::vector<std::string> propNameList;
    if (mAction == CREATE || mAction == CREATE_REMOVE)
    {
        subDObj = createSubD(mFrame, iNode, mParent, propNameList);
        MFnDagNode(subDObj).getPath(mCurrentDagNode);
        if (numSamples > 1)
        {
            mData.mSubDObjList.push_back(subDObj);
        }

        /*
        if (iNode.hasPropertyFrames())
        {
            SampledPair mSampledPair(subDObj, propNameList);
            mData.mPropList.push_back(mSampledPair);
            mData.mPropNodePtrList.push_back(iNode);
        }
        */

    }

    if ( mAction >= CONNECT )
    {
        if (subDObj ==  MObject::kNullObj)
        {
            subDObj = mCurrentDagNode.node();
            MFnMesh fn(subDObj, &status);

            // check that the data types are compatible, they might not be
            // if we have a weird hierarchy, where the node in the scene
            // differs from the node on disk
            if ( status != MS::kSuccess )
            {
                MString theError("No connection done for node '");
                theError += MString(iNode.getName().c_str());
                theError += MString("' with ");
                theError += mCurrentDagNode.fullPathName();
                printError(theError);
                return status;
            }

            checkShaderSelection(fn, mCurrentDagNode.instanceNumber());
        }

        disconnectMesh(subDObj, propNameList);

    }

    return status;
}

MStatus CreateSceneVisitor::operator()(Alembic::AbcGeom::IPolyMesh& iNode)
{
    MStatus status = MS::kSuccess;
    MObject polyObj = MObject::kNullObj;

    size_t numSamples = iNode.getSchema().getNumSamples();

    // add animated SubDs to the list
    if (numSamples > 1)
        mData.mPolyMeshList.push_back(iNode);

    // review other arbitrary attributes and add it to the lists

    std::vector<std::string> propNameList;
    if (mAction == CREATE || mAction == CREATE_REMOVE)
    {
        polyObj = createPoly(mFrame, iNode, mParent, propNameList);
        MFnDagNode(polyObj).getPath(mCurrentDagNode);
        if (numSamples > 1)
        {
            mData.mPolyMeshObjList.push_back(polyObj);
        }

        /*
        if (iNode.hasPropertyFrames())
        {
            SampledPair mSampledPair(subDObj, propNameList);
            mData.mPropList.push_back(mSampledPair);
            mData.mPropNodePtrList.push_back(iNode);
        }
        */

    }


    if ( mAction >= CONNECT )
    {
        if (polyObj == MObject::kNullObj)
        {
            polyObj = mCurrentDagNode.node();
            MFnMesh fn(polyObj, &status);

            // check that the data types are compatible, they might not be
            // if we have a weird hierarchy, where the node in the scene
            // differs from the node on disk
            if ( status != MS::kSuccess )
            {
                MString theError("No connection done for node '");
                theError += MString(iNode.getName().c_str());
                theError += MString("' with ");
                theError += mCurrentDagNode.fullPathName();
                printError(theError);
                return status;
            }

            checkShaderSelection(fn, mCurrentDagNode.instanceNumber());
        }

        disconnectMesh(polyObj, propNameList);

    }

    return status;
}

MStatus CreateSceneVisitor::operator()(Alembic::AbcGeom::IXform & iNode)
{
    MStatus status = MS::kSuccess;
    MObject transObj;

    size_t numChildren = iNode.getNumChildren();
    size_t numSamples = iNode.getSchema().getNumAnimSamples();

    if (numSamples > 1)
    {
        mData.mXformList.push_back(iNode);
        mData.mIsComplexXform.push_back(isComplex(iNode));
    }

    // There might be children under the current DAG node that
    // don't exist in the file.
    // Remove them if the -removeIfNoUpdate flag is set
    if (mAction == REMOVE || mAction == CREATE_REMOVE)
    {
        unsigned int numDags = mCurrentDagNode.childCount();
        std::vector<MDagPath> dagToBeRemoved;

        // get names of immediate children so we can compare with
        // the hierarchy in teh scene
        std::set< std::string > childNodesInFile;
        for (size_t j = 0; j < numChildren; ++j)
        {
            Alembic::Abc::IObject child = iNode.getChild(j);
            childNodesInFile.insert(child.getName());
        }

        for ( unsigned int i = 0; i < numDags; i++ )
        {
            MObject child = mCurrentDagNode.child(i);
            MFnDagNode fn(child, &status);
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
                removeDagNode(dagToBeRemoved[i]);
        }
    }

    // just create the node
    if ( mAction == CREATE || mAction == CREATE_REMOVE )
    {
        MFnTransform trans;
        MString name(iNode.getName().c_str());
        transObj = trans.create(mParent, &status);
        trans.getPath(mCurrentDagNode);

        if (status != MS::kSuccess)
        {
            MString theError("Failed to create transform node ");
            theError += name;
            printError(theError);
            return status;
        }

        trans.setName(name);

        MPlug dstPlug;
        dstPlug = trans.findPlug("inheritsTransform");
        if (!dstPlug.isNull())
        {
            dstPlug.setBool( iNode.getSchema().inherits(
                Alembic::Abc::ISampleSelector(mFrame,
                    Alembic::Abc::ISampleSelector::kNearIndex)) );
        }

        //addProperties(mFrame, iNode, transObj, iSampledPropNameList);
    }

    if ( mAction >= CONNECT )
    {
        if (transObj ==  MObject::kNullObj)
            transObj = mCurrentDagNode.node();

        if (transObj.hasFn(MFn::kTransform))
        {
            std::vector<std::string> transopNameList;
            std::vector<std::string> propNameList;
            bool isComplex = false;
            connectToXform(mFrame, iNode, transObj, propNameList,
                transopNameList);

            unsigned int size = transopNameList.size();
            for (unsigned int i = 0; i < size; i++)
            {
                if (transopNameList[i].find("rotate") != std::string::npos
                    && transopNameList[i].find("rotatePivot")
                        == std::string::npos)
                    mData.mIsSampledXformOpAngle.push_back(true);
                else
                    mData.mIsSampledXformOpAngle.push_back(false);
            }

            if (numSamples > 1)
            {
                SampledPair mSampledPair(transObj, transopNameList);
                mData.mXformOpList.push_back(mSampledPair);
            }

            /*
            if (iNode.hasPropertyFrames())
            {
                SampledPair mSampledPair(transObj, propNameList);
                mData.mPropList.push_back(mSampledPair);
                mData.mPropNodePtrList.push_back(iNode);
            }
            */
        }
        else
        {
            MString theError = mCurrentDagNode.partialPathName();
            theError += MString(" is not compatible as a transform node. ");
            theError += MString("Connection failed.");
            printError(theError);
        }
    }

    /*
    for (size_t i = 0; i < numChildren; ++i)
    {
        Alembic::Abc::IObject child = iNode.getChild(i);
        this->visit(child);
    }
    */

    MObject saveParent = transObj;
    MDagPath saveDag = mCurrentDagNode;
    for (size_t i = 0; i < numChildren; ++i)
    {
        Alembic::Abc::IObject child = iNode.getChild(i);
        mParent = saveParent;

        this->visit(child);
    }

    return status;
}
