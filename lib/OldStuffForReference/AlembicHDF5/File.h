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

#ifndef _AlembicHDF5_File_h_
#define _AlembicHDF5_File_h_

#include <AlembicHDF5/ParentObject.h>
#include <AlembicHDF5/PropertyList.h>

namespace AlembicHDF5 {

//-*****************************************************************************
class File : public ParentObject
{
public:
    File() : ParentObject() {}

    // Open
    void open( const std::string &fileNme,
               unsigned int flags = H5F_ACC_RDONLY,
               const PropertyList &accessPlist = H5P_DEFAULT );

    // Create. Right now does not support anything other than default
    // creation & access types, but will add as needed.
    void create( const std::string &fileNme,
                 unsigned int flags = H5F_ACC_TRUNC,
                 const PropertyList &creationPlist = H5P_DEFAULT,
                 const PropertyList &accessPlist = H5P_DEFAULT );

    // Constructor which calls open
    File( const std::string &fNme,
          OpenConstructionFlag doOpen,
          unsigned int flags = H5F_ACC_RDONLY,
          const PropertyList &accessPlist = H5P_DEFAULT )
      : ParentObject()
    {
        open( fNme, flags, accessPlist );
    }

    // Constructor which calls create
    File( const std::string &fNme,
          CreateConstructionFlag doCreate,
          unsigned int flags = H5F_ACC_TRUNC,
          const PropertyList &creationPlist = H5P_DEFAULT,
          const PropertyList &accessPlist = H5P_DEFAULT )
    {
        create( fNme, flags, creationPlist, accessPlist );
    }

    // Destructor. Will close an existing file.
    virtual ~File();

    // Error to call closed on an unowned or invalid object.
    virtual void close();

    static void flush( BaseObject &object,
                       H5F_scope_t scope = H5F_SCOPE_GLOBAL );

    void flush( H5F_scope_t scope = H5F_SCOPE_GLOBAL )
    {
        File::flush( *this, scope );
    }

    // We are a file object
    virtual bool isFileObject() const;
};

} // End namespace AlembicHDF5

#endif
