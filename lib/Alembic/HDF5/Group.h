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

#ifndef _Alembic_HDF5_Group_h_
#define _Alembic_HDF5_Group_h_

#include <Alembic/HDF5/ParentObject.h>
#include <Alembic/HDF5/PropertyList.h>

namespace Alembic {
namespace HDF5 {

//-*****************************************************************************
class Group : public ParentObject
{
public:
    Group() : ParentObject() {}

    // Open
    void open( const ParentObject &parent,
               const std::string &nme,
               const PropertyList &accessPlist = H5P_DEFAULT,
               bool doCheckExists = true );

    // Create
    void create( const ParentObject &parent,
                 const std::string &nme,
                 const PropertyList &linkPlist = H5P_DEFAULT,
                 const PropertyList &creationPlist = H5P_DEFAULT,
                 const PropertyList &accessPlist = H5P_DEFAULT,
                 bool doCheckExists = true );

    // Constructor which calls open
    Group( const ParentObject &parent,
           const std::string &nme,
           OpenConstructionFlag doOpen,
           const PropertyList &accessPlist = H5P_DEFAULT,
           bool doCheckExists = true )
      : ParentObject()
    {
        open( parent, nme, accessPlist, doCheckExists );
    }

    // Constructor which calls create
    Group( const ParentObject &parent,
           const std::string &nme,
           CreateConstructionFlag doCreate,
           const PropertyList &linkPlist = H5P_DEFAULT,
           const PropertyList &creationPlist = H5P_DEFAULT,
           const PropertyList &accessPlist = H5P_DEFAULT,
           bool doCheckExists = true )
      : ParentObject()
    {
        create( parent, nme, linkPlist, creationPlist, accessPlist,
                doCheckExists );
    }

    // Destructor. Will close an existing group.
    virtual ~Group();

    // Error to call closed on an unowned or invalid object.
    virtual void close();

    // Return that we are not a file object
    virtual bool isFileObject() const;
};

} // End namespace HDF5
} // End namespace Alembic

#endif
