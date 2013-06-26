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

#ifndef _Alembic_AbcCoreOgawa_OwImpl_h_
#define _Alembic_AbcCoreOgawa_OwImpl_h_

#include <Alembic/AbcCoreOgawa/Foundation.h>
#include <Alembic/AbcCoreOgawa/OwData.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
// These are _always_ created as shared ptrs.
class OwImpl
    : public AbcA::ObjectWriter
    , public Alembic::Util::enable_shared_from_this<OwImpl>
{
public:

    OwImpl( AbcA::ArchiveWriterPtr iArchive,
            OwDataPtr iData,
            const AbcA::MetaData & iMetaData );

    OwImpl( AbcA::ObjectWriterPtr iParent,
            Ogawa::OGroupPtr iGroup,
            ObjectHeaderPtr iHeader,
            size_t iIndex );

    virtual ~OwImpl();

    //-*************************************************************************
    // FROM ABSTRACT
    //-*************************************************************************

    virtual const AbcA::ObjectHeader & getHeader() const;

    virtual AbcA::ArchiveWriterPtr getArchive();

    virtual AbcA::ObjectWriterPtr getParent();

    virtual AbcA::CompoundPropertyWriterPtr getProperties();

    virtual size_t getNumChildren();

    virtual const AbcA::ObjectHeader & getChildHeader( size_t i );

    virtual const AbcA::ObjectHeader *
    getChildHeader( const std::string &iName );

    virtual AbcA::ObjectWriterPtr getChild( const std::string &iName );

    virtual AbcA::ObjectWriterPtr
    createChild( const AbcA::ObjectHeader &iHeader );

    virtual AbcA::ObjectWriterPtr asObjectPtr();

    void fillHash( size_t iIndex, Util::uint64_t iHash0,
                   Util::uint64_t iHash1 );

private:
    // The parent object, NULL if it is the "top" object
    AbcA::ObjectWriterPtr m_parent;

    // The archive ptr.
    AbcA::ArchiveWriterPtr m_archive;

    // The header which defines this property.
    ObjectHeaderPtr m_header;

    // child object data, this is owned by the archive for "top" objects
    OwDataPtr m_data;

    size_t m_index;

};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif
