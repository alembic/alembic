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

#ifndef _Alembic_Core_FileReader_h_
#define _Alembic_Core_FileReader_h_

#include <Alembic/Core/Foundation.h>

namespace Alembic {
namespace Core {

//-*****************************************************************************
// Forward declaration
class PageWriter;
typedef boost::shared_ptr<PageWriter> SharedPageWriter;

//-*****************************************************************************
class FileReader : private boost::noncopyable
{
public:
    struct Config
    {
        Config() : dummy( 0 ) {}
        int dummy;
    };

    FileReader( const std::string &fileName,
                const Config &config = Config() );

    // Destructor is not virtual, because this class is not
    // intended to be inherited from.
    ~FileReader();

    const std::string &fileName() const { return m_fileName; }

    // Check validity();
    bool valid() const
    { return ( m_file.valid() && m_rootGroup.valid() ); }

    // Close the file. This happens on destruction as well.
    void close();

protected:
    friend class PageReader;

    // File access
    H5F &file() { return m_file; }
    const H5F &file() const { return m_file; }

    // Root group access
    H5G &rootGroup() { return m_rootGroup; }
    const H5G &rootGroup() const { return m_rootGroup; }

private:
    std::string m_fileName;
    Config m_config;
    H5F m_file;
    H5G m_rootGroup;
};

//-*****************************************************************************
typedef boost::shared_ptr<FileReader> SharedFileReader;

//-*****************************************************************************
// Not inlined, because it would be a useful "action".
SharedFileReader MakeSharedFileReader(
    const std::string &fileName,
    const FileReader::Config &cfg = FileReader::Config() );


} // End namespace Core
} // End namespace Alembic

#endif
