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

#include <Alembic/AbcCoreHDF5/HDF5Util.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//-*****************************************************************************
// CREATION ORDER FOR GROUPS
//-*****************************************************************************
//-*****************************************************************************
hid_t CreationOrderPlist()
{
    herr_t status;
    hid_t ID = H5Pcreate( H5P_GROUP_CREATE );
    ABCA_ASSERT( ID >= 0,
                  "CreationOrderPlist: "
                  "H5Pcreate() failed" );

    status = H5Pset_link_creation_order( ID,
                                         ( H5P_CRT_ORDER_TRACKED |
                                           H5P_CRT_ORDER_INDEXED ) );
    ABCA_ASSERT( status >= 0,
                  "CreationOrderPlist: "
                  "H5Pset_link_creation_order() failed" );

    return ID;
}

//-*****************************************************************************
//-*****************************************************************************
// GZIP COMPRESSION FOR DATASETS
//-*****************************************************************************
//-*****************************************************************************
hid_t DsetGzipCreatePlist( const Dimensions &dims, int level )
{
    herr_t status;
    hid_t ID = H5Pcreate( H5P_DATASET_CREATE );
    ABCA_ASSERT( ID >= 0,
                  "DsetGzipCreatePlist: H5Pcreate failed" );

    // Chunking.
    HDimensions hdims( dims );
    status = H5Pset_chunk( ID, hdims.rank(), hdims.rootPtr() );
    ABCA_ASSERT( status >= 0,
                  "DsetGzipCreatePlist: "
                  "H5Pset_chunk() failed" );

    level = level < 0 ? 0 : level > 9 ? 9 : level;
    status = H5Pset_deflate( ID, ( unsigned int )level );
    ABCA_ASSERT( status >= 0,
                  "DsetGzipCreatePlist: "
                  "H5Pset_link_creation_order() failed" );

    return ID;
}

//-*****************************************************************************
bool EquivalentDatatypes( hid_t iA, hid_t iB )
{
    if ( iA >= 0 && iB >= 0 && H5Tequal( iA, iB ) > 0 )
        return true;

    return false;
}

//-*****************************************************************************
bool ObjectExists( H5Node& iParent, const std::string &iName )
{
    ABCA_ASSERT( iParent.isValidObject(),
                 "Invalid parent node passed into HDF5Util GroupExists: "
                 << iName << std::endl );

    HDF5Hierarchy* H5HPtr = iParent.getH5HPtr();

    if ( H5HPtr )
    {
        return H5HPtr->childExists( iParent.getRef(), iName );
    }
    else
    {
        // First, check to make sure the link exists.
        hid_t iParentObject = iParent.getObject();

        htri_t exi = H5Lexists( iParentObject, iName.c_str(), H5P_DEFAULT );
        if ( exi < 1 )
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

//-*****************************************************************************
bool GroupExists( H5Node& iParent, const std::string &iName )
{
    ABCA_ASSERT( iParent.isValidObject(),
                 "Invalid parent node passed into HDF5Util GroupExists: "
                 << iName << std::endl );

    HDF5Hierarchy* h5HPtr = iParent.getH5HPtr();

    if ( h5HPtr )
    {
        return h5HPtr->childExists( iParent.getRef(), iName );
    }
    else
    {
        // First, check to make sure the link exists.
        hid_t iParentObject = iParent.getObject();

        htri_t exi = H5Lexists( iParentObject, iName.c_str(), H5P_DEFAULT );
        if ( exi < 1 )
        {
            return false;
        }

        // Now make sure it is a group.
        H5O_info_t oinfo;
        herr_t status = H5Oget_info_by_name( iParentObject,
                                             iName.c_str(), &oinfo,
                                             H5P_DEFAULT );
        if ( status < 0 )
        {
            return false;
        }

        if ( oinfo.type != H5O_TYPE_GROUP )
        {
            return false;
        }

        return true;
    }
}

//-*****************************************************************************
bool DatasetExists( H5Node& iParent, const std::string &iName )
{
    ABCA_ASSERT( iParent.isValidObject(),
                 "Invalid parent group passed into HDF5Util DatasetExists: "
                 << iName << std::endl );

    HDF5Hierarchy* h5HPtr = iParent.getH5HPtr();

    if ( h5HPtr )
    {
        return h5HPtr->childExists( iParent.getRef(), iName );
    }
    else
    {
        hid_t iParentObject = iParent.getObject();

        // First, check to make sure the link exists.
        htri_t exi = H5Lexists( iParentObject, iName.c_str(), H5P_DEFAULT );
        if ( exi < 1 )
        {
            return false;
        }

        // Now make sure it is a group.
        H5O_info_t oinfo;
        herr_t status = H5Oget_info_by_name( iParentObject,
                                             iName.c_str(), &oinfo,
                                             H5P_DEFAULT );
        if ( status < 0 )
        {
            return false;
        }

        if ( oinfo.type != H5O_TYPE_DATASET )
        {
            return false;
        }

        return true;
    }
}

//-*****************************************************************************
H5Node OpenGroup( H5Node& iParent, const std::string& iName )
{
    ABCA_ASSERT( iParent.isValidObject(),
                 "Invalid parent group passed into HDF5Util OpenGroup: "
                 << iName << std::endl );

    HDF5Hierarchy* h5HPtr = iParent.getH5HPtr();

    if ( h5HPtr )
    {
        hobj_ref_t childRef = h5HPtr->getChildRef( iParent.getRef(), iName );

        hid_t childId = H5Rdereference( iParent.getObject(),
                                        H5R_OBJECT,
                                        (H5R_type_t)childRef, NULL );

        return H5Node( childId, childRef, h5HPtr );
    }
    else
    {
        hid_t childId = H5Gopen2( iParent.getObject(),
                                  iName.c_str(),
                                  H5P_DEFAULT );

        return H5Node( childId, 0, NULL );
    }
}

//-*****************************************************************************
void CloseObject( H5Node& iNode )
{
    if ( iNode.isValidObject() )
    {
        H5Oclose( iNode.getObject() );
        iNode.setObject( -1 );
    }
}

//-*****************************************************************************
bool AttrExists( H5Node& iParent, const std::string& iName )
{
    ABCA_ASSERT( iParent.isValidObject(),
                 "Invalid parent object in ReadMetaData" );

    HDF5Hierarchy* h5HPtr = iParent.getH5HPtr();

    if ( h5HPtr )
    {
        return h5HPtr->attrExists( iParent.getRef(), iName );
    }
    else
    {
        return H5Aexists( iParent.getObject(), iName.c_str() ) > 0;
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic

