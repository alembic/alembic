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

#ifndef _AlembicHDF5_Dataspace_h_
#define _AlembicHDF5_Dataspace_h_

#include <AlembicHDF5/BaseObject.h>

namespace AlembicHDF5 {

//-*****************************************************************************
class Attribute;
class Dataset;

//-*****************************************************************************
// For now dataspaces are all simple. (Actually, in HDF5, all dataspaces
// are simple).
class Dataspace : public BaseObject
{
public:
    Dataspace() : BaseObject() {}
    Dataspace( hid_t built_in ) : BaseObject( built_in, false ) {}

    void createScalar();
    void createSimple( const Dimensions &dims );
    void createSimple( const Dimensions &dims,
                       const Dimensions &maxDims );
    
    // Open from attributes and datasets.
    void open( const Attribute &attr );
    void open( const Dataset &dset );

    
    //-************************************************************************
    // For compile-time selection of the scalar constructor
    enum kScalarFlag { kScalar };

    // Create scalar
    explicit Dataspace( kScalarFlag )
      : BaseObject() { createScalar(); }

    // Create simple, with no max dims.
    Dataspace( const Dimensions &dims )
      : BaseObject()
    {
        if ( dims.rank() == 0 )
        {
            createScalar();
        }
        else
        {
            createSimple( dims );
        }
    }

    // Create simple, with max dims.
    Dataspace( const Dimensions &dims,
               const Dimensions &maxDims )
      : BaseObject() { createSimple( maxDims ); }

    // Create from attributes and datasets
    Dataspace( const Attribute &attr ) : BaseObject() { open( attr ); }
    Dataspace( const Dataset &dset ) : BaseObject() { open( dset ); }

    // Destroy, close
    virtual ~Dataspace();
    virtual void close();

    // Explicit copyFrom
    void copyFrom( const Dataspace &other );

    // Return whether or not the dataspace is scalar.
    bool isScalar() const;

    // Return dimensions.
    // both functions return the total number of points.
    // This is a slight deviation from the H5S API.
    size_t getSimpleExtentDims( Dimensions &dims ) const;
    size_t getSimpleExtentNpoints() const;
};

} // End namespace AlembicHDF5

#endif
