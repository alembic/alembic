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

#ifndef _AlembicHDF5_PropertyList_h_
#define _AlembicHDF5_PropertyList_h_

#include <AlembicHDF5/BaseObject.h>
#include <boost/smart_ptr.hpp>

namespace AlembicHDF5 {

//-*****************************************************************************
class PropertyList : public BaseObject
{
public:
    PropertyList() : BaseObject() {}
    
    // This wraps, does not copy.
    // Right now I don't bother to track whether something is 'built in'
    // or not.
    PropertyList( hid_t propId ) : BaseObject( propId, false ) {}

    PropertyList &operator=( hid_t propId );

    virtual ~PropertyList();
    virtual void close();

    // Copy functions. These are explicit.
    // This one copies from another property list.
    void copyFrom( const PropertyList &other = H5P_DEFAULT );
};

//-*****************************************************************************
class CreationOrderPlist : public PropertyList
{
public:
    CreationOrderPlist();
};

//-*****************************************************************************
class DsetGzipCreatePlist : public PropertyList
{
public:
    // Levels go from 0-9, 0 begin fast, less 9 being slow, more
    DsetGzipCreatePlist( const Dimensions &dims,
                         int level = 5 );
};

} // End namespace AlembicHDF5

#endif
