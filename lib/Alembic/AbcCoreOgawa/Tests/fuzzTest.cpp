//-*****************************************************************************
//
// Copyright (c) 2020,
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

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/Util/All.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

#include <fstream>
#include <sstream>
#include <vector>

// for our scalar properties
std::vector<std::string> sampStrVec;
std::vector<std::wstring> sampWStrVec;
char samp[4096];

namespace ABCA = Alembic::AbcCoreAbstract;

void walkProps(ABCA::CompoundPropertyReaderPtr parent)
{
    for (std::size_t i = 0; i < parent->getNumProperties(); ++i)
    {
        ABCA::CompoundPropertyReaderPtr childCompound = parent->getCompoundProperty(i);
        if (childCompound)
        {
            walkProps(childCompound);
        }

        ABCA::ScalarPropertyReaderPtr childScalar = parent->getScalarProperty(i);
        if (childScalar)
        {
            if (childScalar->getDataType().getPod() ==
                Alembic::AbcCoreAbstract::kStringPOD)
            {
                sampStrVec.resize(childScalar->getDataType().getExtent());
            }
            else if (childScalar->getDataType().getPod() ==
                     Alembic::AbcCoreAbstract::kWstringPOD)
            {
                sampWStrVec.resize(childScalar->getDataType().getExtent());
            }

            for (std::size_t i = 0; i <  childScalar->getNumSamples(); ++i)
            {
                if (childScalar->getDataType().getPod() ==
                    Alembic::AbcCoreAbstract::kStringPOD)
                {
                    childScalar->getSample(i, &sampStrVec.front());
                }
                else if (childScalar->getDataType().getPod() ==
                         Alembic::AbcCoreAbstract::kWstringPOD)
                {
                    childScalar->getSample(i, &sampWStrVec.front());
                }
                else
                {
                    childScalar->getSample(i, samp);
                }
            }
        }

        ABCA::ArrayPropertyReaderPtr childArray = parent->getArrayProperty(i);
        if (childArray)
        {
            ABCA::ArraySamplePtr samp;
            for (std::size_t i = 0; i <  childArray->getNumSamples(); ++i)
            {
                childArray->getSample(i, samp);
            }
        }
    }
}

void walkObj(ABCA::ObjectReaderPtr parent)
{
    walkProps(parent->getProperties());
    for(std::size_t i = 0; i < parent->getNumChildren(); ++i)
    {
        ABCA::ObjectReaderPtr child = parent->getChild(i);
        walkObj(child);
    }
}

void walkJustObj(ABCA::ObjectReaderPtr parent)
{
    for(std::size_t i = 0; i < parent->getNumChildren(); ++i)
    {
        ABCA::ObjectReaderPtr child = parent->getChild(i);
        walkJustObj(child);
    }
}

void testIssue254(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue254.abc");
    walkJustObj(ar->getTop());

    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Ogawa IData illegal size.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testIssue255(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue255.abc");
    walkJustObj(ar->getTop());

    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "ReadData invalid: Null IDataPtr.";
        TESTING_ASSERT(msg == e.what());
        return;
    }
    TESTING_ASSERT(0);
}

void testIssue256(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        r("issue256.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: TimeSamples sample times.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testIssue257(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue257.abc");
    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Object Headers name and MetaData index.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testIssue258(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        r("issue258.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Indexed MetaData string.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testIssue269(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue269.abc");
    try
    {
        walkJustObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Object Headers name size.";
        TESTING_ASSERT(msg == e.what());
    }

    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "ReadData invalid: Null IDataPtr.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testIssue270(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue270.abc");
    walkJustObj(ar->getTop());
    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Property Headers name.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

}

void testIssue271(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue271.abc");
    walkJustObj(ar->getTop());
    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Property Header MetaData index.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testIssue272(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue272.abc");
    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Object Headers MetaData index.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testIssue282(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue282.abc");
    walkJustObj(ar->getTop());
    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Ogawa IStreams::read failed.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testIssue283(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("issue283.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Invalid Alembic file.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testFuzzer24846(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue24846.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Ogawa IData illegal size.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testFuzzer24853(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue24853.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Ogawa IData illegal size.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}


void testFuzzer24598(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue24598.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Invalid object data group";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testFuzzer25051(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue25051.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Ogawa IData illegal size.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testFuzzer25081(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar;

    try
    {
        ar = r("fuzzer_issue25081.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Invalid object data group";
        TESTING_ASSERT(msg == e.what());
        return;
    }
    TESTING_ASSERT(0);

}

void testFuzzer25166(bool iUseMMap)
{
    // found issue with illegal meta data index
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue25166.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Property Header MetaData index.";
        TESTING_ASSERT(msg == e.what());
        return;
    }
    TESTING_ASSERT(0);
}

void testFuzzer25175(bool iUseMMap)
{
    // seems to be fixed by fix for 25166 (bad meta data index)
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);

    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue25175.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Property Header MetaData index.";
        TESTING_ASSERT(msg == e.what());
        return;
    }
    TESTING_ASSERT(0);
}

void testFuzzer25185(bool iUseMMap)
{
    // seems to be fixed by fix for 25166 (bad meta data index)
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue25185.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Property Header MetaData index.";
        TESTING_ASSERT(msg == e.what());
        return;
    }
    TESTING_ASSERT(0);
}

void testFuzzer25192(bool iUseMMap)
{
    // found leak in AbcOgawa::OrData with raw pointer
    // to m_children
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue25192.abc");
    }
    catch(const std::exception& e)
    {
        // In Issue 346 it was reported that on 32 bit systems
        // we got the second error message, for now guard
        // against it as well
        std::string msg = e.what();
        TESTING_ASSERT(msg == "Ogawa IStreams::read failed." ||
            msg == "Ogawa IData illegal size.");
        return;
    }
    TESTING_ASSERT(0);

}

void testFuzzer25204(bool iUseMMap)
{
    // seems to be fixed by fix for 25166 (bad meta data index)
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue25204.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Property Header MetaData index.";
        TESTING_ASSERT(msg == e.what());
        return;
    }
    TESTING_ASSERT(0);
}

void testFuzzer25236(bool iUseMMap)
{
    // seems to be fixed by fix for 25166 (bad meta data index)
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue25236.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Property Header MetaData index.";
        TESTING_ASSERT(msg == e.what());
        return;
    }
    TESTING_ASSERT(0);
}

void testFuzzer25351(bool iUseMMap)
{
    // seems to be fixed by fix for 25166 (bad meta data index)
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue25351.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Property Header MetaData index.";
        TESTING_ASSERT(msg == e.what());
        return;
    }
    TESTING_ASSERT(0);
}

void testFuzzer25502(bool iUseMMap)
{
    // leak on reading archive metadata because of throw
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue25502.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Invalid object data group";
        TESTING_ASSERT(msg == e.what());
        return;
    }
    TESTING_ASSERT(0);
}

void testFuzzer25695(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("fuzzer_issue25695.abc");
    walkJustObj(ar->getTop());
    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "ScalarPropertyReader::getSample size is not correct expected: 3 got: 9";
        TESTING_ASSERT(msg == e.what());
        return;
    }
    TESTING_ASSERT(0);
}

void testFuzzer26125(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("fuzzer_issue26125.abc");

    try
    {
        walkJustObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Invalid object data group";
        TESTING_ASSERT(msg == e.what());
    }

    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Invalid object data group";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testFuzzer33685(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("fuzzer_issue33685.abc");

    try
    {
        walkJustObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Invalid object data group";
        TESTING_ASSERT(msg == e.what());
    }

    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Invalid data in CprImpl(Object)";
        TESTING_ASSERT(msg == e.what());
        return;
    }
}

int main ( int argc, char *argv[] )
{
    testIssue254(true);
    testIssue254(false);

    testIssue255(true);
    testIssue255(false);

    testIssue256(true);
    testIssue256(false);

    testIssue257(true);
    testIssue257(false);

    testIssue258(true);
    testIssue258(false);

    testIssue269(true);
    testIssue269(false);

    testIssue270(true);
    testIssue270(false);

    testIssue271(true);
    testIssue271(false);

    testIssue272(true);
    testIssue272(false);

    testIssue282(true);
    testIssue282(false);

    testIssue283(true);
    testIssue283(false);

    testFuzzer24846(true);
    testFuzzer24846(false);

    testFuzzer24853(true);
    testFuzzer24853(false);

    testFuzzer24598(true);
    testFuzzer24598(false);

    testFuzzer25051(true);
    testFuzzer25051(false);

    testFuzzer25081(true);
    testFuzzer25081(false);

    testFuzzer25166(true);
    testFuzzer25166(false);

    testFuzzer25175(true);
    testFuzzer25175(false);

    testFuzzer25185(true);
    testFuzzer25185(false);

    testFuzzer25192(true);
    testFuzzer25192(false);

    testFuzzer25204(true);
    testFuzzer25204(false);

    testFuzzer25236(true);
    testFuzzer25236(false);

    testFuzzer25351(true);
    testFuzzer25351(false);

    testFuzzer25502(true);
    testFuzzer25502(false);

    testFuzzer25695(true);
    testFuzzer25695(false);

    testFuzzer26125(true);
    testFuzzer26125(false);

    testFuzzer33685(true);
    testFuzzer33685(false);

    return 0;
}
