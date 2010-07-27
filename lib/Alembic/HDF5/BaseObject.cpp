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

#include <Alembic/HDF5/BaseObject.h>

namespace Alembic {
namespace HDF5 {

//-*****************************************************************************
std::string BaseObject::comment() const
{
    ABCH5_CHECK_VALID( *this,
                       "BaseObject::comment() called on invalid object" );

    ssize_t bufSize = H5Oget_comment( id(), NULL, 0 );
    ABCH5_ASSERT( bufSize >= 0,
                  "BaseObject::comment() H5Oget_comment failed." );
    if ( bufSize == 0 )
    {
        return std::string( "" );
    }

    std::vector<char> buf( bufSize + 10 );
    bufSize = H5Oget_comment( id(), &buf.front(), buf.size() );
    ABCH5_ASSERT( bufSize > 0,
                  "BaseObject::comment() H5Oget_comment failed(2)." );

    return std::string( ( const char * )&buf.front() );
}

//-*****************************************************************************
void BaseObject::setComment( const std::string &cmt )
{
    ABCH5_CHECK_VALID( *this,
                       "BaseObject::setComment() called on invalid object" );
    herr_t status = H5Oset_comment( id(), cmt.c_str() );
    ABCH5_ASSERT( status >= 0,
                  "BaseObject::setComment() H5Oset_comment failed" );
}

} // End namespace HDF5
} // End namespace Alembic

