//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#ifndef _Alembic_AbcCoreHDF5_CprData_h_
#define _Alembic_AbcCoreHDF5_CprData_h_

#include <Alembic/AbcCoreHDF5/Foundation.h>
#include <Alembic/AbcCoreHDF5/HDF5Hierarchy.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

// data class owned by CprImpl, or OrImpl if it is a "top" object
// it owns and makes child properties as well as the group hid_t
// when necessary
class CprData : public Alembic::Util::enable_shared_from_this<CprData>
{
public:

    CprData( H5Node & iParentGroup, int32_t iArchiveVersion,
             const std::string &iName );

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
    // My group.
    H5Node m_group;

    // Property Headers and Made Property Pointers.
    struct SubProperty
    {
        PropertyHeaderPtr header;

        // extra data that doesn't quite fit into the property header
        // but is stuff we only want to read once
        uint32_t numSamples;
        uint32_t firstChangedIndex;
        uint32_t lastChangedIndex;
        bool isScalarLike;

        WeakBprPtr made;
        std::string name;
    };

    typedef std::map<std::string, size_t> SubPropertiesMap;
    typedef std::vector<SubProperty> SubPropertyVec;

    // Allocated mutexes, one per SubProperty
    Alembic::Util::mutex * m_subPropertyMutexes;
    SubPropertyVec m_propertyHeaders;
    SubPropertiesMap m_subProperties;
};

typedef Alembic::Util::shared_ptr<CprData> CprDataPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreHDF5
} // End namespace Alembic

#endif
