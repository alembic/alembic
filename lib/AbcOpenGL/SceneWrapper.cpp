//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#include <AbcOpenGL/SceneWrapper.h>
#include <AbcOpenGL/Scene.h>
#include <AbcOpenGL/GLCamera.h>
#include <ctime>

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

//-*****************************************************************************
static Timer g_playbackTimer;

class SceneWrapper::_scn_impl {
public:
    _scn_impl( const std::string &fileName, bool verbose )
        : m_scene( Scene( fileName, verbose ) )
        , m_currentSeconds( m_scene.getMinTime() )
    {
    }

    std::string selection( int x, int y, GLCamera &camera ) {
        return m_scene.selection(x, y, camera, m_state);
    }

    void drawBounds( const int mode ) {
        m_scene.drawBounds(m_state, mode);
    }

    void draw( bool visibleOnly, bool boundsOnly ) {
        m_scene.draw(m_state, visibleOnly, boundsOnly);
    }

    void playForward(int fps) {
        if ( g_playbackTimer.elapsed() > 1.0f / fps )
        {
            g_playbackTimer.restart();
            if ( m_scene.isConstant() ) { return; }
            m_secondsPerFrame = 1.0f / fps;
            m_currentSeconds += m_secondsPerFrame;
            if ( m_currentSeconds > m_scene.getMaxTime() )
            {
                m_currentSeconds = m_scene.getMinTime();
            }
            m_scene.setTime( m_currentSeconds );
        }
    }

    IArchive getArchive() {
        return m_scene.getArchive();
    }

    IObject getTop() {
        return m_scene.getTop();
    }

    chrono_t getMinTime() {
        return m_scene.getMinTime();
    }

    chrono_t getMaxTime() {
        return m_scene.getMaxTime();
    }

    chrono_t getCurrentTime() {
        return m_currentSeconds;
    }

    bool isConstant() {
        return m_scene.isConstant();
    }

    void setTime(chrono_t newTime) {
        m_scene.setTime(newTime);
    }

    Imath::Box<Imath::Vec3<double> > bounds() {
        return m_scene.getBounds();
    }

    void frame(const Box3d &bounds) {
        m_state.cam.frame(bounds);
    }

    Scene m_scene;
    SceneState m_state;
    chrono_t m_framesPerSecond;
    chrono_t m_secondsPerFrame;
    chrono_t m_currentSeconds;
};

SceneWrapper::SceneWrapper( const std::string &fileName, bool verbose ):
    m_state(new _scn_impl( fileName, verbose ))
{}

std::string SceneWrapper::selection(int x, int y, GLCamera &camera)
{
    return m_state->selection(x, y, camera);
}

void SceneWrapper::drawBounds(const int mode)
{
    m_state->drawBounds(mode);
}

void SceneWrapper::draw(bool visibleOnly, bool boundsOnly)
{
    m_state->draw(visibleOnly, boundsOnly);
}

void SceneWrapper::setTime(chrono_t newTime)
{
    m_state->setTime(newTime);
}

void SceneWrapper::playForward(int fps)
{
    m_state->playForward( fps );
}

IArchive SceneWrapper::getArchive()
{
    return m_state->getArchive();
}

IObject SceneWrapper::getTop()
{
    return m_state->getTop();
}

chrono_t SceneWrapper::getMinTime()
{
    return m_state->getMinTime();
}

chrono_t SceneWrapper::getMaxTime()
{
    return m_state->getMaxTime();
}

chrono_t SceneWrapper::getCurrentTime()
{
    return m_state->getCurrentTime();
}

bool SceneWrapper::isConstant()
{
    return m_state->isConstant();
}

Imath::Box<Imath::Vec3<double> > SceneWrapper::bounds()
{
    return m_state->bounds();
}

void SceneWrapper::frame(const Box3d &bounds)
{
    m_state->frame(bounds);
}

double_vec
SceneWrapper::min_bounds() {
    Imath::Vec3<double> v = m_state->bounds().min;
    double_vec d;
    d.xyz[0] = v.x;
    d.xyz[1] = v.y;
    d.xyz[2] = v.z;
    return d;
}

double_vec
SceneWrapper::max_bounds() {
    Imath::Vec3<double> v = m_state->bounds().max;
    double_vec d;
    d.xyz[0] = v.x;
    d.xyz[1] = v.y;
    d.xyz[2] = v.z;
    return d;
}

} // End namespace ABCOPENGL_VERSION_NS
} // End namespace AbcOpenGL
