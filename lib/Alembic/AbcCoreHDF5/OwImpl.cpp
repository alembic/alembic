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

#include <Alembic/AbcCoreHDF5/OwImpl.h>
#include <Alembic/AbcCoreHDF5/AwImpl.h>
#include <Alembic/AbcCoreHDF5/CpwImpl.h>
#include <Alembic/AbcCoreHDF5/WriteUtil.h>
#include <Alembic/AbcCoreHDF5/HDF5Util.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

OwImpl::OwImpl( AbcA::ArchiveWriterPtr iArchive,
                OwDataPtr iData,
                const AbcA::MetaData & iMetaData )
    : m_archive( iArchive )
    , m_header( new AbcA::ObjectHeader( "ABC", "/", iMetaData ) )
    , m_data( iData )
{
    ABCA_ASSERT( m_archive, "Invalid archive" );
    ABCA_ASSERT( m_data, "Invalid data" );
}

//-*****************************************************************************
OwImpl::OwImpl( AbcA::ObjectWriterPtr iParent,
                hid_t iParentGroup,
                ObjectHeaderPtr iHeader )
  : m_parent( iParent )
  , m_header( iHeader )
{
    // Check validity of all inputs.
    ABCA_ASSERT( m_parent, "Invalid parent" );
    ABCA_ASSERT( m_header, "Invalid header" );

    m_archive = m_parent->getArchive();
    ABCA_ASSERT( m_archive, "Invalid archive" );

    m_data.reset( new OwData( iParentGroup, m_header->getName(),
                              m_header->getMetaData() ) );
}

//-*****************************************************************************
OwImpl::~OwImpl()
{
    // Nothing!!
}

//-*****************************************************************************
const AbcA::ObjectHeader & OwImpl::getHeader() const
{
    ABCA_ASSERT( m_header, "Invalid header" );
    return *m_header;
}

//-*****************************************************************************
AbcA::ArchiveWriterPtr OwImpl::getArchive()
{
    return m_archive;
}

//-*****************************************************************************
AbcA::ObjectWriterPtr OwImpl::getParent()
{
    return m_parent;
}

//-*****************************************************************************
AbcA::CompoundPropertyWriterPtr OwImpl::getProperties()
{
    return m_data->getProperties( asObjectPtr() );
}

//-*****************************************************************************
size_t OwImpl::getNumChildren()
{
    return m_data->getNumChildren();
}

//-*****************************************************************************
const AbcA::ObjectHeader & OwImpl::getChildHeader( size_t i )
{
    return m_data->getChildHeader( i );
}

const AbcA::ObjectHeader * OwImpl::getChildHeader( const std::string &iName )
{
    return m_data->getChildHeader( iName );
}

//-*****************************************************************************
AbcA::ObjectWriterPtr OwImpl::getChild( const std::string &iName )
{
    return m_data->getChild( iName );
}

//-*****************************************************************************
AbcA::ObjectWriterPtr OwImpl::createChild( const AbcA::ObjectHeader &iHeader )
{
    return m_data->createChild( asObjectPtr(), m_header->getFullName(),
                                iHeader );
}

//-*****************************************************************************
AbcA::ObjectWriterPtr OwImpl::asObjectPtr()
{
    return shared_from_this();
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
