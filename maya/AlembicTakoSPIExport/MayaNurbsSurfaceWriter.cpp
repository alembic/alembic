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

#include <AlembicTakoSPIExport/MayaNurbsSurfaceWriter.h>
#include <AlembicTakoSPIExport/MayaUtility.h>

namespace Alembic {

MayaNurbsSurfaceWriter::MayaNurbsSurfaceWriter(
    float iFrame, MDagPath & iDag, TransformWriterPtr iParent,
    bool iWriteVisibility) :
    mIsSurfaceAnimated(false), mIsTrimCurveAnimated(false), mDagPath(iDag)
{
    MStatus stat;
    MObject surface = iDag.node();

    MFnNurbsSurface nurbs(mDagPath);

    mWriter = iParent->createNurbsSurfaceChild(nurbs.name().asChar());

    mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, *mWriter,
        nurbs, iWriteVisibility));

    // so in the case that only the surface or only one of the trim curves is
    // animated, everything will still be written out at each frame will
    // change if necessary

    if (util::isAnimated(surface))
        mIsSurfaceAnimated = true;
    else
        iFrame = FLT_MAX;

    // write out the initial surface data at the first frame
    // (without writing the props)
    std::vector<float> cv;
    std::vector<double> iKnotsInU;
    std::vector<double> iKnotsInV;
    TakoSPI::NurbsSurfaceMiscInfo miscInfo;
    std::vector<TakoSPI::NurbsCurveGrp> tc;
    fillTopology(cv, iKnotsInU, iKnotsInV, miscInfo, tc);
    mWriter->start(iFrame);
    mWriter->write(cv, iKnotsInU, iKnotsInV, miscInfo);
    if (tc.size() > 0)
        mWriter->writeTrimCurve(tc);
    mWriter->end();
}

void MayaNurbsSurfaceWriter::write(float iFrame)
{
    // how to tell if the topology is changed for nurbs surface???
    // for now just write everything out again
    std::vector<float> cv;
    std::vector<double> iKnotsInU;
    std::vector<double> iKnotsInV;
    TakoSPI::NurbsSurfaceMiscInfo miscInfo;
    std::vector<TakoSPI::NurbsCurveGrp> tc;

    fillTopology(cv, iKnotsInU, iKnotsInV, miscInfo, tc);

    mWriter->start(iFrame);
    mWriter->write(cv, iKnotsInU, iKnotsInV, miscInfo);
    if (tc.size() > 0)
        mWriter->writeTrimCurve(tc);
    mWriter->end();

    mAttrs->write(iFrame);
}


bool MayaNurbsSurfaceWriter::isAnimated() const
{
    return  mIsSurfaceAnimated || mIsTrimCurveAnimated ||
            (mAttrs != NULL && mAttrs->isAnimated());
}


unsigned int MayaNurbsSurfaceWriter::getNumCVs()
{
    MFnNurbsSurface nurbs(mDagPath);
    return nurbs.numCVsInU() * nurbs.numCVsInV();
}


// the arrays being passed in are assumed to be empty
void MayaNurbsSurfaceWriter::fillTopology(std::vector<float> & ioCV,
    std::vector<double> & ioKnotsInU, std::vector<double> & ioKnotsInV,
    TakoSPI::NurbsSurfaceMiscInfo & ioMiscInfo,
    std::vector<TakoSPI::NurbsCurveGrp> & ioTC)
{
    MFnNurbsSurface nurbs(mDagPath);
    ioMiscInfo.spanU = nurbs.numSpansInU();
    ioMiscInfo.spanV = nurbs.numSpansInV();
    ioMiscInfo.degreeU = nurbs.degreeU();
    ioMiscInfo.degreeV = nurbs.degreeV();
    ioMiscInfo.formU = nurbs.formInU()-1;
    ioMiscInfo.formV = nurbs.formInV()-1;
    double startU, endU, startV, endV;
    nurbs.getKnotDomain(startU, endU, startV, endV);
    ioMiscInfo.minU = startU;
    ioMiscInfo.minV = startV;
    ioMiscInfo.maxU = endU;
    ioMiscInfo.maxV = endV;

    MDoubleArray knotsInU;
    nurbs.getKnotsInU(knotsInU);
    unsigned int numKnotsInU = knotsInU.length();

    // guard against a degenerative case
    if (numKnotsInU > 1)
    {
        // pad the start and end with a knot on each side, since thats what
        // most apps, like Houdini and Renderman want these two extra knots
        ioKnotsInU.reserve(numKnotsInU+2);

        // push_back a dummy value, we will set it below
        ioKnotsInU.push_back(0);

        for (size_t i = 0; i < numKnotsInU; i++)
            ioKnotsInU.push_back(knotsInU[i]);

        // periodic case
        if  (ioMiscInfo.formU == 2)
        {
            // is 1 always the correct offset?
            ioKnotsInU[0] = ioKnotsInU[1] - 1;
            ioKnotsInU.push_back(ioKnotsInU[numKnotsInU] + 1);
        }
        else
        {
            ioKnotsInU[0] = ioKnotsInU[1];
            ioKnotsInU.push_back(ioKnotsInU[numKnotsInU]);
        }
    }

    MDoubleArray knotsInV;
    nurbs.getKnotsInV(knotsInV);
    unsigned int numKnotsInV = knotsInV.length();

    // do the same thing for V
    if (numKnotsInV > 1)
    {
        ioKnotsInV.reserve(numKnotsInV+2);

        // push back a dummy value that we will end up setting below
        ioKnotsInV.push_back(0);
        for (size_t i = 0; i < numKnotsInV; i++)
        {
            ioKnotsInV.push_back(knotsInV[i]);
        }

        // periodic case
        if  (ioMiscInfo.formV == 2)
        {
            // is 1 always the correct offset?
            ioKnotsInV[0] = ioKnotsInV[1] - 1;
            ioKnotsInV.push_back(ioKnotsInV[numKnotsInV] + 1);
        }
        else
        {
            ioKnotsInV[0] = ioKnotsInV[1];
            ioKnotsInV.push_back(ioKnotsInV[numKnotsInV]);
        }
    }

    // for closed and periodic we are saving duplicate information
    MPointArray cvArray;

    nurbs.getCVs(cvArray);
    unsigned int numCVs = cvArray.length();
    int numCVsInU = nurbs.numCVsInU();
    int numCVsInV = nurbs.numCVsInV();

    ioCV.reserve(numCVs * 4);

    // Maya stores the data where v varies the fastest (v,u order)
    // so we need to pack the data differently u,v order
    // (v reversed to make clockwise???)
    for (int v = numCVsInV - 1; v > -1; v--)
    {
        for (int u = 0; u < numCVsInU; u++)
        {
            int index = u * numCVsInV + v;
            ioCV.push_back(cvArray[index].x);
            ioCV.push_back(cvArray[index].y);
            ioCV.push_back(cvArray[index].z);
            ioCV.push_back(cvArray[index].w);
        }
    }

    if (!nurbs.isTrimmedSurface())
        return;

    // each region is a curvegroup, it can have multiple trim curve segments
    unsigned int numRegions = nurbs.numRegions();
    ioTC.reserve(numRegions);
    for (size_t i = 0; i < numRegions; i++)
    {
        MTrimBoundaryArray result;

        // if the 3rd argument is set to be true, return the 2D curve
        nurbs.getTrimBoundaries(result, i, true);
        TakoSPI::NurbsCurveGrp curveGrp;
        curveGrp.numCurve = 0;
        size_t numTrimCurve = result.length();
        for (size_t j = 0; j < numTrimCurve; j++)
        {
            MObjectArray boundary = result[j];
            size_t length = boundary.length();
            curveGrp.numCurve += length;
            for (size_t k = 0; k < length; k++)
            {
                MObject mNurbsCurveObj = boundary[k];
                if (mNurbsCurveObj.hasFn(MFn::kNurbsCurve))
                {
                    MFnNurbsCurve mFnCurve(mNurbsCurveObj);

                    TakoSPI::NurbsCurveMiscInfo miscInfo;
                    miscInfo.numCV = mFnCurve.numCVs();
                    miscInfo.span = mFnCurve.numSpans();
                    miscInfo.degree = mFnCurve.degree();
                    miscInfo.form = mFnCurve.form()-1;
                    double start, end;
                    mFnCurve.getKnotDomain(start, end);
                    miscInfo.min = start;
                    miscInfo.max = end;
                    curveGrp.miscInfo.push_back(miscInfo);

                    size_t numCVs = mFnCurve.numCVs();
                    if (miscInfo.form == 1)  // close
                        numCVs -= 1;
                    else if (miscInfo.form == 2)  // periodic
                        numCVs -= miscInfo.degree;

                    MPointArray cvArray;
                    mFnCurve.getCVs(cvArray);
                    curveGrp.cv.reserve(numCVs*4);
                    double offsetV = startV+endV;
                    for (size_t l = 0; l < numCVs; l++)
                    {
                        curveGrp.cv.push_back(cvArray[l].x);

                        // v' = maxV + minV - v
                        // this is renderman specific flipping
                        curveGrp.cv.push_back(offsetV-cvArray[l].y);

                        // curveGrp.cv.push_back(cvArray[l].y);
                        curveGrp.cv.push_back(cvArray[l].z);
                        curveGrp.cv.push_back(cvArray[l].w);
                    }

                    MDoubleArray knot;
                    mFnCurve.getKnots(knot);
                    unsigned int numKnots = knot.length();
                    curveGrp.knot.reserve(numKnots+2);
                    curveGrp.knot.push_back(0.0);
                    for (size_t l = 0; l < numKnots; l++)
                        curveGrp.knot.push_back(knot[l]);

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
                }
            }
        }
        ioTC.push_back(curveGrp);
    }
}

} // End namespace Alembic

