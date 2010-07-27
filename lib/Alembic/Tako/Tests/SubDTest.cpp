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

#include <Alembic/Tako/Tests/SubDTest.h>
#include <Alembic/Tako/SubDReader.h>
#include <Alembic/Tako/SubDWriter.h>
#include <Alembic/Tako/TransformOperation.h>
#include <Alembic/Tako/TransformReader.h>
#include <Alembic/Tako/TransformWriter.h>

namespace {
// CPPUNIT_TEST_SUITE_REGISTRATION(SubDTest);
}

using namespace Alembic::Tako;

void SubDTest::testSubDSimple(void)
{
    std::vector<float> p;

    p.push_back(0.0); p.push_back(0.0); p.push_back(0.0);
    p.push_back(0.0); p.push_back(0.0); p.push_back(1.0);
    p.push_back(0.0); p.push_back(1.0); p.push_back(0.0);
    p.push_back(0.0); p.push_back(1.0); p.push_back(1.0);

    p.push_back(1.0); p.push_back(0.0); p.push_back(0.0);
    p.push_back(1.0); p.push_back(0.0); p.push_back(1.0);
    p.push_back(1.0); p.push_back(1.0); p.push_back(0.0);
    p.push_back(1.0); p.push_back(1.0); p.push_back(1.0);

    std::vector<index_t> fp;
    fp.push_back(0); fp.push_back(1); fp.push_back(3); fp.push_back(2);
    fp.push_back(0); fp.push_back(4); fp.push_back(6); fp.push_back(2);
    fp.push_back(0); fp.push_back(4); fp.push_back(5); fp.push_back(1);
    fp.push_back(1); fp.push_back(3); fp.push_back(7); fp.push_back(5);
    fp.push_back(2); fp.push_back(3); fp.push_back(7); fp.push_back(6);
    fp.push_back(4); fp.push_back(6); fp.push_back(7); fp.push_back(5);

    std::vector<index_t> fl;
    fl.push_back(0); fl.push_back(4); fl.push_back(8);
    fl.push_back(12); fl.push_back(16); fl.push_back(20);
    fl.push_back(24);

    // lets define 2 creases
    std::vector<float> crSharp;
    crSharp.push_back(0.25); crSharp.push_back(0.75);

    // first crease, along the YZ face of the cube
    std::vector<index_t> crIndices;
    crIndices.push_back(0); crIndices.push_back(1);
    crIndices.push_back(3); crIndices.push_back(2); crIndices.push_back(0);

    // second crease, along a single edge
    crIndices.push_back(6); crIndices.push_back(7);

    // now define the 2 lengths
    std::vector<index_t> crLengths;
    crLengths.push_back(5); crLengths.push_back(2);

    // now lets mark some corners
    std::vector<float> coSharp;
    coSharp.push_back(0.1); coSharp.push_back(0.2); coSharp.push_back(0.3);

    std::vector<index_t> coIndices;
    coIndices.push_back(4); coIndices.push_back(6); coIndices.push_back(7);

    std::vector<index_t> hpIndices;
    hpIndices.push_back(5);

    hid_t fid = -1;
    {
        TransformWriter root("/tmp/testsubd.hdf");
        fid = H5Iget_file_id(root.getHid());

        TransformWriter tw("test", root);
        Value tx, ty, tz;
        tx = 1.0;
        ty = 2.0;
        tz = 3.0;
        TransformOperationVariant t(Translate(Translate::cTranslate, tx, ty, tz));
        tw.push(t);

        tw.writeTransformStack(0.0, true);

        SubDWriter pw("cube", tw);
        pw.start(FLT_MAX);
        pw.writeGeometry(p, fp, fl);
        pw.writeCreases(crSharp, crIndices, crLengths);
        pw.writeCorners(coSharp, coIndices);
        pw.writeHolePolyIndices(hpIndices);
        pw.writeFaceVaryingInterpolateBoundary(0);
        pw.writeFaceVaryingPropagateCorners(1);
        pw.writeInterpolateBoundary(2);
        pw.end();
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    {

        TransformReader root("/tmp/testsubd.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(
            root.getChild(0));

        TESTING_ASSERT(str == testPtr->getName());
        SubDReaderPtr cubePtr = boost::get< SubDReaderPtr >(
            testPtr->getChild(0));
        TESTING_ASSERT(cubePtr->getFaceVaryingInterpolateBoundary() == 0);
        TESTING_ASSERT(cubePtr->getFaceVaryingPropagateCorners() == 1);
        TESTING_ASSERT(cubePtr->getInterpolateBoundary() == 2);

        str = "cube";
        unsigned int ttype = cubePtr->read(0.0);
        TESTING_ASSERT(ttype == (
            SubDReader::GEOMETRY_STATIC |
            SubDReader::CREASES_STATIC |
            SubDReader::CORNERS_STATIC |
            SubDReader::HOLEPOLY_STATIC));

        TESTING_ASSERT(str == cubePtr->getName());
        TESTING_ASSERT(cubePtr->mPoints.size() == 24);
        TESTING_ASSERT(cubePtr->mFacePoints.size() == 24);
        TESTING_ASSERT(cubePtr->mFaceList.size() == 7);

        TESTING_ASSERT(cubePtr->mCreaseSharpness.size() == 2);
        TESTING_ASSERT(cubePtr->mCreaseSharpness[0] == 0.25);
        TESTING_ASSERT(cubePtr->mCreaseSharpness[1] == 0.75);
        TESTING_ASSERT(cubePtr->mCreaseIndices.size() == 7);
        TESTING_ASSERT(cubePtr->mCreaseLengths.size() == 2);
        TESTING_ASSERT(cubePtr->mCreaseLengths[0] == 5);
        TESTING_ASSERT(cubePtr->mCreaseLengths[1] == 2);

        TESTING_ASSERT(cubePtr->mCornerSharpness.size() == 3);
        TESTING_ASSERT_DOUBLES_EQUAL(cubePtr->mCornerSharpness[0], 0.1,
            0.000001);
        TESTING_ASSERT_DOUBLES_EQUAL(cubePtr->mCornerSharpness[1], 0.2,
            0.000001);
        TESTING_ASSERT_DOUBLES_EQUAL(cubePtr->mCornerSharpness[2], 0.3,
            0.000001);
        TESTING_ASSERT(cubePtr->mCornerIndices.size() == 3);
        TESTING_ASSERT(cubePtr->mCornerIndices[0] == 4);
        TESTING_ASSERT(cubePtr->mCornerIndices[1] == 6);
        TESTING_ASSERT(cubePtr->mCornerIndices[2] == 7);

        TESTING_ASSERT(cubePtr->mHolePolyIndices.size() == 1);
        TESTING_ASSERT(cubePtr->mHolePolyIndices[0] == 5);

        ttype = cubePtr->read(150.0);
        TESTING_ASSERT(ttype == (
            SubDReader::GEOMETRY_STATIC |
            SubDReader::CREASES_STATIC |
            SubDReader::CORNERS_STATIC |
            SubDReader::HOLEPOLY_STATIC));

        std::set<float> frames;
        cubePtr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 0);
        TESTING_ASSERT(!cubePtr->hasFrames());
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

}

void SubDTest::testSubDAnimated()
{
    // create a triangle
    std::vector<float> p;

    p.push_back(0.0); p.push_back(0.0); p.push_back(0.0);
    p.push_back(0.0); p.push_back(0.0); p.push_back(1.0);
    p.push_back(0.0); p.push_back(1.0); p.push_back(0.0);

    std::vector<index_t> fp;
    fp.push_back(0); fp.push_back(1); fp.push_back(2);

    std::vector<index_t> fl;
    fl.push_back(0); fl.push_back(3);

    hid_t fid = -1;
    {
        TransformWriter root("/tmp/testSubDAnim.hdf");
        fid = H5Iget_file_id(root.getHid());

        // identity matrix
        TransformWriter tw("test", root);
        tw.writeTransformStack(0.0, true);

        SubDWriter pw("triangle", tw);
        pw.start(0.0);
        pw.writeGeometry(p, fp, fl);
        pw.end();

        p[0] = -1.0; p[1] = -1.0; p[2] = -1.0;
        p[3] =  2.0; p[4] =  2.0; p[5] =  2.0;
        p[6] =  4.0; p[7] =  4.0; p[8] =  4.0;

        pw.start(1.0);
        pw.writeGeometry(p);
        pw.end();

        p[0] = -1.0; p[1] = -2.0; p[2] = -3.0;
        p[3] =  1.0; p[4] =  2.0; p[5] =  3.0;
        p[6] =  4.0; p[7] =  5.0; p[8] =  6.0;

        pw.start(2.0);
        pw.writeGeometry(p);
        pw.end();

        // write the frame range
        std::vector<float> frames;
        frames.push_back(0.0);
        frames.push_back(1.0);
        frames.push_back(2.0);
        PropertyPair p;
        p.first = frames;
        root.setNonSampledProperty("frameRange", p);
        root.writeProperties(0.0);
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    {
        TransformReader root("/tmp/testSubDAnim.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(
            root.getChild(0));

        TESTING_ASSERT(str == testPtr->getName());
        SubDReaderPtr triPtr = boost::get< SubDReaderPtr >(
            testPtr->getChild(0));

        TESTING_ASSERT(triPtr->getFaceVaryingInterpolateBoundary() == -1);
        TESTING_ASSERT(triPtr->getFaceVaryingPropagateCorners() == -1);
        TESTING_ASSERT(triPtr->getInterpolateBoundary() == -1);

        str = "triangle";
        unsigned int ttype = triPtr->read(0.0);
        TESTING_ASSERT(ttype == SubDReader::GEOMETRY_HOMOGENOUS);
        TESTING_ASSERT(str == triPtr->getName());
        TESTING_ASSERT(triPtr->mPoints.size() == 9);
        TESTING_ASSERT(triPtr->mFacePoints.size() == 3);
        TESTING_ASSERT(triPtr->mFaceList.size() == 2);

        TESTING_ASSERT(triPtr->mPoints[0] == 0.0);
        TESTING_ASSERT(triPtr->mPoints[1] == 0.0);
        TESTING_ASSERT(triPtr->mPoints[2] == 0.0);

        TESTING_ASSERT(triPtr->mPoints[3] == 0.0);
        TESTING_ASSERT(triPtr->mPoints[4] == 0.0);
        TESTING_ASSERT(triPtr->mPoints[5] == 1.0);

        TESTING_ASSERT(triPtr->mPoints[6] == 0.0);
        TESTING_ASSERT(triPtr->mPoints[7] == 1.0);
        TESTING_ASSERT(triPtr->mPoints[8] == 0.0);

        ttype = triPtr->read(1.0);
        TESTING_ASSERT(triPtr->mPoints[0] == -1.0);
        TESTING_ASSERT(triPtr->mPoints[1] == -1.0);
        TESTING_ASSERT(triPtr->mPoints[2] == -1.0);

        TESTING_ASSERT(triPtr->mPoints[3] == 2.0);
        TESTING_ASSERT(triPtr->mPoints[4] == 2.0);
        TESTING_ASSERT(triPtr->mPoints[5] == 2.0);

        TESTING_ASSERT(triPtr->mPoints[6] == 4.0);
        TESTING_ASSERT(triPtr->mPoints[7] == 4.0);
        TESTING_ASSERT(triPtr->mPoints[8] == 4.0);

        ttype = triPtr->read(2.0);
        TESTING_ASSERT(triPtr->mPoints[0] == -1.0);
        TESTING_ASSERT(triPtr->mPoints[1] == -2.0);
        TESTING_ASSERT(triPtr->mPoints[2] == -3.0);

        TESTING_ASSERT(triPtr->mPoints[3] == 1.0);
        TESTING_ASSERT(triPtr->mPoints[4] == 2.0);
        TESTING_ASSERT(triPtr->mPoints[5] == 3.0);

        TESTING_ASSERT(triPtr->mPoints[6] == 4.0);
        TESTING_ASSERT(triPtr->mPoints[7] == 5.0);
        TESTING_ASSERT(triPtr->mPoints[8] == 6.0);
        TESTING_ASSERT(ttype == SubDReader::GEOMETRY_HOMOGENOUS);

        std::set<float> frames;
        triPtr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 3);
        TESTING_ASSERT(triPtr->hasFrames());

        // has 0, 1 and 2 but doesnt have 3
        TESTING_ASSERT(frames.find(0.0) != frames.end());
        TESTING_ASSERT(frames.find(1.0) != frames.end());
        TESTING_ASSERT(frames.find(2.0) != frames.end());
        TESTING_ASSERT(frames.find(3.0) == frames.end());
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);
}
