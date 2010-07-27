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

#include <Alembic/HDF5/Group.h>

namespace Alembic {
namespace HDF5 {

//-*****************************************************************************
void Group::open( const ParentObject &parent,
                  const std::string &nme,
                  const PropertyList &accessPlist,
                  bool doCheckExists )
{
    // Validate inputs.
    ABCH5_CHECK_VALID( parent, "Group::open() passed invalid parent" );
    ABCH5_CHECK_VALID( accessPlist,
                       "Group::open() passed invalid accessPlist" );
    
    // Can't open something that's already occupied.
    ABCH5_ASSERT( id() < 0,
                  "Group::open() Cannot open into an existing Group" );

    
    // See if it exists and is a group.
    if ( doCheckExists )
    {
        if ( parent.isFileObject() )
        {
            // Only "/" group in file object.
            if ( nme != "/" )
            {
                ABCH5_THROW( "Nonexistent Group: "
                             << parent.name() << ", "
                             << nme );
            }
        }
        else
        {
            // First, check to make sure the link exists.
            htri_t exi = H5Lexists( parent.id(), nme.c_str(),
                                    H5P_DEFAULT );
            ABCH5_ASSERT( exi >= 0,
                          "Group::open() H5Lexists() failed." );
            if ( exi == 0 )
            {
                ABCH5_THROW( "Nonexistent Group: "
                             << parent.name() << ", "
                             << nme );
            }

            // Now make sure it is a group.
            H5O_info_t oinfo;
            herr_t status = H5Oget_info_by_name( parent.id(),
                                                 nme.c_str(), &oinfo,
                                                 H5P_DEFAULT );
            ABCH5_ASSERT( status >= 0,
                          "Group::open() H5Oget_info_by_name failed." );
            if ( oinfo.type != H5O_TYPE_GROUP )
            {
                ABCH5_THROW( "Nonexistent Group: "
                             << parent.name() << ", "
                             << nme );
            }
        }
    }
    
    // Open
    hid_t ID = H5Gopen2( parent.id(), nme.c_str(), accessPlist.id() );
    
    // Check success
    ABCH5_ASSERT( ID >= 0,
                  "Group::open() Failed H5Gopen2 of object: " << nme );
    
    // Set
    setId( ID );
    setOwned( true );
    setName( nme );
}

//-*****************************************************************************
// Create. Right now does not support anything other than default
// creation & access types, but will add as needed.
void Group::create( const ParentObject &parent,
                    const std::string &nme,
                    const PropertyList &linkPlist,
                    const PropertyList &creationPlist,
                    const PropertyList &accessPlist,
                    bool doCheckExists )
{
    // Validate inputs.
    ABCH5_CHECK_VALID( parent,
                       "Group::create() passed invalid parent" );
    ABCH5_CHECK_VALID( linkPlist,
                       "Group::create() passed invalid linkPlist" );
    ABCH5_CHECK_VALID( creationPlist,
                       "Group::create() passed invalid creationPlist" );
    ABCH5_CHECK_VALID( accessPlist,
                       "Group::create() passed invalid accessPlist" );

    
    // Can't open something that's already occupied.
    ABCH5_ASSERT( id() < 0,
                  "Group::create() Cannot open into an existing Group" );

    if ( doCheckExists )
    {
        // Check to make sure the link does not exist.
        htri_t exi = H5Lexists( parent.id(), nme.c_str(),
                                H5P_DEFAULT );
        ABCH5_ASSERT( exi >= 0,
                      "Group::create() H5Lexists() failed." );
        if ( exi == 1 )
        {
            ABCH5_THROW( "Duplicate name: " << parent.name()
                         << ", " << nme );
        }
    }
    
    // Open
    hid_t ID = H5Gcreate2( parent.id(), nme.c_str(),
                           linkPlist.id(),
                           creationPlist.id(),
                           accessPlist.id() );
    
    // Check success
    ABCH5_ASSERT( ID >= 0,
                  "Group::create() Failed H5Gcreate2 of object: " << nme );
    
    // Set
    setId( ID );
    setOwned( true );
    setName( nme );
}

//-*****************************************************************************
// Destructor
Group::~Group()
{
    if ( valid() && owned() )
    {
        // No throwing inside destructor.
        // std::cout << "Deleting group: " << id() << " named " << name();
        herr_t status = H5Gclose( id() );
        // std::cout << "." << std::endl;
        ABCH5_WARN( status >= 0,
                    "Group::~Group() H5Gclose failed." );
    }
    setId( -1 );
    setOwned( false );
    setName( "UNKNOWN" );
}

//-*****************************************************************************
// Error to call closed on an unowned object.
void Group::close()
{
    ABCH5_CHECK_VALID( *this, "Cannot close an invalid object" );
    ABCH5_ASSERT( owned(), "Cannot close an unowned object" );
    herr_t status = H5Gclose( id() );
    ABCH5_ASSERT( status >= 0, "Group::close() H5Gclose failed." );
    setId( -1 );
    setOwned( false );
    setName( "UNKNOWN" );
}

//-*****************************************************************************
bool Group::isFileObject() const
{
    return false;
}

} // End namespace HDF5
} // End namespace Alembic
