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

#ifndef _Alembic_Ogawa_IGroup_h_
#define _Alembic_Ogawa_IGroup_h_

#include <Alembic/Ogawa/Foundation.h>
#include <Alembic/Ogawa/IStreams.h>
#include <Alembic/Ogawa/IData.h>

namespace Alembic {
namespace Ogawa {
namespace ALEMBIC_VERSION_NS {

class IGroup;
typedef Alembic::Util::shared_ptr< IGroup > IGroupPtr;

class IGroup
{
public:
    ~IGroup();

    IGroupPtr getGroup(Alembic::Util::uint64_t iIndex, bool iLight,
                       std::size_t iThreadIndex);

    IDataPtr getData(Alembic::Util::uint64_t iIndex, std::size_t iThreadIndex);

    Alembic::Util::uint64_t getNumChildren() const;

    bool isChildGroup(Alembic::Util::uint64_t iIndex) const;
    bool isChildData(Alembic::Util::uint64_t iIndex) const;

    bool isEmptyChildGroup(Alembic::Util::uint64_t iIndex) const;
    bool isEmptyChildData(Alembic::Util::uint64_t iIndex) const;

    bool isLight() const;

private:
    friend class IArchive;
    IGroup(IStreamsPtr iStreams, Alembic::Util::uint64_t iPos, bool iLight,
           std::size_t iThreadIndex);

    class PrivateData;
    Alembic::Util::auto_ptr< PrivateData > mData;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Ogawa

} // End namespace Alembic

#endif
