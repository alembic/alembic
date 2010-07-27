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

#include <AlembicAsset/Init.h>
#include <AlembicAsset/IAsset.h>
#include <AlembicAsset/IObject.h>
#include <AlembicAsset/IProperty.h>
#include <AlembicAsset/Body/IContextBody.h>
#include <AlembicAsset/Base/ITimeSampling.h>
#include <iostream>
#include <stdlib.h>

//-*****************************************************************************
struct indentor
{
    indentor() : v( 0 ), s( " " ) {}
    int v;
    std::string s;

    void operator++() { ++v; }
    void operator++(int) { ++v; }
    void operator--() { --v; }
    void operator--(int) { --v; }
};

inline std::ostream &operator<<( std::ostream &ostr, const indentor &idt )
{
    for ( int v = 0; v < idt.v; ++v )
    {
        ostr << idt.s;
    }
    return ostr;
}

static indentor g_indent;

//-*****************************************************************************
using namespace AlembicAsset;

//-*****************************************************************************
// Forwards.
std::ostream &operator<<( std::ostream &ostr, const IAsset &p );
std::ostream &operator<<( std::ostream &ostr, const IObject &p );
std::ostream &operator<<( std::ostream &ostr, const IProperty &p );

//-*****************************************************************************
// Output children
template <class PARENT>
void childrenOut( std::ostream &ostr, const PARENT &p )
{
    size_t numChildren = p.numChildren();
    if ( numChildren > 0 )
    {
        ostr << g_indent << "children: " << std::endl;
        ++g_indent;
        for ( size_t c = 0; c < numChildren; ++c )
        {
            IObject obj( p, c );
            ostr << obj;
        }          
        --g_indent;
        ostr << std::endl;
    }
}

//-*****************************************************************************
std::ostream &operator<<( std::ostream &ostr, const IAsset &p )
{
    ostr << g_indent << "-------------------------------" << std::endl
         << g_indent << "asset: " << p.fileName() << std::endl;
    ++g_indent;
    ostr << g_indent << "comments: " << p.comments() << std::endl;
    childrenOut( ostr, p );
    --g_indent;
    return ostr;
}

//-*****************************************************************************
std::ostream &operator<<( std::ostream &ostr, const IObject &p )
{
    ostr << g_indent << "-------------------------------" << std::endl
         << g_indent << "object: " << p.name()
         << "(" << p.protocol() << ")" << std::endl;
    ++g_indent;
    
    ostr << g_indent << "comments: " << p.comments() << std::endl;

    // Properties.
    size_t numProperties = p.numProperties();
    if ( numProperties > 0 )
    {
        ostr << g_indent << "properties: " << std::endl;
        ++g_indent;
        for ( size_t prop = 0; prop < numProperties; ++prop )
        {
            PropertyInfo pinfo = p.propertyInfo( prop );
            if ( pinfo )
            {
                if ( pinfo->ptype == kSingularProperty )
                {
                    ISingularProperty sprop( p, pinfo );
                    ostr << sprop;
                }
                else if ( pinfo->ptype == kMultiProperty )
                {
                    IMultiProperty mprop( p, pinfo );
                    ostr << mprop;
                }
                else
                {
                    ostr << "INVALID PROPERTY #: " << prop << std::endl;
                }
            }
            else
            {
                ostr << "INVALID PROPERTY #: " << prop << std::endl;
            }
        }
        --g_indent;
    }

    // Children
    childrenOut( ostr, p );
    --g_indent;
    return ostr;
}

//-*****************************************************************************
std::ostream &operator<<( std::ostream &ostr, const IProperty &p )
{
    ostr << g_indent << p.dataType() << " ";
    if ( p.isMulti() )
    {
        ostr << "MULTI ";
    }
    ostr << p.name() << "(" << p.protocol() << ")";
    if ( p.isAnimated() )
    {
        ostr << " ANIMATED ";
    }
    ostr << ";" << std::endl;
    return ostr;
}

//-*****************************************************************************
//-*****************************************************************************
// DO IT.
//-*****************************************************************************
//-*****************************************************************************
int main( int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        std::cerr << "USAGE: " << argv[0] << " <AlembicAsset.abc>"
                  << std::endl;
        exit( -1 );
    }

    // Scoped.
    {
        AlembicAsset::Init();
        IAsset asset( argv[1] );
        std::cout << asset << std::endl;
    }
    
    return 0;
}
