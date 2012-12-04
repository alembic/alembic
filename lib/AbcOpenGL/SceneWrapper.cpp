//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

class SceneWrapper::_scn_impl {
public:
    _scn_impl(const char* fname) :
        m_scene(Scene(fname))
    {
    }

    void draw() {
        m_scene.draw(m_state);
    }

    Imath::Box<Imath::Vec3<double> > bounds() {
        return m_scene.getBounds();
    }

    Scene m_scene;
    SceneState m_state;
};

SceneWrapper::SceneWrapper(const char* abc_file_name):
    m_state(new _scn_impl(abc_file_name))
{}

void SceneWrapper::draw()
{
    m_state->draw();
}

Imath::Box<Imath::Vec3<double> > SceneWrapper::bounds()
{
    return m_state->bounds();
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
