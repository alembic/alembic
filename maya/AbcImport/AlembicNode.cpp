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
#include <maya/MFileObject.h>

#include <maya/MArrayDataHandle.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnStringArrayData.h>

#include <maya/MFnStringData.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MFnNurbsSurfaceData.h>

#include <maya/MFnGenericAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnEnumAttribute.h>
#if defined(MAYA_WANT_EXTERNALCONTENTTABLE)
#include <maya/MExternalContentInfoTable.h>
#endif

#include <Alembic/AbcCoreFactory/IFactory.h>
#include <Alembic/AbcCoreHDF5/ReadWrite.h>
#include <Alembic/AbcCoreOgawa/ReadWrite.h>
#include <Alembic/AbcGeom/Visibility.h>

MObject AlembicNode::mTimeAttr;
MObject AlembicNode::mAbcFileNameAttr;
MObject AlembicNode::mAbcLayerFileNamesAttr;

MObject AlembicNode::mSpeedAttr;
MObject AlembicNode::mOffsetAttr;
MObject AlembicNode::mCycleTypeAttr;
MObject AlembicNode::mStartFrameAttr;
MObject AlembicNode::mEndFrameAttr;
MObject AlembicNode::mIncludeFilterAttr;
MObject AlembicNode::mExcludeFilterAttr;

MObject AlembicNode::mOutSubDArrayAttr;
MObject AlembicNode::mOutPolyArrayAttr;
MObject AlembicNode::mOutCameraArrayAttr;
MObject AlembicNode::mOutNurbsCurveGrpArrayAttr;
MObject AlembicNode::mOutNurbsSurfaceArrayAttr;
MObject AlembicNode::mOutTransOpArrayAttr;
MObject AlembicNode::mOutPropArrayAttr;
MObject AlembicNode::mOutLocatorPosScaleArrayAttr;

namespace
{
    MString UITemplateMELScriptStr(
"global proc AEAlembicNodeTemplate( string $nodeName )\n"
"{\n"
"editorTemplate -beginScrollLayout;\n"
"editorTemplate -beginLayout \"Alembic Attributes\" -collapse 0;\n"
"editorTemplate -addControl \"abc_File\";\n"
"editorTemplate -addControl \"abc_layerFiles\";\n"
"editorTemplate -addControl \"startFrame\";\n"
"editorTemplate -addControl \"endFrame\";\n"
"editorTemplate -addControl \"time\";\n"
"editorTemplate -addControl \"speed\";\n"
"editorTemplate -addControl \"offset\";\n"
"editorTemplate -addControl \"cycleType\";\n"
"editorTemplate -endLayout;\n"
"AEdependNodeTemplate $nodeName;\n"
"editorTemplate -addExtraControls;\n"
"editorTemplate -endScrollLayout;\n}"
    );
};

MStatus AlembicNode::initialize()
{
    MStatus status;

    MFnUnitAttribute    uAttr;
    MFnTypedAttribute   tAttr, tAttr2;
    MFnNumericAttribute nAttr;
    MFnGenericAttribute gAttr;
    MFnEnumAttribute    eAttr;

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

    // input layer file names
    MFnStringArrayData fileFnStringArrayData;
    MStringArray dummyStringArray;
    MObject layerFileNamesDefaultObject = fileFnStringArrayData.create(dummyStringArray);
    mAbcLayerFileNamesAttr = tAttr2.create("abc_layerFiles", "fns",
        MFnData::kStringArray, layerFileNamesDefaultObject);
    status = tAttr2.setStorable(true);
    status = tAttr2.setUsedAsFilename(true);
    status = tAttr2.setKeyable(false);
    status = tAttr2.setWritable(true);
    status = addAttribute(mAbcLayerFileNamesAttr);

    // playback speed
    mSpeedAttr = nAttr.create("speed", "sp",
        MFnNumericData::kDouble, 1.0, &status);
    status = nAttr.setWritable(true);
    status = nAttr.setStorable(true);
    status = nAttr.setKeyable(true);
    status = addAttribute(mSpeedAttr);

    // frame offset
    mOffsetAttr = nAttr.create("offset", "of",
        MFnNumericData::kDouble, 0, &status);
    status = nAttr.setWritable(true);
    status = nAttr.setStorable(true);
    status = nAttr.setKeyable(true);
    status = addAttribute(mOffsetAttr);

    // cycle type
    mCycleTypeAttr = eAttr.create("cycleType", "ct", 0,  &status );
    status = eAttr.addField("Hold", PLAYTYPE_HOLD);
    status = eAttr.addField("Loop", PLAYTYPE_LOOP);
    status = eAttr.addField("Reverse", PLAYTYPE_REVERSE);
    status = eAttr.addField("Bounce", PLAYTYPE_BOUNCE);
    status = eAttr.setWritable(true);
    status = eAttr.setStorable(true);
    status = eAttr.setKeyable(true);
    status = addAttribute(mCycleTypeAttr);

    // Regex Filter
    // This is a hidden variable to preserve a regexIncludefilter string
    // into a .ma file.
    mIncludeFilterAttr = tAttr.create("regexIncludeFilter", "ift",
        MFnData::kString);
    status = tAttr.setStorable(true);
    status = tAttr.setHidden(true);
    status = addAttribute(mIncludeFilterAttr);

    // Regex Filter
    // This is a hidden variable to preserve a regexExcludefilter string
    // into a .ma file.
    mExcludeFilterAttr = tAttr.create("regexExcludeFilter", "eft",
        MFnData::kString);
    status = tAttr.setStorable(true);
    status = tAttr.setHidden(true);
    status = addAttribute(mExcludeFilterAttr);

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
    status = gAttr.addDataAccept(MFnData::kVectorArray);
    status = gAttr.addDataAccept(MFnData::kPointArray);

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

    status = attributeAffects(mSpeedAttr, mOutSubDArrayAttr);
    status = attributeAffects(mSpeedAttr, mOutPolyArrayAttr);
    status = attributeAffects(mSpeedAttr, mOutNurbsSurfaceArrayAttr);
    status = attributeAffects(mSpeedAttr, mOutNurbsCurveGrpArrayAttr);
    status = attributeAffects(mSpeedAttr, mOutTransOpArrayAttr);
    status = attributeAffects(mSpeedAttr, mOutCameraArrayAttr);
    status = attributeAffects(mSpeedAttr, mOutPropArrayAttr);
    status = attributeAffects(mSpeedAttr, mOutLocatorPosScaleArrayAttr);

    status = attributeAffects(mOffsetAttr, mOutSubDArrayAttr);
    status = attributeAffects(mOffsetAttr, mOutPolyArrayAttr);
    status = attributeAffects(mOffsetAttr, mOutNurbsSurfaceArrayAttr);
    status = attributeAffects(mOffsetAttr, mOutNurbsCurveGrpArrayAttr);
    status = attributeAffects(mOffsetAttr, mOutTransOpArrayAttr);
    status = attributeAffects(mOffsetAttr, mOutCameraArrayAttr);
    status = attributeAffects(mOffsetAttr, mOutPropArrayAttr);
    status = attributeAffects(mOffsetAttr, mOutLocatorPosScaleArrayAttr);

    status = attributeAffects(mCycleTypeAttr, mOutSubDArrayAttr);
    status = attributeAffects(mCycleTypeAttr, mOutPolyArrayAttr);
    status = attributeAffects(mCycleTypeAttr, mOutNurbsSurfaceArrayAttr);
    status = attributeAffects(mCycleTypeAttr, mOutNurbsCurveGrpArrayAttr);
    status = attributeAffects(mCycleTypeAttr, mOutTransOpArrayAttr);
    status = attributeAffects(mCycleTypeAttr, mOutCameraArrayAttr);
    status = attributeAffects(mCycleTypeAttr, mOutPropArrayAttr);
    status = attributeAffects(mCycleTypeAttr, mOutLocatorPosScaleArrayAttr);

    MGlobal::executeCommand( UITemplateMELScriptStr );

    return status;
}

double AlembicNode::getFPS()
{
    float fps = 24.0f;
    MTime::Unit unit = MTime::uiUnit();
    if (unit!=MTime::kInvalid)
    {
        MTime time(1.0, MTime::kSeconds);
        fps = static_cast<float>(time.as(unit));
    }

    if (fps <= 0.f )
    {
        fps = 24.0f;
    }

    return fps;
}

double AlembicNode::computeAdjustedTime(const double inputTime,
                                        const double speed,
                                        const double timeOffset)
{
   return ( inputTime - timeOffset ) * speed;
}

double AlembicNode::computeRetime(const double inputTime,
                                  const double firstTime,
                                  const double lastTime,
                                  const short playStyle)
{
    const double playTime = lastTime - firstTime;
    static const double eps = 0.001;
    double retime = inputTime;

    switch (playStyle)
    {
      case PLAYTYPE_HOLD:
          break;
      case PLAYTYPE_LOOP:
          if (inputTime < (firstTime - eps) || inputTime > (lastTime + eps))
          {
              const double timeOffset = inputTime - firstTime;
              const double playOffset = floor(timeOffset/playTime);
              const double fraction = fabs(timeOffset/playTime - playOffset);

              retime = firstTime + playTime * fraction;
          }
          break;
      case PLAYTYPE_REVERSE:
          if (inputTime > (firstTime + eps) && inputTime < (lastTime - eps))
          {
              const double timeOffset = inputTime - firstTime;
              const double playOffset = floor(timeOffset/playTime);
              const double fraction = fabs(timeOffset/playTime - playOffset);

              retime = lastTime - playTime * fraction;
          }
          else if (inputTime < (firstTime + eps))
          {
              retime = lastTime;
          }
          else
          {
              retime = firstTime;
          }
          break;
      case PLAYTYPE_BOUNCE:
          if (inputTime < (firstTime - eps) || inputTime > (lastTime + eps))
          {
              const double timeOffset = inputTime - firstTime;
              const double playOffset = floor(timeOffset/playTime);
              const double fraction = fabs(timeOffset/playTime - playOffset);

              // forward loop
              if (fmod(playOffset, 2.0)==0.0)
              {
                  retime = firstTime + playTime * fraction;
              }
              // backward loop
              else
              {
                  retime = lastTime - playTime * fraction;
              }
          }
          break;
    }

    return retime;
}

MStatus AlembicNode::setDependentsDirty(const MPlug& plug, MPlugArray& plugArray)
{
    if (plug == mAbcFileNameAttr)
    {
/*  This code was to force refresh of the AlembicNode when there is a file name change
    But since it was only working in particular very simple case, we decided to not enable it
    and only display a warning.
    In all other cases it could result in undesired behavior even scene corruption.
    See issue MAYA-47471
        mFileInitialized = false;
        mCurTime = DBL_MAX; // to force update
*/
        if(mFileInitialized)
        {
            MGlobal::displayWarning("Repathing Alembic Nodes is not supported");
        }
    }
    return MPxNode::setDependentsDirty(plug, plugArray);
}

MStatus AlembicNode::compute(const MPlug & plug, MDataBlock & dataBlock)
{
    MStatus status;

    // update the frame number to be imported
    MDataHandle speedHandle = dataBlock.inputValue(mSpeedAttr, &status);
    double speed = speedHandle.asDouble();

    MDataHandle offsetHandle = dataBlock.inputValue(mOffsetAttr, &status);
    double offset = offsetHandle.asDouble();

    MDataHandle timeHandle = dataBlock.inputValue(mTimeAttr, &status);
    MTime t = timeHandle.asTime();
    double inputTime = t.as(MTime::kSeconds);

    double fps = getFPS();

    // scale and offset inputTime.
    inputTime = computeAdjustedTime(inputTime, speed, offset/fps);

    // this should be done only once per file
    if (mFileInitialized == false)
    {
        mFileInitialized = true;

        //Get list of input filenames
        MFnDependencyNode depNode(thisMObject());
        MPlug layerFilesPlug = depNode.findPlug("abc_layerFiles");
        MFnStringArrayData fnSAD( layerFilesPlug.asMObject() );
        MStringArray storedFilenames = fnSAD.array();

        //Legacy support for single-filename input
        if( storedFilenames.length() == 0 )
        {
            MFileObject fileObject;
            MDataHandle dataHandle = dataBlock.inputValue(mAbcFileNameAttr);
            fileObject.setRawFullName(dataHandle.asString());
            MString fileName = fileObject.resolvedFullName();
            storedFilenames.append( fileName );
        }

        std::vector<std::string> abcFilenames;
        for(unsigned int i = 0; i < storedFilenames.length(); i++)
        {
            abcFilenames.push_back( storedFilenames[i].asChar() );
        }

        Alembic::Abc::IArchive archive;
        Alembic::AbcCoreFactory::IFactory factory;
        factory.setPolicy(Alembic::Abc::ErrorHandler::kQuietNoopPolicy);

        archive = factory.getArchive( abcFilenames );

        if (!archive.valid())
        {
            MString theError = "Error opening these alembic files: ";

            const unsigned int numFilenames = storedFilenames.length();
            for( unsigned int i = 0; i < numFilenames; i++ )
            {
                theError += storedFilenames[ i ];

                if( i != (numFilenames - 1) )
                {
                    theError += ", ";
                }
            }

            printError(theError);
        }

        // initialize some flags for plug update
        mSubDInitialized = false;
        mPolyInitialized = false;

        // When an alembic cache will be imported at the first time using
        // AbcImport, we need to set mIncludeFilterAttr (filterHandle) to be
        // mIncludeFilterString for later use. When we save a maya scene(.ma)
        // mIncludeFilterAttr will be saved. Then when we load the saved
        // .ma file, mIncludeFilterString will be set to be mIncludeFilterAttr.
        MDataHandle includeFilterHandle =
            dataBlock.inputValue(mIncludeFilterAttr, &status);
        MString& includeFilterString = includeFilterHandle.asString();

        if (mIncludeFilterString.length() > 0)
        {
            includeFilterHandle.set(mIncludeFilterString);
            dataBlock.setClean(mIncludeFilterAttr);
        }
        else if (includeFilterString.length() > 0)
        {
            mIncludeFilterString = includeFilterString;
        }

        MDataHandle excludeFilterHandle =
            dataBlock.inputValue(mExcludeFilterAttr, &status);
        MString& excludeFilterString = excludeFilterHandle.asString();

        if (mExcludeFilterString.length() > 0)
        {
            excludeFilterHandle.set(mExcludeFilterString);
            dataBlock.setClean(mExcludeFilterAttr);
        }
        else if (excludeFilterString.length() > 0)
        {
            mExcludeFilterString = excludeFilterString;
        }


        MFnDependencyNode dep(thisMObject());
        MPlug allSetsPlug = dep.findPlug("allColorSets");
        CreateSceneVisitor visitor(inputTime, !allSetsPlug.isNull(),
            MObject::kNullObj, CreateSceneVisitor::NONE, "",
            mIncludeFilterString, mExcludeFilterString);

        visitor.walk(archive);

        if (visitor.hasSampledData())
        {
            // information retrieved from the hierarchy traversal
            // and given to AlembicNode to provide update
            visitor.getData(mData);
            mData.getFrameRange(mSequenceStartTime, mSequenceEndTime);
            MDataHandle startFrameHandle = dataBlock.inputValue(
                mStartFrameAttr, &status);
            startFrameHandle.set(mSequenceStartTime*fps);
            MDataHandle endFrameHandle = dataBlock.inputValue(
                mEndFrameAttr, &status);
            endFrameHandle.set(mSequenceEndTime*fps);
        }
    }

    // Retime
    MDataHandle cycleHandle = dataBlock.inputValue(mCycleTypeAttr, &status);
    short playType = cycleHandle.asShort();
    inputTime = computeRetime(inputTime, mSequenceStartTime, mSequenceEndTime,
                              playType);

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
                else if (mData.mPropList[i].mScalar.valid())
                {
                    // for visibility only
                    if (mData.mPropList[i].mScalar.getName() ==
                        Alembic::AbcGeom::kVisibilityPropertyName)
                    {
                        Alembic::Util::int8_t visVal = 1;
                        mData.mPropList[i].mScalar.get(&visVal,
                            Alembic::Abc::ISampleSelector(mCurTime,
                                Alembic::Abc::ISampleSelector::kNearIndex ));
                        outHandle.setGenericBool(visVal != 0, false);
                    }
                    else
                    {
                        // for all scalar props
                        readProp(mCurTime, mData.mPropList[i].mScalar, outHandle);
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
            // Reference the output to let EM know we are the writer
            // of the data. EM sets the output to holder and causes
            // race condition when evaluating fan-out destinations.
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutSubDArrayAttr, &status);
            const unsigned int elementCount = outArrayHandle.elementCount();
            for (unsigned int j = 0; j < elementCount; j++)
            {
                outArrayHandle.outputValue().data();
                outArrayHandle.next();
            }
            outArrayHandle.setAllClean();
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
            // Reference the output to let EM know we are the writer
            // of the data. EM sets the output to holder and causes
            // race condition when evaluating fan-out destinations.
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutPolyArrayAttr, &status);
            const unsigned int elementCount = outArrayHandle.elementCount();
            for (unsigned int j = 0; j < elementCount; j++)
            {
                outArrayHandle.outputValue().data();
                outArrayHandle.next();
            }
            outArrayHandle.setAllClean();
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
            // Reference the output to let EM know we are the writer
            // of the data. EM sets the output to holder and causes
            // race condition when evaluating fan-out destinations.
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutNurbsSurfaceArrayAttr, &status);
            const unsigned int elementCount = outArrayHandle.elementCount();
            for (unsigned int j = 0; j < elementCount; j++)
            {
                outArrayHandle.outputValue().data();
                outArrayHandle.next();
            }
            outArrayHandle.setAllClean();
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
            // Reference the output to let EM know we are the writer
            // of the data. EM sets the output to holder and causes
            // race condition when evaluating fan-out destinations.
            MArrayDataHandle outArrayHandle =
                dataBlock.outputValue(mOutNurbsCurveGrpArrayAttr, &status);
            const unsigned int elementCount = outArrayHandle.elementCount();
            for (unsigned int j = 0; j < elementCount; j++)
            {
                outArrayHandle.outputValue().data();
                outArrayHandle.next();
            }
            outArrayHandle.setAllClean();
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

#if MAYA_API_VERSION >= 201600
AlembicNode::SchedulingType AlembicNode::schedulingType()const
{
    // Globally serialize this node because the compute method is not thread safe
    return kGloballySerialize;
}
#endif


// returns the list of files to archive.
MStringArray AlembicNode::getFilesToArchive(
    bool /* shortName */,
    bool unresolvedName,
    bool /* markCouldBeImageSequence */) const
{
    MStringArray files;
    MStatus status = MS::kSuccess;

    MPlug layerFilenamesPlug(thisMObject(), mAbcLayerFileNamesAttr);

    MFnStringArrayData fnSAD( layerFilenamesPlug.asMObject() );
    MStringArray layerFilenames = fnSAD.array();

    for( unsigned int i = 0; i < layerFilenames.length(); i++ )
    {
        MString fileName = layerFilenames[i];

        if (status == MS::kSuccess && fileName.length() > 0)
        {
            if(unresolvedName)
            {
                files.append(fileName);
            }
            else
            {
                //unresolvedName is false, resolve the path via MFileObject.
                MFileObject fileObject;
                fileObject.setRawFullName(fileName);
                files.append(fileObject.resolvedFullName());
            }
        }
    }

    return files;
}

#if defined(MAYA_WANT_EXTERNALCONTENTTABLE)
void AlembicNode::getExternalContent(MExternalContentInfoTable& table) const
{
   addExternalContentForFileAttr(table, mAbcLayerFileNamesAttr);
   MPxNode::getExternalContent(table);
}

void AlembicNode::setExternalContent(const MExternalContentLocationTable& table)
{
   setExternalContentForFileAttr(mAbcLayerFileNamesAttr, table);
   MPxNode::setExternalContent(table);
}
#endif

