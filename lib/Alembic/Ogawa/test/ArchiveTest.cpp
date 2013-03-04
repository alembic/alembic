#include "cppunit/TestAssert.h"
#include "ArchiveTest.h"

#include "Ogawa/IArchive.h"
#include "Ogawa/OArchive.h"

#include "Ogawa/IGroup.h"
#include "Ogawa/OGroup.h"

namespace {
    CPPUNIT_TEST_SUITE_REGISTRATION(ArchiveTest);
}

void ArchiveTest::test()
{
    {
        Ogawa::OArchive oa("archiveTest.ogawa");
        CPPUNIT_ASSERT(oa.isValid());
        Ogawa::IArchive ia("archiveTest.ogawa");
        CPPUNIT_ASSERT(ia.isValid());
        CPPUNIT_ASSERT(!ia.isFrozen());
        CPPUNIT_ASSERT(ia.getVersion() == 1);
        CPPUNIT_ASSERT(ia.getGroup()->getNumChildren() == 0);
    }

    Ogawa::IArchive ia("archiveTest.ogawa");
    CPPUNIT_ASSERT(ia.isValid());
    CPPUNIT_ASSERT(ia.isFrozen());
    CPPUNIT_ASSERT(ia.getVersion() == 1);
    CPPUNIT_ASSERT(ia.getGroup()->getNumChildren() == 0);

}

void ArchiveTest::stringStreamTest()
{

    std::stringstream strm;
    strm << "potato!";
    {
        Ogawa::OArchive oa(&strm);
        CPPUNIT_ASSERT(oa.isValid());
    }

    strm.seekg(7);
    std::vector< std::istream * > streams;
    streams.push_back(&strm);
    Ogawa::IArchive ia(streams);
    CPPUNIT_ASSERT(ia.isValid());
    CPPUNIT_ASSERT(ia.isFrozen());
    CPPUNIT_ASSERT(ia.getVersion() == 1);
    CPPUNIT_ASSERT(ia.getGroup()->getNumChildren() == 0);
}