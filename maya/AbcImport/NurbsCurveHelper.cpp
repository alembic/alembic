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

#include "NurbsCurveHelper.h"
#include "util.h"

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
    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame, schema.getTimeSampling(),
        schema.getNumSamples(), index, ceilIndex);

    Alembic::AbcGeom::ICurvesSchema::Sample samp, ceilSamp;
    schema.get(samp, index);

    unsigned int numCurves = samp.getNumCurves();

    bool interp = false;
    if (alpha != 0.0 && index != ceilIndex)
    {
        schema.get(ceilSamp, ceilIndex);
        if (ceilSamp.getNumCurves() == numCurves)
        {
            interp = true;
        }
    }

    Alembic::Abc::V3fArraySamplePtr sampPoints = samp.getPositions();
    Alembic::Abc::V3fArraySamplePtr ceilPoints = ceilSamp.getPositions();

    Alembic::Abc::UInt32ArraySamplePtr numVertices =
        samp.getCurvesNumVertices();
    Alembic::Abc::UInt32ArraySamplePtr ceilNumVertices =
        ceilSamp.getCurvesNumVertices();

    if (interp && sampPoints->size() != ceilPoints->size())
    {
        interp = false;
    }

    std::size_t curVert = 0;
    for (std::size_t i = 0; i < iExpectedCurves && i < numCurves; ++i)
    {
        MPointArray cvs;
        MDoubleArray knots;

        int numVerts = (*numVertices)[i];

        for (int j = 0; j < numVerts; ++j, ++curVert)
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

            knots.append(j/(float)(numVerts-1));
        }

        MFnNurbsCurveData curveData;
        MObject curveDataObj = curveData.create();
        ioCurveObjects.push_back(curveDataObj);
        MFnNurbsCurve curve;
        curve.create(cvs, knots, 1, MFnNurbsCurve::kOpen, false, true,
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
    MObject & iParent, std::vector< MObject > & ioCurves, bool isAnimated)
{
    MObject returnObj;

    std::size_t numCurves = iSample.getNumCurves();
    Alembic::Abc::FloatArraySamplePtr widths = iSample.getWidths();
    Alembic::Abc::UInt32ArraySamplePtr curvesNumVertices =
        iSample.getCurvesNumVertices();
    Alembic::Abc::V3fArraySamplePtr positions = iSample.getPositions();

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
        fnTrans.addAttribute(attrObj,
            MFnDependencyNode::kLocalDynamicAttr);

        // constant width
        if (widths && widths->size() == 1)
        {
            MFnNumericAttribute widthAttr;
            attrObj = widthAttr.create("constantwidth", "constantwidth",
                MFnNumericData::kFloat, (*widths)[0]);
            fnTrans.addAttribute(attrObj,
                MFnDependencyNode::kLocalDynamicAttr);
        }
    }

    unsigned int degree = 1; // samp.getDegree();
    std::size_t curVert = 0;
    for (std::size_t i = 0; i < numCurves; ++i)
    {
        MPointArray cvs;
        MDoubleArray knots;

        int numVerts = (*curvesNumVertices)[i];

        for (int j = 0; j < numVerts; ++j, ++curVert)
        {
            Alembic::Abc::V3f pos = (*positions)[curVert];
            cvs.append(pos.x, pos.y, pos.z);
            knots.append(j/(float)(numVerts-1));
        }

        MFnNurbsCurve curve;
        MObject curveObj = curve.create(cvs, knots, 1,
            MFnNurbsCurve::kOpen, false, true, parent);
        curve.setName(name);

        if (isAnimated)
        {
            ioCurves.push_back(curveObj);
        }

        if (numCurves == 1)
        {
            returnObj = curveObj;

            // constant width
            if (widths && widths->size() == 1)
            {
                MFnNumericAttribute widthAttr;
                MObject attrObj = widthAttr.create("constantwidth",
                    "constantwidth", MFnNumericData::kFloat, (*widths)[0]);
                curve.addAttribute(attrObj,
                    MFnDependencyNode::kLocalDynamicAttr);
            }
        }

        // deal with other types of width
    }


    return returnObj;
}
