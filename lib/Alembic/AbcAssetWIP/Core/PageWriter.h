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

#ifndef _Alembic_Core_PageWriter_h_
#define _Alembic_Core_PageWriter_h_

#include <Alembic/Core/Foundation.h>
#include <Alembic/Core/FileWriter.h>

namespace Alembic {
namespace Core {

//-*****************************************************************************
// For now, at least.
typedef std::string WriteMetaData;

//-*****************************************************************************
// Forwards.
class PageWriter;
typedef boost::shared_ptr<PageWriter> SharedPageWriter;

//-*****************************************************************************
// Pages are just groups with metadata.
// Metadata is written with "MetaDataWriter" class.
class PageWriter : private boost::noncopyable
{
public:    
    // Page Writers need to be initialized from other page writers.
    PageWriter( SharedPageWriter parentPage,
                const std::string &name );
    
    // The "root" page writer.
    explicit PageWriter( SharedFileWriter top );

    // The Destructor. Not virtual because this is not intended
    // for use as a base class.
    ~PageWriter();

    // This function returns the name.
    const std::string &name() const { return m_name; }
    
    // This function returns the full path.
    const std::string &fullPath() const { return m_fullPath; }

    // Returns whether metadata has been written.
    bool metaDataWritten() const { return m_metaDataWritten; }

    // Write meta data. Can only be done once. Will throw
    // an exception if multiple writes are attempted.
    void writeMetaData( const WriteMetaData &md );

    // Check validity
    bool valid() const
    { return m_group.valid(); }

    // The close function just... closes it!
    void close();

protected:
    SharedFileWriter file() { return m_file; }
    H5G &group() { return m_group; }
    const H5G &group() const { return m_group; }

private:
    // init function
    void init( H5G &parentGroup );
    
    // These upward links prevent the parent resources
    // from being deleted. They don't, unfortunately, yet
    // prevent upwind premature closures.
    SharedPageWriter m_parentPage;
    SharedFileWriter m_file;
    
    H5G m_group;
    std::string m_name;
    std::string m_fullPath;
    bool m_metaDataWritten;
};

//-*****************************************************************************
SharedPageWriter MakeSharedPageWriter( SharedPageWriter parent,
                                       const std::string &pageName );

SharedPageWriter MakeSharedPageWriter( SharedFileWriter file );

} // End namespace Core
} // End namespace Alembic


#endif
