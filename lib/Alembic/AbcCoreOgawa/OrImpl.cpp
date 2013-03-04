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

#include <Alembic/AbcCoreHDF5/OrImpl.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//-*****************************************************************************
// OBJECT READER IMPLEMENTATION
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// Reading as a child of a parent.
OrImpl::OrImpl( AbcA::ObjectReaderPtr iParent,
                H5Node & iParentGroup,
                ObjectHeaderPtr iHeader )
    : m_parent( iParent )
    , m_header( iHeader )
{
    // Check validity of all inputs.
    ABCA_ASSERT( m_parent, "Invalid parent in OrImpl(Object)" );
    ABCA_ASSERT( m_header, "Invalid header in OrImpl(Object)" );

    m_archive = m_parent->getArchive();
    ABCA_ASSERT( m_archive, "Invalid archive in OrImpl(Object)" );

    m_data.reset( new OrData( iHeader, iParentGroup,
        iParent->getArchive()->getArchiveVersion() ) );
}

//-*****************************************************************************
OrImpl::OrImpl( AbcA::ArchiveReaderPtr iArchive,
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

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
