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

#include <Alembic/AbcCoreOgawa/OrImpl.h>
#include <Alembic/AbcCoreOgawa/StreamManager.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//-*****************************************************************************
// OBJECT READER IMPLEMENTATION
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// Reading as a child of a parent.
OrImpl::OrImpl( AbcA::ObjectReaderPtr iParent,
                Ogawa::IGroupPtr iParentGroup,
                std::size_t iGroupIndex,
                ObjectHeaderPtr iHeader )
    : m_header( iHeader )
{
    m_parent = Alembic::Util::dynamic_pointer_cast< OrImpl,
        AbcA::ObjectReader > (iParent);

    // Check validity of all inputs.
    ABCA_ASSERT( m_parent, "Invalid parent in OrImpl(Object)" );
    ABCA_ASSERT( m_header, "Invalid header in OrImpl(Object)" );

    m_archive = m_parent->getArchiveImpl();
    ABCA_ASSERT( m_archive, "Invalid archive in OrImpl(Object)" );

    StreamIDPtr streamId = m_archive->getStreamID();
    std::size_t id = streamId->getID();
    Ogawa::IGroupPtr group = iParentGroup->getGroup( iGroupIndex, false, id );
    m_data.reset( new OrData( group, iHeader->getFullName(), id,
        *m_archive, m_archive->getIndexedMetaData() ) );
}

//-*****************************************************************************
OrImpl::OrImpl( Alembic::Util::shared_ptr< ArImpl > iArchive,
                OrDataPtr iData,
                ObjectHeaderPtr iHeader )
    : m_archive( iArchive )
    , m_data( iData )
    , m_header( iHeader )
{

    ABCA_ASSERT( m_archive, "Invalid archive in OrImpl(Archive)" );
    ABCA_ASSERT( m_data, "Invalid data in OrImpl(Archive)" );
    ABCA_ASSERT( m_header, "Invalid header in OrImpl(Archive)" );
}

//-*****************************************************************************
OrImpl::~OrImpl()
{
    // Nothing.
}

//-*****************************************************************************
const AbcA::ObjectHeader & OrImpl::getHeader() const
{
    return *m_header;
}

//-*****************************************************************************
AbcA::ArchiveReaderPtr OrImpl::getArchive()
{
    return m_archive;
}

//-*****************************************************************************
AbcA::ObjectReaderPtr OrImpl::getParent()
{
    return m_parent;
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr OrImpl::getProperties()
{
    return m_data->getProperties( asObjectPtr() );
}

//-*****************************************************************************
size_t OrImpl::getNumChildren()
{
    return m_data->getNumChildren();
}

//-*****************************************************************************
const AbcA::ObjectHeader & OrImpl::getChildHeader( size_t i )
{
    return m_data->getChildHeader( asObjectPtr(), i );
}

//-*****************************************************************************
const AbcA::ObjectHeader * OrImpl::getChildHeader( const std::string &iName )
{
    return m_data->getChildHeader( asObjectPtr(), iName );
}

//-*****************************************************************************
AbcA::ObjectReaderPtr OrImpl::getChild( const std::string &iName )
{
    return m_data->getChild( asObjectPtr(), iName );
}

AbcA::ObjectReaderPtr OrImpl::getChild( size_t i )
{
    return m_data->getChild( asObjectPtr(), i );
}

//-*****************************************************************************
AbcA::ObjectReaderPtr OrImpl::asObjectPtr()
{
    return shared_from_this();
}

//-*****************************************************************************
bool OrImpl::getPropertiesHash( Util::Digest & oDigest )
{
    StreamIDPtr streamId = m_archive->getStreamID();
    std::size_t id = streamId->getID();
    m_data->getPropertiesHash( oDigest, id );
    return true;
}

//-*****************************************************************************
bool OrImpl::getChildrenHash( Util::Digest & oDigest )
{
    StreamIDPtr streamId = m_archive->getStreamID();
    std::size_t id = streamId->getID();
    m_data->getChildrenHash( oDigest, id );
    return true;
}

//-*****************************************************************************
Alembic::Util::shared_ptr< ArImpl > OrImpl::getArchiveImpl() const
{
    return m_archive;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
