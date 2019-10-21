//-*****************************************************************************
//
// Copyright (c) 2009-2014,
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

    void collectNurbsCurves(const MDagPath &dagPath, bool iExcludeInvisible,
        MDagPathArray &dagPaths, bool & oIsAnimated)
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
                        curve.hasFn(MFn::kNurbsCurve) &&
                        (!iExcludeInvisible || util::isRenderable(curve)) )
                    {
                        dagPaths.append(curvePath);
                    }
                    // don't bother checking the animated state if the curve
                    // wasn't appended to the list
                    else
                    {
                        continue;
                    }

                    // with the flag set to true, check the DagPath and its
                    // parent.
                    // Note since we're collecting a group of curves, and
                    // if  even one is animated, the whole group will be,
                    // so don't bother checking additional curves.
                    if (!oIsAnimated)
                    {
                        if (util::isAnimated(curve, true))
                        {
                            oIsAnimated = true;
                        }
                        MObject curveXform(curvePath.transform());
                        if (util::isAnimated(curveXform, true))
                        {
                            oIsAnimated = true;
                        }
                    }
                }
            }
        }
    }  // end of function collectNurbsCurves
}

MayaNurbsCurveWriter::MayaNurbsCurveWriter(MDagPath & iDag,
    Alembic::Abc::OObject & iParent, Alembic::Util::uint32_t iTimeIndex,
    bool iIsCurveGrp, const JobArgs & iArgs) :
    mIsAnimated(false), mRootDagPath(iDag), mIsCurveGrp(iIsCurveGrp)
{
    MStatus stat;
    MFnDependencyNode fnDepNode(iDag.node(), &stat);
    MString name = fnDepNode.name();

    if (mIsCurveGrp)
    {
        collectNurbsCurves(mRootDagPath, iArgs.excludeInvisible,
            mNurbsCurves, mIsAnimated);

        // if no curves were found bail early
        if (mNurbsCurves.length() == 0)
            return;
    }
    else
    {
        MObject curve = iDag.node();

        if (iTimeIndex != 0 && util::isAnimated(curve))
        {
            mIsAnimated = true;
        }
        else
        {
            iTimeIndex = 0;
        }
    }

    name = util::stripNamespaces(name, iArgs.stripNamespace);

    Alembic::AbcGeom::OCurves obj(iParent, name.asChar(), iTimeIndex);
    mSchema = obj.getSchema();

    Alembic::Abc::OCompoundProperty cp;
    Alembic::Abc::OCompoundProperty up;
    if (AttributesWriter::hasAnyAttr(fnDepNode, iArgs))
    {
        cp = mSchema.getArbGeomParams();
        up = mSchema.getUserProperties();
    }

    mAttrs = AttributesWriterPtr(new AttributesWriter(cp, up, obj, fnDepNode,
        iTimeIndex, iArgs, true));

    if (!mIsAnimated || iArgs.setFirstAnimShape)
    {
        write();
    }
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
    samp.setBasis(Alembic::AbcGeom::kBsplineBasis);

    MStatus stat;
    mCVCount = 0;

    // if inheritTransform is on and the curve group is animated,
    // bake the cv positions in the world space
    MMatrix exclusiveMatrixInv = mRootDagPath.exclusiveMatrixInverse(&stat);

    std::size_t numCurves = 1;

    if (mIsCurveGrp)
        numCurves = mNurbsCurves.length();

    std::vector<Alembic::Util::int32_t> nVertices(numCurves);
    std::vector<float> points;
    std::vector<float> width;
    std::vector<float> knots;
    std::vector<Alembic::Util::uint8_t> orders(numCurves);

    MMatrix transformMatrix;
    bool useConstWidth = false;

    MFnDependencyNode dep(mRootDagPath.transform());
    MPlug constWidthPlug = dep.findPlug("width", true);

    if (!constWidthPlug.isNull())
    {
        useConstWidth = true;
        width.push_back(constWidthPlug.asFloat());
    }

    for (unsigned int i = 0; i < numCurves; i++)
    {
        MFnNurbsCurve curve;
        if (mIsCurveGrp)
        {
            curve.setObject(mNurbsCurves[i]);
            MMatrix inclusiveMatrix = mNurbsCurves[i].inclusiveMatrix(&stat);
            transformMatrix = inclusiveMatrix*exclusiveMatrixInv;
        }
        else
        {
            curve.setObject(mRootDagPath.node());
        }

        if (i == 0)
        {
            if (curve.form() == MFnNurbsCurve::kOpen)
            {
                samp.setWrap(Alembic::AbcGeom::kNonPeriodic);
            }
            else
            {
                samp.setWrap(Alembic::AbcGeom::kPeriodic);
            }

            if (curve.degree() == 3)
            {
                samp.setType(Alembic::AbcGeom::kCubic);
            }
            else if (curve.degree() == 1)
            {
                samp.setType(Alembic::AbcGeom::kLinear);
            }
            else
            {
                samp.setType(Alembic::AbcGeom::kVariableOrder);
            }
        }
        else
        {
            if (curve.form() == MFnNurbsCurve::kOpen)
            {
                samp.setWrap(Alembic::AbcGeom::kNonPeriodic);
            }

            if ((samp.getType() == Alembic::AbcGeom::kCubic &&
                curve.degree() != 3) ||
                (samp.getType() == Alembic::AbcGeom::kLinear &&
                curve.degree() != 1))
            {
                samp.setType(Alembic::AbcGeom::kVariableOrder);
            }
        }

        orders[i] = static_cast<Alembic::Util::uint8_t>(curve.degree() + 1);

        Alembic::Util::int32_t numCVs = curve.numCVs(&stat);

        MPointArray cvArray;
        stat = curve.getCVs(cvArray, MSpace::kObject);

        mCVCount += numCVs;
        nVertices[i] = numCVs;

        for (Alembic::Util::int32_t j = 0; j < numCVs; j++)
        {
            MPoint transformdPt;
            if (mIsCurveGrp)
            {
                transformdPt = cvArray[j]*transformMatrix;
            }
            else
            {
                transformdPt = cvArray[j];
            }

            points.push_back(static_cast<float>(transformdPt.x));
            points.push_back(static_cast<float>(transformdPt.y));
            points.push_back(static_cast<float>(transformdPt.z));
        }

        MDoubleArray knotsArray;
        curve.getKnots(knotsArray);
        knots.reserve(knotsArray.length() + 2);

        // need to add a knot to the start and end (M + 2N + 1)
        if (knotsArray.length() > 1)
        {
            unsigned int knotsLength = knotsArray.length();
            if (knotsArray[0] == knotsArray[knotsLength - 1] ||
                knotsArray[0] == knotsArray[1])
            {
                knots.push_back(static_cast<float>(knotsArray[0]));
            }
            else
            {
                knots.push_back(static_cast<float>(2 * knotsArray[0] - knotsArray[1]));
            }

            for (unsigned int j = 0; j < knotsLength; ++j)
            {
                knots.push_back(static_cast<float>(knotsArray[j]));
            }

            if (knotsArray[0] == knotsArray[knotsLength - 1] ||
                knotsArray[knotsLength - 1] == knotsArray[knotsLength - 2])
            {
                knots.push_back(static_cast<float>((knotsArray[knotsLength - 1])));
            }
            else
            {
                knots.push_back(static_cast<float>(2 * knotsArray[knotsLength - 1] -
                                knotsArray[knotsLength - 2]));
            }
        }

        // width
        MPlug widthPlug = curve.findPlug("width", true);

        if (!useConstWidth && !widthPlug.isNull())
        {
            MObject widthObj;
            MStatus status = widthPlug.getValue(widthObj);
            MFnDoubleArrayData fnDoubleArrayData(widthObj, &status);
            MDoubleArray doubleArrayData = fnDoubleArrayData.array();
            Alembic::Util::int32_t arraySum = doubleArrayData.length();
            if (arraySum == numCVs)
            {
                for (Alembic::Util::int32_t i = 0; i < arraySum; i++)
                {
                    width.push_back(static_cast<float>(doubleArrayData[i]));
                }
            }
            // successfully got as a double array but not enough values!
            else if (status == MS::kSuccess)
            {
                MString msg = "Curve ";
                msg += curve.partialPathName();
                msg += " has incorrect size for the width vector.";
                msg += "\nUsing default constant width of 0.1.";
                MGlobal::displayWarning(msg);

                width.clear();
                width.push_back(0.1f);
                useConstWidth = true;
            }
            else
            {
                width.push_back(widthPlug.asFloat());
            }
        }
        // no width plug on the curve, but not yet noticed as constant width
        // force constant
        else if (!useConstWidth)
        {
            // pick a default value
            width.clear();
            width.push_back(0.1f);
            useConstWidth = true;
        }
    }

    Alembic::AbcGeom::GeometryScope scope = Alembic::AbcGeom::kVertexScope;
    if (!useConstWidth && width.size() > 1 && width.size() == numCurves)
    {
        scope = Alembic::AbcGeom::kUniformScope;
    }
    else if (width.size() == 1)
    {
        scope = Alembic::AbcGeom::kConstantScope;
    }


    samp.setCurvesNumVertices(Alembic::Abc::Int32ArraySample(nVertices));
    samp.setPositions(Alembic::Abc::V3fArraySample(
        (const Imath::V3f *)&points.front(), points.size() / 3 ));
    samp.setWidths(Alembic::AbcGeom::OFloatGeomParam::Sample(
        Alembic::Abc::FloatArraySample(width), scope) );

    if (samp.getType() == Alembic::AbcGeom::kVariableOrder)
    {
        samp.setOrders(Alembic::Abc::UcharArraySample(orders));
    }

    if (!knots.empty())
    {
        samp.setKnots(Alembic::Abc::FloatArraySample(knots));
    }

    mSchema.set(samp);
}
