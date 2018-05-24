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
#include "NurbsSurfaceHelper.h"
#include "NodeIteratorVisitorHelper.h"

#include <maya/MString.h>
#include <maya/MDagPath.h>
#include <maya/MDoubleArray.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MGlobal.h>
#include <maya/MObjectArray.h>
#include <maya/MTrimBoundaryArray.h>
#include <maya/MFnTransform.h>
#include <maya/MCommandResult.h>
#include <maya/MDagModifier.h>

namespace
{

    // This function repacks trim curves information back into Maya format
    void trimSurface( Alembic::AbcGeom::INuPatchSchema::Sample & iSamp,
        MFnNurbsSurface & iFnSurface)
    {

        Alembic::Util::int32_t numLoops = iSamp.getTrimNumLoops();
        if (numLoops == 0)
            return;

        MTrimBoundaryArray trimBoundaryArray;

        // this is for getting V back into Maya's coordinate system
        double startU, endU, startV, endV;
        iFnSurface.getKnotDomain(startU, endU, startV, endV);
        double offsetV = startV + endV;

        MStatus status = MS::kSuccess;

        Alembic::Abc::Int32ArraySamplePtr numCurvesPerLoop =
            iSamp.getTrimNumCurves();
        Alembic::Abc::Int32ArraySamplePtr numCurveVerts =
            iSamp.getTrimNumVertices();
        Alembic::Abc::Int32ArraySamplePtr orders = iSamp.getTrimOrders();
        Alembic::Abc::FloatArraySamplePtr knots = iSamp.getTrimKnots();
        Alembic::Abc::FloatArraySamplePtr uVert = iSamp.getTrimU();
        Alembic::Abc::FloatArraySamplePtr vVert = iSamp.getTrimV();
        Alembic::Abc::FloatArraySamplePtr wVert = iSamp.getTrimW();

        Alembic::Util::int32_t curCurve = 0;
        Alembic::Util::int32_t curPos = 0;
        Alembic::Util::int32_t curKnot = 0;

        for (Alembic::Util::int32_t i = 0; i < numLoops; ++i)
        {
            MObjectArray trimLoop;

            Alembic::Util::int32_t numCurves = (*numCurvesPerLoop)[i];
            for (Alembic::Util::int32_t j = 0; j < numCurves; ++j, ++curCurve)
            {
                unsigned int degree = (*orders)[curCurve] - 1;
                Alembic::Util::int32_t numVerts = (*numCurveVerts)[curCurve];
                Alembic::Util::int32_t numKnots = numVerts + degree + 1;

                MPointArray cvs;
                cvs.setLength(numVerts);
                for (Alembic::Util::int32_t k=0 ; k<numVerts; ++k, ++curPos)
                {
                    double x = (*uVert)[curPos];
                    double y = (*vVert)[curPos];
                    double w = (*wVert)[curPos];
                    cvs.set(k, x, offsetV-y, 0.0, w);
                }

                MDoubleArray dknots;
                dknots.setLength(numKnots - 2);
                ++curKnot;
                for (Alembic::Util::int32_t k = 1; k < numKnots - 1;
                    ++k, ++curKnot)
                {
                    dknots.set((*knots)[curKnot], k - 1);
                }
                ++curKnot;

                MFnNurbsCurve fnCurve;

                // when a 2D curve is created without a parent, the function
                // returns the transform node of the curve created.
                // The transform node as well as the curve node need to be
                // deleted once the trim is done, because after all this is not
                // the equivalent of "curveOnSurface" command
                MFnNurbsCurveData data;
                MObject curveData = data.create();
                MObject curve2D = fnCurve.create(cvs, dknots, degree,
                    MFnNurbsCurve::kOpen, true, true, curveData, &status);

                if (status == MS::kSuccess)
                {
                    trimLoop.append(curveData);
                }
            }
            trimBoundaryArray.append(trimLoop);
        }

        MTrimBoundaryArray oneRegion;
        for (unsigned int i = 0; i < trimBoundaryArray.length(); i++)
        {
            if (i > 0)
            {
                MObject loopData = trimBoundaryArray.getMergedBoundary(i, &status);
                if (status != MS::kSuccess) continue;

                MFnNurbsCurve loop(loopData, &status);
                if (status != MS::kSuccess) continue;

                // Check whether this loop is an outer boundary.
                bool isOuterBoundary = false;

                double       length  = loop.length();
                unsigned int segment = std::max(loop.numCVs(), 10);

                MPointArray curvePoints;
                curvePoints.setLength(segment);

                for (unsigned int j = 0; j < segment; j++)
                {
                    double param = loop.findParamFromLength(length * j / segment);
                    loop.getPointAtParam(param, curvePoints[j]);
                }

                // Find the right most curve point
                MPoint       rightMostPoint = curvePoints[0];
                unsigned int rightMostIndex = 0;
                for (unsigned int j = 0; j < curvePoints.length(); j++)
                {
                    if (rightMostPoint.x < curvePoints[j].x)
                    {
                        rightMostPoint = curvePoints[j];
                        rightMostIndex = j;
                    }
                }

                // Find the vertex just before and after the right most vertex
                int beforeIndex = (rightMostIndex == 0) ? curvePoints.length() - 1 : rightMostIndex - 1;
                int afterIndex  = (rightMostIndex == curvePoints.length() - 1) ? 0 : rightMostIndex + 1;

                for (unsigned int j = 0; j < curvePoints.length(); j++)
                {
                    if (fabs(curvePoints[beforeIndex].x - curvePoints[rightMostIndex].x) < 1e-5)
                    {
                        beforeIndex = (beforeIndex == 0) ? curvePoints.length() - 1 : beforeIndex - 1;
                    }
                }

                for (unsigned int j = 0; j < curvePoints.length(); j++)
                {
                    if (fabs(curvePoints[afterIndex].x - curvePoints[rightMostIndex].x) < 1e-5)
                    {
                        afterIndex = (afterIndex == (int)(curvePoints.length()) - 1) ? 0 : afterIndex + 1;
                    }
                }

                // failed. not a closed curve.
                if (fabs(curvePoints[afterIndex].x - curvePoints[rightMostIndex].x) < 1e-5 &&
                    fabs(curvePoints[beforeIndex].x - curvePoints[rightMostIndex].x) < 1e-5)
                {
                    continue;
                }

                if (beforeIndex < 0)
                    beforeIndex += curvePoints.length();
                if (beforeIndex >= (int)(curvePoints.length()))
                    beforeIndex = beforeIndex & curvePoints.length();
                if (afterIndex < 0)
                    afterIndex += curvePoints.length();
                if (afterIndex >= (int)(curvePoints.length()))
                    afterIndex = afterIndex & curvePoints.length();

                // Compute the cross product
                MVector vector1 = curvePoints[beforeIndex] - curvePoints[rightMostIndex];
                MVector vector2 = curvePoints[afterIndex]  - curvePoints[rightMostIndex];
                if ((vector1 ^ vector2).z < 0)
                {
                    isOuterBoundary = true;
                }

                // Trim the NURBS surface. An outer boundary starts a new region.
                if (isOuterBoundary)
                {
#if MAYA_API_VERSION < 201300
                    status = iFnSurface.trimWithBoundaries(oneRegion);
#else
                    status = iFnSurface.trimWithBoundaries(oneRegion, false, 1e-3, 1e-5, true);
#endif
                    if (status != MS::kSuccess)
                    {
                        MGlobal::displayError(
                            "Trimming Nurbs Surface outer failed.");
                    }
                    oneRegion.clear();
                }
            }

            oneRegion.append(trimBoundaryArray[i]);
        }

#if MAYA_API_VERSION < 201300
        status = iFnSurface.trimWithBoundaries(oneRegion);
#else
        status = iFnSurface.trimWithBoundaries(oneRegion, false, 1e-3, 1e-5, true);
#endif
        if (status != MS::kSuccess)
        {
            MGlobal::displayError("Trimming Nurbs Surface failed.");
        }
    }
}

MObject readNurbs(double iFrame, Alembic::AbcGeom::INuPatch & iNode,
    MObject & iObject)
{
    MStatus status;
    MObject obj;

    Alembic::AbcGeom::INuPatchSchema schema = iNode.getSchema();

    // no interpolation for now
    Alembic::AbcCoreAbstract::index_t index, ceilIndex;
    getWeightAndIndex(iFrame, schema.getTimeSampling(),
        schema.getNumSamples(), index, ceilIndex);

    Alembic::AbcGeom::INuPatchSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));

    Alembic::Abc::P3fArraySamplePtr pos = samp.getPositions();
    Alembic::Abc::FloatArraySamplePtr weights = samp.getPositionWeights();

    MString surfaceName(iNode.getName().c_str());

    unsigned int degreeU  = samp.getUOrder() > 0 ? samp.getUOrder() - 1 : 0;
    unsigned int degreeV  = samp.getVOrder() > 0 ? samp.getVOrder() - 1 : 0;
    unsigned int numCVInU = samp.getNumU();
    unsigned int numCVInV = samp.getNumV();

    // cv points
    unsigned int numCV = numCVInU*numCVInV;
    unsigned int curPos = 0;

    MPointArray controlVertices;
    controlVertices.setLength(numCV);

    for (unsigned int v = 0; v < numCVInV; ++v)
    {
        for (unsigned int u = 0; u < numCVInU; ++u, ++curPos)
        {
            unsigned int mayaIndex = u * numCVInV + (numCVInV - v - 1);
            MPoint pt((*pos)[curPos].x, (*pos)[curPos].y, (*pos)[curPos].z);

            if (weights)
            {
                pt.w = (*weights)[curPos];
            }

            // go from u,v order to reversed v, u order
            controlVertices.set(pt, mayaIndex);
        }
    }

    // Nurbs form
    // Alemblic file does not record the form of nurb surface, we get the form
    // by checking the CV data. If the first degree number CV overlap the last
    // degree number CV, then the form is kPeriodic. If only the first CV overlaps
    // the last CV, then the form is kClosed.
    MFnNurbsSurface::Form formU = MFnNurbsSurface::kPeriodic;
    MFnNurbsSurface::Form formV = MFnNurbsSurface::kPeriodic;
    // Check all curves
    bool notOpen = true;
    for (unsigned int v = 0; notOpen && v < numCVInV; v++) {
        for (unsigned int u = 0; u < degreeU; u++) {
            unsigned int firstIndex = u * numCVInV + (numCVInV - v - 1);
            unsigned int lastPeriodicIndex = (numCVInU - degreeU + u) * numCVInV + (numCVInV - v - 1);
            if (!controlVertices[firstIndex].isEquivalent(controlVertices[lastPeriodicIndex])) {
                formU = MFnNurbsSurface::kOpen;
                notOpen = false;
                break;
            }
        }
    }

    if (formU == MFnNurbsSurface::kOpen) {
        formU = MFnNurbsSurface::kClosed;
        for (unsigned int v = 0; v < numCVInV; v++) {
            unsigned int lastUIndex = (numCVInU - 1) * numCVInV + (numCVInV - v - 1);
            if (! controlVertices[numCVInV-v-1].isEquivalent(controlVertices[lastUIndex])) {
                formU = MFnNurbsSurface::kOpen;
                break;
            }
        }
    }

    notOpen = true;
    for (unsigned int u = 0; notOpen && u < numCVInU; u++) {
        for (unsigned int v = 0; v < degreeV; v++) {
            unsigned int firstIndex = u * numCVInV + (numCVInV - v - 1);
            unsigned int lastPeriodicIndex = u * numCVInV + (degreeV - v - 1); //numV - (numV - vDegree + v) - 1;
            if (!controlVertices[firstIndex].isEquivalent(controlVertices[lastPeriodicIndex])) {
                formV = MFnNurbsSurface::kOpen;
                notOpen = false;
                break;
            }
        }
    }
    if (formV == MFnNurbsSurface::kOpen) {
        formV = MFnNurbsSurface::kClosed;
        for (unsigned int u = 0; u < numCVInU; u++) {
            if (! controlVertices[u * numCVInV + (numCVInV-1)].isEquivalent(controlVertices[u * numCVInV])) {
                formV = MFnNurbsSurface::kOpen;
                break;
            }
        }
    }


    Alembic::Abc::FloatArraySamplePtr uKnot = samp.getUKnot();
    Alembic::Abc::FloatArraySamplePtr vKnot = samp.getVKnot();

    unsigned int numKnotsInU = 0;
    if (uKnot->size() > 2)
    {
        numKnotsInU = static_cast<unsigned int>(uKnot->size() - 2);
    }
    MDoubleArray uKnotSequences;
    uKnotSequences.setLength(numKnotsInU);
    for (unsigned int i = 0; i < numKnotsInU; ++i)
    {
        uKnotSequences.set((*uKnot)[i+1], i);
    }

    unsigned int numKnotsInV =  0;
    if (vKnot->size() > 2)
    {
        numKnotsInV = static_cast<unsigned int>(vKnot->size() - 2);
    }
    MDoubleArray vKnotSequences;
    vKnotSequences.setLength(numKnotsInV);
    for (unsigned int i = 0; i < numKnotsInV; i++)
    {
        vKnotSequences.set((*vKnot)[i+1], i);
    }

    // Node creation try the API first
    MFnNurbsSurface mFn;
    obj = mFn.create(controlVertices, uKnotSequences, vKnotSequences,
        degreeU, degreeV, formU, formV,
        true, iObject, &status);

    // something went wrong, try open/open create
    if (status != MS::kSuccess && (formU != MFnNurbsSurface::kOpen ||
        formV != MFnNurbsSurface::kOpen))
    {
        obj = mFn.create(controlVertices, uKnotSequences, vKnotSequences,
            degreeU, degreeV,  MFnNurbsSurface::kOpen,  MFnNurbsSurface::kOpen,
            true, iObject, &status);
    }

    if (status == MS::kSuccess)
    {
        mFn.setName(surfaceName);
    }
    else
    {
        MGlobal::displayError(status.errorString());
        MString errorMsg = "Could not create Nurbs Surface: ";
        errorMsg += surfaceName;
        MGlobal::displayError(errorMsg);
    }

    trimSurface(samp, mFn);

    return obj;
}

MObject createNurbs(double iFrame, Alembic::AbcGeom::INuPatch & iNode,
    MObject & iParent)
{
    // normally we'd only want to do the read if we had only 1 sample, but
    // since we can't seem to create an empty Nurbs surface like we can a mesh
    // we will read the data (eventually multiple times on creation)
    MObject obj = readNurbs(iFrame, iNode, iParent);

    if (!obj.isNull())
    {
        MFnNurbsSurface fn(obj);
        setInitialShadingGroup(fn.partialPathName());
    }

    return obj;
}
