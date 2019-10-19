//-*****************************************************************************
//
// Copyright (c) 2009-2014,
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
#include "NurbsCurveHelper.h"

#include <maya/MString.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MFnTransform.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MPlug.h>

MStatus readCurves(double iFrame, const Alembic::AbcGeom::ICurves & iNode,
    std::size_t iExpectedCurves, std::vector<MObject> & ioCurveObjects)
{
    MFnNurbsCurve mFn;
    MStatus status;

    Alembic::AbcGeom::ICurvesSchema schema = iNode.getSchema();
    Alembic::AbcCoreAbstract::index_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame, schema.getTimeSampling(),
        schema.getNumSamples(), index, ceilIndex);

    Alembic::AbcGeom::ICurvesSchema::Sample samp, ceilSamp;
    schema.get(samp, index);

    unsigned int numCurves = static_cast<unsigned int>(samp.getNumCurves());

    bool interp = false;
    if (alpha != 0.0 && index != ceilIndex)
    {
        schema.get(ceilSamp, ceilIndex);
        if (ceilSamp.getNumCurves() == numCurves)
        {
            interp = true;
        }
    }

    Alembic::Abc::P3fArraySamplePtr sampPoints = samp.getPositions();
    Alembic::Abc::P3fArraySamplePtr ceilPoints = ceilSamp.getPositions();
    Alembic::Abc::FloatArraySamplePtr sampKnots = samp.getKnots();
    Alembic::Abc::FloatArraySamplePtr ceilKnots = ceilSamp.getKnots();
    Alembic::Abc::UcharArraySamplePtr orders = samp.getOrders();

    Alembic::Abc::Int32ArraySamplePtr numVertices =
        samp.getCurvesNumVertices();
    Alembic::Abc::Int32ArraySamplePtr ceilNumVertices =
        ceilSamp.getCurvesNumVertices();

    if (interp && sampPoints->size() != ceilPoints->size())
    {
        interp = false;
    }

    std::size_t curVert = 0;
    std::size_t curKnot = 0;
    for (std::size_t i = 0; i < iExpectedCurves && i < numCurves; ++i)
    {
        unsigned int degree = 1;
        if (samp.getType() == Alembic::AbcGeom::kCubic)
        {
            degree = 3;
        }
        else if (samp.getType() == Alembic::AbcGeom::kVariableOrder &&
                 orders->size() > i)
        {
            degree = (unsigned int)((*orders)[i] - 1);
        }

        MPointArray cvs;

        int numVerts = (*numVertices)[i];
        int j;
        for (j = 0; j < numVerts; ++j, ++curVert)
        {
            Alembic::Abc::V3f pos = (*sampPoints)[curVert];

            if (interp)
            {
                Alembic::Abc::V3f ceilPos = (*ceilPoints)[curVert];
                cvs.append(simpleLerp<float>(alpha, pos.x, ceilPos.x),
                    simpleLerp<float>(alpha, pos.y, ceilPos.y),
                    simpleLerp<float>(alpha, pos.z, ceilPos.z));
            }
            else
            {
                cvs.append(pos.x, pos.y, pos.z);
            }
        }

        MFnNurbsCurve::Form form = MFnNurbsCurve::kOpen;

        // so that we don't have any creation issues, double check the points
        // line up
        if (samp.getWrap() == Alembic::AbcGeom::kPeriodic)
        {
            form = MFnNurbsCurve::kPeriodic;

            if (degree > 1 && numVerts > (int)(degree * 2))
            {
                for (unsigned int j = 0; j < degree; ++j)
                {
                    if (cvs[j] != cvs[numVerts-degree+j])
                    {
                        form = MFnNurbsCurve::kOpen;
                        break;
                    }
                }
            }
            else
            {
                form = MFnNurbsCurve::kOpen;
            }

            if (form == MFnNurbsCurve::kOpen && numVerts > 2 &&
                cvs[0] == cvs[numVerts-1])
            {
                form = MFnNurbsCurve::kClosed;
            }
        }

        MDoubleArray knots;

        if (!sampKnots)
        {
            // for now evenly distribute the knots for non NURBS curves
            int numKnots = numVerts + degree - 1;

            for (j = 0; j < numKnots; ++j)
            {
                knots.append(j/(double)(numKnots-1));
            }

            // we need to make sure the first 3 and last 3 knots repeat
            if (form != MFnNurbsCurve::kPeriodic && degree == 3 && numKnots > 2)
            {
                knots[1] = knots[0];
                knots[2] = knots[0];
                knots[numKnots-2] = knots[numKnots-1];
                knots[numKnots-3] = knots[numKnots-1];
            }
        }
        else if (numVerts > 0)
        {
            int numKnots = numVerts + degree + 1;

            // skip the first and last knot
            for (j = 1; j < numKnots - 1; ++j)
            {
                float knot = (*sampKnots)[curKnot + j];

                if (interp)
                {
                    float ceilKnot = (*ceilKnots)[curKnot + j];
                    knots.append(simpleLerp<float>(alpha, knot, ceilKnot));
                }
                else
                {
                    knots.append(knot);
                }
            }
            curKnot += numKnots;
        }

        MFnNurbsCurveData curveData;
        MObject curveDataObj = curveData.create();
        ioCurveObjects.push_back(curveDataObj);
        MFnNurbsCurve curve;
        curve.create(cvs, knots, degree, form, false, true,
            curveDataObj);
    }

    // push Nulls for extra curves
    for (std::size_t i = numCurves; i < iExpectedCurves; ++i)
    {
        MFnNurbsCurveData curveData;
        MObject curveDataObj = curveData.create();
        ioCurveObjects.push_back(curveDataObj);
    }

    return status;
}

// mObject: if only one curve in the curve group, returns the curve node;
// otherwise returns the transform node of the curve group
MObject createCurves(const std::string & iName,
    Alembic::AbcGeom::ICurvesSchema::Sample & iSample,
    Alembic::AbcGeom::IFloatGeomParam::Sample & iWidths,
    MObject & iParent, std::vector< MObject > & ioCurves, bool isAnimated)
{
    MObject returnObj;

    std::size_t numCurves = iSample.getNumCurves();
    Alembic::Abc::FloatArraySamplePtr widths = iWidths.getVals();
    Alembic::Abc::Int32ArraySamplePtr curvesNumVertices =
        iSample.getCurvesNumVertices();
    Alembic::Abc::P3fArraySamplePtr positions = iSample.getPositions();
    Alembic::Abc::FloatArraySamplePtr knotsSamp = iSample.getKnots();
    Alembic::Abc::UcharArraySamplePtr ordersSamp = iSample.getOrders();

    MString name(iName.c_str());

    MObject parent = iParent;
    if (numCurves > 1)
    {
        MFnTransform fnTrans;

        parent = fnTrans.create(iParent);
        fnTrans.setName(name);
        returnObj = parent;

        if (parent.isNull())
        {
            MString theError("Failed to create curves group parent node: ");
            theError += iName.c_str();
            printError(theError);
            return returnObj;
        }

        MFnNumericAttribute attr;
        MObject attrObj = attr.create("riCurves", "riCurves",
            MFnNumericData::kBoolean, 1);
        fnTrans.addAttribute(attrObj);

        // constant width
        if (widths && widths->size() == 1)
        {
            MFnNumericAttribute widthAttr;
            attrObj = widthAttr.create("width", "width",
                MFnNumericData::kFloat, (*widths)[0]);
            fnTrans.addAttribute(attrObj);
            fnTrans.findPlug("width", true).setValue((*widths)[0]);
        }
    }

    std::size_t curVert = 0;
    std::size_t curKnot = 0;
    for (std::size_t i = 0; i < numCurves; ++i)
    {
        unsigned int degree = 1;
        if (iSample.getType() == Alembic::AbcGeom::kCubic)
        {
            degree = 3;
        }
        else if (iSample.getType() == Alembic::AbcGeom::kVariableOrder &&
                 ordersSamp->size() > i)
        {
            degree = (unsigned int)((*ordersSamp)[i] - 1);
        }

        MPointArray cvs;

        int numVerts = (*curvesNumVertices)[i];

        int j;
        for (j = 0; j < numVerts; ++j, ++curVert)
        {
            Alembic::Abc::V3f pos = (*positions)[curVert];
            cvs.append(pos.x, pos.y, pos.z);
        }

        MFnNurbsCurve::Form form = MFnNurbsCurve::kOpen;

        // so that we don't have any creation issues, double check the points
        // line up
        if (iSample.getWrap() == Alembic::AbcGeom::kPeriodic)
        {
            form = MFnNurbsCurve::kPeriodic;

            if (degree > 1 && numVerts > (int)(degree * 2))
            {
                for (unsigned int j = 0; j < degree; ++j)
                {
                    if (cvs[j] != cvs[numVerts-degree+j])
                    {
                        form = MFnNurbsCurve::kOpen;
                        break;
                    }
                }
            }
            else
            {
                form = MFnNurbsCurve::kOpen;
            }

            if (form == MFnNurbsCurve::kOpen && numVerts > 2 &&
                cvs[0] == cvs[numVerts-1])
            {
                form = MFnNurbsCurve::kClosed;
            }
        }

        MDoubleArray knots;

        if (!knotsSamp)
        {
            // for now evenly distribute the knots for non NURBS curves
            int numKnots = numVerts + degree - 1;

            for (j = 0; j < numKnots; ++j)
            {
                knots.append(j/(double)(numKnots-1));
            }

            // we need to make sure the first 3 and last 3 knots repeat
            if (form != MFnNurbsCurve::kPeriodic && degree == 3 && numKnots > 2)
            {
                knots[1] = knots[0];
                knots[2] = knots[0];
                knots[numKnots-2] = knots[numKnots-1];
                knots[numKnots-3] = knots[numKnots-1];
            }
        }
        else if (numVerts > 0)
        {
            int numKnots = numVerts + degree + 1;

            // skip the first and last knot
            for (j = 1; j < numKnots - 1; ++j)
            {
                float knot = (*knotsSamp)[curKnot + j];
                knots.append(knot);
            }
            curKnot += numKnots;
        }

        MFnNurbsCurve curve;
        MObject curveObj = curve.create(cvs, knots, degree, form, false, true,
            parent);

        curve.setName(name);

        if (isAnimated)
        {
            ioCurves.push_back(curveObj);
        }

        if (numCurves == 1)
        {
            returnObj = curveObj;
        }

        // constant width, 1 curve just put it on the curve shape
        if (numCurves == 1 && widths && widths->size() == 1 &&
            iWidths.getScope() ==  Alembic::AbcGeom::kConstantScope)
        {
            MFnNumericAttribute widthAttr;
            MObject attrObj = widthAttr.create("width",
                "width", MFnNumericData::kFloat, (*widths)[0]);
            curve.addAttribute(attrObj);
            curve.findPlug("width", true).setValue((*widths)[0]);
        }
        else if (widths && widths->size() == numCurves &&
                 iWidths.getScope() ==  Alembic::AbcGeom::kUniformScope)
        {
            MFnNumericAttribute widthAttr;
            MObject attrObj = widthAttr.create("width",
                "width", MFnNumericData::kFloat, (*widths)[i]);
            curve.addAttribute(attrObj);
            curve.findPlug("width", true).setValue((*widths)[i]);
        }
        // per vertex width
        else if (widths && widths->size() >= curVert && numVerts > 0 &&
            iWidths.getScope() ==  Alembic::AbcGeom::kVertexScope)
        {
            MDoubleArray array((const float *)(
                &(*widths)[curVert-numVerts]), numVerts);
            MFnDoubleArrayData mFn;
            MObject attrObject = mFn.create(array);
            MFnGenericAttribute attr(attrObject);
            MObject attrObj = attr.create("width", "width");
            attr.addDataAccept(MFnData::kDoubleArray);
            MFnDependencyNode mParentFn(curve.object());
            mParentFn.addAttribute(attrObj);

            MPlug plug(curveObj, attrObj);
            plug.setValue(attrObject);
        }

    }


    return returnObj;
}
