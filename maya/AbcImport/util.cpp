//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include "util.h"

#include <maya/MAnimControl.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnTransform.h>
#include <maya/MFnSet.h>
#include <maya/MItDag.h>
#include <maya/MSelectionList.h>
#include <maya/MStringArray.h>
#include <maya/MObjectArray.h>
#include <maya/MTime.h>
#include <maya/MItDependencyGraph.h>

#include <algorithm>

MObject createShadingGroup(const MString& iName)
{
    MStatus status;

    MFnSet fnSet;
    MSelectionList selList;
    MObject shadingGroup = fnSet.create(selList,
                                        MFnSet::kRenderableOnly,
                                        &status);
    if (status != MS::kSuccess)
    {
        MString theError("Could not create shading engine: ");
        theError += iName;
        MGlobal::displayError(theError);

        return shadingGroup;
    }

    fnSet.setName(iName);

    return shadingGroup;
}

MObjectArray getOutConnectedSG( const MDagPath &shapeDPath )
{
    MStatus status;

    // Array of connected Shaging Engines
    MObjectArray connSG;

    // Iterator through the dependency graph to find if there are
    // shading engines connected
    MObject obj(shapeDPath.node()); // non const MObject
    MItDependencyGraph itDG( obj, MFn::kShadingEngine,
                             MItDependencyGraph::kDownstream,
                             MItDependencyGraph::kBreadthFirst,
                             MItDependencyGraph::kNodeLevel, &status );

    if( status == MS::kFailure )
        return connSG;

    // we want to prune the iteration if the node is not a shading engine
    itDG.enablePruningOnFilter();

    // iterate through the output connected shading engines
    for( ; itDG.isDone()!= true; itDG.next() )
        connSG.append( itDG.thisNode() );

    return connSG;
}

MStatus replaceDagObject(MObject & oldObject, MObject & newObject,
            const MString & name)
{
    MStatus status;
    MFnDagNode mFnOld(oldObject, &status);
    if (status == MS::kSuccess)
    {
        unsigned int numChild = mFnOld.childCount();

        std::vector<MObject> children;
        children.reserve(numChild);
        for (unsigned int i = 0; i < numChild; i++)
        {
            MObject child = mFnOld.child(i, &status);
            if (status == MS::kSuccess)
            {
                children.push_back(child);
            }
            else
            {
                MString theError("Failed to get child ");
                theError += i;
                theError += " of ";
                theError += mFnOld.name();
                theError += ", status = ";
                theError += status.errorString();
                MGlobal::displayError(theError);
            }
        }

        MFnDagNode mFnNew(newObject, &status);
        if (status == MS::kSuccess)
        {
            for (unsigned int i = 0; i < numChild; i++)
            {
                status = mFnNew.addChild(children[i]);
                if (status != MS::kSuccess)
                {
                    MString theError("Failed to add child ");
                    theError += i;
                    theError += " of ";
                    theError += mFnOld.name();
                    theError += " to ";
                    theError += name;
                    theError += ", status = ";
                    theError += status.errorString();
                    MGlobal::displayError(theError);
                }
            }
        }
    }
    return status;
}

void disconnectProps(MFnDependencyNode & iNode,
    std::vector<Prop> & iSampledPropList,
    std::size_t iFirstProp)
{
    // get prop names and make sure they are disconnected before
    // trying to connect to them
    std::size_t numProps = iSampledPropList.size();
    for (std::size_t i = iFirstProp; i < numProps; ++i)
    {
        std::string propName;
        if (iSampledPropList[i].mArray.valid())
        {
            propName = iSampledPropList[i].mArray.getName();
        }
        else
        {
            propName = iSampledPropList[i].mScalar.getName();
        }

        // disconnect connections to animated props
        MPlug dstPlug;
        if (propName == Alembic::AbcGeom::kVisibilityPropertyName)
        {
            dstPlug = iNode.findPlug("visibility");
        }
        else
        {
            dstPlug = iNode.findPlug(propName.c_str());
        }

        // make sure the long name matches
        if (propName == Alembic::AbcGeom::kVisibilityPropertyName ||
            dstPlug.partialName(false, false, false, false, false, true) == propName.c_str())
        {
            disconnectAllPlugsTo(dstPlug);
        }
    }
}

MStatus disconnectAllPlugsTo(MPlug & dstPlug)
{
    MStatus status = MS::kSuccess;
    MPlugArray array;
    dstPlug.connectedTo(array, true, false, &status);
    unsigned int arrayLength = array.length();

    for (unsigned int i = 0; i < arrayLength; i++)
    {
        MPlug srcPlug = array[i];
        if (status == MS::kSuccess)
        {
            MDGModifier modifier;
            status = modifier.disconnect(srcPlug, dstPlug);
            status = modifier.doIt();
            if (status != MS::kSuccess)
            {
                MString theError("Disconnect ");
                theError += srcPlug.name();
                theError += MString(" -> ");
                theError += dstPlug.name();
                theError += MString(" failed, status = ");
                theError += status.errorString();
                MGlobal::displayError(theError);
                return status;
            }
        }
    }
    return MS::kSuccess;
}

MStatus getDagPathListByName(const MString & objectNames,
    std::vector<MDagPath> & dagPathList)
{
    MStatus status;
    MStringArray theArray;
    status = objectNames.split(' ', theArray);
    if (status == MS::kSuccess)
    {
        unsigned int len = theArray.length();
        for (unsigned int i = 0; i < len; i++)
        {
            MDagPath dagPath;
            status = getDagPathByName(theArray[i], dagPath);
            if (status == MS::kSuccess)
                dagPathList.push_back(dagPath);
            else
            {
                MString theError(theArray[i]);
                theError += MString(" doesn't exist");
                MGlobal::displayError(theError);
            }
        }
    }

    return status;
}

MStatus getObjectByName(const MString & name, MObject & object)
{
    object = MObject::kNullObj;

    MSelectionList sList;
    MStatus status = sList.add(name);
    if (status == MS::kSuccess)
    {
        status = sList.getDependNode(0, object);
    }

    return status;
}

MStatus getDagPathByName(const MString & name, MDagPath & dagPath)
{
    MSelectionList sList;
    MStatus status = sList.add(name);
    if (status == MS::kSuccess)
    {
        status = sList.getDagPath(0, dagPath);
    }

    return status;
}

std::string stripPathAndNamespace(const std::string & iPath)
{
    std::string childName;
    std::size_t lastPath = iPath.rfind('|');
    if (lastPath != std::string::npos)
    {
        childName =  iPath.substr(lastPath + 1);
    }
    else
    {
        childName = iPath;
    }

    std::size_t lastNamespace = childName.rfind(':');
    if (lastNamespace != std::string::npos)
    {
        childName = childName.substr(lastNamespace+1);
    }
    return childName;
}

bool getDagPathByChildName(MDagPath & ioDagPath, const std::string & iChildName)
{
    unsigned int numChildren = ioDagPath.childCount();
    std::string strippedName = stripPathAndNamespace(iChildName);
    MObject closeMatch;

    for (unsigned int i = 0; i < numChildren; ++i)
    {
        MObject child = ioDagPath.child(i);
        MFnDagNode dagChild(child);
        std::string name = dagChild.partialPathName().asChar();
        if (name == iChildName)
        {
            ioDagPath.push(child);
            return true;
        }

        if (closeMatch.isNull())
        {
            if (strippedName == stripPathAndNamespace(name))
            {
                closeMatch = child;
            }
        }
    }

    if (!closeMatch.isNull())
    {
        ioDagPath.push(closeMatch);
        return true;
    }

    return false;
}

MStatus getPlugByName(const MString & objName, const MString & attrName,
            MPlug & plug)
{
    MObject object = MObject::kNullObj;
    MStatus status = getObjectByName(objName, object);
    if (status == MS::kSuccess)
    {
        MFnDependencyNode mFn(object, &status);
        if (status == MS::kSuccess)
            plug = mFn.findPlug(attrName, &status);
    }

    return status;
}

MStatus setPlayback(double min, double max)
{
    MStatus status = MS::kSuccess;
    MAnimControl anim;
    MTime minTime, maxTime, curTime;
    minTime.setValue(min);
    maxTime.setValue(max);

    status = anim.setMinTime(minTime);
    status = anim.setAnimationStartTime(minTime);

    if (max > 0)
    {
        status = anim.setMaxTime(maxTime);
        status = anim.setAnimationEndTime(maxTime);
    }

    return status;
}

MStatus setInitialShadingGroup(const MString & dagNodeName)
{
    MObject initShader;
    MDagPath dagPath;

    if (getObjectByName("initialShadingGroup", initShader) == MS::kSuccess &&
        getDagPathByName(dagNodeName, dagPath) == MS::kSuccess)
    {
        MFnSet set(initShader);
        set.addMember(dagPath);
    }
    else
    {
        MString theError("Error getting adding ");
        theError += dagNodeName;
        theError += MString(" to initalShadingGroup.");
        MGlobal::displayError(theError);
        return MS::kFailure;
    }
    return MS::kSuccess;
}

MStatus deleteDagNode(MDagPath & dagPath)
{
    MObject obj = dagPath.node();
    return MGlobal::deleteNode(obj);
}

MStatus deleteCurrentSelection()
{
    MStatus status;

    MDGModifier modifier;
    status = modifier.commandToExecute("\ndelete;\n");
    status = modifier.doIt();

    return status;
}

bool stripFileName(const MString & filePath, MString & fileName)
{
    std::string str(filePath.asChar());
    size_t found;
    found = str.find_last_of("/\\");
    str = str.substr(found+1);

    // str is now in the form of xxx.abc
    found = str.find_first_of(".");
    str = str.substr(0, found);
    fileName = MString(str.c_str());
    return true;
}

double getWeightAndIndex(double iFrame,
    Alembic::AbcCoreAbstract::TimeSamplingPtr iTime, size_t numSamps,
    Alembic::AbcCoreAbstract::index_t & oIndex,
    Alembic::AbcCoreAbstract::index_t & oCeilIndex)
{
    if (numSamps == 0)
        numSamps = 1;

    std::pair<Alembic::AbcCoreAbstract::index_t, double> floorIndex =
        iTime->getFloorIndex(iFrame, numSamps);

    oIndex = floorIndex.first;
    oCeilIndex = oIndex;

    if (fabs(iFrame - floorIndex.second) < 0.0001)
        return 0.0;

    std::pair<Alembic::AbcCoreAbstract::index_t, double> ceilIndex =
        iTime->getCeilIndex(iFrame, numSamps);

    if (oIndex == ceilIndex.first)
        return 0.0;

    oCeilIndex = ceilIndex.first;

    double alpha = (iFrame - floorIndex.second) /
        (ceilIndex.second - floorIndex.second);

    // we so closely match the ceiling so we'll just use it
    if (fabs(1.0 - alpha) < 0.0001)
    {
        oIndex = oCeilIndex;
        return 0.0;
    }

    return alpha;
}

bool isColorSet(const Alembic::AbcCoreAbstract::PropertyHeader & iHeader,
    bool iUnmarkedFaceVaryingColors)
{
    bool isColor = Alembic::AbcGeom::IC3fGeomParam::matches(iHeader) ||
            Alembic::AbcGeom::IC4fGeomParam::matches(iHeader);

    Alembic::AbcGeom::GeometryScope scope =
        Alembic::AbcGeom::GetGeometryScope(iHeader.getMetaData());

    bool isScoped = (scope == Alembic::AbcGeom::kFacevaryingScope ||
                     scope == Alembic::AbcGeom::kVaryingScope ||
                     scope == Alembic::AbcGeom::kVertexScope);
    return (isColor && isScoped &&
            (iUnmarkedFaceVaryingColors ||
            iHeader.getMetaData().get("mayaColorSet") != ""));
}
