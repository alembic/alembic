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
#include <iostream>
#include <stdlib.h>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace AlembicAsset;

namespace ba = boost::algorithm;

static std::string g_assetName;

//-*****************************************************************************
// Forwards.
std::ostream &operator<<( std::ostream &ostr, const IAsset &p );
std::ostream &operator<<( std::ostream &ostr, const IObject &p );

//-*****************************************************************************
bool keepPathElem( const std::string &prev, const std::string &next )
{

    bool keep = ( ( prev == "Alembic" && next == "Children" ) ||
                  ( prev == "Children" && next == prev ) );

    return keep;
}

//-*****************************************************************************
const std::string hdfPathToAlembic( const std::string &p )
{
    std::vector<std::string> pvec;
    std::vector<std::string> nvec;
    nvec.push_back( "" );

    ba::split( pvec, p,  ba::is_any_of( "/" ) );

    if ( pvec.size() < 2 ) { return "/"; }

    for ( size_t i = 1 ; i < pvec.size() ; ++i )
    {
        if ( i == ( pvec.size() - 1 ) ) // the last element is wanted
        {
            nvec.push_back( pvec[i] );
        }
        else if ( keepPathElem( pvec[i - 1], pvec[i + 1] ) )
        {
            nvec.push_back( pvec[i] );
        }
    }

    return ba::join( nvec, "/" );
}

//-*****************************************************************************
// Output children
template <class PARENT>
void childrenOut( std::ostream &ostr, const PARENT &p )
{
    size_t numChildren = p.numChildren();
    if ( numChildren > 0 )
    {
        for ( size_t c = 0; c < numChildren; ++c )
        {
            ostr << g_assetName << " ";
            IObject obj( p, c );
            ostr << obj;
        }
    }
}

//-*****************************************************************************
std::ostream &operator<<( std::ostream &ostr, const IAsset &p )
{
    childrenOut( ostr, p );
    return ostr;
}

//-*****************************************************************************
std::ostream &operator<<( std::ostream &ostr, const IObject &p )
{
    std::string prot = p.protocol();
    if ( prot == "" ) { prot = "__NO_PROT__"; }

    ostr << hdfPathToAlembic( p.fullPathName() ) << " " << prot
         << std::endl;

    // Children
    childrenOut( ostr, p );
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
        g_assetName = asset.fileName();
        std::cout << asset << std::endl;
    }

    return 0;
}
