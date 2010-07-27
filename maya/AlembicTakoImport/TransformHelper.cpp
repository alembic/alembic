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

#include <AlembicTakoImport/TransformHelper.h>
#include <AlembicTakoImport/Util.h>
#include <AlembicTakoImport/NodeIteratorVisitorHelper.h>

namespace Alembic {

using Tako::TransformReaderPtr;
using Tako::Scale;
using Tako::Rotate;
using Tako::Translate;
using Tako::Matrix4x4;
using Tako::Mat4x4;
using Tako::Value;

namespace
{
    // I'm not sure why kPreTransform works and kTransform does not for all
    // cases except when the transform is complicated and we need to use the
    // matrix.
    MSpace::Space gSpace = MSpace::kPreTransform;
    const bool gBalance = false;

    // This visitor is used to determine
    class ComplexTransformVisitor : public boost::static_visitor<>
    {
    public:

        ComplexTransformVisitor(MFnTransform & iTransform, float iFrame) :
            mTransform(iTransform)
        {
            mFrame = iFrame;

            // the state value must always increase
            // and it is only allowed to stay the same for rotation
            state = 0;

            // special case flags that indicate a rotate pivot or scale pivot
            // has occurred, if it has then another operation of the same kind
            // must occur to undo it
            roPivot = false;
            scPivot = false;

            // since rotates can occur around any axis in any order
            // we need to keep track of what ones have occurred
            xAxis = false;
            yAxis = false;
            zAxis = false;

            rotOrder[0] = MTransformationMatrix::kInvalid;
            rotOrder[1] = MTransformationMatrix::kInvalid;

            mIsComplex = false;
        }

        ~ComplexTransformVisitor()
        {
        }

        void operator()(const Scale& iOp)
        {
            Scale::ScaleType sType = iOp.getType();
            if (sType == Scale::cScale)
            {
                if (state < 12)
                {
                    state = 12;

                    if (!mTransform.object().isNull())
                    {
                        Value oX, oY, oZ;
                        iOp.get(oX, oY, oZ);
                        double scale[3] = {oX.mValue, oY.mValue, oZ.mValue};
                        mTransform.setScale(scale);

                        if (!oX.mIsStatic) mSampledList.push_back("scaleX");
                        if (!oY.mIsStatic) mSampledList.push_back("scaleY");
                        if (!oZ.mIsStatic) mSampledList.push_back("scaleZ");
                    }
                }
                else
                    mIsComplex = true;
            }
            else
            {
                mIsComplex = true;
            }
        }

        void operator()(const Translate& iOp)
        {
            Translate::TranslateType tType = iOp.getType();
            switch (tType)
            {
            case Translate::cTranslate:
                if (state < 1)
                {
                    state = 1;
                    if (!mTransform.object().isNull())
                    {
                        Value oX, oY, oZ;
                        iOp.get(oX, oY, oZ);
                        MVector vec(oX.mValue, oY.mValue, oZ.mValue);
                        mTransform.setTranslation(vec, gSpace);

                        if (!oX.mIsStatic)
                            mSampledList.push_back("translateX");
                        if (!oY.mIsStatic)
                            mSampledList.push_back("translateY");
                        if (!oZ.mIsStatic)
                            mSampledList.push_back("translateZ");
                    }
                }
                else
                {
                    mIsComplex = true;
                }
            break;

            case Translate::cScalePivotPoint:

                // we have not yet encountered this pivot
                if (state < 10 && !scPivot)
                {
                    scPivot = true;
                    state = 10;
                    if (!mTransform.object().isNull())
                    {
                        Value oX, oY, oZ;
                        iOp.get(oX, oY, oZ);
                        MPoint point(oX.mValue, oY.mValue, oZ.mValue);
                        mTransform.setScalePivot(point, gSpace, gBalance);

                        if (!oX.mIsStatic)
                            mSampledList.push_back("scalePivotX");
                        if (!oY.mIsStatic)
                            mSampledList.push_back("scalePivotY");
                        if (!oZ.mIsStatic)
                            mSampledList.push_back("scalePivotZ");
                    }
                }
                // we have encounted this pivot before
                // this this one undoes the first one
                else if (state < 13 && scPivot)
                {
                    scPivot = false;
                    state = 13;

                    Value oX, oY, oZ;
                    iOp.get(oX, oY, oZ);

                    // add them to the sampled list (if sampled) but dont do
                    // anything with their values because this is the undo op.
                    if (!oX.mIsStatic)
                        mSampledList.push_back("scalePivotXInv");
                    if (!oY.mIsStatic)
                        mSampledList.push_back("scalePivotYInv");
                    if (!oZ.mIsStatic)
                        mSampledList.push_back("scalePivotZInv");
                }
                else
                    mIsComplex = true;

            break;

            case Translate::cScalePivotTranslation:
                if (state < 9)
                {
                    state = 9;
                    if (!mTransform.object().isNull())
                    {
                        Value oX, oY, oZ;
                        iOp.get(oX, oY, oZ);
                        MVector vec(oX.mValue, oY.mValue, oZ.mValue);
                        mTransform.setScalePivotTranslation(vec, gSpace);

                        if (!oX.mIsStatic)
                            mSampledList.push_back("scalePivotTranslateX");
                        if (!oY.mIsStatic)
                            mSampledList.push_back("scalePivotTranslateY");
                        if (!oZ.mIsStatic)
                            mSampledList.push_back("scalePivotTranslateZ");
                    }
                }
                else
                    mIsComplex = true;
            break;

            case Translate::cRotatePivotPoint:
                // we have not yet encountered this pivot
                if (state < 3 && !roPivot)
                {
                    roPivot = true;
                    state = 3;

                    if (!mTransform.object().isNull())
                    {
                        Value oX, oY, oZ;
                        iOp.get(oX, oY, oZ);
                        MPoint point(oX.mValue, oY.mValue, oZ.mValue);
                        mTransform.setRotatePivot(point, gSpace, gBalance);

                        if (!oX.mIsStatic)
                            mSampledList.push_back("rotatePivotX");
                        if (!oY.mIsStatic)
                            mSampledList.push_back("rotatePivotY");
                        if (!oZ.mIsStatic)
                            mSampledList.push_back("rotatePivotZ");
                    }
                }
                // we have encounted this pivot before this this one undoes
                // the first one
                else if (state < 8 && roPivot)
                {
                    roPivot = false;
                    state = 8;

                    Value oX, oY, oZ;
                    iOp.get(oX, oY, oZ);

                    // add them to the sampled list (if sampled) but dont do
                    // anything with their values because this is the undo op.
                    if (!oX.mIsStatic)
                        mSampledList.push_back("rotatePivotXInv");
                    if (!oY.mIsStatic)
                        mSampledList.push_back("rotatePivotYInv");
                    if (!oZ.mIsStatic)
                        mSampledList.push_back("rotatePivotZInv");
                }
                else
                    mIsComplex = true;
            break;

            case Translate::cRotatePivotTranslation:
                if (state < 2)
                {
                    state = 2;
                    if (!mTransform.object().isNull())
                    {
                        Value oX, oY, oZ;
                        iOp.get(oX, oY, oZ);
                        MVector vec(oX.mValue, oY.mValue, oZ.mValue);
                        mTransform.setRotatePivotTranslation(vec, gSpace);

                        if (!oX.mIsStatic)
                            mSampledList.push_back("rotatePivotTranslateX");
                        if (!oY.mIsStatic)
                            mSampledList.push_back("rotatePivotTranslateY");
                        if (!oZ.mIsStatic)
                            mSampledList.push_back("rotatePivotTranslateZ");
                    }
                }
                else
                    mIsComplex = true;
            break;

            default:
            {
                mIsComplex = true;
            }
            break;
            }  // switch
        }

        void operator()(const Rotate& iOp)
        {
            Value oA;
            float x, y, z;
            iOp.get(oA, x, y, z);
            Rotate::RotateType rType = iOp.getType();
            switch (rType)
            {
            case Rotate::cRotate:
                // if we have not yet encountered a rotate about x
                if (x == 1 && y == 0 && z == 0 && !xAxis && state <= 4)
                {
                    state = 4;
                    xAxis = true;

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

                    if (!mTransform.object().isNull())
                    {
                        double angle = oA.mValue;
                        if (!oA.mIsStatic) mSampledList.push_back("rotateX");
                        MVector axis(x, y, z);
                        MQuaternion quat(angle, axis);
                        mTransform.rotateBy(quat, gSpace);
                    }
                }
                // if we have not yet encountered a rotate about y
                else if (x == 0 && y == 1 && z == 0 && !yAxis && state <= 4)
                {
                    state = 4;
                    yAxis = true;

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

                    if (!mTransform.object().isNull())
                    {
                        double angle = oA.mValue;
                        if (!oA.mIsStatic) mSampledList.push_back("rotateY");
                        MVector axis(x, y, z);
                        MQuaternion quat(angle, axis);
                        mTransform.rotateBy(quat, gSpace);
                    }
                }
                // if we have not yet encountered a rotate about z
                else if (x == 0 && y == 0 && z == 1 && !zAxis && state <= 4)
                {
                    state = 4;
                    zAxis = true;

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

                    if (!mTransform.object().isNull())
                    {
                        double angle = oA.mValue;
                        if (!oA.mIsStatic) mSampledList.push_back("rotateZ");
                        MVector axis(x, y, z);
                        MQuaternion quat(angle, axis);
                        mTransform.rotateBy(quat, gSpace);
                    }
                }
                // for either a weird rotation or a duplicate axis
                else
                {
                    mIsComplex = true;
                }
            break;
            case Rotate::cRotateOrientation:
                // rotate orient always occurs in XYZ order
                if (x == 1 && y == 0 && z == 0 && state < 7)
                {
                    state = 7;

                    if (!mTransform.object().isNull())
                    {
                        double angle = oA.mValue;
                        if (!oA.mIsStatic)
                            mSampledList.push_back("rotateAxisX");
                        // get the existing quat and set the axis
                        MQuaternion curq = mTransform.rotateOrientation(gSpace);
                        MQuaternion quat;
                        quat.setToXAxis(angle);
                        mTransform.setRotateOrientation(curq*quat,
                            gSpace, gBalance);
                    }
                }
                else if (x == 0 && y == 1 && z == 0 && state < 6)
                {
                    state = 6;
                    if (!mTransform.object().isNull())
                    {
                        double angle = oA.mValue;
                        if (!oA.mIsStatic)
                            mSampledList.push_back("rotateAxisY");
                        // get the existing quat and set the axis
                        MQuaternion curq = mTransform.rotateOrientation(gSpace);
                        MQuaternion quat;
                        quat.setToYAxis(angle);
                        mTransform.setRotateOrientation(curq*quat,
                            gSpace, gBalance);
                    }
                }
                else if (x == 0 && y == 0 && z == 1 && state < 5)
                {
                    state = 5;
                    if (!mTransform.object().isNull())
                    {
                        double angle = oA.mValue;
                        if (!oA.mIsStatic)
                            mSampledList.push_back("rotateAxisZ");
                        // get the existing quat and set the axis
                        MQuaternion curq = mTransform.rotateOrientation(gSpace);
                        MQuaternion quat;
                        quat.setToYAxis(angle);
                        mTransform.setRotateOrientation(curq*quat,
                            gSpace, gBalance);
                    }
                }
                // a weird axis, wrong order, or duplicates
                else
                {
                    mIsComplex = true;
                }
            break;

            default:
                mIsComplex = true;
            break;
            }
        }

        void operator()(const Matrix4x4& iOp)
        {
            Matrix4x4::Matrix4x4Type matType = iOp.getType();
            switch (matType)
            {
            case Matrix4x4::cMayaShear:
                if (state < 11)
                {
                    state = 11;
                    if (!mTransform.object().isNull())
                    {
                        Value oM[16];
                        iOp.get(oM[ 0], oM[ 1], oM[ 2], oM[ 3],
                                oM[ 4], oM[ 5], oM[ 6], oM[ 7],
                                oM[ 8], oM[ 9], oM[10], oM[11],
                                oM[12], oM[13], oM[14], oM[15]);
                        double shear[3] = {oM[4].mValue,
                                           oM[8].mValue,
                                           oM[9].mValue};
                        mTransform.setShear(shear);

                        if (!oM[4].mIsStatic)
                            mSampledList.push_back("shearXY");
                        if (!oM[8].mIsStatic)
                            mSampledList.push_back("shearXZ");
                        if (!oM[9].mIsStatic)
                            mSampledList.push_back("shearYZ");
                    }
                }
                else
                    mIsComplex = true;
            break;

            // includes Matrix4x4::cMatrix
            default:
                mIsComplex = true;
            break;
            }
        }

        bool isComplex()
        {
            // if the pivots have occurred once but not undone, then we'll
            // treat it like its complex
            return mIsComplex || roPivot || scPivot;
        }


        // if we have a more complex operation stack then we will have to go
        // through MTransformationMatrix., otherwise set the rotation order
        // if it is appropriate
        void checkAndSet(const TransformReaderPtr & iNode)
        {
            // just set rotate order assuming it is not animated
            if (!isComplex() && !mTransform.object().isNull())
            {
                // no rotates were found so default to XYZ
                if (rotOrder[0] == MTransformationMatrix::kInvalid)
                {
                    rotOrder[0] =  MTransformationMatrix::kXYZ;
                }

                mTransform.setRotationOrder(rotOrder[0], true);

                return;
            }
            // mTransform was NULL so we'll just bail early
            else if (!isComplex())
            {
                return;
            }

            // something is animated so we need to add everything
            if (iNode->hasFrames())
            {
                mSampledList.clear();
                mSampledList.push_back("translateX");
                mSampledList.push_back("translateY");
                mSampledList.push_back("translateZ");
                mSampledList.push_back("rotatePivotTranslateX");
                mSampledList.push_back("rotatePivotTranslateY");
                mSampledList.push_back("rotatePivotTranslateZ");
                mSampledList.push_back("rotatePivotX");
                mSampledList.push_back("rotatePivotY");
                mSampledList.push_back("rotatePivotZ");
                mSampledList.push_back("rotateX");
                mSampledList.push_back("rotateY");
                mSampledList.push_back("rotateZ");
                mSampledList.push_back("rotateAxisX");
                mSampledList.push_back("rotateAxisY");
                mSampledList.push_back("rotateAxisZ");
                mSampledList.push_back("scalePivotTranslateX");
                mSampledList.push_back("scalePivotTranslateY");
                mSampledList.push_back("scalePivotTranslateZ");
                mSampledList.push_back("scalePivotX");
                mSampledList.push_back("scalePivotY");
                mSampledList.push_back("scalePivotZ");
                mSampledList.push_back("shearXY");
                mSampledList.push_back("shearXZ");
                mSampledList.push_back("shearYZ");
                mSampledList.push_back("scaleX");
                mSampledList.push_back("scaleY");
                mSampledList.push_back("scaleZ");
            }

            // if the transform isn't associated with a real object then
            // do not try to set anything on it
            if (mTransform.object().isNull())
            {
                return;
            }

            Mat4x4 mat = iNode->getMatrix();
            MTransformationMatrix mmat(MMatrix(mat.x));

            // Everywhere else we use kPreTransform, but this doesn't work
            // when we pass in the matrix for some reason
            MSpace::Space tSpace = MSpace::kTransform;

            mTransform.setTranslation(mmat.getTranslation(tSpace), tSpace);

            mTransform.setRotatePivotTranslation(
                mmat.rotatePivotTranslation(tSpace), tSpace);

            mTransform.setRotatePivot(
                mmat.rotatePivot(tSpace), tSpace, gBalance);

            mTransform.setRotation(mmat.eulerRotation());

            mTransform.setRotateOrientation(
                mmat.rotationOrientation(), tSpace, gBalance);

            mTransform.setScalePivotTranslation(
                mmat.scalePivotTranslation(tSpace), tSpace);

            mTransform.setScalePivot(
                mmat.scalePivot(tSpace), tSpace, gBalance);

            double shear[3];
            mmat.getShear(shear, tSpace);
            mTransform.setShear(shear);

            double scale[3];
            mmat.getScale(scale, tSpace);
            mTransform.setScale(scale);
        }

        // name of the sampled transforms
        std::vector<std::string> mSampledList;

    private:
        MFnTransform & mTransform;
        float mFrame;

        unsigned int state;
        bool roPivot, scPivot;
        bool xAxis, yAxis, zAxis;

        MTransformationMatrix::RotationOrder rotOrder[2];
        bool mIsComplex;
    };
};

MStatus read(float iFrame, const TransformReaderPtr & iNode,
    std::vector<double> & sampleList, bool isComplex)
{
    MStatus status = MS::kSuccess;
    if (!isComplex)
    {
        iNode->getSamples(iFrame, sampleList);
        return status;
    }

    iNode->read(iFrame);
    Mat4x4 mat = iNode->getMatrix();

    MTransformationMatrix mmat(MMatrix(mat.x));

    // Everywhere else we use kPreTransform, but this doesn't work
    // when we pass in the matrix for some reason
    MSpace::Space tSpace = MSpace::kTransform;

    // push the results into sampleList
    MVector vec = mmat.getTranslation(tSpace, &status);
    sampleList.push_back(vec.x);
    sampleList.push_back(vec.y);
    sampleList.push_back(vec.z);

    vec = mmat.rotatePivotTranslation(tSpace, &status);
    sampleList.push_back(vec.x);
    sampleList.push_back(vec.y);
    sampleList.push_back(vec.z);

    MPoint pt = mmat.rotatePivot(tSpace, &status);
    sampleList.push_back(pt.x);
    sampleList.push_back(pt.y);
    sampleList.push_back(pt.z);

    double rot[3];
    MTransformationMatrix::RotationOrder order;
    mmat.getRotation(rot, order);
    sampleList.push_back(rot[0]);
    sampleList.push_back(rot[1]);
    sampleList.push_back(rot[2]);

    MQuaternion quat = mmat.rotationOrientation();
    vec = quat.asEulerRotation().asVector();
    sampleList.push_back(vec.x);
    sampleList.push_back(vec.y);
    sampleList.push_back(vec.z);

    pt = mmat.scalePivotTranslation(tSpace, &status);
    sampleList.push_back(vec.x);
    sampleList.push_back(vec.y);
    sampleList.push_back(vec.z);

    vec = mmat.scalePivot(tSpace, &status);
    sampleList.push_back(vec.x);
    sampleList.push_back(vec.y);
    sampleList.push_back(vec.z);

    mmat.scalePivot(tSpace, &status);
    double shear[3];
    status = mmat.getShear(shear, tSpace);
    sampleList.push_back(shear[0]);
    sampleList.push_back(shear[1]);
    sampleList.push_back(shear[2]);

    double scale[3];
    status = mmat.getScale(scale, tSpace);
    sampleList.push_back(scale[0]);
    sampleList.push_back(scale[1]);
    sampleList.push_back(scale[2]);

    return status;
}

bool    isComplex(float iFrame, const TransformReaderPtr & iNode)
{
    // traverse the stack using the const iterator
    iNode->read(iFrame);
    Tako::TransformReader::const_stack_iterator stackIter = iNode->begin();
    MFnTransform trans;
    ComplexTransformVisitor transformVisitor(trans, iFrame);
    while (stackIter != iNode->end())
    {
        boost::apply_visitor(transformVisitor, *stackIter);
        stackIter++;
    }
    return transformVisitor.isComplex();
}

MStatus create(float iFrame, const TransformReaderPtr& iNode,
    MObject & iParent, MObject & iObject,
    std::vector<std::string> & iSampledPropNameList,
    std::vector<std::string> & iSampledTransOpNameList,
    bool & oIsComplex, bool iSwap)
{
    MStatus status = MS::kSuccess;
    MString name(iNode->getName());

    iNode->read(iFrame);

    MFnTransform trans;

    if (iSwap)
    {
        trans.setObject(iObject);

        // disconnect and reset all the native attributes

        const MVector   vec(0, 0, 0);
        const MPoint    point(0, 0, 0);
        const double    zero[3] = {0, 0, 0};

        MPlug dstPlug;

        dstPlug = trans.findPlug("translateX");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("translateY");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("translateZ");
        disconnectAllPlugsTo(dstPlug);
        trans.setTranslation(vec, gSpace);

        dstPlug = trans.findPlug("scaleX");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("scaleY");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("scaleZ");
        disconnectAllPlugsTo(dstPlug);
        const double scale[3] = {1, 1, 1};
        trans.setScale(scale);

        dstPlug = trans.findPlug("shearXY");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("shearXZ");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("shearYZ");
        disconnectAllPlugsTo(dstPlug);
        trans.setShear(zero);

        dstPlug = trans.findPlug("rotateX");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("rotateY");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("rotateZ");
        disconnectAllPlugsTo(dstPlug);
        MTransformationMatrix::RotationOrder order =
            MTransformationMatrix::kXYZ;
        trans.setRotation(zero, order);

        dstPlug = trans.findPlug("scalePivotX");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("scalePivotY");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("scalePivotZ");
        disconnectAllPlugsTo(dstPlug);
        trans.setScalePivot(point, gSpace, gBalance);

        dstPlug = trans.findPlug("scalePivotTranslateX");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("scalePivotTranslateY");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("scalePivotTranslateZ");
        disconnectAllPlugsTo(dstPlug);
        trans.setScalePivotTranslation(vec, gSpace);

        dstPlug = trans.findPlug("rotatePivotX");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("rotatePivotY");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("rotatePivotZ");
        disconnectAllPlugsTo(dstPlug);
        trans.setScalePivot(point, gSpace, gBalance);

        dstPlug = trans.findPlug("rotatePivotTranslateX");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("rotatePivotTranslateY");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("rotatePivotTranslateZ");
        disconnectAllPlugsTo(dstPlug);
        trans.setScalePivotTranslation(vec, gSpace);

        dstPlug = trans.findPlug("rotateAxisX");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("rotateAxisY");
        disconnectAllPlugsTo(dstPlug);
        dstPlug = trans.findPlug("rotateAxisZ");
        disconnectAllPlugsTo(dstPlug);
        const MQuaternion quat;
        trans.setRotateOrientation(quat, gSpace, gBalance);

        dstPlug = trans.findPlug("rotateOrder");
        disconnectAllPlugsTo(dstPlug);
        bool reorder = true;
        trans.setRotationOrder(order, reorder);

        dstPlug = trans.findPlug("inheritsTransform");
        disconnectAllPlugsTo(dstPlug);
        dstPlug.setBool(true);

        // disconnect connections to animated props
        iNode->readProperties(iFrame);
        Tako::PropertyMap::const_iterator propIter =
            iNode->beginSampledProperties();
        while ( propIter != iNode->endSampledProperties() )
        {
            std::string propName = propIter->first;
            // SPT_xxColor is special
            if ( propName.find("SPT_") != std::string::npos
                && propName.find("Color") != std::string::npos )
            {
                std::string colorR = propName+std::string("R");
                dstPlug = trans.findPlug(colorR.c_str());
                disconnectAllPlugsTo(dstPlug);
                std::string colorG = propName+std::string("G");
                dstPlug = trans.findPlug(colorG.c_str());
                disconnectAllPlugsTo(dstPlug);
                std::string colorB = propName+std::string("B");
                dstPlug = trans.findPlug(colorB.c_str());
                disconnectAllPlugsTo(dstPlug);
            }
            else
            {
                dstPlug = trans.findPlug(propIter->first.c_str());
                disconnectAllPlugsTo(dstPlug);
            }
            propIter++;
        }
        addProperties(iFrame, *iNode, iObject, iSampledPropNameList);
    }
    else
    {
        iObject = trans.create(iParent, &status);
        if (status != MS::kSuccess)
        {
            MString theError("Failed to create transform node ");
            theError += MString(iNode->getName());
            printError(theError);
            oIsComplex = false;
            iObject = MObject::kNullObj;
            return status;
        }

        trans.setName(name);

        // if there is a better MFnTransform API way to do this, then use it.
        MPlug dstPlug;
        dstPlug = trans.findPlug("inheritsTransform");
        if (!dstPlug.isNull())
            dstPlug.setBool(iNode->inheritsTransform());

        addProperties(iFrame, *iNode, iObject, iSampledPropNameList);
    }

    // traverse the stack using the const iterator
    Tako::TransformReader::const_stack_iterator stackIter = iNode->begin();
    ComplexTransformVisitor transformVisitor(trans, iFrame);
    while (stackIter != iNode->end())
    {
        boost::apply_visitor(transformVisitor, *stackIter);
        stackIter++;
    }

    transformVisitor.checkAndSet(iNode);
    iSampledTransOpNameList = transformVisitor.mSampledList;
    oIsComplex = transformVisitor.isComplex();

    return status;
}

} // End namespace Alembic

