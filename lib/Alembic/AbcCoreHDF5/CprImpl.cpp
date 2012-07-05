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

#include <Alembic/AbcCoreHDF5/CprImpl.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//-*****************************************************************************
// CLASS
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
CprImpl::CprImpl( AbcA::CompoundPropertyReaderPtr iParent,
                  H5Node & iParentGroup,
                  PropertyHeaderPtr iHeader )
    : m_parent( iParent )
    , m_header( iHeader )
{
    ABCA_ASSERT( m_parent, "Invalid parent in CprImpl(Compound)" );
    ABCA_ASSERT( m_header, "invalid header in CprImpl(Compound)" );

    AbcA::PropertyType pType = m_header->getPropertyType();
    if ( pType != AbcA::kCompoundProperty )
    {
        ABCA_THROW( "Tried to create compound property with the wrong "
                    "property type: " << pType );
    }

    // Set object.
    AbcA::ObjectReaderPtr optr = m_parent->getObject();
    ABCA_ASSERT( optr, "Invalid object in CprImpl::CprImpl(Compound)" );
    m_object = optr;

    m_data.reset( new CprData( iParentGroup,
        m_parent->getObject()->getArchive()->getArchiveVersion(),
        m_header->getName() ) );
}

//-*****************************************************************************
CprImpl::CprImpl( AbcA::ObjectReaderPtr iObject,
                  CprDataPtr iData )
    : m_object( iObject )
    , m_data( iData )
{
    ABCA_ASSERT( m_object, "Invalid object in CprImpl(Object)" );
    ABCA_ASSERT( m_data, "Invalid data in CprImpl(Object)" );

    m_header.reset( new AbcA::PropertyHeader( "",  m_object->getMetaData() ) );
}

//-*****************************************************************************
CprImpl::~CprImpl()
{
    // Nothing
}

//-*****************************************************************************
const AbcA::PropertyHeader &CprImpl::getHeader() const
{
    ABCA_ASSERT( m_header, "Invalid header" );
    return *m_header;
}

AbcA::ObjectReaderPtr CprImpl::getObject()
{
    return m_object;
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr CprImpl::getParent()
{
    return m_parent;
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr CprImpl::asCompoundPtr()
{
    return shared_from_this();
}

//-*****************************************************************************
size_t CprImpl::getNumProperties()
{
    return m_data->getNumProperties();
}

//-*****************************************************************************
const AbcA::PropertyHeader & CprImpl::getPropertyHeader( size_t i )
{
    return m_data->getPropertyHeader( asCompoundPtr(), i );
}

//-*****************************************************************************
const AbcA::PropertyHeader *
CprImpl::getPropertyHeader( const std::string &iName )
{
    return m_data->getPropertyHeader( asCompoundPtr(), iName );
}

//-*****************************************************************************
AbcA::ScalarPropertyReaderPtr
CprImpl::getScalarProperty( const std::string &iName )
{
    return m_data->getScalarProperty( asCompoundPtr(), iName );
}

//-*****************************************************************************
AbcA::ArrayPropertyReaderPtr
CprImpl::getArrayProperty( const std::string &iName )
{
    return m_data->getArrayProperty( asCompoundPtr(), iName );
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr
CprImpl::getCompoundProperty( const std::string &iName )
{
    return m_data->getCompoundProperty( asCompoundPtr(), iName );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
