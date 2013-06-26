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

#include <Alembic/AbcCoreOgawa/AprImpl.h>
#include <Alembic/AbcCoreOgawa/ReadUtil.h>
#include <Alembic/AbcCoreOgawa/StreamManager.h>
#include <Alembic/AbcCoreOgawa/OrImpl.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
AprImpl::AprImpl( AbcA::CompoundPropertyReaderPtr iParent,
                  Ogawa::IGroupPtr iGroup,
                  PropertyHeaderPtr iHeader )
  : m_parent( iParent )
  , m_group( iGroup )
  , m_header( iHeader )
{
    // Validate all inputs.
    ABCA_ASSERT( m_parent, "Invalid parent" );
    ABCA_ASSERT( m_group, "Invalid array property group" );
    ABCA_ASSERT( m_header, "Invalid header" );

    if ( m_header->header.getPropertyType() != AbcA::kArrayProperty )
    {
        ABCA_THROW( "Attempted to create a ArrayPropertyReader from a "
                    "non-array property type" );
    }
}

//-*****************************************************************************
const AbcA::PropertyHeader & AprImpl::getHeader() const
{
    return m_header->header;
}

//-*****************************************************************************
AbcA::ObjectReaderPtr AprImpl::getObject()
{
    return m_parent->getObject();
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr AprImpl::getParent()
{
    return m_parent;
}

//-*****************************************************************************
AbcA::ArrayPropertyReaderPtr AprImpl::asArrayPtr()
{
    return shared_from_this();
}

//-*****************************************************************************
size_t AprImpl::getNumSamples()
{
    return m_header->nextSampleIndex;
}

//-*****************************************************************************
bool AprImpl::isConstant()
{
    return ( m_header->firstChangedIndex == 0 );
}

//-*****************************************************************************
void AprImpl::getSample( index_t iSampleIndex, AbcA::ArraySamplePtr &oSample )
{
    size_t index = m_header->verifyIndex( iSampleIndex ) * 2;

    StreamIDPtr streamId = Alembic::Util::dynamic_pointer_cast< ArImpl,
        AbcA::ArchiveReader > ( getObject()->getArchive() )->getStreamID();

    std::size_t id = streamId->getID();
    Ogawa::IDataPtr dims = m_group->getData(index + 1, id);
    Ogawa::IDataPtr data = m_group->getData(index, id);

    ReadArraySample( dims, data, id, m_header->header.getDataType(), oSample );
}

//-*****************************************************************************
std::pair<index_t, chrono_t> AprImpl::getFloorIndex( chrono_t iTime )
{
    return m_header->header.getTimeSampling()->getFloorIndex( iTime,
        m_header->nextSampleIndex );
}

//-*****************************************************************************
std::pair<index_t, chrono_t> AprImpl::getCeilIndex( chrono_t iTime )
{
    return m_header->header.getTimeSampling()->getCeilIndex( iTime,
        m_header->nextSampleIndex );
}

//-*****************************************************************************
std::pair<index_t, chrono_t> AprImpl::getNearIndex( chrono_t iTime )
{
    return m_header->header.getTimeSampling()->getNearIndex( iTime,
        m_header->nextSampleIndex );
}

//-*****************************************************************************
bool AprImpl::getKey( index_t iSampleIndex, AbcA::ArraySampleKey & oKey )
{
    oKey.readPOD = m_header->header.getDataType().getPod();
    oKey.origPOD = oKey.readPOD;
    oKey.numBytes = 0;

    // * 2 for Array properties (since we also write the dimensions)
    size_t index = m_header->verifyIndex( iSampleIndex ) * 2;

    StreamIDPtr streamId = Alembic::Util::dynamic_pointer_cast< ArImpl,
        AbcA::ArchiveReader > ( getObject()->getArchive() )->getStreamID();

    std::size_t id = streamId->getID();
    Ogawa::IDataPtr data = m_group->getData( index, id );

    if ( data )
    {
        if ( data->getSize() >= 16 )
        {
            oKey.numBytes = data->getSize() - 16;
            data->read( 16, oKey.digest.d, 0, id );
        }

        return true;
    }

    return false;
}

//-*****************************************************************************
bool AprImpl::isScalarLike()
{
    return m_header->isScalarLike;
}

//-*****************************************************************************
void AprImpl::getDimensions( index_t iSampleIndex,
                             Alembic::Util::Dimensions & oDim )
{
    size_t index = m_header->verifyIndex( iSampleIndex ) * 2;

    StreamIDPtr streamId = Alembic::Util::dynamic_pointer_cast< ArImpl,
        AbcA::ArchiveReader > ( getObject()->getArchive() )->getStreamID();

    std::size_t id = streamId->getID();
    Ogawa::IDataPtr dims = m_group->getData(index + 1, id);
    Ogawa::IDataPtr data = m_group->getData(index, id);

    ReadDimensions( dims, data, id, m_header->header.getDataType(), oDim );

}

//-*****************************************************************************
void AprImpl::getAs( index_t iSampleIndex, void *iIntoLocation,
                     Alembic::Util::PlainOldDataType iPod )
{
    size_t index = m_header->verifyIndex( iSampleIndex ) * 2;

    StreamIDPtr streamId = Alembic::Util::dynamic_pointer_cast< ArImpl,
        AbcA::ArchiveReader > ( getObject()->getArchive() )->getStreamID();

    std::size_t id = streamId->getID();
    Ogawa::IDataPtr data = m_group->getData( index, id );
    ReadData( iIntoLocation, data, id, m_header->header.getDataType(), iPod );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
