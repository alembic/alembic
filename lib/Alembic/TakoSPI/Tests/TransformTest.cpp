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

#include <Alembic/TakoSPI/Tests/TransformTest.h>
#include <Alembic/TakoAbstract/TransformOperation.h>
#include <Alembic/TakoAbstract/TransformReader.h>
#include <Alembic/TakoAbstract/TransformWriter.h>

namespace {

// CPPUNIT_TEST_SUITE_REGISTRATION(TransformTest);

void transformMaker(TransformWriterPtr parent, int depth)
{
    unsigned int C = 0;
    for (;C < 3; C++)
    {
        std::stringstream strm;
        strm << C;
        TransformWriterPtr child =
            MakeTransformWriterPtr(strm.str(), parent);
        Value xScale = 1.5;
        Value yScale = 1.25;
        Value zScale = 1.0;
        Value tVal = 2.0;
        TransformOperationVariant s(
            Scale(Scale::cScale, xScale, yScale, zScale));
        child->push(s);
        TransformOperationVariant t(Translate(Translate::cTranslate,
            tVal, tVal, tVal));
        child->push(t);
        child->writeTransformStack(0.0, true);
        if (depth < 8)
            transformMaker(child, depth+1);
    }
}

}

void TransformTest::testTransformOperations(void)
{

    Value tx = 2.0;
    Value ty = 4.0;
    Value tz = 6.0;

    Translate t(Translate::cTranslate, tx, ty, tz);
    TESTING_ASSERT(t.get() == Mat4x4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        2.0, 4.0, 6.0, 1.0));

    Value vx,vy,vz;
    t.get(vx,vy,vz);

    TESTING_ASSERT(vx == 2.0);
    TESTING_ASSERT(vx.mIsStatic);
    TESTING_ASSERT(vy == 4.0);
    TESTING_ASSERT(vy.mIsStatic);
    TESTING_ASSERT(vz == 6.0);
    TESTING_ASSERT(vz.mIsStatic);

    Value sx = 3.0;
    Value sy = 6.0;
    Value sz = 9.0;

    Scale s(Scale::cScale, sx, sy, sz);
    TESTING_ASSERT(s.get() == Mat4x4(
        3.0, 0.0, 0.0, 0.0,
        0.0, 6.0, 0.0, 0.0,
        0.0, 0.0, 9.0, 0.0,
        0.0, 0.0, 0.0, 1.0));

    s.get(vx,vy,vz);
    TESTING_ASSERT(vx == 3.0);
    TESTING_ASSERT(vx.mIsStatic);
    TESTING_ASSERT(vy == 6.0);
    TESTING_ASSERT(vy.mIsStatic);
    TESTING_ASSERT(vz == 9.0);
    TESTING_ASSERT(vz.mIsStatic);

    double angle = M_PI * 0.25;
    Value rAngle = angle;
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);

    Rotate rXAxis(Rotate::cRotate, rAngle, 1.0, 0.0, 0.0);
    TESTING_ASSERT(rXAxis.get().equalWithRelError(Mat4x4(
        1.0, 0.0, 0.0, 0.0,
        0.0, cosAngle, sinAngle, 0.0,
        0.0, -sinAngle, cosAngle, 0.0,
        0.0, 0.0, 0.0, 1.0), 0.00001));

    Rotate rYAxis(Rotate::cRotate, rAngle, 0.0, 1.0, 0.0);
    TESTING_ASSERT(rYAxis.get().equalWithRelError(Mat4x4(
        cosAngle, 0.0, -sinAngle, 0.0,
        0.0, 1.0, 0.0, 0.0,
        sinAngle, 0.0, cosAngle, 0.0,
        0.0, 0.0, 0.0, 1.0), 0.00001));
    TESTING_ASSERT(rYAxis.getType() == Rotate::cRotate);

    Rotate rZAxis(Rotate::cRotateOrientation, rAngle, 0.0, 0.0, 1.0);
    TESTING_ASSERT(rZAxis.get().equalWithRelError(Mat4x4(
        cosAngle, sinAngle, 0.0, 0.0,
        -sinAngle, cosAngle, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0), 0.00001));
    TESTING_ASSERT(rZAxis.getType() == Rotate::cRotateOrientation);

    float xaxis, yaxis, zaxis;
    rZAxis.get(vx,xaxis, yaxis, zaxis);
    TESTING_ASSERT(vx == angle);
    TESTING_ASSERT(vx.mIsStatic);
    TESTING_ASSERT(xaxis == 0.0);
    TESTING_ASSERT(yaxis == 0.0);
    TESTING_ASSERT(zaxis == 1.0);

}

void TransformTest::testTransformWriterSimple(void)
{
    // hid_t fid = -1;

    // scope it so file will be closed, when root goes out of scope
    {
        TransformWriterPtr root = WriteScene("/tmp/test_simpleTrans.hdf");
        // fid = H5Iget_file_id(root.getHid());

        TransformWriterPtr tw = MakeTransformWriterPtr("test", root);
        Mat4x4 m;
        TESTING_ASSERT(m == tw->getMatrix());

        Value tx, ty, tz;
        tx = 1.0;
        ty = 2.0;
        tz = 3.0;

        TransformOperationVariant t(Translate(Translate::cTranslate, tx,
            ty, tz));
        tw->push(t);

        Value rAngle = 1.5707963267948966;
        TransformOperationVariant ry(Rotate(Rotate::cRotate, rAngle, 0.0, 1.0,
            0.0));

        tw->push(ry);

        Value ma[16];
        ma[ 0] = 1.0; ma[ 1] = 0.0; ma[ 2] = 0.0; ma[ 3] = 0.0;
        ma[ 4] = 0.0; ma[ 5] = 1.0; ma[ 6] = 0.0; ma[ 7] = 0.0;
        ma[ 8] = 0.0; ma[ 9] = 0.0; ma[10] = 1.0; ma[11] = 0.0;
        ma[12] = 0.0; ma[13] = 0.0; ma[14] = 0.0; ma[15] = 1.0;
        TransformOperationVariant mat(Matrix4x4(Matrix4x4::cMatrix,
            ma[ 0], ma[ 1], ma[ 2], ma[ 3],
            ma[ 4], ma[ 5], ma[ 6], ma[ 7],
            ma[ 8], ma[ 9], ma[10], ma[11],
            ma[12], ma[13], ma[14], ma[15]));
        tw->push(mat);

        Value sx, sy, sz;
        sx = 3.0;
        sy = 4.0;
        sz = 5.0;

        TransformOperationVariant s(Scale(Scale::cScale, sx, sy, sz));
        tw->push(s);

        PropertyPair p;
        p.first = "town";
        tw->setNonSampledProperty("taco", p);

        TESTING_ASSERT(tw->getMatrix().equalWithAbsError(Mat4x4(
            0.0, 0.0, -3.0, 0.0,
            0.0, 4.0, 0.0, 0.0,
            5.0, 0.0, 0.0, 0.0,
            1.0, 2.0, 3.0, 1.0), 0.00001));

        tw->writeTransformStack(0.0, false);
        tw->writeProperties(0.0);
    }
    //TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    //    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    //H5Fclose(fid);

    // open and read
    {
        TransformReaderPtr root = ReadScene("/tmp/test_simpleTrans.hdf");
        //fid = H5Iget_file_id(root.getHid());

        TransformReaderPtr tr = MakeTransformReaderPtr("test", root);

        TransformReader::SampleType s = tr->read(0.0);

        TESTING_ASSERT(s == TransformReader::SAMPLES_NONE);
        TESTING_ASSERT(tr->inheritsTransform() == 0);

        TransformReader::const_stack_iterator it = tr->begin();
        Translate tOp = boost::get<Translate>(*it);
        Value tx, ty, tz;
        tOp.get(tx, ty, tz);
        TESTING_ASSERT(tx == 1.0);
        TESTING_ASSERT(tx.mIsStatic);
        TESTING_ASSERT(ty == 2.0);
        TESTING_ASSERT(ty.mIsStatic);
        TESTING_ASSERT(tz == 3.0);
        TESTING_ASSERT(tz.mIsStatic);

        tr->readProperties(0.0);

        std::string strTest = "town";
        PropertyPair pr;
        tr->getProperty("taco", pr);
        TESTING_ASSERT(boost::get<std::string>(pr.first) == strTest);

        TESTING_ASSERT(tr->getMatrix().equalWithAbsError(Mat4x4(
            0.0, 0.0, -3.0, 0.0,
            0.0, 4.0, 0.0, 0.0,
            5.0, 0.0, 0.0, 0.0,
            1.0, 2.0, 3.0, 1.0), 0.00001));

        std::set<float> frames;
        tr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 0);
        TESTING_ASSERT(!tr->hasFrames());

        std::vector<double> samples;
        s = tr->getSamples(0.0, samples);
        TESTING_ASSERT(samples.size() == 0);
        TESTING_ASSERT(s == TransformReader::SAMPLES_NONE);
    }
    //TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    //    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    //H5Fclose(fid);
}


void TransformTest::testTransformWriterAnimated(void)
{
    // hid_t fid = -1;
    {
        TransformWriterPtr root = WriteScene("/tmp/test_animTrans.hdf");
        //fid = H5Iget_file_id(root.getHid());

        TransformWriterPtr tw = MakeTransformWriterPtr("test", root);
        Mat4x4 m;
        TESTING_ASSERT(m == tw->getMatrix());


        Value tx, ty, tz;
        tx = 1.0;
        tx.mIsStatic = false;

        ty = 2.0;
        ty.mIsStatic = false;

        tz = 3.0;
        tz.mIsStatic = false;

        TransformOperationVariant t(Translate(Translate::cTranslate, tx, ty,
            tz));
        tw->push(t);

        Value rAngle = 42.0;
        rAngle.mIsStatic = false;

        TransformOperationVariant ry(Rotate(Rotate::cRotate, rAngle, 0.0, 1.0,
            0.0));
        tw->push(ry);

        Value sx, sy, sz;
        sx = 100.0;
        sx.mIsStatic = false;

        sy = 200.0;
        sy.mIsStatic = false;

        sz = 300.0;
        sz.mIsStatic = false;

        TransformOperationVariant s(Scale(Scale::cScale, sx, sy, sz));
        tw->push(s);

        tw->writeTransformStack(0.0, true);

        std::vector<double> dVec;

        // for the translate
        dVec.push_back(10.0); dVec.push_back(20.0); dVec.push_back(30.0);

        // for the rotate
        dVec.push_back(43.0);

        // for the scale
        dVec.push_back(101.0); dVec.push_back(201.0); dVec.push_back(301.0);

        tw->writeSamples(1.0, dVec);

        // write the frame range
        std::vector<float> frames;
        frames.push_back(0.0);
        frames.push_back(1.0);
        PropertyPair p;
        p.first = frames;
        root->setNonSampledProperty("frameRange", p);
        root->writeProperties(0.0);
    }
    //TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    //    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    //H5Fclose(fid);

    // open and read
    {
        TransformReaderPtr root = ReadScene("/tmp/test_animTrans.hdf");
        // fid = H5Iget_file_id(root.getHid());

        TransformReaderPtr tr = MakeTransformReaderPtr("test", root);

        // read it wrong first to see if it will still build the stack
        // correctly.
        TransformReader::SampleType s = tr->read(70.0);
        TESTING_ASSERT(s == TransformReader::READ_ERROR);

        s = tr->read(0.0);
        TESTING_ASSERT(s == TransformReader::SAMPLES_READ);

        TransformReader::const_stack_iterator it = tr->begin();
        Translate tOp = boost::get<Translate>(*it);

        Value tvOpX, tvOpY, tvOpZ;
        tOp.get(tvOpX, tvOpY, tvOpZ);
        TESTING_ASSERT(tvOpX == 1.0);
        TESTING_ASSERT(!tvOpX.mIsStatic);
        TESTING_ASSERT(tvOpY == 2.0);
        TESTING_ASSERT(!tvOpY.mIsStatic);
        TESTING_ASSERT(tvOpZ == 3.0);
        TESTING_ASSERT(!tvOpZ.mIsStatic);

        it++;
        Rotate rOp = boost::get<Rotate>(*it);

        float rOpX, rOpY, rOpZ;
        Value rvOpA;
        rOp.get(rvOpA, rOpX, rOpY, rOpZ);
        TESTING_ASSERT(rvOpA == 42.0);
        TESTING_ASSERT(!rvOpA.mIsStatic);
        TESTING_ASSERT(rOpX == 0.0);
        TESTING_ASSERT(rOpY == 1.0);
        TESTING_ASSERT(rOpZ == 0.0);

        it++;
        Scale sOp = boost::get<Scale>(*it);
        Value svOpX, svOpY, svOpZ;
        sOp.get(svOpX, svOpY, svOpZ);
        TESTING_ASSERT(svOpX == 100.0);
        TESTING_ASSERT(!svOpX.mIsStatic);
        TESTING_ASSERT(svOpY == 200.0);
        TESTING_ASSERT(!svOpY.mIsStatic);
        TESTING_ASSERT(svOpZ == 300.0);
        TESTING_ASSERT(!svOpZ.mIsStatic);

        it++;
        TESTING_ASSERT(it == tr->end());

        s = tr->read(1.0);
        TESTING_ASSERT(s == TransformReader::SAMPLES_READ);
        it = tr->begin();

        tOp = boost::get<Translate>(*it);
        tOp.get(tvOpX, tvOpY, tvOpZ);
        TESTING_ASSERT(tvOpX == 10.0);
        TESTING_ASSERT(!tvOpX.mIsStatic);
        TESTING_ASSERT(tvOpY == 20.0);
        TESTING_ASSERT(!tvOpY.mIsStatic);
        TESTING_ASSERT(tvOpZ == 30.0);
        TESTING_ASSERT(!tvOpZ.mIsStatic);

        it++;
        rOp = boost::get<Rotate>(*it);
        rOp.get(rvOpA, rOpX, rOpY, rOpZ);
        TESTING_ASSERT(rvOpA == 43.0);
        TESTING_ASSERT(!rvOpA.mIsStatic);
        TESTING_ASSERT(rOpX == 0.0);
        TESTING_ASSERT(rOpY == 1.0);
        TESTING_ASSERT(rOpZ == 0.0);

        it++;
        sOp = boost::get<Scale>(*it);
        sOp.get(svOpX, svOpY, svOpZ);
        TESTING_ASSERT(svOpX == 101.0);
        TESTING_ASSERT(!svOpX.mIsStatic);
        TESTING_ASSERT(svOpY == 201.0);
        TESTING_ASSERT(!svOpY.mIsStatic);
        TESTING_ASSERT(svOpZ == 301.0);
        TESTING_ASSERT(!svOpZ.mIsStatic);

        it++;
        TESTING_ASSERT(it == tr->end());

        std::vector <double> samples;
        s = tr->getSamples(1.0, samples);
        TESTING_ASSERT(s == TransformReader::SAMPLES_READ);
        TESTING_ASSERT(samples.size() == 7);

        // illegal frame test
        samples.clear();
        s = tr->getSamples(2.0, samples);
        TESTING_ASSERT(s == TransformReader::READ_ERROR);
        TESTING_ASSERT(samples.size() == 0);

        // another illegal frame test
        s = tr->read(2.0);
        TESTING_ASSERT(s == TransformReader::READ_ERROR);

        std::set<float> frames;
        tr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 2);
        TESTING_ASSERT(tr->hasFrames());

        TESTING_ASSERT(frames.find(0.0) != frames.end());
        TESTING_ASSERT(frames.find(1.0) != frames.end());
        TESTING_ASSERT(frames.find(2.0) == frames.end());

    }
    //TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    //    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    //H5Fclose(fid);
}

void TransformTest::testTransformHierarchy(void)
{
    // hid_t fid = -1;

    // scope it so that the objects are released and their groups are closed
    {
        TransformWriterPtr root = WriteScene("/tmp/test_hierTrans.hdf");
        //fid = H5Iget_file_id(root.getHid());

        TransformWriterPtr test = MakeTransformWriterPtr("test", root);

        TransformWriterPtr taco = MakeTransformWriterPtr("taco", test);
        TransformWriterPtr burrito = MakeTransformWriterPtr("burrito", taco);

        TransformWriterPtr town = MakeTransformWriterPtr("town", test);
        TransformWriterPtr tuesday = MakeTransformWriterPtr("tuesday", town);
        TransformWriterPtr wednesday = MakeTransformWriterPtr("wednesday", town);
        TransformWriterPtr monday = MakeTransformWriterPtr("monday", town);

    }
    //TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    //    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    //H5Fclose(fid);

    {
        TransformReaderPtr root = ReadScene("/tmp/test_hierTrans.hdf");
        //fid = H5Iget_file_id(root.getHid());

        TESTING_ASSERT(root->numChildren() == 1);
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(
            root->getChild(0));
        std::string str = "test";
        TESTING_ASSERT(str == testPtr->getName());

        TESTING_ASSERT(testPtr->numChildren() == 2);
        TransformReaderPtr tacoPtr = boost::get< TransformReaderPtr >(
            testPtr->getChild(0));
        TransformReaderPtr townPtr = boost::get< TransformReaderPtr >(
            testPtr->getChild(1));
        str = "taco";
        TESTING_ASSERT(str == tacoPtr->getName());
        str = "town";
        TESTING_ASSERT(str == townPtr->getName());
        TESTING_ASSERT(tacoPtr->numChildren() == 1);
        TESTING_ASSERT(townPtr->numChildren() == 3);

        TransformReaderPtr burritoPtr = boost::get< TransformReaderPtr >(
            tacoPtr->getChild(0));
        TESTING_ASSERT(burritoPtr->numChildren() == 0);
        str = "burrito";
        TESTING_ASSERT(str == burritoPtr->getName());

        TransformReaderPtr tuesdayPtr = boost::get< TransformReaderPtr >(
            townPtr->getChild(0));
        TESTING_ASSERT(tuesdayPtr->numChildren() == 0);
        str = "tuesday";
        TESTING_ASSERT(str == tuesdayPtr->getName());

        TransformReaderPtr wednesdayPtr = boost::get< TransformReaderPtr >(
            townPtr->getChild(1));
        TESTING_ASSERT(wednesdayPtr->numChildren() == 0);
        str = "wednesday";
        TESTING_ASSERT(str == wednesdayPtr->getName());

        TransformReaderPtr mondayPtr = boost::get< TransformReaderPtr >(
            townPtr->getChild(2));
        TESTING_ASSERT(mondayPtr->numChildren() == 0);
        str = "monday";
        TESTING_ASSERT(str == mondayPtr->getName());
    }
    //TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    //    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    //H5Fclose(fid);
}

void TransformTest::testDeepHierarchy(void)
{
    //hid_t fid = -1;
    {
        TransformWriterPtr root = WriteScene("/tmp/test_bigHier.hdf");
        //fid = H5Iget_file_id(root.getHid());

        TransformWriterPtr parent = MakeTransformWriterPtr("main", root);

        transformMaker(parent, 0);
    }
    //TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    //    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    //H5Fclose(fid);

    {
        TransformReaderPtr root = ReadScene("/tmp/test_bigHier.hdf");
        // fid = H5Iget_file_id(root.getHid());

        TransformReaderPtr main = MakeTransformReaderPtr("main", root);

        TESTING_ASSERT(main->numChildren() == 3);
        TESTING_ASSERT(std::string("main") == main->getName());

        TransformReaderPtr child = MakeTransformReaderPtr("main/0/1/0/2/0/2/1", root);
        TESTING_ASSERT(child->numChildren() == 3);

        TransformReader::const_stack_iterator it, end;
        child->read(0.0);
        unsigned int opCount = 0;
        for (it = child->begin(), end = child->end(); it != end; ++it)
            opCount ++;

        TESTING_ASSERT(std::string("1") == child->getName());
        TESTING_ASSERT(opCount == 2);
    }
    //TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    //    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    //H5Fclose(fid);
}
