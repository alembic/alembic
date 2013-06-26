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

#ifndef _Alembic_AbcCoreOgawa_SpwImpl_h_
#define _Alembic_AbcCoreOgawa_SpwImpl_h_

#include <Alembic/AbcCoreOgawa/Foundation.h>
#include <Alembic/AbcCoreOgawa/WrittenSampleMap.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
// Scalar Property Writer.
class SpwImpl
    : public AbcA::ScalarPropertyWriter
    , public Alembic::Util::enable_shared_from_this<SpwImpl>
{
protected:
    friend class CpwData;

    //-*************************************************************************
    SpwImpl( AbcA::CompoundPropertyWriterPtr iParent,
             Ogawa::OGroupPtr iGroup,
             PropertyHeaderPtr iHeader,
             size_t iIndex );

    AbcA::ScalarPropertyWriterPtr asScalarPtr();

public:
    virtual ~SpwImpl();

    // ScalarPropertyWriter overrides
    virtual void setSample( const void *iSamp );
    virtual void setFromPreviousSample();
    virtual size_t getNumSamples();
    virtual void setTimeSamplingIndex( Util::uint32_t iIndex );

    // BasePropertyWriter overrides
    virtual const AbcA::PropertyHeader & getHeader() const;
    virtual AbcA::ObjectWriterPtr getObject();
    virtual AbcA::CompoundPropertyWriterPtr getParent();

protected:
    // Previous written array sample identifier!
    WrittenSampleIDPtr m_previousWrittenSampleID;

private:
    // The parent compound property writer.
    AbcA::CompoundPropertyWriterPtr m_parent;

    // The header which defines this property.
    // And extra data the parent needs to eventually write out
    PropertyHeaderPtr m_header;

    // for accumulating our hierarchical hash
    Util::Digest m_hash;

    Ogawa::OGroupPtr m_group;

    size_t m_index;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif
