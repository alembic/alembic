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

#include <Alembic/HDF5/Datatype.h>
#include <Alembic/HDF5/Attribute.h>
#include <Alembic/HDF5/Dataset.h>

namespace Alembic {
namespace HDF5 {

//-*****************************************************************************
void Datatype::open( const Attribute &attr )
{
    // Make sure not already open.
    ABCH5_ASSERT( id() < 0,
                  "Cannot create Datatype into existing Datatype" );

    // Validate inputs.
    ABCH5_CHECK_VALID( attr,
                       "Datatype::open() passed invalid attribute" );

    // Get it!
    hid_t ID = H5Aget_type( attr.id() );
    ABCH5_ASSERT( ID >= 0,
                  "Datatype::open() H5Aget_type failed." );

    // We DO own this type, it is a copy.
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
void Datatype::open( const Dataset &dset )
{
    // Make sure not already open.
    ABCH5_ASSERT( id() < 0,
                  "Cannot create Datatype into existing Datatype" );

    // Validate inputs.
    ABCH5_CHECK_VALID( dset,
                       "Datatype::open() passed invalid Dataset" );

    // Get it!
    hid_t ID = H5Dget_type( dset.id() );
    ABCH5_ASSERT( ID >= 0,
                  "Datatype::open() H5Dget_type failed." );

    // We DO own this type, it is a copy.
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
// Array creation
void Datatype::arrayCreate( const Datatype &baseType,
                            const Dimensions &dims )
{
    // Array creation.
    ABCH5_ASSERT( id() < 0,
                  "Cannot create array Datatype into existing Datatype" );

    // Validate inputs.
    ABCH5_CHECK_VALID( baseType,
                       "Datatype::arrayCreate() Invalid baseType" );
    ABCH5_ASSERT( dims.size() > 0,
                  "Datatype::arrayCreate() Invalid rank: " << dims.size() );

    // Create array
    HDimensions hdims( dims );
    hid_t ID = H5Tarray_create2( baseType.id(),
                                 hdims.size(),
                                 hdims.rootPtr() );
    ABCH5_ASSERT( ID >= 0,
                  "Datatype::arrayCreate() H5Tarray_create2 failed" );

    // Set
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
Datatype &Datatype::operator=( hid_t propId )
{
    // Make sure we're not created.
    ABCH5_ASSERT( id() < 0,
                  "Cannot assign to a Datatype that already exists." );

    setId( propId );
    setOwned( false );
    return *this;
}

//-*****************************************************************************
Datatype::~Datatype()
{
    if ( valid() && owned() )
    {
        // std::cout << "Deleting datatype: " << id();
        herr_t status = H5Tclose( id() );
        // std::cout << "." << std::endl;
        // if ( status < 0 ) { abort(); }
        ABCH5_WARN( status >= 0,
                    "Datatype::~Datatype() H5Tclose() failed." );
    }
    setId( -1 );
    setOwned( false );
}

//-*****************************************************************************
void Datatype::close()
{
    // Invalid to close a non-valid or non-owned object.
    ABCH5_CHECK_VALID( *this, "Cannot close invalid Datatype" );
    ABCH5_ASSERT( owned(), "Cannot close non-owned Datatype" );
    herr_t status = H5Tclose( id() );
    ABCH5_ASSERT( status >= 0,
                  "Datatype::close() H5Tclose() failed." );
    setId( -1 );
    setOwned( false );
}

//-*****************************************************************************
// Explicit copy functions.
void Datatype::copyFrom( const Datatype &other )
{
    // Check this.
    ABCH5_ASSERT( id() < 0,
                  "Cannot copy into Datatype that exists" );

    // Validate input
    ABCH5_CHECK_VALID( other,
                       "Datatype::copyFrom() passed invalid Datatype" );

    hid_t ID = H5Tcopy( other.id() );
    ABCH5_ASSERT( ID >= 0,
                  "Datatype::copyFrom() H5Tcopy() failed" );

    // Set
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
bool Datatype::equal( const Datatype &other ) const
{
    if ( valid() && other.valid() )
    {
        htri_t eq = H5Tequal( id(), other.id() );
        ABCH5_ASSERT( eq >= 0,
                      "Datatype::equal() H5Tequal failed." );
        return ( bool )eq;
    }

    return false;
}

//-*****************************************************************************
bool Datatype::equivalent( const Datatype &other ) const
{
    if ( valid() && other.valid() )
    {
        if ( equal( other ) )
        {
            return true;
        }

        // If they're not equal, but they are both arrayed and
        // both have the same super type
        // and dimensions, they're equivalent
        if ( isArrayClass() && other.isArrayClass() )
        {
            Datatype mySuper( getSuper() );
            if ( mySuper.equal( other.getSuper() ) )
            {
                Dimensions myDims;
                getArrayDims( myDims );
                Dimensions otherDims;
                other.getArrayDims( otherDims );
                if ( myDims == otherDims )
                {
                    return true;
                }
            }
        }
    }
    return false;
}

//-*****************************************************************************
void Datatype::lock()
{
    ABCH5_CHECK_VALID( *this,
                       "Datatype::lock() called on invalid object" );
    herr_t status = H5Tlock( id() );
    ABCH5_ASSERT( status >= 0,
                  "Datatype::lock() H5Tlock() failed." );
}

//-*****************************************************************************
H5T_class_t Datatype::getClass() const
{
    ABCH5_CHECK_VALID( *this,
                       "Datatype::getClass() called on invalid object" );
    H5T_class_t c = H5Tget_class( id() );
    ABCH5_ASSERT( c != H5T_NO_CLASS,
                  "Datatype::getClass() H5Tget_class failed." );
    return c;
}

//-*****************************************************************************
// Have to return hid_t, rather than Datatype.
hid_t Datatype::getSuper() const
{
    ABCH5_CHECK_VALID( *this,
                       "Datatype::getSuper() called on invalid object" );
    hid_t super_id = H5Tget_super( id() );
    ABCH5_ASSERT( super_id >= 0,
                  "Datatype::getSuper() H5Tget_super failed." );
    return super_id;
}

//-*****************************************************************************
// Atomic
size_t Datatype::getSize() const
{
    ABCH5_CHECK_VALID( *this,
                       "Datatype::getSize() called on invalid object" );
    size_t sze = H5Tget_size( id() );
    ABCH5_ASSERT( sze > 0,
                  "Datatype::getSize() H5Tget_size failed" );
    return sze;
}

//-*****************************************************************************
void Datatype::setSize( size_t s )
{
    ABCH5_CHECK_VALID( *this,
                       "Datatype::setSize() called on invalid object" );
    herr_t status = H5Tset_size( id(), s );
    ABCH5_ASSERT( status >= 0,
                  "Datatype::setSize() H5Tset_size failed" );
}

//-*****************************************************************************
H5T_order_t Datatype::getOrder() const
{
    ABCH5_CHECK_VALID( *this,
                       "Datatype::getOrder() called on invalid object" );
    H5T_order_t ord = H5Tget_order( id() );
    ABCH5_ASSERT( ord != H5T_ORDER_ERROR,
                  "Datatype::getOrder() H5Tget_order() failed" );
    return ord;
}

//-*****************************************************************************
H5T_sign_t Datatype::getSign() const
{
    ABCH5_CHECK_VALID( *this,
                       "Datatype::getSign() called on invalid object" );
    H5T_sign_t sgn = H5Tget_sign( id() );
    ABCH5_ASSERT( sgn != H5T_SGN_ERROR,
                  "Datatype::getSign() H5Tget_sign() failed" );
    return sgn;
}

//-*****************************************************************************
// Array stuff
int Datatype::getArrayNdims() const
{
    ABCH5_CHECK_VALID( *this,
                       "Datatype::getArrayNdims() called on invalid object" );
    int ndims = H5Tget_array_ndims( id() );
    ABCH5_ASSERT( ndims > 0,
                  "Datatype::getArrayNdims() H5Tget_array_ndims failed" );
    return ndims;
}

//-*****************************************************************************
void Datatype::getArrayDims( Dimensions &dims ) const
{
    ABCH5_CHECK_VALID( *this,
                       "Datatype::getArrayDims() called on invalid object" );
    int ndims = H5Tget_array_ndims( id() );
    ABCH5_ASSERT( ndims > 0,
                  "Datatype::getArrayDims() H5Tget_array_ndims failed" );

    HDimensions hdims( ( size_t )ndims );
    ndims = H5Tget_array_dims2( id(), hdims.rootPtr() );
    ABCH5_ASSERT( ndims > 0,
                  "Datatype::getArrayDims() H5Tget_array_dims failed" );
    ABCH5_ASSERT( ndims == hdims.size(),
                  "Datatype::getArrayDims() inconsistent ranks" );
    dims = hdims;
}

//-*****************************************************************************
size_t Datatype::getRank0Dim() const
{
    Dimensions dims;
    getArrayDims( dims );
    ABCH5_ASSERT( dims.size() == 1,
                  "Datatype::getRank0Dim() returned bad ndims: "
                  << dims.size() );
    return dims[0];
}

} // End namespace HDF5
} // End namespace Alembic

