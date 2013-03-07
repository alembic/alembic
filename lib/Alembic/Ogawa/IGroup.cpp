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
        streams = iStreams;
    }

    ~PrivateData() {}

    IStreamsPtr streams;

    std::vector<Alembic::Util::uint64_t> childVec;
};

IGroup::IGroup(IStreamsPtr iStreams,
               Alembic::Util::uint64_t iPos,
               std::size_t iThreadIndex) :
    mData(new IGroup::PrivateData(iStreams))
{
    // all done, we have no children, or our streams aren't good
    if (iPos == EMPTY_GROUP || !mData->streams || !mData->streams->isValid())
    {
        return;
    }

    Alembic::Util::uint64_t numChildren = 0;
    mData->streams->read(iThreadIndex, iPos, 8, &numChildren);

    // 0 should NOT have been written, this groups should have been the
    // special EMPTY_GROUP instead

    mData->childVec.resize(numChildren);
    mData->streams->read(iThreadIndex, iPos + 8, numChildren * 8,
                         &(mData->childVec.front()));
}

IGroup::~IGroup()
{

}

IGroupPtr IGroup::getGroup(std::size_t iIndex, std::size_t iThreadIndex)
{
    IGroupPtr child;
    if (isChildGroup(iIndex))
    {
        child.reset(new IGroup(mData->streams, mData->childVec[iIndex],
                               iThreadIndex));
    }
    return child;
}

IDataPtr IGroup::getData(std::size_t iIndex, std::size_t iThreadIndex)
{
    IDataPtr child;
    if (isChildData(iIndex))
    {
        child.reset(new IData(mData->streams, mData->childVec[iIndex],
                              iThreadIndex));
    }
    return child;
}

std::size_t IGroup::getNumChildren() const
{
    return mData->childVec.size();
}

bool IGroup::isChildGroup(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            (mData->childVec[iIndex] & EMPTY_DATA) == 0);
}

bool IGroup::isChildData(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            (mData->childVec[iIndex] & EMPTY_DATA) != 0);
}

bool IGroup::isEmptyChildGroup(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
            mData->childVec[iIndex] == EMPTY_GROUP);
}

bool IGroup::isEmptyChildData(std::size_t iIndex) const
{
    return (iIndex < mData->childVec.size() &&
        mData->childVec[iIndex] == EMPTY_DATA);
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Ogawa
} // End namespace Alembic
