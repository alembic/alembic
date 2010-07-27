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

#include <Alembic/Tako/Tests/PolyMeshTest.h>
#include <Alembic/Tako/Tests/Utility.h>

#include <Alembic/Tako/PolyMeshReader.h>
#include <Alembic/Tako/PolyMeshWriter.h>
#include <Alembic/Tako/TransformOperation.h>
#include <Alembic/Tako/TransformReader.h>
#include <Alembic/Tako/TransformWriter.h>

namespace
{
// CPPUNIT_TEST_SUITE_REGISTRATION(PolyMeshTest);
}

using namespace Alembic::Tako;


void PolyMeshTest::testPolyMeshSimple(void)
{
    // points
    std::vector<float> p;
    float pArray[] = {0,0,0, 0,0,1, 0,1,0, 0,1,1, 1,0,0, 1,0,1, 1,1,0, 1,1,1};
    arrayToVec<float>( 24, pArray, p );

    // normals
    std::vector<float> n;
    float nArray[] = {  -1,0,0, -1,0,0, -1,0,0, -1,0,0, // face [2, 3, 1, 0]
                        0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1, // face [2, 6, 4, 0]
                        0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, // face [1, 5, 4, 0]
                        0,0,+1, 0,0,+1, 0,0,+1, 0,0,+1, // face [5, 7, 3, 1]
                        0,+1,0, 0,+1,0, 0,+1,0, 0,+1,0, // face [6, 7, 3, 2]
                        +1,0,0, +1,0,0, +1,0,0, +1,0,0, // face [2, 3, 1, 0]
                        };
    arrayToVec<float>( 8*3*3, nArray, n );

    // facepoints
    std::vector<index_t> fp;
    index_t fpArray[] = { 0, 1, 3, 2,   0, 4, 6, 2,   0, 4, 5, 1,
                          1, 3, 7, 5,   2, 3, 7, 6,   4, 6, 7, 5 };
    arrayToVec<index_t>( 6*4, fpArray, fp );

    // facelist
    std::vector<index_t> fl;
    index_t flArray[] = { 0, 4, 8, 12, 16, 20, 24 };
    arrayToVec<index_t>( 7, flArray, fl );

    hid_t fid = -1;
    {
        TransformWriter root("/tmp/testpoly.hdf");
        fid = H5Iget_file_id(root.getHid());

        TransformWriter tw("test", root);
        Value tx, ty, tz;
        tx = 1.0;
        ty = 2.0;
        tz = 3.0;
        TransformOperationVariant t(Translate(Translate::cTranslate, tx, ty, tz));
        tw.push(t);

        tw.writeTransformStack(0.0, true);

        PolyMeshWriter pw("cube", tw);
        pw.write(FLT_MAX, p, n, fp, fl);
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    {
        TransformReader root("/tmp/testpoly.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(
            root.getChild(0));

        TESTING_ASSERT(str == testPtr->getName());
        PolyMeshReaderPtr cubePtr = boost::get< PolyMeshReaderPtr >(
            testPtr->getChild(0));

        str = "cube";
        PolyMeshReader::TopologyType ttype = cubePtr->read(0.0);
        TESTING_ASSERT(ttype == PolyMeshReader::TOPOLOGY_STATIC);
        TESTING_ASSERT(str == cubePtr->getName());
        TESTING_ASSERT(cubePtr->mPoints.size() == 24);
        TESTING_ASSERT(cubePtr->mNormals.size() == 72);
        TESTING_ASSERT(cubePtr->mFacePoints.size() == 24);
        TESTING_ASSERT(cubePtr->mFaceList.size() == 7);
        ttype = cubePtr->read(150.0);
        TESTING_ASSERT(ttype == PolyMeshReader::TOPOLOGY_STATIC);

        std::set<float> frames;
        cubePtr->getFrames(frames);
        TESTING_ASSERT(frames.size() == 0);
        TESTING_ASSERT(!cubePtr->hasFrames());
        TESTING_ASSERT(!cubePtr->hasPropertyFrames());
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);
}

void PolyMeshTest::testPolyMeshAnimated()
{
    // create a triangle

    // points
    std::vector<float> pFrame01;
    pFrame01.push_back(0.0); pFrame01.push_back(0.0); pFrame01.push_back(0.0);
    pFrame01.push_back(0.0); pFrame01.push_back(0.0); pFrame01.push_back(1.0);
    pFrame01.push_back(0.0); pFrame01.push_back(1.0); pFrame01.push_back(0.0);
    std::vector<float> pFrame02;
    pFrame02.push_back(-1.0); pFrame02.push_back(-1.0); pFrame02.push_back(-1.0);
    pFrame02.push_back(+2.0); pFrame02.push_back(+2.0); pFrame02.push_back(+2.0);
    pFrame02.push_back(+4.0); pFrame02.push_back(+4.0); pFrame02.push_back(+4.0);
    std::vector<float> pFrame03;
    pFrame03.push_back(-1.0); pFrame03.push_back(-2.0); pFrame03.push_back(-3.0);
    pFrame03.push_back(+1.0); pFrame03.push_back(+2.0); pFrame03.push_back(+3.0);
    pFrame03.push_back(+4.0); pFrame03.push_back(+5.0); pFrame03.push_back(+6.0);
    std::vector<float> pArrays[3] = {pFrame01, pFrame02, pFrame03};

    std::vector<float> nFrame01;
    nFrame01.push_back(0.0);  nFrame01.push_back(1.0);  nFrame01.push_back(0.0);
    nFrame01.push_back(0.0);  nFrame01.push_back(1.0);  nFrame01.push_back(0.0);
    nFrame01.push_back(0.0);  nFrame01.push_back(1.0);  nFrame01.push_back(0.0);
    std::vector<float> nFrame02;
    nFrame02.push_back(0.5);  nFrame02.push_back(1.0);  nFrame02.push_back(0.0);
    nFrame02.push_back(-0.5); nFrame02.push_back(1.0);  nFrame02.push_back(0.75);
    nFrame02.push_back(0.15); nFrame02.push_back(1.0);  nFrame02.push_back(0.25);
    std::vector<float> nFrame03;
    nFrame03.push_back(1.0);  nFrame03.push_back(0.5);  nFrame03.push_back(-0.6);
    nFrame03.push_back(1.0);  nFrame03.push_back(0.75); nFrame03.push_back(0.6);
    nFrame03.push_back(0.6);  nFrame03.push_back(1.0);  nFrame03.push_back(0.0);
    std::vector<float> nArrays[3] = {nFrame01, nFrame02, nFrame03};

    std::vector<index_t> fp;
    fp.push_back(0); fp.push_back(1); fp.push_back(2);

    std::vector<index_t> fl;
    fl.push_back(0); fl.push_back(3);

    hid_t fid = -1;
    {
        TransformWriter root("/tmp/testTriangleAnim.hdf");
        fid = H5Iget_file_id(root.getHid());

        // identity matrix
        TransformWriter tw("test", root);
        tw.writeTransformStack(0.0, true);

        PolyMeshWriter pw("triangle", tw);
        pw.write(0.0, pFrame01, nFrame01, fp, fl);
        pw.write(1.0, pFrame02, nFrame02);
        pw.write(2.0, pFrame03, nFrame03);

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
        TransformReader root("/tmp/testTriangleAnim.hdf");
        fid = H5Iget_file_id(root.getHid());

        std::string str = "test";
        TransformReaderPtr testPtr = boost::get< TransformReaderPtr >(
            root.getChild(0));

        TESTING_ASSERT(str == testPtr->getName());
        PolyMeshReaderPtr triPtr = boost::get< PolyMeshReaderPtr >(
            testPtr->getChild(0));

        str = "triangle";
        PolyMeshReader::TopologyType ttype;
        for ( unsigned int frame = 0; frame < 3; frame ++ )
        {
            ttype = triPtr->read(float(frame));
            TESTING_ASSERT(ttype == PolyMeshReader::TOPOLOGY_HOMOGENOUS);
            TESTING_ASSERT(str == triPtr->getName());
            TESTING_ASSERT(triPtr->mPoints.size() == 9);
            TESTING_ASSERT(triPtr->mNormals.size() == 9);
            TESTING_ASSERT(triPtr->mFacePoints.size() == 3);
            TESTING_ASSERT(triPtr->mFaceList.size() == 2);

            // check points
            for ( unsigned int i = 0; i < 9; i++ )
            {
                TESTING_ASSERT(triPtr->mPoints[i] == pArrays[frame][i]);
                TESTING_ASSERT(triPtr->mNormals[i] == nArrays[frame][i]);
            }
        }

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
