//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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
#include <algorithm>
#include <iostream>

//-*****************************************************************************
//INSERT YOUR OWN TOKENIZATION CODE AND STYLE HERE
ProcArgs::ProcArgs( const char * paramStr )
  : frame(0.0)
  , fps(24.0)
  , shutterOpen(0)
  , shutterClose(0)
  , excludeXform(false)
  , makeInstance(false)
  , subdIterations(0)
  , proceduralNode(0)
{
    // TODO, grab the shutter a camera attached to AiUniverse if present
    
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
        std::string token = tokens[i];
        std::transform( token.begin(), token.end(), token.begin(), ::tolower );

        if ( token == "-frame" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                frame = atof( tokens[i].c_str() );
            }
        }
        else if ( token == "-fps" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                fps = atof( tokens[i].c_str() );
            }
        }
        else if ( token == "-shutteropen" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                shutterOpen = atof( tokens[i].c_str() );
            }
        }
        else if ( token == "-shutterclose" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                shutterClose = atof( tokens[i].c_str() );
            }
        }
        else if ( token == "-filename" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                filename = tokens[i];
            }
        }
        else if ( token == "-nameprefix" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                nameprefix = tokens[i];
            }
        }
        else if ( token == "-objectpath" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                objectpath = tokens[i];
            }
        }
        else if ( token == "-excludexform" )
        {
            excludeXform = true;
            
        }
        else if ( token == "-subditerations" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                subdIterations = atoi( tokens[i].c_str() );
            }
        }
        else if ( token == "-makeinstance" )
        {
            makeInstance = true;
        }
        
    }
}


void ProcArgs::usage()
{
    std::cerr << "AlembicArnoldProcedural usage:" << std::endl;
    std::cerr << std::endl;
    
    
    std::cerr << "-filename /path/to/some/archive.abc" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "This is the only required argument. "
                 "It has no default value." << std::endl;
    std::cerr << std::endl;
    

    std::cerr << "-frame 42" << std::endl;
    std::cerr << std::endl;

    std::cerr << "The frame number to load from within the archive. "
                 "The default value is 0. This is combined with -fps to map "
                 "to Alembic time units (double-precision seconds).";
    
    std::cerr << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "-fps 24" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "Combined with -frame above. The default value is 24.0.";
    std::cerr << std::endl;
    std::cerr << std::endl;
    
    
    std::cerr << "-shutteropen 0.0" << std::endl;
    std::cerr << "-shutterclose 0.5" << std::endl;
    std::cerr << std::endl;


    std::cerr << "These are frame-relative values which specify the shutter "
                 "window. The procedural will include all samples present in "
                 "the archive which are relevant to the shutter window. "
                 "The default value of both is 0.0 (no motion blur).";
    std::cerr << std::endl;
    std::cerr << std::endl;


    std::cerr << "-objectpath /assetroot/characters" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "If specified, only objects at or below the provided path "
                 "(within the archive) will be emitted. When combined with "
                 "-excludexform, this can also be used to load individual "
                 "leaf locations within an externally defined hierarchy. Be "
                 "aware that in that case, you'd need to set the \"matrix\" "
                 "and \"inherit_xform\" parameters on the procedural node "
                 "itself. If the path points to a single \"faceset\" object "
                 "directly beneath a polymesh or subdivision mesh, it'll add "
                 "a \"face_visibility\" user data array.";
    std::cerr << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "-excludexform" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "If specified, the \"matrix\" parameter will not be set on "
                 "the resulting primitive nodes." << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "-subditerations 2" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "For AbcGeom::ISubD objects, this option specifies the "
                 "\"subdiv_iterations\" value. It currently has no effect for "
                 "other primitive types. The default value is 0.";
    std::cerr << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "-nameprefix some_prefix__" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "Because node names are unique scene-wide in arnold, this "
                 "allows you control potential name clashes when loading or "
                 "instancing an archive (or multiple equivalently named "
                 "archives) multiple times. The default name of each node is "
                 "its full path within the alembic archive.";
    std::cerr << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "-makeinstance" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "This behavior is disabled by default. If enabled, the "
                 "procedural will attempt to identify identical primitives "
                 "(using Alembic's per-array-property hash keys) and create "
                 "corresponding \"ginstance\" nodes. Two primitives are "
                 "considered equivalent if the keys of their relevant point "
                 "position samples match along with any specified "
                 "subdivision values. This works across multiple archives or "
                 "invokations of the procedural. It currently does not write "
                 "unique user data per instance but will likely do so "
                 "automatically (when necessary) in a future release. "
                 "The ray visibility of the source primitive will be set to "
                 "AI_RAY_NONE and the \"ginstance\" node's will be set to "
                 "that of the calling \"procedural\" node.";
    
    std::cerr << std::endl;

    
}
