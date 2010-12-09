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
#include "ProcArgs.h"

#include <boost/tokenizer.hpp>

#include <vector>

//-*****************************************************************************
//INSERT YOUR OWN TOKENIZATION CODE AND STYLE HERE
ProcArgs::ProcArgs( RtString paramStr )
  : frame(0.0)
  , fps(24.0)
  , shutterOpen(0)
  , shutterClose(0)
{
    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;

    std::vector<std::string> tokens;
    std::string params( paramStr );

    Tokenizer tokenizer( params, Separator(" ") );
    for ( Tokenizer::iterator iter = tokenizer.begin(); iter != tokenizer.end() ;
          ++iter )
    {
        if ( (*iter).empty() ) { continue; }

        tokens.push_back( *iter );
    }

    for ( size_t i = 0; i < tokens.size(); ++i )
    {
        if ( tokens[i] == "-frame" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                frame = atof( tokens[i].c_str() );
            }
        }
        else if ( tokens[i] == "-fps" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                fps = atof( tokens[i].c_str() );
            }
        }
        else if ( tokens[i] == "-shutterOpen" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                shutterOpen = atof( tokens[i].c_str() );
            }
        }
        else if ( tokens[i] == "-shutterClose" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                shutterClose = atof( tokens[i].c_str() );
            }
        }
        else if ( tokens[i] == "-filename" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                filename = tokens[i];
            }
        }
        else if ( tokens[i] == "-objectpath" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                objectpath = tokens[i];
            }
        }
    }
}
