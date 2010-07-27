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

#ifndef _Alembic_PdbIO_Source_h_
#define _Alembic_PdbIO_Source_h_

#include <Alembic/PdbIO/Foundation.h>

namespace Alembic {
namespace PdbIO {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// For Writing, we have a SOURCE class. Override the virtuals to implement
// whatever you want.
class Source
{
public:
    Source() {}
    virtual ~Source() {}

    // Called when writing begins. It will pass you an empty ObjectInfo
    // object - which is just a lightweight struct defined above - and
    // you fill in the ObjectInfo.
    virtual void beginObjectWrite( ObjectInfo &obj )
    {
        // Meaningless empty object
        obj.numParticles = 0;
        obj.numAttributes = 0;
        obj.swapped = false;
    }

    // Called when an attribute write begins.
    // User must fill the AttributeInfo structure "attr" with
    // the attribute's name and type,
    // and return a read-only buffer of data, of size
    // attr.bytesTotal( obj.numParticles ), which is the data to be written.
    // Though it is odd that you'd want to skip an attribute's data,
    // returning NULL will cause the attribute to be filled with 0.
    virtual const char *beginAttributeWrite( const ObjectInfo &obj,
                                             AttributeInfo &attr )
    {
        attr.name = "emptyAttribute";
        attr.type = k_IntAttribute;
        return NULL;
    }

    // Called when an attribute write ends. Same info as attribute write
    // begin. No action required, just a heads up.
    virtual void endAttributeWrite( const ObjectInfo &obj,
                                    const AttributeInfo &attr ) {}

    // Called when writing ends.
    virtual void endObjectWrite( const ObjectInfo &obj ) {}
};

} // End namespace PdbIO
} // End namespace Alembic

#endif
