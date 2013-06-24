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

#include <Alembic/Ogawa/OGroup.h>
#include <Alembic/Ogawa/OData.h>
#include <Alembic/Ogawa/OStream.h>

namespace Alembic {
namespace Ogawa {
namespace ALEMBIC_VERSION_NS {

class OData::PrivateData
{
public:

    // for the empty dataset
    PrivateData()
    {
        pos = 0;
        size = 0;
    }

    PrivateData(OStreamPtr iStream,
                Alembic::Util::uint64_t iPos,
                Alembic::Util::uint64_t iSize) :
        stream(iStream), pos(iPos), size(iSize) {}

    ~PrivateData() {}

    OStreamPtr stream;

    Alembic::Util::uint64_t pos;
    Alembic::Util::uint64_t size;
};

OData::OData() : mData(new OData::PrivateData())
{
}

OData::OData(OStreamPtr iStream,
             Alembic::Util::uint64_t iPos,
             Alembic::Util::uint64_t iSize)
    : mData(new OData::PrivateData(iStream, iPos, iSize))
{
}

OData::~OData()
{
}

void OData::rewrite(Alembic::Util::uint64_t iSize, void * iData,
                    Alembic::Util::uint64_t iOffset)
{

    // don't write anything if we will write beyond our buffer or the
    // stream is invalid
    if (!mData->stream || iSize == 0 || mData->size == 0 ||
        iOffset + iSize > mData->size)
    {
        return;
    }

    // +8 is to account for the written out size
    mData->stream->seek(mData->pos + iOffset + 8);
    mData->stream->write(iData, iSize);
}

Alembic::Util::uint64_t OData::getSize() const
{
    return mData->size;
}

Alembic::Util::uint64_t OData::getPos() const
{
    return mData->pos;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Ogawa
} // End namespace Alembic
