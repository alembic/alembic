//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#include <Alembic/Core/PageWriter.h>
#include <Alembic/Core/WriteUtil.h>
#include <Alembic/Core/Assert.h>

namespace Alembic {
namespace Core {

//-*****************************************************************************
PageWriter::PageWriter( SharedPageWriter prnt,
                        const std::string &pageName )
  : m_parentPage( prnt ),
    m_name( pageName ),
    m_metaDataWritten( false )
{
    // Check validity of inputs.
    ABC_CORE_ASSERT( (( bool )m_parentPage) &&
                     m_parentPage->valid(),
                     "PageWriter::PageWriter passed invalid page parent." );

    // Set file and full path
    m_file = m_parentPage->file();
    m_fullPath = m_parentPage->fullPath() + "/" + pageName;

    // Init
    init( m_parentPage->group() );
}

//-*****************************************************************************
PageWriter::PageWriter( SharedFileWriter theFile )
  : m_parentPage(),
    m_file( theFile ),
    m_name( "Alembic" ),
    m_metaDataWritten( false )
{
    // Check validity of inputs.
    ABC_CORE_ASSERT( (( bool )m_file) &&
                     m_file->valid(),
                     "PageWriter::PageWriter passed invalid file parent." );

    // Set full path
    m_fullPath = "/Alembic";

    // Init.
    init( m_file->rootGroup() );
}

//-*****************************************************************************
PageWriter::~PageWriter()
{
    if ( m_group.valid() )
    {
        try
        {
            close();
        }
        catch ( std::exception &exc )
        {
            std::cerr << "ERROR: Alembic::Core::PageWriter::~PageWriter() "
                      << "Exception: " << exc.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr << "ERROR: Alembic::Core::PageWriter::~PageWriter() "
                      << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
void PageWriter::writeMetaData( const WriteMetaData &meta )
{
    ABC_CORE_ASSERT( !m_metaDataWritten,
                     "PageWriter::writeMetaData() called multiple times. "
                     << std::endl
                     << "Can only write metaData once. " << std::endl
                     << "Bad metadata: " << meta );

    ABC_CORE_ASSERT( m_group.valid(),
                     "PageWriter::writeMetaData() ERROR: Invalid Page."
                     << std::endl
                     << "Page name: " << m_name << std::endl );

    WriteString( m_group, "PageMetaData", meta );

    m_metaDataWritten = true;
}

//-*****************************************************************************
void PageWriter::close()
{
    ABC_CORE_ASSERT( m_group.valid(),
                     "PageWriter::close() ERROR: Invalid group."
                     << std::endl
                     << "Page name: " << m_name << std::endl );

    // Before we leave, if we haven't written metadata, output blank
    // metadata.
    if ( !m_metaDataWritten )
    {
        writeMetaData( "ALEMBIC_CORE_EMPTY_METADATA" );
    }

    m_group.close();
    assert( !m_group.valid() );

    m_parentPage.reset();
    m_file.reset();
}   

//-*****************************************************************************
void PageWriter::init( H5G &parentGroup )
{
    // Create the base group of this page.
    HDF5::CreationOrderPlist copl;
    m_group.create( parentGroup, m_name, H5P_DEFAULT, copl );

    // Write an identifier so that we know this is an alembic core page.
    // This is in addition to the eventual metadata.
    WriteString( m_group,
                 "AlembicCoreType",
                 "Page" );
}


//-*****************************************************************************
SharedPageWriter MakeSharedPageWriter( SharedPageWriter parent,
                                       const std::string &pageName )
{
    return boost::make_shared<PageWriter>( parent, pageName );
}

SharedPageWriter MakeSharedPageWriter( SharedFileWriter file )
{
    return boost::make_shared<PageWriter>( file );
}

} // End namespace Core
} // End namespace Alembic
