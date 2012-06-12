//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#ifndef _Alembic_AbcCoreHDF5_CpwData_h_
#define _Alembic_AbcCoreHDF5_CpwData_h_

#include <Alembic/AbcCoreHDF5/Foundation.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

// data class owned by CpwImpl, or OwImpl if it is a "top" object
// it owns and makes child properties as well as the group hid_t
// when necessary
class CpwData : public Alembic::Util::enable_shared_from_this<CpwData>
{
public:

    CpwData( const std::string & iName, hid_t iParentGroup );

    ~CpwData();

    size_t getNumProperties();

    const AbcA::PropertyHeader & getPropertyHeader( size_t i );

    const AbcA::PropertyHeader * getPropertyHeader( const std::string &iName );

    AbcA::BasePropertyWriterPtr getProperty( const std::string & iName );

    AbcA::ScalarPropertyWriterPtr
    createScalarProperty( AbcA::CompoundPropertyWriterPtr iParent,
        const std::string & iName,
        const AbcA::MetaData & iMetaData,
        const AbcA::DataType & iDataType,
        uint32_t iTimeSamplingIndex );

    AbcA::ArrayPropertyWriterPtr
    createArrayProperty( AbcA::CompoundPropertyWriterPtr iParent,
        const std::string & iName,
        const AbcA::MetaData & iMetaData,
        const AbcA::DataType & iDataType,
        uint32_t iTimeSamplingIndex );

    AbcA::CompoundPropertyWriterPtr
    createCompoundProperty( AbcA::CompoundPropertyWriterPtr iParent,
        const std::string & iName,
        const AbcA::MetaData & iMetaData );

private:

    hid_t getGroup();

    // The parent group. We need to keep this around because we
    // don't create our group until we need to. This is guaranteed to
    // exist because our parent (or object) is guaranteed to exist.
    hid_t m_parentGroup;

    // The group corresponding to this property.
    // It may never be created or written.
    hid_t m_group;

    // if m_group gets created it will be given this name
    std::string m_name;

    typedef std::map<std::string, WeakBpwPtr> MadeProperties;

    PropertyHeaderPtrs m_propertyHeaders;
    MadeProperties m_madeProperties;
};

typedef Alembic::Util::shared_ptr<CpwData> CpwDataPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreHDF5
} // End namespace Alembic

#endif
