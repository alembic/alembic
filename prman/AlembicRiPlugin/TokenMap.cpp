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

#include "TokenMap.h"
#include <boost/tokenizer.hpp>

namespace AlembicRiPlugin {

//-*****************************************************************************
#define NULLDEF "__NULL__"
typedef boost::char_separator<char> Sep;
typedef boost::tokenizer<Sep> Tok;

//-*****************************************************************************
TokenMap::TokenMap( const std::string &str )
{
    static const Sep PairSep( ";" );
    static const Sep KeyValSep( "=" );

    Tok pairTok( str, PairSep );
    for ( Tok::iterator pairIter = pairTok.begin();
          pairIter != pairTok.end(); ++pairIter )
    {
        Tok kvTok( (*pairIter), KeyValSep );
        std::vector<std::string> strings;
        for ( Tok::iterator kvIter = kvTok.begin();
              kvIter != kvTok.end(); ++kvIter )
        {
            strings.push_back( (*kvIter) );
        }

        if ( strings.size() != 2 )
        {
            ABCRI_WARN( "Invalid token pair: " << (*pairIter) );
        }
        else
        {
            m_map[strings[0]] = strings[1];
        }
    }
}

//-*****************************************************************************
bool TokenMap::find( const std::string &name, std::string &into ) const
{
    NameValueMap::const_iterator iter = m_map.find( name );
    if ( iter != m_map.end() )
    {
        into = (*iter).second;
        return true;
    }
    return false;
}

//-*****************************************************************************
bool TokenMap::find( const std::string &name, float &into ) const
{
    NameValueMap::const_iterator iter = m_map.find( name );
    if ( iter != m_map.end() )
    {
        into = atof( (*iter).second.c_str() );
        return true;
    }
    return false;
}

//-*****************************************************************************
bool TokenMap::find( const std::string &name, ChronoSamples &into ) const
{
    std::string sampleString;
    if ( !find( name, sampleString ) )
    {
        return false;
    }
    
    static const Sep SampleSep( "," );
    Tok sampleTok( sampleString, SampleSep );
    
    for ( Tok::iterator iter = sampleTok.begin();
          iter != sampleTok.end() ; ++iter )
    {
        into.push_back( ( Abc::chrono_t )atof( (*iter).c_str() ) );
    }
    
    return true;
}

} // End namespace AlembicRiPlugin
