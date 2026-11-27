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

#include <cstdio>
#include <fstream>
#include <iterator>
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

// catch and ignore exceptions
void walkPropsThrow(ABCA::CompoundPropertyReaderPtr parent)
{
    for (std::size_t i = 0; i < parent->getNumProperties(); ++i)
    {
        ABCA::CompoundPropertyReaderPtr childCompound;
        try
        {
            childCompound = parent->getCompoundProperty(i);
            if (childCompound)
            {
                ABCA::MetaData md = childCompound->getMetaData();
                md.serialize();
            }
        }
        catch(const std::exception& e)
        {
        }

        if (childCompound)
        {
            walkPropsThrow(childCompound);
        }

        ABCA::ScalarPropertyReaderPtr childScalar;
        try
        {
            childScalar = parent->getScalarProperty(i);
            if (childScalar)
            {
                ABCA::MetaData md = childScalar->getMetaData();
                md.serialize();
            }
        }
        catch(const std::exception& e)
        {
        }

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
                    try
                    {
                        childScalar->getSample(i, &sampStrVec.front());
                    }
                    catch(const std::exception& e)
                    {
                    }

                }
                else if (childScalar->getDataType().getPod() ==
                         Alembic::AbcCoreAbstract::kWstringPOD)
                {
                    try
                    {
                        childScalar->getSample(i, &sampWStrVec.front());
                    }
                    catch(const std::exception& e)
                    {
                    }
                }
                else
                {
                    try
                    {
                        childScalar->getSample(i, samp);
                    }
                    catch(const std::exception& e)
                    {
                    }
                }
            }
        }

        ABCA::ArrayPropertyReaderPtr childArray;
        try
        {
            childArray = parent->getArrayProperty(i);
            if (childArray)
            {
                ABCA::MetaData md = childArray->getMetaData();
                md.serialize();
            }
        }
        catch(const std::exception& e)
        {
        }

        if (childArray)
        {
            ABCA::ArraySamplePtr samp;
            for (std::size_t i = 0; i <  childArray->getNumSamples(); ++i)
            {
                try
                {
                    childArray->getSample(i, samp);
                }
                catch(const std::exception& e)
                {
                }
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

// this variant we catch the throws to make sure we check other issues
void walkObjThrow(ABCA::ObjectReaderPtr parent)
{
    ABCA::CompoundPropertyReaderPtr props;
    try
    {
        props = parent->getProperties();
        ABCA::MetaData md = props->getMetaData();
        md.serialize();

    }
    catch(const std::exception& e)
    {
    }

    if (props)
    {
        walkPropsThrow(props);
    }

    for(std::size_t i = 0; i < parent->getNumChildren(); ++i)
    {
        ABCA::ObjectReaderPtr child;
        try
        {
            child = parent->getChild(i);
        }
        catch(const std::exception& e)
        {
            continue;
        }

        if (child)
        {
            walkObjThrow(child);
        }
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

void walkArchiveNoThrow(const char * iName, bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);

    ABCA::ArchiveReaderPtr ar;
    try
    {
        ar = r(iName);
    }
    catch(const std::exception& e)
    {
    }

    if (ar)
    {
        walkObjThrow(ar->getTop());
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
        std::string msg = "Read invalid: Object Headers name and MetaData index.";
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
        std::string msg = "Read invalid: Property Headers name.";
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
        std::string msg = "Read invalid: Property Headers name.";
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
        std::string msg = "Read invalid: Property Headers name.";
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
        TESTING_ASSERT(msg == "Read invalid: Object Headers name and MetaData index." ||
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
        std::string msg = "Read invalid: Property Headers name.";
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
        std::string msg = "Read invalid: Property Headers name.";
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
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue26125.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Object Headers name and MetaData index.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testFuzzer33685(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);

    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue33685.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Object Headers name and MetaData index.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

// This one ended up getting flagged for using too much memory
// which is where we noticed that empty property and object names
// were getting through AND a way too large Indexed meta data buffer
void testFuzzer49213(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);

    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue49213.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: Indexed MetaData buffer unexpectedly big.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testFuzzer52703(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_issue52703.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: TimeSamples sample times.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void testFuzzer52939(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);

    ABCA::ArchiveReaderPtr ar = r("fuzzer_issue52939.abc");
    ABCA::MetaData md = ar->getMetaData();

    // bad metadata ends up getting ignored
    TESTING_ASSERT(md.begin() == md.end());
    TESTING_ASSERT(md.size() == 0);
}

void testFuzzere53205(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);

    ABCA::ArchiveReaderPtr ar = r("fuzzer_issue53205.abc");
    TESTING_ASSERT(ar->getNumTimeSamplings() == 0);
}

void testFuzzere53406(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);

    ABCA::ArchiveReaderPtr ar = r("fuzzer_issue53406.abc");
    ABCA::MetaData md = ar->getMetaData();
    try
    {
        md.serialize();
    }
    catch(const std::exception& e)
    {
        std::string msg = "TokenMap::get: Token-Value pair  contains separator characters: ; or = for";
        std::string what = e.what();
        TESTING_ASSERT(what.substr(0, msg.size()) == msg);
    }
}

void testBadDimensions(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("fuzzer_ReadDimensions.abc");
    walkObj(ar->getTop());
}

void testFuzzerTaoTaoGu3513(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("fuzzer_Taotao_Gu_3513.abc");

    ABCA::MetaData md = ar->getMetaData();
    try
    {
        md.serialize();
    }
    catch(const std::exception& e)
    {
        std::string msg = "TokenMap::get: Token-Value pair  contains separator characters: ; or = for";
        std::string what = e.what();
        TESTING_ASSERT(what.substr(0, msg.size()) == msg);
    }

    walkJustObj(ar->getTop());
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

    TESTING_ASSERT(0);
}

void testFuzzerTaoTaoGu3699(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);

    try
    {
        ABCA::ArchiveReaderPtr ar = r("fuzzer_Taotao_Gu_3699.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Read invalid: TimeSamples sample times.";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

static Alembic::Util::uint64_t readU64LE(const std::vector<char>& buf,
                                         std::size_t off)
{
    TESTING_ASSERT(off + 8 <= buf.size());
    Alembic::Util::uint64_t val = 0;
    for (std::size_t i = 0; i < 8; ++i)
    {
        val |= static_cast<Alembic::Util::uint64_t>(
            static_cast<unsigned char>(buf[off + i])) << (i * 8);
    }
    return val;
}

static void writeU64LE(std::vector<char>& buf,
                       std::size_t off,
                       Alembic::Util::uint64_t val)
{
    TESTING_ASSERT(off + 8 <= buf.size());
    for (std::size_t i = 0; i < 8; ++i)
    {
        buf[off + i] = static_cast<char>((val >> (i * 8)) & 0xff);
    }
}

static std::vector<char> buildReadDimensionsTestArchive()
{
    const std::string fileName = "readDimensionsNullTestBase.abc";
    {
        Alembic::AbcCoreOgawa::WriteArchive w;
        ABCA::ArchiveWriterPtr ar = w(fileName, ABCA::MetaData());
        ABCA::CompoundPropertyWriterPtr props = ar->getTop()->getProperties();

        ABCA::DataType i32dt(Alembic::Util::kInt32POD, 1);

        // Property 0: 1D array (data >= 16 bytes, implicit empty dims).
        ABCA::ArrayPropertyWriterPtr p1d =
            props->createArrayProperty("arr1d", ABCA::MetaData(), i32dt, 0);
        Alembic::Util::int32_t vals1d[4] = {10, 20, 30, 40};
        p1d->setSample(ABCA::ArraySample(
            vals1d, i32dt, Alembic::Util::Dimensions(4)));

        // Property 1: 2D array (data >= 16 bytes, explicit 2x3 dims).
        ABCA::ArrayPropertyWriterPtr p2d =
            props->createArrayProperty("arr2d", ABCA::MetaData(), i32dt, 0);
        Alembic::Util::int32_t vals2d[6] = {1, 2, 3, 4, 5, 6};
        Alembic::Util::Dimensions dims2d;
        dims2d.setRank(2);
        dims2d[0] = 2;
        dims2d[1] = 3;
        p2d->setSample(ABCA::ArraySample(vals2d, i32dt, dims2d));

        // Property 2: empty array (data < 16 bytes, implicit empty dims).
        ABCA::ArrayPropertyWriterPtr pEmpty =
            props->createArrayProperty("arrEmpty", ABCA::MetaData(), i32dt, 0);
        pEmpty->setSample(ABCA::ArraySample(
            NULL, i32dt, Alembic::Util::Dimensions(0)));
    }

    std::ifstream in(fileName.c_str(), std::ios::binary);
    TESTING_ASSERT(in.good());
    std::vector<char> bytes(
        (std::istreambuf_iterator<char>(in)),
        std::istreambuf_iterator<char>());
    in.close();
    std::remove(fileName.c_str());
    TESTING_ASSERT(bytes.size() > 16);
    return bytes;
}

static Alembic::Util::uint64_t findTopArrayPropGroupPos(
    const std::vector<char>& bytes, std::size_t propIndex)
{
    const Alembic::Util::uint64_t kDataBit = 0x8000000000000000ULL;

    // Ogawa header bytes [8..15] store the root group offset.
    Alembic::Util::uint64_t rootPos = readU64LE(bytes, 8);
    TESTING_ASSERT(rootPos != 0 && (rootPos & kDataBit) == 0);
    TESTING_ASSERT(readU64LE(bytes, rootPos) > 2);

    // Root child 2 is the top object group.
    Alembic::Util::uint64_t topObjPos = readU64LE(bytes, rootPos + 8 + 2 * 8);
    TESTING_ASSERT(topObjPos != 0 && (topObjPos & kDataBit) == 0);
    TESTING_ASSERT(readU64LE(bytes, topObjPos) > 0);

    // Top object child 0 is the top compound property group.
    Alembic::Util::uint64_t topPropsPos = readU64LE(bytes, topObjPos + 8);
    TESTING_ASSERT(topPropsPos != 0 && (topPropsPos & kDataBit) == 0);
    TESTING_ASSERT(readU64LE(bytes, topPropsPos) > propIndex);

    // Child propIndex is the target array property group.
    Alembic::Util::uint64_t propPos =
        readU64LE(bytes, topPropsPos + 8 + propIndex * 8);
    TESTING_ASSERT(propPos != 0 && (propPos & kDataBit) == 0);
    TESTING_ASSERT(readU64LE(bytes, propPos) == 2);
    return propPos;
}

static void writeArchiveBytes(const std::string& fileName,
                              const std::vector<char>& bytes)
{
    std::ofstream out(fileName.c_str(), std::ios::binary | std::ios::trunc);
    TESTING_ASSERT(out.good());
    out.write(&bytes[0], static_cast<std::streamsize>(bytes.size()));
    out.close();
    TESTING_ASSERT(out.good());
}

static void expectNullIDataPtrThrow(ABCA::ArrayPropertyReaderPtr prop)
{
    const std::string expected = "ReadDimensions invalid: Null IDataPtr.";
    int sampleThrew = 0;
    try
    {
        ABCA::ArraySamplePtr samp;
        prop->getSample(0, samp);
    }
    catch (const std::exception& e)
    {
        TESTING_ASSERT(expected == e.what());
        sampleThrew = 1;
    }
    TESTING_ASSERT(sampleThrew != 0);

    int dimsThrew = 0;
    try
    {
        Alembic::Util::Dimensions dims;
        prop->getDimensions(0, dims);
    }
    catch (const std::exception& e)
    {
        TESTING_ASSERT(expected == e.what());
        dimsThrew = 1;
    }
    TESTING_ASSERT(dimsThrew != 0);
}

void testReadDimensionsValidControl(bool iUseMMap)
{
    const std::string fileName = "readDimensionsValidControl.abc";
    std::vector<char> bytes = buildReadDimensionsTestArchive();
    writeArchiveBytes(fileName, bytes);

    {
        Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
        ABCA::ArchiveReaderPtr ar = r(fileName);
        ABCA::CompoundPropertyReaderPtr props = ar->getTop()->getProperties();

        ABCA::ArrayPropertyReaderPtr arr1d = props->getArrayProperty("arr1d");
        TESTING_ASSERT(arr1d && arr1d->getNumSamples() != 0);
        Alembic::Util::Dimensions d1;
        arr1d->getDimensions(0, d1);
        TESTING_ASSERT(d1.rank() == 1 && d1[0] == 4);
        ABCA::ArraySamplePtr s1;
        arr1d->getSample(0, s1);
        TESTING_ASSERT(s1 && s1->size() == 4);
        const Alembic::Util::int32_t* p1 =
            static_cast<const Alembic::Util::int32_t*>(s1->getData());
        TESTING_ASSERT(p1[0] == 10 && p1[1] == 20 && p1[2] == 30 && p1[3] == 40);

        ABCA::ArrayPropertyReaderPtr arr2d = props->getArrayProperty("arr2d");
        TESTING_ASSERT(arr2d && arr2d->getNumSamples() != 0);
        Alembic::Util::Dimensions d2;
        arr2d->getDimensions(0, d2);
        TESTING_ASSERT(d2.rank() == 2 && d2[0] == 2 && d2[1] == 3);
        ABCA::ArraySamplePtr s2;
        arr2d->getSample(0, s2);
        TESTING_ASSERT(s2 && s2->size() == 6);
        const Alembic::Util::int32_t* p2 =
            static_cast<const Alembic::Util::int32_t*>(s2->getData());
        TESTING_ASSERT(p2[0] == 1 && p2[5] == 6);

        ABCA::ArrayPropertyReaderPtr arrEmpty =
            props->getArrayProperty("arrEmpty");
        TESTING_ASSERT(arrEmpty && arrEmpty->getNumSamples() != 0);
        Alembic::Util::Dimensions d0;
        arrEmpty->getDimensions(0, d0);
        TESTING_ASSERT(d0.numPoints() == 0);
        ABCA::ArraySamplePtr s0;
        arrEmpty->getSample(0, s0);
        TESTING_ASSERT(s0 && s0->size() == 0);
    }

    std::remove(fileName.c_str());
}

void testReadDimensionsNullDataRejected(bool iUseMMap)
{
    const std::string fileName = "readDimensionsNullData.abc";
    const Alembic::Util::uint64_t kDataBit = 0x8000000000000000ULL;
    std::vector<char> bytes = buildReadDimensionsTestArchive();

    // Clear the Ogawa data-marker bit on arr1d child 0 (sample data), so
    // IGroup::getData(0) returns a null IDataPtr while child 1 (dims) stays valid.
    Alembic::Util::uint64_t pos0 = findTopArrayPropGroupPos(bytes, 0);
    Alembic::Util::uint64_t dataWord = readU64LE(bytes, pos0 + 8);
    TESTING_ASSERT((dataWord & kDataBit) != 0);
    writeU64LE(bytes, pos0 + 8, dataWord & ~kDataBit);
    writeArchiveBytes(fileName, bytes);

    {
        Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
        ABCA::ArchiveReaderPtr ar = r(fileName);
        ABCA::CompoundPropertyReaderPtr props = ar->getTop()->getProperties();

        expectNullIDataPtrThrow(props->getArrayProperty("arr1d"));

        // Uncorrupted sibling property remains readable.
        ABCA::ArrayPropertyReaderPtr arr2d = props->getArrayProperty("arr2d");
        TESTING_ASSERT(arr2d && arr2d->getNumSamples() != 0);
        Alembic::Util::Dimensions d2;
        arr2d->getDimensions(0, d2);
        TESTING_ASSERT(d2.rank() == 2 && d2[0] == 2 && d2[1] == 3);
    }

    std::remove(fileName.c_str());
}

void testReadDimensionsNullDimsRejected(bool iUseMMap)
{
    const std::string fileName = "readDimensionsNullDims.abc";
    const Alembic::Util::uint64_t kDataBit = 0x8000000000000000ULL;
    std::vector<char> bytes = buildReadDimensionsTestArchive();

    // Clear the Ogawa data-marker bit on child 1 (dimensions) for 1D (data>=16),
    // 2D (data>=16), and empty (data<16) array properties.
    for (std::size_t idx = 0; idx < 3; ++idx)
    {
        Alembic::Util::uint64_t pos = findTopArrayPropGroupPos(bytes, idx);
        Alembic::Util::uint64_t dimsWord = readU64LE(bytes, pos + 16);
        TESTING_ASSERT((dimsWord & kDataBit) != 0);
        writeU64LE(bytes, pos + 16, dimsWord & ~kDataBit);
    }
    writeArchiveBytes(fileName, bytes);

    {
        Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
        ABCA::ArchiveReaderPtr ar = r(fileName);
        ABCA::CompoundPropertyReaderPtr props = ar->getTop()->getProperties();

        expectNullIDataPtrThrow(props->getArrayProperty("arr1d"));
        expectNullIDataPtrThrow(props->getArrayProperty("arr2d"));
        expectNullIDataPtrThrow(props->getArrayProperty("arrEmpty"));
    }

    std::remove(fileName.c_str());
}

void testReadDimensionsTruncatedGroupRejected(bool iUseMMap)
{
    const std::string fileName = "readDimensionsTruncGroup.abc";
    std::vector<char> bytes = buildReadDimensionsTestArchive();

    // Truncate arr1d group child count from 2 to 1: iData is valid, iDims is null.
    Alembic::Util::uint64_t pos0 = findTopArrayPropGroupPos(bytes, 0);
    writeU64LE(bytes, pos0, 1);

    // Inflate arr2d group child count beyond stream size / 8 so IGroup resets
    // numChildren to 0: both iData and iDims are null.
    Alembic::Util::uint64_t pos1 = findTopArrayPropGroupPos(bytes, 1);
    writeU64LE(bytes, pos1, static_cast<Alembic::Util::uint64_t>(bytes.size()));

    writeArchiveBytes(fileName, bytes);

    {
        Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
        ABCA::ArchiveReaderPtr ar = r(fileName);
        ABCA::CompoundPropertyReaderPtr props = ar->getTop()->getProperties();

        expectNullIDataPtrThrow(props->getArrayProperty("arr1d"));
        expectNullIDataPtrThrow(props->getArrayProperty("arr2d"));
    }

    std::remove(fileName.c_str());
}

void test_badver_pr463()
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, false);
    try
    {
        ABCA::ArchiveReaderPtr ar = r("badver.abc");
    }
    catch(const std::exception& e)
    {
        std::string msg = "Could not open as Ogawa file: badver.abc";
        TESTING_ASSERT(msg == e.what());
        return;
    }

    TESTING_ASSERT(0);
}

void test_walkAllNoThrow(bool iUseMMap)
{
    walkArchiveNoThrow("bad_strings_ogawa.abc", iUseMMap);
    walkArchiveNoThrow("issue254.abc", iUseMMap);
    walkArchiveNoThrow("issue255.abc", iUseMMap);
    walkArchiveNoThrow("issue256.abc", iUseMMap);
    walkArchiveNoThrow("issue257.abc", iUseMMap);
    walkArchiveNoThrow("issue258.abc", iUseMMap);
    walkArchiveNoThrow("issue269.abc", iUseMMap);
    walkArchiveNoThrow("issue270.abc", iUseMMap);
    walkArchiveNoThrow("issue271.abc", iUseMMap);
    walkArchiveNoThrow("issue272.abc", iUseMMap);
    walkArchiveNoThrow("issue282.abc", iUseMMap);
    walkArchiveNoThrow("issue283.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue24846.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue24853.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue24598.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue25051.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue25081.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue25166.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue25175.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue25185.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue25192.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue25204.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue25236.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue25351.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue25502.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue25695.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue26125.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue33685.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue49213.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue52703.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue52939.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue53205.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_issue53406.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_ReadDimensions.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_Taotao_Gu_3513.abc", iUseMMap);
    walkArchiveNoThrow("fuzzer_Taotao_Gu_3699.abc", iUseMMap);
    walkArchiveNoThrow("zdi-23700-poc0.abc", iUseMMap);
    walkArchiveNoThrow("zdi-23700-poc1.abc", iUseMMap);
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

    testFuzzer49213(true);
    testFuzzer49213(false);

    testFuzzer52703(true);
    testFuzzer52703(false);

    testFuzzer52939(true);
    testFuzzer52939(false);

    testFuzzere53205(true);
    testFuzzere53205(false);

    testFuzzere53406(true);
    testFuzzere53406(false);

    testFuzzerTaoTaoGu3513(true);
    testFuzzerTaoTaoGu3513(false);

    testFuzzerTaoTaoGu3699(true);
    testFuzzerTaoTaoGu3699(false);

    testBadDimensions(true);
    testBadDimensions(false);

    test_walkAllNoThrow(true);
    test_walkAllNoThrow(false);

    test_badver_pr463();

    testReadDimensionsValidControl(true);
    testReadDimensionsValidControl(false);

    testReadDimensionsNullDataRejected(true);
    testReadDimensionsNullDataRejected(false);

    testReadDimensionsNullDimsRejected(true);
    testReadDimensionsNullDimsRejected(false);

    testReadDimensionsTruncatedGroupRejected(true);
    testReadDimensionsTruncatedGroupRejected(false);

    return 0;
}
