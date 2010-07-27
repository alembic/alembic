//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#include "AssetDrw.h"

namespace SimpleAbcViewer {

//-*****************************************************************************
AssetDrw::AssetDrw( const std::string & i_fileName )
  : ParentDrw(),
    m_fileName( i_fileName )
{
    // Try to load the file name.
    // Let the exceptions throw if there's a problem.
    std::cout << "About to try to create top-level transform"
              << std::endl;
    Atk::TransformReaderPtr xform(
        new Atk::TransformReader( i_fileName.c_str() ) );
    std::cout << "Totally did it!" << std::endl;

    // Just make sure.
    assert( xform );

    // And then set it, which will create children.
    std::cout << "About to try to set transform reader, which will build"
              << " children" << std::endl;
    setTakoTransformReader( xform );

    // That's it.
    // time to figure out what frames we have in our flat hierarchy
    PropertyPair frameProp;

    std::set < float > frames;

    if ( xform->readProperty( "frameRange", 0.0, frameProp )
         == PROPERTY_STATIC )
    {
        std::vector <float> & fr = boost::get< std::vector<float> >
            ( frameProp.first );
        frames.insert( fr.begin(), fr.end() );
    }
    else if ( xform->readProperty( "frameRangeShape", 0.0, frameProp )
              == PROPERTY_STATIC )
    {
        std::vector <float> & fr = boost::get< std::vector<float> >
            ( frameProp.first );
        frames.insert( fr.begin(), fr.end() );
    }

    // for the static case
    if ( frames.empty() || frames.count( FLT_MAX ) > 0 )
    {
        m_startFrame = m_endFrame = 0;
        frames.insert( FLT_MAX );
    }
    else
    {
        m_startFrame = (*(frames.begin()));
        m_endFrame = (*(frames.rbegin()));
    }

    if ( m_startFrame > m_endFrame )
    {
        FSD_THROW( "Invalid frame range in file: "
                   << m_fileName << ": " << m_startFrame
                   << " to " << m_endFrame );
    }

    std::cout << "Scene: " << m_fileName << " read. " << std::endl
              << "Start frame: " << m_startFrame << std::endl
              << "End frame: " << m_endFrame << std::endl;
}

//-*****************************************************************************
AssetDrw::~AssetDrw()
{
    // Nothing
}

} // End namespace SimpleAbcViewer
