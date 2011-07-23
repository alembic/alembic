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
#include <maya/MFnCamera.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnStringData.h>
#include <maya/MFnTransform.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnSet.h>
#include <maya/MFnTypedAttribute.h>

#include <Alembic/AbcGeom/Visibility.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "util.h"
#include "CameraHelper.h"
#include "LocatorHelper.h"
#include "MeshHelper.h"
#include "NurbsCurveHelper.h"
#include "NurbsSurfaceHelper.h"
#include "PointHelper.h"
#include "XformHelper.h"
#include "CreateSceneHelper.h"

namespace
{
    void addFaceSets(MObject & iNode, Alembic::Abc::IObject & iObj)
    {
        MStatus status;

        MFnDependencyNode mesh(iNode, &status);

        if (status != MS::kSuccess)
            return;

        std::size_t numChildren = iObj.getNumChildren();
        for ( std::size_t i = 0 ; i < numChildren; ++i )
        {
            Alembic::Abc::IObject child = iObj.getChild(i);
            if (Alembic::AbcGeom::IFaceSet::matches(child.getHeader()))
            {
                Alembic::AbcGeom::IFaceSet faceSet(child,
                    Alembic::Abc::kWrapExisting);

                Alembic::AbcGeom::IFaceSetSchema::Sample samp;
                faceSet.getSchema().get(samp);

                MString faceName = "FACESET_";
                faceName += faceSet.getName().c_str();

                MFnIntArrayData fnData;
                MIntArray arr((int *) samp.getFaces()->getData(),
                    samp.getFaces()->size());
                MObject attrObj = fnData.create(arr);
                MFnTypedAttribute typedAttr;
                MObject faceObj = typedAttr.create(faceName, faceName,
                    MFnData::kIntArray, attrObj);

                mesh.addAttribute(faceObj,
                    MFnDependencyNode::kLocalDynamicAttr);

                if (Alembic::AbcGeom::GetVisibility(faceSet) ==
                    Alembic::AbcGeom::kVisibilityHidden)
                {
                    MString visName = "FACESETVIS_";
                    visName += faceSet.getName().c_str();

                    MFnNumericAttribute numAttr;
                    MObject visObj = numAttr.create(visName, visName,
                        MFnNumericData::kBoolean, false);
                    mesh.addAttribute(visObj,
                        MFnDependencyNode::kLocalDynamicAttr);
                }
            }
        }
    }

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

    Alembic::Abc::IScalarProperty getVisible(Alembic::Abc::IObject & iNode,
        bool isObjConstant,
        std::vector<Prop> & oPropList,
        std::vector<Alembic::AbcGeom::IObject> & oAnimVisStaticObj)
    {
        Alembic::AbcGeom::IVisibilityProperty visProp =
            Alembic::AbcGeom::GetVisibilityProperty(iNode);

        if (visProp && !visProp.isConstant())
        {
            Prop prop;
            prop.mScalar = visProp;
            oPropList.push_back(prop);
            if (isObjConstant)
            {
                oAnimVisStaticObj.push_back(iNode);
            }
        }
        return visProp;
    }

    void setConstantVisibility(Alembic::Abc::IScalarProperty iVisProp,
        MObject & iParent)
    {
        if (iVisProp.valid() && iVisProp.isConstant())
        {
            Alembic::Util::int8_t visVal;
            iVisProp.get(&visVal);
            MFnDependencyNode dep(iParent);
            MPlug plug = dep.findPlug("visibility");
            if (!plug.isNull())
            {
                plug.setBool(visVal != 0);
            }
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

    // Currently there's no support for bringing in particle system simulation
    return (mData.mPropList.size() > 0
        || mData.mXformList.size() > 0
        || mData.mSubDList.size() > 0
        || mData.mPolyMeshList.size() > 0
        || mData.mCameraList.size() > 0
        || mData.mNurbsList.size() > 0
        || mData.mCurvesList.size() > 0
        || mData.mLocList.size() > 0);
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

void CreateSceneVisitor::addToPropList(std::size_t iFirst, MObject & iObject)
{
    std::size_t last = mData.mPropList.size();
    std::vector<std::string> attrList;
    for (std::size_t i = iFirst; i < last; ++i)
    {
        if (mData.mPropList[i].mArray.valid())
        {
            attrList.push_back(mData.mPropList[i].mArray.getName());
        }
        else
        {
            attrList.push_back(mData.mPropList[i].mScalar.getName());
        }
    }
    mData.mPropObjList.push_back(SampledPair(iObject, attrList));
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
    else if ( Alembic::AbcGeom::ICamera::matches(iObj.getHeader()) )
    {
        Alembic::AbcGeom::ICamera cam(iObj, Alembic::Abc::kWrapExisting);
        (*this)(cam);
    }
    else if ( Alembic::AbcGeom::ICurves::matches(iObj.getHeader()) )
    {
        Alembic::AbcGeom::ICurves curves(iObj, Alembic::Abc::kWrapExisting);
        (*this)(curves);
    }
    else if ( Alembic::AbcGeom::INuPatch::matches(iObj.getHeader()) )
    {
        Alembic::AbcGeom::INuPatch nurbs(iObj, Alembic::Abc::kWrapExisting);
        (*this)(nurbs);
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
                mConnectDagNode = dagPath;
                mConnectDagNode.pop();
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
    else if (mRootNodes.size() > connectUpdateNodes.size() &&
        (mAction == REMOVE || mAction == CREATE_REMOVE))
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
            if (existInScene && !existInFile)
            {
                removeDagNode(dagPath);
            }
            else if (!existInScene && !existInFile)
            {
                MString theWarning(name.c_str());
                theWarning +=
                    " exists neither in file nor in the scene";
                printWarning(theWarning);
            }
        }
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(Alembic::AbcGeom::ICamera & iNode)
{
    MStatus status = MS::kSuccess;
    MObject cameraObj = MObject::kNullObj;

    size_t numSamples = iNode.getSchema().getNumSamples();

    // add animated camera to the list
    if (numSamples > 1)
        mData.mCameraList.push_back(iNode);

    Alembic::Abc::ICompoundProperty arbProp =
        iNode.getSchema().getArbGeomParams();

    std::size_t firstProp = mData.mPropList.size();
    getAnimatedProps(arbProp, mData.mPropList);
    Alembic::Abc::IScalarProperty visProp = getVisible(iNode, numSamples < 2,
        mData.mPropList, mData.mAnimVisStaticObjList);

    bool hasDag = false;
    if (mAction != NONE && mConnectDagNode.isValid())
    {
        hasDag = getDagPathByChildName(mConnectDagNode, iNode.getName());
    }

    if (mAction == CREATE || mAction == CREATE_REMOVE)
    {
        if (hasDag)
        {
            cameraObj = mConnectDagNode.node();
        }
        else
        {
            cameraObj = create(iNode, mParent);
            if (numSamples > 1)
            {
                mData.mCameraObjList.push_back(cameraObj);
            }
        }

        setConstantVisibility(visProp, cameraObj);
        addProps(arbProp, cameraObj);

    }

    if ( mAction >= CONNECT )
    {
        if (cameraObj == MObject::kNullObj && getDagPathByChildName(
            mConnectDagNode, iNode.getName()))
        {
            cameraObj = mConnectDagNode.node();
            MFnCamera fn(cameraObj, &status);

            // check that the data types are compatible, they might not be
            // if we have a weird hierarchy, where the node in the scene
            // differs from the node on disk
            if ( status != MS::kSuccess )
            {
                MString theError("No connection done for node '");
                theError += MString(iNode.getName().c_str());
                theError += MString("' with ");
                theError += mConnectDagNode.fullPathName();
                printError(theError);
                return status;
            }
        }
        else
        {
            mConnectDagNode.pop();
        }

        addToPropList(firstProp, cameraObj);
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(Alembic::AbcGeom::ICurves & iNode)
{
    MStatus status = MS::kSuccess;
    MObject curvesObj = MObject::kNullObj;

    size_t numSamples = iNode.getSchema().getNumSamples();

    // read sample 0 to determine and use it to set the number of total
    // curves.  We can't support changing the number of curves over time.
    Alembic::AbcGeom::ICurvesSchema::Sample samp;
    iNode.getSchema().get(samp);
    Alembic::Abc::ICompoundProperty arbProp =
        iNode.getSchema().getArbGeomParams();
    Alembic::AbcGeom::IFloatGeomParam::Sample widthSamp;
    if (iNode.getSchema().getWidthsParam())
    {
        iNode.getSchema().getWidthsParam().getExpanded(widthSamp);
    }
    std::size_t numCurves = samp.getNumCurves();

    if (numCurves == 0)
    {
        MString theWarning(iNode.getName().c_str());
        theWarning += " has no curves, skipping.";
        printWarning(theWarning);
        return MS::kFailure;
    }
    // add animated curves to the list
    else if (numSamples > 1)
    {
        mData.mNumCurves.push_back(numCurves);
        mData.mCurvesList.push_back(iNode);
    }

    std::size_t firstProp = mData.mPropList.size();
    getAnimatedProps(arbProp, mData.mPropList);
    Alembic::Abc::IScalarProperty visProp = getVisible(iNode, numSamples < 2,
        mData.mPropList, mData.mAnimVisStaticObjList);

    bool hasDag = false;
    if (mAction != NONE && mConnectDagNode.isValid())
    {
        hasDag = getDagPathByChildName(mConnectDagNode, iNode.getName());
    }

    if (mAction == CREATE || mAction == CREATE_REMOVE)
    {
        if (hasDag)
        {
            curvesObj = mConnectDagNode.node();
        }
        else
        {
            curvesObj = createCurves(iNode.getName(), samp, widthSamp, mParent,
                mData.mNurbsCurveObjList, numSamples > 1);
            MFnDagNode(curvesObj).getPath(mConnectDagNode);
        }

        setConstantVisibility(visProp, curvesObj);
        addProps(arbProp, curvesObj);
    }


    if (mAction >= CONNECT)
    {
        if (curvesObj == MObject::kNullObj && getDagPathByChildName(
            mConnectDagNode, iNode.getName()))
        {
            curvesObj = mConnectDagNode.node();
            MFnNurbsCurve fncurve(curvesObj, &status);

            // not a single curve, try the transform for a group of curves
            if (status != MS::kSuccess)
            {
                MFnTransform fntrans(curvesObj, &status);
            }

            // check that the data types are compatible, they might not be
            // if we have a weird hierarchy, where the node in the scene
            // differs from the node on disk
            if (status != MS::kSuccess)
            {
                MString theError("No connection done for node '");
                theError += MString(iNode.getName().c_str());
                theError += MString("' with ");
                theError += mConnectDagNode.fullPathName();
                printError(theError);
                return status;
            }
            else
            {
                mConnectDagNode.pop();
            }
        }

        addToPropList(firstProp, curvesObj);
    }

    if (hasDag)
    {
        mConnectDagNode.pop();
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(Alembic::AbcGeom::IPoints& iNode)
{
    MStatus status = MS::kSuccess;
    MObject particleObj = MObject::kNullObj;

    if (iNode.getSchema().getNumSamples() > 1)
        mData.mPointsList.push_back(iNode);

    // since we don't really support animated points, don't bother
    // with the animated properties on it

    bool hasDag = false;
    if (mAction != NONE && mConnectDagNode.isValid())
    {
        hasDag = getDagPathByChildName(mConnectDagNode, iNode.getName());
    }

    if (mAction == CREATE || mAction == CREATE_REMOVE)
    {
        if (hasDag)
        {
            particleObj = mConnectDagNode.node();
        }
        else
        {
            status = create(mFrame, iNode, mParent, particleObj);
            if (iNode.getSchema().getNumSamples() > 1)
            {
                mData.mPointsObjList.push_back(particleObj);
            }

            Alembic::Abc::ICompoundProperty arbProp =
                iNode.getSchema().getArbGeomParams();
        }

        // don't currently care about anything animated on a particleObj
        std::vector<Prop> fakePropList;
        std::vector<Alembic::AbcGeom::IObject> fakeObjList;

        Alembic::Abc::IScalarProperty visProp =
            getVisible(iNode, false, fakePropList, fakeObjList);

        setConstantVisibility(visProp, particleObj);
        Alembic::Abc::ICompoundProperty arbProp =
            iNode.getSchema().getArbGeomParams();
        addProps(arbProp, particleObj);
    }

    if (hasDag)
    {
        mConnectDagNode.pop();
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

    Alembic::Abc::ICompoundProperty arbProp =
        iNode.getSchema().getArbGeomParams();

    std::size_t firstProp = mData.mPropList.size();
    getAnimatedProps(arbProp, mData.mPropList);
    Alembic::Abc::IScalarProperty visProp = getVisible(iNode, numSamples < 2,
        mData.mPropList, mData.mAnimVisStaticObjList);

    bool hasDag = false;
    if (mAction != NONE && mConnectDagNode.isValid())
    {
        hasDag = getDagPathByChildName(mConnectDagNode, iNode.getName());
    }

    if (mAction == CREATE || mAction == CREATE_REMOVE)
    {
        if (hasDag)
        {
            subDObj = mConnectDagNode.node();
        }
        else
        {
            subDObj = createSubD(mFrame, iNode, mParent);
            if (numSamples > 1)
            {
                mData.mSubDObjList.push_back(subDObj);
            }
        }

        setConstantVisibility(visProp, subDObj);
        addProps(arbProp, subDObj);
    }

    if ( mAction >= CONNECT )
    {
        if (subDObj == MObject::kNullObj && getDagPathByChildName(
            mConnectDagNode, iNode.getName()))
        {
            subDObj = mConnectDagNode.node();
            MFnMesh fn(subDObj, &status);

            // check that the data types are compatible, they might not be
            // if we have a weird hierarchy, where the node in the scene
            // differs from the node on disk
            if ( status != MS::kSuccess )
            {
                MString theError("No connection done for node '");
                theError += MString(iNode.getName().c_str());
                theError += MString("' with ");
                theError += mConnectDagNode.fullPathName();
                printError(theError);
                return status;
            }
            else
            {
                mConnectDagNode.pop();
            }

            checkShaderSelection(fn, mConnectDagNode.instanceNumber());
        }

        disconnectMesh(subDObj, mData.mPropList, firstProp);
        addToPropList(firstProp, subDObj);
        addFaceSets(subDObj, iNode);

    }

    if (hasDag)
    {
        mConnectDagNode.pop();
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(Alembic::AbcGeom::IPolyMesh& iNode)
{
    MStatus status = MS::kSuccess;
    MObject polyObj = MObject::kNullObj;

    size_t numSamples = iNode.getSchema().getNumSamples();

    // add animated poly mesh to the list
    if (numSamples > 1)
        mData.mPolyMeshList.push_back(iNode);

    Alembic::Abc::ICompoundProperty arbProp =
        iNode.getSchema().getArbGeomParams();

    std::size_t firstProp = mData.mPropList.size();
    getAnimatedProps(arbProp, mData.mPropList);
    Alembic::Abc::IScalarProperty visProp = getVisible(iNode, numSamples < 2,
        mData.mPropList, mData.mAnimVisStaticObjList);

    bool hasDag = false;
    if (mAction != NONE && mConnectDagNode.isValid())
    {
        hasDag = getDagPathByChildName(mConnectDagNode, iNode.getName());
    }

    if (mAction == CREATE || mAction == CREATE_REMOVE)
    {
        if (hasDag)
        {
            polyObj = mConnectDagNode.node();
        }
        else
        {
            polyObj = createPoly(mFrame, iNode, mParent);
            MFnDagNode(polyObj).getPath(mConnectDagNode);
            if (numSamples > 1)
            {
                mData.mPolyMeshObjList.push_back(polyObj);
            }
        }

        setConstantVisibility(visProp, polyObj);
        addProps(arbProp, polyObj);
        addFaceSets(polyObj, iNode);

    }


    if ( mAction >= CONNECT )
    {
        if (polyObj == MObject::kNullObj && getDagPathByChildName(
            mConnectDagNode, iNode.getName()))
        {
            polyObj = mConnectDagNode.node();
            MFnMesh fn(polyObj, &status);

            // check that the data types are compatible, they might not be
            // if we have a weird hierarchy, where the node in the scene
            // differs from the node on disk
            if ( status != MS::kSuccess )
            {
                MString theError("No connection done for node '");
                theError += MString(iNode.getName().c_str());
                theError += MString("' with ");
                theError += mConnectDagNode.fullPathName();
                printError(theError);
                return status;
            }
            else
            {
                mConnectDagNode.pop();
            }

            checkShaderSelection(fn, mConnectDagNode.instanceNumber());
        }

        disconnectMesh(polyObj, mData.mPropList, firstProp);
        addToPropList(firstProp, polyObj);
    }

    if (hasDag)
    {
        mConnectDagNode.pop();
    }

    return status;
}

MStatus CreateSceneVisitor::operator()(Alembic::AbcGeom::INuPatch& iNode)
{
    MStatus status = MS::kSuccess;
    MObject nurbsObj = MObject::kNullObj;

    size_t numSamples = iNode.getSchema().getNumSamples();

    // add animated poly mesh to the list
    if (numSamples > 1)
        mData.mNurbsList.push_back(iNode);

    Alembic::Abc::ICompoundProperty arbProp =
        iNode.getSchema().getArbGeomParams();

    std::size_t firstProp = mData.mPropList.size();
    getAnimatedProps(arbProp, mData.mPropList);
    Alembic::Abc::IScalarProperty visProp = getVisible(iNode, numSamples < 2,
        mData.mPropList, mData.mAnimVisStaticObjList);

    bool hasDag = false;
    if (mAction != NONE && mConnectDagNode.isValid())
    {
        hasDag = getDagPathByChildName(mConnectDagNode, iNode.getName());
    }

    if (mAction == CREATE || mAction == CREATE_REMOVE)
    {
        if (hasDag)
        {
            nurbsObj = mConnectDagNode.node();
        }
        else
        {
            nurbsObj = createNurbs(mFrame, iNode, mParent);
            MFnDagNode(nurbsObj).getPath(mConnectDagNode);
            if (numSamples > 1)
            {
                mData.mNurbsObjList.push_back(nurbsObj);
            }

            addProps(arbProp, nurbsObj);
        }

        setConstantVisibility(visProp, nurbsObj);

    }


    if ( mAction >= CONNECT )
    {
        if (nurbsObj == MObject::kNullObj && getDagPathByChildName(
            mConnectDagNode, iNode.getName()))
        {
            nurbsObj = mConnectDagNode.node();
            MFnNurbsSurface fn(nurbsObj, &status);

            // check that the data types are compatible, they might not be
            // if we have a weird hierarchy, where the node in the scene
            // differs from the node on disk
            if ( status != MS::kSuccess )
            {
                MString theError("No connection done for node '");
                theError += MString(iNode.getName().c_str());
                theError += MString("' with ");
                theError += mConnectDagNode.fullPathName();
                printError(theError);
                return status;
            }
            else
            {
                mConnectDagNode.pop();
            }

        }

        disconnectMesh(nurbsObj, mData.mPropList, firstProp);
        addToPropList(firstProp, nurbsObj);
    }

    if (hasDag)
    {
        mConnectDagNode.pop();
    }
    return status;
}

MStatus CreateSceneVisitor::operator()(Alembic::AbcGeom::IXform & iNode)
{
    MStatus status = MS::kSuccess;
    MObject xformObj = MObject::kNullObj;

    Alembic::Abc::ICompoundProperty arbProp =
        iNode.getSchema().getArbGeomParams();

    std::size_t firstProp = mData.mPropList.size();
    getAnimatedProps(arbProp, mData.mPropList);

    if (iNode.getProperties().getPropertyHeader("locator") != NULL)
    {
        Alembic::Abc::ICompoundProperty props = iNode.getProperties();
        const Alembic::AbcCoreAbstract::PropertyHeader * locHead =
            props.getPropertyHeader("locator");
        if (locHead != NULL && locHead->isScalar() &&
            locHead->getDataType().getPod() == Alembic::Util::kFloat64POD &&
            locHead->getDataType().getExtent() == 6)
        {
            Alembic::Abc::IScalarProperty locProp(props, "locator");
            bool isConstant = locProp.isConstant();

            Alembic::Abc::IScalarProperty visProp = getVisible(iNode,
                isConstant, mData.mPropList, mData.mAnimVisStaticObjList);

            // add animated locator to the list
            if (!isConstant)
                mData.mLocList.push_back(iNode);

            bool hasDag = false;
            if (mAction != NONE && mConnectDagNode.isValid())
            {
                hasDag = getDagPathByChildName(mConnectDagNode,
                    iNode.getName());
            }

            if ( mAction == CREATE || mAction == CREATE_REMOVE )
            {
                if (hasDag)
                {
                    xformObj = mConnectDagNode.node();
                }
                else
                {
                    xformObj = create(iNode, mParent, locProp, mConnectDagNode);
                    if (!isConstant)
                    {
                        mData.mLocObjList.push_back(xformObj);
                    }
                    addProps(arbProp, xformObj);
                }
                setConstantVisibility(visProp, xformObj);
            }

            if ( mAction >= CONNECT )
            {
                if (xformObj == MObject::kNullObj && getDagPathByChildName(
                    mConnectDagNode, iNode.getName()))
                {
                    xformObj = mConnectDagNode.node();
                    if (!xformObj.hasFn(MFn::kLocator))
                    {
                        MString theError("No connection done for node '");
                        theError += MString(iNode.getName().c_str());
                        theError += MString("' with ");
                        theError += mConnectDagNode.fullPathName();
                        printError(theError);
                        return status;
                    }
                    else
                    {
                        mConnectDagNode.pop();
                    }

                }

                addToPropList(firstProp, xformObj);
            }

            if (hasDag)
            {
                mConnectDagNode.pop();
            }
        }
    }
    else    // transform node
    {
        MString name(iNode.getName().c_str());

        size_t numChildren = iNode.getNumChildren();
        bool isConstant = iNode.getSchema().isConstant();

        Alembic::Abc::IScalarProperty visProp = getVisible(iNode,
            isConstant, mData.mPropList, mData.mAnimVisStaticObjList);

        Alembic::AbcGeom::XformSample samp;
        iNode.getSchema().get(samp, 0);
        if (!isConstant)
        {
            mData.mXformList.push_back(iNode);
            mData.mIsComplexXform.push_back(isComplex(samp));
        }

        if (isConstant && visProp.valid() && !visProp.isConstant())
        {
             mData.mAnimVisStaticObjList.push_back(iNode);
        }

        bool hasDag = false;
        if (mAction != NONE && mConnectDagNode.isValid())
        {
            hasDag = getDagPathByChildName(mConnectDagNode, iNode.getName());
        }

        // There might be children under the current DAG node that
        // doesn't exist in the file.
        // Remove them if the -removeIfNoUpdate flag is set
        if ((mAction == REMOVE || mAction == CREATE_REMOVE) &&
            mConnectDagNode.isValid())
        {
            unsigned int numDags = mConnectDagNode.childCount();
            std::vector<MDagPath> dagToBeRemoved;

            // get names of immediate children so we can compare with
            // the hierarchy in the scene
            std::set< std::string > childNodesInFile;
            for (size_t j = 0; j < numChildren; ++j)
            {
                Alembic::Abc::IObject child = iNode.getChild(j);
                childNodesInFile.insert(child.getName());
            }

            for (unsigned int i = 0; i < numDags; i++)
            {
                MObject child = mConnectDagNode.child(i);
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
                            getDagPathByName(fn.fullPathName(), dagPath);
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
            if (hasDag)
            {
                xformObj = mConnectDagNode.node();
            }
            else
            {
                MFnTransform trans;
                xformObj = trans.create(mParent, &status);
                trans.getPath(mConnectDagNode);

                if (status != MS::kSuccess)
                {
                    MString theError("Failed to create transform node ");
                    theError += name;
                    printError(theError);
                    return status;
                }

                trans.setName(name);
            }

            setConstantVisibility(visProp, xformObj);
            addProps(arbProp, xformObj);
        }

        if ( mAction >= CONNECT )
        {
            if (xformObj == MObject::kNullObj)
            {
                xformObj = mConnectDagNode.node();
            }

            if (xformObj.hasFn(MFn::kTransform))
            {
                std::vector<std::string> transopNameList;
                connectToXform(samp, isConstant, xformObj, transopNameList,
                    mData.mPropList, firstProp);

                if (!isConstant)
                {
                    SampledPair sampPair(xformObj, transopNameList);
                    mData.mXformOpList.push_back(sampPair);
                }
                addToPropList(firstProp, xformObj);
            }
            else
            {
                MString theError = mConnectDagNode.partialPathName();
                theError += MString(" is not compatible as a transform node. ");
                theError += MString("Connection failed.");
                printError(theError);
                return MS::kFailure;
            }

        }

        MObject saveParent = xformObj;
        for (size_t i = 0; i < numChildren; ++i)
        {
            Alembic::Abc::IObject child = iNode.getChild(i);
            mParent = saveParent;

            this->visit(child);
        }

        if (hasDag)
        {
            mConnectDagNode.pop();
        }
    }

    return status;
}
