#ifndef OGAWA_TEST_ARCHIVE_TEST_H
#define OGAWA_TEST_ARCHIVE_TEST_H

#include "cppunit/TestFixture.h"

#include "cppunit/extensions/HelperMacros.h"

class ArchiveTest : public SPI_CppUnit_v1::TestFixture {

    public:

        CPPUNIT_TEST_SUITE(ArchiveTest);

        CPPUNIT_TEST(test);
        CPPUNIT_TEST(stringStreamTest);

        CPPUNIT_TEST_SUITE_END();

        void test();
        void stringStreamTest();

    private:

};

#endif
