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

#include <Alembic/HDF5/Dataspace.h>
#include <Alembic/HDF5/Attribute.h>
#include <Alembic/HDF5/Dataset.h>

namespace Alembic {
namespace HDF5 {

//-*****************************************************************************
class Attribute;
class Dataset;

//-*****************************************************************************
void Dataspace::createScalar()
{
    // Make sure we're not occupied.
    ABCH5_ASSERT( id() < 0,
                  "Cannot creat new Dataspace in existing." );

    // Create scalar
    hid_t ID = H5Screate( H5S_SCALAR );
    ABCH5_ASSERT( ID > 0,
                  "Dataspace::createScalar() H5Screate() failed." );

    // Set it.
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
void Dataspace::createSimple( const Dimensions &dims )
{
    // Make sure we're not occupied.
    ABCH5_ASSERT( id() < 0,
                  "Cannot create new Dataspace in existing." );

    // Convert dimensions to HDimensions
    HDimensions hdims( dims );
    size_t npoints = hdims.numPoints();
    ABCH5_ASSERT( npoints > 0,
                  "Cannot create degenerate dataspace" );

    // Make it
    hid_t ID = H5Screate_simple( hdims.size(), hdims.rootPtr(), NULL );
    ABCH5_ASSERT( ID > 0,
                  "Dataspace::createSimple() H5Screate_simple() failed." );

    // Set it.
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
void Dataspace::createSimple( const Dimensions &dims,
                              const Dimensions &maxDims )
{
    
    // Make sure we're not occupied.
    ABCH5_ASSERT( id() < 0,
                  "Cannot create new Dataspace in existing." );

    // Convert dimensions to HDimensions
    HDimensions hdims( dims );
    size_t npoints = hdims.numPoints();
    ABCH5_ASSERT( npoints > 0,
                  "Cannot create degenerate dataspace" );
    HDimensions maxHdims( maxDims );
    ABCH5_ASSERT( hdims.size() == maxHdims.size(),
                  "Dataspace::createSimple() "
                  "Dims and Maxdims have different rank" );

    // Make it
    hid_t ID = H5Screate_simple( hdims.size(),
                                 hdims.rootPtr(),
                                 maxHdims.rootPtr() );
    ABCH5_ASSERT( ID > 0,
                  "Dataspace::createSimple() H5Screate_simple() failed." );

    // Set it.
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
// Open from attributes and datasets.
void Dataspace::open( const Attribute &attr )
{
    // Make sure we're not occupied.
    ABCH5_ASSERT( id() < 0,
                  "Cannot open Dataspace in existing." );

    // Validate input
    ABCH5_CHECK_VALID( attr, "Dataspace::open() passed invalid attribute" );

    // Make it.
    hid_t ID = H5Aget_space( attr.id() );
    ABCH5_ASSERT( ID >= 0,
                  "Dataspace::open() H5Aget_space failed." );

    // Set it.
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
void Dataspace::open( const Dataset &dset )
{
    
    // Make sure we're not occupied.
    ABCH5_ASSERT( id() < 0,
                  "Cannot open Dataspace in existing." );

    // Validate input
    ABCH5_CHECK_VALID( dset, "Dataspace::open() passed invalid dataset" );

    // Make it.
    hid_t ID = H5Dget_space( dset.id() );
    ABCH5_ASSERT( ID >= 0,
                  "Dataspace::open() H5Dget_space failed." );

    // Set it.
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
// Destroy, close
Dataspace::~Dataspace()
{
    if ( valid() && owned() )
    {
        // std::cout << "Deleting dataspace: " << id();
        herr_t status = H5Sclose( id() );
        // std::cout << "." << std::endl;
        ABCH5_WARN( status >= 0,
                    "Dataspace::~Dataspace() H5Sclose() failed." );
    }
    setId( -1 );
    setOwned( false );
}

//-*****************************************************************************
void Dataspace::close()
{
    // Invalid to close a non-valid or non-owned object.
    ABCH5_CHECK_VALID( *this, "Cannot close invalid Dataspace" );
    ABCH5_ASSERT( owned(), "Cannot close non-owned Dataspace" );
    herr_t status = H5Sclose( id() );
    ABCH5_ASSERT( status >= 0,
                  "Dataspace::close() H5Sclose() failed." );
    setId( -1 );
    setOwned( false );
}

//-*****************************************************************************
void Dataspace::copyFrom( const Dataspace &other )
{
    // Check this.
    ABCH5_ASSERT( id() < 0,
                  "Cannot copy into Dataspace that exists" );

    // Validate input
    ABCH5_CHECK_VALID( other,
                       "Dataspace::copyFrom() passed invalid Dataspace" );

    hid_t ID = H5Scopy( other.id() );
    ABCH5_ASSERT( ID >= 0,
                  "Dataspace::copyFrom() H5Scopy() failed" );

    // Set
    setId( ID );
    setOwned( true );
}

//-*****************************************************************************
bool Dataspace::isScalar() const
{
    // Check this.
    ABCH5_CHECK_VALID( *this,
                       "Dataspace::isScalar() on invalid object." );
    H5S_class_t sclass = H5Sget_simple_extent_type( id() );
    ABCH5_ASSERT( sclass != H5S_NO_CLASS,
                  "Dataspace::isScalar() H5Sget_simple_extent_type failed." );
    return ( sclass == H5S_SCALAR );
}

//-*****************************************************************************
size_t Dataspace::getSimpleExtentDims( Dimensions &dims ) const
{
    // Check this.
    ABCH5_CHECK_VALID( *this,
                       "Dataspace::getSimpleExtentDims() on invalid object." );

#if 0
    H5S_class_t sclass = H5Sget_simple_extent_type( id() );
    ABCH5_ASSERT( sclass != H5S_NO_CLASS,
                  "Dataspace::getSimpleExtentDims() "
                  "H5Sget_simple_extent_class() failed." );
    if ( sclass == H5S_SIMPLE )
    {
        int rank = H5Sget_simple_extent_ndims( id() );
        ABCH5_ASSERT( rank >= 0,
                      "Dataspace::getSimpleExtentDims() "
                      "H5Sget_simple_extent_ndims() failed." );
        
        HDimensions hdims( ( size_t )rank );
        rank = H5Sget_simple_extent_dims( id(), hdims.rootPtr(), NULL );
        ABCH5_ASSERT( rank == hdims.size(),
                      "Dataspace::getSimpleExtentDims() "
                      "found inconsistent ranks." );
        dims = hdims;
        
        size_t npoints = hdims.numPoints();
        hssize_t npts = H5Sget_simple_extent_npoints( id() );
        ABCH5_ASSERT( npoints == ( size_t )npts,
                      "Dataspace::getSimpleExtentDims() "
                      "found inconsistent num points:" << npoints
                      << " VS " << npts );
        
        return npoints;
    }
    else
    {
        // Scalar = one point.
        dims.resize( 1 );
        dims[0] = 1;
        return 1;
    }
#else
    if ( isScalar() )
    {
        return 0;
    }
    
    int rank = H5Sget_simple_extent_ndims( id() );
    ABCH5_ASSERT( rank >= 0,
                  "Dataspace::getSimpleExtentDims() "
                  "H5Sget_simple_extent_ndims() failed." );
        
    HDimensions hdims( ( size_t )rank );
    rank = H5Sget_simple_extent_dims( id(), hdims.rootPtr(), NULL );
    ABCH5_ASSERT( rank == hdims.size(),
                  "Dataspace::getSimpleExtentDims() "
                  "found inconsistent ranks." );
    dims = hdims;
        
    size_t npoints = hdims.numPoints();
    hssize_t npts = H5Sget_simple_extent_npoints( id() );
    ABCH5_ASSERT( npoints == ( size_t )npts,
                  "Dataspace::getSimpleExtentDims() "
                  "found inconsistent num points:" << npoints
                  << " VS " << npts );
        
    return npoints;


#endif
}

//-*****************************************************************************
size_t Dataspace::getSimpleExtentNpoints() const
{
    // Check this.
    ABCH5_CHECK_VALID( *this,
                       "Dataspace::getSimpleExtentNpoints() "
                       "on invalid object." );
    
    hssize_t npts = H5Sget_simple_extent_npoints( id() );
    ABCH5_ASSERT( npts > 0,
                  "Dataspace::getSimpleExtentNpoints() "
                  "found degenerate num points." );

    return ( size_t )npts;
}

} // End namespace HDF5
} // End namespace Alembic

