//
//  Copyright (c) 2009, Tweak Software
//  All rights reserved.
// 
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
//     * Neither the name of the Tweak Software nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
// 
//  THIS SOFTWARE IS PROVIDED BY Tweak Software ''AS IS'' AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL Tweak Software BE LIABLE FOR
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
//  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//

#ifndef _GtoContainer_Protocol_h_
#define _GtoContainer_Protocol_h_

#include <Gto/Header.h>
#include <iostream>
#include <string>

namespace GtoContainer {

//-*****************************************************************************
struct Protocol
{
    Protocol() : protocol( "" ), version( 0 ) {}
    Protocol( const std::string &p, Gto::uint32 v )
      : protocol( p ), version( v ) {}
    Protocol( const Protocol &copy )
      : protocol( copy.protocol ),
        version( copy.version ) {}

    Protocol &operator=( const Protocol &copy )
    {
        protocol = copy.protocol;
        version = copy.version;
        return *this;
    }
    
    std::string protocol;
    Gto::uint32 version;
};

} // End namespace GtoContainer

//-*****************************************************************************
inline bool operator==( const GtoContainer::Protocol &a,
                        const GtoContainer::Protocol &b )
{
    return ( a.protocol == b.protocol ) && ( a.version == b.version );
}


//-*****************************************************************************
inline bool operator!=( const GtoContainer::Protocol &a,
                        const GtoContainer::Protocol &b )
{
    return ( a.protocol != b.protocol ) || ( a.version != b.version );
}

//-*****************************************************************************
inline bool operator<( const GtoContainer::Protocol &a,
                       const GtoContainer::Protocol &b )
{
    if ( a.protocol == b.protocol )
    {
        return a.version < b.version;
    }
    else
    {
        return a.protocol < b.protocol;
    }
}

//-*****************************************************************************
inline bool operator>( const GtoContainer::Protocol &a,
                       const GtoContainer::Protocol &b )
{
    if ( a.protocol == b.protocol )
    {
        return a.version > b.version;
    }
    else
    {
        return a.protocol > b.protocol;
    }
}

//-*****************************************************************************
inline bool operator<=( const GtoContainer::Protocol &a,
                        const GtoContainer::Protocol &b )
{
    if ( a.protocol == b.protocol )
    {
        return a.version <= b.version;
    }
    else
    {
        return a.protocol < b.protocol;
    }
}

//-*****************************************************************************
inline bool operator>=( const GtoContainer::Protocol &a,
                        const GtoContainer::Protocol &b )
{
    if ( a.protocol == b.protocol )
    {
        return a.version >= b.version;
    }
    else
    {
        return a.protocol > b.protocol;
    }
}

//-*****************************************************************************
inline std::ostream &operator<<( std::ostream &str,
                                 const GtoContainer::Protocol &a )
{
    str << a.protocol << "(" << a.version << ")";
    return str;
}

#endif







