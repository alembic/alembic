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

#include <AlembicHDF5/File.h>

namespace AlembicHDF5 {

//-*****************************************************************************
void File::open( const std::string &nme,
                 unsigned int flags,
                 const PropertyList &accessPlist )
{
    // Validate inputs
    ABCH5_CHECK_VALID( accessPlist,
                       "File::open() passed invalid accessPlist" );
    
    // Can't open something that's already occupied.
    ABCH5_ASSERT( id() < 0,
                  "File::open() Cannot open into an existing File" );

    // Verify that File exists and is HDF5.
    htri_t exi = H5Fis_hdf5( nme.c_str() );
    if ( exi != 1 )
    {
        NonexistentFileExc exc( nme );
        throw( exc );
    }
    
    // Open
    hid_t ID = H5Fopen( nme.c_str(), flags, accessPlist.id() );
    
    // Check success
    ABCH5_ASSERT( ID >= 0,
                  "File::open() Failed H5Fopen2 of object: " << nme );
    
    // Set
    setId( ID );
    setOwned( true );
    setName( nme );
}

//-*****************************************************************************
// Create. Right now does not support anything other than default
// creation & access types, but will add as needed.
void File::create( const std::string &nme,
                   unsigned int flags,
                   const PropertyList &creationPlist,
                   const PropertyList &accessPlist )
{
    // Validate inputs.
    ABCH5_CHECK_VALID( creationPlist,
                       "File::create() passed invalid creationPlist" );
    ABCH5_CHECK_VALID( accessPlist,
                       "File::create() passed invalid accessPlist" );
    
    // Can't open something that's already occupied.
    ABCH5_ASSERT( id() < 0,
                  "File::create() Cannot open into an existing File" );
    // Open
    hid_t ID = H5Fcreate( nme.c_str(), flags, 
                          creationPlist.id(),
                          accessPlist.id() );
    
    // Check success
    ABCH5_ASSERT( ID >= 0,
                  "File::create() Failed H5Fcreate of object: " << nme );
    
    // Set
    setId( ID );
    setOwned( true );
    setName( nme );
}

//-*****************************************************************************
// Destructor
File::~File()
{
    if ( valid() && owned() )
    {
        // No throwing inside destructor.
        // std::cout << "Deleting file: " << id();
        herr_t status = H5Fclose( id() );
        // std::cout << "." << std::endl;
        ABCH5_WARN( status >= 0,
                    "File::~File() H5Fclose failed." );
    }
    setId( -1 );
    setOwned( false );
    setName( "UNKNOWN" );
}

//-*****************************************************************************
// Error to call closed on an unowned object.
void File::close()
{
    ABCH5_CHECK_VALID( *this, "Cannot close an invalid object" );
    ABCH5_ASSERT( owned(), "Cannot close an unowned object" );
    herr_t status = H5Fclose( id() );
    ABCH5_ASSERT( status >= 0, "File::close() H5Fclose failed." );
    setId( -1 );
    setOwned( false );
    setName( "UNKNOWN" );
}

//-*****************************************************************************
// Flush (static)
void File::flush( BaseObject &object, H5F_scope_t scope )
{
    // Validate inputs.
    ABCH5_CHECK_VALID( object, "File::flush() passed invalid object" );

    herr_t status = H5Fflush( object.id(), scope );
    ABCH5_ASSERT( status >= 0, "File::flush() H5Fflush failed." );
}

//-*****************************************************************************
bool File::isFileObject() const
{
    return true;
}

} // End namespace AlembicHDF5
