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

#ifndef _Alembic_MD5Hash_Digest_h_
#define _Alembic_MD5Hash_Digest_h_

#include <Alembic/MD5Hash/Foundation.h>

namespace Alembic {
namespace MD5Hash {

//-*************************************************************************
// Digest class. This is what the MD5 produces. it is handy!
struct MD5Digest
{
    union
    {
        UCHAR d[16];
        UINT4 words[4];
    };
    
    MD5Digest() { words[0] = words[1] = words[2] = words[3] = 0; }
    MD5Digest( const MD5Digest &copy )
    {
        words[0] = copy.words[0];
        words[1] = copy.words[1];
        words[2] = copy.words[2];
        words[3] = copy.words[3];
    }
    
    MD5Digest &operator=( const MD5Digest &copy )
    {
        words[0] = copy.words[0];
        words[1] = copy.words[1];
        words[2] = copy.words[2];
        words[3] = copy.words[3];
        return *this;
    }
    
    UCHAR& operator[]( size_t i ) { return d[i]; }
    UCHAR operator[]( size_t i ) const { return d[i]; }
    
    void print( std::ostream &ostr ) const
    {
        for ( int i = 0; i < 16; ++i )
        {
            ostr << ( boost::format( "%02x" ) % ( int )(d[i]) );
        }
    }
    
    std::string str() const
    {
        std::stringstream sstr;
        print( sstr );
        return sstr.str();
    }
};

//-*****************************************************************************
inline bool operator==( const MD5Digest &a, const MD5Digest &b )
{
    return ( ( a.words[0] == b.words[0] ) &&
             ( a.words[1] == b.words[1] ) &&
             ( a.words[2] == b.words[2] ) &&
             ( a.words[3] == b.words[3] ) );
}

//-*****************************************************************************
inline bool operator!=( const MD5Digest &a, const MD5Digest &b )
{
    return ( ( a.words[0] != b.words[0] ) ||
             ( a.words[1] != b.words[1] ) ||
             ( a.words[2] != b.words[2] ) ||
             ( a.words[3] != b.words[3] ) );
}

//-*****************************************************************************
// Now there's something to expand.
inline bool operator<( const MD5Digest &a, const MD5Digest &b )
{
    return ( a.words[0] < b.words[0] ? true :
             ( a.words[0] > b.words[0] ? false :
               ( a.words[1] < b.words[1] ? true :
                 ( a.words[1] > b.words[1] ? false :
                   ( a.words[2] < b.words[2] ? true :
                     ( a.words[2] > b.words[2] ? false :
                       ( a.words[3] < b.words[3] ) ) ) ) ) ) );
}

//-*****************************************************************************
inline bool operator>( const MD5Digest &a, const MD5Digest &b )
{
    return ( a.words[0] > b.words[0] ? true :
             ( a.words[0] < b.words[0] ? false :
               ( a.words[1] > b.words[1] ? true :
                 ( a.words[1] < b.words[1] ? false :
                   ( a.words[2] > b.words[2] ? true :
                     ( a.words[2] < b.words[2] ? false :
                       ( a.words[3] > b.words[3] ) ) ) ) ) ) );
}

//-*****************************************************************************
inline bool operator<=( const MD5Digest &a, const MD5Digest &b )
{
    return ( a < b ) || ( a == b );
}

//-*****************************************************************************
inline bool operator>=( const MD5Digest &a, const MD5Digest &b )
{
    return ( a > b ) || ( a == b );
}

//-*****************************************************************************
inline std::ostream &operator<<( std::ostream &ostr, const MD5Digest &a )
{
    a.print( ostr );
    return ostr;
}

} // End namespace MD5Hash
} // End namespace Alembic

#endif
