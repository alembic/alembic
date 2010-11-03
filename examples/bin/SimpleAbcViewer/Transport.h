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

#ifndef _SimpleAbcViewer_Transport_h_
#define _SimpleAbcViewer_Transport_h_

#include "Foundation.h"
#include "Scene.h"

namespace SimpleAbcViewer {

//-*****************************************************************************
class Transport
{
public:
    Transport( const std::string &iAbcFileName,
               chrono_t iFps );

    void draw( SceneState &iState )
    {
        return m_scene.draw( iState );
    }
    
    Box3d getBounds() const
    {
        return m_scene.getBounds();
    }

    void tickForward()
    {
        if ( m_scene.isConstant() ) { return; }
        
        m_currentSeconds += m_secondsPerFrame;                           
        
        if ( m_currentSeconds > m_scene.getMaxTime() )
        {
            m_currentSeconds = m_scene.getMinTime();
        }

        m_scene.setTime( m_currentSeconds );
    }
    
    void tickBackward()
    {
        if ( m_scene.isConstant() ) { return; }
        
        m_currentSeconds -= m_secondsPerFrame;                           
        
        if ( m_currentSeconds < m_scene.getMinTime() )
        {
            m_currentSeconds = m_scene.getMaxTime();
        }

        m_scene.setTime( m_currentSeconds );
    }

    const std::string &getFileName() const
    { return m_scene.getFileName(); }
    
    int getCurrentFrame() const
    {
        return ( int )floor( 0.5 +
                             ( m_currentSeconds * m_framesPerSecond ) );
    }
    
protected:
    Scene m_scene;
    
    chrono_t m_framesPerSecond;
    chrono_t m_secondsPerFrame;
    chrono_t m_currentSeconds;
};

} // End namespace SimpleAbcViewer

#endif
