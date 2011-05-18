//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include "MayaNurbsCurveWriter.h"
#include "MayaUtility.h"
#include "MayaTransformWriter.h"

namespace
{
    // get all the nurbs curves from below the given dagpath.
    // the curve group is considered animated if at least one curve is animated

    void collectNurbsCurves(const MDagPath &dagPath, MDagPathArray &dagPaths,
        bool & oIsAnimated)
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
                        oIsAnimated = true;
                    }
                }
            }
        }
    }  // end of function collectNurbsCurves
}

MayaNurbsCurveWriter::MayaNurbsCurveWriter(MDagPath & iDag,
    Alembic::Abc::OObject & iParent, uint32_t iTimeIndex, bool iIsCurveGrp,
    bool iWriteVisibility, bool iForceStatic) :
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

    Alembic::AbcGeom::OCurves obj(iParent, name.asChar(), iTimeIndex);
    mSchema = obj.getSchema();

    Alembic::Abc::OCompoundProperty cp = mSchema.getArbGeomParams();

    mAttrs = AttributesWriterPtr(new AttributesWriter(cp, iDag,
        iTimeIndex, iWriteVisibility, iForceStatic));

    write();
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

void MayaNurbsCurveWriter::write()
{
    Alembic::AbcGeom::OCurvesSchema::Sample samp;

    MStatus stat;
    mCVCount = 0;

    // if inheritTransform is on and the curve group is animated,
    // bake the cv positions in the world space
    MMatrix exclusiveMatrixInv = mRootDagPath.exclusiveMatrixInverse(&stat);

    std::size_t numCurves = 1;

    if (mIsCurveGrp)
        numCurves = mNurbsCurves.length();

    std::vector<float> minCurves(numCurves);
    std::vector<float> maxCurves(numCurves);
    std::vector<int32_t> nVertices(numCurves);
    std::vector<float> points;
    std::vector<float> width;
    std::vector<float> knotCurves;

    MMatrix transformMatrix;
    bool useConstWidth = false;

    MFnDependencyNode dep(mRootDagPath.transform());
    MPlug constWidthPlug = dep.findPlug("constantwidth");

    // pushing each width twice for now because width for some
    // reason is a V2f

    if (!constWidthPlug.isNull())
    {
        useConstWidth = true;
        width.push_back(constWidthPlug.asFloat());
        width.push_back(constWidthPlug.asFloat());
    }

    for (size_t i = 0; i < numCurves; i++)
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

        /*if (i == 0)
        {
            samp.setSpan(curve.numSpans());
            samp.setDegree(curve.degree());
            samp.setForm(curve.form()-1);
        }*/

        double start, end;
        curve.getKnotDomain(start, end);

        minCurves[i] = start;
        maxCurves[i] = end;

        int32_t numCVs = curve.numCVs(&stat);
        nVertices[i] = numCVs;
        mCVCount += numCVs;

        MPointArray cvArray;
        stat = curve.getCVs(cvArray, MSpace::kObject);
        for (size_t j = 0; j < numCVs; j++)
        {
            MPoint transformdPt;
            if (mIsCurveGrp)
                transformdPt = cvArray[j]*transformMatrix;
            else
                transformdPt = cvArray[j];

            points.push_back(transformdPt.x);
            points.push_back(transformdPt.y);
            points.push_back(transformdPt.z);
        }

        // normalize the knot vector for historical reasons
        /*MDoubleArray knots;
        stat = curve.getKnots(knots);
        unsigned int numKnots = knots.length();
        unsigned int totalNumKnots = knotCurves.size();

        // push_back a dummy value, we will set it below
        knotCurves.push_back(0.0);
        for (size_t j = 0 ; j < numKnots ; ++j)
            knotCurves.push_back(knots[j]);

        // for a knot sequence with multiple end knots, duplicate the existing
        // first and last knots once more. For a knot sequence with uniform
        // end knots, create their the new knots offset at an interval equal to
        // the existing first and last knot intervals
        double k1 = knotCurves[totalNumKnots+1];
        double k2 = curveGrp.knot[totalNumKnots+2];
        double klast_1 = curveGrp.knot[totalNumKnots+numKnots];
        double klast_2 = curveGrp.knot[totalNumKnots+numKnots-1];
        if (k1 == knotCurves[totalNumKnots+samp.getDegree()])
        {
            knotCurves[totalNumKnots] = k1;
            knotCurves.push_back(klast_1);
        }
        else
        {
            // if degree == 1, will not be in this part of code
            knotCurves[totalNumKnots] = 2* k1 - k2;
            knotCurves.push_back(2 * klast_1 - klast_2);
        }
        */

        // width
        MPlug widthPlug = curve.findPlug("width");

        if (!useConstWidth && !widthPlug.isNull())
        {
            MObject widthObj;
            MStatus status = widthPlug.getValue(widthObj);
            MFnDoubleArrayData fnDoubleArrayData(widthObj, &status);
            MDoubleArray doubleArrayData = fnDoubleArrayData.array();
            size_t arraySum = doubleArrayData.length();
            size_t correctVecLen = arraySum; //samp.getSpan();
            //if (samp.getForm() == 0)
            //    correctVecLen += 1;
            if (arraySum == correctVecLen)
            {
                for (size_t i = 0; i < arraySum; i++)
                {
                    width.push_back(doubleArrayData[i]);
                    width.push_back(doubleArrayData[i]);
                }
            }
            else
            {
                MString msg = "Curve ";
                msg += curve.partialPathName();
                msg += " has incorrect size for the width vector.";
                msg += "\nUsing default constantwidth.";
                MGlobal::displayWarning(msg);

                width.clear();
                width.push_back(0.1);
                width.push_back(0.1);
                useConstWidth = true;
            }
        }
        else
        {
            // pick a default value
            width.clear();
            width.push_back(0.1);
            width.push_back(0.1);
            useConstWidth = true;
        }
    }

    samp.setCurvesNumVertices(Alembic::Abc::Int32ArraySample(nVertices));
    samp.setPositions(Alembic::Abc::V3fArraySample(
        (const Imath::V3f *)&points.front(), points.size() / 3 ));
    samp.setWidths(Alembic::Abc::V2fArraySample(
        (const Imath::V2f *)&width.front(), width.size() / 3 ));
    mSchema.set(samp);
}
