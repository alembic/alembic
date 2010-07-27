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

#include <AlembicTakoExport/MayaTransformWriter.h>
#include <AlembicTakoExport/MayaUtility.h>

namespace Alembic {

MayaTransformWriter::MayaTransformWriter(float iFrame,
    TransformWriterPtr iParent, MDagPath & iDag, bool iAddWorld,
    bool iWriteVisibility)
{
    if (iDag.hasFn(MFn::kJoint))
    {
        MFnIkJoint joint(iDag);
        mWriter = TransformWriterPtr(new Tako::TransformWriter(
            joint.name().asChar(), *iParent));

        mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, *mWriter,
            joint, iWriteVisibility));

        if (!iAddWorld)
        {
            pushTransformStack(iFrame, joint, true);
            return;
        }
    }
    else
    {
        MFnTransform trans(iDag);
        mWriter = TransformWriterPtr(new Tako::TransformWriter(
            trans.name().asChar(), *iParent));

        mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, *mWriter,
            trans, iWriteVisibility));

        if (!iAddWorld)
        {
            pushTransformStack(iFrame, trans, true);
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
        MFnTransform trans(dag);
        MPlug inheritPlug = trans.findPlug("inheritsTransform");

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
            pushTransformStack(iFrame, joint, false);
        }
        else
        {
            MFnTransform trans(*iCur);
            pushTransformStack(iFrame, trans, false);
        }
    }

    // finally add any transform info on the final node and write it
    if (iCur->hasFn(MFn::kJoint))
    {
        MFnIkJoint joint(*iCur);
        pushTransformStack(iFrame, joint, true);
    }
    else
    {
        MFnTransform trans(*iCur);
        pushTransformStack(iFrame, trans, true);
    }
}

MayaTransformWriter::MayaTransformWriter(float iFrame,
    MayaTransformWriter & iParent, MDagPath & iDag,
    bool iWriteVisibility)
{

    if (iDag.hasFn(MFn::kJoint))
    {
        MFnIkJoint joint(iDag);
        mWriter = TransformWriterPtr(new Tako::TransformWriter(
            joint.name().asChar(), *iParent.mWriter));

        mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, *mWriter,
            joint, iWriteVisibility));

        pushTransformStack(iFrame, joint, true);
    }
    else
    {
        MFnTransform trans(iDag);
        mWriter = TransformWriterPtr(new Tako::TransformWriter(
            trans.name().asChar(), *iParent.mWriter));

        mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, *mWriter,
            trans, iWriteVisibility));

        pushTransformStack(iFrame, trans, true);
    }


}


MayaTransformWriter::~MayaTransformWriter()
{
}

TransformWriterPtr MayaTransformWriter::getWriter() const
{
    return mWriter;
}

void MayaTransformWriter::write(float iFrame)
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
        mWriter->writeSamples(iFrame,  samples);
    }
    mAttrs->write(iFrame);
}

bool MayaTransformWriter::isAnimated() const
{
    return mAttrs->isAnimated() || mSampledList.size() > 0;
}

void MayaTransformWriter::pushTransformStack(float iFrame,
    const MFnTransform & iTrans, bool writeStack)
{
    bool forceStatic = (iFrame == FLT_MAX);

    // inspect the translate
    addTranslate(iTrans, "translate", "translateX", "translateY", "translateZ",
        Tako::Translate::cTranslate, false, forceStatic);

    // inspect the rotate pivot translate
    addTranslate(iTrans, "rotatePivotTranslate", "rotatePivotTranslateX",
        "rotatePivotTranslateY", "rotatePivotTranslateZ",
        Tako::Translate::cRotatePivotTranslation, false,
            forceStatic);

    // inspect the rotate pivot
    addTranslate(iTrans, "rotatePivot", "rotatePivotX", "rotatePivotY",
        "rotatePivotZ", Tako::Translate::cRotatePivotPoint,
        false, forceStatic);

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
            Tako::Rotate::cRotate, forceStatic, false);
    }

    // now look at the rotation orientation, aka rotate axis
    rotateNames[0] = "rotateAxisX";
    rotateNames[1] = "rotateAxisY";
    rotateNames[2] = "rotateAxisZ";
    rotOrder[0] = 0;
    rotOrder[1] = 1;
    rotOrder[2] = 2;
    addRotate(iTrans, "rotateAxis", rotateNames, rotOrder,
        Tako::Rotate::cRotateOrientation, forceStatic, false);

    // invert the rotate pivot if necessary
    addTranslate(iTrans, "rotatePivot", "rotatePivotX", "rotatePivotY",
        "rotatePivotZ", Tako::Translate::cRotatePivotPoint,
        true, forceStatic);

    // inspect the scale pivot translation
    addTranslate(iTrans, "scalePivotTranslate", "scalePivotTranslateX",
        "scalePivotTranslateY", "scalePivotTranslateZ",
        Tako::Translate::cScalePivotTranslation, false, forceStatic);

    // inspect the scale pivot point
    addTranslate(iTrans, "scalePivot", "scalePivotX", "scalePivotY",
        "scalePivotZ", Tako::Translate::cScalePivotPoint, false,
        forceStatic);

    // inspect the shear
    addShear(iTrans, forceStatic);

    // add the scale
    addScale(iTrans, "scale", "scaleX", "scaleY", "scaleZ", forceStatic);

    // inverse the scale pivot point if necessary
    addTranslate(iTrans, "scalePivot", "scalePivotX", "scalePivotY",
        "scalePivotZ", Tako::Translate::cScalePivotPoint, true,
        forceStatic);

    // we haven't yet written the transform stack
    if (writeStack)
    {
        // need to look at inheritsTransform
        MPlug inheritPlug = iTrans.findPlug("inheritsTransform");
        bool inheritsTransform = true;
        if (!inheritPlug.isNull())
            inheritsTransform = inheritPlug.asBool();

        mWriter->writeTransformStack(iFrame, inheritsTransform);
    }
}

void MayaTransformWriter::pushTransformStack(float iFrame,
    const MFnIkJoint & iJoint, bool writeStack)
{
    bool forceStatic = (iFrame == FLT_MAX);

    // inspect the translate
    addTranslate(iJoint, "translate", "translateX", "translateY", "translateZ",
        Tako::Translate::cTranslate, false, forceStatic);

    // inspect the inverseParent scale
    addScale(iJoint, "inverseScale", "inverseScaleX", "inverseScaleY",
        "inverseScaleZ", forceStatic);

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
            Tako::Rotate::cRotate, forceStatic, true);
    }

    rotateNames[0] = "rotateX";
    rotateNames[1] = "rotateY";
    rotateNames[2] = "rotateZ";

    // if this returns false then the rotation order was kInvalid or kLast
    if (util::getRotOrder(iJoint.rotationOrder(), rotOrder[0], rotOrder[1],
        rotOrder[2]))
    {
        addRotate(iJoint, "rotate", rotateNames, rotOrder,
            Tako::Rotate::cRotate, forceStatic, true);
    }

    // now look at the rotation orientation, aka rotate axis
    rotateNames[0] = "rotateAxisX";
    rotateNames[1] = "rotateAxisY";
    rotateNames[2] = "rotateAxisZ";

    iJoint.getScaleOrientation(vals, order);
    if (util::getRotOrder(order, rotOrder[0], rotOrder[1], rotOrder[2]))
    {
        addRotate(iJoint, "rotateAxis", rotateNames, rotOrder,
            Tako::Rotate::cRotateOrientation, forceStatic, true);
    }

    // inspect the scale
    addScale(iJoint, "scale", "scaleX", "scaleY", "scaleZ", forceStatic);

    // we haven't yet written the transform stack
    if (writeStack)
    {
        // need to look at inheritsTransform
        MPlug inheritPlug = iJoint.findPlug("inheritsTransform");
        bool inheritsTransform = true;
        if (!inheritPlug.isNull())
            inheritsTransform = inheritPlug.asBool();

        mWriter->writeTransformStack(iFrame, inheritsTransform);
    }
}

void MayaTransformWriter::addTranslate(const MFnDependencyNode & iTrans,
    MString parentName, MString xName, MString yName, MString zName,
    Tako::Translate::TranslateType iType, bool inverse,
    bool forceStatic)
{
    Tako::Value vx, vy, vz;
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
            vx = xVal;
        }
        else
        {
            vx = xVal;
            vx.mIsStatic = false;
            mSampledList.push_back(std::pair<MPlug, bool >(xPlug, inverse));
        }

        if (ySamp == 0)
        {
            vy = yVal;
        }
        else
        {
            vy = yVal;
            vy.mIsStatic = false;
            mSampledList.push_back(std::pair< MPlug, bool >(yPlug, inverse));
        }

        if (zSamp == 0)
        {
            vz = zVal;
        }
        else
        {
            vz = zVal;
            vz.mIsStatic = false;
            mSampledList.push_back(std::pair< MPlug, bool >(zPlug, inverse));
        }
        Tako::Translate t(iType, vx, vy, vz);

        mWriter->push(t);
    }
}

void MayaTransformWriter::addRotate(const MFnDependencyNode & iTrans,
    MString parentName, const MString* iNames, const unsigned int* iOrder,
    Tako::Rotate::RotateType iType, bool forceStatic, bool forceAnimated)
{
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
        Tako::Value v;

        // the sampled case
        if (samp != 0)
        {
            v = plugVal;
            v.mIsStatic = false;
            mSampledList.push_back(std::pair< MPlug, bool >(plug, false));
        }
        // the non XYZ axis or nonzero angle case
        else if (!isXYZ || plugVal != 0.0)
        {
            v = plugVal;
        }

        // non sampled, XYZ axis and the angle is 0, do not add to the stack
        else
            continue;

        Tako::Rotate r(iType, v, rotVecs[index][0], rotVecs[index][1],
            rotVecs[index][2]);

        mWriter->push(r);
    }
}

// the test on whether or not to add it is very similiar to addTranslate
// but the operation it creates is very different
void MayaTransformWriter::addShear(const MFnDependencyNode & iTrans,
    bool forceStatic)
{
    Tako::Value vxy, vxz, vyz;

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
        if (xySamp == 0)
        {
            vxy = xyVal;
        }
        else
        {
            vxy = xyVal;
            vxy.mIsStatic = false;
            mSampledList.push_back(std::pair< MPlug, bool >(xyPlug, false));
        }

        if (xzSamp == 0)
        {
            vxz = xzVal;
        }
        else
        {
            vxz = xzVal;
            vxz.mIsStatic = false;
            mSampledList.push_back(std::pair< MPlug, bool >(xzPlug, false));
        }

        if (yzSamp == 0)
        {
            vyz = yzVal;
        }
        else
        {
            vyz = yzVal;
            vyz.mIsStatic = false;
            mSampledList.push_back(std::pair< MPlug, bool >(yzPlug, false));
        }

        static const Tako::Value one = 1.0;
        static const Tako::Value zero = 0.0;
        Tako::Matrix4x4 m(Tako::Matrix4x4::cMayaShear,
            one , zero, zero, zero,
            vxy , one , zero, zero,
            vxz , vyz , one , zero,
            zero, zero, zero, one);

        mWriter->push(m);
    }
}

// this test is very similiar to addTranslate, except that it doesn't add it
// to the stack if x,y, and z are 1.0
void MayaTransformWriter::addScale(const MFnDependencyNode & iTrans,
    MString parentName, MString xName, MString yName, MString zName,
    bool forceStatic)
{
    Tako::Value vx, vy, vz;

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
            vx = xVal;
        }
        else
        {
            vx = xVal;
            vx.mIsStatic = false;
            mSampledList.push_back(std::pair< MPlug, bool >(xPlug, false));
        }

        if (ySamp == 0)
        {
            vy = yVal;
        }
        else
        {
            vy = yVal;
            vy.mIsStatic = false;
            mSampledList.push_back(std::pair< MPlug, bool >(yPlug, false));
        }

        if (zSamp == 0)
        {
            vz = zVal;
        }
        else
        {
            vz = zVal;
            vz.mIsStatic = false;
            mSampledList.push_back(std::pair< MPlug, bool >(zPlug, false));
        }
        Tako::Scale s(Tako::Scale::cScale, vx, vy, vz);

        mWriter->push(s);
    }
}

} // End namespace Alembic
