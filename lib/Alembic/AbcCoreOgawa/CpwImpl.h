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

#ifndef Alembic_AbcCoreOgawa_CpwImpl_h
#define Alembic_AbcCoreOgawa_CpwImpl_h

#include <Alembic/AbcCoreOgawa/Foundation.h>
#include <Alembic/AbcCoreOgawa/CpwData.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class CpwImpl
    : public AbcA::CompoundPropertyWriter
    , public Alembic::Util::enable_shared_from_this<CpwImpl>
{
public:

    // "top" compound creation called by the object
    CpwImpl( AbcA::ObjectWriterPtr iParent,
             CpwDataPtr iData,
             const AbcA::MetaData & iMeta );

    // child compound creation
    CpwImpl( AbcA::CompoundPropertyWriterPtr iParent,
             Ogawa::OGroupPtr iGroup,
             PropertyHeaderPtr iHeader,
             size_t iIndex );

    virtual ~CpwImpl();

    //-*************************************************************************
    // FROM BasePropertyWriter ABSTRACT
    //-*************************************************************************

    virtual const AbcA::PropertyHeader & getHeader() const;

    virtual AbcA::ObjectWriterPtr getObject();

    virtual AbcA::CompoundPropertyWriterPtr getParent();

    virtual AbcA::CompoundPropertyWriterPtr asCompoundPtr();

    //-*************************************************************************
    // FROM CompoundPropertyWriter ABSTRACT
    //-*************************************************************************

    virtual size_t getNumProperties();

    virtual const AbcA::PropertyHeader & getPropertyHeader( size_t i );

    virtual const AbcA::PropertyHeader *
    getPropertyHeader( const std::string &iName );

    virtual AbcA::BasePropertyWriterPtr
    getProperty( const std::string & iName );

    virtual AbcA::ScalarPropertyWriterPtr
    createScalarProperty( const std::string & iName,
        const AbcA::MetaData & iMetaData,
        const AbcA::DataType & iDataType,
        Util::uint32_t iTimeSamplingIndex );

    virtual AbcA::ArrayPropertyWriterPtr
    createArrayProperty( const std::string & iName,
        const AbcA::MetaData & iMetaData,
        const AbcA::DataType & iDataType,
        Util::uint32_t iTimeSamplingIndex );

    virtual AbcA::CompoundPropertyWriterPtr
    createCompoundProperty( const std::string & iName,
        const AbcA::MetaData & iMetaData );

    void fillHash( size_t iIndex, Util::uint64_t iHash0,
                   Util::uint64_t iHash1 );

private:

    // The object we belong to.
    AbcA::ObjectWriterPtr m_object;

    // The parent compound property writer.
    AbcA::CompoundPropertyWriterPtr m_parent;

    // The header which defines this property.
    PropertyHeaderPtr m_header;

    // child data, this is owned by the object for "top" compounds
    CpwDataPtr m_data;

    // Index position within parent
    size_t m_index;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif
