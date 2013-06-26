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

#include <Alembic/AbcCoreOgawa/CpwImpl.h>
#include <Alembic/AbcCoreOgawa/AwImpl.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************

// "top" compound creation called by object writers.
CpwImpl::CpwImpl( AbcA::ObjectWriterPtr iParent,
                  CpwDataPtr iData,
                  const AbcA::MetaData & iMeta )
    : m_object( iParent)
    , m_header( new PropertyHeaderAndFriends( "", iMeta ) )
    , m_data( iData )
    , m_index( 0 )
{
    // we don't need to write the property info, the object has done it already

    ABCA_ASSERT( m_object, "Invalid object" );
    ABCA_ASSERT( m_data, "Invalid compound data" );
}

// With the compound property writer as an input.
CpwImpl::CpwImpl( AbcA::CompoundPropertyWriterPtr iParent,
                  Ogawa::OGroupPtr iGroup,
                  PropertyHeaderPtr iHeader,
                  size_t iIndex )
  : m_parent( iParent )
  , m_header( iHeader )
  , m_index( iIndex )
{
    // Check the validity of all inputs.
    ABCA_ASSERT( m_parent, "Invalid parent" );
    ABCA_ASSERT( m_header, "Invalid header" );

    m_object = iParent->getObject();

    ABCA_ASSERT( m_parent, "Invalid parent object" );

    ABCA_ASSERT( m_header->header.getName() != "" &&
                 m_header->header.getName().find('/') == std::string::npos,
                 "Invalid name" );

    m_data.reset( new CpwData( iGroup ) );
}

//-*****************************************************************************
CpwImpl::~CpwImpl()
{
    // objects are responsible for calling this on the CpWData they own
    // as part of their "top" compound
    if ( m_parent )
    {
        MetaDataMapPtr mdMap = Alembic::Util::dynamic_pointer_cast<
            AwImpl, AbcA::ArchiveWriter >(
                getObject()->getArchive() )->getMetaDataMap();
        m_data->writePropertyHeaders( mdMap );

        Util::SpookyHash hash;
        hash.Init( 0, 0 );
        m_data->computeHash( hash );
        HashPropertyHeader( m_header->header, hash );

        Util::uint64_t hash0, hash1;
        hash.Final( &hash0, &hash1 );
        Util::shared_ptr< CpwImpl > parent =
            Alembic::Util::dynamic_pointer_cast< CpwImpl,
                AbcA::CompoundPropertyWriter > ( m_parent );
        parent->fillHash( m_index, hash0, hash1 );
    }
}

//-*****************************************************************************
const AbcA::PropertyHeader &CpwImpl::getHeader() const
{
    return m_header->header;
}

//-*****************************************************************************
AbcA::ObjectWriterPtr CpwImpl::getObject()
{
    return m_object;
}

//-*****************************************************************************
AbcA::CompoundPropertyWriterPtr CpwImpl::getParent()
{
    // this will be NULL for "top" compound properties
    return m_parent;
}

//-*****************************************************************************
AbcA::CompoundPropertyWriterPtr CpwImpl::asCompoundPtr()
{
    return shared_from_this();
}

//-*****************************************************************************
size_t CpwImpl::getNumProperties()
{
    return m_data->getNumProperties();
}

//-*****************************************************************************
const AbcA::PropertyHeader & CpwImpl::getPropertyHeader( size_t i )
{
    return m_data->getPropertyHeader( i );
}

//-*****************************************************************************
const AbcA::PropertyHeader *
CpwImpl::getPropertyHeader( const std::string &iName )
{
    return m_data->getPropertyHeader( iName );
}

//-*****************************************************************************
AbcA::BasePropertyWriterPtr CpwImpl::getProperty( const std::string & iName )
{
    return m_data->getProperty( iName );
}

//-*****************************************************************************
AbcA::ScalarPropertyWriterPtr
CpwImpl::createScalarProperty( const std::string & iName,
                      const AbcA::MetaData & iMetaData,
                      const AbcA::DataType & iDataType,
                      Util::uint32_t iTimeSamplingIndex )
{
    AbcA::ScalarPropertyWriterPtr scalarProp =
        m_data->createScalarProperty( asCompoundPtr(), iName, iMetaData,
                                      iDataType, iTimeSamplingIndex );

    return scalarProp;
}

//-*****************************************************************************
AbcA::ArrayPropertyWriterPtr
CpwImpl::createArrayProperty( const std::string & iName,
                              const AbcA::MetaData & iMetaData,
                              const AbcA::DataType & iDataType,
                              Util::uint32_t iTimeSamplingIndex )
{
    AbcA::ArrayPropertyWriterPtr arrayProp =
        m_data->createArrayProperty( asCompoundPtr(), iName, iMetaData,
                                     iDataType, iTimeSamplingIndex );

    return arrayProp;
}

//-*****************************************************************************
AbcA::CompoundPropertyWriterPtr
CpwImpl::createCompoundProperty( const std::string & iName,
                                 const AbcA::MetaData & iMetaData )
{
    AbcA::CompoundPropertyWriterPtr compoundProp =
        m_data->createCompoundProperty( asCompoundPtr(), iName, iMetaData );

    return compoundProp;
}

void CpwImpl::fillHash( std::size_t iIndex, Util::uint64_t iHash0,
                        Util::uint64_t iHash1 )
{
    m_data->fillHash( iIndex, iHash0, iHash1 );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic

