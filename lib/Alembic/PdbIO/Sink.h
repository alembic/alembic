//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic, nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
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

#ifndef _Alembic_PdbIO_Sink_h_
#define _Alembic_PdbIO_Sink_h_

#include <Alembic/PdbIO/Foundation.h>

namespace Alembic {
namespace PdbIO {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// For Reading, we have a SINK class. Override the virtuals to
// implement whatever you want.
class Sink
{
public:
    Sink() {}
    virtual ~Sink() {}

    // Called when reading begins. It will pass you a filled ObjectInfo
    // object - which is just a lightweight struct defined above - and
    // you init whatever you need to.
    virtual void beginObjectRead( const ObjectInfo &obj ) {}
    
    // Called when an attribute begins. User must return a buffer,
    // attr.bytesTotal( obj.numParticles ) in size,
    // which will be filled with float32_t's,
    // float32_t * 3's, or int32_t , depending on the attribute type.
    // If you want to skip an attribute (ignore it), simply return NULL.
    virtual char *beginAttributeRead( const ObjectInfo &obj,
                                      const AttributeInfo &attr )
    { return NULL; }

    // Called when an attribute read ends. Same info as attribute read
    // begin. No action required, just a heads up.
    virtual void endAttributeRead( const ObjectInfo &obj,
                                   const AttributeInfo &attr ) {}

    // Called when reading ends.
    virtual void endObjectRead( const ObjectInfo &obj ) {}
};

} // End namespace PdbIO
} // End namespace Alembic

#endif
