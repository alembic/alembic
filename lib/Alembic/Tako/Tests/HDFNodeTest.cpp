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

#include <Alembic/Tako/Tests/HDFNodeTest.h>
#include <Alembic/Tako/TransformReader.h>
#include <Alembic/Tako/TransformWriter.h>

using namespace Alembic::Tako;
using Alembic::Tako::uint16_t;
using Alembic::Tako::int16_t;
using Alembic::Tako::uint32_t;
using Alembic::Tako::int32_t;
using Alembic::Tako::uint64_t;
using Alembic::Tako::int64_t;

namespace {

// CPPUNIT_TEST_SUITE_REGISTRATION(HDFNodeTest);

}

void HDFNodeTest::test(void)
{
    // set up some of the variables which we will be adding to the property
    // maps
    int i = 100;

    double d = 42.3;

    std::vector<int> iv;
    iv.push_back(32);
    iv.push_back(64);
    iv.push_back(128);

    std::vector<double> dv;
    dv.push_back(0.5);
    dv.push_back(0.25);
    dv.push_back(0.125);

    int si = 22;

    std::vector<double> sdv;
    sdv.push_back(2.2);
    sdv.push_back(4.4);
    sdv.push_back(8.8);

    std::string str = "Hello, World!";
    std::string empty = "";

    std::vector<std::string> strVec;
    strVec.push_back("one");
    strVec.push_back("two");
    strVec.push_back("three");
    strVec.push_back("four");

    hid_t fid = -1;

    // scope this so that HDFWriterNode goes out of scope (and closes its group)
    // before the file is closed
    {

        TransformWriter node("/tmp/testNode.hdf");
        fid = H5Iget_file_id(node.getHid());

        PropertyPair p;
        p.second.scope = SCOPE_PRIMITIVE;
        p.second.inputType = ARBATTR_NONE;
        p.second.outputType = ARBATTR_NONE;

        p.first = i;
        node.setNonSampledProperty("integer", p);

        p.first = d;
        node.setNonSampledProperty("double", p);

        p.first = iv;
        node.setNonSampledProperty("int vector", p);

        p.first = dv;
        node.setNonSampledProperty("double vector", p);

        p.first = strVec;
        node.setNonSampledProperty("string vector", p);

        p.first = str;
        p.second.scope = SCOPE_POINT;
        p.second.inputType = ARBATTR_VECTOR3;
        p.second.outputType = ARBATTR_COLOR4;
        p.second.index.push_back(45);
        p.second.index.push_back(33);
        node.setNonSampledProperty("string", p);

        PropertyPair pr;
        TESTING_ASSERT(node.getProperty("integer", pr) == PROPERTY_STATIC);
        TESTING_ASSERT(boost::get<int>(pr.first) == i);

        TESTING_ASSERT(node.getProperty("double", pr) == PROPERTY_STATIC);
        TESTING_ASSERT(boost::get<double>(pr.first) == d);

        TESTING_ASSERT(node.getProperty("int vector", pr) == PROPERTY_STATIC);
        TESTING_ASSERT(boost::get<std::vector<int> >(pr.first) == iv);

        TESTING_ASSERT(node.getProperty("double vector", pr) ==
            PROPERTY_STATIC);
        TESTING_ASSERT(boost::get<std::vector<double> >(pr.first) == dv);

        TESTING_ASSERT(node.getProperty("string vector", pr) ==
            PROPERTY_STATIC);
        std::vector<std::string> curStrVec =
            boost::get< std::vector<std::string> >(pr.first);
        TESTING_ASSERT(curStrVec.size() == strVec.size());
        for (size_t j = 0; j < curStrVec.size(); ++j)
             TESTING_ASSERT(curStrVec[j] == strVec[j]);

        TESTING_ASSERT(node.getProperty("string", pr) == PROPERTY_STATIC);

        // a getting property that doesnt exist is not supposed to change
        // any part of the property, so the checks after this will test it
        // as well as the string values
        TESTING_ASSERT(node.getProperty("bad fake prop", pr) == PROPERTY_NONE);

        TESTING_ASSERT(boost::get<std::string>(pr.first) == str);
        TESTING_ASSERT(pr.second.scope == SCOPE_POINT);
        TESTING_ASSERT(pr.second.inputType == ARBATTR_VECTOR3);
        TESTING_ASSERT(pr.second.outputType == ARBATTR_COLOR4);
        TESTING_ASSERT(pr.second.index.size() == 2);
        TESTING_ASSERT(pr.second.index[0] == 45);
        TESTING_ASSERT(pr.second.index[1] == 33);

        int numProps = 0;
        PropertyMap::const_iterator cur =
            node.beginNonSampledProperties();

        PropertyMap::const_iterator end =
            node.endNonSampledProperties();

        for (;cur != end; cur++)
        {
            numProps ++;
        }

        TESTING_ASSERT(numProps == 6);
        TESTING_ASSERT(!node.propertiesWritten());

        p.second.index.clear();
        p.second.scope = SCOPE_PRIMITIVE;
        p.second.inputType = ARBATTR_NONE;
        p.second.outputType = ARBATTR_NONE;

        p.first = i;
        node.setSampledProperty("sampInt", p);
        p.first = dv;
        node.setSampledProperty("sampDoubleVector", p);

        // make sure duplicates throw properly
        TESTING_ASSERT_THROW(node.setSampledProperty("string", p),
            std::logic_error);
        TESTING_ASSERT_THROW(node.setNonSampledProperty("sampInt", p),
            std::logic_error);

        numProps = 0;
        cur = node.beginSampledProperties();
        end = node.endSampledProperties();
        for (;cur != end; cur++)
        {
            numProps ++;
        }

        TESTING_ASSERT(numProps == 2);

        node.writeProperties(0.0);
        TESTING_ASSERT(node.propertiesWritten());

        PropertyPair ps;
        ps.second.scope = SCOPE_POINT;
        ps.second.inputType = ARBATTR_VECTOR3;
        ps.second.outputType = ARBATTR_COLOR4;
        ps.second.index.push_back(1);
        ps.second.index.push_back(2);
        ps.second.index.push_back(3);

        p.first = si;
        node.updateSample("sampInt", p);
        TESTING_ASSERT(node.getProperty("sampInt", pr) == PROPERTY_ANIMATED);

        ps.first = sdv;
        node.updateSample("sampDoubleVector", ps);
        node.writeProperties(1.0);

        si = 40;
        p.first = si;
        node.updateSample("sampInt", p);
        sdv[0] = -1.2;
        sdv[1] = -2.3;
        sdv[2] = -3.4;

        ps.second.scope = SCOPE_FACE;
        ps.second.inputType = ARBATTR_FLOAT;
        ps.second.outputType = ARBATTR_DOUBLE;
        ps.second.index.clear();
        ps.second.index.push_back(3);
        ps.second.index.push_back(4);
        ps.first = sdv;

        node.updateSample("sampDoubleVector", ps);
        TESTING_ASSERT(node.getProperty("sampDoubleVector", pr)
            == PROPERTY_ANIMATED);

        node.writeProperties(2.0);

        // new property fail test
        TESTING_ASSERT_THROW(node.setNonSampledProperty("test", ps),
            std::logic_error);

        // wrong type fail test
        TESTING_ASSERT_THROW(node.updateSample("sampInt", ps),
            std::logic_error);

        // updating non sampled fail test
        TESTING_ASSERT_THROW(node.updateSample("taco", ps),
            std::logic_error);

    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);
    H5Fclose(fid);

    {
        TransformReader rNode("/tmp/testNode.hdf");
        fid = H5Iget_file_id(rNode.getHid());

        // before reading all of the properties, test reading a couple of
        // individual properties
        PropertyPair pr;
        TESTING_ASSERT(rNode.readProperty("integer", 1.0, pr)
            == PROPERTY_STATIC);
        TESTING_ASSERT(boost::get<int>(pr.first) == i);

        TESTING_ASSERT(rNode.readProperty("sampInt", 1.0, pr)
            == PROPERTY_ANIMATED);
        TESTING_ASSERT(boost::get<int>(pr.first) == 22);

        TESTING_ASSERT(rNode.readProperty("fake and bad", 1.0, pr)
            == PROPERTY_NONE);

        rNode.readProperties(1.0);
        int numProps = 0;
        PropertyMap::const_iterator cur =
            rNode.beginNonSampledProperties();

        PropertyMap::const_iterator end =
            rNode.endNonSampledProperties();

        for (;cur != end; cur++)
        {
            numProps ++;
        }
        TESTING_ASSERT(numProps == 6);

        numProps = 0;
        cur = rNode.beginSampledProperties();
        end = rNode.endSampledProperties();
        for (;cur != end; cur++)
        {
            numProps ++;
        }
        TESTING_ASSERT(numProps == 2);

        std::set<float> frames;
        rNode.getPropertyFrames(frames);
        TESTING_ASSERT(frames.size() == 3);
        TESTING_ASSERT(rNode.hasPropertyFrames());

        TESTING_ASSERT(rNode.getProperty("integer", pr) == PROPERTY_STATIC);
        TESTING_ASSERT(boost::get<int>(pr.first) == i);

        TESTING_ASSERT(rNode.getProperty("double", pr) == PROPERTY_STATIC);
        TESTING_ASSERT(boost::get<double>(pr.first) == d);

        TESTING_ASSERT(rNode.getProperty("int vector", pr) == PROPERTY_STATIC);
        TESTING_ASSERT(boost::get<std::vector<int> >(pr.first) == iv);

        TESTING_ASSERT(rNode.getProperty("double vector", pr) ==
            PROPERTY_STATIC);
        TESTING_ASSERT(boost::get<std::vector<double> >(pr.first) == dv);

        TESTING_ASSERT(rNode.getProperty("string vector", pr) == PROPERTY_STATIC);
        std::vector<std::string> curStrVec =
            boost::get< std::vector<std::string> >(pr.first);
        TESTING_ASSERT(curStrVec.size() == strVec.size());
        for (size_t j = 0; j < curStrVec.size(); ++j)
        {
            TESTING_ASSERT(curStrVec[j] == strVec[j]);
        }

        TESTING_ASSERT(rNode.getProperty("string", pr) == PROPERTY_STATIC);
        TESTING_ASSERT(boost::get<std::string>(pr.first) == str);

        // getting a nonexistant prop is not supposed to change what is stored
        // in the prop
        TESTING_ASSERT(rNode.getProperty("fake bad", pr) == PROPERTY_NONE);

        TESTING_ASSERT(boost::get<std::string>(pr.first) == str);
        TESTING_ASSERT(pr.second.scope == SCOPE_POINT);
        TESTING_ASSERT(pr.second.inputType == ARBATTR_VECTOR3);
        TESTING_ASSERT(pr.second.outputType == ARBATTR_COLOR4);
        TESTING_ASSERT(pr.second.index.size() == 2);
        TESTING_ASSERT(pr.second.index[0] == 45);
        TESTING_ASSERT(pr.second.index[1] == 33);

        TESTING_ASSERT(rNode.getProperty("sampInt", pr) == PROPERTY_ANIMATED);
        TESTING_ASSERT(boost::get<int>(pr.first) == 22);

        sdv[0] = 2.2;
        sdv[1] = 4.4;
        sdv[2] = 8.8;
        TESTING_ASSERT(rNode.getProperty("sampDoubleVector", pr)
            == PROPERTY_ANIMATED);

        TESTING_ASSERT(boost::get<std::vector<double> >(pr.first) == sdv);
        TESTING_ASSERT(pr.second.scope == SCOPE_POINT);
        TESTING_ASSERT(pr.second.inputType == ARBATTR_VECTOR3);
        TESTING_ASSERT(pr.second.outputType == ARBATTR_COLOR4);
        TESTING_ASSERT(pr.second.index.size() == 3);
        TESTING_ASSERT(pr.second.index[0] == 1);
        TESTING_ASSERT(pr.second.index[1] == 2);
        TESTING_ASSERT(pr.second.index[2] == 3);

        rNode.readProperties(2.0);
        TESTING_ASSERT(rNode.getProperty("sampInt", pr) == PROPERTY_ANIMATED);
        TESTING_ASSERT(boost::get<int>(pr.first) == 40);

        sdv[0] = -1.2;
        sdv[1] = -2.3;
        sdv[2] = -3.4;
        TESTING_ASSERT(rNode.getProperty("sampDoubleVector", pr)
            == PROPERTY_ANIMATED);
        TESTING_ASSERT(boost::get<std::vector<double> >(pr.first) == sdv);
        TESTING_ASSERT(pr.second.scope == SCOPE_FACE);
        TESTING_ASSERT(pr.second.inputType == ARBATTR_FLOAT);
        TESTING_ASSERT(pr.second.outputType == ARBATTR_DOUBLE);
        TESTING_ASSERT(pr.second.index.size() == 2);
        TESTING_ASSERT(pr.second.index[0] == 3);
        TESTING_ASSERT(pr.second.index[1] == 4);
    }

    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);

    H5Fclose(fid);
    {
        TransformWriter node("/tmp/testNode.hdf");
        TESTING_ASSERT(node.getHid() > -1);
    }
}

void HDFNodeTest::testSmallVec(void)
{
    hid_t fid = -1;
    {
        TransformWriter node("/tmp/testSmallNode.hdf");
        fid = H5Iget_file_id(node.getHid());

        PropertyPair p;

        std::vector<double> dVec;
        p.first = dVec;
        node.setNonSampledProperty("emptyDouble", p);

        dVec.push_back(35.0);
        p.first = dVec;
        node.setNonSampledProperty("oneDouble", p);

        std::vector<float> fVec;
        p.first = fVec;
        node.setNonSampledProperty("emptyFloat", p);

        fVec.push_back(3.0);
        p.first = fVec;
        node.setNonSampledProperty("oneFloat", p);

        std::vector< int16_t > sVec;
        p.first = sVec;
        node.setNonSampledProperty("emptyShort", p);

        sVec.push_back(29);
        p.first = sVec;
        node.setNonSampledProperty("oneShort", p);

        std::vector< int32_t > iVec;
        p.first = iVec;
        node.setNonSampledProperty("emptyInt", p);

        iVec.push_back(745);
        p.first = iVec;
        node.setNonSampledProperty("oneInt", p);

        std::vector< int64_t > lVec;
        p.first = lVec;
        node.setNonSampledProperty("emptyLongLong", p);

        lVec.push_back(1632);
        p.first = lVec;
        node.setNonSampledProperty("oneLongLong", p);

        std::string str;
        p.first = str;
        node.setNonSampledProperty("emptyString", p);

        str = "l";
        p.first = str;
        node.setNonSampledProperty("oneString", p);

        std::vector<std::string> strVec;
        p.first = strVec;
        node.setNonSampledProperty("emptyStringVec", p);

        strVec.push_back("");
        strVec.push_back("a");
        strVec.push_back("");
        p.first = strVec;
        node.setNonSampledProperty("nearEmptyStringVec", p);

        node.writeProperties(0.0);
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);

    {
        TransformReader rNode("/tmp/testSmallNode.hdf");
        fid = H5Iget_file_id(rNode.getHid());

        rNode.readProperties(0.0);

        PropertyPair pr;
        rNode.getProperty("emptyDouble", pr);
        TESTING_ASSERT(boost::get< std::vector<double> >
            (pr.first).size() == 0);

        rNode.getProperty("emptyFloat", pr);
        TESTING_ASSERT(boost::get< std::vector<float> >
            (pr.first).size() == 0);

        rNode.getProperty("emptyShort", pr);
        TESTING_ASSERT(boost::get< std::vector< int16_t > >
            (pr.first).size() == 0);

        rNode.getProperty("emptyInt", pr);
        TESTING_ASSERT(boost::get< std::vector< int32_t > >
            (pr.first).size() == 0);

        rNode.getProperty("emptyLongLong", pr);
        TESTING_ASSERT(boost::get< std::vector< int64_t > >
            (pr.first).size() == 0);

        rNode.getProperty("emptyString", pr);
        TESTING_ASSERT(boost::get< std::string >
            (pr.first).size() == 0);

        rNode.getProperty("emptyStringVec", pr);
        TESTING_ASSERT(boost::get< std::vector<std::string> >
            (pr.first).size() == 0);

        rNode.getProperty("oneDouble", pr);
        std::vector <double> dVec = boost::get< std::vector<double> >
            (pr.first);
        TESTING_ASSERT(dVec.size() == 1);
        TESTING_ASSERT(dVec[0] == 35.0);

        rNode.getProperty("oneFloat", pr);
        std::vector <float> fVec = boost::get< std::vector<float> >
            (pr.first);
        TESTING_ASSERT(fVec.size() == 1);
        TESTING_ASSERT(fVec[0] == 3.0);

        rNode.getProperty("oneShort", pr);
        std::vector < int16_t > sVec = boost::get< std::vector< int16_t > >
            (pr.first);
        TESTING_ASSERT(sVec.size() == 1);
        TESTING_ASSERT(sVec[0] == 29);

        rNode.getProperty("oneInt", pr);
        std::vector < int32_t > iVec = boost::get< std::vector< int32_t > >
            (pr.first);
        TESTING_ASSERT(iVec.size() == 1);
        TESTING_ASSERT(iVec[0] == 745);

        rNode.getProperty("oneLongLong", pr);
        std::vector < int64_t > lVec = boost::get< std::vector< int64_t > >
            (pr.first);
        TESTING_ASSERT(lVec.size() == 1);
        TESTING_ASSERT(lVec[0] == 1632);

        rNode.getProperty("oneString", pr);
        std::string str = boost::get< std::string >
            (pr.first);
        TESTING_ASSERT(str.size() == 1);
        TESTING_ASSERT(str == "l");

        rNode.getProperty("nearEmptyStringVec", pr);
        std::vector<std::string> strVec = boost::get<
            std::vector<std::string> >(pr.first);

        TESTING_ASSERT(strVec.size() == 3);
        TESTING_ASSERT(strVec[0] == "");
        TESTING_ASSERT(strVec[1] == "a");
        TESTING_ASSERT(strVec[2] == "");
    }
    TESTING_ASSERT(H5Fget_obj_count(fid, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
        H5F_OBJ_DATATYPE | H5F_OBJ_ATTR) == 0);

}
