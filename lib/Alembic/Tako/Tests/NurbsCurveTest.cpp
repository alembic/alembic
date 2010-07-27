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

#include <Alembic/Tako/Tests/NurbsCurveTest.h>

#include <Alembic/Tako/NurbsCurveReader.h>
#include <Alembic/Tako/NurbsCurveWriter.h>
#include <Alembic/Tako/TransformOperation.h>
#include <Alembic/Tako/TransformReader.h>
#include <Alembic/Tako/TransformWriter.h>
#include <Alembic/Tako/NurbsDef.h>

namespace {
// CPPUNIT_TEST_SUITE_REGISTRATION(NurbsCurveTest);
const unsigned int NUMCURVE = 3;
}

using namespace Alembic::Tako;

void NurbsCurveTest::testNurbsCurveSimple(void)
{
    NurbsCurveGrp mCurveGrp;
    mCurveGrp.numCurve = NUMCURVE;
    NurbsCurveMiscInfo miscInfo={4, 1, 3, 0, 0.0, 1.0};

    for (unsigned int i = 0; i<NUMCURVE; i++)
    {
        mCurveGrp.miscInfo.push_back(miscInfo);

        // each curve has four points
        mCurveGrp.cv.push_back(-0.5); mCurveGrp.cv.push_back(0.0); mCurveGrp.cv.push_back(0.5); mCurveGrp.cv.push_back(1.0);
        mCurveGrp.cv.push_back(-0.5); mCurveGrp.cv.push_back(0.0); mCurveGrp.cv.push_back(0.167); mCurveGrp.cv.push_back(1.0);
        mCurveGrp.cv.push_back(-0.5); mCurveGrp.cv.push_back(0.0); mCurveGrp.cv.push_back(-0.167); mCurveGrp.cv.push_back(1.0);
        mCurveGrp.cv.push_back(-0.5); mCurveGrp.cv.push_back(0.0); mCurveGrp.cv.push_back(-0.5); mCurveGrp.cv.push_back(1.0);

        // #knots = #cv+degree+1
        mCurveGrp.knot.push_back(-1.0);
        mCurveGrp.knot.push_back(0.0);
        mCurveGrp.knot.push_back(0.0);
        mCurveGrp.knot.push_back(0.0);
        mCurveGrp.knot.push_back(1.0);
        mCurveGrp.knot.push_back(1.0);
        mCurveGrp.knot.push_back(1.0);
        mCurveGrp.knot.push_back(1.0);

        mCurveGrp.constantWidth = 0.1;
    }

    hid_t fid = -1;
    // write
    {
        TransformWriter root("/tmp/testNurbsCurve.hdf");
        fid = H5Iget_file_id(root.getHid());

        TransformWriter tw("test", root);
        Value tx, ty, tz;
        tx = 1.0;
        ty = 2.0;
        tz = 3.0;
        TransformOperationVariant t(Translate(Translate::cTranslate, tx, ty, tz));
        tw.push(t);
        tw.writeTransformStack(0.0, true);

        NurbsCurveWriter pw("curve", tw);
        pw.write(FLT_MAX, mCurveGrp);
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    // read
    {
        TransformReader root("/tmp/testNurbsCurve.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(root.getChild(0));
        TESTING_ASSERT(str == testPtr->getName());

        str = "curve";
        NurbsCurveReaderPtr ptr = boost::get< NurbsCurveReaderPtr >(testPtr->getChild(0));
        TESTING_ASSERT(str == ptr->getName());

        unsigned int ttype = ptr->read(0.0);
        TESTING_ASSERT(ttype == NurbsCurveReader::GEOMETRY_STATIC);

        unsigned int numCV = ptr->mCurveGrp.getNumCV();
        TESTING_ASSERT(numCV == mCurveGrp.getNumCV());

        for (unsigned int i = 0; i < numCV; i++)
        {
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3] == mCurveGrp.cv[i*3]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+1] == mCurveGrp.cv[i*3+1]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+2] == mCurveGrp.cv[i*3+2]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+3] == mCurveGrp.cv[i*3+3]);
        }

        if (ptr->mCurveGrp.constantWidth > 0)
            TESTING_ASSERT(ptr->mCurveGrp.constantWidth == mCurveGrp.constantWidth);
        else
        {
            // could be zero length
            unsigned int sizeWidthVec = mCurveGrp.width.size();
            for (unsigned int i = 0; i < sizeWidthVec; i++)
                TESTING_ASSERT(ptr->mCurveGrp.width[i] == mCurveGrp.width[i]);
        }

        unsigned int numKnots = ptr->mCurveGrp.knot.size();
        TESTING_ASSERT(numKnots == mCurveGrp.knot.size());
        for (unsigned int i = 0; i < numKnots; i++)
            TESTING_ASSERT(ptr->mCurveGrp.knot[i] == mCurveGrp.knot[i]);

        unsigned int numCurve = mCurveGrp.numCurve;
        TESTING_ASSERT(numCurve == ptr->mCurveGrp.numCurve);
        TESTING_ASSERT(numCurve == ptr->mCurveGrp.miscInfo.size());
        for (unsigned int i=0; i<numCurve; i++)
        {
            TESTING_ASSERT(miscInfo.numCV == ptr->mCurveGrp.miscInfo[i].numCV);
            TESTING_ASSERT(miscInfo.span == ptr->mCurveGrp.miscInfo[i].span);
            TESTING_ASSERT(miscInfo.degree == ptr->mCurveGrp.miscInfo[i].degree);
            TESTING_ASSERT(miscInfo.form == ptr->mCurveGrp.miscInfo[i].form);
            TESTING_ASSERT(miscInfo.min == ptr->mCurveGrp.miscInfo[i].min);
            TESTING_ASSERT(miscInfo.max == ptr->mCurveGrp.miscInfo[i].max);
        }

        ttype = ptr->read(150.0);
        TESTING_ASSERT(ttype == (NurbsCurveReader::GEOMETRY_STATIC));

        std::set<float> frames;
        ptr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 0);
        TESTING_ASSERT(!ptr->hasFrames());
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);
}

void NurbsCurveTest::testNurbsCurveAnimated()
{
    std::vector<float> cv, cv2, cv3;

    NurbsCurveGrp mCurveGrp;
    mCurveGrp.numCurve = NUMCURVE;
    NurbsCurveMiscInfo mInfo={4, 1, 3, 0, 0.0, 1.0};

    for (unsigned int i = 0; i<NUMCURVE; i++)
    {
        mCurveGrp.miscInfo.push_back(mInfo);

        mCurveGrp.cv.push_back(-0.5); mCurveGrp.cv.push_back(0.0); mCurveGrp.cv.push_back(0.5); mCurveGrp.cv.push_back(1.0);
        mCurveGrp.cv.push_back(-0.5); mCurveGrp.cv.push_back(0.0); mCurveGrp.cv.push_back(0.167); mCurveGrp.cv.push_back(1.0);
        mCurveGrp.cv.push_back(-0.5); mCurveGrp.cv.push_back(0.0); mCurveGrp.cv.push_back(-0.167); mCurveGrp.cv.push_back(1.0);
        mCurveGrp.cv.push_back(-0.5); mCurveGrp.cv.push_back(0.0); mCurveGrp.cv.push_back(-0.5); mCurveGrp.cv.push_back(1.0);

        mCurveGrp.knot.push_back(-1.0);
        mCurveGrp.knot.push_back(0.0);
        mCurveGrp.knot.push_back(0.0);
        mCurveGrp.knot.push_back(0.0);
        mCurveGrp.knot.push_back(1.0);
        mCurveGrp.knot.push_back(1.0);
        mCurveGrp.knot.push_back(1.0);
        mCurveGrp.knot.push_back(1.0);

        mCurveGrp.constantWidth = 0.1;
    }

    hid_t fid = -1;

    // write
    {
        TransformWriter root("/tmp/testNurbsCurveAnim.hdf");
        fid = H5Iget_file_id(root.getHid());

        TransformWriter tw("test", root);
        Value tx, ty, tz;
        tx = 1.0;
        ty = 2.0;
        tz = 3.0;
        TransformOperationVariant t(Translate(Translate::cTranslate, tx, ty, tz));
        tw.push(t);
        tw.writeTransformStack(0.0, true);

        NurbsCurveWriter pw("curve", tw);
        pw.write(1.0, mCurveGrp);

        unsigned int size = mCurveGrp.cv.size();
        cv.resize(size);
        cv = mCurveGrp.cv;
        cv2.resize(size);
        std::srand(0);
        for (unsigned int i = 0; i<size; i++)
            cv2[i] = cv[i] + (std::rand()%10)/10.0;
        mCurveGrp.cv = cv2;
        pw.write(2.0, mCurveGrp);

        cv3.resize(size);
        std::srand(0);
        for (unsigned int i = 0; i<size; i++)
            cv3[i] = cv[i] + (std::rand()%10)/10.0;
        pw.write(3.0, cv3);

        // write the frame range
        std::vector<float> frames;
        frames.push_back(1.0);
        frames.push_back(2.0);
        frames.push_back(3.0);
        PropertyPair p;
        p.first = frames;
        root.setNonSampledProperty("frameRange", p);
        root.writeProperties(0.0);
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    {
        TransformReader root("/tmp/testNurbsCurveAnim.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr =
            boost::get< TransformReaderPtr >(root.getChild(0));
        TESTING_ASSERT(str == testPtr->getName());

        str = "curve";
        NurbsCurveReaderPtr ptr =
            boost::get< NurbsCurveReaderPtr >(testPtr->getChild(0));
        TESTING_ASSERT(str == ptr->getName());

        unsigned int ttype = ptr->read(1.0);
        TESTING_ASSERT(ttype == NurbsCurveReader::GEOMETRY_ANIMATED);

        unsigned int numCV = ptr->mCurveGrp.getNumCV();
        TESTING_ASSERT(numCV == mCurveGrp.getNumCV());
        for (unsigned int i = 0; i < numCV; i++)
        {
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3] == cv[i*3]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+1] == cv[i*3+1]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+2] == cv[i*3+2]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+3] == cv[i*3+3]);
        }

        if (ptr->mCurveGrp.constantWidth > 0)
            TESTING_ASSERT(ptr->mCurveGrp.constantWidth == mCurveGrp.constantWidth);
        else
        {
            // could be zero length
            unsigned int sizeWidthVec = mCurveGrp.width.size();
            for (unsigned int i = 0; i < sizeWidthVec; i++)
                TESTING_ASSERT(ptr->mCurveGrp.width[i] == mCurveGrp.width[i]);
        }

        unsigned int numKnots = ptr->mCurveGrp.knot.size();
        TESTING_ASSERT(numKnots == mCurveGrp.knot.size());
        for (unsigned int i = 0; i < numKnots; i++)
            TESTING_ASSERT(ptr->mCurveGrp.knot[i] == mCurveGrp.knot[i]);

        unsigned int numCurve = mCurveGrp.numCurve;
        TESTING_ASSERT(numCurve == ptr->mCurveGrp.numCurve);

        ////////////////////////////////////////////////////////////////////////////////////////////

        ttype = ptr->read(2.0);
        TESTING_ASSERT(ttype == NurbsCurveReader::GEOMETRY_ANIMATED);

        numCV = ptr->mCurveGrp.getNumCV();
        TESTING_ASSERT(numCV == mCurveGrp.getNumCV());
        for (unsigned int i = 0; i < numCV; i++)
        {
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3] == cv2[i*3]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+1] == cv2[i*3+1]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+2] == cv2[i*3+2]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+3] == cv2[i*3+3]);
        }

        if (ptr->mCurveGrp.constantWidth > 0)
            TESTING_ASSERT(ptr->mCurveGrp.constantWidth == mCurveGrp.constantWidth);
        else
        {
            // could be zero length
            unsigned int sizeWidthVec = mCurveGrp.width.size();
            for (unsigned int i = 0; i < sizeWidthVec; i++)
                TESTING_ASSERT(ptr->mCurveGrp.width[i] == mCurveGrp.width[i]);
        }

        numKnots = ptr->mCurveGrp.knot.size();
        TESTING_ASSERT(numKnots == mCurveGrp.knot.size());
        for (unsigned int i = 0; i < numKnots; i++)
            TESTING_ASSERT(ptr->mCurveGrp.knot[i] == mCurveGrp.knot[i]);

        numCurve = mCurveGrp.numCurve;
        TESTING_ASSERT(numCurve == ptr->mCurveGrp.numCurve);

        //////////////////////////////////////////////////////////////////////////////////////////////////

        ttype = ptr->read(3.0);
        TESTING_ASSERT(ttype == NurbsCurveReader::GEOMETRY_ANIMATED);

        numCV = ptr->mCurveGrp.getNumCV();
        TESTING_ASSERT(numCV == mCurveGrp.getNumCV());
        for (unsigned int i = 0; i < numCV; i++)
        {
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3] == cv3[i*3]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+1] == cv3[i*3+1]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+2] == cv3[i*3+2]);
            TESTING_ASSERT(ptr->mCurveGrp.cv[i*3+3] == cv3[i*3+3]);
        }

        if (ptr->mCurveGrp.constantWidth > 0)
            TESTING_ASSERT(ptr->mCurveGrp.constantWidth == mCurveGrp.constantWidth);
        else
        {
            // could be zero length
            unsigned int sizeWidthVec = mCurveGrp.width.size();
            for (unsigned int i = 0; i < sizeWidthVec; i++)
                TESTING_ASSERT(ptr->mCurveGrp.width[i] == mCurveGrp.width[i]);
        }

        numKnots = ptr->mCurveGrp.knot.size();
        TESTING_ASSERT(numKnots == mCurveGrp.knot.size());
        for (unsigned int i = 0; i < numKnots; i++)
            TESTING_ASSERT(ptr->mCurveGrp.knot[i] == mCurveGrp.knot[i]);

        numCurve = mCurveGrp.numCurve;
        TESTING_ASSERT(numCurve == ptr->mCurveGrp.numCurve);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////

        ttype = ptr->read(4.0);
        TESTING_ASSERT(ttype == NurbsCurveReader::READ_ERROR);

        std::set<float> frames;
        ptr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 3);
        TESTING_ASSERT(ptr->hasFrames());

        // has 1, 2 and 3 but doesnt have 4
        TESTING_ASSERT(frames.find(1.0) != frames.end());
        TESTING_ASSERT(frames.find(2.0) != frames.end());
        TESTING_ASSERT(frames.find(3.0) != frames.end());
        TESTING_ASSERT(frames.find(4.0) == frames.end());
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);
}
