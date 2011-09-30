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

#include "util.h"
#include "AlembicNode.h"
#include "CreateSceneHelper.h"
#include "CameraHelper.h"
#include "LocatorHelper.h"
#include "MeshHelper.h"
#include "NurbsCurveHelper.h"
#include "NurbsSurfaceHelper.h"
#include "PointHelper.h"
#include "XformHelper.h"

#include <maya/MAngle.h>
#include <maya/MGlobal.h>
#include <maya/MTime.h>

#include <maya/MArrayDataHandle.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnVectorArrayData.h>

#include <maya/MFnStringData.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MFnNurbsSurfaceData.h>

#include <maya/MFnGenericAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>

#include <Alembic/AbcCoreHDF5/ReadWrite.h>
#include <Alembic/AbcGeom/Visibility.h>

MObject AlembicNode::mTimeAttr;
MObject AlembicNode::mAbcFileNameAttr;

MObject AlembicNode::mStartFrameAttr;
MObject AlembicNode::mEndFrameAttr;

MObject AlembicNode::mOutSubDArrayAttr;
MObject AlembicNode::mOutPolyArrayAttr;
MObject AlembicNode::mOutCameraArrayAttr;
MObject AlembicNode::mOutNurbsCurveGrpArrayAttr;
MObject AlembicNode::mOutNurbsSurfaceArrayAttr;
MObject AlembicNode::mOutTransOpArrayAttr;
MObject AlembicNode::mOutPropArrayAttr;
MObject AlembicNode::mOutLocatorPosScaleArrayAttr;

MStatus AlembicNode::initialize()
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
    mAbcFileNameAttr = tAttr.create("abc_File", "fn",
        MFnData::kString, fileNameDefaultObject);
    status = tAttr.setStorable(true);
    status = tAttr.setUsedAsFilename(true);
    status = addAttribute(mAbcFileNameAttr);

    // sequence min and max in frames
    mStartFrameAttr = nAttr.create("startFrame", "sf",
        MFnNumericData::kDouble, 0, &status);
    status = nAttr.setWritable(false);
    status = nAttr.setStorable(true);
    status = addAttribute(mStartFrameAttr);

    mEndFrameAttr = nAttr.create("endFrame", "ef",
        MFnNumericData::kDouble, 0, &status);
    status = nAttr.setWritable(false);
    status = nAttr.setStorable(true);
    status = addAttribute(mEndFrameAttr);

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

    // sampled locator
    mOutLocatorPosScaleArrayAttr = nAttr.create("outLoc", "olo",
        MFnNumericData::kDouble, 0.0, &status);
    status = nAttr.setStorable(false);
    status = nAttr.setWritable(false);
    status = nAttr.setArray(true);
    status = nAttr.setUsesArrayDataBuilder(true);
    status = addAttribute(mOutLocatorPosScaleArrayAttr);

    // sampled transform operations
    mOutTransOpArrayAttr = nAttr.create("transOp", "to",
        MFnNumericData::kDouble, 0.0, &status);
    status = nAttr.setStorable(false);
    status = nAttr.setWritable(false);
    status = nAttr.setArray(true);
    status = nAttr.setUsesArrayDataBuilder(true);
    status = addAttribute(mOutTransOpArrayAttr);

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
    status = gAttr.addNumericDataAccept(MFnNumericData::kBoolean);
    status = gAttr.addNumericDataAccept(MFnNumericData::kByte);
    status = gAttr.addNumericDataAccept(MFnNumericData::kShort);
    status = gAttr.addNumericDataAccept(MFnNumericData::k2Short);
    status = gAttr.addNumericDataAccept(MFnNumericData::k3Short);
    status = gAttr.addNumericDataAccept(MFnNumericData::kInt);
    status = gAttr.addNumericDataAccept(MFnNumericData::k2Int);
    status = gAttr.addNumericDataAccept(MFnNumericData::k3Int);
    status = gAttr.addNumericDataAccept(MFnNumericData::kFloat);
    status = gAttr.addNumericDataAccept(MFnNumericData::k2Float);
    status = gAttr.addNumericDataAccept(MFnNumericData::k3Float);
    status = gAttr.addNumericDataAccept(MFnNumericData::kDouble);
    status = gAttr.addNumericDataAccept(MFnNumericData::k2Double);
    status = gAttr.addNumericDataAccept(MFnNumericData::k3Double);
    status = gAttr.addNumericDataAccept(MFnNumericData::k4Double);
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
    status = attributeAffects(mTimeAttr, mOutCameraArrayAttr);
    status = attributeAffects(mTimeAttr, mOutPropArrayAttr);
    status = attributeAffects(mTimeAttr, mOutLocatorPosScaleArrayAttr);

    return status;
}

MStatus AlembicNode::compute(const MPlug & plug, MDataBlock & dataBlock)
{
    MStatus status;

    // update the frame number to be imported
    MDataHandle timeHandle = dataBlock.inputValue(mTimeAttr, &status);
    MTime t = timeHandle.asTime();
    double inputTime = t.as(MTime::kSeconds);

    // this should be done only once per file
    if (mFileInitialized == false)
    {
        mFileInitialized = true;

        MDataHandle dataHandle = dataBlock.inputValue(mAbcFileNameAttr);
        MString fileName = dataHandle.asString();

        // no caching!
        Alembic::Abc::IArchive archive(Alembic::AbcCoreHDF5::ReadArchive(),
            fileName.asChar(), Alembic::Abc::ErrorHandler::Policy(),
            Alembic::AbcCoreAbstract::ReadArraySampleCachePtr());

        if (!archive.valid())
        {
            MString theError = "Cannot read file " + fileName;
            printError(theError);
        }

        // initialize some flags for plug update
        mSubDInitialized = false;
        mPolyInitialized = false;

        MFnDependencyNode dep(thisMObject());
        MPlug allSetsPlug = dep.findPlug("allColorSets");
        CreateSceneVisitor visitor(inputTime, !allSetsPlug.isNull(),
            MObject::kNullObj, CreateSceneVisitor::NONE, "");

        visitor.walk(archive);

        if (visitor.hasSampledData())
        {
            // information retrieved from the hierarchy traversal
            // and given to AlembicNode to provide update
            visitor.getData(mData);
            mData.getFrameRange(mSequenceStartTime, mSequenceEndTime);
        }
    }

    clamp<double>(mSequenceStartTime, mSequenceEndTime, inputTime);

    // update only when the time lapse is big enough
    if (fabs(inputTime - mCurTime) > 0.00001)
    {
        mOutRead = std::vector<bool>(mOutRead.size(), false);
        mCurTime = inputTime;
    }

    if (plug == mOutPropArrayAttr)
    {

        if (mOutRead[0])
        {
            dataBlock.setClean(plug);
            return MS::kSuccess;
        }

        mOutRead[0] = true;

        unsigned int propSize =
            static_cast<unsigned int>(mData.mPropList.size());

        if (propSize > 0)
        {
            MArrayDataHandle outArrayHandle = dataBlock.outputValue(
                mOutPropArrayAttr, &status);

            unsigned int outHandleIndex = 0;
            MDataHandle outHandle;

            // for all of the nodes with sampled attributes
            for (unsigned int i = 0; i < propSize; i++)
            {
                // only use the handle if it matches the index.
                // The index wont line up in the sparse case so we
                // can just skip that element.
                if (outArrayHandle.elementIndex() == outHandleIndex++)
                {
                    outHandle = outArrayHandle.outputValue();
                }
                else
                {
                    continue;
                }

                if (mData.mPropList[i].mArray.valid())
                {
                    readProp(mCurTime, mData.mPropList[i].mArray, outHandle);
                }
                // meant for special properties (like visible)
                else
                {
                    if (mData.mPropList[i].mScalar.getName() ==
                        Alembic::AbcGeom::kVisibilityPropertyName)
                    {
                        Alembic::Util::int8_t visVal = 1;
                        mData.mPropList[i].mScalar.get(&visVal,
                            Alembic::Abc::ISampleSelector(mCurTime,
                                Alembic::Abc::ISampleSelector::kNearIndex ));
                        outHandle.setGenericBool(visVal != 0, false);
                    }
                }
                outArrayHandle.next();
            }
            outArrayHandle.setAllClean();
        }

    }
    else if (plug == mOutTransOpArrayAttr )
    {
        if (mOutRead[1])
        {
            dataBlock.setClean(plug);
            return MS::kSuccess;
        }

        mOutRead[1] = true;

        unsigned int xformSize =
            static_cast<unsigned int>(mData.mXformList.size());

        if (xformSize > 0)
        {
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutTransOpArrayAttr, &status);

            MPlug arrayPlug(thisMObject(), mOutTransOpArrayAttr);

            MDataHandle outHandle;
            unsigned int outHandleIndex = 0;

            for (unsigned int i = 0; i < xformSize; i++)
            {
                std::vector<double> sampleList;

                if (mData.mIsComplexXform[i])
                {
                    readComplex(mCurTime, mData.mXformList[i], sampleList);
                }
                else
                {
                    Alembic::AbcGeom::XformSample samp;
                    read(mCurTime, mData.mXformList[i], sampleList, samp);
                }

                unsigned int sampleSize = (unsigned int)sampleList.size();

                for (unsigned int j = 0; j < sampleSize; j++)
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
                    outHandle.set(sampleList[j]);
                }
            }
            outArrayHandle.setAllClean();
        }
    }
    else if (plug == mOutLocatorPosScaleArrayAttr )
    {
        if (mOutRead[8])
        {
            dataBlock.setClean(plug);
            return MS::kSuccess;
        }

        mOutRead[8] = true;

        unsigned int locSize =
            static_cast<unsigned int>(mData.mLocList.size());

        if (locSize > 0)
        {
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutLocatorPosScaleArrayAttr, &status);

            MPlug arrayPlug(thisMObject(), mOutLocatorPosScaleArrayAttr);

            MDataHandle outHandle;
            unsigned int outHandleIndex = 0;

            for (unsigned int i = 0; i < locSize; i++)
            {
                std::vector< double > sampleList;
                read(mCurTime, mData.mLocList[i], sampleList);

                unsigned int sampleSize = (unsigned int)sampleList.size();
                for (unsigned int j = 0; j < sampleSize; j++)
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
                    outHandle.set(sampleList[j]);
                }
            }
            outArrayHandle.setAllClean();
        }
    }
    else if (plug == mOutSubDArrayAttr)
    {
        if (mOutRead[2])
        {
            dataBlock.setClean(plug);
            return MS::kSuccess;
        }

        mOutRead[2] = true;

        unsigned int subDSize =
            static_cast<unsigned int>(mData.mSubDList.size());

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
                    MFnMesh fnMesh(obj);
                    readSubD(mCurTime, fnMesh, obj, mData.mSubDList[j],
                        mSubDInitialized);
                    outHandle.set(obj);
                }
            }
            mSubDInitialized = true;
            outArrayHandle.setAllClean();
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
            mSubDInitialized = true;
            outArrayHandle.setAllClean();
        }
    }
    else if (plug == mOutPolyArrayAttr)
    {
        if (mOutRead[3])
        {
            dataBlock.setClean(plug);
            return MS::kSuccess;
        }

        mOutRead[3] = true;

        unsigned int polySize =
            static_cast<unsigned int>(mData.mPolyMeshList.size());

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
                    readPoly(mCurTime, fnMesh, obj, mData.mPolyMeshList[j],
                        mPolyInitialized);
                    outHandle.set(obj);
                }
            }
            mPolyInitialized = true;
            outArrayHandle.setAllClean();
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
            mPolyInitialized = true;
            outArrayHandle.setAllClean();
        }
    }
    else if (plug == mOutCameraArrayAttr)
    {
        if (mOutRead[4])
        {
            dataBlock.setClean(plug);
            return MS::kSuccess;
        }

        mOutRead[4] = true;

        unsigned int cameraSize =
            static_cast<unsigned int>(mData.mCameraList.size());

        if (cameraSize > 0)
        {
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutCameraArrayAttr, &status);
            MPlug arrayPlug(thisMObject(), mOutCameraArrayAttr);
            double angleConversion = 1.0;

            switch (MAngle::uiUnit())
            {
                case MAngle::kRadians:
                    angleConversion = 0.017453292519943295;
                break;
                case MAngle::kAngMinutes:
                    angleConversion = 60.0;
                break;
                case MAngle::kAngSeconds:
                    angleConversion = 3600.0;
                break;
                default:
                break;
            }

            MDataHandle outHandle;
            unsigned int index = 0;

            for (unsigned int cameraIndex = 0; cameraIndex < cameraSize;
                cameraIndex++)
            {
                Alembic::AbcGeom::ICamera & cam =
                    mData.mCameraList[cameraIndex];
                std::vector<double> array;

                read(mCurTime, cam, array);

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
                    if (dataIndex != 11)
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
        {
            dataBlock.setClean(plug);
            return MS::kSuccess;
        }

        mOutRead[5] = true;

        unsigned int nSurfaceSize =
            static_cast<unsigned int>(mData.mNurbsList.size());

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
                    readNurbs(mCurTime, mData.mNurbsList[j], obj);
                    outHandle.set(obj);
                }
            }
            outArrayHandle.setAllClean();
        }
    }
    else if (plug == mOutNurbsCurveGrpArrayAttr)
    {
        if (mOutRead[6])
        {
            dataBlock.setClean(plug);
            return MS::kSuccess;
        }

        mOutRead[6] = true;

        unsigned int nCurveGrpSize =
            static_cast<unsigned int>(mData.mCurvesList.size());

        if (nCurveGrpSize > 0)
        {
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutNurbsCurveGrpArrayAttr, &status);
            MDataHandle outHandle;

            std::vector<MObject> curvesObj;
            for (unsigned int i = 0; i < nCurveGrpSize; ++i)
            {
                readCurves(mCurTime, mData.mCurvesList[i],
                    mData.mNumCurves[i], curvesObj);
            }

            std::size_t numChild = curvesObj.size();

            // not the best way to do this
            // only reading bunches of curves based on the connections would be
            // more efficient when there is a bunch of broken connections
            for (unsigned int i = 0; i < numChild; i++)
            {
                if (outArrayHandle.elementIndex() != i)
                {
                    continue;
                }

                outHandle = outArrayHandle.outputValue(&status);
                outArrayHandle.next();
                status = outHandle.set(curvesObj[i]);
            }

            outArrayHandle.setAllClean();
        }
    }
    else
    {
        return MS::kUnknownParameter;
    }

    dataBlock.setClean(plug);
    return status;
}

// make the far clipping plane passive, as packages like C4D don't have a
// far clipping plane and when it doesn't exist it isn't easy to guess what
// the correct value should be
bool AlembicNode::isPassiveOutput(const MPlug & plug) const
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
