//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <Alembic/AbcCoreHDF5/CpwImpl.h>
#include <Alembic/AbcCoreHDF5/WriteUtil.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************

// "top" compound creation called by object writers.
CpwImpl::CpwImpl( AbcA::ObjectWriterPtr iParent,
                  CpwDataPtr iData,
                  const AbcA::MetaData & iMeta )
    : m_object( iParent)
    , m_header( "", iMeta )
    , m_data( iData )
{
    // we don't need to write the property info, the object has done it already

    ABCA_ASSERT( m_object, "Invalid object" );
    ABCA_ASSERT( m_data, "Invalid compound data" );
}

// With the compound property writer as an input.
CpwImpl::CpwImpl( AbcA::CompoundPropertyWriterPtr iParent,
                  hid_t iParentGroup,
                  const std::string & iName,
                  const AbcA::MetaData & iMeta )
  : m_parent( iParent )
  , m_header( iName, iMeta )
{
    // Check the validity of all inputs.
    ABCA_ASSERT( m_parent, "Invalid parent" );

    ABCA_ASSERT( iName != "" && iName.find('/') == std::string::npos,
                 "Invalid name" );

    // Set the object.
    AbcA::ObjectWriterPtr optr = m_parent->getObject();
    ABCA_ASSERT( optr, "Invalid object" );
    m_object = optr;

    m_data.reset( new CpwData( iName, iParentGroup ) );

    // Write the property header.
    WritePropertyInfo( iParentGroup, m_header, false, 0, 0, 0, 0 );
}

//-*****************************************************************************
CpwImpl::~CpwImpl()
{
    // Nothing!
}

//-*****************************************************************************
const AbcA::PropertyHeader &CpwImpl::getHeader() const
{
    return m_header;
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
                      uint32_t iTimeSamplingIndex )
{
    return m_data->createScalarProperty( asCompoundPtr(), iName, iMetaData,
                                         iDataType, iTimeSamplingIndex );
}

//-*****************************************************************************
AbcA::ArrayPropertyWriterPtr
CpwImpl::createArrayProperty( const std::string & iName,
                              const AbcA::MetaData & iMetaData,
                              const AbcA::DataType & iDataType,
                              uint32_t iTimeSamplingIndex )
{
    return m_data->createArrayProperty( asCompoundPtr(), iName, iMetaData,
                                        iDataType, iTimeSamplingIndex );
}

//-*****************************************************************************
AbcA::CompoundPropertyWriterPtr
CpwImpl::createCompoundProperty( const std::string & iName,
                                 const AbcA::MetaData & iMetaData )
{
    return m_data->createCompoundProperty( asCompoundPtr(), iName, iMetaData );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic

