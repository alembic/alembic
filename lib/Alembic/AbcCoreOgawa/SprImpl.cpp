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

#include <Alembic/AbcCoreOgawa/SprImpl.h>
#include <Alembic/AbcCoreOgawa/ReadUtil.h>
#include <Alembic/AbcCoreOgawa/StreamManager.h>
#include <Alembic/AbcCoreOgawa/OrImpl.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
SprImpl::SprImpl( AbcA::CompoundPropertyReaderPtr iParent,
                  Ogawa::IGroupPtr iGroup,
                  PropertyHeaderPtr iHeader )
  : m_parent( iParent )
  , m_group( iGroup )
  , m_header( iHeader )
{
    // Validate all inputs.
    ABCA_ASSERT( m_parent, "Invalid parent" );
    ABCA_ASSERT( m_group, "Invalid scalar property group" );
    ABCA_ASSERT( m_header, "Invalid header" );

    if ( m_header->header.getPropertyType() != AbcA::kScalarProperty )
    {
        ABCA_THROW( "Attempted to create a ScalarPropertyReader from a "
                    "non-array property type" );
    }
}

//-*****************************************************************************
const AbcA::PropertyHeader & SprImpl::getHeader() const
{
    return m_header->header;
}

//-*****************************************************************************
AbcA::ObjectReaderPtr SprImpl::getObject()
{
    return m_parent->getObject();
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr SprImpl::getParent()
{
    return m_parent;
}

//-*****************************************************************************
AbcA::ScalarPropertyReaderPtr SprImpl::asScalarPtr()
{
    return shared_from_this();
}

//-*****************************************************************************
size_t SprImpl::getNumSamples()
{
    return m_header->nextSampleIndex;
}

//-*****************************************************************************
bool SprImpl::isConstant()
{
    return ( m_header->firstChangedIndex == 0 );
}

//-*****************************************************************************
void SprImpl::getSample( index_t iSampleIndex, void * iIntoLocation )
{
    size_t index = m_header->verifyIndex( iSampleIndex );

    StreamIDPtr streamId = Alembic::Util::dynamic_pointer_cast< ArImpl,
        AbcA::ArchiveReader > ( getObject()->getArchive() )->getStreamID();

    std::size_t id = streamId->getID();
    Ogawa::IDataPtr data = m_group->getData( index, id );
    ReadData( iIntoLocation, data, id,
              m_header->header.getDataType(),
              m_header->header.getDataType().getPod() );
}

//-*****************************************************************************
std::pair<index_t, chrono_t> SprImpl::getFloorIndex( chrono_t iTime )
{
    return m_header->header.getTimeSampling()->getFloorIndex( iTime,
        m_header->nextSampleIndex );
}

//-*****************************************************************************
std::pair<index_t, chrono_t> SprImpl::getCeilIndex( chrono_t iTime )
{
    return m_header->header.getTimeSampling()->getCeilIndex( iTime,
        m_header->nextSampleIndex );
}

//-*****************************************************************************
std::pair<index_t, chrono_t> SprImpl::getNearIndex( chrono_t iTime )
{
    return m_header->header.getTimeSampling()->getNearIndex( iTime,
        m_header->nextSampleIndex );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
