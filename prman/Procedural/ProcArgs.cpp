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
#include <map>

#include <fstream>
#include <streambuf>

namespace
{
    typedef boost::shared_ptr<ProcArgs> ProcArgsSharedPtr;
    typedef std::map<std::string, ProcArgsSharedPtr> ProcArgsMap;
    
    ProcArgsMap g_argsFileCache;
    
    ProcArgsSharedPtr readArgsFile( const std::string & filePath )
    {
        ProcArgsMap::iterator I = g_argsFileCache.find( filePath );
        
        if ( I != g_argsFileCache.end() )
        {
            return (*I).second;
        }
        
        std::string workString;
        
        try
        {
            std::ifstream fs( filePath.c_str() );
            
            fs.seekg( 0, std::ios::end );
            workString.reserve( fs.tellg() );
            fs.seekg( 0, std::ios::beg );
            
            workString.assign(
                    ( std::istreambuf_iterator<char>( fs ) ),
                    std::istreambuf_iterator<char>() );
        }
        catch ( const std::exception & e )
        {
            std::cerr << "AlembicRiProcedural: cannot read arguments file: ";
            std::cerr << filePath << std::endl;
            
            g_argsFileCache[filePath] = ProcArgsSharedPtr();
            return ProcArgsSharedPtr();
        }
        
        ProcArgsSharedPtr result(
                new ProcArgs( const_cast<char*>( workString.c_str() ),
                        true ) );
        
        g_argsFileCache[filePath] = result;
        
        return result;
    }

}


//-*****************************************************************************
//INSERT YOUR OWN TOKENIZATION CODE AND STYLE HERE
ProcArgs::ProcArgs( RtString paramStr, bool fromReference )
  : frame(0.0)
  , fps(24.0)
  , shutterOpen(0)
  , shutterClose(0)
  , excludeXform(false)
  , flipv(false)
  , motionBegin(0)
  , motionEnd(0)
{
    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;

    std::vector<std::string> tokens;
    std::string params( paramStr );
    
    
    Tokenizer tokenizer( params, Separator(" \n\t") );
    for ( Tokenizer::iterator iter = tokenizer.begin(); iter != tokenizer.end() ;
          ++iter )
    {
        if ( (*iter).empty() ) { continue; }

        tokens.push_back( *iter );
    }
    
    bool lastResourceWasLocal = true;
    
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
                frame_defined = true;
            }
        }
        else if ( token == "-fps" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                fps = atof( tokens[i].c_str() );
                fps_defined = true;
            }
        }
        else if ( token == "-shutteropen" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                shutterOpen = atof( tokens[i].c_str() );
                shutterOpen_defined = true;
            }
        }
        else if ( token == "-shutterclose" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                shutterClose = atof( tokens[i].c_str() );
                shutterClose_defined = true;
            }
        }
        else if ( token == "-motionbegin" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                motionBegin = atof( tokens[i].c_str() );
                motionBegin_defined = true;
            }
        }
        else if ( token == "-motionend" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                motionEnd = atof( tokens[i].c_str() );
                motionEnd_defined = true;
            }
        }
        else if ( token == "-filename" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                filename = tokens[i];
                filename_defined = true;
            }
        }
        else if ( token == "-objectpath" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                objectpath = tokens[i];
                objectpath_defined = true;
            }
        }
        else if ( token == "-excludexform" )
        {
            excludeXform = true;
            excludeXform_defined = true;
            
        }
        else if ( token == "-flipv" )
        {
            flipv = true;
            flipv_defined = true;
        }
        else if ( token == "-argsfile" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                if ( ProcArgsSharedPtr args = 
                        readArgsFile( tokens[i] ) )
                {
                    size_t beforeSize = resourceSearchPath.size();
                    
                    applyArgs( *args );
                    
                    if ( beforeSize != resourceSearchPath.size() )
                    {
                        lastResourceWasLocal = false;
                    }
                }
            }
        }
        else if ( token == "-resource" )
        {
            ++i;
            if ( i < tokens.size() )
            {
                std::string objectName = tokens[i];
                ++i;
                if ( i < tokens.size() )
                {
                    std::string resourceName = tokens[i];
                    
                    if ( resourceSearchPath.empty() || !lastResourceWasLocal )
                    {
                        resourceSearchPath.push_back( StringMapRefPtr(
                                new StringMap ) );
                        lastResourceWasLocal = true;
                    }
                    
                    (*resourceSearchPath.back())[objectName] = resourceName;
                }
            }
        }
        
        
        
    }
    
    if ( filename.empty() && !fromReference )
    {
        usage();
    }
}

void ProcArgs::applyArgs(ProcArgs & args)
{
    if ( args.filename_defined )
    {
        filename = args.filename;
        filename_defined = true;
    }
    
    if ( args.objectpath_defined )
    {
        objectpath = args.objectpath;
        objectpath_defined = true;
    }
    
    if ( args.frame_defined )
    {
        frame = args.frame;
        frame_defined = true;
    }
    
    if ( args.fps_defined )
    {
        fps = args.fps;
        fps_defined = true;
    }
    
    if ( args.shutterOpen_defined )
    {
        shutterOpen = args.shutterOpen;
        shutterOpen_defined = true;
    }
    
    if ( args.shutterClose_defined )
    {
        shutterClose = args.shutterClose;
        shutterClose_defined = true;
    }
    
    if ( args.motionBegin_defined )
    {
        motionBegin = args.motionBegin;
        motionBegin_defined = true;
    }
    
    if ( args.motionEnd_defined )
    {
        motionEnd = args.motionEnd;
        motionEnd_defined = true;
    }
    
    if ( args.excludeXform_defined )
    {
        excludeXform = args.excludeXform;
        excludeXform_defined = true;
    }
    
    if ( args.flipv_defined )
    {
        flipv = args.flipv;
        flipv_defined = true;
    }
    
    if ( !args.resourceSearchPath.empty() )
    {
        resourceSearchPath.reserve(
                resourceSearchPath.size() + args.resourceSearchPath.size() );
        
        resourceSearchPath.insert( resourceSearchPath.end(),
                args.resourceSearchPath.begin(), args.resourceSearchPath.end() );
    }
    
}
    
bool ProcArgs::userMotionBlockDefined() {
    return motionBegin_defined || motionEnd_defined;
}

std::string ProcArgs::getResource( const std::string & name )
{
    for ( StringMapRefPtrVector::reverse_iterator I = 
            resourceSearchPath.rbegin(), E = resourceSearchPath.rend();
                    I != E; ++I )
    {
        StringMapRefPtr & entry = (*I);
        
        if ( entry )
        {
            StringMap::iterator I = entry->find(name);
            if ( I != entry->end() )
            {
                return (*I).second;
            }
        }
    }
    
    return "";
}


void ProcArgs::usage()
{
    std::cerr << "AlembicRiProcedural usage:" << std::endl;
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
                 "leaf locations within an externally defined hierarchy. "
                 "If the path points to a single \"faceset\" object directly "
                 "beneath a subdivision mesh, it'll add \"hole\" tags for "
                 "faces not contained within the \"faceset.\"";
    std::cerr << std::endl;
    std::cerr << std::endl;

    std::cerr << "-excludexform" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "If specified, no transformation statements will be written "
                 "and AttributeBegin blocks and identifiers will only be "
                 "created around geometric primitives. The default behavior "
                 "is to write all transformations and include AttributeBegin "
                 "blocks around each level of the hierarchy.";
    std::cerr << std::endl;
    std::cerr << std::endl;
    
    
    
    std::cerr << "-flipv" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "Like many systems (Maya included), AbcGeom's default "
                 "interpretation of texture coordinates is of a bottom-left "
                 "corner origin. RenderMan's texture lookups assume a top-left "
                 "corner origin by default. If your shaders are not already "
                 "compensating for this, enabling -flipv applies (1-v) to "
                 "texture coordinates of polymesh and subdmesh primitives.";
    std::cerr << std::endl;
    std::cerr << std::endl;
    
    
    std::cerr << "-motionbegin 0.0" << std::endl;
    std::cerr << "-motionend 0.0" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "Use motionbegin and/or motionend to specify the MotionBlock "
                 "window to map the Alembic samples to. This allows for RIBs "
                 "that use Shutter ranges other than frame-relative values. "
                 "The Alembic sample times are remapped using a standard fit ";
    std::cerr << std::endl;
    std::cerr << "motion_time = (x-a)*(d-c)/(b-a) + c " << std::endl;
    std::cerr << "Where - x = alembic sample time "  << std::endl;
    std::cerr << "        a = requested shutter open time (frame+shutteropen)*fps " << std::endl;
    std::cerr << "        b = requested shutter close time " << std::endl;
    std::cerr << "        c = motionbegin " << std::endl;
    std::cerr << "        d = motionend " << std::endl;
    std::cerr << "motionbegin must be less than motionend or the motionbegin/end "
                 "functionality will be ignored.";
    std::cerr << std::endl;
    std::cerr << std::endl;
    
   
    std::cerr << "-resource nameOrPath resourceName" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "For each occurance of this flag, resourceName is added to "
                 "an internal map using nameOrPath as a key. When traversing the "
                 "scene, the full path and base name of each object (in that "
                 "order) are looked up from this map. If it contains a value, "
                 "it will emit an RiResource call to \"restore\" the resource "
                 "block of that name with the expectation that a resource "
                 "block of that name has been declared externally. Also, "
                 "unless an -objectpath flag is pointing directly at a FaceSet "
                 "object beneath a SubD, this test will be done for each "
                 "FaceSet. Any matches at the FaceSet level will be added as "
                 "\"faceedit\" tags to the resulting "
                 "RiHierarchicalSubdivisionMesh call. This is useful for "
                 "associating shading information with individual shapes and "
                 "faces.";
    
    std::cerr << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "-argsfile /path/to/some/file" << std::endl;
    std::cerr << std::endl;
    
    std::cerr << "This allows arguments to be stored and shared in external files. "
                 "The files are parsed once and cached so that they may be shared "
                 "between invocations of the procedural. They are applied in the "
                 "same order and manner as if the contents of the file appeared "
                 "in place of the -argsfile flag. This is mostly useful for defining "
                 "and sharing large blocks of -resource flags without redundant "
                 "parsing or copying of the parsed results.";
    
    std::cerr << std::endl;
    std::cerr << std::endl;
    
}

