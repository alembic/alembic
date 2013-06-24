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

#ifndef _Alembic_AbcCoreOgawa_AwImpl_h_
#define _Alembic_AbcCoreOgawa_AwImpl_h_

#include <Alembic/AbcCoreOgawa/Foundation.h>
#include <Alembic/AbcCoreOgawa/WrittenSampleMap.h>
#include <Alembic/AbcCoreOgawa/WriteUtil.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class OwData;
class OwImpl;

//-*****************************************************************************
class AwImpl : public AbcA::ArchiveWriter
             , public Alembic::Util::enable_shared_from_this<AwImpl>
{
private:
    friend struct WriteArchive;

    AwImpl( const std::string &iFileName,
            const AbcA::MetaData &iMetaData );

    AwImpl( std::ostream * iStream,
            const AbcA::MetaData & iMetaData );

public:
    virtual ~AwImpl();

    //-*************************************************************************
    // ABSTRACT FUNCTIONS
    //-*************************************************************************
    virtual const std::string &getName() const;

    virtual const AbcA::MetaData &getMetaData() const;

    virtual AbcA::ObjectWriterPtr getTop();

    virtual AbcA::ArchiveWriterPtr asArchivePtr();

    //-*************************************************************************
    // GLOBAL FILE CONTEXT STUFF.
    //-*************************************************************************
    WrittenSampleMap &getWrittenSampleMap()
    {
        return m_writtenSampleMap;
    }

    MetaDataMapPtr getMetaDataMap()
    {
        return m_metaDataMap;
    }

    virtual Util::uint32_t addTimeSampling( const AbcA::TimeSampling & iTs );

    virtual AbcA::TimeSamplingPtr getTimeSampling( Util::uint32_t iIndex );

    virtual Util::uint32_t getNumTimeSamplings()
    { return m_timeSamples.size(); }

    virtual AbcA::index_t getMaxNumSamplesForTimeSamplingIndex(
        Util::uint32_t iIndex );

    virtual void setMaxNumSamplesForTimeSamplingIndex( Util::uint32_t iIndex,
                                                      AbcA::index_t iMaxIndex );

private:
    void init();
    std::string m_fileName;
    AbcA::MetaData m_metaData;
    Alembic::Ogawa::OArchive m_archive;

    Alembic::Util::weak_ptr< AbcA::ObjectWriter > m_top;
    Alembic::Util::shared_ptr < OwData > m_data;

    std::vector < AbcA::TimeSamplingPtr > m_timeSamples;

    std::vector < AbcA::index_t > m_maxSamples;

    WrittenSampleMap m_writtenSampleMap;
    MetaDataMapPtr m_metaDataMap;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif
