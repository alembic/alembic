//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#ifndef _Alembic_AbcCoreAbstract_StaticScalarPropertyWriter_h_
#define _Alembic_AbcCoreAbstract_StaticScalarPropertyWriter_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace V0001 {

//-*****************************************************************************
//! \brief A static scalar property has only one sample, and it is a scalar
//! instance of a datatype. This can be stored in the file with great
//! simplicity. The property still has metadata.
class StaticScalarPropertyWriter
{
public:
    virtual ~StaticScalarPropertyWriter();

    // Scalar property metadata.
    virtual const std::string &getName() const = 0;
    virtual const DataType &getDataType() const = 0;
    virtual const MetaData &getMetaData() const = 0;

    // Read the sample into the memory address specified
    // by the void pointer.
    // The number of bytes will be determined by the datatype
    // above.
    virtual void setSample( const void *iFrom ) = 0;
};

} // End namespace V0001
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif
