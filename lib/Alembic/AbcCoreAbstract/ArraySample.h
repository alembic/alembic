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

#ifndef _Alembic_AbcCoreAbstract_ArraySample_h_
#define _Alembic_AbcCoreAbstract_ArraySample_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/ReadOnlyData.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
//! The ArraySample class is a lightweight wrapper around a block of bytes
//! that is managed elsewhere. This is basically just a named pair of
//! Dimensions and ReadOnlyBytes. It does not own, nor purport to own,
//! the data stored herein, it is merely a binding structure.
class ArraySample
{
public:
    //! Default constructor creates NULL bytes with degenerate dimensions.
    //! ...
    ArraySample()
      : m_bytes(),
        m_dimensions() {}

    //! Explicit constructor takes bytes and dims by reference
    //! and creates its own reference to them.
    ArraySample( const ReadOnlyBytes & iBytes,
                 const Dimensions & iDims )
      : m_bytes( iBytes ),
        m_dimensions( iDims ) {}

    //! Copy constructor - just a standard copy of the internal
    //! elements
    ArraySample( const ArraySample & iCopy )
      : m_bytes( iCopy.m_bytes ),
        m_dimensions( iCopy.m_dimensions ) {}

    //! Assignment - standard copy,
    //! returning *this.
    ArraySample& operator=( const ArraySample & iCopy )
    {
        m_bytes = iCopy.m_bytes;
        m_dimensions = iCopy.m_dimensions;
        return *this;
    }

    //! Return the bytes
    //! ...
    const ReadOnlyBytes& getBytes() const { return m_bytes; }

    //! Return the dimensions
    //! ...
    const Dimensions& getDimensions() const { return m_dimensions; }

private:
    ReadOnlyBytes m_bytes;  
    Dimensions m_dimensions;
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif
