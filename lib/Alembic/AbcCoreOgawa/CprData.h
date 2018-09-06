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

#ifndef Alembic_AbcCoreOgawa_CprData_h
#define Alembic_AbcCoreOgawa_CprData_h

#include <Alembic/AbcCoreOgawa/Foundation.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

// data class owned by CprImpl, or OrImpl if it is a "top" object
// it owns and makes child properties
class CprData : public Alembic::Util::enable_shared_from_this<CprData>
{
public:

    CprData( Ogawa::IGroupPtr iGroup,
             std::size_t iThreadId,
             AbcA::ArchiveReader & iArchive,
             const std::vector< AbcA::MetaData > & iIndexedMetaData );

    ~CprData();

    size_t getNumProperties();

    const AbcA::PropertyHeader &
    getPropertyHeader( AbcA::CompoundPropertyReaderPtr iParent, size_t i );

    const AbcA::PropertyHeader *
    getPropertyHeader( AbcA::CompoundPropertyReaderPtr iParent,
                       const std::string &iName );

    AbcA::ScalarPropertyReaderPtr
    getScalarProperty( AbcA::CompoundPropertyReaderPtr iParent,
                       const std::string &iName );

    AbcA::ArrayPropertyReaderPtr
    getArrayProperty( AbcA::CompoundPropertyReaderPtr iParent,
                      const std::string &iName );

    AbcA::CompoundPropertyReaderPtr
    getCompoundProperty( AbcA::CompoundPropertyReaderPtr iParent,
                         const std::string &iName );

private:
    Ogawa::IGroupPtr m_group;

    // Property Headers and Made Property Pointers.
    struct SubProperty
    {
        PropertyHeaderPtr header;
        WeakBprPtr made;
        Alembic::Util::mutex lock;
    };

    typedef std::map<std::string, size_t> SubPropertiesMap;

    SubProperty * m_propertyHeaders;
    SubPropertiesMap m_subProperties;
};

typedef Alembic::Util::shared_ptr<CprData> CprDataPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif
