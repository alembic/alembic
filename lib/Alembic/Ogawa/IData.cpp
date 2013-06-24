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

#include <Alembic/Ogawa/Foundation.h>
#include <Alembic/Ogawa/IGroup.h>
#include <Alembic/Ogawa/IData.h>
#include <Alembic/Ogawa/IStreams.h>

namespace Alembic {
namespace Ogawa {
namespace ALEMBIC_VERSION_NS {

class IData::PrivateData
{
public:
    PrivateData(IStreamsPtr iStreams)
    {
        streams = iStreams;
    };

    ~PrivateData() {};

    IStreamsPtr streams;

    // set after freeze
    Alembic::Util::uint64_t pos;
    Alembic::Util::uint64_t size;
};

IData::~IData()
{

}

IData::IData(IStreamsPtr iStreams,
             Alembic::Util::uint64_t iPos,
             std::size_t iThreadId) :
    mData(new IData::PrivateData(iStreams))
{
    mData->size = 0;

    // strip off the top bit (indicates data) to get our seek position
    mData->pos = iPos & INVALID_GROUP;

    Alembic::Util::uint64_t size = 0;

    // not the empty group?  then figure out our size
    if ( mData->pos != 0 )
    {
        mData->streams->read(iThreadId, mData->pos, 8, &size);
        mData->size = size;
    }
}

void IData::read(Alembic::Util::uint64_t iSize, void * iData,
                 Alembic::Util::uint64_t iOffset, std::size_t iThreadId)
{
    // don't read anything if we will read beyond our buffer
    if (iSize == 0 || mData->size == 0 || iOffset + iSize > mData->size)
    {
        return;
    }

    // +8 is to account for the size
    mData->streams->read(iThreadId, mData->pos + iOffset + 8, iSize, iData);
}

Alembic::Util::uint64_t IData::getSize() const
{
    return mData->size;
}

Alembic::Util::uint64_t IData::getPos() const
{
    return mData->pos;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Ogawa
} // End namespace Alembic
