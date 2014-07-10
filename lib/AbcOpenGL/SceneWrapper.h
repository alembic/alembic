//-*****************************************************************************
//
// Copyright (c) 2013,
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

#ifndef _AbcOpenGL_SceneWrapper_h_
#define _AbcOpenGL_SceneWrapper_h_

#include <sstream>
#include "Foundation.h"
#include <AbcOpenGL/GLCamera.h>

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {
    
//-*****************************************************************************
struct double_vec {
    float xyz[3];
};

//-*****************************************************************************
class SceneWrapper {
public:
    SceneWrapper( const std::string &fileName, bool verbose = false );

    std::string selection(int x, int y, GLCamera &camera);
    void drawBounds( const int mode = GL_LINES );
    void draw(bool visibleOnly = false, bool boundsOnly = false);

    void setTime(chrono_t newTime);
    void playForward(int fps);
    bool isConstant();

    Imath::Box<Imath::Vec3<double> > bounds();

    void frame(const Box3d &bounds);

    chrono_t getMinTime();
    chrono_t getMaxTime();
    chrono_t getCurrentTime();
    IArchive getArchive();
    IObject getTop();

    double_vec min_bounds();
    double_vec max_bounds();

protected:
    class _scn_impl;
    _scn_impl* m_state;

    std::string m_filename;
};

} // End namespace ABCOPENGL_VERSION_NS

using namespace ABCOPENGL_VERSION_NS;

} // End namespace AbcOpenGL

#endif
