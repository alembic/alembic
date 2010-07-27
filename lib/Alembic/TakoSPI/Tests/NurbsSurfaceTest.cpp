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

#include <Alembic/TakoSPI/Tests/NurbsSurfaceTest.h>
#include <Alembic/TakoSPI/Tests/Utility.h>

#include <Alembic/TakoSPI/NurbsDef.h>
#include <Alembic/TakoSPI/NurbsSurfaceReader.h>
#include <Alembic/TakoSPI/NurbsSurfaceWriter.h>
#include <Alembic/TakoSPI/TransformOperation.h>
#include <Alembic/TakoSPI/TransformReader.h>
#include <Alembic/TakoSPI/TransformWriter.h>

using namespace Alembic::Tako;

namespace
{

// CPPUNIT_TEST_SUITE_REGISTRATION(NurbsSurfaceTest);

    const unsigned int NUMTRIMCURVE = 9;
    const unsigned int numCV = 5*4*4;
    const unsigned int numKnotU = 9;
    const unsigned int numKnotV = 8;
    const unsigned int numSpanU = 2;
    const unsigned int numSpanV = 1;

    /*
    The Maya commands to build this trimmed surface:
    MayaCmds.nurbsPlane(d=3, u=2, v=1, ch=True, name='nPlane')
    MayaCmds.curveOnSurface( 'nPlaneShape', degree=3, per=True,
        uv=((0, 0), (0.2, 0.6), (0.4, 0.7), (0.9, 0.9), (0.0, 0.0), (0.2, 0.6), (0.4, 0.7)),
        k=(-2, -1, 0, 1, 2, 3, 4, 5, 6) )
    */
    void buildTrimmedSurface(
        std::vector<float> & cv,
        NurbsSurfaceMiscInfo & miscInfo,
        std::vector<double> & knotsInU,
        std::vector<double> & knotsInV,
        NurbsCurveGrp & curveGrp)
    {
        float cvArray[] = {
                -0.5, 3.06162e-17, -0.5, 1,
                -0.333333, 3.06162e-17, -0.5, 1,
                -2.77556e-17, 3.06162e-17, -0.5, 1,
                0.333333, 3.06162e-17, -0.5, 1,
                0.5, 3.06162e-17, -0.5, 1,
                -0.5, 1.02054e-17, -0.166667, 1,
                -0.333333, 1.02054e-17, -0.166667, 1,
                -2.77556e-17, 1.02054e-17, -0.166667, 1,
                0.333333, 1.02054e-17, -0.166667, 1,
                0.5, 1.02054e-17, -0.166667, 1,
                -0.5, -1.02054e-17, 0.166667, 1,
                -0.333333, -1.02054e-17, 0.166667, 1,
                -2.77556e-17, -1.02054e-17, 0.166667, 1,
                0.333333, -1.02054e-17, 0.166667, 1,
                0.5, -1.02054e-17, 0.166667, 1,
                -0.5, -3.06162e-17, 0.5, 1,
                -0.333333, -3.06162e-17, 0.5, 1,
                -2.77556e-17, -3.06162e-17, 0.5, 1,
                0.333333, -3.06162e-17, 0.5, 1,
                0.5, -3.06162e-17, 0.5, 1};
        unsigned int size = sizeof(cvArray)/sizeof(float);
        arrayToVec<float>(size, cvArray, cv);

        miscInfo.spanU = 2;     miscInfo.spanV = 1;
        miscInfo.degreeU = 3;   miscInfo.degreeV = 3;
        miscInfo.formU = 0;     miscInfo.formV = 0;
        miscInfo.minU = 0.0;    miscInfo.maxU = 1.0;
        miscInfo.minV = 0.0;    miscInfo.maxV = 1.0;

        double kuArray[] = {0, 0, 0, 0, 0.5, 1, 1, 1, 1};
        size = sizeof(kuArray)/sizeof(double);
        arrayToVec<double>(size, kuArray, knotsInU);

        double kvArray[] = {0, 0, 0, 0, 1, 1, 1, 1};
        size = sizeof(kvArray)/sizeof(double);
        arrayToVec<double>(size, kvArray, knotsInV);

        // trim curve group
        curveGrp.numCurve = NUMTRIMCURVE;
        float tcCvArray[] = {
            0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0,
            0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0.2, 0.483333, 0, 1, 0.266667,
            0.366667, 0, 1, 0.372682, 0.313659, 0, 1, 0.605139, 0.218504, 0, 1,
            0.650577, 0.218504, 0, 1, 0.650577, 0.218504, 0, 1, 0.669958,
            0.218504, 0, 1, 0.681909, 0.224929, 0, 1, 0.681909, 0.240971, 0, 1,
            0.681909, 0.240971, 0, 1, 0.681909, 0.25104, 0, 1, 0.662303,
            0.308746, 0, 1, 0.45, 0.55, 0, 1, 0.219221, 0.743854, 0, 1,
            0.160322, 0.757872, 0, 1, 0.15178, 0.757872, 0, 1, 0.15178,
            0.757872, 0, 1, 0.128846, 0.757872, 0, 1, 0.119597, 0.739962, 0, 1,
            0.119597, 0.712207, 0, 1, 0.119597, 0.712207, 0, 1, 0.119597,
            0.654342, 0, 1, 0.159798, 0.553686, 0, 1, 0.2, 0.483333, 0, 1};
        size = sizeof(tcCvArray)/sizeof(float);
        arrayToVec<float>(size, tcCvArray, curveGrp.cv);

        float tcKnotArray[] = {
            0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1,
            1.59023, 1.59023, 1.59023, 1, 0, 1.59023, 1.59023, 1.59023,
            1.84198, 1.84198, 1.84198, 1, 0, 1.84198, 1.84198, 1.84198, 2,
            2.5, 3, 3.10774, 3.10774, 3.10774, 0, 0, 3.10774, 3.10774,
            3.10774, 3.39698, 3.39698, 3.39698, 1, 0, 3.39698, 3.39698,
            3.39698, 4, 4, 4, 1};
        size = sizeof(tcKnotArray)/sizeof(float);
        arrayToVec<float>(size, tcKnotArray, curveGrp.knot);

        NurbsCurveMiscInfo mInfo = {2, 1, 1, 0, 0, 1};
        curveGrp.miscInfo.push_back(mInfo);
        curveGrp.miscInfo.push_back(mInfo);
        curveGrp.miscInfo.push_back(mInfo);
        curveGrp.miscInfo.push_back(mInfo);
        NurbsCurveMiscInfo mInfo5 = {5, 2, 3, 0, 0, 1.59023};
        curveGrp.miscInfo.push_back(mInfo5);
        NurbsCurveMiscInfo mInfo6 = {4, 1, 3, 0, 1.59023, 1.84198};
        curveGrp.miscInfo.push_back(mInfo6);
        NurbsCurveMiscInfo mInfo7 = {7, 4, 3, 0, 1.84198, 3.10774};
        curveGrp.miscInfo.push_back(mInfo7);
        NurbsCurveMiscInfo mInfo8 = {4, 1, 3, 0, 3.10774, 3.39698};
        curveGrp.miscInfo.push_back(mInfo8);
        NurbsCurveMiscInfo mInfo9 = {4, 1, 3, 0, 3.39698, 4};
        curveGrp.miscInfo.push_back(mInfo9);
        for (unsigned int i = 0; i<NUMTRIMCURVE; i++)
            curveGrp.constantWidth = 0.1;
    }
}


// a nurbsPlane
void NurbsSurfaceTest::testNurbsSurfaceSimple(void)
{
    std::vector<float> cv;
    NurbsSurfaceMiscInfo miscInfo;
    std::vector<double> knotsInU, knotsInV;
    NurbsCurveGrp curveGrp;
    buildTrimmedSurface(cv, miscInfo, knotsInU, knotsInV, curveGrp);
    std::vector<NurbsCurveGrp> curveGrpVec;
    curveGrpVec.push_back(curveGrp);

    hid_t fid = -1;

    // write
    {
        TransformWriter root("/tmp/testNurbsSurface.hdf");
        fid = H5Iget_file_id(root.getHid());

        TransformWriter tw("test", root);
        Value tx, ty, tz;
        tx = 1.0;
        ty = 2.0;
        tz = 3.0;
        TransformOperationVariant t(Translate(Translate::cTranslate, tx, ty, tz));
        tw.push(t);
        tw.writeTransformStack(0.0, true);

        NurbsSurfaceWriter pw("plane", tw);
        pw.start(FLT_MAX);
        pw.write(cv, knotsInU, knotsInV, miscInfo);
        pw.writeTrimCurve(curveGrpVec);
        pw.end();
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    // read
    {
        TransformReader root("/tmp/testNurbsSurface.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(
            root.getChild(0));
        TESTING_ASSERT(str == testPtr->getName());

        str = "plane";
        NurbsSurfaceReaderPtr planePtr = boost::get< NurbsSurfaceReaderPtr >(testPtr->getChild(0));
        TESTING_ASSERT(str == planePtr->getName());
        unsigned int ttype = planePtr->read(0.0);
        unsigned int surfacetype = ttype & 0x000F;
        TESTING_ASSERT( surfacetype == NurbsSurfaceReader::GEOMETRY_STATIC);

        TESTING_ASSERT(planePtr->mCV.size() == numCV);
        for (unsigned int i = 0; i < numCV; i++)
            TESTING_ASSERT(planePtr->mCV[i] == cv[i]);

        TESTING_ASSERT(planePtr->mKnotsInU.size() == numKnotU);
        TESTING_ASSERT(planePtr->mMiscInfo.degreeU == 3);
        TESTING_ASSERT(planePtr->mMiscInfo.spanU == numSpanU);
        TESTING_ASSERT(planePtr->mMiscInfo.formU == 0);
        TESTING_ASSERT(planePtr->mMiscInfo.minU == 0.0);
        TESTING_ASSERT(planePtr->mMiscInfo.maxU == 1.0);

        TESTING_ASSERT(planePtr->mKnotsInV.size() == numKnotV);
        TESTING_ASSERT(planePtr->mMiscInfo.degreeV == 3);
        TESTING_ASSERT(planePtr->mMiscInfo.spanV == numSpanV);
        TESTING_ASSERT(planePtr->mMiscInfo.formV == 0);
        TESTING_ASSERT(planePtr->mMiscInfo.minV == 0.0);
        TESTING_ASSERT(planePtr->mMiscInfo.maxV == 1.0);

        // testing trim curve reading
        unsigned int tctype = ttype & 0x00F0;
        TESTING_ASSERT( tctype == NurbsSurfaceReader::TRIM_CURVE_STATIC);
        TESTING_ASSERT(planePtr->mTrimCurve.size() == curveGrpVec.size());

        unsigned int grpSize = planePtr->mTrimCurve.size();
        for (unsigned int i=0; i<grpSize; i++)
        {
            NurbsCurveGrp curveGrp = planePtr->mTrimCurve[i];
            TESTING_ASSERT(curveGrp.numCurve == curveGrp.numCurve);
            unsigned int numCurve = curveGrp.numCurve;
            for (unsigned int j=0; j<numCurve; j++)
            {
                TESTING_ASSERT(curveGrp.miscInfo[j].numCV == curveGrp.miscInfo[j].numCV);
                TESTING_ASSERT(curveGrp.miscInfo[j].span == curveGrp.miscInfo[j].span);
                TESTING_ASSERT(curveGrp.miscInfo[j].degree == curveGrp.miscInfo[j].degree);
                TESTING_ASSERT(curveGrp.miscInfo[j].form == curveGrp.miscInfo[j].form);
                TESTING_ASSERT(curveGrp.miscInfo[j].min == curveGrp.miscInfo[j].min);
                TESTING_ASSERT(curveGrp.miscInfo[j].max == curveGrp.miscInfo[j].max);
            }

            TESTING_ASSERT(curveGrp.getNumCV() == curveGrp.getNumCV());
            unsigned int numCV = curveGrp.cv.size();
            for (unsigned int j=0; j<numCV; j++)
                TESTING_ASSERT(curveGrp.cv[j] == curveGrp.cv[j]);
        }

        ttype = planePtr->read(150.0);
        surfacetype = ttype & 0x000F;
        tctype = ttype & 0x00F0;
        TESTING_ASSERT(surfacetype == (NurbsSurfaceReader::GEOMETRY_STATIC));
        TESTING_ASSERT(tctype == (NurbsSurfaceReader::TRIM_CURVE_STATIC));

        std::set<float> frames;
        planePtr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 0);
        TESTING_ASSERT(!planePtr->hasFrames());
    }

    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);
}

void NurbsSurfaceTest::testNurbsSurfaceAnimated()
{
    std::vector<float> cv;
    NurbsSurfaceMiscInfo miscInfo;
    std::vector<double> knotsInU, knotsInV;
    NurbsCurveGrp curveGrp;
    buildTrimmedSurface(cv, miscInfo, knotsInU, knotsInV, curveGrp);
    std::vector<NurbsCurveGrp> curveGrpVec;
    curveGrpVec.push_back(curveGrp);

    std::vector<float> cv2, cv3, cv4;
    hid_t fid = -1;

    // write
    {
        TransformWriter root("/tmp/testNurbsSurfaceAnim.hdf");
        fid = H5Iget_file_id(root.getHid());

        TransformWriter tw("test", root);
        tw.writeTransformStack(0.0, true);

        NurbsSurfaceWriter pw("plane", tw);
        pw.start(1.0);
        pw.write(cv, knotsInU, knotsInV, miscInfo);
        pw.writeTrimCurve(curveGrpVec);
        pw.end();

        cv2.resize(numCV);
        std::srand(0);
        for (unsigned int i = 0; i<numCV; i++)
            cv2[i] = cv[i] + (std::rand()%10)/10.0;

        pw.start(2.0);
        pw.write(cv2, knotsInU, knotsInV, miscInfo);
        pw.writeTrimCurve(curveGrpVec);
        pw.end();

        cv3.resize(numCV);
        std::srand(0);
        for (unsigned int i = 0; i<numCV; i++)
            cv3[i] = cv[i] - (std::rand()%10)/10.0;

        pw.start(3.0);
        pw.write(cv3);
        pw.writeTrimCurve(curveGrpVec);
        pw.end();

        cv4.resize(numCV);
        std::srand(0);
        for (unsigned int i = 0; i<numCV; i++)
            cv4[i] = cv[i] - (std::rand()%10)/10.0;

        pw.start(4.0);
        pw.write(cv4, knotsInU, knotsInV, miscInfo);
        pw.writeTrimCurveCV(curveGrpVec);
        pw.end();

        // write the frame range
        std::vector<float> frames;
        frames.push_back(1.0);
        frames.push_back(2.0);
        frames.push_back(3.0);
        frames.push_back(4.0);
        PropertyPair p;
        p.first = frames;
        root.setNonSampledProperty("frameRange", p);
        root.writeProperties(0.0);
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    {
        TransformReader root("/tmp/testNurbsSurfaceAnim.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(
            root.getChild(0));
        TESTING_ASSERT(str == testPtr->getName());

        str = "plane";
        NurbsSurfaceReaderPtr planePtr = boost::get< NurbsSurfaceReaderPtr >(
            testPtr->getChild(0));
        TESTING_ASSERT(str == planePtr->getName());

        unsigned int ttype = planePtr->read(1.0);
        TESTING_ASSERT( (ttype & 0x000F) == NurbsSurfaceReader::GEOMETRY_HOMOGENOUS);
        TESTING_ASSERT((ttype & 0x00F0) == NurbsSurfaceReader::TRIM_CURVE_HOMOGENOUS);
        TESTING_ASSERT(planePtr->mCV.size() == numCV);
        for (unsigned int i = 0; i < numCV; i++)
            TESTING_ASSERT(planePtr->mCV[i] == cv[i]);

        TESTING_ASSERT(planePtr->mKnotsInU.size() == numKnotU);
        TESTING_ASSERT(planePtr->mKnotsInV.size() == numKnotV);
        TESTING_ASSERT(planePtr->mMiscInfo.degreeU == 3);
        TESTING_ASSERT(planePtr->mMiscInfo.degreeV == 3);
        TESTING_ASSERT(planePtr->mMiscInfo.spanU == numSpanU);
        TESTING_ASSERT(planePtr->mMiscInfo.spanV == numSpanV);
        TESTING_ASSERT(planePtr->mMiscInfo.formU == 0);
        TESTING_ASSERT(planePtr->mMiscInfo.formV == 0);
        TESTING_ASSERT(planePtr->mMiscInfo.minU == 0.0);
        TESTING_ASSERT(planePtr->mMiscInfo.maxU == 1.0);
        TESTING_ASSERT(planePtr->mMiscInfo.minV == 0.0);
        TESTING_ASSERT(planePtr->mMiscInfo.maxV == 1.0);

        ttype = planePtr->read(2.0);
        TESTING_ASSERT((ttype & 0x000F) == NurbsSurfaceReader::GEOMETRY_HETEROGENOUS);
        TESTING_ASSERT((ttype & 0x00F0) == NurbsSurfaceReader::TRIM_CURVE_HETEROGENOUS);
        TESTING_ASSERT(planePtr->mCV.size() == numCV);
        for (unsigned int i = 0; i < numCV; i++)
            TESTING_ASSERT(planePtr->mCV[i] == cv2[i]);

        ttype = planePtr->read(3.0);
        TESTING_ASSERT((ttype & 0x000F) == NurbsSurfaceReader::GEOMETRY_HOMOGENOUS);
        TESTING_ASSERT((ttype & 0x00F0) == NurbsSurfaceReader::TRIM_CURVE_HETEROGENOUS);
        TESTING_ASSERT(planePtr->mCV.size() == numCV);
        for (unsigned int i = 0; i < numCV; i++)
            TESTING_ASSERT(planePtr->mCV[i] == cv3[i]);

        ttype = planePtr->read(4.0);
        TESTING_ASSERT((ttype & 0x000F) == NurbsSurfaceReader::GEOMETRY_HETEROGENOUS);
        TESTING_ASSERT((ttype & 0x00F0) == NurbsSurfaceReader::TRIM_CURVE_HOMOGENOUS);
        TESTING_ASSERT(planePtr->mCV.size() == numCV);
        for (unsigned int i = 0; i < numCV; i++)
            TESTING_ASSERT(planePtr->mCV[i] == cv4[i]);

        ttype = planePtr->read(5.0);
        TESTING_ASSERT(ttype == NurbsSurfaceReader::READ_ERROR);

        std::set<float> frames;
        planePtr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 4);
        TESTING_ASSERT(planePtr->hasFrames());

        TESTING_ASSERT(frames.find(1.0) != frames.end());
        TESTING_ASSERT(frames.find(2.0) != frames.end());
        TESTING_ASSERT(frames.find(3.0) != frames.end());
        TESTING_ASSERT(frames.find(4.0) != frames.end());
        TESTING_ASSERT(frames.find(5.0) == frames.end());
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);
}
