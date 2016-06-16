//-*****************************************************************************
//
// Copyright (c) 2013,
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

#include <Alembic/AbcCoreOgawa/OrData.h>
#include <Alembic/AbcCoreOgawa/OrImpl.h>
#include <Alembic/AbcCoreOgawa/CprData.h>
#include <Alembic/AbcCoreOgawa/CprImpl.h>
#include <Alembic/AbcCoreOgawa/ReadUtil.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
OrData::OrData( Ogawa::IGroupPtr iGroup,
                const std::string & iParentName,
                std::size_t iThreadId,
                AbcA::ArchiveReader & iArchive,
                const std::vector< AbcA::MetaData > & iIndexedMetaData )
    : m_children( NULL )
{
    ABCA_ASSERT( iGroup, "Invalid object data group" );

    m_group = iGroup;

    std::size_t numChildren = m_group->getNumChildren();

    if ( numChildren > 0 && m_group->isChildData( numChildren - 1 ) )
    {
        std::vector< ObjectHeaderPtr > headers;
        ReadObjectHeaders( m_group, numChildren - 1, iThreadId,
                           iParentName, iIndexedMetaData, headers );

        if ( !headers.empty() )
        {
            m_children = new Child[ headers.size() ];
        }

        for ( std::size_t i = 0; i < headers.size(); ++i )
        {
            m_childrenMap[headers[i]->getName()] = i;
            m_children[i].header = headers[i];
        }
    }

    if ( numChildren > 0 && m_group->isChildGroup( 0 ) )
    {
        Ogawa::IGroupPtr group = m_group->getGroup( 0, false, iThreadId );
        m_data = Alembic::Util::shared_ptr<CprData>(
            new CprData( group, iThreadId, iArchive, iIndexedMetaData ) );
    }
}

//-*****************************************************************************
OrData::~OrData()
{
    if ( m_children )
    {
        delete [] m_children;
    }
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr
OrData::getProperties( AbcA::ObjectReaderPtr iParent )
{
    Alembic::Util::scoped_lock l( m_cprlock );
    AbcA::CompoundPropertyReaderPtr ret = m_top.lock();

    if ( ! ret )
    {
        // time to make a new one
        ret = Alembic::Util::shared_ptr<CprImpl>(
            new CprImpl( iParent, m_data ) );
        m_top = ret;
    }

    return ret;
}

//-*****************************************************************************
size_t OrData::getNumChildren()
{
    return m_childrenMap.size();
}

//-*****************************************************************************
const AbcA::ObjectHeader &
OrData::getChildHeader( AbcA::ObjectReaderPtr iParent, size_t i )
{
    ABCA_ASSERT( i < m_childrenMap.size(),
        "Out of range index in OrData::getChildHeader: " << i );

    return *( m_children[i].header );
}

//-*****************************************************************************
const AbcA::ObjectHeader *
OrData::getChildHeader( AbcA::ObjectReaderPtr iParent,
                        const std::string &iName )
{
    ChildrenMap::iterator fiter = m_childrenMap.find( iName );
    if ( fiter == m_childrenMap.end() )
    {
        return NULL;
    }

    return & getChildHeader( iParent, fiter->second );
}

//-*****************************************************************************
AbcA::ObjectReaderPtr
OrData::getChild( AbcA::ObjectReaderPtr iParent, const std::string &iName )
{
    ChildrenMap::iterator fiter = m_childrenMap.find( iName );
    if ( fiter == m_childrenMap.end() )
    {
        return AbcA::ObjectReaderPtr();
    }

    return getChild( iParent, fiter->second );
}

//-*****************************************************************************
AbcA::ObjectReaderPtr
OrData::getChild( AbcA::ObjectReaderPtr iParent, size_t i )
{
    ABCA_ASSERT( i < m_childrenMap.size(),
        "Out of range index in OrData::getChild: " << i );

    Alembic::Util::scoped_lock l( m_children[i].lock );
    AbcA::ObjectReaderPtr optr = m_children[i].made.lock();

    if ( ! optr )
    {
        // Make a new one.
        optr = Alembic::Util::shared_ptr<OrImpl>(
            new OrImpl( iParent, m_group, i + 1, m_children[i].header ) );
        m_children[i].made = optr;
    }

    return optr;
}

void OrData::getPropertiesHash( Util::Digest & oDigest, size_t iThreadId )
{
    std::size_t numChildren = m_group->getNumChildren();
    Ogawa::IDataPtr data = m_group->getData( numChildren - 1, iThreadId );
    if ( data && data->getSize() >= 32 )
    {
        // last 32 bytes are properties hash, followed by children hash
        data->read( 16, oDigest.d, data->getSize() - 32, iThreadId );
    }
}

void OrData::getChildrenHash( Util::Digest & oDigest, size_t iThreadId )
{
    std::size_t numChildren = m_group->getNumChildren();
    Ogawa::IDataPtr data = m_group->getData( numChildren - 1, iThreadId );
    if ( data && data->getSize() >= 32 )
    {
        // children hash is the last 16 bytes
        data->read( 16, oDigest.d, data->getSize() - 16, iThreadId );
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
