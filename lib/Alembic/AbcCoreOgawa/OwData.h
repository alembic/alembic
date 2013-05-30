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

#ifndef _Alembic_AbcCoreOgawa_OwData_h_
#define _Alembic_AbcCoreOgawa_OwData_h_

#include <Alembic/AbcCoreOgawa/Foundation.h>
#include <Alembic/AbcCoreOgawa/MetaDataMap.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
// Forwards
class CpwData;

// data class owned by OwImpl, or AwImpl if it is a "top" object.
// it owns and makes child properties
class OwData : public Alembic::Util::enable_shared_from_this<OwData>
{
public:
    OwData( Ogawa::OGroupPtr iGroup );

    ~OwData();

    AbcA::CompoundPropertyWriterPtr getProperties(
        AbcA::ObjectWriterPtr iParent );

    size_t getNumChildren();

    const AbcA::ObjectHeader & getChildHeader( size_t i );

    const AbcA::ObjectHeader *
    getChildHeader( const std::string &iName );

    AbcA::ObjectWriterPtr getChild( const std::string &iName );

    AbcA::ObjectWriterPtr createChild( AbcA::ObjectWriterPtr iParent,
                                       const std::string & iFullName,
                                       const AbcA::ObjectHeader &iHeader );

    Ogawa::OGroupPtr getGroup();

    void writeHeaders( MetaDataMapPtr iMetaDataMap, Util::SpookyHash & ioHash );

    void fillHash( std::size_t iIndex, Util::uint64_t iHash0,
                   Util::uint64_t iHash1 );

private:

    // The group corresponding to the object
    Ogawa::OGroupPtr m_group;

    typedef std::vector<ObjectHeaderPtr> ChildHeaders;
    typedef std::map<std::string,WeakOwPtr> MadeChildren;

    // The children
    ChildHeaders m_childHeaders;
    MadeChildren m_madeChildren;

    Alembic::Util::weak_ptr< AbcA::CompoundPropertyWriter > m_top;

    // Our "top" property
    Alembic::Util::shared_ptr < CpwData > m_data;

    // child hashes
    std::vector< Util::uint64_t > m_hashes;
};

typedef Alembic::Util::shared_ptr<OwData> OwDataPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif
