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

#include <Alembic/Tako/Tests/PointPrimitiveTest.h>

#include <Alembic/Tako/PointPrimitiveReader.h>
#include <Alembic/Tako/PointPrimitiveWriter.h>
#include <Alembic/Tako/TransformOperation.h>
#include <Alembic/Tako/TransformReader.h>
#include <Alembic/Tako/TransformWriter.h>

namespace {

const unsigned int NUMPOINT = 10;

// CPPUNIT_TEST_SUITE_REGISTRATION(PointPrimitiveTest);

}

using namespace Alembic::Tako;

void PointPrimitiveTest::testPointPrimitiveSimple(void)
{
    std::vector<float> position;
    std::vector<float> velocity;
    std::vector<int> particleIds;
    std::vector<float> width;

    for (unsigned int i=0; i< NUMPOINT; i++)
    {
        position.push_back(0.0); position.push_back(0.0); position.push_back(0.0);
        velocity.push_back(0.0); velocity.push_back(0.0); velocity.push_back(0.0);
        particleIds.push_back(i);
        width.push_back(0.2);
    }

    hid_t fid = -1;

    {
        TransformWriter root("/tmp/testpoint.hdf");
        fid = H5Iget_file_id(root.getHid());

        TransformWriter tw("test", root);
        Value tx, ty, tz;
        tx = 1.0;
        ty = 2.0;
        tz = 3.0;
        TransformOperationVariant t(Translate(Translate::cTranslate, tx, ty, tz));
        tw.push(t);
        tw.writeTransformStack(0.0, true);

        PointPrimitiveWriter pw("point primitive", tw);
        pw.write(FLT_MAX, position, velocity, particleIds, width);

        PointPrimitiveWriter psw("just_points", tw);
        psw.write(FLT_MAX, position, std::vector<float>(), std::vector<int>(), 0.25);
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    {
        TransformReader root("/tmp/testpoint.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(
            root.getChild(0));
        TESTING_ASSERT(str == testPtr->getName());

        PointPrimitiveReaderPtr ptr = boost::get< PointPrimitiveReaderPtr >(
            testPtr->getChild(0));
        str = "point primitive";
        int ttype = ptr->read(0.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_STATIC);
        TESTING_ASSERT(str == ptr->getName());
        TESTING_ASSERT(ptr->mPosition.size() == NUMPOINT*3);
        TESTING_ASSERT(ptr->mVelocity.size() == NUMPOINT*3);
        TESTING_ASSERT(ptr->mParticleIds.size() == NUMPOINT);
        TESTING_ASSERT(ptr->mWidth.size() == NUMPOINT);

        for (unsigned int i=0; i< NUMPOINT; i++)
        {
            TESTING_ASSERT(ptr->mPosition[3*i+0] == position[3*i+0]);
            TESTING_ASSERT(ptr->mPosition[3*i+1] == position[3*i+1]);
            TESTING_ASSERT(ptr->mPosition[3*i+2] == position[3*i+2]);

            TESTING_ASSERT(ptr->mVelocity[3*i+0] == velocity[3*i+0]);
            TESTING_ASSERT(ptr->mVelocity[3*i+1] == velocity[3*i+1]);
            TESTING_ASSERT(ptr->mVelocity[3*i+2] == velocity[3*i+2]);

            TESTING_ASSERT(ptr->mParticleIds[i] == particleIds[i]);
            TESTING_ASSERT(ptr->mWidth[i] == width[i]);
        }

        ttype = ptr->read(150.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_STATIC);

        std::set<float> frames;
        ptr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 0);
        TESTING_ASSERT(!ptr->hasFrames());
        TESTING_ASSERT(!ptr->hasPropertyFrames());

        ptr = boost::get< PointPrimitiveReaderPtr >(testPtr->getChild(1));
        str = "just_points";
        ttype = ptr->read(0.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_STATIC);
        TESTING_ASSERT(str == ptr->getName());
        TESTING_ASSERT(ptr->mPosition.size() == NUMPOINT*3);
        TESTING_ASSERT(ptr->mVelocity.empty());
        TESTING_ASSERT(ptr->mParticleIds.empty());
        TESTING_ASSERT(ptr->mWidth.empty());
        TESTING_ASSERT(ptr->mConstantWidth == 0.25);
        for (unsigned int i=0; i< NUMPOINT; i++)
        {
            TESTING_ASSERT(ptr->mPosition[3*i+0] == position[3*i+0]);
            TESTING_ASSERT(ptr->mPosition[3*i+1] == position[3*i+1]);
            TESTING_ASSERT(ptr->mPosition[3*i+2] == position[3*i+2]);
        }

        ttype = ptr->read(150.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_STATIC);
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);
}

void PointPrimitiveTest::testPointPrimitiveAnimated()
{
    std::vector<float> position;
    std::vector<float> velocity;
    std::vector<int> particleIds;
    std::vector<float> widths;

    std::vector <float> empty;
    std::vector <int> emptyId;

    for (unsigned int i=0; i< NUMPOINT; i++)
    {
        particleIds.push_back(i);
        widths.push_back(i*0.1);
    }

    hid_t fid = -1;

    {
        TransformWriter root("/tmp/testpoint.hdf");
        fid = H5Iget_file_id(root.getHid());

        TransformWriter tw("test", root);
        tw.writeTransformStack(0.0, true);

        position = std::vector<float>(3*NUMPOINT, 0.0);
        velocity = std::vector<float>(3*NUMPOINT, 1.0);
        PointPrimitiveWriter pw("point primitive", tw);
        pw.write(0.0, position, velocity, particleIds, 2.0);
        position = std::vector<float>(3*NUMPOINT, 3.0);
        velocity = std::vector<float>(3*NUMPOINT, 4.0);
        pw.write(1.0, position, velocity, particleIds, widths);
        pw.write(2.0, empty, empty, emptyId, empty);
        pw.write(3.0, position, empty, emptyId, 4.5);
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    {
        TransformReader root("/tmp/testpoint.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(
            root.getChild(0));
        TESTING_ASSERT(str == testPtr->getName());

        PointPrimitiveReaderPtr ptr = boost::get< PointPrimitiveReaderPtr >(
            testPtr->getChild(0));
        str = "point primitive";
        TESTING_ASSERT(str == ptr->getName());

        int ttype = ptr->read(0.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_ANIMATED);
        TESTING_ASSERT(ptr->mPosition.size() == NUMPOINT*3);
        TESTING_ASSERT(ptr->mPosition == std::vector<float>(3*NUMPOINT, 0.0));
        TESTING_ASSERT(ptr->mVelocity.size() == NUMPOINT*3);
        TESTING_ASSERT(ptr->mVelocity == std::vector<float>(3*NUMPOINT, 1.0));
        TESTING_ASSERT(ptr->mParticleIds.size() == NUMPOINT);
        TESTING_ASSERT(ptr->mParticleIds == particleIds);
        TESTING_ASSERT(ptr->mWidth.size() == 0);
        TESTING_ASSERT(ptr->mConstantWidth == 2.0);

        ttype = ptr->read(1.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_ANIMATED);
        TESTING_ASSERT(ptr->mPosition.size() == NUMPOINT*3);
        TESTING_ASSERT(ptr->mPosition == std::vector<float>(3*NUMPOINT, 3.0));
        TESTING_ASSERT(ptr->mVelocity.size() == NUMPOINT*3);
        TESTING_ASSERT(ptr->mVelocity == std::vector<float>(3*NUMPOINT, 4.0));
        TESTING_ASSERT(ptr->mParticleIds.size() == NUMPOINT);
        TESTING_ASSERT(ptr->mParticleIds == particleIds);
        TESTING_ASSERT(ptr->mWidth.size() == NUMPOINT);
        TESTING_ASSERT(ptr->mWidth == widths);
        TESTING_ASSERT(ptr->mConstantWidth == 0.0);

        ttype = ptr->read(2.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_ANIMATED);
        TESTING_ASSERT(ptr->mPosition.empty());
        TESTING_ASSERT(ptr->mVelocity.empty());
        TESTING_ASSERT(ptr->mParticleIds.empty());
        TESTING_ASSERT(ptr->mWidth.size() == 0);
        TESTING_ASSERT(ptr->mConstantWidth == 0.0);

        ttype = ptr->read(3.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_ANIMATED);
        TESTING_ASSERT(ptr->mPosition.size() == NUMPOINT*3);
        TESTING_ASSERT(ptr->mPosition == std::vector<float>(3*NUMPOINT, 3.0));
        TESTING_ASSERT(ptr->mVelocity.empty());
        TESTING_ASSERT(ptr->mParticleIds.empty());
        TESTING_ASSERT(ptr->mWidth.size() == 0);
        TESTING_ASSERT(ptr->mConstantWidth == 4.5);

        std::set<float> frames;
        ptr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 4);
        TESTING_ASSERT(ptr->hasFrames());
        TESTING_ASSERT(!ptr->hasPropertyFrames());
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);
}

void PointPrimitiveTest::testPointPrimitiveIdsAnimated()
{
    std::vector<float> position;
    std::vector<int> particleIds;

    std::vector <float> empty;
    std::vector <int> emptyId;

    for (unsigned int i=0; i< NUMPOINT; i++)
    {
        particleIds.push_back(i);
    }

    hid_t fid = -1;

    {
        TransformWriter root("/tmp/testpointids.hdf");
        fid = H5Iget_file_id(root.getHid());

        TransformWriter tw("test", root);
        tw.writeTransformStack(0.0, true);

        position = std::vector<float>(3*NUMPOINT, 0.0);

        PointPrimitiveWriter pw("point primitive", tw);
        pw.write(0.0, position, empty, particleIds, 2.0);
        position = std::vector<float>(3*NUMPOINT, 3.0);
        pw.write(1.0, position, empty, particleIds, 4.0);
        pw.write(2.0, empty, empty, emptyId, empty);
        pw.write(3.0, position, empty, particleIds, 8.0);
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    {
        TransformReader root("/tmp/testpointids.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(
        root.getChild(0));
        TESTING_ASSERT(str == testPtr->getName());

        PointPrimitiveReaderPtr ptr = boost::get< PointPrimitiveReaderPtr >(
        testPtr->getChild(0));
        str = "point primitive";
        TESTING_ASSERT(str == ptr->getName());

        int ttype = ptr->read(0.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_ANIMATED);
        TESTING_ASSERT(ptr->mPosition.size() == NUMPOINT*3);
        TESTING_ASSERT(ptr->mPosition == std::vector<float>(3*NUMPOINT, 0.0));
        TESTING_ASSERT(ptr->mVelocity.empty());
        TESTING_ASSERT(ptr->mParticleIds.size() == NUMPOINT);
        TESTING_ASSERT(ptr->mParticleIds == particleIds);
        TESTING_ASSERT(ptr->mWidth.size() == 0);
        TESTING_ASSERT(ptr->mConstantWidth == 2.0);

        ttype = ptr->read(1.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_ANIMATED);
        TESTING_ASSERT(ptr->mPosition.size() == NUMPOINT*3);
        TESTING_ASSERT(ptr->mPosition == std::vector<float>(3*NUMPOINT, 3.0));
        TESTING_ASSERT(ptr->mVelocity.empty());
        TESTING_ASSERT(ptr->mParticleIds.size() == NUMPOINT);
        TESTING_ASSERT(ptr->mParticleIds == particleIds);
        TESTING_ASSERT(ptr->mWidth.empty());
        TESTING_ASSERT(ptr->mConstantWidth == 4.0);

        ttype = ptr->read(2.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_ANIMATED);
        TESTING_ASSERT(ptr->mPosition.empty());
        TESTING_ASSERT(ptr->mVelocity.empty());
        TESTING_ASSERT(ptr->mParticleIds.empty());
        TESTING_ASSERT(ptr->mWidth.size() == 0);
        TESTING_ASSERT(ptr->mConstantWidth == 0.0);

        ttype = ptr->read(3.0);
        TESTING_ASSERT(ttype == PointPrimitiveReader::POINT_ANIMATED);
        TESTING_ASSERT(ptr->mPosition.size() == NUMPOINT*3);
        TESTING_ASSERT(ptr->mPosition == std::vector<float>(3*NUMPOINT, 3.0));
        TESTING_ASSERT(ptr->mVelocity.empty());
        TESTING_ASSERT(ptr->mParticleIds == particleIds);
        TESTING_ASSERT(ptr->mWidth.size() == 0);
        TESTING_ASSERT(ptr->mConstantWidth == 8.0);

        std::set<float> frames;
        ptr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 4);
        TESTING_ASSERT(ptr->hasFrames());
        TESTING_ASSERT(!ptr->hasPropertyFrames());
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
    H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);
}
