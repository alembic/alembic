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

#include <AlembicTakoSPIExport/MayaNurbsCurveWriter.h>
#include <AlembicTakoSPIExport/MayaUtility.h>
#include <AlembicTakoSPIExport/MayaTransformWriter.h>

namespace Alembic {

namespace
{
    // get all the nurbs curves from below the given dagpath.
    // the curve group is considered animated if at least one curve is animated

    void collectNurbsCurves(const MDagPath &dagPath, MDagPathArray &dagPaths,
        bool & mIsAnimated)
    {
        MStatus stat;

        MItDag itDag(MItDag::kDepthFirst, MFn::kNurbsCurve, &stat);
        stat = itDag.reset(dagPath, MItDag::kDepthFirst, MFn::kNurbsCurve);

        if (stat == MS::kSuccess)
        {
            for (;!itDag.isDone();itDag.next())
            {
                MDagPath curvePath;
                stat = itDag.getPath(curvePath);
                if (stat == MS::kSuccess)
                {
                    MObject curve = curvePath.node();

                    if ( !util::isIntermediate(curve) &&
                        curve.hasFn(MFn::kNurbsCurve) )
                    {
                        dagPaths.append(curvePath);
                    }
                    // don't bother checking the animated state if the curve
                    // wasn't appended to the list
                    else
                    {
                        continue;
                    }
                    // with the flag set to true, check the DagPath and it's
                    // parent
                    if (util::isAnimated(curve, true))
                    {
                        mIsAnimated = true;
                    }
                }
            }
        }
    }  // end of function collectNurbsCurves
}

MayaNurbsCurveWriter::MayaNurbsCurveWriter(
    float iFrame, MDagPath & iDag, TransformWriterPtr iParent,
    bool iIsCurveGrp, bool iWriteVisibility) :
    mIsAnimated(false), mRootDagPath(iDag), mIsCurveGrp(iIsCurveGrp)
{
    MStatus stat;
    MFnDependencyNode fnDepNode(iDag.node(), &stat);
    MString name = fnDepNode.name();

    if (mIsCurveGrp)
    {
        collectNurbsCurves(mRootDagPath, mNurbsCurves, mIsAnimated);

        // if no curves were found bail early
        if (mNurbsCurves.length() == 0)
            return;
    }
    else
    {
        MObject curve = iDag.node();

        if (util::isAnimated(curve))
            mIsAnimated = true;
    }

    mWriter = iParent->createNurbsCurveChild(name.asChar());

    mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, *mWriter,
        iDag, iWriteVisibility));

    TakoSPI::NurbsCurveGrp curveGrp;
    fillTopology(curveGrp);
    if (!mIsAnimated)
        iFrame = FLT_MAX;
    mWriter->write(iFrame, curveGrp);
}

unsigned int MayaNurbsCurveWriter::getNumCVs()
{
    return mCVCount;
}

unsigned int MayaNurbsCurveWriter::getNumCurves()
{
    if (mIsCurveGrp)
        return mNurbsCurves.length();
    else
        return 1;
}

bool MayaNurbsCurveWriter::isAnimated() const
{
    return mIsAnimated;
}

void MayaNurbsCurveWriter::write(float iFrame)
{
    TakoSPI::NurbsCurveGrp curveGrp;
    fillTopology(curveGrp);
    mWriter->write(iFrame, curveGrp);

    mAttrs->write(iFrame);
}


void MayaNurbsCurveWriter::fillTopology(TakoSPI::NurbsCurveGrp & curveGrp)
{
    MStatus stat;
    mCVCount = 0;

    // if inheritTransform is on and the curve group is animated,
    // bake the cv positions in the world space
    MMatrix exclusiveMatrixInv = mRootDagPath.exclusiveMatrixInverse(&stat);

    if (mIsCurveGrp)
        curveGrp.numCurve = mNurbsCurves.length();
    else
        curveGrp.numCurve = 1;

    MMatrix transformMatrix;
    bool useConstWidth = false;
    for (size_t i = 0; i < curveGrp.numCurve; i++)
    {
        MFnNurbsCurve curve;
        if (mIsCurveGrp)
        {
            curve.setObject(mNurbsCurves[i]);
            MMatrix inclusiveMatrix = mNurbsCurves[i].inclusiveMatrix(&stat);
            transformMatrix = inclusiveMatrix*exclusiveMatrixInv;
        }
        else
            curve.setObject(mRootDagPath.node());

        TakoSPI::NurbsCurveMiscInfo miscInfo;
        miscInfo.span = curve.numSpans(&stat);
        miscInfo.degree = curve.degree(&stat);
        miscInfo.form = curve.form(&stat)-1;
        double start, end;
        curve.getKnotDomain(start, end);
        miscInfo.min = start;
        miscInfo.max = end;

        unsigned int numCVs = curve.numCVs(&stat);
        miscInfo.numCV = numCVs;
        mCVCount += numCVs;
        MPointArray cvArray;
        stat = curve.getCVs(cvArray, MSpace::kObject);
        curveGrp.cv.reserve(numCVs*4);
        for (size_t j = 0; j < numCVs; j++)
        {
            MPoint transformdPt;
            if (mIsCurveGrp)
                transformdPt = cvArray[j]*transformMatrix;
            else
                transformdPt = cvArray[j];

            curveGrp.cv.push_back(transformdPt.x);
            curveGrp.cv.push_back(transformdPt.y);
            curveGrp.cv.push_back(transformdPt.z);
            curveGrp.cv.push_back(transformdPt.w);
        }

        // knot vector is normalized because maya2pit does this
        MDoubleArray knots;
        stat = curve.getKnots(knots);
        unsigned int numKnots = knots.length();
        curveGrp.knot.reserve(numKnots+2);
        curveGrp.knot.push_back(0.0);
        for (size_t j = 0 ; j < numKnots ; ++j)
            curveGrp.knot.push_back(knots[j]);

        // periodic case
        if  (miscInfo.form == 2)
        {
            // is 1 always the correct offset?
            curveGrp.knot[0] = curveGrp.knot[1] - 1;
            curveGrp.knot.push_back(curveGrp.knot[numKnots] + 1);
        }
        else
        {
            curveGrp.knot[0] = curveGrp.knot[1];
            curveGrp.knot.push_back(curveGrp.knot[numKnots]);
        }

        curveGrp.miscInfo.push_back(miscInfo);

        // width
        MPlug widthPlug = curve.findPlug("width");
        MPlug constWidthPlug = curve.findPlug("constantwidth");

        if (!useConstWidth && !widthPlug.isNull())
        {
            MObject widthObj;
            MStatus status = widthPlug.getValue(widthObj);
            MFnDoubleArrayData fnDoubleArrayData(widthObj, &status);
            MDoubleArray doubleArrayData = fnDoubleArrayData.array();
            size_t arraySum = doubleArrayData.length();
            size_t correctVecLen = curveGrp.getSpan(i);
            if (miscInfo.form == 0)
                correctVecLen += 1;
            if (arraySum == correctVecLen)
            {
                curveGrp.width.reserve(arraySum);
                for (size_t i = 0; i < arraySum; i++)
                    curveGrp.width.push_back(doubleArrayData[i]);
            }
            else
            {
                MString msg = "Curve ";
                msg += curve.partialPathName();
                msg += " has incorrect size for the width vector.";
                msg += "\nUsing default constantwidth.";
                MGlobal::displayWarning(msg);

                curveGrp.constantWidth = 0.1;
                useConstWidth = true;
            }
        }
        else if (!constWidthPlug.isNull())
        {
            constWidthPlug.getValue(curveGrp.constantWidth);
            useConstWidth = true;
        }
        else
        {
            // pick a default value
            curveGrp.constantWidth = 0.1;
            useConstWidth = true;
        }
        if (useConstWidth)
        {
            curveGrp.width.clear();
        }
    }
}

} // End namespace Alembic

