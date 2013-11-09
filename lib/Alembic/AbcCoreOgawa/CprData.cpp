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

#include <Alembic/AbcCoreOgawa/CprData.h>
#include <Alembic/AbcCoreOgawa/ReadUtil.h>
#include <Alembic/AbcCoreOgawa/CprImpl.h>
#include <Alembic/AbcCoreOgawa/SprImpl.h>
#include <Alembic/AbcCoreOgawa/AprImpl.h>
#include <Alembic/AbcCoreOgawa/StreamManager.h>
#include <Alembic/AbcCoreOgawa/ArImpl.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
CprData::CprData( Ogawa::IGroupPtr iGroup,
                  std::size_t iThreadId,
                  AbcA::ArchiveReader & iArchive,
                  const std::vector< AbcA::MetaData > & iIndexedMetaData )
    : m_propertyHeaders( NULL )
{
    ABCA_ASSERT( iGroup, "invalid compound data group" );

    m_group = iGroup;

    std::size_t numChildren = m_group->getNumChildren();

    if ( numChildren > 0 && m_group->isChildData( numChildren - 1 ) )
    {
        PropertyHeaderPtrs headers;
        ReadPropertyHeaders( m_group, numChildren - 1, iThreadId,
                             iArchive, iIndexedMetaData, headers );

        m_propertyHeaders = new SubProperty[ headers.size() ];
        for ( std::size_t i = 0; i < headers.size(); ++i )
        {
            m_subProperties[headers[i]->header.getName()] = i;
            m_propertyHeaders[i].header = headers[i];
        }
    }
}

//-*****************************************************************************
CprData::~CprData()
{
    if ( m_propertyHeaders )
    {
        delete [] m_propertyHeaders;
    }
}

//-*****************************************************************************
size_t CprData::getNumProperties()
{
    // fixed length and resize called in ctor, so multithread safe.
    return m_subProperties.size();
}

//-*****************************************************************************
const AbcA::PropertyHeader &
CprData::getPropertyHeader( AbcA::CompoundPropertyReaderPtr iParent, size_t i )
{
    // fixed length and resize called in ctor, so multithread safe.
    if ( i > m_subProperties.size() )
    {
        ABCA_THROW( "Out of range index in "
                    << "CprData::getPropertyHeader: " << i );
    }

    return m_propertyHeaders[i].header->header;
}

//-*****************************************************************************
const AbcA::PropertyHeader *
CprData::getPropertyHeader( AbcA::CompoundPropertyReaderPtr iParent,
                            const std::string &iName )
{
    // map of names to indexes filled by ctor (CprAttrVistor),
    // so multithread safe.
    SubPropertiesMap::iterator fiter = m_subProperties.find( iName );
    if ( fiter == m_subProperties.end() )
    {
        return NULL;
    }

    return &(getPropertyHeader(iParent, fiter->second));
}

//-*****************************************************************************
AbcA::ScalarPropertyReaderPtr
CprData::getScalarProperty( AbcA::CompoundPropertyReaderPtr iParent,
                            const std::string &iName )
{
    SubPropertiesMap::iterator fiter = m_subProperties.find( iName );
    if ( fiter == m_subProperties.end() )
    {
        return AbcA::ScalarPropertyReaderPtr();
    }

    SubProperty & sub = m_propertyHeaders[fiter->second];

    if ( !(sub.header->header.isScalar()) )
    {
        ABCA_THROW( "Tried to read a scalar property from a non-scalar: "
                    << iName << ", type: "
                    << sub.header->header.getPropertyType() );
    }

    Alembic::Util::scoped_lock l( sub.lock );
    AbcA::BasePropertyReaderPtr bptr = sub.made.lock();
    if ( ! bptr )
    {
        StreamIDPtr streamId = Alembic::Util::dynamic_pointer_cast< ArImpl,
            AbcA::ArchiveReader > (
                iParent->getObject()->getArchive() )->getStreamID();

        Ogawa::IGroupPtr group = m_group->getGroup( fiter->second, true,
                                                    streamId->getID() );

        ABCA_ASSERT( group, "Scalar Property not backed by a valid group.");

        // Make a new one.
        bptr = Alembic::Util::shared_ptr<SprImpl>(
            new SprImpl( iParent, group, sub.header ) );
        sub.made = bptr;
    }

    AbcA::ScalarPropertyReaderPtr ret =
        Alembic::Util::dynamic_pointer_cast<AbcA::ScalarPropertyReader,
        AbcA::BasePropertyReader>( bptr );
    return ret;
}

//-*****************************************************************************
AbcA::ArrayPropertyReaderPtr
CprData::getArrayProperty( AbcA::CompoundPropertyReaderPtr iParent,
                           const std::string &iName )
{
    // map of names to indexes filled by ctor (CprAttrVistor),
    // so multithread safe.
    SubPropertiesMap::iterator fiter = m_subProperties.find( iName );
    if ( fiter == m_subProperties.end() )
    {
        return AbcA::ArrayPropertyReaderPtr();
    }

    SubProperty & sub = m_propertyHeaders[fiter->second];

    if ( !(sub.header->header.isArray()) )
    {
        ABCA_THROW( "Tried to read an array property from a non-array: "
                    << iName << ", type: "
                    << sub.header->header.getPropertyType() );
    }

    Alembic::Util::scoped_lock l( sub.lock );
    AbcA::BasePropertyReaderPtr bptr = sub.made.lock();
    if ( ! bptr )
    {
        StreamIDPtr streamId = Alembic::Util::dynamic_pointer_cast< ArImpl,
            AbcA::ArchiveReader > (
                iParent->getObject()->getArchive() )->getStreamID();

        Ogawa::IGroupPtr group = m_group->getGroup( fiter->second, true,
                                                    streamId->getID() );

        ABCA_ASSERT( group, "Array Property not backed by a valid group.");

        // Make a new one.
        bptr = Alembic::Util::shared_ptr<AprImpl>(
            new AprImpl( iParent, group, sub.header ) );

        sub.made = bptr;
    }

    AbcA::ArrayPropertyReaderPtr ret =
        Alembic::Util::dynamic_pointer_cast<AbcA::ArrayPropertyReader,
        AbcA::BasePropertyReader>( bptr );
    return ret;
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr
CprData::getCompoundProperty( AbcA::CompoundPropertyReaderPtr iParent,
                              const std::string &iName )
{
    // map of names to indexes filled by ctor (CprAttrVistor),
    // so multithread safe.
    SubPropertiesMap::iterator fiter = m_subProperties.find( iName );
    if ( fiter == m_subProperties.end() )
    {
        return AbcA::CompoundPropertyReaderPtr();
    }

    SubProperty & sub = m_propertyHeaders[fiter->second];

    if ( !(sub.header->header.isCompound()) )
    {
        ABCA_THROW( "Tried to read a compound property from a non-compound: "
                    << iName << ", type: "
                    << sub.header->header.getPropertyType() );
    }

    Alembic::Util::scoped_lock l( sub.lock );
    AbcA::BasePropertyReaderPtr bptr = sub.made.lock();
    if ( ! bptr )
    {
        Alembic::Util::shared_ptr<  ArImpl > implPtr =
            Alembic::Util::dynamic_pointer_cast< ArImpl, AbcA::ArchiveReader > (
                iParent->getObject()->getArchive() );

        StreamIDPtr streamId = implPtr->getStreamID();

        Ogawa::IGroupPtr group = m_group->getGroup( fiter->second, false,
                                                    streamId->getID() );

        ABCA_ASSERT( group, "Compound Property not backed by a valid group.");

        // Make a new one.
        bptr = Alembic::Util::shared_ptr<CprImpl>(
            new CprImpl( iParent, group, sub.header, streamId->getID(),
                         implPtr->getIndexedMetaData() ) );

        sub.made = bptr;
    }

    AbcA::CompoundPropertyReaderPtr ret =
        Alembic::Util::dynamic_pointer_cast<AbcA::CompoundPropertyReader,
        AbcA::BasePropertyReader>( bptr );
    return ret;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
