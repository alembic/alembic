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

#include "MayaTransformWriter.h"
#include "MayaUtility.h"

void addTranslate(const MFnDependencyNode & iTrans,
    MString parentName, MString xName, MString yName, MString zName,
    uint8_t iHint, bool inverse, bool forceStatic,
    Alembic::AbcGeom::XformOpVec & oOpVec,
    std::vector<double> & oStatic,
    std::vector<double> & oAnim,
    std::vector< std::pair<MPlug, bool > > & oSampledList)
{
    Alembic::AbcGeom::XformOp op(Alembic::AbcGeom::kTranslateOperation, iHint);

    MPlug xPlug = iTrans.findPlug(xName);
    int xSamp = 0;
    if (!forceStatic)
    {
        xSamp = util::getSampledType(xPlug);
    }
    double xVal = xPlug.asDouble();

    MPlug yPlug = iTrans.findPlug(yName);
    int ySamp = 0;
    if (!forceStatic)
    {
        ySamp = util::getSampledType(yPlug);
    }
    double yVal = yPlug.asDouble();

    MPlug zPlug = iTrans.findPlug(zName);
    int zSamp = 0;
    if (!forceStatic)
    {
        zSamp = util::getSampledType(zPlug);
    }
    double zVal = zPlug.asDouble();

    // this is to handle the case where there is a connection to the parent
    // plug but not to the child plugs, if the connection is there then all
    // of the children are considered animated
    MPlug parentPlug = iTrans.findPlug(parentName);
    if (!forceStatic && util::getSampledType(parentPlug) != 0)
    {
        xSamp = 1;
        ySamp = 1;
        zSamp = 1;
    }

    // something is sampled or not identity, add it to the stack
    if (xSamp != 0 || ySamp != 0 || zSamp != 0 || xVal != 0.0 || yVal != 0.0 ||
        zVal != 0.0)
    {
        if (inverse)
        {
            xVal = -xVal;
            yVal = -yVal;
            zVal = -zVal;
        }

        if (xSamp == 0)
        {
            oStatic.push_back(xVal);
        }
        else
        {
            oAnim.push_back(xVal);
            op.setXAnimated(true);
            oSampledList.push_back(std::pair<MPlug, bool >(xPlug, inverse));
        }

        if (ySamp == 0)
        {
            oStatic.push_back(yVal);
        }
        else
        {
            oAnim.push_back(yVal);
            op.setYAnimated(true);
            oSampledList.push_back(std::pair< MPlug, bool >(yPlug, inverse));
        }

        if (zSamp == 0)
        {
            oStatic.push_back(zVal);
        }
        else
        {
            oAnim.push_back(zVal);
            op.setZAnimated(true);
            oSampledList.push_back(std::pair< MPlug, bool >(zPlug, inverse));
        }

        oOpVec.push_back(op);
    }
}

// names need to be passed in x,y,z order, iOrder is the order to
// use these indices
void addRotate(const MFnDependencyNode & iTrans,
    MString parentName, const MString* iNames, const unsigned int* iOrder,
    uint8_t iHint, bool forceStatic, bool forceAnimated,
    Alembic::AbcGeom::XformOpVec & oOpVec,
    std::vector<double> & oStatic,
    std::vector<double> & oAnim,
    std::vector< std::pair<MPlug, bool > > & oSampledList)
{
    Alembic::AbcGeom::XformOp op(Alembic::AbcGeom::kRotateOperation, iHint);

    // for the rotation axis
    static const float rotVecs[3][3] = {
         {1.0, 0.0, 0.0},
         {0.0, 1.0, 0.0},
         {0.0, 0.0, 1.0}
    };

    // this is to handle the case where there is a connection to the parent
    // plug but not to the child plugs, if the connection is there then all
    // of the children are considered animated
    MPlug parentPlug = iTrans.findPlug(parentName);
    int parentSamp = 0;
    if (!forceStatic)
    {
        if (!forceAnimated)
            parentSamp = util::getSampledType(parentPlug);
        else
            parentSamp = 1;
    }

    // whether we are using the XYZ rotation order
    bool isXYZ = ((iOrder[0] == 0) && (iOrder[1] == 1) && (iOrder[2] == 2));

    // add them in backwards since we are dealing with a stack
    int i = 2;
    for (; i > -1; i--)
    {
        unsigned int index = iOrder[i];
        MPlug plug = iTrans.findPlug(iNames[index]);
        int samp = 0;
        if (!forceStatic)
        {
            if (!forceAnimated)
                samp = util::getSampledType(plug);
            else
                samp = 1;

            if (samp == 0)
                samp = parentSamp;
        }

        double plugVal = plug.asDouble();

        // the sampled case
        if (samp != 0)
        {
            // push the rotation axis first
            oStatic.push_back(rotVecs[index][0]);
            oStatic.push_back(rotVecs[index][1]);
            oStatic.push_back(rotVecs[index][2]);

            op.setAngleAnimated(true);
            oAnim.push_back(plugVal);
            oSampledList.push_back(std::pair< MPlug, bool >(plug, false));
        }
        // the non XYZ axis or nonzero angle case
        else if (!isXYZ || plugVal != 0.0)
        {

            // push the rotation axis first
            oStatic.push_back(rotVecs[index][0]);
            oStatic.push_back(rotVecs[index][1]);
            oStatic.push_back(rotVecs[index][2]);

            oStatic.push_back(plugVal);
        }

        // non sampled, XYZ axis and the angle is 0, do not add to the stack
        else
            continue;

        oOpVec.push_back(op);
    }
}

// the test on whether or not to add it is very similiar to addTranslate
// but the operation it creates is very different
void addShear(const MFnDependencyNode & iTrans, bool forceStatic,
    Alembic::AbcGeom::XformOpVec & oOpVec, std::vector<double> & oStatic,
    std::vector<double> & oAnim,
    std::vector< std::pair<MPlug, bool > > & oSampledList)
{
    Alembic::AbcGeom::XformOp op(Alembic::AbcGeom::kMatrixOperation,
        Alembic::AbcGeom::kMayaShearHint);

    MString str = "shearXY";
    MPlug xyPlug = iTrans.findPlug(str);
    int xySamp = 0;
    if (!forceStatic)
    {
        xySamp = util::getSampledType(xyPlug);
    }
    double xyVal = xyPlug.asDouble();

    str = "shearXZ";
    MPlug xzPlug = iTrans.findPlug(str);
    int xzSamp = 0;
    if (!forceStatic)
    {
        xzSamp = util::getSampledType(xzPlug);
    }
    double xzVal = xzPlug.asDouble();

    str = "shearYZ";
    MPlug yzPlug = iTrans.findPlug(str);
    int yzSamp = 0;
    if (!forceStatic)
    {
        yzSamp = util::getSampledType(yzPlug);
    }
    double yzVal = yzPlug.asDouble();

    // this is to handle the case where there is a connection to the parent
    // plug but not to the child plugs, if the connection is there then all
    // of the children are considered animated
    str = "shear";
    MPlug parentPlug = iTrans.findPlug(str);
    if (!forceStatic && util::getSampledType(parentPlug) != 0)
    {
        xySamp = 1;
        xzSamp = 1;
        yzSamp = 1;
    }

    // something is sampled or not identity, add it to the stack
    if (xySamp != 0 || xzSamp != 0 || yzSamp != 0 ||
        xyVal != 0.0 || xzVal != 0.0 || yzVal != 0.0)
    {
        oStatic.push_back(1.0);
        oStatic.push_back(0.0);
        oStatic.push_back(0.0);
        oStatic.push_back(0.0);

        if (xySamp == 0)
        {
            oStatic.push_back(xyVal);
        }
        else
        {
            oAnim.push_back(xyVal);
            op.setIndexAnimated(4, true);
            oSampledList.push_back(std::pair< MPlug, bool >(xyPlug, false));
        }

        oStatic.push_back(1.0);
        oStatic.push_back(0.0);
        oStatic.push_back(0.0);

        if (xzSamp == 0)
        {
            oStatic.push_back(xzVal);
        }
        else
        {
            oAnim.push_back(xzVal);
            op.setIndexAnimated(8, true);
            oSampledList.push_back(std::pair< MPlug, bool >(xzPlug, false));
        }

        if (yzSamp == 0)
        {
            oStatic.push_back(yzVal);
        }
        else
        {
            oAnim.push_back(yzVal);
            op.setIndexAnimated(9, true);
            oSampledList.push_back(std::pair< MPlug, bool >(yzPlug, false));
        }

        oStatic.push_back(1.0);
        oStatic.push_back(0.0);

        oStatic.push_back(0.0);
        oStatic.push_back(0.0);
        oStatic.push_back(0.0);
        oStatic.push_back(1.0);

        oOpVec.push_back(op);
    }
}

// this test is very similiar to addTranslate, except that it doesn't add it
// to the stack if x,y, and z are 1.0
void addScale(const MFnDependencyNode & iTrans,
    MString parentName, MString xName, MString yName, MString zName,
    bool forceStatic, Alembic::AbcGeom::XformOpVec & oOpVec,
    std::vector<double> & oStatic,
    std::vector<double> & oAnim,
    std::vector< std::pair<MPlug, bool > > & oSampledList)
{

    Alembic::AbcGeom::XformOp op(Alembic::AbcGeom::kScaleOperation,
        Alembic::AbcGeom::kScaleHint);

    MPlug xPlug = iTrans.findPlug(xName);
    int xSamp = 0;
    if (!forceStatic)
    {
        xSamp = util::getSampledType(xPlug);
    }
    double xVal = xPlug.asDouble();

    MPlug yPlug = iTrans.findPlug(yName);
    int ySamp = 0;
    if (!forceStatic)
    {
        ySamp = util::getSampledType(yPlug);
    }
    double yVal = yPlug.asDouble();

    MPlug zPlug = iTrans.findPlug(zName);
    int zSamp = 0;
    if (!forceStatic)
    {
        zSamp = util::getSampledType(zPlug);
    }
    double zVal = zPlug.asDouble();

    // this is to handle the case where there is a connection to the parent
    // plug but not to the child plugs, if the connection is there then all
    // of the children are considered animated
    MPlug parentPlug = iTrans.findPlug(parentName);
    if (!forceStatic && util::getSampledType(parentPlug) != 0)
    {
        xSamp = 1;
        ySamp = 1;
        zSamp = 1;
    }

    // something is sampled or not identity, add it to the stack
    if (xSamp != 0 || ySamp != 0 || zSamp != 0 || xVal != 1.0 || yVal != 1.0 ||
        zVal != 1.0)
    {
        if (xSamp == 0)
        {
            oStatic.push_back(xVal);
        }
        else
        {
            oAnim.push_back(xVal);
            op.setXAnimated(true);
            oSampledList.push_back(std::pair< MPlug, bool >(xPlug, false));
        }

        if (ySamp == 0)
        {
            oStatic.push_back(yVal);
        }
        else
        {
            oAnim.push_back(yVal);
            op.setYAnimated(true);
            oSampledList.push_back(std::pair< MPlug, bool >(yPlug, false));
        }

        if (zSamp == 0)
        {
            oStatic.push_back(zVal);
        }
        else
        {
            oAnim.push_back(zVal);
            op.setZAnimated(true);
            oSampledList.push_back(std::pair< MPlug, bool >(zPlug, false));
        }

        oOpVec.push_back(op);
    }
}

MayaTransformWriter::MayaTransformWriter(double iFrame,
    Alembic::AbcGeom::OObject & iParent, MDagPath & iDag,
    Alembic::AbcCoreAbstract::v1::TimeSamplingType & iTimeType,
    bool iAddWorld, bool iWriteVisibility)
{
    mCurIndex = 0;

    Alembic::AbcGeom::XformOpVec opVec;
    std::vector<double> staticData;
    std::vector<double> animData;

    if (iDag.hasFn(MFn::kJoint))
    {
        MFnIkJoint joint(iDag);
        Alembic::AbcGeom::OXform obj(iParent, joint.name().asChar(), iTimeType);
        mSchema = obj.getSchema();

        mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, obj,
            joint, iTimeType, iWriteVisibility));

        if (!iAddWorld)
        {
            pushTransformStack(iFrame, joint, opVec, staticData,
                animData);

            // need to look at inheritsTransform
            MFnDagNode dagNode(iDag);
            MPlug inheritPlug = dagNode.findPlug("inheritsTransform");
            if (!inheritPlug.isNull() && inheritPlug.asBool() == false)
                mSchema.setInherits(false);

            if (opVec.empty())
                return;

            mSchema.setXform(opVec,
                Alembic::Abc::DoubleArraySample(staticData));

            if (animData.empty())
            {
                mSchema.set(Alembic::Abc::DoubleArraySample(animData),
                    Alembic::Abc::OSampleSelector(mCurIndex++, iFrame));
            }

            return;
        }
    }
    else
    {
        MFnTransform trans(iDag);
        Alembic::AbcGeom::OXform obj(iParent, trans.name().asChar(), iTimeType);
        mSchema = obj.getSchema();

        mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, obj,
            trans, iTimeType, iWriteVisibility));

        if (!iAddWorld)
        {
            pushTransformStack(iFrame, trans, opVec, staticData, animData);

            // need to look at inheritsTransform
            MFnDagNode dagNode(iDag);
            MPlug inheritPlug = dagNode.findPlug("inheritsTransform");
            if (!inheritPlug.isNull() && inheritPlug.asBool() == false)
                mSchema.setInherits(false);

            if (opVec.empty())
                return;

            mSchema.setXform(opVec,
                Alembic::Abc::DoubleArraySample(staticData));

            if (animData.empty())
            {
                mSchema.set(Alembic::Abc::DoubleArraySample(animData),
                    Alembic::Abc::OSampleSelector(mCurIndex++, iFrame));
            }

            return;
        }
    }

    // if we didn't bail early then we need to add all the transform
    // information at the current node and above

    // copy the dag path because we'll be popping from it
    MDagPath dag(iDag);

    int i;
    int numPaths = dag.length();
    std::vector< MDagPath > dagList;
    for (i = numPaths - 1; i > -1; i--, dag.pop())
    {
        dagList.push_back(dag);

        // inheritsTransform exists on both joints and transforms
        MFnDagNode dagNode(dag);
        MPlug inheritPlug = dagNode.findPlug("inheritsTransform");

        // if inheritsTransform exists and is set to false, then we
        // don't need to worry about ancestor nodes above this one
        if (!inheritPlug.isNull() && !inheritPlug.asBool())
            break;
    }


    std::vector< MDagPath >::iterator iStart = dagList.begin();

    std::vector< MDagPath >::iterator iCur = dagList.end();
    iCur--;

    // now loop backwards over our dagpath list so we push ancestor nodes
    // first, all the way down to the current node
    for (; iCur != iStart; iCur--)
    {
        // only add it to the stack don't write it yet!

        if (iCur->hasFn(MFn::kJoint))
        {
            MFnIkJoint joint(*iCur);
            pushTransformStack(iFrame, joint, opVec, staticData, animData);
        }
        else
        {
            MFnTransform trans(*iCur);
            pushTransformStack(iFrame, trans, opVec, staticData, animData);
        }
    }

    // finally add any transform info on the final node and write it
    if (iCur->hasFn(MFn::kJoint))
    {
        MFnIkJoint joint(*iCur);
        pushTransformStack(iFrame, joint, opVec, staticData, animData);
    }
    else
    {
        MFnTransform trans(*iCur);
        pushTransformStack(iFrame, trans, opVec, staticData, animData);
    }

    // need to look at inheritsTransform
    MFnDagNode dagNode(iDag);
    MPlug inheritPlug = dagNode.findPlug("inheritsTransform");
    if (!inheritPlug.isNull() && inheritPlug.asBool() == false)
        mSchema.setInherits(false);

    // we don't need to write anything since no operations were added
    if (opVec.empty())
        return;

    mSchema.setXform(opVec, Alembic::Abc::DoubleArraySample(staticData));

    if (animData.size() > 0)
    {
        mSchema.set(Alembic::Abc::DoubleArraySample(animData),
            Alembic::Abc::OSampleSelector(mCurIndex++, iFrame));
    }
}

MayaTransformWriter::MayaTransformWriter(double iFrame,
    MayaTransformWriter & iParent, MDagPath & iDag,
    Alembic::AbcCoreAbstract::v1::TimeSamplingType & iTimeType,
    bool iWriteVisibility)
{
    mCurIndex = 0;

    Alembic::AbcGeom::XformOpVec opVec;
    std::vector<double> staticData;
    std::vector<double> animData;

    if (iDag.hasFn(MFn::kJoint))
    {
        MFnIkJoint joint(iDag);

        Alembic::AbcGeom::OXform obj(iParent.getObject(), joint.name().asChar(),
            iTimeType);
        mSchema = obj.getSchema();

        mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, obj,
            joint, iTimeType, iWriteVisibility));

        pushTransformStack(iFrame, joint, opVec, staticData, animData);
    }
    else
    {
        MFnTransform trans(iDag);
        Alembic::AbcGeom::OXform obj(iParent.getObject(), trans.name().asChar(),
            iTimeType);
        mSchema = obj.getSchema();

        mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, obj,
            trans, iTimeType, iWriteVisibility));

        pushTransformStack(iFrame, trans, opVec, staticData, animData);
    }


    // need to look at inheritsTransform
    MFnDagNode dagNode(iDag);
    MPlug inheritPlug = dagNode.findPlug("inheritsTransform");
    if (!inheritPlug.isNull() && inheritPlug.asBool() == false)
        mSchema.setInherits(false);

    // we don't need to write anything since no operations were added
    if (opVec.empty())
        return;

    mSchema.setXform(opVec, Alembic::Abc::DoubleArraySample(staticData));

    if (animData.size() > 0)
    {
        mSchema.set(Alembic::Abc::DoubleArraySample(animData),
            Alembic::Abc::OSampleSelector(mCurIndex++, iFrame));
    }
}


MayaTransformWriter::~MayaTransformWriter()
{
}

void MayaTransformWriter::write(double iFrame)
{
    size_t numSamples = mSampledList.size();
    if (numSamples > 0)
    {
        std::vector<double> samples(numSamples);
        size_t i;
        for (i = 0; i < numSamples; ++i)
        {
            // the invert case
            if (mSampledList[i].second)
                samples[i] = -mSampledList[i].first.asDouble();
            else
                samples[i] = mSampledList[i].first.asDouble();
        }
        mSchema.set(Alembic::Abc::DoubleArraySample(samples),
            Alembic::Abc::OSampleSelector(mCurIndex++, iFrame));
    }
    mAttrs->write(iFrame);
}

bool MayaTransformWriter::isAnimated() const
{
    return mAttrs->isAnimated() || mSampledList.size() > 0;
}

void MayaTransformWriter::pushTransformStack(double iFrame,
    const MFnTransform & iTrans, Alembic::AbcGeom::XformOpVec & oOpVec,
    std::vector<double> & oStatic, std::vector<double> & oAnim)
{
    bool forceStatic = (iFrame == DBL_MAX);

    // inspect the translate
    addTranslate(iTrans, "translate", "translateX", "translateY", "translateZ",
        Alembic::AbcGeom::kTranslateHint, false, forceStatic, oOpVec,
        oStatic, oAnim, mSampledList);

    // inspect the rotate pivot translate
    addTranslate(iTrans, "rotatePivotTranslate", "rotatePivotTranslateX",
        "rotatePivotTranslateY", "rotatePivotTranslateZ",
        Alembic::AbcGeom::kRotatePivotTranslationHint, false,
            forceStatic, oOpVec, oStatic, oAnim, mSampledList);

    // inspect the rotate pivot
    addTranslate(iTrans, "rotatePivot", "rotatePivotX", "rotatePivotY",
        "rotatePivotZ",  Alembic::AbcGeom::kRotatePivotPointHint,
        false, forceStatic, oOpVec, oStatic, oAnim, mSampledList);

    // inspect rotate names
    MString rotateNames[3];
    rotateNames[0] = "rotateX";
    rotateNames[1] = "rotateY";
    rotateNames[2] = "rotateZ";

    unsigned int rotOrder[3];

    // if this returns false then the rotation order was kInvalid or kLast
    if (util::getRotOrder(iTrans.rotationOrder(), rotOrder[0], rotOrder[1],
        rotOrder[2]))
    {
        addRotate(iTrans, "rotate", rotateNames, rotOrder,
            Alembic::AbcGeom::kRotateHint, forceStatic, false, 
            oOpVec, oStatic, oAnim, mSampledList);
    }

    // now look at the rotation orientation, aka rotate axis
    rotateNames[0] = "rotateAxisX";
    rotateNames[1] = "rotateAxisY";
    rotateNames[2] = "rotateAxisZ";
    rotOrder[0] = 0;
    rotOrder[1] = 1;
    rotOrder[2] = 2;
    addRotate(iTrans, "rotateAxis", rotateNames, rotOrder,
        Alembic::AbcGeom::kRotateOrientationHint, forceStatic, false,
        oOpVec, oStatic, oAnim, mSampledList);

    // invert the rotate pivot if necessary
    addTranslate(iTrans, "rotatePivot", "rotatePivotX", "rotatePivotY",
        "rotatePivotZ", Alembic::AbcGeom::kRotatePivotPointHint,
        true, forceStatic, oOpVec, oStatic, oAnim, mSampledList);

    // inspect the scale pivot translation
    addTranslate(iTrans, "scalePivotTranslate", "scalePivotTranslateX",
        "scalePivotTranslateY", "scalePivotTranslateZ",
        Alembic::AbcGeom::kScalePivotTranslationHint, false, forceStatic,
        oOpVec, oStatic, oAnim, mSampledList);

    // inspect the scale pivot point
    addTranslate(iTrans, "scalePivot", "scalePivotX", "scalePivotY",
        "scalePivotZ", Alembic::AbcGeom::kScalePivotPointHint, false,
        forceStatic, oOpVec, oStatic, oAnim, mSampledList);

    // inspect the shear
    addShear(iTrans, forceStatic, oOpVec, oStatic, oAnim, mSampledList);

    // add the scale
    addScale(iTrans, "scale", "scaleX", "scaleY", "scaleZ", forceStatic,
        oOpVec, oStatic, oAnim, mSampledList);

    // inverse the scale pivot point if necessary
    addTranslate(iTrans, "scalePivot", "scalePivotX", "scalePivotY",
        "scalePivotZ", Alembic::AbcGeom::kScalePivotPointHint, true,
        forceStatic, oOpVec, oStatic, oAnim, mSampledList);
}

void MayaTransformWriter::pushTransformStack(double iFrame,
    const MFnIkJoint & iJoint, Alembic::AbcGeom::XformOpVec & oOpVec,
    std::vector<double> & oStatic, std::vector<double> & oAnim)
{
    bool forceStatic = (iFrame == DBL_MAX);

    // inspect the translate
    addTranslate(iJoint, "translate", "translateX", "translateY", "translateZ",
        Alembic::AbcGeom::kTranslateHint, false, forceStatic,
        oOpVec, oStatic, oAnim, mSampledList);

    // inspect the inverseParent scale
    addScale(iJoint, "inverseScale", "inverseScaleX", "inverseScaleY",
        "inverseScaleZ", forceStatic, oOpVec, oStatic, oAnim, mSampledList);

    MTransformationMatrix::RotationOrder order;
    double vals[3];

    // for reordering rotate names
    MString rotateNames[3];
    unsigned int rotOrder[3];

    // now look at the joint orientation
    rotateNames[0] = "jointOrientX";
    rotateNames[1] = "jointOrientY";
    rotateNames[2] = "jointOrientZ";

    iJoint.getOrientation(vals, order);
    if (util::getRotOrder(order, rotOrder[0], rotOrder[1], rotOrder[2]))
    {
        addRotate(iJoint, "jointOrient", rotateNames, rotOrder,
            Alembic::AbcGeom::kRotateHint, forceStatic, true,
            oOpVec, oStatic, oAnim, mSampledList);
    }

    rotateNames[0] = "rotateX";
    rotateNames[1] = "rotateY";
    rotateNames[2] = "rotateZ";

    // if this returns false then the rotation order was kInvalid or kLast
    if (util::getRotOrder(iJoint.rotationOrder(), rotOrder[0], rotOrder[1],
        rotOrder[2]))
    {
        addRotate(iJoint, "rotate", rotateNames, rotOrder,
            Alembic::AbcGeom::kRotateHint, forceStatic, true,
            oOpVec, oStatic, oAnim, mSampledList);
    }

    // now look at the rotation orientation, aka rotate axis
    rotateNames[0] = "rotateAxisX";
    rotateNames[1] = "rotateAxisY";
    rotateNames[2] = "rotateAxisZ";

    iJoint.getScaleOrientation(vals, order);
    if (util::getRotOrder(order, rotOrder[0], rotOrder[1], rotOrder[2]))
    {
        addRotate(iJoint, "rotateAxis", rotateNames, rotOrder,
            Alembic::AbcGeom::kRotateOrientationHint, forceStatic, true,
            oOpVec, oStatic, oAnim, mSampledList);
    }

    // inspect the scale
    addScale(iJoint, "scale", "scaleX", "scaleY", "scaleZ", forceStatic,
        oOpVec, oStatic, oAnim, mSampledList);
}
