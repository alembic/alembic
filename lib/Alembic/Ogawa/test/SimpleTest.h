#ifndef OGAWA_TEST_SIMPLE_TEST_H
#define OGAWA_TEST_SIMPLE_TEST_H

#include "cppunit/TestFixture.h"

#include "cppunit/extensions/HelperMacros.h"

class SimpleTest : public SPI_CppUnit_v1::TestFixture {

    public:

        CPPUNIT_TEST_SUITE(SimpleTest);

        CPPUNIT_TEST(test);

        CPPUNIT_TEST_SUITE_END();

        void test();

    private:

};

#endif
