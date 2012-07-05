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
#include <Alembic/AbcCoreHDF5/HDF5HierarchyWriter.h>
#include <Alembic/AbcCoreHDF5/WriteUtil.h>
#include <Alembic/AbcCoreHDF5/HDF5Hierarchy.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
HDF5HierarchyWriter::HDF5HierarchyWriter( hid_t iFile, HDF5Hierarchy& iH5H )
    : m_H5H( iH5H )
{
    m_H5H.build( iFile );

    writeHierarchy( iFile );

    // Hierarchy info is succcessfully saved.
    // Let's set a flag.
    //
    int enabled = 1;
    H5LTset_attribute_int( iFile, ".", "abc_ref_hierarchy", &enabled, 1 );
}

//-*****************************************************************************
void HDF5HierarchyWriter::writeHierarchy(hid_t iFile )
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
 
    m_H5H.makeCompactObjectHierarchy( objectRefs,
                                      childrenSizes, childrenNames,
                                      childrenRefs,
                                      attrSizes, attrNames,
                                      hasMask, maskBits,
                                      hasMeta, metaStrs );

    WriteReferences( iFile, "object_references",
                     objectRefs.size(), &objectRefs.front() );

    // Children
    H5LTset_attribute_uint( iFile, ".", "children_sizes",
                            &childrenSizes.front(), childrenSizes.size() );

    WriteStrings( iFile, "children_names",
                  childrenNames.size(), &childrenNames.front() );

    WriteReferences( iFile, "children_references",
                     childrenRefs.size(), &childrenRefs.front() );

    // Attributes
    H5LTset_attribute_uint( iFile , ".", "attr_sizes",
                            &attrSizes.front(), attrSizes.size() );

    WriteStrings( iFile, "attr_names",
                  attrNames.size(), &attrNames.front() );

    // Masks
    H5LTset_attribute_char( iFile, ".", "mask_on",
                            &hasMask.front(), hasMask.size() );
    H5LTset_attribute_uint( iFile, ".", "mask_bits",
                            &maskBits.front(), maskBits.size() );

    // Metadata
    H5LTset_attribute_char( iFile, ".", "meta_on",
                            &hasMeta.front(), hasMeta.size() );
    WriteStrings( iFile, "meta_strs",
                  metaStrs.size(), &metaStrs.front() );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
