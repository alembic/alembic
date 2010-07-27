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

#include "Scenes.h"

namespace SimpleGtoViewer {

//-*****************************************************************************
Scenes::Scenes( const std::string &gtoFileBase,
                int startFrame,
                int endFrame,
                bool anim )
  : m_gtoFileBase( gtoFileBase ),
    m_startFrame( startFrame ),
    m_endFrame( endFrame ),
    m_anim( anim ),
    m_currentFrame( startFrame )
{
    if ( m_startFrame > m_endFrame ) { m_anim = false; }

    loadCurrentFrame();
}

//-*****************************************************************************
void Scenes::loadCurrentFrame()
{
    ScenePtr sp;
    if ( m_anim )
    {
        std::string fileName = ( boost::format( m_gtoFileBase ) %
                                 m_currentFrame ).str();
        
        if ( m_sceneMap.count( fileName ) > 0 )
        {
            sp = m_sceneMap[fileName];
            if ( !sp )
            {
                sp.reset( new Scene( fileName ) );
                m_sceneMap[fileName] = sp;
            }
        }
        else
        {
            sp.reset( new Scene( fileName ) );
            m_sceneMap[fileName] = sp;
        }
        
    }
    else
    {
        if ( m_sceneMap.count( m_gtoFileBase ) > 0 )
        {
            sp = m_sceneMap[m_gtoFileBase];
            if ( !sp )
            {
                sp.reset( new Scene( m_gtoFileBase ) );
                m_sceneMap[m_gtoFileBase] = sp;
            }
        }
        else
        {
            sp.reset( new Scene( m_gtoFileBase ) );
            m_sceneMap[m_gtoFileBase] = sp;
        }
    }
    
    m_currentScene = sp;
}

//-*****************************************************************************
void Scenes::tickForward()
{
    if ( m_anim )
    {
        m_currentFrame += 1;
        if ( m_currentFrame > m_endFrame )
        {
            m_currentFrame = m_startFrame;
        }
        loadCurrentFrame();
    }
}

//-*****************************************************************************
void Scenes::tickBackward()
{
    if ( m_anim )
    {
        m_currentFrame -= 1;
        if ( m_currentFrame < m_startFrame )
        {
            m_currentFrame = m_endFrame;
        }
        loadCurrentFrame();
    }
}

} // End namespace SimpleGtoViewer
