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

#ifndef _Alembic_Core_WriteSlab_h_
#define _Alembic_Core_WriteSlab_h_

#include <Alembic/Core/Foundation.h>
#include <Alembic/Core/DataType.h>

namespace Alembic {
namespace Core {

//-*****************************************************************************
// This is basically a bundling struct designed to unify the passing of data
// into the slab writer.
class WriteSlab
{
public:
    WriteSlab()
      : m_data( NULL ),
        m_dataType( kUnknownPOD, 0 ),
        m_dimensions( 0 ) {}
    
    WriteSlab( const void *Data,
               const DataType &dtype,
               const Dimensions &dim );

    WriteSlab( const WriteSlab &copy )
      : m_data( copy.m_data ),
        m_dataType( copy.m_dataType ),
        m_dimensions( copy.m_dimensions ) {}

    WriteSlab& operator=( const WriteSlab &copy )
    {
        m_data = copy.m_data;
        m_dataType = copy.m_dataType;
        m_dimensions = copy.m_dimensions;
        return *this;
    }

    const void *data() const { return m_data; }
    const DataType &dataType() const { return m_dataType; }
    const Dimensions &dimensions() const { return m_dimensions; }

protected:
    const void *m_data;
    DataType m_dataType;
    Dimensions m_dimensions;
};

} // End namespace Core
} // End namespace Alembic


#endif

