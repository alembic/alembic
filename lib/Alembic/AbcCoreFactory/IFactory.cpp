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

#include <fstream>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreLayer/Read.h>
#include <Alembic/AbcCoreFactory/IFactory.h>

#ifdef ALEMBIC_WITH_HDF5
#include <Alembic/AbcCoreHDF5/All.h>
#endif

namespace Alembic {
namespace AbcCoreFactory {
namespace ALEMBIC_VERSION_NS {

IFactory::IFactory()
{
    m_cacheHierarchy = true;
    m_numStreams = 1;
    m_policy = Alembic::Abc::ErrorHandler::kThrowPolicy;
}

IFactory::~IFactory()
{
}

Alembic::Abc::IArchive IFactory::getArchive( const std::string & iFileName,
                                            CoreType & oType )
{

    // try Ogawa first, use kQuietNoop at first in case we fail
    Alembic::AbcCoreOgawa::ReadArchive ogawa( m_numStreams );
    Alembic::Abc::IArchive archive( ogawa, iFileName,
        Alembic::Abc::ErrorHandler::kQuietNoopPolicy, m_cachePtr );

    if ( archive.valid() )
    {
        oType = kOgawa;
        archive.getErrorHandler().setPolicy( m_policy );
        return archive;
    }

#ifdef ALEMBIC_WITH_HDF5
    Alembic::AbcCoreHDF5::ReadArchive hdf( m_cacheHierarchy );
    archive = Alembic::Abc::IArchive( hdf, iFileName,
        Alembic::Abc::ErrorHandler::kQuietNoopPolicy, m_cachePtr );
    if ( archive.valid() )
    {
        oType = kHDF5;
        archive.getErrorHandler().setPolicy( m_policy );
        return archive;
    }
#else
    // check the first 8 bytes to see if this is an HDF5 file according to
    // www.hdfgroup.org/HDF5/doc/H5.format.html#Superblock
    std::ifstream filestream;
    filestream.open(iFileName.c_str(), std::ios::binary);

    if (filestream.is_open())
    {
        char bf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        filestream.read(bf, 8);
        filestream.close();

        if (bf[0] == '\211' && bf[1] == 'H' && bf[2] == 'D' && bf[3] == 'F' &&
            bf[4] == '\r' && bf[5] == '\n' && bf[6] == '\032' && bf[7] == '\n')
        {
            oType = kHDF5;
            return Alembic::Abc::IArchive();
        }
    }
#endif

    oType = kUnknown;
    return Alembic::Abc::IArchive();
}

Alembic::Abc::IArchive IFactory::getArchive( const std::string & iFileName )
{
    CoreType coreType;
    return getArchive( iFileName, coreType );
}

Alembic::Abc::IArchive IFactory::getArchive(
    const std::vector< std::string > & iFileNames)
{
    CoreType coreType;
    return getArchive(iFileNames, coreType);
}

Alembic::Abc::IArchive IFactory::getArchive(
    const std::vector< std::string > & iFileNames, CoreType & oType )
{
    Alembic::AbcCoreLayer::ReadArchive layer;

    Alembic::AbcCoreLayer::ArchiveReaderPtrs archives;

    // first read our archives, skipping over bad ones
    std::vector< std::string >::const_iterator it = iFileNames.begin();
    for ( ; it != iFileNames.end(); ++it )
    {
        Alembic::Abc::IArchive archive = getArchive( *it );
        if ( archive.getPtr() )
        {
            archives.push_back( archive.getPtr() );
        }
    }


    if ( ! archives.empty() )
    {
        Alembic::AbcCoreAbstract::ArchiveReaderPtr arPtr = layer( archives );
        oType = kLayer;
        return Alembic::Abc::IArchive( arPtr, Alembic::Abc::kWrapExisting,
                                       m_policy );
    }

    // no valid archives pushed, so invalid
    oType = kUnknown;
    return Alembic::Abc::IArchive();
}

Alembic::Abc::IArchive IFactory::getArchive(
    const std::vector< std::istream * > & iStreams, CoreType & oType)
{
    // Ogawa is the only one which can do this
    Alembic::AbcCoreOgawa::ReadArchive ogawa( iStreams );
    Alembic::Abc::IArchive archive( ogawa, "", m_policy, m_cachePtr );
    if ( archive.valid() )
    {
        oType = kOgawa;
        return archive;
    }

    oType = kUnknown;
    return Alembic::Abc::IArchive();
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreFactory
} // End namespace Alembic
