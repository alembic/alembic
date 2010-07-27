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

#include <AlembicTakoSPIImport/AlembicTakoCacheNode.h>
#include <AlembicTakoSPIImport/Util.h>
#include <AlembicTakoSPIImport/CreateSceneVisitor.h>
#include <AlembicTakoSPIImport/MeshHelper.h>
#include <AlembicTakoSPIImport/LightHelper.h>
#include <AlembicTakoSPIImport/CameraHelper.h>
#include <AlembicTakoSPIImport/PointPrimitiveHelper.h>
#include <AlembicTakoSPIImport/NurbsSurfaceHelper.h>
#include <AlembicTakoSPIImport/NurbsCurveHelper.h>
#include <AlembicTakoSPIImport/TransformHelper.h>

namespace Alembic {

MObject AlembicTakoCacheNode::mSequenceAttr;
MObject AlembicTakoCacheNode::mTimeAttr;
MObject AlembicTakoCacheNode::mTakoFileNameAttr;

MObject AlembicTakoCacheNode::mLoadUVsAttr;
MObject AlembicTakoCacheNode::mLoadNormalsAttr;

MObject AlembicTakoCacheNode::mConnectAttr;
MObject AlembicTakoCacheNode::mCreateIfNotFoundAttr;
MObject AlembicTakoCacheNode::mRemoveIfNoUpdateAttr;
MObject AlembicTakoCacheNode::mConnectRootNodesAttr;

MObject AlembicTakoCacheNode::mSampledNurbsCurveNumCurveAttr;
MObject AlembicTakoCacheNode::mIsSampledTransOpAngleAttr;

MObject AlembicTakoCacheNode::mOutSubDArrayAttr;
MObject AlembicTakoCacheNode::mOutPolyArrayAttr;
MObject AlembicTakoCacheNode::mOutCameraArrayAttr;
MObject AlembicTakoCacheNode::mOutNurbsCurveGrpArrayAttr;
MObject AlembicTakoCacheNode::mOutNurbsSurfaceArrayAttr;
MObject AlembicTakoCacheNode::mOutTransOpArrayAttr;
MObject AlembicTakoCacheNode::mOutTransOpAngleArrayAttr;
MObject AlembicTakoCacheNode::mOutPropArrayAttr;

MStatus AlembicTakoCacheNode::initialize()
{
    MStatus status;

    MFnUnitAttribute    uAttr;
    MFnTypedAttribute   tAttr;
    MFnNumericAttribute nAttr;
    MFnGenericAttribute gAttr;

    // add the input attributes: time, file, sequence time
    mTimeAttr = uAttr.create("time", "tm", MFnUnitAttribute::kTime, 0.0);
    status = uAttr.setStorable(true);
    status = addAttribute(mTimeAttr);

    // input file name
    MFnStringData fileFnStringData;
    MObject fileNameDefaultObject = fileFnStringData.create("");
    mTakoFileNameAttr = tAttr.create("AlembicTako_File", "fn",
        MFnData::kString, fileNameDefaultObject);
    status = tAttr.setStorable(true);
    status = addAttribute(mTakoFileNameAttr);

    // sequence to be loaded
    MFnStringData seqFnStringData;
    MObject seqenceDefaultObject = seqFnStringData.create("0 - 0");
    mSequenceAttr = tAttr.create("sequence", "sq",
        MFnData::kString, seqenceDefaultObject);
    status = tAttr.setStorable(true);
    status = addAttribute(mSequenceAttr);

    // set mLoadUVs
    mLoadUVsAttr = nAttr.create("loadUV", "uv",
        MFnNumericData::kBoolean, false, &status);
    status = nAttr.setStorable(true);
    status = addAttribute(mLoadUVsAttr);

    // set mLoadNormals
    mLoadNormalsAttr = nAttr.create("loadNormal", "n",
        MFnNumericData::kBoolean, false, &status);
    status = nAttr.setStorable(true);
    status = addAttribute(mLoadNormalsAttr);

    // set mConnect
    mConnectAttr = nAttr.create("connect", "ct",
        MFnNumericData::kBoolean, false, &status);
    status = nAttr.setStorable(true);
    status = addAttribute(mConnectAttr);

    // set mCreateIfNotFound
    mCreateIfNotFoundAttr = nAttr.create("createIfNotFound", "crt",
        MFnNumericData::kBoolean, false, &status);
    status = nAttr.setStorable(true);
    status = addAttribute(mCreateIfNotFoundAttr);

    // set mRemoveIfNoUpdate
    mRemoveIfNoUpdateAttr = nAttr.create("removeIfNoUpdate", "rm",
        MFnNumericData::kBoolean, false, &status);
    status = nAttr.setStorable(true);
    status = addAttribute(mRemoveIfNoUpdateAttr);

    // set mConnectUpdateNodes
    fileNameDefaultObject = fileFnStringData.create("");
    mConnectRootNodesAttr = tAttr.create("connectRoot", "cr",
        MFnData::kString, fileNameDefaultObject);
    status = tAttr.setStorable(true);
    status = addAttribute(mConnectRootNodesAttr);

    // add the output attributes
    // sampled subD mesh
    MFnMeshData fnMeshData;
    MObject meshDefaultObject = fnMeshData.create(&status);
    mOutSubDArrayAttr = tAttr.create("outSubDMesh", "osubd",
        MFnData::kMesh, meshDefaultObject);
    status = tAttr.setStorable(false);
    status = tAttr.setWritable(false);
    status = tAttr.setKeyable(false);
    status = tAttr.setArray(true);
    status = tAttr.setUsesArrayDataBuilder(true);
    status = addAttribute(mOutSubDArrayAttr);

    // sampled poly mesh
    mOutPolyArrayAttr = tAttr.create("outPolyMesh", "opoly",
        MFnData::kMesh, meshDefaultObject);
    status = tAttr.setStorable(false);
    status = tAttr.setWritable(false);
    status = tAttr.setKeyable(false);
    status = tAttr.setArray(true);
    status = tAttr.setUsesArrayDataBuilder(true);
    status = addAttribute(mOutPolyArrayAttr);

    // sampled nurbs surface
    MFnNurbsSurfaceData fnNSData;
    MObject nsDefaultObject = fnNSData.create(&status);
    mOutNurbsSurfaceArrayAttr = tAttr.create("outNSurface", "ons",
        MFnData::kNurbsSurface, nsDefaultObject);
    status = tAttr.setStorable(false);
    status = tAttr.setWritable(false);
    status = tAttr.setKeyable(false);
    status = tAttr.setArray(true);
    status = tAttr.setUsesArrayDataBuilder(true);
    status = addAttribute(mOutNurbsSurfaceArrayAttr);

    // sampled nurbs curve group
    MFnNurbsCurveData fnNCData;
    MObject ncDefaultObject = fnNCData.create(&status);
    mOutNurbsCurveGrpArrayAttr = tAttr.create("outNCurveGrp", "onc",
        MFnData::kNurbsCurve, ncDefaultObject);
    status = tAttr.setStorable(false);
    status = tAttr.setWritable(false);
    status = tAttr.setKeyable(false);
    status = tAttr.setArray(true);
    status = tAttr.setUsesArrayDataBuilder(true);
    status = addAttribute(mOutNurbsCurveGrpArrayAttr);

    // sampled transform operations
    mOutTransOpArrayAttr = nAttr.create("transOp", "to",
        MFnNumericData::kDouble, 0.0, &status);
    status = nAttr.setStorable(false);
    status = nAttr.setWritable(false);
    status = nAttr.setArray(true);
    status = nAttr.setUsesArrayDataBuilder(true);
    status = addAttribute(mOutTransOpArrayAttr);

    // sampled transform operation angle channels
    mOutTransOpAngleArrayAttr = uAttr.create("outTransOpAngle", "ota",
        MFnUnitAttribute::kAngle, 0.0);
    status = uAttr.setStorable(false);
    status = uAttr.setWritable(false);
    status = uAttr.setKeyable(false);
    status = uAttr.setArray(true);
    status = uAttr.setUsesArrayDataBuilder(true);
    status = addAttribute(mOutTransOpAngleArrayAttr);

    // sampled camera
    // assume the boolean variables cannot be keyed
    mOutCameraArrayAttr = nAttr.create("outCamera", "ocam",
        MFnNumericData::kDouble, 0.0, &status);
    status = nAttr.setStorable(false);
    status = nAttr.setWritable(false);
    status = nAttr.setArray(true);
    status = nAttr.setUsesArrayDataBuilder(true);
    status = addAttribute(mOutCameraArrayAttr);

    // sampled custom-attributes
    mOutPropArrayAttr = gAttr.create("prop", "pr", &status);
    status = gAttr.addNumericDataAccept(MFnNumericData::kByte);
    status = gAttr.addNumericDataAccept(MFnNumericData::kShort);
    status = gAttr.addNumericDataAccept(MFnNumericData::kLong);
    status = gAttr.addNumericDataAccept(MFnNumericData::kInt);
    status = gAttr.addNumericDataAccept(MFnNumericData::kFloat);
    status = gAttr.addNumericDataAccept(MFnNumericData::kDouble);
    status = gAttr.addDataAccept(MFnData::kString);
    status = gAttr.addDataAccept(MFnData::kIntArray);
    status = gAttr.addDataAccept(MFnData::kDoubleArray);

    status = gAttr.setWritable(false);
    status = gAttr.setKeyable(false);
    status = gAttr.setArray(true);
    status = gAttr.setUsesArrayDataBuilder(true);
    status = addAttribute(mOutPropArrayAttr);

    // set up affection relationships
    status = attributeAffects(mTimeAttr, mOutSubDArrayAttr);
    status = attributeAffects(mTimeAttr, mOutPolyArrayAttr);
    status = attributeAffects(mTimeAttr, mOutNurbsSurfaceArrayAttr);
    status = attributeAffects(mTimeAttr, mOutNurbsCurveGrpArrayAttr);
    status = attributeAffects(mTimeAttr, mOutTransOpArrayAttr);
    status = attributeAffects(mTimeAttr, mOutTransOpAngleArrayAttr);
    status = attributeAffects(mTimeAttr, mOutCameraArrayAttr);
    status = attributeAffects(mTimeAttr, mOutPropArrayAttr);

    MFnIntArrayData fnIntArrayData;
    MObject intArrayDefaultObject = fnIntArrayData.create(&status);
    mIsSampledTransOpAngleAttr = tAttr.create("transOpAngle", "ta",
        MFnData::kIntArray, intArrayDefaultObject);
    status = tAttr.setStorable(true);
    status = tAttr.setWritable(true);
    status = addAttribute(mIsSampledTransOpAngleAttr);

    mSampledNurbsCurveNumCurveAttr = tAttr.create("numCurve", "nc",
        MFnData::kIntArray, intArrayDefaultObject);
    status = tAttr.setStorable(true);
    status = tAttr.setWritable(true);
    status = addAttribute(mSampledNurbsCurveNumCurveAttr);

    return status;
}

MStatus AlembicTakoCacheNode::compute(const MPlug & plug, MDataBlock & dataBlock)
{
    MStatus status;

    // this should be done only once per file
    if (mFileInitialized == false)
    {
        mFileInitialized = true;

        MDataHandle dataHandle = dataBlock.inputValue(mTakoFileNameAttr);
        MString fileName = dataHandle.asString();

        TakoSPI::TransformReaderPtr rNode = TakoSPI::v1::ReadScene(fileName.asChar());

        if (!rNode)
        {
            MString theError = "Cannot read file " + fileName;
            printError(theError);
            return MS::kFailure;
        }

        getTakoFrameRange(rNode, mSequenceStartFrame, mSequenceEndFrame,
            mFrameRange);

        dataHandle = dataBlock.inputValue(mLoadUVsAttr, &status);
        mLoadUVs = dataHandle.asBool();
        dataHandle = dataBlock.inputValue(mLoadNormalsAttr, &status);
        mLoadNormals = dataHandle.asBool();

        // connect attributes
        dataHandle = dataBlock.inputValue(mConnectAttr, &status);
        mConnect = dataHandle.asBool();
        dataHandle = dataBlock.inputValue(mCreateIfNotFoundAttr, &status);
        mCreateIfNotFound = dataHandle.asBool();
        dataHandle = dataBlock.inputValue(mRemoveIfNoUpdateAttr, &status);
        mRemoveIfNoUpdate = dataHandle.asBool();
        dataHandle = dataBlock.inputValue(mConnectRootNodesAttr, &status);
        mConnectRootNodes = dataHandle.asString();

        // initialize some flags for plug update
        mSubDInitialized = false;
        mPolyInitialized = false;

        // getting the arrays so there's no need to repeat the tedious work
        dataHandle = dataBlock.inputValue(mIsSampledTransOpAngleAttr, &status);
        if (status == MS::kSuccess)
        {
            MObject intArrayObj = dataHandle.data();
            MFnIntArrayData intArray(intArrayObj, &status);
            if (status == MS::kSuccess)
            {
                unsigned int length = intArray.length();
                mData.mIsSampledTransOpAngle.clear();
                mData.mIsSampledTransOpAngle.reserve(length);
                for (unsigned int i = 0; i < length; i++)
                    mData.mIsSampledTransOpAngle.push_back(intArray[i]);
            }
        }

        dataHandle =
            dataBlock.inputValue(mSampledNurbsCurveNumCurveAttr, &status);
        if (status == MS::kSuccess)
        {
            MObject intArrayObj = dataHandle.data();
            MFnIntArrayData intArray(intArrayObj, &status);
            if (status == MS::kSuccess)
            {
                unsigned int length = intArray.length();
                mData.mNurbsCurveNumCurveList.clear();
                mData.mNurbsCurveNumCurveList.reserve(length);
                for (unsigned int i = 0; i < length; i++)
                    mData.mNurbsCurveNumCurveList.push_back(intArray[i]);
            }
        }

        TakoSPI::ChildNodePtr rootNode = rNode;

        CreateSceneVisitor visitor(mSequenceStartFrame, MObject::kNullObj, 1);
        visitor.setConnectArgs(mConnect, mConnectRootNodes,
            mCreateIfNotFound, mRemoveIfNoUpdate);

        boost::apply_visitor(visitor, rootNode);

        if ( mConnect )
        {
            removeDangleAlembicTakoCacheNodes();
        }

        if (visitor.hasSampledData())
        {
            // information retrieved from the hierarchy traversal
            // and given to AlembicTakoCacheNode to provide update
            visitor.getData(mData);
         }
    }

    // update the frame number to be imported
    MDataHandle timeHandle = dataBlock.inputValue(mTimeAttr, &status);
    MTime t = timeHandle.asTime();
    double inputTime = t.as(MTime::kFilm);

    float frame = getValidCurrentFrame(inputTime, mFrameRange);
    if (frame != mCurFrame)
    {
        mCurFrame = frame;
        mOutRead = std::vector<bool>(mOutRead.size(), false);
    }

    if (plug == mOutPropArrayAttr)
    {
        if (mOutRead[0])
            return MS::kSuccess;

        mOutRead[0] = true;

        unsigned int propSize = mData.mPropNodePtrList.size();

        if (propSize > 0)
        {
            MArrayDataHandle outArrayHandle = dataBlock.outputValue(
                mOutPropArrayAttr, &status);

            MDataHandle outHandle;
            bool isNumeric = false;
            bool isNull = false;
            unsigned int handlePos = 0;

            // for all of the nodes with sampled attributes
            for (unsigned int i = 0; i < propSize; i++)
            {
                UpdatePropVisitor visitor(mCurFrame);
                boost::apply_visitor(visitor, mData.mPropNodePtrList[i]);
                TakoSPI::PropertyMap::const_iterator j = visitor.begin();
                TakoSPI::PropertyMap::const_iterator jEnd = visitor.end();

                ReadPropAttrVisitor propVisitor(mCurFrame);

                // for all of the sampled properties with this node
                for (; j != jEnd; j++)
                {
                    // set the arbitrary attr information per attribute
                    TakoSPI::ArbAttrInfo attrInfo = j->second.second;
                    propVisitor.setArbAttrInfo(attrInfo);

                    int kSize = 1;
                    if (attrInfo.inputType == TakoSPI::ARBATTR_COLOR3
                        || attrInfo.inputType == TakoSPI::ARBATTR_VECTOR3)
                    {
                        kSize = 3;
                    }

                    // there is an assumption here that for every color
                    // attribute, 3 data handles have been added, one for
                    // red, one for green, and one for blue
                    for (int k = 0; k < kSize; k++)
                    {
                        if (handlePos != outArrayHandle.elementIndex())
                        {
                            handlePos++;
                            continue;
                        }

                        handlePos++;
                        outHandle = outArrayHandle.outputValue(&status);
                        if (outHandle.isGeneric(isNumeric, isNull))
                        {
                            propVisitor.setHandle(outHandle);
                            boost::apply_visitor(propVisitor, j->second.first);
                        }
                        outArrayHandle.next();
                    }
                }
            }
            outArrayHandle.setAllClean();
        }
    }
    else if (plug == mOutTransOpArrayAttr || plug == mOutTransOpAngleArrayAttr)
    {
        if (mOutRead[1])
            return MS::kSuccess;

        mOutRead[1] = true;

        unsigned int transopSize = mData.mTransOpNodePtrList.size();

        if (transopSize > 0)
        {
            unsigned int isAngleIndex = 0;

            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutTransOpArrayAttr, &status);

            MPlug arrayPlug(thisMObject(), mOutTransOpArrayAttr);

            MArrayDataHandle outAngleArrayHandle = dataBlock.outputValue(
                mOutTransOpAngleArrayAttr, &status);

            MPlug angleArrayPlug(thisMObject(), mOutTransOpAngleArrayAttr);

            MDataHandle outHandle;

            unsigned int angleHandleIndex = 0;
            unsigned int outHandleIndex = 0;

            for (unsigned int i = 0; i < transopSize; i++)
            {
                std::vector<double> sampleList;
                read(mCurFrame, mData.mTransOpNodePtrList[i], sampleList,
                    mData.mIsComplexTrans[i]);

                unsigned int sampleSize = sampleList.size();

                for (unsigned int j = 0; j < sampleSize; j++)
                {
                    if (mData.mIsSampledTransOpAngle[isAngleIndex++] == true)
                    {
                        // only use the handle if it matches the index.
                        // The index wont line up in the sparse case so we
                        // can just skip that element.
                        if (outAngleArrayHandle.elementIndex() ==
                            angleHandleIndex++)
                        {
                            outHandle = outAngleArrayHandle.outputValue();
                        }
                        else
                            continue;

                        outAngleArrayHandle.next();
                    }
                    else
                    {
                        // only use the handle if it matches the index.
                        // The index wont line up in the sparse case so we
                        // can just skip that element.
                        if (outArrayHandle.elementIndex() == outHandleIndex++)
                        {
                            outHandle = outArrayHandle.outputValue(&status);
                        }
                        else
                            continue;

                        outArrayHandle.next();
                    }
                    outHandle.set(sampleList[j]);
                }
            }
            outArrayHandle.setAllClean();
            outAngleArrayHandle.setAllClean();
        }
    }
    else if (plug == mOutSubDArrayAttr)
    {
        if (mOutRead[2])
            return MS::kSuccess;

        mOutRead[2] = true;

        unsigned int subDSize = mData.mSubDNodePtrList.size();

        if (subDSize > 0)
        {
            MArrayDataHandle outArrayHandle = dataBlock.outputValue(
                mOutSubDArrayAttr, &status);

            MDataHandle outHandle;

            for (unsigned int j = 0; j < subDSize; j++)
            {
                // these elements can be sparse if they have been deleted
                if (outArrayHandle.elementIndex() != j)
                {
                    continue;
                }

                outHandle = outArrayHandle.outputValue(&status);
                outArrayHandle.next();

                MObject obj = outHandle.data();
                if (obj.hasFn(MFn::kMesh))
                {
                    MFnMesh mesh(obj);
                    TakoSPI::SubDReaderPtr subd = mData.mSubDNodePtrList[j];
                    unsigned int readVal = subd->read(mCurFrame);

                    // since we dont yet support creases, corners, or holes
                    // we'll just check the geometry state
                    bool topologyChange = ((readVal & 0x0f) ==
                        TakoSPI::SubDReader::GEOMETRY_HETEROGENOUS);

                    std::string meshName("");

                    if (mSubDInitialized == false || topologyChange)
                        meshName = subd->getName();

                    readSubD(mesh, obj, meshName, subd, mLoadUVs, mLoadNormals);
                    outHandle.set(obj);
                }
            }
            outArrayHandle.setAllClean();
            mSubDInitialized = true;
        }
        // for the case where we don't have any nodes, we want to make sure
        // to push out empty meshes on our connections, this can happen if
        // the input file was offlined, currently we only need to do this for
        // meshes as Nurbs, curves, and the other channels don't crash Maya
        else
        {
            MArrayDataHandle outArrayHandle = dataBlock.outputValue(
                mOutSubDArrayAttr, &status);

            if (outArrayHandle.elementCount() > 0)
            {
                do
                {
                    MDataHandle outHandle = outArrayHandle.outputValue();
                    MObject obj = outHandle.data();
                    if (obj.hasFn(MFn::kMesh))
                    {
                        MFloatPointArray emptyVerts;
                        MIntArray emptyCounts;
                        MIntArray emptyConnects;
                        MFnMesh emptyMesh;
                        emptyMesh.create(0, 0, emptyVerts, emptyCounts,
                            emptyConnects, obj);
                        outHandle.set(obj);
                    }
                }
                while (outArrayHandle.next() == MS::kSuccess);
            }
            outArrayHandle.setAllClean();
            mSubDInitialized = true;
        }
    }
    else if (plug == mOutPolyArrayAttr)
    {
        if (mOutRead[3])
            return MS::kSuccess;

        mOutRead[3] = true;

        unsigned int polySize = mData.mPolyNodePtrList.size();

        if (polySize > 0)
        {
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutPolyArrayAttr, &status);

            MDataHandle outHandle;

            for (unsigned int j = 0; j < polySize; j++)
            {
                // these elements can be sparse if they have been deleted
                if (outArrayHandle.elementIndex() != j)
                {
                    continue;
                }

                outHandle = outArrayHandle.outputValue(&status);
                outArrayHandle.next();

                MObject obj = outHandle.data();
                if (obj.hasFn(MFn::kMesh))
                {
                    MFnMesh fnMesh(obj);
                    TakoSPI::PolyMeshReaderPtr poly = mData.mPolyNodePtrList[j];
                    unsigned int ttype = poly->read(mCurFrame);

                    std::string meshName("");

                    if (mPolyInitialized == false ||
                        ttype == TakoSPI::PolyMeshReader::TOPOLOGY_HETEROGENOUS)
                    {
                        meshName = poly->getName();
                    }

                    readPoly(fnMesh, obj, meshName.c_str(), poly,
                        mLoadUVs, mLoadNormals);
                    outHandle.set(obj);
                }
            }
            outArrayHandle.setAllClean();
            mPolyInitialized = true;
        }
        // for the case where we don't have any nodes, we want to make sure
        // to push out empty meshes on our connections, this can happen if
        // the input file was offlined, currently we only need to do this for
        // meshes as Nurbs, curves, and the other channels don't crash Maya
        else
        {
            MArrayDataHandle outArrayHandle = dataBlock.outputValue(
                mOutPolyArrayAttr, &status);

            if (outArrayHandle.elementCount() > 0)
            {
                do
                {
                    MDataHandle outHandle = outArrayHandle.outputValue(&status);
                    MObject obj = outHandle.data();
                    if (obj.hasFn(MFn::kMesh))
                    {
                        MFloatPointArray emptyVerts;
                        MIntArray emptyCounts;
                        MIntArray emptyConnects;
                        MFnMesh emptyMesh;
                        emptyMesh.create(0, 0, emptyVerts, emptyCounts,
                            emptyConnects, obj);
                        outHandle.set(obj);
                    }
                }
                while (outArrayHandle.next() == MS::kSuccess);
            }
            outArrayHandle.setAllClean();
            mPolyInitialized = true;
        }
    }
    else if (plug == mOutCameraArrayAttr)
    {
        if (mOutRead[4])
            return MS::kSuccess;

        mOutRead[4] = true;

        unsigned int cameraSize = mData.mCameraNodePtrList.size();

        if (cameraSize > 0)
        {
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutCameraArrayAttr, &status);

            MPlug arrayPlug(thisMObject(), mOutCameraArrayAttr);
            double angleConversion = 1.0;

            switch (MAngle::uiUnit())
            {
                case MAngle::kDegrees:
                    angleConversion = 57.295779513082323;
                break;
                case MAngle::kAngMinutes:
                    angleConversion = 3437.7467707849391;
                break;
                case MAngle::kAngSeconds:
                    angleConversion = 206264.80624709636;
                break;
                default:
                break;
            }

            MDataHandle outHandle;
            unsigned int index = 0;

            for (unsigned int cameraIndex = 0; cameraIndex < cameraSize;
                cameraIndex++)
            {
                TakoSPI::CameraReaderPtr camPtr =
                    mData.mCameraNodePtrList[cameraIndex];
                std::vector<double> array;
                read(mCurFrame, camPtr, array);

                for (unsigned int dataIndex = 0; dataIndex < array.size();
                    dataIndex++, index++)
                {
                    // skip over sparse elements
                    if (index != outArrayHandle.elementIndex())
                    {
                        continue;
                    }

                    outHandle = outArrayHandle.outputValue(&status);
                    outArrayHandle.next();

                    // not shutter angle index, so not an angle
                    if (dataIndex != 22)
                    {
                        outHandle.set(array[dataIndex]);
                    }
                    else
                    {
                        outHandle.set(array[dataIndex] * angleConversion);
                    }
                }  // for the per camera data handles
            }  // for each camera
            outArrayHandle.setAllClean();
        }
    }
    else if (plug == mOutNurbsSurfaceArrayAttr)
    {
        if (mOutRead[5])
            return MS::kSuccess;

        mOutRead[5] = true;

        unsigned int nSurfaceSize =
            mData.mNurbsSurfaceNodePtrList.size();

        if (nSurfaceSize > 0)
        {
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutNurbsSurfaceArrayAttr, &status);

            MDataHandle outHandle;

            for (unsigned int j = 0; j < nSurfaceSize; j++)
            {
                // these elements can be sparse if they have been deleted
                if (outArrayHandle.elementIndex() != j)
                    continue;

                outHandle = outArrayHandle.outputValue(&status);
                outArrayHandle.next();

                MObject obj = outHandle.data();
                if (obj.hasFn(MFn::kNurbsSurface))
                {
                     TakoSPI::NurbsSurfaceReaderPtr nSPtr
                        = mData.mNurbsSurfaceNodePtrList[j];
                    read(mCurFrame, nSPtr, obj);
                    status = outHandle.set(obj);
                }
            }
            outArrayHandle.setAllClean();
        }
    }
    else if (plug == mOutNurbsCurveGrpArrayAttr)
    {
        if (mOutRead[6])
            return MS::kSuccess;

        mOutRead[6] = true;

        unsigned int nCurveGrpSize = mData.mNurbsCurveNodePtrList.size();

        if (nCurveGrpSize > 0)
        {
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutNurbsCurveGrpArrayAttr, &status);
            MDataHandle outHandle;

            unsigned int logicalIndex = 0;
            for (unsigned int curveGrpIndex = 0; curveGrpIndex < nCurveGrpSize;
                curveGrpIndex++)
            {
                std::vector<MObject> ncObj;
                unsigned int numChild =
                    mData.mNurbsCurveNumCurveList[curveGrpIndex];

                ncObj.reserve(numChild);

                for (unsigned int i = 0; i < numChild; i++)
                {
                    MFnNurbsCurveData curveData;
                    ncObj.push_back(curveData.create());
                }
                read(mCurFrame,
                        mData.mNurbsCurveNodePtrList[curveGrpIndex], ncObj);

                for (unsigned int i = 0; i < numChild; i++, logicalIndex++)
                {
                    if (outArrayHandle.elementIndex() != logicalIndex)
                    {
                        continue;
                    }

                    outHandle = outArrayHandle.outputValue(&status);
                    outArrayHandle.next();
                    status = outHandle.set(ncObj[i]);
                }
            }
            outArrayHandle.setAllClean();
        }
    }
    else
    {
        return MS::kUnknownParameter;
    }

    return status;
}

// make the far clipping plane passive, as packages like C4D don't have a
// far clipping plane and when it doesn't exist it isn't easy to guess what
// the correct value should be
bool AlembicTakoCacheNode::isPassiveOutput(const MPlug & plug) const
{
    MStatus status = MS::kSuccess;
    MPlug arrayPlug(thisMObject(), mOutCameraArrayAttr);
    unsigned int farClipPlaneIndex = 11;
    MPlug farClipPlanePlug =
        arrayPlug.elementByPhysicalIndex(farClipPlaneIndex, &status);

    if (status == MS::kSuccess && plug == farClipPlanePlug)
        return true;

    return MPxNode::isPassiveOutput( plug );
}

} // End namespace Alembic

