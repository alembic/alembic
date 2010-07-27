//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#include <Alembic/HDF5/PropertyList.h>

namespace Alembic {
namespace HDF5 {

//-*****************************************************************************
PropertyList &PropertyList::operator=( hid_t prop_id )
{
    ABCH5_ASSERT( prop_id >= 0,
                  "Invalid prop_id passed into PropertyList assign" );
    ABCH5_ASSERT( id() < 0,
                  "Cannot assign into a PropertyList that already exists" );
    setId( prop_id );
    setOwned( false );
}

//-*****************************************************************************
// Destructor. Cannot throw.
PropertyList::~PropertyList()
{
    if ( valid() && owned() )
    {
        // No throwing inside destructor.
        // std::cout << "Deleting property list: " << id();
        herr_t status = H5Pclose( id() );
        // std::cout << "." << std::endl;
        ABCH5_WARN( status >= 0,
                    "PropertyList::~PropertyList() H5Pclose failed." );
    }
    setId( -1 );
    setOwned( false );
}

//-*****************************************************************************
void PropertyList::close()
{
    ABCH5_CHECK_VALID( *this, "Cannot close an invalid object" );
    ABCH5_ASSERT( owned(), "Cannot close an unowned object" );
    herr_t status = H5Pclose( id() );
    ABCH5_ASSERT( status >= 0, "PropertyList::close() H5Pclose failed." );
    setId( -1 );
    setOwned( false );
}

//-*****************************************************************************
void PropertyList::copyFrom( const PropertyList& other )
{
    ABCH5_ASSERT( id() < 0,
                  "Cannot copy into a PropertyList that already exists" );
    ABCH5_CHECK_VALID( other,
                       "Invalid object passed to PropertyList::copyFrom()" );
    hid_t ID = H5Pcopy( other.id() );
    ABCH5_ASSERT( ID >= 0,
                  "PropertyList::copyFrom() H5Pcopy() failed" );
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
//-*****************************************************************************
// CREATION ORDER FOR GROUPS
//-*****************************************************************************
//-*****************************************************************************

CreationOrderPlist::CreationOrderPlist()
  : PropertyList()
{
    herr_t status;
    hid_t ID = H5Pcreate( H5P_GROUP_CREATE );
    ABCH5_ASSERT( ID >= 0,
                  "CreationOrderPlist: H5Pcreate failed" );
    status = H5Pset_link_creation_order( ID,
                                         ( H5P_CRT_ORDER_TRACKED |
                                           H5P_CRT_ORDER_INDEXED ) );
    ABCH5_ASSERT( status >= 0,
                  "CreationOrderPlist: "
                  "H5Pset_link_creation_order() failed" );
    
    // Kick ass. Done.
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
//-*****************************************************************************
// GZIP COMPRESSION FOR DATASETS
//-*****************************************************************************
//-*****************************************************************************

DsetGzipCreatePlist::DsetGzipCreatePlist( const Dimensions &dims,
                                          int level )
  : PropertyList()
{
    herr_t status;
    hid_t ID = H5Pcreate( H5P_DATASET_CREATE );
    ABCH5_ASSERT( ID >= 0,
                  "DsetGzipCreatePlist: H5Pcreate failed" );

    // Chunking.
    HDimensions hdims( dims );
    status = H5Pset_chunk( ID, hdims.size(), &hdims.front() );
    ABCH5_ASSERT( status >= 0,
                  "DsetGzipCreatePlist: "
                  "H5Pset_chunk() failed" );
    
    level = level < 0 ? 0 : level > 9 ? 9 : level;
    status = H5Pset_deflate( ID, ( unsigned int )level );
    ABCH5_ASSERT( status >= 0,
                  "DsetGzipCreatePlist: "
                  "H5Pset_link_creation_order() failed" );
    
    // Kick ass. Done.
    setId( ID );
    setOwned( true );
}

} // End namespace HDF5
} // End namespace Alembic
