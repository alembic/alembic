//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#include <Alembic/Util/Exception.h>
#include <Alembic/Util/Naming.h>

namespace Alembic {
namespace Util {
namespace ALEMBIC_VERSION_NS {

bool isStandardStartChar( const char c )
{
    // exclude names starting with numbers
    return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) || c == '_';
}

bool isStandardChar( const char c )
{
    return isStandardStartChar( c ) || ( c >= '0' && c <= '9' );
}

bool isStandardName( const std::string & name )
{
    size_t length = name.size();

    if ( length == 0 || !isStandardStartChar( name[0] ) )
    {
        return false;
    }

    for ( size_t i=1; i<length; ++i )
    {
        if ( !isStandardChar( name[i] ) )
        {
            return false;
        }
    }

    return true;
}

void makeStandardName( std::string& name, const char fixChar )
{
    if ( !isStandardStartChar( fixChar ) )
    {
        ALEMBIC_THROW( "Cannot fix bad name (bad fix character)." );
    }

    size_t length = name.size();
    if ( length == 0 || name[0] == 0 )
    {
        name = fixChar;
        return;
    }

    if ( !isStandardStartChar( name[0] ) )
    {
        name[0] = fixChar;
    }

    for ( size_t ii=1; ii<length; ++ii )
    {
        if ( !isStandardChar( name[ii] ) )
        {
            name[ii] = fixChar;
        }
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Util
} // End namespace Alembic
