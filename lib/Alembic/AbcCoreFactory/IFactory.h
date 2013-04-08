//-*****************************************************************************
//
// Copyright (c) 2013,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#ifndef _Alembic_AbcCoreFactory_IFactory_h_
#define _Alembic_AbcCoreFactory_IFactory_h_

#include <Alembic/AbcCoreAbstract/ReadArraySampleCache.h>
#include <Alembic/Abc/IArchive.h>

namespace Alembic {
namespace AbcCoreFactory {
namespace ALEMBIC_VERSION_NS {

class IFactory
{
public:
    IFactory();
    ~IFactory();

    //! The type which yielded a valid IArchive (or kUnknown if invalid)
    enum CoreType
    {
        kHDF5,
        kOgawa,
        kUnknown
    };

    //! Try to open a file and set oType to the one that yields a successful
    //! oType, or kUnknown if the IArchive isn't valid
    Alembic::Abc::IArchive getArchive( const std::string & iFileName,
                                       CoreType & oType );

    //! Try to open a file and return IArchive.  If the file wasn't a valid
    //! file or known type and invalid archive is returned.
    Alembic::Abc::IArchive getArchive( const std::string & iFileName );

    //! Use the streams (Alembic does not take ownership) to read the data from
    //! This is currently only valid for Ogawa.  The streams must all reference
    //! the same data.
    Alembic::Abc::IArchive getArchive(
        const std::vector< std::istream * > & iStreams, CoreType & oType );

    //! If opening an HDF5 file, sets whether to use the cached hierarchy
    //! if it exists, the default value is true
    void setHDF5CacheHierarchy( bool iCacheHierarchy )
    {
        m_cacheHierarchy = iCacheHierarchy;
    }

    //! Gets whether an HDF5 file will use the cached hierarchy
    bool getHDF5CacheHierarchy() const { return m_cacheHierarchy; }

    //! Set the array sample cache, the HDF5 implementation optionally uses this
    void setSampleCache(
        Alembic::AbcCoreAbstract::ReadArraySampleCachePtr iCachePtr )
    {
        m_cachePtr = iCachePtr;
    }

    //! Get the array sample cache
    Alembic::AbcCoreAbstract::ReadArraySampleCachePtr getSampleCache() const
    {
        return m_cachePtr;
    }

    //! Gets the number of streams that will be opened when opening an Ogawa
    //! file
    size_t getOgawaNumStreams() const { return m_numStreams; }

    //! Sets the number of streams that will be opened when opening an Ogawa
    //! file, the default is 1
    void setOgawaNumStreams( size_t iNumStreams )
    {
        m_numStreams = iNumStreams;
    }

    //! Gets the error handler policy
    Alembic::Abc::ErrorHandler::Policy getPolicy() { return m_policy; }

    //! Sets the error handler policy, the default is kThrowPolicy
    void setPolicy( Alembic::Abc::ErrorHandler::Policy iPolicy )
    {
        m_policy = iPolicy;
    }

private:
    bool m_cacheHierarchy;
    size_t m_numStreams;
    Alembic::AbcCoreAbstract::ReadArraySampleCachePtr m_cachePtr;
    Alembic::Abc::ErrorHandler::Policy m_policy;

};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Abc
} // End namespace Alembic

#endif
