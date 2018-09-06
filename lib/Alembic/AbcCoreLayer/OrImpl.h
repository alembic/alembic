//-*****************************************************************************
//
// Copyright (c) 2016,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#ifndef Alembic_AbcCoreLayer_OrImpl_h
#define Alembic_AbcCoreLayer_OrImpl_h

#include <Alembic/AbcCoreLayer/Foundation.h>
#include <Alembic/AbcCoreLayer/ArImpl.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

typedef std::pair < AbcA::ObjectReaderPtr, size_t > ObjectAndIndex;

//-*****************************************************************************
class OrImpl
    : public AbcA::ObjectReader
    , public Alembic::Util::enable_shared_from_this<OrImpl>
{

public:

    OrImpl( ArImplPtr iArchive,
            std::vector< AbcA::ObjectReaderPtr > & iTops,
            ObjectHeaderPtr iHeader);

    OrImpl( OrImplPtr iParent, size_t iIndex );

    virtual ~OrImpl();

    //-*************************************************************************
    // ABSTRACT
    //-*************************************************************************
    virtual const AbcA::ObjectHeader & getHeader() const;

    virtual AbcA::ArchiveReaderPtr getArchive();

    virtual AbcA::ObjectReaderPtr getParent();

    virtual AbcA::CompoundPropertyReaderPtr getProperties();

    virtual size_t getNumChildren();

    virtual const AbcA::ObjectHeader & getChildHeader( size_t i );

    virtual const AbcA::ObjectHeader * getChildHeader
    ( const std::string &iName );

    virtual AbcA::ObjectReaderPtr getChild( const std::string &iName );

    virtual AbcA::ObjectReaderPtr getChild( size_t i );

    virtual AbcA::ObjectReaderPtr asObjectPtr();

    virtual bool getPropertiesHash( Util::Digest & oDigest );

    virtual bool getChildrenHash( Util::Digest & oDigest );

private:

    // builds up our data
    void init( std::vector< AbcA::ObjectReaderPtr > & iObjects );

    // The parent object
    OrImplPtr m_parent;

    // The index within the parent object where we can get our data
    size_t m_index;

    ArImplPtr m_archive;

    // this objects header
    ObjectHeaderPtr m_header;

    // all of our compounded child headers
    std::vector< ObjectHeaderPtr > m_childHeaders;

    // each child is made up of the original parent objects and the index
    // in each of them where that child lives
    std::vector< std::vector< ObjectAndIndex > > m_children;
    std::vector< Alembic::Util::weak_ptr< AbcA::ObjectReader > > m_children_ptrs;
    Alembic::Util::mutex m_lock;

    // all of our top properties, will be combined into m_top
    std::vector< AbcA::CompoundPropertyReaderPtr > m_properties;
    Alembic::Util::weak_ptr< AbcA::CompoundPropertyReader > m_top;

    ChildNameMap m_childNameMap;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_OrImpl_h_
