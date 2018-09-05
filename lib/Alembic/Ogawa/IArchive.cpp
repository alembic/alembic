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

#include <Alembic/Ogawa/IArchive.h>

namespace Alembic {
namespace Ogawa {
namespace ALEMBIC_VERSION_NS {

IArchive::IArchive(const std::string & iFileName,
                   std::size_t iNumStreams,
                   bool iUseMMap) :
    mStreams(new IStreams(iFileName, iNumStreams, iUseMMap))
{
    init();
}

IArchive::IArchive(const std::vector< std::istream * > & iStreams) :
    mStreams(new IStreams(iStreams))
{
    init();
}

void IArchive::init()
{
    if (mStreams->isValid())
    {
        Alembic::Util::uint64_t pos = 0;
        mStreams->read(0, 8, 8, &pos);
        mGroup.reset(new IGroup(mStreams, pos, false, 0));
    }
}

IArchive::~IArchive()
{
}

bool IArchive::isValid() const
{
    return mStreams->isValid();
}

bool IArchive::isFrozen() const
{
    return mStreams->isFrozen();
}

Alembic::Util::uint16_t IArchive::getVersion() const
{
    return mStreams->getVersion();
}

IGroupPtr IArchive::getGroup() const
{
    return mGroup;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Ogawa
} // End namespace Alembic
