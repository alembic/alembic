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

#include <Alembic/Core/FileReader.h>
#include <Alembic/Core/Assert.h>
#include <Alembic/Core/ReadUtil.h>
#include <Alembic/Core/Version.h>

namespace Alembic {
namespace Core {

//-*****************************************************************************
FileReader::FileReader( const std::string &fname,
                        const Config &cfg )
  : m_fileName( fname ),
    m_config( cfg )
{
    // Init HDF5
    HDF5::Init();

    // Check existence.
    if ( H5Fis_hdf5( fname.c_str() ) != 1 )
    {
        ABC_CORE_THROW( "FileReader::FileReader: ERROR: "
                        << "File: " << fname
                        << " does not exist or is not an HDF5 file." );
    }
    
    std::string vers;
    try
    {
        // Open the input file.
        m_file.open( m_fileName );

        // Get the version number.
        // This is how you check whether or not it's
        // an HDF5 file.
        ReadString( m_file,
                    "AlembicCoreVersion",
                    vers );

        std::cout << "ALEMBIC CORE VERSION: "
                  << vers << std::endl;
    }
    catch( ... )
    {
        ABC_CORE_THROW( "FileReader::FileReader() ERROR. "
                        << "File: " << m_fileName
                        << " could not be opened, or has no Alembic "
                        << "version info." );
    }
    
    if ( vers != FullVersionString() )
    {
        ABC_CORE_THROW( "FileReader::FileReader() ERROR. "
                        << "File: " << m_fileName
                        << " is probably not an Alembic File, or "
                        << " it is the wrong version. " << std::endl
                        << "Got version: " << vers
                        << ", but expected: " << FullVersionString() );
    }

    // Open root.
    m_rootGroup.open( m_file, "/" );
}

//-*****************************************************************************
FileReader::~FileReader()
{
    if ( m_file.valid() || m_rootGroup.valid() )
    {
        try
        {
            close();
            assert( !m_file.valid() );
            assert( !m_rootGroup.valid() );
        }
        catch ( std::exception &exc )
        {
            std::cerr << "ERROR: Alembic::Core::FileReader::~FileReader() "
                      << "Exception: " << exc.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr << "ERROR: Alembic::Core::FileReader::~FileReader() "
                      << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
void FileReader::close()
{
    ABC_CORE_ASSERT( m_file.valid() && m_rootGroup.valid(),
                     "FileReader::close() ERROR: Invalid file."
                     << std::endl
                     << "File Name; " << m_fileName << std::endl );

    m_rootGroup.close();
    m_file.close();
}

//-*****************************************************************************
SharedFileReader MakeSharedFileReader( const std::string &fname,
                                       const FileReader::Config &cfg )
{
    return boost::make_shared<FileReader>( fname, cfg );
}

} // End namespace Core
} // End namespace Alembic

