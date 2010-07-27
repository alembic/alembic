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

#ifndef _SimpleGtoViewer_Scenes_h_
#define _SimpleGtoViewer_Scenes_h_

#include "Foundation.h"
#include "Scene.h"

namespace SimpleGtoViewer {

//-*****************************************************************************
class Scenes
{
public:
    Scenes( const std::string &gtoFileBase,
            int startFrame,
            int endFrame,
            bool anim );

    void draw( SceneState &s_state ) { return m_currentScene->draw( s_state ); }
    const B3f &bounds() const { return m_currentScene->bounds(); }

    void tickForward();
    void tickBackward();

protected:
    void loadCurrentFrame();
    
    std::string m_gtoFileBase;
    int m_startFrame;
    int m_endFrame;
    bool m_anim;
    
    typedef boost::shared_ptr<Scene> ScenePtr;
    typedef std::map<std::string,ScenePtr> SceneMap;

    int m_currentFrame;
    ScenePtr m_currentScene;
    SceneMap m_sceneMap;
};

} // End namespace SimpleGtoViewer

#endif
