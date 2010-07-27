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

#include <AlembicTakoImport/NodeIteratorVisitorHelper.h>
#include <AlembicTakoImport/Util.h>
#include <AlembicTakoImport/AlembicTakoCacheNode.h>
#include <AlembicTakoImport/CreateSceneVisitor.h>

namespace Alembic {

namespace
{
    std::string arbToString(Tako::ArbAttrType iType)
    {
        std::string retStr;

        switch (iType)
        {
            // do nothing case
            case Tako::ARBATTR_NONE:
            break;

            case Tako::ARBATTR_INT:
                retStr = "int";
            break;

            case Tako::ARBATTR_FLOAT:
                retStr = "float";
            break;

            case Tako::ARBATTR_DOUBLE:
                retStr = "double";
            break;

            case Tako::ARBATTR_STRING:
                retStr = "string";
            break;

            case Tako::ARBATTR_COLOR3:
                retStr = "color3";
            break;

            case Tako::ARBATTR_COLOR4:
                retStr = "color4";
            break;

            case Tako::ARBATTR_NORMAL2:
                retStr = "normal2";
            break;

            case Tako::ARBATTR_NORMAL3:
                retStr = "normal3";
            break;

            case Tako::ARBATTR_VECTOR2:
                retStr = "vector2";
            break;

            case Tako::ARBATTR_VECTOR3:
                retStr = "vector3";
            break;

            case Tako::ARBATTR_VECTOR4:
                retStr = "vector4";
            break;

            case Tako::ARBATTR_POINT2:
                retStr = "point2";
            break;

            case Tako::ARBATTR_POINT3:
                retStr = "point3";
            break;

            case Tako::ARBATTR_POINT4:
                retStr = "point4";
            break;
        }
        return retStr;
    }
}

AddAttrVisitor::AddAttrVisitor(std::string iAttrName, MObject & iParent,
    Tako::ArbAttrInfo & iInfo) :
    mAttrName(iAttrName), mParent(iParent)
{
    mInputType = iInfo.inputType;
    mOutputType = iInfo.outputType;
    mScope = iInfo.scope;
}

void AddAttrVisitor::operator()(const int8_t iProp)
{
    MStatus status;
    MFnDependencyNode mParentFn(mParent, &status);

    // it is not called visibility because on maya2tako side
    // it doesn't propagate the same way Maya would behave
    std::string visibilityAttrName = "visible";
    if (mAttrName == visibilityAttrName)
    {
        MPlug plug = mParentFn.findPlug("visibility", &status);
        plug.setValue(iProp != 0);
    }
    else
    {
        MFnNumericAttribute attr;
        MString attrName(mAttrName.c_str());
        MObject attrObj = attr.create(attrName, attrName,
            MFnNumericData::kByte, iProp, &status);
        attr.setKeyable(true);
        attr.setHidden(false);
        mParentFn.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);
        addArbAttrAndScope();
    }
}

void AddAttrVisitor::operator()(const int16_t iProp)
{
    MStatus status;
    MFnNumericAttribute attr;
    MString attrName(mAttrName.c_str());
    MObject attrObj = attr.create(attrName, attrName,
        MFnNumericData::kShort, iProp, &status);
    attr.setKeyable(true);
    attr.setHidden(false);
    MFnDependencyNode mParentFn(mParent, &status);
    mParentFn.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);
    addArbAttrAndScope();
}

void AddAttrVisitor::operator()(const int32_t iProp)
{
    MStatus status;
    MFnNumericAttribute attr;
    MString attrName(mAttrName.c_str());
    MObject attrObj = attr.create(attrName, attrName,
        MFnNumericData::kInt, iProp, &status);
    attr.setKeyable(true);
    attr.setHidden(false);
    MFnDependencyNode mParentFn(mParent, &status);
    mParentFn.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);
    addArbAttrAndScope();
}

void AddAttrVisitor::operator()(const int64_t iProp)
{
    MStatus status;
    MFnNumericAttribute attr;
    MString attrName(mAttrName.c_str());
    MObject attrObj = attr.create(attrName, attrName,
        MFnNumericData::kLong, iProp, &status);
    attr.setKeyable(true);
    attr.setHidden(false);
    MFnDependencyNode mParentFn(mParent, &status);
    mParentFn.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);
    addArbAttrAndScope();
}

void AddAttrVisitor::operator()(const float iProp)
{
    MStatus status;
    MFnNumericAttribute attr;
    MString attrName(mAttrName.c_str());
    MObject attrObj = attr.create(attrName, attrName,
        MFnNumericData::kFloat, iProp, &status);
    attr.setKeyable(true);
    attr.setHidden(false);
    MFnDependencyNode mParentFn(mParent, &status);
    mParentFn.addAttribute(attrObj,
        MFnDependencyNode::kLocalDynamicAttr);
    addArbAttrAndScope();
}

void AddAttrVisitor::operator()(const double iProp)
{
    MStatus status;
    MFnNumericAttribute attr;
    MString attrName(mAttrName.c_str());
    MObject attrObj = attr.create(attrName, attrName,
        MFnNumericData::kDouble, iProp, &status);
    attr.setKeyable(true);
    attr.setHidden(false);
    MFnDependencyNode mParentFn(mParent, &status);
    mParentFn.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);
    addArbAttrAndScope();
}

// to prevent crazy recursion we have this so addArbAttrAndScope
// and operator(const std::string & iProp) can call it
void AddAttrVisitor::addString(const std::string & iProp)
{
    MStatus status;
    MFnStringData fnStringData;
    MString attrValue(iProp.c_str());
    MString attrName(mAttrName.c_str());
    MObject strAttrObject = fnStringData.create(attrValue, &status);

    MFnTypedAttribute attr;
    MObject attrObj = attr.create(attrName, attrName, MFnData::kString,
        strAttrObject, &status);
    MFnDependencyNode mParentFn(mParent, &status);
    status = mParentFn.addAttribute(attrObj,
        MFnDependencyNode::kLocalDynamicAttr);
}

void AddAttrVisitor::operator()(const std::string & iProp)
{
    addString(iProp);
    addArbAttrAndScope();
}

void AddAttrVisitor::operator()(const std::vector<int16_t> & iProp)
{
    MStatus status;

    unsigned int length = iProp.size();
    MIntArray array;
    array.setLength(length);
    for (unsigned int i = 0; i < length; i++)
        array[i] = iProp[i];

    MFnIntArrayData mFn;
    MObject attrObject = mFn.create(array, &status);

    MFnTypedAttribute attr;
    MString attrName(mAttrName.c_str());
    MObject attrObj = attr.create(attrName, attrName,
        MFnData::kIntArray, attrObject, &status);
    attr.setKeyable(true);
    MFnDependencyNode mParentFn(mParent, &status);
    mParentFn.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);
    addArbAttrAndScope();
}

void AddAttrVisitor::operator()(const std::vector<int32_t> & iProp)
{
    MStatus status;

    unsigned int length = iProp.size();
    MIntArray array;
    array.setLength(length);
    for (unsigned int i = 0; i < length; i++)
        array[i] = iProp[i];

    MFnIntArrayData mFn;
    MObject attrObject = mFn.create(array, &status);

    MFnTypedAttribute attr;
    MString attrName(mAttrName.c_str());
    MObject attrObj = attr.create(attrName, attrName,
        MFnData::kIntArray, attrObject, &status);
    attr.setKeyable(true);
    MFnDependencyNode mParentFn(mParent, &status);
    mParentFn.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);
    addArbAttrAndScope();
}

void AddAttrVisitor::operator()(const std::vector<int64_t> & iProp)
{
    MStatus status;

    // add the attribute, Maya can't handle this so we'll
    // cast to doubles
    unsigned int length = iProp.size();
    MDoubleArray array;
    array.setLength(length);
    for (unsigned int i = 0; i < length; i++)
        array[i] = iProp[i];

    MFnDoubleArrayData mFn;
    MObject attrObject = mFn.create(array, &status);
    MFnTypedAttribute attr;
    MString attrName(mAttrName.c_str());
    MObject attrObj = attr.create(attrName, attrName,
        MFnData::kDoubleArray, attrObject, &status);
    attr.setKeyable(true);
    MFnDependencyNode mParentFn(mParent, &status);
    mParentFn.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);
    addArbAttrAndScope();
}

void AddAttrVisitor::operator()(const std::vector<float> & iProp)
{
    MStatus status;
    MString attrName(mAttrName.c_str());

    // could be any of the SPT_XXXColor (SPT_HwColor, etc. )
    if (mInputType == Tako::ARBATTR_COLOR3 && iProp.size() == 3)
    {
        MFnNumericAttribute attr;
        MObject attrObj = attr.createColor(attrName, attrName, &status);
        attr.setKeyable(true);
        attr.setDefault(iProp[0], iProp[1], iProp[2]);
        MFnDependencyNode mParentFn(mParent, &status);
        status = mParentFn.addAttribute(
            attrObj,  MFnDependencyNode::kLocalDynamicAttr);

        // set to none since we already handled it, we don't
        // want to add a color3 string for _ArbInput
        mInputType = Tako::ARBATTR_NONE;
        addArbAttrAndScope();
    }
    else
    {
        unsigned int length = iProp.size();
        MDoubleArray array;
        array.setLength(length);
        for (unsigned int i = 0; i < length; i++)
            array[i] = iProp[i];

        MFnDoubleArrayData mFn;
        MObject attrObject = mFn.create(array, &status);

        MFnTypedAttribute attr;
        MObject attrObj = attr.create(attrName, attrName,
            MFnData::kDoubleArray, attrObject, &status);
        attr.setKeyable(true);
        MFnDependencyNode mParentFn(mParent, &status);
        mParentFn.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);
        addArbAttrAndScope();
    }
}

void AddAttrVisitor::operator()(const std::vector<double> & iProp)
{
    MStatus status;

    // set the MDoubleArray
    unsigned int length = iProp.size();
    MDoubleArray array;
    array.setLength(length);
    for (unsigned int i = 0; i < length; i++)
        array[i] = iProp[i];

    MFnDoubleArrayData mFn;
    MObject attrObject = mFn.create(array, &status);

    MFnTypedAttribute attr;
    MString attrName(mAttrName.c_str());
    MObject attrObj = attr.create(attrName, attrName,
        MFnData::kDoubleArray, attrObject, &status);
    attr.setKeyable(true);
    MFnDependencyNode mParentFn(mParent, &status);
    mParentFn.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);
    addArbAttrAndScope();
}


void AddAttrVisitor::operator()(const std::vector<std::string> & iProp)
{
    MStatus status;

    unsigned int length = iProp.size();
    MStringArray array;
    array.setLength(length);
    for (unsigned int i = 0; i < length; i++)
        array[i] = iProp[i].c_str();

    MFnStringArrayData mFn;
    MObject attrObject = mFn.create(array, &status);

    MFnTypedAttribute attr;
    MString attrName(mAttrName.c_str());
    MObject attrObj = attr.create(attrName, attrName, MFnData::kStringArray,
        attrObject, &status);
    attr.setKeyable(true);
    MFnDependencyNode mParentFn(mParent, &status);
    mParentFn.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);
    addArbAttrAndScope();
}

void AddAttrVisitor::addArbAttrAndScope()
{
    if (mInputType != Tako::ARBATTR_NONE)
    {
        std::string arb = arbToString(mInputType);
        std::string oldName = mAttrName;
        mAttrName += "_AttrInput";
        addString(arb);
        mAttrName = oldName;
    }

    if (mOutputType != Tako::ARBATTR_NONE)
    {
        std::string arb = arbToString(mOutputType);
        std::string oldName = mAttrName;
        mAttrName += "_AttrOutput";
        addString(arb);
        mAttrName = oldName;
    }

    if (mScope != Tako::SCOPE_PRIMITIVE)
    {
        std::string attr;
        std::string oldName = mAttrName;
        mAttrName += "_AttrScope";

        switch (mScope)
        {
            case Tako::SCOPE_FACE:
            {
                attr = "face";
            }
            break;

            case Tako::SCOPE_POINT:
            {
                attr = "point";
            }
            break;

            case Tako::SCOPE_VERTEX:
            {
                attr = "vertex";
            }
            break;

            default:
            break;
        }

        addString(attr);
        mAttrName = oldName;
    }
}

//=============================================================================


void addProperties(float iFrame, Tako::HDFReaderNode & iNode,
    MObject iObject, std::vector<std::string> & iSampledPropNameList)
{
    iNode.readProperties(iFrame);

    // add custom-attributes to node
    Tako::PropertyMap::const_iterator propIter
        = iNode.beginNonSampledProperties();
    Tako::PropertyPair pPair;
    while (propIter != iNode.endNonSampledProperties())
    {
        pPair = propIter->second;
        AddAttrVisitor attrVisitor(
            propIter->first, iObject, pPair.second);
        boost::apply_visitor(attrVisitor, pPair.first);
        propIter++;
    }

    // Adding sampled attributes to node
    propIter = iNode.beginSampledProperties();
    while (propIter != iNode.endSampledProperties())
    {
        pPair = propIter->second;
        AddAttrVisitor attrVisitor(propIter->first, iObject, pPair.second);
        boost::apply_visitor(attrVisitor, pPair.first);
        iSampledPropNameList.push_back(propIter->first);
        propIter++;
    }
}

//=============================================================================


void addPolyProperties(float iFrame,
    const Tako::PolyMeshReaderPtr & iNode, MObject iObject,
    std::vector<std::string> & iSampledPropNameList)
{
    MStatus status = MS::kSuccess;

    // add attribute "noNormals" before going through all props
    // this way if prop "N" exists, noNormals will be set to off
    MFnNumericAttribute attr;
    MString attrName("noNormals");
    MObject attrObj = attr.create(attrName, attrName,
        MFnNumericData::kBoolean, true, &status);
    attr.setKeyable(true);
    attr.setHidden(false);
    MFnMesh fnMesh(iObject);
    fnMesh.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);

    // there can still potentially be input files that has arbitrary attr called
    // "N" that stores the normal data, the way we write out poly normals for
    // scenegraph version 1. Just ignoring this by skipping
    const std::string normalAttrName("N");
    const std::string uvAttrName("st");

    iNode->readProperties(iFrame);

    // add "noNormals" to its default value. If normals exist in the mesh,
    // reset the value to this attribute later

    // add custom-attributes to node
    Tako::PropertyMap::const_iterator propIter =
        iNode->beginNonSampledProperties();
    Tako::PropertyPair pPair;
    while (propIter != iNode->endNonSampledProperties())
    {
        pPair = propIter->second;
        if (propIter->first != uvAttrName)
        {
            AddAttrVisitor attrVisitor(propIter->first, iObject,
                pPair.second);
            boost::apply_visitor(attrVisitor, pPair.first);
        }
        propIter++;
    }

    // Adding sampled attributes to node
    propIter = iNode->beginSampledProperties();
    while (propIter != iNode->endSampledProperties())
    {
        pPair = propIter->second;

        AddAttrVisitor attrVisitor(propIter->first, iObject,
            pPair.second);
        boost::apply_visitor(attrVisitor, pPair.first);
        iSampledPropNameList.push_back(propIter->first);

        propIter++;
    }
}

//=============================================================================


void ReadPropAttrVisitor::setArbAttrInfo(const Tako::ArbAttrInfo & iArbAttrInfo)
{
    mArbAttrInfo = iArbAttrInfo;
}

void ReadPropAttrVisitor::setHandle(MDataHandle & iHandle)
{
    mMode = 0;
    mHandle = iHandle;
}

void ReadPropAttrVisitor::setPlug(MPlug & iPlug)
{
    mMode = 1;
    mPlug = iPlug;
}

MStatus ReadPropAttrVisitor::operator()(const int8_t & iProp)
{
    MStatus status = MS::kSuccess;
    if (mMode == 0)
        mHandle.setGenericChar(iProp, true);
    else if (mMode == 1)
        status = mPlug.setValue(iProp);

    return status;
}

MStatus ReadPropAttrVisitor::operator()(const int16_t & iProp)
{
    MStatus status = MS::kSuccess;
    if (mMode == 0)
        mHandle.setGenericShort(iProp, true);
    else if (mMode == 1)
        status = mPlug.setValue(iProp);

    return status;
}

MStatus ReadPropAttrVisitor::operator()(const int32_t & iProp)
{
    MStatus status = MS::kSuccess;
    if (mMode == 0)
        mHandle.setGenericInt(iProp, true);
    else if (mMode == 1)
        status = mPlug.setValue(iProp);

    return status;
}

MStatus ReadPropAttrVisitor::operator()(const int64_t & iProp)
{
    MStatus status = MS::kSuccess;

    return status;
}

MStatus ReadPropAttrVisitor::operator()(const float & iProp)
{
    MStatus status = MS::kSuccess;
    if (mMode == 0)
        mHandle.setGenericFloat(iProp, true);
    else if (mMode == 1)
        status = mPlug.setValue(iProp);

    return status;
}

MStatus ReadPropAttrVisitor::operator()(const double & iProp)
{
    MStatus status = MS::kSuccess;
    if (mMode == 0)
        mHandle.setGenericDouble(iProp, true);
    else if (mMode == 1)
        status = mPlug.setValue(iProp);

    return status;
}

MStatus ReadPropAttrVisitor::operator()(const std::vector<int16_t> & iProp)
{
    MStatus status;
    unsigned int length = iProp.size();
    MIntArray array;
    array.setLength(length);
    for (unsigned int i = 0; i < length; i++)
        array[i] = iProp[i];

    MFnIntArrayData mFn;
    MObject attrObject = mFn.create(array, &status);

    if (mMode == 0)
        status = mHandle.set(attrObject);
    else if (mMode == 1)
        status = mPlug.setValue(attrObject);

    return status;
}

MStatus ReadPropAttrVisitor::operator()(const std::vector<int32_t> & iProp)
{
    MStatus status = MS::kSuccess;
    unsigned int length = iProp.size();
    MIntArray array;
    array.setLength(length);
    for (unsigned int i = 0; i < length; i++)
        array[i] = iProp[i];

    MFnIntArrayData mFn;
    MObject attrObject = mFn.create(array, &status);

    if (mMode == 0)
        status = mHandle.set(attrObject);
    else if (mMode == 1)
        status = mPlug.setValue(attrObject);

    return status;
}

MStatus ReadPropAttrVisitor::operator()(const std::vector<int64_t> & iProp)
{
    return MS::kSuccess;
}

MStatus ReadPropAttrVisitor::operator()(const std::vector<float> & iProp)
{
    MStatus status = MS::kSuccess;
    static unsigned int colorCnt = 0;

    if ( mArbAttrInfo.inputType == Tako::ARBATTR_COLOR3 )
    {
        if (mMode == 0)
        {
            float val = 0.0;
            if (iProp.size() > colorCnt)
                val = iProp[colorCnt];

            mHandle.setGenericFloat(val, true);

            colorCnt++;
            if (colorCnt == 3)
                colorCnt = 0;
        }
        else if (mMode == 1 && mPlug.isCompound())
        {
            unsigned int numChildren = mPlug.numChildren();
            for (unsigned int i = 0; i < numChildren; i++)
            {
                MPlug childPlug = mPlug.child(i, &status);
                if (status == MS::kSuccess)
                    childPlug.setValue(iProp[i]);
            }
        }
    }
    else  // non-color float vector
    {
        if (mMode == 0)
        {
            unsigned int length = iProp.size();
            MDoubleArray array;
            array.setLength(length);
            for (unsigned int i = 0; i < length; i++)
                array[i] = iProp[i];
            MFnDoubleArrayData mFn;
            MObject valObj = mFn.create(array, &status);
            mHandle.setMObject(valObj);
        }
    }

    return status;
}

MStatus ReadPropAttrVisitor::operator()(const std::vector<double> & iProp)
{
    MStatus status = MS::kSuccess;
    static unsigned int vec3Cnt = 0;

    if ( mArbAttrInfo.inputType == Tako::ARBATTR_COLOR3
        || mArbAttrInfo.inputType == Tako::ARBATTR_VECTOR3 )
    {
        if (mMode == 0)
        {
            double val = 0.0;
            if (iProp.size() > vec3Cnt)
                val = iProp[vec3Cnt];
            else
                val = 0.0;  // give a default value here

            mHandle.setGenericDouble(val, true);

            vec3Cnt++;
            if (vec3Cnt == 3)
                vec3Cnt = 0;
        }
        else if (mMode == 1 && mPlug.isCompound())
        {
            unsigned int numChildren = mPlug.numChildren();
            for (unsigned int i = 0; i < numChildren; i++)
            {
                MPlug childPlug = mPlug.child(i, &status);
                if (status == MS::kSuccess)
                    childPlug.setValue(iProp[i]);
            }
        }
    }
    else  // non-color double vector
    {
        if (mMode == 0)
        {
            unsigned int length = iProp.size();
            MDoubleArray array;
            array.setLength(length);
            for (unsigned int i = 0; i < length; i++)
                array[i] = iProp[i];
            MFnDoubleArrayData mFn;
            MObject valObj = mFn.create(array, &status);
            mHandle.setMObject(valObj);
        }
    }

    return status;
}

MStatus ReadPropAttrVisitor::operator()(const std::string & iProp)
{
    MStatus status = MS::kSuccess;
    if (mMode == 0)
    {
        MString str = iProp.c_str();
        mHandle.setString(str);
    }
    else if (mMode == 1)
    {
        MString str = iProp.c_str();
        status = mPlug.setValue(str);
    }

    return status;
}

MStatus ReadPropAttrVisitor::operator()(const std::vector< std::string > &
    iProp)
{
    return MS::kSuccess;
}

WriterData::WriterData()
{
}

WriterData::WriterData(const WriterData & rhs)
{
    *this = rhs;
}

WriterData & WriterData::operator=(const WriterData & rhs)
{
    mSubDNodePtrList = rhs.mSubDNodePtrList;
    mPolyNodePtrList = rhs.mPolyNodePtrList;
    mCameraNodePtrList = rhs.mCameraNodePtrList;
    mParticleNodePtrList = rhs.mParticleNodePtrList;
    mTransOpNodePtrList = rhs.mTransOpNodePtrList;
    mNurbsSurfaceNodePtrList = rhs.mNurbsSurfaceNodePtrList;
    mNurbsCurveNodePtrList = rhs.mNurbsCurveNodePtrList;
    mPropNodePtrList = rhs.mPropNodePtrList;

    // get all the sampled Maya objects
    mSubDObjList = rhs.mSubDObjList;
    mPolyObjList = rhs.mPolyObjList;
    mCameraObjList = rhs.mCameraObjList;
    mParticleObjList = rhs.mParticleObjList;
    mTransOpList = rhs.mTransOpList;
    mNurbsSurfaceObjList  = rhs.mNurbsSurfaceObjList;
    mNurbsCurveObjList = rhs.mNurbsCurveObjList;
    mPropList = rhs.mPropList;

    mIsComplexTrans = rhs.mIsComplexTrans;
    mIsSampledTransOpAngle = rhs.mIsSampledTransOpAngle;
    mNurbsCurveNumCurveList = rhs.mNurbsCurveNumCurveList;
    mParticlePos = rhs.mParticlePos;
    mParticleIds = rhs.mParticleIds;

    return *this;
}

ArgData::ArgData(MString iFileName,
    double iSequenceStartFrame, double iSequenceEndFrame,
    bool iDebugOn, MObject iReparentObj, bool iConnect,
    MString iConnectRootNodes, bool iCreateIfNotFound, bool iRemoveIfNoUpdate,
    bool iLoadUVs, bool iLoadNormals) :
        mFileName(iFileName),
        mSequenceStartFrame(iSequenceStartFrame),
        mSequenceEndFrame(iSequenceEndFrame),
        mDebugOn(iDebugOn), mReparentObj(iReparentObj), mConnect(iConnect),
        mConnectRootNodes(iConnectRootNodes),
        mCreateIfNotFound(iCreateIfNotFound),
        mRemoveIfNoUpdate(iRemoveIfNoUpdate),
        mLoadUVs(iLoadUVs), mLoadNormals(iLoadNormals)
{
    setDebug(mDebugOn);
}

ArgData::ArgData(const ArgData & rhs)
{
    *this = rhs;
}

ArgData & ArgData::operator=(const ArgData & rhs)
{
    mFileName = rhs.mFileName;
    mSequenceStartFrame = rhs.mSequenceStartFrame;
    mSequenceEndFrame = rhs.mSequenceEndFrame;
    mDebugOn = rhs.mDebugOn;

    mReparentObj = rhs.mReparentObj;

    // optional information for the "connect" flag
    mConnect = rhs.mConnect;
    mConnectRootNodes = rhs.mConnectRootNodes;
    mCreateIfNotFound = rhs.mCreateIfNotFound;
    mRemoveIfNoUpdate = rhs.mRemoveIfNoUpdate;

    // optional information for loading normals and(or) uvs
    mLoadUVs = rhs.mLoadUVs;
    mLoadNormals = rhs.mLoadNormals;

    mData = rhs.mData;

    return *this;
}

MString createScene(ArgData & iArgData)
{
    MString returnName("");

    Tako::TransformReaderPtr rNode(
      new Tako::TransformReader(iArgData.mFileName.asChar()));

    std::set<float> frames;
    getTakoFrameRange(rNode,
        iArgData.mSequenceStartFrame, iArgData.mSequenceEndFrame, frames);
    Tako::ChildNodePtr rootNode = rNode;

    CreateSceneVisitor visitor(
        iArgData.mSequenceStartFrame, iArgData.mReparentObj);

    visitor.setLoadUVNormalState(iArgData.mLoadUVs, iArgData.mLoadNormals);

    visitor.setConnectArgs(iArgData.mConnect, iArgData.mConnectRootNodes,
        iArgData.mCreateIfNotFound, iArgData.mRemoveIfNoUpdate);

    boost::apply_visitor(visitor, rootNode);

    if (visitor.hasSampledData())
    {
        visitor.getData(iArgData.mData);
        returnName = connectAttr(iArgData);
    }

    if (iArgData.mConnect)
    {
        visitor.applyShaderSelection();
    }

    return returnName;
}

MString connectAttr(ArgData & iArgData)
{
    MStatus status = MS::kSuccess;

    // create a new AlembicTakoCacheNode and initialize all its input attributes
    MDGModifier modifier;
    MPlug srcPlug, dstPlug;

    MObject takoCacheNodeObj =
        modifier.createNode("AlembicTakoCacheNode", &status);
    MFnDependencyNode takoCacheNodeFn(takoCacheNodeObj, &status);

    AlembicTakoCacheNode *takoNodePtr =
        reinterpret_cast<AlembicTakoCacheNode*>(takoCacheNodeFn.userNode(&status));
    if (status == MS::kSuccess)
    {
        takoNodePtr->setReaderPtrList(iArgData.mData);
        takoNodePtr->setDebugMode(iArgData.mDebugOn);
    }

    // set AlembicTakoCacheNode name
    MString fileName;
    stripTakoFileName(iArgData.mFileName, fileName);
    MString takoCacheNodeName = fileName +"_AlembicTakoCacheNode";
    takoCacheNodeFn.setName(takoCacheNodeName, &status);

    // set input file name
    MPlug plug = takoCacheNodeFn.findPlug("AlembicTako_File", true, &status);
    plug.setValue(iArgData.mFileName);

    // set sequence string
    plug = takoCacheNodeFn.findPlug("sequence", true, &status);
    MString seqStr;
    seqStr += iArgData.mSequenceStartFrame;
    seqStr += " - ";
    seqStr += iArgData.mSequenceEndFrame;
    plug.setValue(seqStr);

    // set load uvs flag
    plug = takoCacheNodeFn.findPlug("loadUV", true, &status);
    plug.setValue(iArgData.mLoadUVs);
    // set load normals flag
    plug = takoCacheNodeFn.findPlug("loadNormal", true, &status);
    plug.setValue(iArgData.mLoadNormals);

    // set connect input info
    plug = takoCacheNodeFn.findPlug("connect", true, &status);
    plug.setValue(iArgData.mConnect);
    plug = takoCacheNodeFn.findPlug("createIfNotFound", true, &status);
    plug.setValue(iArgData.mCreateIfNotFound);
    plug = takoCacheNodeFn.findPlug("removeIfNoUpdate", true, &status);
    plug.setValue(iArgData.mRemoveIfNoUpdate);
    plug = takoCacheNodeFn.findPlug("connectRoot", true, &status);
    plug.setValue(iArgData.mConnectRootNodes);

    MFnIntArrayData fnIntArray;
    fnIntArray.create();
    MObject intArrayObj;
    MIntArray intArray;
    unsigned length;

    // set the mIsSampledTransOpAngle
    length = iArgData.mData.mIsSampledTransOpAngle.size();
    if (length > 0)
    {
        plug = takoCacheNodeFn.findPlug("transOpAngle", true, &status);
        intArray.setLength(length);
        for (unsigned int i = 0; i < length; i++)
            intArray.set(iArgData.mData.mIsSampledTransOpAngle[i], i);
        status = fnIntArray.set(intArray);
        intArrayObj = fnIntArray.object(&status);
        status = plug.setValue(intArrayObj);
    }

    // set the mNurbsCurveNumCurveList
    length = iArgData.mData.mNurbsCurveNumCurveList.size();
    if (length > 0)
    {
        plug = takoCacheNodeFn.findPlug("numCurve", true, &status);
        intArray.setLength(length);
        for (unsigned int i = 0; i < length; i++)
            intArray.set(iArgData.mData.mNurbsCurveNumCurveList[i], i);
        status = fnIntArray.set(intArray);
        intArrayObj = fnIntArray.object(&status);
        status = plug.setValue(intArrayObj);
    }

    // make connection: time1.outTime --> AlembicTakoCacheNode.intime
    dstPlug = takoCacheNodeFn.findPlug("time", true, &status);
    status = getPlugByName("time1", "outTime", srcPlug);
    status = modifier.connect(srcPlug, dstPlug);
    status = modifier.doIt();

    unsigned int subDSize       = iArgData.mData.mSubDObjList.size();
    unsigned int polySize       = iArgData.mData.mPolyObjList.size();
    unsigned int cameraSize     = iArgData.mData.mCameraObjList.size();
    unsigned int particleSize   = iArgData.mData.mParticleObjList.size();
    unsigned int transopSize    = iArgData.mData.mTransOpList.size();
    unsigned int nSurfaceSize   = iArgData.mData.mNurbsSurfaceObjList.size();
    unsigned int nCurveSize     = iArgData.mData.mNurbsCurveObjList.size();
    unsigned int propSize       = iArgData.mData.mPropList.size();

    // making dynamic connections
    if (particleSize > 0)
    {
        printWarning("Currently no support for animated particle system");
    }
    if (cameraSize > 0)
    {
        MPlug srcArrayPlug = takoCacheNodeFn.findPlug("outCamera", true);

        unsigned int logicalIndex = 0;
        for (unsigned int i = 0; i < cameraSize; i++)
        {
            MFnCamera fnCamera(iArgData.mData.mCameraObjList[i]);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("centerOfInterest", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("focalLength", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("lensSqueezeRatio", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("cameraScale", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("horizontalFilmAperture", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("verticalFilmAperture", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("horizontalFilmOffset", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("verticalFilmOffset", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("filmFitOffset", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("overscan", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("nearClipPlane", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("farClipPlane", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("preScale", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug  = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("filmTranslateH", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("filmTranslateV", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("horizontalRollPivot", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("verticalRollPivot", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("filmRollValue", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("postScale", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("orthographicWidth", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("fStop", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("focusDistance", true);
            status = modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("shutterAngle", true);
            status = modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("tumblePivotX", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("tumblePivotY", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("tumblePivotZ", true);
            modifier.connect(srcPlug, dstPlug);

            status = modifier.doIt();
        }
    }
    if (nSurfaceSize > 0)
    {
        MPlug srcArrayPlug = takoCacheNodeFn.findPlug("outNSurface", true);
        for (unsigned int i = 0; i < nSurfaceSize; i++)
        {
            srcPlug = srcArrayPlug.elementByLogicalIndex(i);
            MFnNurbsSurface fnNSurface(iArgData.mData.mNurbsSurfaceObjList[i]);
            dstPlug = fnNSurface.findPlug("create", true);
            modifier.connect(srcPlug, dstPlug);
            status = modifier.doIt();
        }
    }
    if (nCurveSize > 0)
    {
        MPlug srcArrayPlug = takoCacheNodeFn.findPlug("outNCurveGrp", true);
        for (unsigned int i = 0; i < nCurveSize; i++)
        {
            srcPlug = srcArrayPlug.elementByLogicalIndex(i);
            MFnNurbsCurve fnNCurve(iArgData.mData.mNurbsCurveObjList[i]);
            dstPlug = fnNCurve.findPlug("create", true);
            modifier.connect(srcPlug, dstPlug);
            status = modifier.doIt();
        }
    }

    if (propSize > 0)
    {
        MPlug srcArrayPlug = takoCacheNodeFn.findPlug("prop", true, &status);
        unsigned int index = 0;
        for (unsigned int i = 0 ; i < propSize; i++)
        {
            SampledPair mSampledPair = iArgData.mData.mPropList[i];
            MObject mObject = mSampledPair.getObject();
            MFnDependencyNode mFn(mObject);

            unsigned int sampleSize = mSampledPair.sampledChannelSize();
            for (unsigned int j = 0; j < sampleSize; j ++)
            {
                std::string attrName = mSampledPair.getSampleElement(j);
                if (attrName == "visible")
                    dstPlug = mFn.findPlug("visibility", true, &status);
                else
                    dstPlug = mFn.findPlug(attrName.c_str(), true, &status);

                if (status != MS::kSuccess)
                {
                    MString theError(attrName.c_str());
                    theError += MString(" not found for connection");
                    printError(theError);
                    continue;
                }

                if (!dstPlug.isCompound())
                {
                    srcPlug = srcArrayPlug.elementByLogicalIndex(index++);

                    if (!dstPlug.isConnected())
                    {
                        status = modifier.connect(srcPlug, dstPlug);
                        status = modifier.doIt();
                    }

                    // The attr is already connected. Since this typically only
                    // occurs when SPT_HwColor AND the individual components
                    // (SPT_HwColor R,G, and B) are written out, we'll just
                    // create a float value.  If we later care about this
                    // duplicated data, we may want to set it to its proper
                    // type.  This probably wouldn't work for strings, but
                    // I am not currently aware of a workflow that would have
                    // duplicated non MFnNumericData.
                    else
                    {
                        // first try to create the new attribute
                        MString dupName("dup_");
                        dupName += dstPlug.partialName(false, false, false,
                            false, false, true);
                        MFnNumericAttribute attr;
                        MObject attrObj = attr.create(dupName, dupName,
                            MFnNumericData::kFloat, 0.0f);
                        attr.setKeyable(true);
                        attr.setHidden(false);
                        status = mFn.addAttribute(attrObj,
                            MFnDependencyNode::kLocalDynamicAttr);

                        if (status != MS::kSuccess)
                        {
                            MString theError("Duplicated Attribute ");
                            theError += dupName;
                            theError += MString(" could not be created");
                            printError(theError);
                            continue;
                        }

                        // now try again to grab the new plug
                        dstPlug = mFn.findPlug(dupName, true, &status);
                        if (status != MS::kSuccess)
                        {
                            MString theError = dupName;
                             theError += MString(
                             " new duplicate not found for connection");
                            printError(theError);
                            continue;
                        }

                        status = modifier.connect(srcPlug, dstPlug);
                        status = modifier.doIt();
                    }

                    if (status != MS::kSuccess)
                    {
                        MString theError(srcPlug.name());
                        theError += MString(" --> ");
                        theError += dstPlug.name();
                        theError += MString(" connection not made");
                        printError(theError);
                    }
                }
                else
                {
                    unsigned int numChildren = dstPlug.numChildren();
                    for (unsigned int i = 0; i < numChildren; i++)
                    {
                        srcPlug = srcArrayPlug.elementByLogicalIndex(index++);
                        MPlug childPlug = dstPlug.child(i, &status);
                        if (status == MS::kSuccess)
                        {
                            status = modifier.connect(srcPlug, childPlug);
                            status = modifier.doIt();
                            if (status != MS::kSuccess)
                            {
                                MString theError(srcPlug.name());
                                theError += MString(" --> ");
                                theError += childPlug.name();
                                theError +=
                                    MString(" child plug connection not made");
                                printError(theError);
                            }
                        }
                    }
                }
            }
        }
    }

    if (subDSize > 0)
    {
        MPlug srcArrayPlug = takoCacheNodeFn.findPlug("outSubDMesh", true);
        for (unsigned int i = 0; i < subDSize; i++)
        {
            srcPlug = srcArrayPlug.elementByLogicalIndex(i);
            MFnMesh mFn(iArgData.mData.mSubDObjList[i], &status);
            dstPlug = mFn.findPlug("inMesh", true, &status);
            status = modifier.connect(srcPlug, dstPlug);
            status = modifier.doIt();
            if (status != MS::kSuccess)
            {
                MString theError("AlembicTakoCacheNode.outSubDMesh[");
                theError += i;
                theError += "] --> ";
                theError += mFn.name();
                theError += ".inMesh connection not made";
                printError(theError);
            }
        }
    }

    if (polySize > 0)
    {
        MPlug srcArrayPlug = takoCacheNodeFn.findPlug("outPolyMesh", true);
        for (unsigned int i = 0; i < polySize; i++)
        {
            srcPlug = srcArrayPlug.elementByLogicalIndex(i);
            MFnMesh mFn(iArgData.mData.mPolyObjList[i]);
            dstPlug = mFn.findPlug("inMesh", true);
            modifier.connect(srcPlug, dstPlug);
            status = modifier.doIt();
        }
    }

    if (transopSize > 0)
    {
        unsigned int isSampledAngleIndex = 0;

        unsigned int logicalIndex = 0;
        MPlug srcArrayPlug = takoCacheNodeFn.findPlug("transOp", true);

        // for angleArray Attribute
        unsigned int angleLogicalIndex = 0;
        MPlug srcAngleArrayPlug =
            takoCacheNodeFn.findPlug("outTransOpAngle", true);

        for (unsigned int i = 0 ; i < transopSize; i++)
        {
            SampledPair mSampledPair = iArgData.mData.mTransOpList[i];
            MObject mObject = mSampledPair.getObject();
            MFnTransform mFn(mObject, &status);
            unsigned int sampleSize = mSampledPair.sampledChannelSize();
            for (unsigned int j = 0; j < sampleSize; j ++)
            {
                // is angle channel
                if (iArgData.mData.mIsSampledTransOpAngle[isSampledAngleIndex++] == true)
                {
                    srcPlug = srcAngleArrayPlug.elementByLogicalIndex(
                        angleLogicalIndex++);
                }
                else
                {
                    srcPlug = srcArrayPlug.elementByLogicalIndex(
                        logicalIndex++);
                }

                std::string attrName = mSampledPair.getSampleElement(j);
                dstPlug = mFn.findPlug(attrName.c_str(), true);
                if (dstPlug.isNull())
                    continue;

                modifier.connect(srcPlug, dstPlug);
                status = modifier.doIt();
            }
        }
    }

    return takoCacheNodeFn.name();
}

} // End namespace Alembic

