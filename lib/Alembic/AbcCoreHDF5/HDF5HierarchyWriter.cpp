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
#include <Alembic/AbcCoreHDF5/WriteUtil.h>

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

    WriteSmallArray( iFile, "abc_ref_hierarchy", H5T_STD_I32LE,
      H5T_NATIVE_INT32, 1, &enabled );
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
    WriteSmallArray( iFile, "children_sizes", H5T_STD_U32LE, H5T_NATIVE_UINT32,
                     childrenSizes.size(), &childrenSizes.front() );

    WriteStrings( iFile, "children_names",
                  childrenNames.size(), &childrenNames.front() );

    WriteReferences( iFile, "children_references",
                     childrenRefs.size(), &childrenRefs.front() );

    // Attributes
    WriteSmallArray( iFile, "attr_sizes", H5T_STD_U32LE, H5T_NATIVE_UINT32,
                     attrSizes.size(), &attrSizes.front() );

    WriteStrings( iFile, "attr_names",
                  attrNames.size(), &attrNames.front() );

    // Masks
    WriteSmallArray( iFile, "mask_on", H5T_STD_I8LE, H5T_NATIVE_INT8,
                     hasMask.size(), &hasMask.front() );
    WriteSmallArray( iFile, "mask_bits", H5T_STD_U32LE, H5T_NATIVE_UINT32,
                     maskBits.size(), &maskBits.front() );

    // Metadata
    WriteSmallArray( iFile, "meta_on", H5T_STD_I8LE, H5T_NATIVE_INT8,
                     hasMeta.size(), &hasMeta.front() );
    WriteStrings( iFile, "meta_strs",
                  metaStrs.size(), &metaStrs.front() );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
