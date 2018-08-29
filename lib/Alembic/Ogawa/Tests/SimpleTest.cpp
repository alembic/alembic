//-*****************************************************************************
//
// Copyright (c) 2013,
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

#include <Alembic/Ogawa/All.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>
#include <iostream>

void test(bool iUseMMap)
{

{
    Alembic::Ogawa::OArchive oa("simpleTest.ogawa");
    Alembic::Ogawa::OGroupPtr top = oa.getGroup();
    TESTING_ASSERT(!top->isFrozen());

    Alembic::Ogawa::OGroupPtr a = top->addGroup();
    Alembic::Ogawa::OGroupPtr b = top->addGroup();
    top->addEmptyData();

    char data[] = {0, 1, 2, 3, 4, 5, 6, 7};
    {
        Alembic::Ogawa::OGroupPtr aa = a->addGroup();
        a->addData(1, data);

        const void * datas[] = {data, &(data[5])};
        Alembic::Util::uint64_t sizes[] = {2, 3};

        aa->addData(2, sizes, datas); // 0, 1, 5, 6, 7
    }
    a->freeze();
    TESTING_ASSERT(a->isFrozen());
    TESTING_ASSERT(a->getNumChildren() == 2);

    Alembic::Ogawa::OGroupPtr ba = b->addGroup();
    Alembic::Ogawa::OGroupPtr bb = b->addGroup();
    Alembic::Ogawa::OGroupPtr bc = b->addGroup();

    ba->addData(3, data);
    ba->addData(4, data);

    bb->addData(5, data);
    bb->addData(6, data);
    bb->addData(7, data);

    Alembic::Ogawa::ODataPtr bcd = bc->addData(8, data);
    char nine = 9;
    bcd->rewrite(1, &nine, 4); // 0 1 2 3 9 5 6 7
}

    Alembic::Ogawa::IArchive ia("simpleTest.ogawa", 1, iUseMMap);
    Alembic::Ogawa::IGroupPtr top = ia.getGroup();

    TESTING_ASSERT(top->getNumChildren() == 3);
    TESTING_ASSERT(top->isChildGroup(0));
    TESTING_ASSERT(!top->isEmptyChildGroup(0));
    TESTING_ASSERT(top->isChildGroup(1));
    TESTING_ASSERT(!top->isEmptyChildGroup(1));
    TESTING_ASSERT(top->isChildData(2));
    TESTING_ASSERT(top->isEmptyChildData(2));

    Alembic::Ogawa::IGroupPtr a = top->getGroup(0, false, 0);
    TESTING_ASSERT(!top->getData(0, 0));
    TESTING_ASSERT(a->getNumChildren() == 2);
    Alembic::Ogawa::IGroupPtr aa = a->getGroup(0, false, 0);
    Alembic::Ogawa::IDataPtr ad = a->getData(1, 0);
    TESTING_ASSERT(!a->getGroup(1, false, 0));
    TESTING_ASSERT(ad->getSize() == 1);

    TESTING_ASSERT(aa->getNumChildren() == 1);
    TESTING_ASSERT(aa->getData(0, 0)->getSize() == 5);
    char data[5] = {0,0,0,0,0};
    aa->getData(0, 0)->read(5, data, 0, 0);
    TESTING_ASSERT(data[0] == 0);
    TESTING_ASSERT(data[1] == 1);
    TESTING_ASSERT(data[2] == 5);
    TESTING_ASSERT(data[3] == 6);
    TESTING_ASSERT(data[4] == 7);

    Alembic::Ogawa::IGroupPtr b = top->getGroup(1, false, 0);
    TESTING_ASSERT(b->getNumChildren() == 3);
    Alembic::Ogawa::IGroupPtr ba = b->getGroup(0, false, 0);
    Alembic::Ogawa::IGroupPtr bb = b->getGroup(1, false, 0);
    Alembic::Ogawa::IGroupPtr bc = b->getGroup(2, false, 0);
    TESTING_ASSERT(ba->getNumChildren() == 2);
    TESTING_ASSERT(bb->getNumChildren() == 3);
    TESTING_ASSERT(bc->getNumChildren() == 1);

    TESTING_ASSERT(ba->getData(0, 0)->getSize() == 3);
    TESTING_ASSERT(ba->getData(1, 0)->getSize() == 4);

    TESTING_ASSERT(bb->getData(0, 0)->getSize() == 5);
    TESTING_ASSERT(bb->getData(1, 0)->getSize() == 6);
    TESTING_ASSERT(bb->getData(2, 0)->getSize() == 7);

    Alembic::Ogawa::IDataPtr bcd = bc->getData(0, 0);
    TESTING_ASSERT(bcd->getSize() == 8);
    char data2[8] = {0,0,0,0,0,0,0,0};
    bcd->read(8, data2, 0, 0);
    TESTING_ASSERT(data2[0] == 0);
    TESTING_ASSERT(data2[1] == 1);
    TESTING_ASSERT(data2[2] == 2);
    TESTING_ASSERT(data2[3] == 3);
    TESTING_ASSERT(data2[4] == 9);
    TESTING_ASSERT(data2[5] == 5);
    TESTING_ASSERT(data2[6] == 6);
    TESTING_ASSERT(data2[7] == 7);

    // now let's check it all but with the light flag set to true
    a = top->getGroup(0, true, 0);
    TESTING_ASSERT(a->getNumChildren() == 2);
    aa = a->getGroup(0, true, 0);
    ad = a->getData(1, 0);
    TESTING_ASSERT(!a->getGroup(1, true, 0));
    TESTING_ASSERT(ad->getSize() == 1);

    TESTING_ASSERT(aa->getNumChildren() == 1);
    TESTING_ASSERT(aa->getData(0, 0)->getSize() == 5);
    char data3[5] = {0,0,0,0,0};
    aa->getData(0, 0)->read(5, data3, 0, 0);
    TESTING_ASSERT(data3[0] == 0);
    TESTING_ASSERT(data3[1] == 1);
    TESTING_ASSERT(data3[2] == 5);
    TESTING_ASSERT(data3[3] == 6);
    TESTING_ASSERT(data3[4] == 7);

    b = top->getGroup(1, true, 0);
    TESTING_ASSERT(b->getNumChildren() == 3);
    ba = b->getGroup(0, true, 0);
    bb = b->getGroup(1, true, 0);
    bc = b->getGroup(2, true, 0);
    TESTING_ASSERT(ba->getNumChildren() == 2);
    TESTING_ASSERT(bb->getNumChildren() == 3);
    TESTING_ASSERT(bc->getNumChildren() == 1);

    TESTING_ASSERT(ba->getData(0, 0)->getSize() == 3);
    TESTING_ASSERT(ba->getData(1, 0)->getSize() == 4);

    TESTING_ASSERT(bb->getData(0, 0)->getSize() == 5);
    TESTING_ASSERT(bb->getData(1, 0)->getSize() == 6);
    TESTING_ASSERT(bb->getData(2, 0)->getSize() == 7);

    bcd = bc->getData(0, 0);
    TESTING_ASSERT(bcd->getSize() == 8);
    char data4[8] = {0,0,0,0,0,0,0,0};
    bcd->read(8, data4, 0, 0);
    TESTING_ASSERT(data4[0] == 0);
    TESTING_ASSERT(data4[1] == 1);
    TESTING_ASSERT(data4[2] == 2);
    TESTING_ASSERT(data4[3] == 3);
    TESTING_ASSERT(data4[4] == 9);
    TESTING_ASSERT(data4[5] == 5);
    TESTING_ASSERT(data4[6] == 6);
    TESTING_ASSERT(data4[7] == 7);

}

int main ( int argc, char *argv[] )
{
    test(true);     // Use mmap
    test(false);    // Use streams

    return 0;
}
