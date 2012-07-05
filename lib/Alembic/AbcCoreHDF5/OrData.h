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

#ifndef _Alembic_AbcCoreHDF5_OrData_h_
#define _Alembic_AbcCoreHDF5_OrData_h_

#include <Alembic/AbcCoreHDF5/HDF5Hierarchy.h>
#include <Alembic/AbcCoreHDF5/Foundation.h>


namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

class CprData;

// data class owned by OrImpl, or ArImpl if it is a "top" object.
// it owns and makes child objects as well as the group hid_t
// when necessary
class OrData : public Alembic::Util::enable_shared_from_this<OrData>
{
public:
    OrData( ObjectHeaderPtr iHeader,
            H5Node & iParentGroup,
            int32_t iArchiveVersion );

    ~OrData();

    AbcA::CompoundPropertyReaderPtr
    getProperties( AbcA::ObjectReaderPtr iParent );

    size_t getNumChildren();

    const AbcA::ObjectHeader &
    getChildHeader( AbcA::ObjectReaderPtr iParent, size_t i );

    const AbcA::ObjectHeader *
    getChildHeader( AbcA::ObjectReaderPtr, const std::string &iName );

    AbcA::ObjectReaderPtr
    getChild( AbcA::ObjectReaderPtr iParent, const std::string &iName );

    AbcA::ObjectReaderPtr
    getChild( AbcA::ObjectReaderPtr iParent, size_t i );

private:

    struct Child
    {
        bool loadedMetaData;
        ObjectHeaderPtr header;
        WeakOrPtr made;
    };

    typedef std::map<std::string, size_t> ChildrenMap;
    typedef std::vector<Child> ChildrenVec;

    H5Node m_group;
    H5Node m_oldGroup;

    Alembic::Util::mutex m_childObjectsMutex;

    // The children
    ChildrenVec m_children;
    ChildrenMap m_childrenMap;

    // Our "top" property.
    Alembic::Util::weak_ptr< AbcA::CompoundPropertyReader > m_top;
    Alembic::Util::shared_ptr < CprData > m_data;
};

typedef Alembic::Util::shared_ptr<OrData> OrDataPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreHDF5
} // End namespace Alembic

#endif
