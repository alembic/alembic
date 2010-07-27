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

#ifndef _AlembicHDF5_BufferedObject_h_
#define _AlembicHDF5_BufferedObject_h_

#include <AlembicHDF5/Datatype.h>

namespace AlembicHDF5 {

//-*****************************************************************************
// "BufferedObject" is the somewhat badly named base class for Attributes and
// Datasets, both of which store Data buffers with roughly the same API.
// The only difference is that Attributes have to be written all at once,
// but Datasets can be written in pieces, though we don't use this
// functionality. Attributes are for smallish amounts of data, Datasets are
// for largish amounts of data.
// Note that this is NOT derived from any BaseObjects.
class BufferedObject
{
protected:
    BufferedObject() {}
    virtual ~BufferedObject() {}

public:
    virtual void readAll(
        const Datatype &memType,
        void *intoBuffer ) const = 0;
    
    virtual void writeAll(
        const Datatype &memType,
        const void *outofBuffer ) = 0;
};

} // End namespace AlembicHDF5

#endif
