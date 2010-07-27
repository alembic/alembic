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

#include <AlembicHDF5/Attribute.h>

namespace AlembicHDF5 {

//-*****************************************************************************
// Destroy and close
Attribute::~Attribute()
{
    if ( valid() && owned() )
    {
        // std::cout << "Deleting attribute: " << id();
        herr_t status = H5Aclose( id() );
        // std::cout << "." << std::endl;
        ABCH5_WARN( status >= 0,
                    "Attribute::~Attribute() H5Aclose() failed." );
    }
    setId( -1 );
    setOwned( false );
}

//-*****************************************************************************
// Invalid to close an invalid or unowned object.
void Attribute::close()
{
    ABCH5_CHECK_VALID( *this,
                       "Attribute::close() called in invalid object" );
    ABCH5_ASSERT( owned(),
                  "Attribute::close() called on unowned object" );
    herr_t status = H5Aclose( id() );
    ABCH5_ASSERT( status >= 0,
                  "Attribute::close() H5Aclose() failed." );
    setId( -1 );
    setOwned( false );
}

//-*****************************************************************************
void Attribute::create( const AttributedObject &loc,
                        const std::string &attrName,
                        const Datatype &dtype,
                        const Dataspace &dspace,
                        bool doCheckExists )
{
    // Check empty
    ABCH5_ASSERT( id() < 0,
                  "Cannot create into an existing Attribute" );

    // Validate inputs.
    ABCH5_CHECK_VALID( loc, "Attribute::create() invalid location" );
    ABCH5_CHECK_VALID( dtype, "Attribute::create() invalid dtype" );
    ABCH5_CHECK_VALID( dspace, "Attribute::create() invalid dspace" );

    // Check to see whether attribute exists.
    if ( doCheckExists )
    {
        htri_t exi = H5Aexists( loc.id(), attrName.c_str() );
        ABCH5_ASSERT( exi >= 0,
                      "Attribute::create() H5Aexists() failed." );
        if ( exi == 1 )
        {
            // This is a special error. Throw a specific execption.
            DuplicateNameExc exc( loc.name(), attrName );
            throw( exc );
        }
    }

    // Create
    hid_t ID = H5Acreate2( loc.id(), attrName.c_str(), dtype.id(),
                           dspace.id(), H5P_DEFAULT, H5P_DEFAULT );
    ABCH5_ASSERT( ID >= 0,
                  "Attribute::create() H5Acreate2 failed." );

    // Set
    setId( ID );
    setOwned( true );
    setName( attrName );
}

//-*****************************************************************************
void Attribute::open( const AttributedObject &loc,
                      const std::string &attrName,
                      bool doCheckExists )
{
    // Check empty
    ABCH5_ASSERT( id() < 0,
                  "Cannot open into an existing Attribute" );

    // Validate inputs.
    ABCH5_CHECK_VALID( loc, "Attribute::open() invalid location" );

    // Check to see whether attribute exists.
    if ( doCheckExists )
    {
        htri_t exi = H5Aexists( loc.id(), attrName.c_str() );
        ABCH5_ASSERT( exi >= 0,
                      "Attribute::open() H5Aexists() failed." );
        if ( exi == 0 )
        {
            // This is a special error. Throw a specific execption.
            NonexistentAttributeExc exc( loc.name(), attrName );
            throw( exc );
        }
    }
    
    // Create
    hid_t ID = H5Aopen( loc.id(), attrName.c_str(), H5P_DEFAULT );
    ABCH5_ASSERT( ID >= 0,
                  "Attribute::open() H5Aopen failed." );

    // Set
    setId( ID );
    setOwned( true );
    setName( attrName );
}

//-*****************************************************************************
void Attribute::open( const AttributedObject &loc,
                      const std::string &parentName,
                      const std::string &attrName,
                      bool doCheckExists )
{
    // Check empty
    ABCH5_ASSERT( id() < 0,
                  "Cannot open into an existing Attribute" );

    // Validate inputs.
    ABCH5_CHECK_VALID( loc, "Attribute::open() invalid location" );

    // Check existence
    if ( doCheckExists )
    {
        htri_t exi = H5Aexists_by_name( loc.id(),
                                        parentName.c_str(),
                                        attrName.c_str(),
                                        H5P_DEFAULT );
        ABCH5_ASSERT( exi >= 0,
                      "Attribute::open H5Aexists_by_name() failed." );
        if ( exi == 0 )
        {
            NonexistentAttributeExc exc( loc.name() + parentName,
                                         attrName );
            throw( exc );
        }
    }
    
    // Create
    hid_t ID = H5Aopen_by_name( loc.id(),
                                parentName.c_str(),
                                attrName.c_str(),
                                H5P_DEFAULT,
                                H5P_DEFAULT );
    ABCH5_ASSERT( ID >= 0,
                  "Attribute::open() H5Aopen_by_name failed." );

    // Set
    setId( ID );
    setOwned( true );
    setName( attrName );
}

//-*****************************************************************************
void Attribute::open( const AttributedObject &loc,
                      hsize_t idx,
                      H5_index_t idxType,
                      H5_iter_order_t idxOrder,
                      const PropertyList &accessPlist,
                      const PropertyList &linkPlist,
                      bool doCheckExists )
{
    // Check empty
    ABCH5_ASSERT( id() < 0,
                  "Cannot open into an existing Attribute" );

    // Validate inputs.
    ABCH5_CHECK_VALID( loc, "Attribute::open() invalid location" );
    ABCH5_CHECK_VALID( accessPlist,
                       "Attribute::open() invalid access plist" );
    ABCH5_CHECK_VALID( linkPlist,
                       "Attribute::open() invalid link plist" );

    if ( doCheckExists )
    {
        if ( idx >= loc.numAttributes() )
        {
            NonexistentAttributeExc exc( loc.name(), "INDEXED" );
            throw( exc );
        }
    }
    
    // Create
    hid_t ID = H5Aopen_by_idx( loc.id(),
                               ".",
                               idxType,
                               idxOrder,
                               idx,
                               accessPlist.id(),
                               linkPlist.id() );
    ABCH5_ASSERT( ID >= 0,
                  "Attribute::open() H5Aopen_by_idx failed." );

    // Have to get name now.
    hssize_t numCharsInName = H5Aget_name( ID, 0, NULL );
    ABCH5_ASSERT( numCharsInName > 0,
                  "Attribute::open() H5Aget_name failed." );
    std::vector<char> nme( numCharsInName + 10 );
    numCharsInName = H5Aget_name( ID, nme.size(),
                                  ( char * )&nme.front() );
    ABCH5_ASSERT( numCharsInName > 0,
                  "Attribute::open() H5Aget_name failed." );

    // Set
    setId( ID );
    setOwned( true );
    setName( ( const char * )&nme.front() );
}

//-*****************************************************************************
void Attribute::readAll( const Datatype &memType,
                         void *intoBuffer ) const
{
    ABCH5_CHECK_VALID( *this,
                       "Attribute::readAll called on invalid object" );
    herr_t status = H5Aread( id(), memType.id(), intoBuffer );
    ABCH5_ASSERT( status >= 0,
                  "Attribute::readAll() H5Aread() failed." );
}

//-*****************************************************************************
void Attribute::writeAll( const Datatype &memType,
                          const void *fromBuffer )
{
    ABCH5_CHECK_VALID( *this,
                       "Attribute::writeAll called on invalid object" );
    herr_t status = H5Awrite( id(), memType.id(), fromBuffer );
    ABCH5_ASSERT( status >= 0,
                  "Attribute::writeAll() H5Awrite() failed." );
}

} // End namespace AlembicHDF5
