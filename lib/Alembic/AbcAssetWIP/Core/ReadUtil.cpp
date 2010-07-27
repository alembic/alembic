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

#include <Alembic/Core/ReadUtil.h>
#include <Alembic/Core/Assert.h>

namespace Alembic {
namespace Core {

//-*****************************************************************************
void ReadString( const H5A &attr,
                 std::string &into )
{
    // For scalar strings, we just use H5T_C_S1 x bytes
    // don't bother checking. yet.
    H5T dtype( attr );
    H5S dspace( attr );
    
    ABC_CORE_ASSERT( dspace.isScalar(),
                     "ReadString() called on non-scalar attribute: "
                     << attr.name() );
    
    size_t len = dtype.getSize();
    std::vector<char> sbuf( len + 1, ( char )0 );
    attr.readAll( dtype, ( void * )&sbuf.front() );
    into = ( const char * )&sbuf.front();
}

//-*****************************************************************************
void ReadString( const H5AttrObj &parent,
                 const std::string &attrName,
                 std::string &into )
{
    H5A attr( parent, attrName );
    ReadString( attr, into );
}

//-*****************************************************************************
void ReadString( const H5AttrObj &grandparent,
                 const std::string &parentName,
                 const std::string &attrName,
                 std::string &into)
{
    H5A attr( grandparent, parentName, attrName );
    ReadString( attr, into );
}

} // End namespace Core
} // End namespace Alembic
