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

#ifndef _Alembic_HDF5_Attribute_h_
#define _Alembic_HDF5_Attribute_h_

#include <Alembic/HDF5/BufferedObject.h>
#include <Alembic/HDF5/AttributedObject.h>
#include <Alembic/HDF5/PropertyList.h>
#include <Alembic/HDF5/Datatype.h>
#include <Alembic/HDF5/Dataspace.h>

namespace Alembic {
namespace HDF5 {

//-*****************************************************************************
// Attributes are buffered, but obviously not Attributed.
class Attribute : public NamedObject, public BufferedObject
{
public:
    Attribute() : NamedObject(), BufferedObject() {}

    // For creating
    Attribute( const AttributedObject &loc,
               const std::string &attrName,
               const Datatype &dtype,
               const Dataspace &dspace,
               bool doCheckExists = true )
      : NamedObject(), BufferedObject()
    {
        create( loc, attrName, dtype, dspace, doCheckExists );
    }

    // For opening by name.
    Attribute( const AttributedObject &loc,
               const std::string &attrName,
               bool doCheckExists = true )
      : NamedObject(), BufferedObject()
    {
        open( loc, attrName, doCheckExists );
    }

    // For opening by name & path
    Attribute( const AttributedObject &loc,
               const std::string &parentName,
               const std::string &attrName,
               bool doCheckExists = true )
      : NamedObject(), BufferedObject()
    {
        open( loc, parentName, attrName, doCheckExists );
    }

    // For opening by index.
    Attribute( const AttributedObject &loc,
               hsize_t index,

               H5_index_t idxType = H5_INDEX_CRT_ORDER,
               H5_iter_order_t idxOrder = H5_ITER_INC,
               const PropertyList &accessPlist = H5P_DEFAULT,
               const PropertyList &linkPlist = H5P_DEFAULT,
               bool doCheckExists = true )
      : NamedObject(), BufferedObject()
    {
        open( loc, index, idxType, idxOrder,
              accessPlist, linkPlist, doCheckExists );
    }

    // Destroy and close
    virtual ~Attribute();
    virtual void close();

    // create2
    void create( const AttributedObject &loc,
                 const std::string &attrName,
                 const Datatype &dtype,
                 const Dataspace &dspace,
                 bool doCheckExists = true );

    // open by name.
    void open( const AttributedObject &loc,
               const std::string &attrName,
               bool doCheckExists = true );

    // open by name and path
    void open( const AttributedObject &loc,
               const std::string &pathName,
               const std::string &attrName,
               bool doCheckExists = true );

    // Open by index.
    void open( const AttributedObject &loc,
               hsize_t index,

               H5_index_t idxType = H5_INDEX_CRT_ORDER,
               H5_iter_order_t idxOrder = H5_ITER_INC,
               const PropertyList &accessPlist = H5P_DEFAULT,
               const PropertyList &linkPlist = H5P_DEFAULT,
               bool doCheckExists = true );

    
    // xferPlist ignored..
    virtual void readAll( const Datatype &memType,
                          void *intoBuffer ) const;
    virtual void writeAll( const Datatype &memType,
                           const void *fromBuffer );

    // Read/write.
    void read( const Datatype &memType,
               void *intoBuffer ) const
    {
        Attribute::readAll( memType, intoBuffer );
    }

    void write( const Datatype &memType,
                const void *fromBuffer )
    {
        Attribute::writeAll( memType, fromBuffer );
    }
};  

} // End namespace HDF5
} // End namespace Alembic

#endif
