//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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
    Alembic::Util::uint8_t iHint, bool inverse, bool forceStatic,
    bool forceAnimated, Alembic::AbcGeom::XformSample & oSample,
    std::vector < AnimChan > & oAnimChanList)
{
    Alembic::AbcGeom::XformOp op(Alembic::AbcGeom::kTranslateOperation, iHint);

    MPlug xPlug = iTrans.findPlug(xName, true);
    int xSamp = 0;
    if (!forceStatic)
    {
        if (!forceAnimated)
            xSamp = util::getSampledType(xPlug);
        else
            xSamp = 1;
    }
    double xVal = xPlug.asDouble();

    MPlug yPlug = iTrans.findPlug(yName, true);
    int ySamp = 0;
    if (!forceStatic)
    {
        if (!forceAnimated)
            ySamp = util::getSampledType(yPlug);
        else
            ySamp = 1;
    }
    double yVal = yPlug.asDouble();

    MPlug zPlug = iTrans.findPlug(zName, true);
    int zSamp = 0;
    if (!forceStatic)
    {
        if (!forceAnimated)
            zSamp = util::getSampledType(zPlug);
        else
            zSamp = 1;
    }
    double zVal = zPlug.asDouble();

    // this is to handle the case where there is a connection to the parent
    // plug but not to the child plugs, if the connection is there then all
    // of the children are considered animated
    MPlug parentPlug = iTrans.findPlug(parentName, true);
    int parentSamp = 0;
    if (!forceStatic)
    {
        if (!forceAnimated)
            parentSamp = util::getSampledType(parentPlug);
        else
            parentSamp = 1;
    }

    if (parentSamp != 0)
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

        op.setChannelValue(0, xVal);
        op.setChannelValue(1, yVal);
        op.setChannelValue(2, zVal);

        if (xSamp != 0)
        {
            AnimChan chan;
            chan.plug = xPlug;
            chan.scale = 1.0;
            if (inverse)
                chan.scale = -1.0;
            chan.opNum = oSample.getNumOps();
            chan.channelNum = 0;
            oAnimChanList.push_back(chan);
        }

        if (ySamp != 0)
        {
            AnimChan chan;
            chan.plug = yPlug;
            chan.scale = 1.0;
            if (inverse)
                chan.scale = -1.0;
            chan.opNum = oSample.getNumOps();
            chan.channelNum = 1;
            oAnimChanList.push_back(chan);
        }

        if (zSamp != 0)
        {
            AnimChan chan;
            chan.plug = zPlug;
            chan.scale = 1.0;
            if (inverse)
                chan.scale = -1.0;
            chan.opNum = oSample.getNumOps();
            chan.channelNum = 2;
            oAnimChanList.push_back(chan);
        }

        oSample.addOp(op);
    }
}

// names need to be passed in x,y,z order, iOrder is the order to
// use these indices
void addRotate(const MFnDependencyNode & iTrans,
    MString parentName, const MString* iNames, const unsigned int* iOrder,
    Alembic::Util::uint8_t iHint, bool forceStatic, bool forceAnimated,
    Alembic::AbcGeom::XformSample & oSample,
    std::vector < AnimChan > & oAnimChanList,
    size_t oOpIndex[3])
{
    // for each possible rotation axis
    static const Alembic::AbcGeom::XformOperationType rots[3] = {
         Alembic::AbcGeom::kRotateXOperation,
         Alembic::AbcGeom::kRotateYOperation,
         Alembic::AbcGeom::kRotateZOperation
    };

    // this is to handle the case where there is a connection to the parent
    // plug but not to the child plugs, if the connection is there then all
    // of the children are considered animated
    MPlug parentPlug = iTrans.findPlug(parentName, true);
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
        MPlug plug = iTrans.findPlug(iNames[index], true);
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


        Alembic::AbcGeom::XformOp op(rots[index], iHint);
        op.setChannelValue(0, Alembic::AbcGeom::RadiansToDegrees(plugVal));

        // the sampled case
        if (samp != 0)
        {
            AnimChan chan;
            chan.plug = plug;
            chan.scale = Alembic::AbcGeom::RadiansToDegrees(1.0);
            chan.opNum = oSample.getNumOps();
            chan.channelNum = 0;
            oAnimChanList.push_back(chan);
        }
        // non sampled, XYZ axis and the angle is 0, do not add to the stack
        else if (isXYZ && plugVal == 0.0)
            continue;

        oOpIndex[index] = oSample.addOp(op);
    }
}

// the test on whether or not to add it is very similiar to addTranslate
// but the operation it creates is very different
void addShear(const MFnDependencyNode & iTrans, bool forceStatic,
    Alembic::AbcGeom::XformSample & oSample,
    std::vector < AnimChan > & oAnimChanList)
{
    Alembic::AbcGeom::XformOp op(Alembic::AbcGeom::kMatrixOperation,
        Alembic::AbcGeom::kMayaShearHint);

    MString str = "shearXY";
    MPlug xyPlug = iTrans.findPlug(str, true);
    int xySamp = 0;
    if (!forceStatic)
    {
        xySamp = util::getSampledType(xyPlug);
    }
    double xyVal = xyPlug.asDouble();

    str = "shearXZ";
    MPlug xzPlug = iTrans.findPlug(str, true);
    int xzSamp = 0;
    if (!forceStatic)
    {
        xzSamp = util::getSampledType(xzPlug);
    }
    double xzVal = xzPlug.asDouble();

    str = "shearYZ";
    MPlug yzPlug = iTrans.findPlug(str, true);
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
    MPlug parentPlug = iTrans.findPlug(str, true);
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
        Alembic::Abc::M44d m;
        m.makeIdentity();
        op.setMatrix(m);
        op.setChannelValue(4, xyVal);
        op.setChannelValue(8, xzVal);
        op.setChannelValue(9, yzVal);

        if (xySamp != 0)
        {
            AnimChan chan;
            chan.plug = xyPlug;
            chan.scale = 1.0;
            chan.opNum = oSample.getNumOps();
            chan.channelNum = 4;
            oAnimChanList.push_back(chan);
        }

        if (xzSamp != 0)
        {
            AnimChan chan;
            chan.plug = xzPlug;
            chan.scale = 1.0;
            chan.opNum = oSample.getNumOps();
            chan.channelNum = 8;
            oAnimChanList.push_back(chan);
        }

        if (yzSamp != 0)
        {
            AnimChan chan;
            chan.plug = yzPlug;
            chan.scale = 1.0;
            chan.opNum = oSample.getNumOps();
            chan.channelNum = 9;
            oAnimChanList.push_back(chan);
        }

        oSample.addOp(op);
    }
}

// this test is very similiar to addTranslate, except that it doesn't add it
// to the stack if x,y, and z are 1.0
void addScale(const MFnDependencyNode & iTrans,
    MString parentName, MString xName, MString yName, MString zName, bool inverse,
    bool forceStatic, bool forceAnimated, Alembic::AbcGeom::XformSample & oSample,
    std::vector < AnimChan > & oAnimChanList)
{

    Alembic::AbcGeom::XformOp op(Alembic::AbcGeom::kScaleOperation,
        Alembic::AbcGeom::kScaleHint);

    MPlug xPlug = iTrans.findPlug(xName, true);
    int xSamp = 0;
    if (!forceStatic)
    {
        if (!forceAnimated)
            xSamp = util::getSampledType(xPlug);
        else
            xSamp = 1;
    }
    double xVal = xPlug.asDouble();

    MPlug yPlug = iTrans.findPlug(yName, true);
    int ySamp = 0;
    if (!forceStatic)
    {
        if (!forceAnimated)
            ySamp = util::getSampledType(yPlug);
        else
            ySamp = 1;
    }
    double yVal = yPlug.asDouble();

    MPlug zPlug = iTrans.findPlug(zName, true);
    int zSamp = 0;
    if (!forceStatic)
    {
        if (!forceAnimated)
            zSamp = util::getSampledType(zPlug);
        else
            zSamp = 1;
    }
    double zVal = zPlug.asDouble();

    // this is to handle the case where there is a connection to the parent
    // plug but not to the child plugs, if the connection is there then all
    // of the children are considered animated
    MPlug parentPlug = iTrans.findPlug(parentName, true);
    int parentSamp = 0;
    if (!forceStatic)
    {
        if (!forceAnimated)
            parentSamp = util::getSampledType(parentPlug);
        else
            parentSamp = 1;
    }

    if (parentSamp != 0)
    {
        xSamp = 1;
        ySamp = 1;
        zSamp = 1;
    }

    // something is sampled or not identity, add it to the stack
    if (xSamp != 0 || ySamp != 0 || zSamp != 0 || xVal != 1.0 || yVal != 1.0 ||
        zVal != 1.0)
    {
        if (inverse)
        {
            xVal = util::inverseScale(xVal);
            yVal = util::inverseScale(yVal);
            zVal = util::inverseScale(zVal);
        }

        op.setChannelValue(0, xVal);
        op.setChannelValue(1, yVal);
        op.setChannelValue(2, zVal);

        if (xSamp != 0)
        {
            AnimChan chan;
            chan.plug = xPlug;
            chan.scale = 1.0;
            if (inverse)
                chan.scale = -std::numeric_limits<double>::infinity();
            chan.opNum = oSample.getNumOps();
            chan.channelNum = 0;
            oAnimChanList.push_back(chan);
        }

        if (ySamp != 0)
        {
            AnimChan chan;
            chan.plug = yPlug;
            chan.scale = 1.0;
            if (inverse)
                chan.scale = -std::numeric_limits<double>::infinity();
            chan.opNum = oSample.getNumOps();
            chan.channelNum = 1;
            oAnimChanList.push_back(chan);
        }

        if (zSamp != 0)
        {
            AnimChan chan;
            chan.plug = zPlug;
            chan.scale = 1.0;
            if (inverse)
                chan.scale = -std::numeric_limits<double>::infinity();
            chan.opNum = oSample.getNumOps();
            chan.channelNum = 2;
            oAnimChanList.push_back(chan);
        }

        oSample.addOp(op);
    }
}

bool getSampledRotation(const Alembic::AbcGeom::XformSample& sample,
    const size_t opIndex[3], double& xx, double& yy, double& zz)
{
    bool success = false;

    xx = 0.0;
    if (opIndex[0] < sample.getNumOps())
    {
        double angleX = sample[opIndex[0]].getChannelValue(0);
        xx = Alembic::AbcGeom::DegreesToRadians(angleX);
        success = true;
    }

    yy = 0.0;
    if (opIndex[1] < sample.getNumOps())
    {
        double angleY = sample[opIndex[1]].getChannelValue(0);
        yy = Alembic::AbcGeom::DegreesToRadians(angleY);
        success = true;
    }

    zz = 0.0;
    if (opIndex[2] < sample.getNumOps())
    {
        double angleZ = sample[opIndex[2]].getChannelValue(0);
        zz = Alembic::AbcGeom::DegreesToRadians(angleZ);
        success = true;
    }

    return success;
}

bool setSampledRotation(Alembic::AbcGeom::XformSample& sample,
    const size_t opIndex[3], double xx, double yy, double zz)
{
    bool success = false;

    if (opIndex[0] < sample.getNumOps())
    {
        sample[opIndex[0]].setChannelValue(0, Alembic::AbcGeom::RadiansToDegrees(xx));
        success = true;
    }

    if (opIndex[1] < sample.getNumOps())
    {
        sample[opIndex[1]].setChannelValue(0, Alembic::AbcGeom::RadiansToDegrees(yy));
        success = true;
    }

    if (opIndex[2] < sample.getNumOps())
    {
        sample[opIndex[2]].setChannelValue(0, Alembic::AbcGeom::RadiansToDegrees(zz));
        success = true;
    }

    return success;
}

// parented to the Alembic root, has extra logic for writing the rest of the
// worldspace of the Maya DAG
MayaTransformWriter::MayaTransformWriter(Alembic::AbcGeom::OObject & iParent,
    MDagPath & iDag, Alembic::Util::uint32_t iTimeIndex, const JobArgs & iArgs)
{
    mFilterEulerRotations = iArgs.filterEulerRotations;
    mJointOrientOpIndex[0] = mJointOrientOpIndex[1] = mJointOrientOpIndex[2] =
    mRotateOpIndex[0]      = mRotateOpIndex[1]      = mRotateOpIndex[2]      =
    mRotateAxisOpIndex[0]  = mRotateAxisOpIndex[1]  = mRotateAxisOpIndex[2]  = ~size_t(0);

    MFnDagNode dagNode(iDag);
    MString name = dagNode.name();

    name = util::stripNamespaces(name, iArgs.stripNamespace);

    bool hasAttrs = AttributesWriter::hasAnyAttr(dagNode, iArgs);

    Alembic::Abc::OObject obj;
    Alembic::Abc::OCompoundProperty cp;
    Alembic::Abc::OCompoundProperty up;

    if (!iArgs.writeTransforms)
    {
        obj = Alembic::Abc::OObject (iParent, name.asChar());
        mObject = obj;
        if (hasAttrs)
        {
            Alembic::Abc::OCompoundProperty xformProp(obj.getProperties(), ".xform");
            cp = Alembic::Abc::OCompoundProperty(xformProp, ".arbGeomParams");
            up = Alembic::Abc::OCompoundProperty(xformProp, ".userProperties");
        }
    }
    else
    {
        Alembic::AbcGeom::OXform xform(iParent, name.asChar(), iTimeIndex);
        mSchema = xform.getSchema();
        obj = xform;
        if (hasAttrs)
        {
            cp = mSchema.getArbGeomParams();
            up = mSchema.getUserProperties();
        }
    }

    mAttrs = AttributesWriterPtr(new AttributesWriter(cp, up, obj, dagNode,
        iTimeIndex, iArgs, false));

    if (!iArgs.writeTransforms)
    {
        return;
    }

    // build  up our joint stack
    if (iDag.hasFn(MFn::kJoint) && !iArgs.worldSpace)
    {
        MFnIkJoint joint(iDag);
        pushTransformStack(joint, iTimeIndex == 0);
    }
    // build up the normal xform stack
    else if (!iArgs.worldSpace)
    {
        MFnTransform trans(iDag);
        pushTransformStack(trans, iTimeIndex == 0);
    }

    if (!iArgs.worldSpace)
    {
        MPlug inheritPlug = dagNode.findPlug("inheritsTransform", true);
        if (!inheritPlug.isNull())
        {
            if (util::getSampledType(inheritPlug) != 0)
            {
                mInheritsPlug = inheritPlug;
            }
            mSample.setInheritsXforms(inheritPlug.asBool());
        }

        // no animated inherits plug and no animated samples?
        // then use the default time sampling
        if (mAnimChanList.empty() && mInheritsPlug.isNull())
        {
            mSchema.setTimeSampling(0);
        }

        // everything is default, don't write anything
        if (mSample.getNumOps() == 0 && mSample.getInheritsXforms())
        {
            return;
        }

        mSchema.set(mSample);
        return;
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
        MPlug inheritPlug = dagNode.findPlug("inheritsTransform", true);

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
            pushTransformStack(joint, iTimeIndex == 0);
        }
        else
        {
            MFnTransform trans(*iCur);
            pushTransformStack(trans, iTimeIndex == 0);
        }
    }

    // finally add any transform info on the final node and write it
    if (iCur->hasFn(MFn::kJoint))
    {
        MFnIkJoint joint(*iCur);
        pushTransformStack(joint, iTimeIndex == 0);
    }
    else
    {
        MFnTransform trans(*iCur);
        pushTransformStack(trans, iTimeIndex == 0);
    }

    // need to look at inheritsTransform
    MPlug inheritPlug = dagNode.findPlug("inheritsTransform", true);
    if (!inheritPlug.isNull())
    {
        if (util::getSampledType(inheritPlug) != 0)
        {
            mInheritsPlug = inheritPlug;
        }
        mSample.setInheritsXforms(inheritPlug.asBool());
    }

    // no animated inherits plug and no animated samples?
    // then use the default time sampling
    if (mAnimChanList.empty() && mInheritsPlug.isNull())
    {
        mSchema.setTimeSampling(0);
    }

    // everything is default, don't write anything and use the default
    // time sampling
    if (mSample.getNumOps() == 0 && mSample.getInheritsXforms())
    {
        return;
    }

    mSchema.set(mSample);

}

// this is the normal way for writing just the local xform
MayaTransformWriter::MayaTransformWriter(MayaTransformWriter & iParent,
    MDagPath & iDag, Alembic::Util::uint32_t iTimeIndex, const JobArgs & iArgs)
{
    mFilterEulerRotations = iArgs.filterEulerRotations;
    mJointOrientOpIndex[0] = mJointOrientOpIndex[1] = mJointOrientOpIndex[2] =
    mRotateOpIndex[0]      = mRotateOpIndex[1]      = mRotateOpIndex[2]      =
    mRotateAxisOpIndex[0]  = mRotateAxisOpIndex[1]  = mRotateAxisOpIndex[2]  = ~size_t(0);

    MFnDagNode dagNode(iDag);
    MString name = dagNode.name();

    name = util::stripNamespaces(name, iArgs.stripNamespace);

    bool hasAttrs = AttributesWriter::hasAnyAttr(dagNode, iArgs);

    Alembic::Abc::OObject obj;
    Alembic::Abc::OCompoundProperty cp;
    Alembic::Abc::OCompoundProperty up;

    if (!iArgs.writeTransforms)
    {
        obj = Alembic::Abc::OObject (iParent.getObject(), name.asChar());
        mObject = obj;
        if (hasAttrs)
        {
            Alembic::Abc::OCompoundProperty xformProp(obj.getProperties(), ".xform");
            cp = Alembic::Abc::OCompoundProperty(xformProp, ".arbGeomParams");
            up = Alembic::Abc::OCompoundProperty(xformProp, ".userProperties");
        }
    }
    else
    {
        Alembic::AbcGeom::OXform xform(iParent.getObject(), name.asChar(), iTimeIndex);
        obj = xform;
        mSchema = xform.getSchema();
        if (hasAttrs)
        {
            cp = mSchema.getArbGeomParams();
            up = mSchema.getUserProperties();
        }
    }

    mAttrs = AttributesWriterPtr(new AttributesWriter(cp, up, obj, dagNode,
        iTimeIndex, iArgs, false));

    if (!iArgs.writeTransforms)
    {
        return;
    }

    // build  up our joint stack
    if (iDag.hasFn(MFn::kJoint))
    {
        MFnIkJoint joint(iDag);
        pushTransformStack(joint, iTimeIndex == 0);
    }
    // build up the normal xform stack
    else
    {
        MFnTransform trans(iDag);
        pushTransformStack(trans, iTimeIndex == 0);
    }

    MPlug inheritPlug = dagNode.findPlug("inheritsTransform", true);
    if (!inheritPlug.isNull())
    {
        if (util::getSampledType(inheritPlug) != 0)
        {
            mInheritsPlug = inheritPlug;
        }
        mSample.setInheritsXforms(inheritPlug.asBool());
    }

    // no animated inherits plug and no animated samples?
    // then use the default time sampling
    if (mAnimChanList.empty() && mInheritsPlug.isNull())
    {
        mSchema.setTimeSampling(0);
    }

    // everything is default, don't write anything
    if (mSample.getNumOps() == 0 && mSample.getInheritsXforms())
    {
        return;
    }

    mSchema.set(mSample);
}


MayaTransformWriter::~MayaTransformWriter()
{
}

void MayaTransformWriter::write()
{
    size_t numSamples = mAnimChanList.size();
    if (numSamples > 0)
    {
        std::vector < AnimChan >::iterator it, itEnd;

        for (it = mAnimChanList.begin(), itEnd = mAnimChanList.end();
            it != itEnd; ++it)
        {
            double val = it->plug.asDouble();

            if (it->scale == -std::numeric_limits<double>::infinity())
                val = util::inverseScale(val);
            else if (it->scale != 1.0)
                val *= it->scale;

            mSample[it->opNum].setChannelValue(it->channelNum, val);
        }

        if (!mInheritsPlug.isNull())
        {
            mSample.setInheritsXforms(mInheritsPlug.asBool());
        }

        if (mFilterEulerRotations)
        {
            double xx(0), yy(0), zz(0);

            if (getSampledRotation(mSample, mJointOrientOpIndex, xx, yy, zz))
            {
                MEulerRotation euler(xx, yy, zz, mPrevJointOrientSolution.order);
                euler.setToClosestSolution(mPrevJointOrientSolution);

                // update sample with new solution
                setSampledRotation(mSample, mJointOrientOpIndex, euler.x, euler.y, euler.z);
                mPrevJointOrientSolution = euler;
            }

            if (getSampledRotation(mSample, mRotateOpIndex, xx, yy, zz))
            {
                MEulerRotation euler(xx, yy, zz, mPrevRotateSolution.order);
                euler.setToClosestSolution(mPrevRotateSolution);

                // update sample with new solution
                setSampledRotation(mSample, mRotateOpIndex, euler.x, euler.y, euler.z);
                mPrevRotateSolution = euler;
            }

            if (getSampledRotation(mSample, mRotateAxisOpIndex, xx, yy, zz))
            {
                MEulerRotation euler(xx, yy, zz, mPrevRotateAxisSolution.order);
                euler.setToClosestSolution(mPrevRotateAxisSolution);

                // update sample with new solution
                setSampledRotation(mSample, mRotateAxisOpIndex, euler.x, euler.y, euler.z);
                mPrevRotateAxisSolution = euler;
            }
        }

        mSchema.set(mSample);
    }
}

bool MayaTransformWriter::isAnimated() const
{
    return mAnimChanList.size() > 0 || !mInheritsPlug.isNull();
}

Alembic::Abc::OObject MayaTransformWriter::getObject()
{
    if (mObject.valid())
    {
        return mObject;
    }

    return mSchema.getObject();

}

void MayaTransformWriter::pushTransformStack(const MFnTransform & iTrans,
    bool iForceStatic)
{

    // inspect the translate
    addTranslate(iTrans, "translate", "translateX", "translateY", "translateZ",
        Alembic::AbcGeom::kTranslateHint, false, iForceStatic, false, mSample,
        mAnimChanList);


    // inspect the rotate pivot translate
    addTranslate(iTrans, "rotatePivotTranslate", "rotatePivotTranslateX",
        "rotatePivotTranslateY", "rotatePivotTranslateZ",
        Alembic::AbcGeom::kRotatePivotTranslationHint, false,
            iForceStatic, false, mSample, mAnimChanList);

    // inspect the rotate pivot
    addTranslate(iTrans, "rotatePivot", "rotatePivotX", "rotatePivotY",
        "rotatePivotZ",  Alembic::AbcGeom::kRotatePivotPointHint,
        false, iForceStatic, false, mSample, mAnimChanList);

    // inspect rotate names
    MString rotateNames[3];
    rotateNames[0] = "rotateX";
    rotateNames[1] = "rotateY";
    rotateNames[2] = "rotateZ";

    unsigned int rotOrder[3];

    // if this returns false then the rotation order was kInvalid or kLast
    MTransformationMatrix::RotationOrder eRotOrder(iTrans.rotationOrder());
    if (util::getRotOrder(eRotOrder, rotOrder[0], rotOrder[1],
        rotOrder[2]))
    {
        addRotate(iTrans, "rotate", rotateNames, rotOrder,
            Alembic::AbcGeom::kRotateHint, iForceStatic, false,
            mSample, mAnimChanList, mRotateOpIndex);
    }

    // now look at the rotation orientation, aka rotate axis
    rotateNames[0] = "rotateAxisX";
    rotateNames[1] = "rotateAxisY";
    rotateNames[2] = "rotateAxisZ";
    rotOrder[0] = 0;
    rotOrder[1] = 1;
    rotOrder[2] = 2;
    addRotate(iTrans, "rotateAxis", rotateNames, rotOrder,
        Alembic::AbcGeom::kRotateOrientationHint, iForceStatic, false,
        mSample, mAnimChanList, mRotateAxisOpIndex);

    // invert the rotate pivot if necessary
    addTranslate(iTrans, "rotatePivot", "rotatePivotX", "rotatePivotY",
        "rotatePivotZ", Alembic::AbcGeom::kRotatePivotPointHint,
        true, iForceStatic, false, mSample, mAnimChanList);

    // inspect the scale pivot translation
    addTranslate(iTrans, "scalePivotTranslate", "scalePivotTranslateX",
        "scalePivotTranslateY", "scalePivotTranslateZ",
        Alembic::AbcGeom::kScalePivotTranslationHint, false, iForceStatic,
        false, mSample, mAnimChanList);

    // inspect the scale pivot point
    addTranslate(iTrans, "scalePivot", "scalePivotX", "scalePivotY",
        "scalePivotZ", Alembic::AbcGeom::kScalePivotPointHint, false,
        iForceStatic, false, mSample, mAnimChanList);

    // inspect the shear
    addShear(iTrans, iForceStatic, mSample, mAnimChanList);

    // add the scale
    addScale(iTrans, "scale", "scaleX", "scaleY", "scaleZ", false,
        iForceStatic, false, mSample, mAnimChanList);

    // inverse the scale pivot point if necessary
    addTranslate(iTrans, "scalePivot", "scalePivotX", "scalePivotY",
        "scalePivotZ", Alembic::AbcGeom::kScalePivotPointHint, true,
        iForceStatic, false, mSample, mAnimChanList);

    // remember current rotation
    if (mFilterEulerRotations)
    {
        double xx(0), yy(0), zz(0);

        // there are 2 rotation order enum definitions:
        //     MEulerRotation::RotationOrder = MTransformationMatrix::RotationOrder-1
        if (getSampledRotation( mSample, mRotateOpIndex, xx, yy, zz ))
        {
            mPrevRotateSolution.setValue(xx, yy, zz, (MEulerRotation::RotationOrder)(eRotOrder-1));
        }

        if (getSampledRotation( mSample, mRotateAxisOpIndex, xx, yy, zz ))
        {
            mPrevRotateAxisSolution.setValue(xx, yy, zz, MEulerRotation::kXYZ);
        }
    }

}

void MayaTransformWriter::pushTransformStack(const MFnIkJoint & iJoint,
    bool iForceStatic)
{
    // Some special cases that the joint is animated but has no input connections.
    bool forceAnimated = util::isDrivenByFBIK(iJoint) || util::isDrivenBySplineIK(iJoint);

    // inspect the translate
    addTranslate(iJoint, "translate", "translateX", "translateY", "translateZ",
        Alembic::AbcGeom::kTranslateHint, false, iForceStatic, forceAnimated,
        mSample, mAnimChanList);

    // inspect the inverseParent scale
    // [IS] is ignored when Segment Scale Compensate is false
    MPlug scaleCompensatePlug = iJoint.findPlug("segmentScaleCompensate", true);
    if (scaleCompensatePlug.asBool())
    {
        addScale(iJoint, "inverseScale", "inverseScaleX", "inverseScaleY",
            "inverseScaleZ", true, iForceStatic, forceAnimated, mSample, mAnimChanList);
    }

    MTransformationMatrix::RotationOrder eJointOrientOrder, eRotOrder, eRotateAxisOrder;
    double vals[3];

    // for reordering rotate names
    MString rotateNames[3];
    unsigned int rotOrder[3];

    // now look at the joint orientation
    rotateNames[0] = "jointOrientX";
    rotateNames[1] = "jointOrientY";
    rotateNames[2] = "jointOrientZ";

    iJoint.getOrientation(vals, eJointOrientOrder);
    if (util::getRotOrder(eJointOrientOrder, rotOrder[0], rotOrder[1], rotOrder[2]))
    {
        addRotate(iJoint, "jointOrient", rotateNames, rotOrder,
            Alembic::AbcGeom::kRotateHint, iForceStatic, true,
            mSample, mAnimChanList, mJointOrientOpIndex);
    }

    rotateNames[0] = "rotateX";
    rotateNames[1] = "rotateY";
    rotateNames[2] = "rotateZ";

    // if this returns false then the rotation order was kInvalid or kLast
    eRotOrder = iJoint.rotationOrder();
    if (util::getRotOrder(eRotOrder, rotOrder[0], rotOrder[1],
        rotOrder[2]))
    {
        addRotate(iJoint, "rotate", rotateNames, rotOrder,
            Alembic::AbcGeom::kRotateHint, iForceStatic, true,
            mSample, mAnimChanList, mRotateOpIndex);
    }

    // now look at the rotation orientation, aka rotate axis
    rotateNames[0] = "rotateAxisX";
    rotateNames[1] = "rotateAxisY";
    rotateNames[2] = "rotateAxisZ";

    iJoint.getScaleOrientation(vals, eRotateAxisOrder);
    if (util::getRotOrder(eRotateAxisOrder, rotOrder[0], rotOrder[1], rotOrder[2]))
    {
        addRotate(iJoint, "rotateAxis", rotateNames, rotOrder,
            Alembic::AbcGeom::kRotateOrientationHint, iForceStatic, true,
            mSample, mAnimChanList, mRotateAxisOpIndex);
    }

    // inspect the scale
    addScale(iJoint, "scale", "scaleX", "scaleY", "scaleZ", false,
        iForceStatic, forceAnimated, mSample, mAnimChanList);

    // remember current rotation
    if (mFilterEulerRotations)
    {
        double xx(0), yy(0), zz(0);

        // there are 2 rotation order enum definitions:
        //     MEulerRotation::RotationOrder = MTransformationMatrix::RotationOrder-1
        if (getSampledRotation( mSample, mJointOrientOpIndex, xx, yy, zz ))
        {
            mPrevJointOrientSolution.setValue(xx, yy, zz, (MEulerRotation::RotationOrder)(eJointOrientOrder-1));
        }

        if (getSampledRotation( mSample, mRotateOpIndex, xx, yy, zz ))
        {
            mPrevRotateSolution.setValue(xx, yy, zz, (MEulerRotation::RotationOrder)(eRotOrder-1));
        }

        if (getSampledRotation( mSample, mRotateAxisOpIndex, xx, yy, zz ))
        {
            mPrevRotateAxisSolution.setValue(xx, yy, zz, (MEulerRotation::RotationOrder)(eRotateAxisOrder-1));
        }
    }
}
