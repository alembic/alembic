#include "cppunit/TestAssert.h"
#include "SimpleTest.h"

#include "Ogawa/IArchive.h"
#include "Ogawa/OArchive.h"

#include "Ogawa/IGroup.h"
#include "Ogawa/OGroup.h"

#include "Ogawa/IData.h"
#include "Ogawa/OData.h"

#include <iostream>

namespace {
    CPPUNIT_TEST_SUITE_REGISTRATION(SimpleTest);
}

void SimpleTest::test()
{

{
    Ogawa::OArchive oa("simpleTest.ogawa");
    Ogawa::OGroupPtr top = oa.getGroup();
    CPPUNIT_ASSERT(!top->isFrozen());

    Ogawa::OGroupPtr a = top->addGroup();
    Ogawa::OGroupPtr b = top->addGroup();
    top->addEmptyData();

    char data[] = {0, 1, 2, 3, 4, 5, 6, 7};
    {
        Ogawa::OGroupPtr aa = a->addGroup();
        a->addData(1, data);

        const void * datas[] = {data, &(data[5])};
        std::size_t sizes[] = {2, 3};

        aa->addData(2, sizes, datas); // 0, 1, 5, 6, 7
    }
    a->freeze();
    CPPUNIT_ASSERT(a->isFrozen());
    CPPUNIT_ASSERT(a->getNumChildren() == 2);

    Ogawa::OGroupPtr ba = b->addGroup();
    Ogawa::OGroupPtr bb = b->addGroup();
    Ogawa::OGroupPtr bc = b->addGroup();

    ba->addData(3, data);
    ba->addData(4, data);

    bb->addData(5, data);
    bb->addData(6, data);
    bb->addData(7, data);

    Ogawa::ODataPtr bcd = bc->addData(8, data);
    char nine = 9;
    bcd->rewrite(1, &nine, 4); // 0 1 2 3 9 5 6 7
}

    Ogawa::IArchive ia("simpleTest.ogawa");
    Ogawa::IGroupPtr top = ia.getGroup();

    CPPUNIT_ASSERT(top->getNumChildren() == 3);
    CPPUNIT_ASSERT(top->isChildGroup(0));
    CPPUNIT_ASSERT(!top->isEmptyChildGroup(0));
    CPPUNIT_ASSERT(top->isChildGroup(1));
    CPPUNIT_ASSERT(!top->isEmptyChildGroup(1));
    CPPUNIT_ASSERT(top->isChildData(2));
    CPPUNIT_ASSERT(top->isEmptyChildData(2));

    Ogawa::IGroupPtr a = top->getGroup(0);
    CPPUNIT_ASSERT(!top->getData(0));
    CPPUNIT_ASSERT(a->getNumChildren() == 2);
    Ogawa::IGroupPtr aa = a->getGroup(0);
    Ogawa::IDataPtr ad = a->getData(1);
    CPPUNIT_ASSERT(!a->getGroup(1));
    CPPUNIT_ASSERT(ad->getSize() == 1);

    CPPUNIT_ASSERT(aa->getNumChildren() == 1);
    CPPUNIT_ASSERT(aa->getData(0)->getSize() == 5);
    char data[5] = {0,0,0,0,0};
    aa->getData(0)->read(5, data);
    CPPUNIT_ASSERT(data[0] == 0);
    CPPUNIT_ASSERT(data[1] == 1);
    CPPUNIT_ASSERT(data[2] == 5);
    CPPUNIT_ASSERT(data[3] == 6);
    CPPUNIT_ASSERT(data[4] == 7);

    Ogawa::IGroupPtr b = top->getGroup(1);
    CPPUNIT_ASSERT(b->getNumChildren() == 3);
    Ogawa::IGroupPtr ba = b->getGroup(0);
    Ogawa::IGroupPtr bb = b->getGroup(1);
    Ogawa::IGroupPtr bc = b->getGroup(2);
    CPPUNIT_ASSERT(ba->getNumChildren() == 2);
    CPPUNIT_ASSERT(bb->getNumChildren() == 3);
    CPPUNIT_ASSERT(bc->getNumChildren() == 1);

    CPPUNIT_ASSERT(ba->getData(0)->getSize() == 3);
    CPPUNIT_ASSERT(ba->getData(1)->getSize() == 4);

    CPPUNIT_ASSERT(bb->getData(0)->getSize() == 5);
    CPPUNIT_ASSERT(bb->getData(1)->getSize() == 6);
    CPPUNIT_ASSERT(bb->getData(2)->getSize() == 7);

    Ogawa::IDataPtr bcd = bc->getData(0);
    CPPUNIT_ASSERT(bcd->getSize() == 8);
    char data2[8] = {0,0,0,0,0,0,0,0};
    bcd->read(8, data2);
    CPPUNIT_ASSERT(data2[0] == 0);
    CPPUNIT_ASSERT(data2[1] == 1);
    CPPUNIT_ASSERT(data2[2] == 2);
    CPPUNIT_ASSERT(data2[3] == 3);
    CPPUNIT_ASSERT(data2[4] == 9);
    CPPUNIT_ASSERT(data2[5] == 5);
    CPPUNIT_ASSERT(data2[6] == 6);
    CPPUNIT_ASSERT(data2[7] == 7);

}