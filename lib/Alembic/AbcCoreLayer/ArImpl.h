//-*****************************************************************************
//
// Copyright (c) 2016,
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

#ifndef Alembic_AbcCoreLayer_ArImpl_h
#define Alembic_AbcCoreLayer_ArImpl_h

#include <Alembic/AbcCoreLayer/Foundation.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//class OrData;

//-*****************************************************************************
class ArImpl
    : public AbcA::ArchiveReader
    , public Alembic::Util::enable_shared_from_this<ArImpl>
{
private:
    friend class ReadArchive;

    ArImpl( ArchiveReaderPtrs & iArchives );


public:

    virtual ~ArImpl();

    //-*************************************************************************
    // ABSTRACT FUNCTIONS
    //-*************************************************************************
    virtual const std::string &getName() const;

    virtual const AbcA::MetaData &getMetaData() const;

    virtual AbcA::ObjectReaderPtr getTop();

    virtual AbcA::TimeSamplingPtr getTimeSampling( Util::uint32_t iIndex );

    virtual AbcA::ArchiveReaderPtr asArchivePtr();

    virtual AbcA::ReadArraySampleCachePtr getReadArraySampleCachePtr();

    virtual void setReadArraySampleCachePtr( AbcA::ReadArraySampleCachePtr iPtr );

    virtual AbcA::index_t getMaxNumSamplesForTimeSamplingIndex(
        Util::uint32_t iIndex );

    virtual Util::uint32_t getNumTimeSamplings();

    virtual Util::int32_t getArchiveVersion();

private:
    std::string m_fileName;

    ArchiveReaderPtrs m_archives;

    Alembic::Util::weak_ptr< AbcA::ObjectReader > m_top;
    Alembic::Util::mutex m_lock;

    std::vector <  AbcA::TimeSamplingPtr > m_timeSamples;
    std::vector <  AbcA::index_t > m_maxSamples;
    ObjectHeaderPtr m_header;

    Util::int32_t m_archiveVersion;

};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreLayer
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_ArImpl_h_
