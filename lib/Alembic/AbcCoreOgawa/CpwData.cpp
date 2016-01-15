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

#include <Alembic/AbcCoreOgawa/CpwData.h>
#include <Alembic/AbcCoreOgawa/WriteUtil.h>
#include <Alembic/AbcCoreOgawa/SpwImpl.h>
#include <Alembic/AbcCoreOgawa/ApwImpl.h>
#include <Alembic/AbcCoreOgawa/CpwImpl.h>
#include <Alembic/AbcCoreOgawa/AwImpl.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
CpwData::CpwData( Ogawa::OGroupPtr iGroup )
    : m_group( iGroup )
{
}

//-*****************************************************************************
CpwData::~CpwData()
{
}

//-*****************************************************************************
size_t CpwData::getNumProperties()
{
    return m_propertyHeaders.size();
}

//-*****************************************************************************
const AbcA::PropertyHeader &
CpwData::getPropertyHeader( size_t i )
{
    if ( i > m_propertyHeaders.size() )
    {
        ABCA_THROW( "Out of range index in " <<
                    "CpwImpl::getPropertyHeader: " << i );
    }

    PropertyHeaderPtr ptr = m_propertyHeaders[i];
    ABCA_ASSERT( ptr, "Invalid property header ptr in CpwImpl" );

    return ptr->header;
}

//-*****************************************************************************
const AbcA::PropertyHeader *
CpwData::getPropertyHeader( const std::string &iName )
{
    for ( PropertyHeaderPtrs::iterator piter = m_propertyHeaders.begin();
          piter != m_propertyHeaders.end(); ++piter )
    {
        if ( (*piter)->header.getName() == iName )
        {
            return &( (*piter)->header );
        }
    }
    return NULL;
}

//-*****************************************************************************
AbcA::BasePropertyWriterPtr
CpwData::getProperty( const std::string &iName )
{
    MadeProperties::iterator fiter = m_madeProperties.find( iName );
    if ( fiter == m_madeProperties.end() )
    {
        return AbcA::BasePropertyWriterPtr();
    }

    WeakBpwPtr wptr = (*fiter).second;

    return wptr.lock();
}

//-*****************************************************************************
AbcA::ScalarPropertyWriterPtr
CpwData::createScalarProperty( AbcA::CompoundPropertyWriterPtr iParent,
                               const std::string & iName,
                               const AbcA::MetaData & iMetaData,
                               const AbcA::DataType & iDataType,
                               Util::uint32_t iTimeSamplingIndex )
{
    if ( m_madeProperties.count( iName ) )
    {
        ABCA_THROW( "Already have a property named: " << iName );
    }

    ABCA_ASSERT( iDataType.getExtent() != 0 &&
                 iDataType.getPod() != Alembic::Util::kNumPlainOldDataTypes &&
                 iDataType.getPod() != Alembic::Util::kUnknownPOD,
                 "createScalarProperty, illegal DataType provided.");

    ABCA_ASSERT( iName != "" && iName.find('/') == std::string::npos,
                 "Invalid name" );

    // will assert if TimeSamplingPtr not found
    AbcA::TimeSamplingPtr ts =
        iParent->getObject()->getArchive()->getTimeSampling(
            iTimeSamplingIndex );

    PropertyHeaderPtr headerPtr( new PropertyHeaderAndFriends( iName,
        AbcA::kScalarProperty, iMetaData, iDataType, ts, iTimeSamplingIndex ) );

    Alembic::Util::shared_ptr<SpwImpl>
        ret( new SpwImpl( iParent, m_group->addGroup(), headerPtr,
                          m_propertyHeaders.size() ) );

    m_propertyHeaders.push_back( headerPtr );
    m_madeProperties[iName] = WeakBpwPtr( ret );

    m_hashes.push_back(0);
    m_hashes.push_back(0);

    return ret;
}

//-*****************************************************************************
AbcA::ArrayPropertyWriterPtr
CpwData::createArrayProperty( AbcA::CompoundPropertyWriterPtr iParent,
                              const std::string & iName,
                              const AbcA::MetaData & iMetaData,
                              const AbcA::DataType & iDataType,
                              Util::uint32_t iTimeSamplingIndex )
{
    if ( m_madeProperties.count( iName ) )
    {
        ABCA_THROW( "Already have a property named: " << iName );
    }

    ABCA_ASSERT( iDataType.getExtent() != 0 &&
                 iDataType.getPod() != Alembic::Util::kNumPlainOldDataTypes &&
                 iDataType.getPod() != Alembic::Util::kUnknownPOD,
                 "createArrayProperty, illegal DataType provided.");

    ABCA_ASSERT( iName != "" && iName.find('/') == std::string::npos,
                 "Invalid name" );

    // will assert if TimeSamplingPtr not found
    AbcA::TimeSamplingPtr ts =
        iParent->getObject()->getArchive()->getTimeSampling(
            iTimeSamplingIndex );

    PropertyHeaderPtr headerPtr( new PropertyHeaderAndFriends( iName,
        AbcA::kArrayProperty, iMetaData, iDataType, ts, iTimeSamplingIndex ) );

    Alembic::Util::shared_ptr<ApwImpl>
        ret( new ApwImpl( iParent, m_group->addGroup(), headerPtr,
                          m_propertyHeaders.size() ) );

    m_propertyHeaders.push_back( headerPtr );
    m_madeProperties[iName] = WeakBpwPtr( ret );

    m_hashes.push_back(0);
    m_hashes.push_back(0);

    return ret;
}

//-*****************************************************************************
AbcA::CompoundPropertyWriterPtr
CpwData::createCompoundProperty( AbcA::CompoundPropertyWriterPtr iParent,
                                 const std::string & iName,
                                 const AbcA::MetaData & iMetaData )
{
    if ( m_madeProperties.count( iName ) )
    {
        ABCA_THROW( "Already have a property named: " << iName );
    }

   PropertyHeaderPtr headerPtr( new PropertyHeaderAndFriends( iName,
                                iMetaData ) );

    Alembic::Util::shared_ptr<CpwImpl>
        ret( new CpwImpl( iParent, m_group->addGroup(), headerPtr,
                          m_propertyHeaders.size() ) );

    m_propertyHeaders.push_back( headerPtr );
    m_madeProperties[iName] = WeakBpwPtr( ret );

    m_hashes.push_back(0);
    m_hashes.push_back(0);

    return ret;
}

//-*****************************************************************************
void CpwData::writePropertyHeaders( MetaDataMapPtr iMetaDataMap )
{
    // pack in child header and other info
    std::vector< Util::uint8_t > data;
    for ( size_t i = 0; i < getNumProperties(); ++i )
    {
        PropertyHeaderPtr prop = m_propertyHeaders[i];
        WritePropertyInfo( data,
                           prop->header,
                           prop->isScalarLike,
                           prop->isHomogenous,
                           prop->timeSamplingIndex,
                           prop->nextSampleIndex,
                           prop->firstChangedIndex,
                           prop->lastChangedIndex,
                           iMetaDataMap );
    }

    if ( !data.empty() )
    {
        m_group->addData( data.size(), &( data.front() ) );
    }
}

//-*****************************************************************************
void CpwData::fillHash( size_t iIndex, Util::uint64_t iHash0,
    Util::uint64_t iHash1 )
{

    ABCA_ASSERT( iIndex < m_propertyHeaders.size() &&
                 iIndex * 2 < m_hashes.size(),
                 "Invalid property requested in CpwData::fillHash" );

    m_hashes[ iIndex * 2     ] = iHash0;
    m_hashes[ iIndex * 2 + 1 ] = iHash1;
}

//-*****************************************************************************
void CpwData::computeHash( Util::SpookyHash & ioHash )
{
    if ( !m_hashes.empty() )
    {
        ioHash.Update( &m_hashes.front(), m_hashes.size() * 8 );
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
