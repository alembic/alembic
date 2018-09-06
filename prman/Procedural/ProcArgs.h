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

#ifndef Alembic_Prman_ProcArgs_h
#define Alembic_Prman_ProcArgs_h

#define PRMAN_USE_ABCMATERIAL


#include <string>
#include <map>
#include <vector>

#include <ri.h>

#include <boost/shared_ptr.hpp>

//-*****************************************************************************
struct ProcArgs
{
    //constructor parses
    ProcArgs( RtString paramStr, bool fromReference = false );

    //copy constructor
    ProcArgs( const ProcArgs &rhs )
    : filename( rhs.filename )
    , objectpath( rhs.objectpath )
    , frame( rhs.frame )
    , fps( rhs.fps )
    , shutterOpen( rhs.shutterOpen )
    , shutterClose( rhs.shutterClose )
    , motionBegin( rhs.motionBegin )
    , motionEnd( rhs.motionEnd )
    , excludeXform( false )
    , flipv ( false )
    
    , filename_defined(false)
    , objectpath_defined(false)
    , frame_defined(false)
    , fps_defined(false)
    , shutterOpen_defined(false)
    , shutterClose_defined(false)
    , motionBegin_defined(false)
    , motionEnd_defined(false)
    , excludeXform_defined(false)
    , flipv_defined(false)
    {}
    
    void usage();
    
    //member variables
    std::string filename;
    std::string objectpath;
    double frame;
    double fps;
    double shutterOpen;
    double shutterClose;
    double motionBegin;
    double motionEnd;
    bool excludeXform;
    bool flipv;
    
    std::string getResource( const std::string & name );
    bool userMotionBlockDefined();
    
private:
    
    void applyArgs(ProcArgs & args);
    
    bool filename_defined;
    bool objectpath_defined;
    bool frame_defined;
    bool fps_defined;
    bool shutterOpen_defined;
    bool shutterClose_defined;
    bool motionBegin_defined;
    bool motionEnd_defined;
    bool excludeXform_defined;
    bool flipv_defined;
    
    typedef std::map<std::string, std::string> StringMap;
    typedef boost::shared_ptr<StringMap> StringMapRefPtr;
    typedef std::vector<StringMapRefPtr> StringMapRefPtrVector;
    
    StringMapRefPtrVector resourceSearchPath;
    
    
};

#endif
