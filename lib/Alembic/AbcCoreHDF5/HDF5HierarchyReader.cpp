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
#include <Alembic/AbcCoreHDF5/HDF5HierarchyReader.h>
#include <Alembic/AbcCoreHDF5/HDF5Hierarchy.h>
#include <Alembic/AbcCoreHDF5/ReadUtil.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
HDF5HierarchyReader::HDF5HierarchyReader( hid_t iFile,
                                          HDF5Hierarchy& iH5H,
                                          const bool iCacheHierarchy )
    : m_H5H( iH5H )
{
    int enabled( 0 );
    if (iCacheHierarchy && H5Aexists( iFile, "abc_ref_hierarchy" ))
    {
        H5LTget_attribute_int( iFile, ".", "abc_ref_hierarchy", &enabled );
    }

    m_H5H.clear();
    m_H5H.setEnabled( enabled != 0 );

    if( enabled )
    {
        readHierarchy( iFile );
    }

}

//-*****************************************************************************
void HDF5HierarchyReader::readHierarchy( hid_t iFile )
{
    std::vector<hobj_ref_t>     objectRefs;

    std::vector<uint32_t>       childrenSizes;
    std::vector<std::string>    childrenNames;
    std::vector<hobj_ref_t>     childrenRefs;

    std::vector<uint32_t>       attrSizes;
    std::vector<std::string>    attrNames;
    std::vector<char>           hasMask;
    std::vector<uint32_t>       maskBits;
    std::vector<char>           hasMeta;
    std::vector<std::string>    metaStrs;

    ReadReferences( iFile, "object_references", objectRefs );

    // Children
    childrenSizes.resize( objectRefs.size() );
    H5LTget_attribute_uint( iFile, ".", "children_sizes",
                            &childrenSizes.front() );

    ReadReferences( iFile, "children_references", childrenRefs );

    childrenNames.resize( childrenRefs.size() );
    ReadStrings( iFile, "children_names",
                 childrenNames.size(), &childrenNames.front() );

    // Attributes
    attrSizes.resize( objectRefs.size() );
    H5LTget_attribute_uint( iFile, ".", "attr_sizes", &attrSizes.front() );

    size_t totalA(0);
    for( size_t i=0; i < attrSizes.size(); ++i )
        totalA += attrSizes[i];

    attrNames.resize( totalA );
    ReadStrings( iFile, "attr_names", totalA, &attrNames.front() );

    // Masks
    hasMask.resize( totalA );
    H5LTget_attribute_char( iFile, ".", "mask_on", &hasMask.front() );
    size_t totalMask(0);
    for( size_t i = 0; i < hasMask.size(); ++i )
        totalMask += hasMask[i];

    maskBits.resize( totalMask * 6 );
    H5LTget_attribute_uint( iFile, ".", "mask_bits", &maskBits.front() );

    // MetaData
    hasMeta.resize( totalA );
    H5LTget_attribute_char( iFile, ".", "meta_on", &hasMeta.front() );
    size_t totalMeta(0);
    for( size_t i = 0; i < hasMeta.size(); ++i )
        totalMeta += hasMeta[i];

    metaStrs.resize( totalMeta );
    ReadStrings( iFile, "meta_strs", totalMeta, &metaStrs.front() );

    m_H5H.extractFromCompactObjectHierarchy( iFile, objectRefs,
                                             childrenSizes, childrenNames,
                                             childrenRefs,
                                             attrSizes, attrNames,
                                             hasMask, maskBits,
                                             hasMeta, metaStrs );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
