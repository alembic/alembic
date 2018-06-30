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
#include "XformHelper.h"
#include "NodeIteratorVisitorHelper.h"

#include <Alembic/AbcGeom/IXform.h>

#include <maya/MObject.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnTransform.h>
#include <maya/MMatrix.h>
#include <maya/MPoint.h>
#include <maya/MQuaternion.h>
#include <maya/MString.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MVector.h>
#include <maya/MGlobal.h>
#include <maya/MDagModifier.h>

namespace
{
    // I'm not sure why kPreTransform works and kTransform does not for all
    // cases except when the transform is complicated and we need to use the
    // matrix.
    MSpace::Space gSpace = MSpace::kPreTransform;
    const bool gBalance = false;
};

// complex matrix doesn't map to Maya directly, so get the matrix
// and then try to break it down into Maya parts
void readComplex(double iFrame, Alembic::AbcGeom::IXform & iNode,
    std::vector<double> & oSampleList)
{
    Alembic::AbcCoreAbstract::index_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame,
        iNode.getSchema().getTimeSampling(),
        iNode.getSchema().getNumSamples(),
        index, ceilIndex);

    Alembic::Abc::M44d m; 

    if (alpha != 0.0 && index != ceilIndex)
    {
        Alembic::AbcGeom::XformSample samp;
        iNode.getSchema().get(samp, Alembic::Abc::ISampleSelector(index));
        Alembic::Abc::M44d mlo = samp.getMatrix();

        iNode.getSchema().get(samp, Alembic::Abc::ISampleSelector(ceilIndex));
        Alembic::Abc::M44d mhi = samp.getMatrix();

        m = ((1 - alpha) * mlo) + (alpha * mhi);
    }
    else
    {
        Alembic::AbcGeom::XformSample samp;
         iNode.getSchema().get(samp, Alembic::Abc::ISampleSelector(index));
        m = samp.getMatrix();
    }

    MTransformationMatrix mmat(MMatrix(m.x));

    // Everywhere else we use kPreTransform, but this doesn't work
    // when we pass in the matrix for some reason
    MSpace::Space tSpace = MSpace::kTransform;

    // push the results into sampleList
    MVector vec = mmat.getTranslation(tSpace);
    oSampleList.push_back(vec.x);
    oSampleList.push_back(vec.y);
    oSampleList.push_back(vec.z);

    vec = mmat.rotatePivotTranslation(tSpace);
    oSampleList.push_back(vec.x);
    oSampleList.push_back(vec.y);
    oSampleList.push_back(vec.z);

    MPoint pt = mmat.rotatePivot(tSpace);
    oSampleList.push_back(pt.x);
    oSampleList.push_back(pt.y);
    oSampleList.push_back(pt.z);

    double rot[3];
    MTransformationMatrix::RotationOrder order;
    mmat.getRotation(rot, order);
    oSampleList.push_back(Alembic::AbcGeom::RadiansToDegrees(rot[0]));
    oSampleList.push_back(Alembic::AbcGeom::RadiansToDegrees(rot[1]));
    oSampleList.push_back(Alembic::AbcGeom::RadiansToDegrees(rot[2]));

    MQuaternion quat = mmat.rotationOrientation();
    vec = quat.asEulerRotation().asVector();
    oSampleList.push_back(Alembic::AbcGeom::RadiansToDegrees(vec.x));
    oSampleList.push_back(Alembic::AbcGeom::RadiansToDegrees(vec.y));
    oSampleList.push_back(Alembic::AbcGeom::RadiansToDegrees(vec.z));

    pt = mmat.scalePivotTranslation(tSpace);
    oSampleList.push_back(vec.x);
    oSampleList.push_back(vec.y);
    oSampleList.push_back(vec.z);

    vec = mmat.scalePivot(tSpace);
    oSampleList.push_back(vec.x);
    oSampleList.push_back(vec.y);
    oSampleList.push_back(vec.z);

    double shear[3];
    mmat.getShear(shear, tSpace);
    oSampleList.push_back(shear[0]);
    oSampleList.push_back(shear[1]);
    oSampleList.push_back(shear[2]);

    double scale[3];
    mmat.getScale(scale, tSpace);
    oSampleList.push_back(scale[0]);
    oSampleList.push_back(scale[1]);
    oSampleList.push_back(scale[2]);
}

void read(double iFrame, Alembic::AbcGeom::IXform & iNode,
    std::vector<double> & oSampleList,
    Alembic::AbcGeom::XformSample & oSamp)
{
    Alembic::AbcCoreAbstract::index_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame,
        iNode.getSchema().getTimeSampling(),
        iNode.getSchema().getNumSamples(), index, ceilIndex);

    if (alpha != 0 && index != ceilIndex &&
        !iNode.getSchema().isConstant())
    {
        iNode.getSchema().get(oSamp, Alembic::Abc::ISampleSelector(index));

        Alembic::AbcGeom::XformSample hiSamp;
            iNode.getSchema().get(hiSamp,
                Alembic::Abc::ISampleSelector(ceilIndex));

        // loop over the operations and channels and add only the animated
        // values
        std::vector<double> loVec, hiVec;
        size_t numOps = oSamp.getNumOps();
        for (size_t i = 0; i < numOps; ++i)
        {
            Alembic::AbcGeom::XformOp loOp = oSamp[i];
            Alembic::AbcGeom::XformOp hiOp = hiSamp[i];
            size_t numChannels = loOp.getNumChannels();
            for (size_t j = 0; j < numChannels; ++j)
            {
                if (loOp.isChannelAnimated(j))
                {
                    loVec.push_back(loOp.getChannelValue(j));
                    hiVec.push_back(hiOp.getChannelValue(j));
                }
            }
        }

        size_t numAnim = loVec.size();
        oSampleList.resize(numAnim);
        for (size_t i = 0; i < numAnim; ++i)
        {
            oSampleList[i] = simpleLerp<double>(alpha,loVec[i], hiVec[i]);
        }
    }
    else
    {
        iNode.getSchema().get(oSamp, Alembic::Abc::ISampleSelector(index));

        oSampleList.clear();
        // add only animated values
        size_t numOps = oSamp.getNumOps();
        for (size_t i = 0; i < numOps; ++i)
        {
            Alembic::AbcGeom::XformOp op = oSamp[i];

            size_t numChannels = op.getNumChannels();
            for (size_t j = 0; j < numChannels; ++j)
            {
                if (op.isChannelAnimated(j))
                {
                    oSampleList.push_back(op.getChannelValue(j));
                }
            }
        }
    }
}

bool isComplex(const Alembic::AbcGeom::XformSample & iSamp)
{
    int state = 0;

    bool scPivot = false;
    bool roPivot = false;
    bool xAxis = false;
    bool yAxis = false;
    bool zAxis = false;

    size_t numOps = iSamp.getNumOps();
    for (size_t i = 0; i < numOps; ++i)
    {
        Alembic::AbcGeom::XformOp op = iSamp[i];
        switch (op.getType())
        {
            case Alembic::AbcGeom::kScaleOperation:
            {
                if (state < 12)
                {
                    state = 12;
                }
                else
                    return true;
            }
            break;

            case Alembic::AbcGeom::kTranslateOperation:
            {
                switch (op.getHint())
                {
                    case Alembic::AbcGeom::kTranslateHint:
                    {
                        if (state < 1)
                            state = 1;
                        else
                            return true;
                    }
                    break;

                    case Alembic::AbcGeom::kScalePivotPointHint:
                    {
                        if (state < 10 && !scPivot)
                        {
                            scPivot = true;
                            state = 10;
                        }
                        // we have encounted this pivot before,
                        // this one undoes the first one
                        else if (state < 13 && scPivot)
                        {
                            scPivot = false;
                            state = 13;
                        }
                        else
                            return true;
                    }
                    break;

                    case Alembic::AbcGeom::kScalePivotTranslationHint:
                    {
                        if (state < 9)
                            state = 9;
                        else
                            return true;
                    }
                    break;

                    case Alembic::AbcGeom::kRotatePivotPointHint:
                    {
                        if (state < 3 && !roPivot)
                        {
                            roPivot = true;
                            state = 3;
                        }
                        // we have encounted this pivot before,
                        // this one undoes the first one
                        else if (state < 8 && roPivot)
                        {
                            roPivot = false;
                            state = 8;
                        }
                        else
                            return true;
                    }
                    break;

                    case Alembic::AbcGeom::kRotatePivotTranslationHint:
                    {
                        if (state < 2)
                            state = 2;
                        else
                            return true;
                    }
                    break;

                    default:
                        return true;
                    break;
                }

            }
            break;

            case Alembic::AbcGeom::kRotateXOperation:
            case Alembic::AbcGeom::kRotateYOperation:
            case Alembic::AbcGeom::kRotateZOperation:
            case Alembic::AbcGeom::kRotateOperation:
            {
                // if any axis is animated, we have a complex xform
                if (op.isXAnimated() || op.isYAnimated() || op.isZAnimated())
                    return true;

                Alembic::Abc::V3d v = op.getAxis();

                switch (op.getHint())
                {
                    case Alembic::AbcGeom::kRotateHint:
                    {
                        if (v.x == 1 && v.y == 0 && v.z == 0 &&
                            !xAxis && state <= 4)
                        {
                            state = 4;
                            xAxis = true;
                        }
                        else if (v.x == 0 && v.y == 1 && v.z == 0 && !yAxis && 
                            state <= 4)
                        {
                            state = 4;
                            yAxis = true;
                        }
                        else if (v.x == 0 && v.y == 0 && v.z == 1 && !zAxis && 
                            state <= 4)
                        {
                            state = 4;
                            zAxis = true;
                        }
                        else
                            return true;
                    }
                    break;

                    case Alembic::AbcGeom::kRotateOrientationHint:
                    {
                        if (v.x == 1 && v.y == 0 && v.z == 0 && state < 7)
                        {
                            state = 7;
                        }
                        else if (v.x == 0 && v.y == 1 && v.z == 0 && state < 6)
                        {
                            state = 6;
                        }
                        else if (v.x == 0 && v.y == 0 && v.z == 1 && state < 5)
                        {
                            state = 5;
                        }
                        else
                            return true;
                    }
                    break;

                    default:
                        return true;
                    break;
                }
            }
            break;

            case Alembic::AbcGeom::kMatrixOperation:
            {
                if (op.getHint() == Alembic::AbcGeom::kMayaShearHint &&
                    state < 11)
                {
                    state = 11;
                }
                else
                    return true;
            }
            break;

            default:
            {
                return true;
            }
            break;
        } // switch

    } // for i

    // if we encounter a scale or rotate pivot point,
    // it needs to be inverted again otherwise the matrix is complex
    return scPivot || roPivot;
}

MStatus connectToXform(const Alembic::AbcGeom::XformSample & iSamp,
    bool isConstant,
    MObject & iObject,
    std::vector<std::string> & oSampledTransOpNameList,
    std::vector<Prop> & iSampledPropList,
    std::size_t iFirstProp)
{
    MStatus status = MS::kSuccess;

    MFnTransform trans;
    trans.setObject(iObject);

    // disconnect and reset all the native attributes

    const MVector   vec(0, 0, 0);
    const MPoint    point(0, 0, 0);
    const double    zero[3] = {0, 0, 0};

    MPlug dstPlug;

    dstPlug = trans.findPlug("scalePivotX", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("scalePivotY", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("scalePivotZ", true);
    disconnectAllPlugsTo(dstPlug);
    trans.setScalePivot(point, gSpace, gBalance);

    dstPlug = trans.findPlug("scaleX", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("scaleY", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("scaleZ", true);
    disconnectAllPlugsTo(dstPlug);
    const double scale[3] = {1, 1, 1};
    trans.setScale(scale);

    dstPlug = trans.findPlug("shearXY", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("shearXZ", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("shearYZ", true);
    disconnectAllPlugsTo(dstPlug);
    trans.setShear(zero);

    dstPlug = trans.findPlug("scalePivotTranslateX", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("scalePivotTranslateY", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("scalePivotTranslateZ", true);
    disconnectAllPlugsTo(dstPlug);
    trans.setScalePivotTranslation(vec, gSpace);

    dstPlug = trans.findPlug("rotatePivotX", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("rotatePivotY", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("rotatePivotZ", true);
    disconnectAllPlugsTo(dstPlug);
    trans.setRotatePivot(point, gSpace, gBalance);

    dstPlug = trans.findPlug("rotateAxisX", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("rotateAxisY", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("rotateAxisZ", true);
    disconnectAllPlugsTo(dstPlug);
    const MQuaternion quat;
    trans.setRotateOrientation(quat, gSpace, gBalance);

    MTransformationMatrix::RotationOrder order =
        MTransformationMatrix::kXYZ;

    dstPlug = trans.findPlug("rotateOrder", true);
    disconnectAllPlugsTo(dstPlug);
    bool reorder = true;
    trans.setRotationOrder(order, reorder);

    dstPlug = trans.findPlug("rotateX", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("rotateY", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("rotateZ", true);
    disconnectAllPlugsTo(dstPlug);
    trans.setRotation(zero, order);

    dstPlug = trans.findPlug("rotatePivotTranslateX", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("rotatePivotTranslateY", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("rotatePivotTranslateZ", true);
    disconnectAllPlugsTo(dstPlug);
    trans.setRotatePivotTranslation(vec, gSpace);

    dstPlug = trans.findPlug("translateX", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("translateY", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug = trans.findPlug("translateZ", true);
    disconnectAllPlugsTo(dstPlug);
    trans.setTranslation(vec, gSpace);

    dstPlug = trans.findPlug("inheritsTransform", true);
    disconnectAllPlugsTo(dstPlug);
    dstPlug.setBool(iSamp.getInheritsXforms());

    // disconnect connections to animated props
    disconnectProps(trans, iSampledPropList, iFirstProp);

    if (isComplex(iSamp))
    {
        if (!isConstant)
        {
            oSampledTransOpNameList.push_back("translateX");
            oSampledTransOpNameList.push_back("translateY");
            oSampledTransOpNameList.push_back("translateZ");
            oSampledTransOpNameList.push_back("rotatePivotTranslateX");
            oSampledTransOpNameList.push_back("rotatePivotTranslateY");
            oSampledTransOpNameList.push_back("rotatePivotTranslateZ");
            oSampledTransOpNameList.push_back("rotatePivotX");
            oSampledTransOpNameList.push_back("rotatePivotY");
            oSampledTransOpNameList.push_back("rotatePivotZ");
            oSampledTransOpNameList.push_back("rotateX");
            oSampledTransOpNameList.push_back("rotateY");
            oSampledTransOpNameList.push_back("rotateZ");
            oSampledTransOpNameList.push_back("rotateAxisX");
            oSampledTransOpNameList.push_back("rotateAxisY");
            oSampledTransOpNameList.push_back("rotateAxisZ");
            oSampledTransOpNameList.push_back("scalePivotTranslateX");
            oSampledTransOpNameList.push_back("scalePivotTranslateY");
            oSampledTransOpNameList.push_back("scalePivotTranslateZ");
            oSampledTransOpNameList.push_back("scalePivotX");
            oSampledTransOpNameList.push_back("scalePivotY");
            oSampledTransOpNameList.push_back("scalePivotZ");
            oSampledTransOpNameList.push_back("shearXY");
            oSampledTransOpNameList.push_back("shearXZ");
            oSampledTransOpNameList.push_back("shearYZ");
            oSampledTransOpNameList.push_back("scaleX");
            oSampledTransOpNameList.push_back("scaleY");
            oSampledTransOpNameList.push_back("scaleZ");
        }

        Alembic::Abc::M44d m = iSamp.getMatrix();
        MTransformationMatrix mmat(MMatrix(m.x));

        // Everywhere else we use kPreTransform, but this doesn't work
        // when we pass in the matrix for some reason
        MSpace::Space tSpace = MSpace::kTransform;

        trans.setTranslation(mmat.getTranslation(tSpace), tSpace);

        trans.setRotatePivotTranslation(
            mmat.rotatePivotTranslation(tSpace), tSpace);

        trans.setRotatePivot(
            mmat.rotatePivot(tSpace), tSpace, gBalance);

        trans.setRotation(mmat.eulerRotation());

        trans.setRotateOrientation(
            mmat.rotationOrientation(), tSpace, gBalance);

        trans.setScalePivotTranslation(
            mmat.scalePivotTranslation(tSpace), tSpace);

        trans.setScalePivot(
            mmat.scalePivot(tSpace), tSpace, gBalance);

        double shear[3];
        mmat.getShear(shear, tSpace);
        trans.setShear(shear);

        double scale[3];
        mmat.getScale(scale, tSpace);
        trans.setScale(scale);
        return status;
    }

    bool scPivot = false;
    bool roPivot = false;

    MTransformationMatrix::RotationOrder rotOrder[2];
    rotOrder[0] = MTransformationMatrix::kInvalid;
    rotOrder[1] = MTransformationMatrix::kInvalid;

    size_t numOps = iSamp.getNumOps();

    for (size_t i = 0; i < numOps; ++i)
    {
        Alembic::AbcGeom::XformOp op = iSamp[i];
        switch (op.getType())
        {
            case Alembic::AbcGeom::kScaleOperation:
            {
                double scale[3];

                if (op.isXAnimated())
                {
                    oSampledTransOpNameList.push_back("scaleX");
                }

                scale[0] = op.getChannelValue(0);

                if (op.isYAnimated())
                {
                    oSampledTransOpNameList.push_back("scaleY");
                }

                scale[1] = op.getChannelValue(1);

                if (op.isZAnimated())
                {
                    oSampledTransOpNameList.push_back("scaleZ");
                }

                scale[2] = op.getChannelValue(2);

                trans.setScale(scale);
            }
            break;

            case Alembic::AbcGeom::kRotateXOperation:
            case Alembic::AbcGeom::kRotateYOperation:
            case Alembic::AbcGeom::kRotateZOperation:
            case Alembic::AbcGeom::kRotateOperation:
            {
                Alembic::Abc::V3d axis = op.getAxis();
                double x = axis.x;
                double y = axis.y;
                double z = axis.z;
                double angle = 0.0;

                MEulerRotation rot;
                trans.getRotation(rot);

                if (op.getHint() == Alembic::AbcGeom::kRotateHint)
                {
                    if (x == 1 && y == 0 && z == 0)
                    {
                        if (op.isAngleAnimated())
                        {
                            oSampledTransOpNameList.push_back("rotateX");
                        }
                        angle = op.getAngle();
                        rot.x = Alembic::AbcGeom::DegreesToRadians(angle);

                        // we have encountered the first rotation, set it
                        // to the 2 X possibilities
                        if (rotOrder[0] == MTransformationMatrix::kInvalid)
                        {
                            rotOrder[0] = MTransformationMatrix::kYZX;
                            rotOrder[1] = MTransformationMatrix::kZYX;
                        }
                        // we have filled in the two possibilities, now choose
                        // which one we should use
                        else if (rotOrder[1] != MTransformationMatrix::kInvalid)
                        {
                            if (rotOrder[1] == MTransformationMatrix::kYXZ)
                            {
                                rotOrder[0] = rotOrder[1];
                            }

                            rotOrder[1] = MTransformationMatrix::kInvalid;
                        }
                    }
                    else if (x == 0 && y == 1 && z == 0)
                    {

                        if (op.isAngleAnimated())
                        {
                            oSampledTransOpNameList.push_back("rotateY");
                        }
                        angle = op.getAngle();
                        rot.y = Alembic::AbcGeom::DegreesToRadians(angle);

                        // we have encountered the first rotation, set it
                        // to the 2 X possibilities
                        if (rotOrder[0] == MTransformationMatrix::kInvalid)
                        {
                            rotOrder[0] = MTransformationMatrix::kZXY;
                            rotOrder[1] = MTransformationMatrix::kXZY;
                        }
                        // we have filled in the two possibilities, now choose
                        // which one we should use
                        else if (rotOrder[1] != MTransformationMatrix::kInvalid)
                        {
                            if (rotOrder[1] == MTransformationMatrix::kZYX)
                            {
                                rotOrder[0] = rotOrder[1];
                            }

                            rotOrder[1] = MTransformationMatrix::kInvalid;
                        }
                    }
                    else if (x == 0 && y == 0 && z == 1)
                    {
                        if (op.isAngleAnimated())
                        {
                            oSampledTransOpNameList.push_back("rotateZ");
                        }
                        angle = op.getAngle();
                        rot.z = Alembic::AbcGeom::DegreesToRadians(angle);

                        // we have encountered the first rotation, set it
                        // to the 2 X possibilities
                        if (rotOrder[0] == MTransformationMatrix::kInvalid)
                        {
                            rotOrder[0] = MTransformationMatrix::kXYZ;
                            rotOrder[1] = MTransformationMatrix::kYXZ;
                        }
                        // we have filled in the two possibilities, now choose
                        // which one we should use
                        else if (rotOrder[1] != MTransformationMatrix::kInvalid)
                        {
                            if (rotOrder[1] == MTransformationMatrix::kXZY)
                            {
                                rotOrder[0] = rotOrder[1];
                            }

                            rotOrder[1] = MTransformationMatrix::kInvalid;
                        }
                    }

                    trans.setRotation(rot);
                }
                // kRotateOrientationHint
                else
                {
                    MQuaternion quat;

                    if (x == 1 && y == 0 && z == 0)
                    {
                        if (op.isAngleAnimated())
                        {
                            oSampledTransOpNameList.push_back("rotateAxisX");
                        }
                        angle = op.getAngle();

                        quat.setToXAxis(
                            Alembic::AbcGeom::DegreesToRadians(angle));

                    }
                    else if (x == 0 && y == 1 && z == 0)
                    {
                        if (op.isAngleAnimated())
                        {
                            oSampledTransOpNameList.push_back("rotateAxisY");
                        }
                        angle = op.getAngle();

                        quat.setToYAxis(
                            Alembic::AbcGeom::DegreesToRadians(angle));
                    }
                    else if (x == 0 && y == 0 && z == 1)
                    {
                        if (op.isAngleAnimated())
                        {
                            oSampledTransOpNameList.push_back("rotateAxisZ");
                        }
                        angle = op.getAngle();

                        quat.setToZAxis(
                            Alembic::AbcGeom::DegreesToRadians(angle));
                    }

                    MQuaternion curq = trans.rotateOrientation(gSpace);
                    trans.setRotateOrientation(curq*quat, gSpace, gBalance);
                }

            }
            break;

            // shear
            case Alembic::AbcGeom::kMatrixOperation:
            {
                double shear[3];

                // we only care about shearXY, shearXZ, and shearYZ here
                if (op.isChannelAnimated(4))
                {
                    oSampledTransOpNameList.push_back("shearXY");
                }
                shear[0] = op.getChannelValue(4);

                if (op.isChannelAnimated(8))
                {
                    oSampledTransOpNameList.push_back("shearXZ");
                }
                shear[1] = op.getChannelValue(8);

                if (op.isChannelAnimated(9))
                {
                    oSampledTransOpNameList.push_back("shearYZ");
                }
                shear[2] = op.getChannelValue(9);

                trans.setShear(shear);
            }
            break;

            case Alembic::AbcGeom::kTranslateOperation:
            {
                Alembic::Util::uint8_t hint = op.getHint();
                switch (hint)
                {
                    case Alembic::AbcGeom::kTranslateHint:
                    {
                        MVector vec;

                        if (op.isXAnimated())
                        {
                            oSampledTransOpNameList.push_back("translateX");
                        }
                        vec.x = op.getChannelValue(0);


                        if (op.isYAnimated())
                        {
                            oSampledTransOpNameList.push_back("translateY");
                        }
                        vec.y = op.getChannelValue(1);

                        if (op.isZAnimated())
                        {
                            oSampledTransOpNameList.push_back("translateZ");
                        }
                        vec.z = op.getChannelValue(2);

                        trans.setTranslation(vec, gSpace);
                    }
                    break;

                    case Alembic::AbcGeom::kScalePivotPointHint:
                    {
                        MPoint point;
                        point.w = 1.0;

                        if (op.isXAnimated())
                        {
                            if (!scPivot)
                            {
                                oSampledTransOpNameList.push_back(
                                    "scalePivotX");
                            }
                            else
                            {
                                oSampledTransOpNameList.push_back(
                                    "scalePivotXInv");
                            }
                        }
                        point.x = op.getChannelValue(0);


                        if (op.isYAnimated())
                        {
                            if (!scPivot)
                            {
                                oSampledTransOpNameList.push_back(
                                    "scalePivotY");
                            }
                            else
                            {
                                oSampledTransOpNameList.push_back(
                                    "scalePivotYInv");
                            }
                        }
                        point.y = op.getChannelValue(1);


                        if (op.isZAnimated())
                        {
                            if (!scPivot)
                            {
                                oSampledTransOpNameList.push_back(
                                    "scalePivotZ");
                            }
                            else
                            {
                                oSampledTransOpNameList.push_back(
                                    "scalePivotZInv");
                            }
                        }
                        point.z = op.getChannelValue(2);


                        // we only want to apply this to the first one
                        // the second one is the inverse
                        if (!scPivot)
                        {
                            trans.setScalePivot(point, gSpace, gBalance);
                        }

                        scPivot = !scPivot;
                    }
                    break;

                    case Alembic::AbcGeom::kScalePivotTranslationHint:
                    {
                        MVector vec;

                        if (op.isXAnimated())
                        {
                            oSampledTransOpNameList.push_back(
                                "scalePivotTranslateX");
                        }
                        vec.x = op.getChannelValue(0);


                        if (op.isYAnimated())
                        {
                            oSampledTransOpNameList.push_back(
                                "scalePivotTranslateY");
                        }
                        vec.y = op.getChannelValue(1);

                        if (op.isZAnimated())
                        {
                            oSampledTransOpNameList.push_back(
                                "scalePivotTranslateZ");
                        }
                        vec.z = op.getChannelValue(2);

                        trans.setScalePivotTranslation(vec, gSpace);
                    }
                    break;

                    case Alembic::AbcGeom::kRotatePivotPointHint:
                    {
                        MPoint point;
                        point.w = 1.0;

                        if (op.isXAnimated())
                        {
                            if (!roPivot)
                            {
                                oSampledTransOpNameList.push_back(
                                    "rotatePivotX");
                            }
                            else
                            {
                                oSampledTransOpNameList.push_back(
                                    "rotatePivotXInv");
                            }
                        }
                        point.x = op.getChannelValue(0);

                        if (op.isYAnimated())
                        {
                            if (!roPivot)
                            {
                                oSampledTransOpNameList.push_back(
                                    "rotatePivotY");
                            }
                            else
                            {
                                oSampledTransOpNameList.push_back(
                                    "rotatePivotYInv");
                            }
                        }
                        point.y = op.getChannelValue(1);


                        if (op.isZAnimated())
                        {
                            if (!roPivot)
                            {
                                oSampledTransOpNameList.push_back(
                                    "rotatePivotZ");
                            }
                            else
                            {
                                oSampledTransOpNameList.push_back(
                                    "rotatePivotZInv");
                            }
                        }
                        point.z = op.getChannelValue(2);


                        // only set rotate pivot on the first one, the second
                        // one is just the inverse
                        if (!roPivot)
                            trans.setRotatePivot(point, gSpace, gBalance);

                        roPivot = !roPivot;
                    }
                    break;

                    case Alembic::AbcGeom::kRotatePivotTranslationHint:
                    {
                        MVector vec;

                        if (op.isXAnimated())
                        {
                            oSampledTransOpNameList.push_back(
                                "rotatePivotTranslateX");
                        }
                        vec.x = op.getChannelValue(0);

                        if (op.isYAnimated())
                        {
                            oSampledTransOpNameList.push_back(
                                "rotatePivotTranslateY");
                        }
                        vec.y = op.getChannelValue(1);

                        if (op.isZAnimated())
                        {
                            oSampledTransOpNameList.push_back(
                                "rotatePivotTranslateZ");
                        }
                        vec.z = op.getChannelValue(2);

                        trans.setRotatePivotTranslation(vec, gSpace);
                    }
                    break;

                    default:
                    break;
                }
            }
            break;

            default:
            break;
        }

    }

    // no rotates were found so default to XYZ
    if (rotOrder[0] == MTransformationMatrix::kInvalid)
    {
        rotOrder[0] =  MTransformationMatrix::kXYZ;
    }

    trans.setRotationOrder(rotOrder[0], false);
    return status;
}
