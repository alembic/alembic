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

#ifndef _Alembic_Ogawa_OGroup_h_
#define _Alembic_Ogawa_OGroup_h_

#include <Alembic/Ogawa/Foundation.h>
#include <Alembic/Ogawa/OStream.h>
#include <Alembic/Ogawa/OData.h>

namespace Alembic {
namespace Ogawa {
namespace ALEMBIC_VERSION_NS {

class OGroup;
typedef Alembic::Util::shared_ptr< OGroup > OGroupPtr;

class OGroup : public Alembic::Util::enable_shared_from_this< OGroup >
{
public:
    ~OGroup();

    // create a group and add it as a child to this group
    OGroupPtr addGroup();

    // write the data stream and add it as a child to this group
    ODataPtr addData(Alembic::Util::uint64_t iSize, const void * iData);

    // write data streams from multiple sources as one continuous data stream
    // and add it as a child to this group
    ODataPtr addData(Alembic::Util::uint64_t iNumData,
                     const Alembic::Util::uint64_t * iSizes,
                     const void ** iDatas);

    // write a data stream but DON'T add it as a child to this group
    // If ODataPtr isn't added to this or any other group, you will
    // end up abandoning it within the file and waste disk space.
    ODataPtr createData(Alembic::Util::uint64_t iSize, const void * iData);

    // write data streams as one continuous data stream but DON'T add it as a
    // child to this group.
    // If ODataPtr isn't added to this or any other group, you will
    // end up abandoning it within the file and waste disk space.
    ODataPtr createData(Alembic::Util::uint64_t iNumData,
                        const Alembic::Util::uint64_t * iSizes,
                        const void ** iDatas);

    // reference existing data
    void addData(ODataPtr iData);

    // reference an existing group
    void addGroup(OGroupPtr iGroup);

    // convenience function for adding a default NULL group
    void addEmptyGroup();

    // convenience function for adding empty data
    void addEmptyData();

    // can no longer add any more children, we can still update them
    // via the replace calls though
    void freeze();

    bool isFrozen();

    Alembic::Util::uint64_t getNumChildren() const;

    bool isChildGroup(Alembic::Util::uint64_t iIndex) const;

    bool isChildData(Alembic::Util::uint64_t iIndex) const;

    bool isChildEmptyGroup(Alembic::Util::uint64_t iIndex) const;

    bool isChildEmptyData(Alembic::Util::uint64_t iIndex) const;

    void replaceData(Alembic::Util::uint64_t iIndex, ODataPtr iData);

    // currently I'm going to leave this out, because a bad implementation
    // could cause all sorts of subtle race conditions when unfrozen children
    // are suddenly frozen.  It may also not be necessary (you can still
    // reference an existing group)
    // if this is necessary, an easy compromise might be that iGroup HAS to
    // be frozen, much like how replaceData deals with something implicitly
    // frozen
    //void replaceGroup(Alembic::Util::uint64_t iIndex, OGroupPtr iGroup);

private:
    friend class OArchive;
    OGroup(OStreamPtr iStream);

    OGroup(OGroupPtr iParent, Alembic::Util::uint64_t iIndex);

    class PrivateData;
    Alembic::Util::auto_ptr< PrivateData > mData;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Ogawa

} // End namespace Alembic

#endif
