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

#include "MayaNurbsSurfaceWriter.h"
#include "MayaUtility.h"

MayaNurbsSurfaceWriter::MayaNurbsSurfaceWriter(MDagPath & iDag,
    Alembic::Abc::OObject & iParent, Alembic::Util::uint32_t iTimeIndex,
    const JobArgs & iArgs) :
    mIsSurfaceAnimated(false), mDagPath(iDag)
{
    MStatus stat;
    MObject surface = iDag.node();

    MFnNurbsSurface nurbs(mDagPath, &stat);
    if (!stat)
    {
        MGlobal::displayError(
            "MFnNurbsSurface() failed for MayaNurbsSurfaceWriter" );
    }

    MString name = nurbs.name();
    name = util::stripNamespaces(name, iArgs.stripNamespace);


    Alembic::AbcGeom::ONuPatch obj(iParent, name.asChar(), iTimeIndex);
    mSchema = obj.getSchema();

    Alembic::Abc::OCompoundProperty cp;
    Alembic::Abc::OCompoundProperty up;
    if (AttributesWriter::hasAnyAttr(nurbs, iArgs))
    {
        cp = mSchema.getArbGeomParams();
        up = mSchema.getUserProperties();
    }

    mAttrs = AttributesWriterPtr(new AttributesWriter(cp, up, obj, nurbs,
        iTimeIndex, iArgs, true));

    // for now if it a trim surface, treat it like it's animated
    if ( iTimeIndex != 0 && (nurbs.isTrimmedSurface() ||
        util::isAnimated(surface)) )
    {
        mIsSurfaceAnimated = true;
    }
    else
    {
        iTimeIndex = 0;
    }

    if (!mIsSurfaceAnimated || iArgs.setFirstAnimShape)
    {
        write();
    }
}

bool MayaNurbsSurfaceWriter::isAnimated() const
{
    return  mIsSurfaceAnimated ||
            (mAttrs != NULL && mAttrs->isAnimated());
}


unsigned int MayaNurbsSurfaceWriter::getNumCVs()
{
    MFnNurbsSurface nurbs(mDagPath);
    return nurbs.numCVsInU() * nurbs.numCVsInV();
}


// the arrays being passed in are assumed to be empty
void MayaNurbsSurfaceWriter::write()
{
    MFnNurbsSurface nurbs(mDagPath);

    double startU, endU, startV, endV;
    nurbs.getKnotDomain(startU, endU, startV, endV);

    Alembic::AbcGeom::ONuPatchSchema::Sample samp;
    samp.setUOrder(nurbs.degreeU() + 1);
    samp.setVOrder(nurbs.degreeV() + 1);

    unsigned int numKnotsInU = nurbs.numKnotsInU();
    std::vector<float> sampKnotsInU;

    // guard against a degenerative case
    if (numKnotsInU > 1)
    {
        MDoubleArray knotsInU;
        nurbs.getKnotsInU(knotsInU);

        // pad the start and end with a knot on each side, since thats what
        // most apps, like Houdini and Renderman want these two extra knots
        sampKnotsInU.reserve(numKnotsInU+2);

        // push_back a dummy value, we will set it below
        sampKnotsInU.push_back(0.0);
        for (unsigned int i = 0; i < numKnotsInU; i++)
            sampKnotsInU.push_back( static_cast<float>(knotsInU[i]));

        double k1 = sampKnotsInU[1];
        double k2 = sampKnotsInU[2];
        double klast_1 = sampKnotsInU[numKnotsInU];
        double klast_2 = sampKnotsInU[numKnotsInU-1];

        sampKnotsInU[0] = static_cast<float>(2.0 * k1 - k2);
        sampKnotsInU.push_back(static_cast<float>(2.0 * klast_1 - klast_2));
        samp.setUKnot(Alembic::Abc::FloatArraySample(sampKnotsInU));
    }


    unsigned int numKnotsInV = nurbs.numKnotsInV();
    std::vector<float> sampKnotsInV;

    // do it for V
    if (numKnotsInV > 1)
    {
        MDoubleArray knotsInV;
        nurbs.getKnotsInV(knotsInV);

        // pad the start and end with a knot on each side, since thats what
        // most apps, like Houdini and Renderman want these two extra knots
        sampKnotsInV.reserve(numKnotsInV+2);

        // push_back a dummy value, we will set it below
        sampKnotsInV.push_back(0.0);
        for (unsigned int i = 0; i < numKnotsInV; i++)
            sampKnotsInV.push_back(static_cast<float>(knotsInV[i]));

        double k1 = sampKnotsInV[1];
        double k2 = sampKnotsInV[2];
        double klast_1 = sampKnotsInV[numKnotsInV];
        double klast_2 = sampKnotsInV[numKnotsInV-1];

        sampKnotsInV[0] = static_cast<float>(2.0 * k1 - k2);
        sampKnotsInV.push_back(static_cast<float>(2.0 * klast_1 - klast_2));
        samp.setVKnot(Alembic::Abc::FloatArraySample(sampKnotsInV));
    }

    // for closed and periodic we are saving duplicate information
    MPointArray cvArray;
    nurbs.getCVs(cvArray);
    unsigned int numCVs = cvArray.length();
    int numCVsInU = nurbs.numCVsInU();
    int numCVsInV = nurbs.numCVsInV();

    samp.setNu(numCVsInU);
    samp.setNv(numCVsInV);

    std::vector<Alembic::Abc::V3f> sampPos;
    sampPos.reserve(numCVs);

    std::vector<float> sampPosWeights;
    sampPosWeights.reserve(numCVs);
    bool weightsOne = true;

    // Maya stores the data where v varies the fastest (v,u order)
    // so we need to pack the data differently u,v order
    // (v reversed to make clockwise???)
    for (int v = numCVsInV - 1; v > -1; v--)
    {
        for (int u = 0; u < numCVsInU; u++)
        {
            int index = u * numCVsInV + v;
            sampPos.push_back(Alembic::Abc::V3f(
                static_cast<float>(cvArray[index].x),
                static_cast<float>(cvArray[index].y),
                static_cast<float>(cvArray[index].z) ));

            if (fabs(cvArray[index].w - 1.0) > 1e-12)
            {
                weightsOne = false;
            }
            sampPosWeights.push_back(static_cast<float>(cvArray[index].w));
        }
    }
    samp.setPositions(Alembic::Abc::V3fArraySample(sampPos));
    if (!weightsOne)
    {
        samp.setPositionWeights(Alembic::Abc::FloatArraySample(sampPosWeights));
    }

    if (!nurbs.isTrimmedSurface())
    {
        mSchema.set(samp);
        return;
    }

    unsigned int numRegions = nurbs.numRegions();

    // each boundary is a curvegroup, it can have multiple trim curve segments

    // A Maya's trimmed NURBS surface has multiple regions.
    // Inside a region, there are multiple boundaries.
    // There are one CCW outer boundary and optional CW inner boundaries.
    // Each boundary is a closed boundary and consists of multiple curves.

    // Alembic has the same semantic as RenderMan.
    // RenderMan's doc says: "The curves of a loop connect
    //   in head-to-tail fashion and must be explicitly closed. "

    // A Maya boundary is equivalent to an Alembic/RenderMan loop
    std::vector<Alembic::Util::int32_t> trimNumCurves;
    std::vector<Alembic::Util::int32_t> trimNumPos;
    std::vector<Alembic::Util::int32_t> trimOrder;
    std::vector<float> trimKnot;
    std::vector<float> trimMin;
    std::vector<float> trimMax;
    std::vector<float> trimU;
    std::vector<float> trimV;
    std::vector<float> trimW;

    Alembic::Util::int32_t numLoops = 0;
    for (unsigned int i = 0; i < numRegions; i++)
    {
        MTrimBoundaryArray result;

        // if the 3rd argument is set to be true, return the 2D curve
        nurbs.getTrimBoundaries(result, i, true);
        unsigned int numBoundaries = result.length();
        for (unsigned int j = 0; j < numBoundaries; j++)
        {
            MObjectArray boundary = result[j];
            unsigned int numTrimCurve = boundary.length();
            trimNumCurves.push_back(static_cast<Alembic::Util::int32_t>(numTrimCurve));
            numLoops++;
            for (unsigned int k = 0; k < numTrimCurve; k++)
            {
                MObject curveObj = boundary[k];
                if (curveObj.hasFn(MFn::kNurbsCurve))
                {
                    MFnNurbsCurve mFnCurve(curveObj);

                    Alembic::Util::int32_t numCVs = mFnCurve.numCVs();
                    trimNumPos.push_back(numCVs);
                    trimOrder.push_back(mFnCurve.degree()+1);

                    double start, end;
                    mFnCurve.getKnotDomain(start, end);
                    trimMin.push_back(static_cast<float>(start));
                    trimMax.push_back(static_cast<float>(end));

                    MPointArray cvArray;
                    mFnCurve.getCVs(cvArray);
                    //append to curveGrp.cv vector
                    double offsetV = startV+endV;
                    for (Alembic::Util::int32_t l = 0; l < numCVs; l++)
                    {
                        trimU.push_back(static_cast<float>(cvArray[l].x));

                        // v' = maxV + minV - v
                        // this is because we flipped v on the surface
                        trimV.push_back(
                            static_cast<float>(offsetV-cvArray[l].y));

                        trimW.push_back(static_cast<float>(cvArray[l].w));
                    }

                    MDoubleArray knot;
                    mFnCurve.getKnots(knot);
                    unsigned int numKnots = knot.length();

                    // push_back a dummy value, we will set it below
                    std::size_t totalNumKnots = trimKnot.size();
                    trimKnot.push_back(0.0);
                    for (unsigned int l = 0; l < numKnots; l++)
                    {
                        trimKnot.push_back(static_cast<float>(knot[l]));
                    }

                    // for a knot sequence with multiple end knots, duplicate
                    // the existing first and last knots once more.
                    // for a knot sequence with uniform end knots, create their
                    // the new knots offset at an interval equal to the existing
                    // first and last knot intervals
                    double k1 = trimKnot[totalNumKnots+1];
                    double k2 = trimKnot[totalNumKnots+2];
                    double klast_1 = trimKnot[trimKnot.size()-1];
                    double klast_2 = trimKnot[trimKnot.size()-2];
                    trimKnot[totalNumKnots] = static_cast<float>(2.0 * k1 - k2);
                    trimKnot.push_back(
                        static_cast<float>(2.0 * klast_1 - klast_2));
                }
            } // for k
        } // for j
    } // for i

    samp.setTrimCurve(numLoops,
        Alembic::Abc::Int32ArraySample(trimNumCurves),
        Alembic::Abc::Int32ArraySample(trimNumPos),
        Alembic::Abc::Int32ArraySample(trimOrder),
        Alembic::Abc::FloatArraySample(trimKnot),
        Alembic::Abc::FloatArraySample(trimMin),
        Alembic::Abc::FloatArraySample(trimMax),
        Alembic::Abc::FloatArraySample(trimU),
        Alembic::Abc::FloatArraySample(trimV),
        Alembic::Abc::FloatArraySample(trimW));

    mSchema.set(samp);
}
