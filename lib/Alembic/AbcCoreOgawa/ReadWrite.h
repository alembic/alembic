//-*****************************************************************************
//
// Copyright (c) 2013-2015,
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

#ifndef Alembic_AbcCoreOgawa_ReadWrite_h
#define Alembic_AbcCoreOgawa_ReadWrite_h

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/Util/Export.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//! Will return a shared pointer to the archive writer
class ALEMBIC_EXPORT WriteArchive
{
public:
    WriteArchive();

    ::Alembic::AbcCoreAbstract::ArchiveWriterPtr
    operator()( const std::string &iFileName,
                const ::Alembic::AbcCoreAbstract::MetaData &iMetaData ) const;

    ::Alembic::AbcCoreAbstract::ArchiveWriterPtr
    operator()( std::ostream * iStream,
                const ::Alembic::AbcCoreAbstract::MetaData &iMetaData ) const;
};

//-*****************************************************************************
//! Will return a shared pointer to the archive reader
//! This version creates a cache associated with the archive.
class ALEMBIC_EXPORT ReadArchive
{
public:
    ReadArchive();

    // Open the file iNumStreams times and manage them internally. If iUseMMap
    // is true, then use memory mapped file I/O, otherwise use file streams.
    ReadArchive( size_t iNumStreams, bool iUseMMap );

    // Read from the provided streams, we do not own these, expect them
    // to remain open and all have the same data in them, and do not try to
    // delete them
    ReadArchive( const std::vector< std::istream * > & iStreams );

    // open the file
    ::Alembic::AbcCoreAbstract::ArchiveReaderPtr
    operator()( const std::string &iFileName ) const;

    // The given cache is ignored.
    ::Alembic::AbcCoreAbstract::ArchiveReaderPtr
    operator()( const std::string &iFileName,
                ::Alembic::AbcCoreAbstract::ReadArraySampleCachePtr iCache
              ) const;

private:
    size_t m_numStreams;
    bool m_useMMap;
    std::vector< std::istream * > m_streams;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif

