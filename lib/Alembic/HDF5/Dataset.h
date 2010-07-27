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

#ifndef _Alembic_HDF5_Dataset_h_
#define _Alembic_HDF5_Dataset_h_

#include <Alembic/HDF5/BufferedObject.h>
#include <Alembic/HDF5/AttributedObject.h>
#include <Alembic/HDF5/ParentObject.h>
#include <Alembic/HDF5/Datatype.h>
#include <Alembic/HDF5/Dataspace.h>
#include <Alembic/HDF5/PropertyList.h>

namespace Alembic {
namespace HDF5 {

//-*****************************************************************************
// Normally I frown on multiple inheritance, but here it was hard to do it
// any other way. Datasets can have attributes, and they are really
// needed a lot of the time.
class Dataset : public AttributedObject, public BufferedObject
{
public:
    Dataset() : AttributedObject(), BufferedObject() {}

    // For creating
    Dataset( const ParentObject &loc,
             const std::string &attrName,
             const Datatype &dtype,
             const Dataspace &dspace,
             const PropertyList &linkPlist = H5P_DEFAULT,
             const PropertyList &createPlist = H5P_DEFAULT,
             const PropertyList &accessPlist = H5P_DEFAULT,
             bool doCheckExists = true )
      : AttributedObject(), BufferedObject()
    {
        create( loc, attrName, dtype, dspace,
                linkPlist, createPlist, accessPlist,
                doCheckExists );
    }

    // For opening.
    Dataset( const ParentObject &loc,
             const std::string &attrName,
             const PropertyList &accessPlist = H5P_DEFAULT,
             bool doCheckExists = true )
      : AttributedObject(), BufferedObject()
    {
        open( loc, attrName, accessPlist, doCheckExists );
    }

    // For opening from a reference.
    Dataset( const ParentObject &loc,
             hobj_ref_t refId )
      : AttributedObject(), BufferedObject()
    {
        openReference( loc, refId );
    }

    // Destroy and close
    virtual ~Dataset();
    virtual void close();

    // create2
    void create( const ParentObject &loc,
                 const std::string &attrName,
                 const Datatype &dtype,
                 const Dataspace &dspace,
                 const PropertyList &linkPlist = H5P_DEFAULT,
                 const PropertyList &createPlist = H5P_DEFAULT,
                 const PropertyList &accessPlist = H5P_DEFAULT,
                 bool doCheckExists = true );

    // open
    void open( const ParentObject &loc,
               const std::string &attrName,
               const PropertyList &accessPlist = H5P_DEFAULT,
               bool doCheckExists = true );

    // open reference
    void openReference( const ParentObject &loc,
                        hobj_ref_t refId );
    
    // For reading and writing the whole dset.
    virtual void readAll( const Datatype &memType,
                          void *intoBuffer ) const;
    virtual void writeAll( const Datatype &memType,
                           const void *fromBuffer );

    // Read/write. These allow for reading and writing
    // partial dataset data. 
    void read( const Datatype &memType,
               const Dataspace &memSpace,
               const Dataspace &fileSpace,
               const PropertyList &xferPlist,
               void *intoBuffer ) const;

    void write( const Datatype &memType,
                const Dataspace &memSpace,
                const Dataspace &fileSpace,
                const PropertyList &xferPlist,
                const void *fromBuffer );
};  

} // End namespace HDF5
} // End namespace Alembic

#endif
