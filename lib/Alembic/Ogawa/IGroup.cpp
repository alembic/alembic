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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#include <Alembic/Ogawa/IGroup.h>
#include <Alembic/Ogawa/IArchive.h>
#include <Alembic/Ogawa/IStreams.h>

namespace Alembic {
namespace Ogawa {
namespace ALEMBIC_VERSION_NS {

class IGroup::PrivateData
{
public:
    PrivateData(IStreamsPtr iStreams)
    {
        numChildren = 0;
        pos = 0;
        streams = iStreams;
    }

    ~PrivateData() {}

    IStreamsPtr streams;

    std::vector<Alembic::Util::uint64_t> childVec;

    Alembic::Util::uint64_t numChildren;
    Alembic::Util::uint64_t pos;
};

IGroup::IGroup(IStreamsPtr iStreams,
               Alembic::Util::uint64_t iPos,
               bool iLight,
               std::size_t iThreadIndex) :
    mData(new IGroup::PrivateData(iStreams))
{
    // all done, we have no children, or our streams aren't good
    if (iPos == EMPTY_GROUP || !mData->streams || !mData->streams->isValid())
    {
        return;
    }

    mData->pos = iPos;
    mData->streams->read(iThreadIndex, iPos, 8, &mData->numChildren);

    // 0 should NOT have been written, this groups should have been the
    // special EMPTY_GROUP instead

    // read all our child indices, unless we are light and have more than 8
    // children
    if (!iLight || mData->numChildren < 9)
    {
        mData->childVec.resize(mData->numChildren);
        mData->streams->read(iThreadIndex, iPos + 8, mData->numChildren * 8,
                             &(mData->childVec.front()));
    }
}

IGroup::~IGroup()
{

}

IGroupPtr IGroup::getGroup(Alembic::Util::uint64_t iIndex, bool iLight,
                           std::size_t iThreadIndex)
{
    IGroupPtr child;
    if (isLight())
    {
        if (iIndex < mData->numChildren)
        {
            Alembic::Util::uint64_t childPos = 0;
            mData->streams->read(iThreadIndex, mData->pos + 8 * iIndex + 8, 8,
                                 &childPos);

            // top bit should not be set for groups
            if ((childPos & EMPTY_DATA) == 0)
            {
                child.reset(new IGroup(mData->streams, childPos, iLight,
                                       iThreadIndex));
            }
        }
    }
    else if (isChildGroup(iIndex))
    {
        child.reset(new IGroup(mData->streams, mData->childVec[iIndex], iLight,
                               iThreadIndex));
    }
    return child;
}

IDataPtr IGroup::getData(Alembic::Util::uint64_t iIndex,
                         std::size_t iThreadIndex)
{
    IDataPtr child;
    if (isLight())
    {
        if (iIndex < mData->numChildren)
        {
            Alembic::Util::uint64_t childPos = 0;
            mData->streams->read(iThreadIndex, mData->pos + 8 * iIndex + 8, 8,
                                 &childPos);

            // top bit should be set for data
            if ((childPos & EMPTY_DATA) != 0)
            {
                child.reset(new IData(mData->streams, childPos, iThreadIndex));
            }
        }
    }
    else if (isChildData(iIndex))
    {
        child.reset(new IData(mData->streams, mData->childVec[iIndex],
                              iThreadIndex));
    }
    return child;
}

Alembic::Util::uint64_t IGroup::getNumChildren() const
{
    return mData->numChildren;
}

bool IGroup::isChildGroup(Alembic::Util::uint64_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            (mData->childVec[iIndex] & EMPTY_DATA) == 0);
}

bool IGroup::isChildData(Alembic::Util::uint64_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            (mData->childVec[iIndex] & EMPTY_DATA) != 0);
}

bool IGroup::isEmptyChildGroup(Alembic::Util::uint64_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            mData->childVec[iIndex] == EMPTY_GROUP);
}

bool IGroup::isEmptyChildData(Alembic::Util::uint64_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
        mData->childVec[iIndex] == EMPTY_DATA);
}

bool IGroup::isLight() const
{
    return mData->numChildren != 0 && mData->childVec.empty();
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Ogawa
} // End namespace Alembic
