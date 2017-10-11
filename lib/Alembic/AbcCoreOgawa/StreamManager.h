//-*****************************************************************************
//
// Copyright (c) 2013,
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

#ifndef _Alembic_AbcCoreOgawa_StreamManager_h_
#define _Alembic_AbcCoreOgawa_StreamManager_h_

#include <Alembic/AbcCoreOgawa/Foundation.h>
#include <Alembic/Util/Foundation.h>

#if !defined(ALEMBIC_LIB_USES_TR1) && __cplusplus >= 201103L
#include <atomic>
#endif

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

class StreamID;
typedef Alembic::Util::shared_ptr< StreamID > StreamIDPtr;

//-*****************************************************************************
class StreamManager : Alembic::Util::noncopyable
{
public:
    StreamManager( std::size_t iNumStreams );
    ~StreamManager();
    StreamIDPtr get();
private:
    friend class StreamID;
    void put( std::size_t iStreamID );

    std::size_t m_numStreams;

    // for the locked implementation
    std::vector< std::size_t > m_streamIDs;
    std::size_t m_curStream;
    Alembic::Util::mutex m_lock;

    // for the CAS impl
#if !defined(ALEMBIC_LIB_USES_TR1) && __cplusplus >= 201103L
    std::atomic< Alembic::Util::int64_t > m_streams;
#else
    Alembic::Util::int64_t m_streams;
#endif

    StreamIDPtr m_default;
};

//-*****************************************************************************
class StreamID : Alembic::Util::noncopyable
{
public:
    ~StreamID();
    std::size_t getID() { return m_streamID; }
private:
    friend class StreamManager;
    StreamID( StreamManager * iManager, std::size_t iStreamID );
    StreamManager * m_manager;
    std::size_t m_streamID;
};


} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif
