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

#include <Alembic/HDF5/Dataset.h>

//-*****************************************************************************
// The Reference interface treats hobj_ref_t (which is defined as a size_t)
// and const void * as the same types. Make sure they're the same size.
// typedef void* VoidPtr;
// CJH: This fails on OSX.
// BOOST_STATIC_ASSERT( sizeof( hobj_ref_t ) == sizeof( size_t ) );
//-*****************************************************************************

namespace Alembic {
namespace HDF5 {

//-*****************************************************************************
// Destroy and close
Dataset::~Dataset()
{
    if ( valid() && owned() )
    {
        // std::cout << "Deleting dataset: " << id();
        herr_t status = H5Dclose( id() );
        // std::cout << "." << std::endl;
        ABCH5_WARN( status >= 0,
                    "Dataset::~Dataset() H5Dclose() failed." );
    }
    setId( -1 );
    setOwned( false );
}

//-*****************************************************************************
void Dataset::create( const ParentObject &loc,
                      const std::string &dsetName,
                      const Datatype &dtype,
                      const Dataspace &dspace,
                      const PropertyList &linkPlist,
                      const PropertyList &createPlist,
                      const PropertyList &accessPlist,
                      bool doCheckExists )
{
    // Check empty
    ABCH5_ASSERT( id() < 0,
                  "Cannot create into an existing Dataset" );

    // Validate inputs.
    ABCH5_CHECK_VALID( loc, "Dataset::create() invalid location" );
    ABCH5_CHECK_VALID( dtype, "Dataset::create() invalid dtype" );
    ABCH5_CHECK_VALID( dspace, "Dataset::create() invalid dspace" );
    ABCH5_CHECK_VALID( linkPlist,
                       "Dataset::create() invalid linkPlist" );
    ABCH5_CHECK_VALID( createPlist,
                       "Dataset::create() invalid createPlist" );
    ABCH5_CHECK_VALID( accessPlist,
                       "Dataset::create() invalid accessPlist" );

    if ( doCheckExists )
    {
        // Check to make sure the link does not exist.
        htri_t exi = H5Lexists( loc.id(), dsetName.c_str(),
                                H5P_DEFAULT );
        ABCH5_ASSERT( exi >= 0,
                      "Dataset::create() H5Lexists() failed." );
        if ( exi == 1 )
        {
            ABCH5_THROW( "Duplicate Name: " << loc.name()
                         << ", " << dsetName );
        }
    }

    // Create
    hid_t ID = H5Dcreate2( loc.id(), dsetName.c_str(), dtype.id(),
                           dspace.id(), linkPlist.id(), createPlist.id(),
                           accessPlist.id() );
    ABCH5_ASSERT( ID >= 0,
                  "Dataset::create() H5Dcreate2 failed." );

    // Set
    setId( ID );
    setOwned( true );
    setName( dsetName );
}

//-*****************************************************************************
void Dataset::open( const ParentObject &loc,
                    const std::string &dsetName,
                    const PropertyList &accessPlist,
                    bool doCheckExists )
{
    // Check empty
    ABCH5_ASSERT( id() < 0,
                  "Cannot open into an existing Dataset" );

    // Validate inputs.
    ABCH5_CHECK_VALID( loc, "Dataset::open() invalid location" );
    ABCH5_CHECK_VALID( accessPlist,
                       "Dataset::open() invalid accessPlist" );
    
    // See if it exists and is a group.
    if ( doCheckExists )
    {
        // First, check to make sure the link exists.
        htri_t exi = H5Lexists( loc.id(), dsetName.c_str(),
                                H5P_DEFAULT );
        ABCH5_ASSERT( exi >= 0,
                      "Dataset::open() H5Lexists() failed." );
        if ( exi == 0 )
        {
            ABCH5_THROW( "Nonexistent Dataset: "
                         << loc.name() << ", "
                         << dsetName );
        }
        
        // Now make sure it is a group.
        H5O_info_t oinfo;
        herr_t status = H5Oget_info_by_name( loc.id(),
                                             dsetName.c_str(), &oinfo,
                                             H5P_DEFAULT );
        ABCH5_ASSERT( status >= 0,
                      "Dataset::open() H5Oget_info_by_name failed." );
        if ( oinfo.type != H5O_TYPE_DATASET )
        {
            ABCH5_THROW( "Nonexistent Dataset: "
                         << loc.name() << ", "
                         << dsetName );
        }
    }

    // Create
    hid_t ID = H5Dopen( loc.id(), dsetName.c_str(), accessPlist.id() );
    ABCH5_ASSERT( ID >= 0,
                  "Dataset::open() H5Dopen failed." );

    // Set
    setId( ID );
    setOwned( true );
    setName( dsetName );
}

//-*****************************************************************************
void Dataset::openReference( const ParentObject &loc,
                             hobj_ref_t refId )
{
    // Convert to const void *
    const void *refIdCV = ( const void * )&refId;
    
    // Make sure we're empty.
    ABCH5_ASSERT( id() < 0,
                  "Can't open reference into existing Dataset" );

    // Validate inputs.
    ABCH5_CHECK_VALID( loc,
                       "Dataset::openReference() passed invalid location" );
    ABCH5_ASSERT( refId >= 0,
                  "Dataset::openReference() passed invalid reference" );
    
    // Open.
    hid_t ID = H5Rdereference( loc.id(), H5R_OBJECT, refIdCV );
    ABCH5_ASSERT( ID >= 0,
                  "Dataset::openReference() H5Rdereference() failed." );

    // Verify that we're actually loading a dataset.
    H5O_type_t objType;
    herr_t status = H5Rget_obj_type2( ID, H5R_OBJECT, refIdCV,
                                      &objType );
    ABCH5_ASSERT( status >= 0,
                  "Dataset::openReference() H5Rget_obj_type2 failed." );
    ABCH5_ASSERT( objType == H5O_TYPE_DATASET,
                  "Dataset::openReference() passed invalid dataset ref" );

    // std::cout << "ABOUT TO GET NAME FIRST TIME" << std::endl;


    // Get the name.
    char nameBuf[1024];
    ssize_t numChars = H5Rget_name( loc.id(), H5R_OBJECT, refIdCV,
                                    nameBuf, 1024 );
    // std::cout << "returned numChars = " << numChars << std::endl;
    if ( numChars == 0 )
    {
        // No name.
        setName( "UNNAMED" );
    }
    else
    {
        ABCH5_ASSERT( numChars > 0,
                      "Dataset::openReference() H5Rget_name() failed." );
        std::vector<char> chars( 10 + ( size_t )numChars, 0 );
        // std::cout << "ABOUT TO GET NAME SECOND TIME" << std::endl;
        numChars = H5Rget_name( loc.id(), H5R_OBJECT, refIdCV,
                                &chars.front(), numChars+1 );
        ABCH5_ASSERT( numChars > 0,
                      "Dataset::openReference() H5Rget_name()(2) failed." );
        //std::cout << "name: " << ( const char * )&chars.front()
        //          << std::endl;
        setName( ( const char * )&chars.front() );
    }

    // Set
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
// Invalid to close an invalid or unowned object.
void Dataset::close()
{
    ABCH5_CHECK_VALID( *this,
                       "Dataset::close() called in invalid object" );
    ABCH5_ASSERT( owned(),
                  "Dataset::close() called on unowned object" );
    herr_t status = H5Dclose( id() );
    ABCH5_ASSERT( status >= 0,
                  "Dataset::close() H5Dclose() failed." );
    setId( -1 );
    setOwned( false );
}

//-*****************************************************************************
void Dataset::readAll( const Datatype &memType,
                       void *intoBuffer ) const
{
    ABCH5_CHECK_VALID( *this,
                       "Dataset::readAll called on invalid object" );
    herr_t status = H5Dread( id(), memType.id(),
                             H5S_ALL, H5S_ALL, H5P_DEFAULT,
                             intoBuffer );
    ABCH5_ASSERT( status >= 0,
                  "Dataset::readAll() H5Dread() failed." );
}

//-*****************************************************************************
void Dataset::writeAll( const Datatype &memType,
                        const void *fromBuffer )
{
    ABCH5_CHECK_VALID( *this,
                       "Dataset::writeAll called on invalid object" );
    herr_t status = H5Dwrite( id(), memType.id(),
                              H5S_ALL, H5S_ALL, H5P_DEFAULT,
                              fromBuffer );
    ABCH5_ASSERT( status >= 0,
                  "Dataset::writeAll() H5Dwrite() failed." );
}


//-*****************************************************************************
void Dataset::read( const Datatype &memType,
                    const Dataspace &memSpace,
                    const Dataspace &fileSpace,
                    const PropertyList &xferPlist,
                    void *intoBuffer ) const
{
    ABCH5_CHECK_VALID( *this,
                       "Dataset::read called on invalid object" );
    herr_t status = H5Dread( id(), memType.id(),
                             memSpace.id(), fileSpace.id(),
                             xferPlist.id(),
                             intoBuffer );
    ABCH5_ASSERT( status >= 0,
                  "Dataset::read() H5Dread() failed." );
}

//-*****************************************************************************
void Dataset::write( const Datatype &memType,
                     const Dataspace &memSpace,
                     const Dataspace &fileSpace,
                     const PropertyList &xferPlist,
                     const void *fromBuffer )
{
    ABCH5_CHECK_VALID( *this,
                       "Dataset::write called on invalid object" );
    herr_t status = H5Dwrite( id(), memType.id(),
                              memSpace.id(), fileSpace.id(),
                              xferPlist.id(),
                              fromBuffer );
    ABCH5_ASSERT( status >= 0,
                  "Dataset::write() H5Dwrite() failed." );
}

} // End namespace HDF5
} // End namespace Alembic

