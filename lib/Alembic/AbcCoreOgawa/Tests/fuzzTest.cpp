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
    }
}

void testIssue254(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue254.abc");
    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "Ogawa IStreams::read failed.";
        TESTING_ASSERT(msg == e.what());
    }

}

void testIssue255(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue255.abc");
    walkObj(ar->getTop());

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
    }

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
    }
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
    }
}

void testIssue269(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue269.abc");
    try
    {
        walkObj(ar->getTop());
    }
    catch(const std::exception& e)
    {
        std::string msg = "ReadData invalid: Null IDataPtr.";
        TESTING_ASSERT(msg == e.what());
    }
}

void testIssue270(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue270.abc");
    walkObj(ar->getTop());
}

void testIssue271(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue271.abc");
    walkObj(ar->getTop());
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
    }
}

void testIssue282(bool iUseMMap)
{
    Alembic::AbcCoreOgawa::ReadArchive r(1, iUseMMap);
    ABCA::ArchiveReaderPtr ar = r("issue282.abc");
    walkObj(ar->getTop());
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

    return 0;
}
