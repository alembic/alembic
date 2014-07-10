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

#ifndef _AbcOpenGL_Scene_h_
#define _AbcOpenGL_Scene_h_

#include "Foundation.h"
#include "GLCamera.h"
#include "Drawable.h"
#include <ctime>

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

//-*****************************************************************************
void setMaterials( float o, bool negMatrix );

// global object index for GL picking
extern std::vector<std::string> OBJECT_MAP;

//-*****************************************************************************
class Timer
{
public:
	Timer() { Init(); }
	void Init() { m_start = clock(); }
	clock_t elapsed() const { return (clock() - m_start); }
    void restart() { m_start = clock(); }

private:
    Timer(const Timer& t);
	void operator=(const Timer& t);
	clock_t m_start;
};

//-*****************************************************************************
struct SceneState
{
    GLCamera cam;
    float pointSize;
};

//-*****************************************************************************
class Scene
{
public:
    //! Load a scene from the alembic archive given by the filename.
    //! ...
    Scene( const std::string &abcFileName, bool verbose = true );
    IArchive getArchive() { return m_archive; }
    IObject getTop() { return m_topObject; }

    //! Return the filename of the archive
    //! ...
    const std::string &getFileName() const { return m_fileName; }

    //! Return the min time, in seconds.
    //! ...
    chrono_t getMinTime() const { return m_minTime; }

    //! Return the max time, in seconds.
    //! ...
    chrono_t getMaxTime() const { return m_maxTime; }

    //! Return whether it's animated.
    //! ...
    bool isConstant() const { return m_minTime >= m_maxTime; }

    //! Cause the drawable state to be loaded to the given time.
    //! ...
    void setTime( chrono_t newTime );

    //! Return the bounds at the current time.
    //! ...
    Box3d getBounds() const { return m_bounds; }

    //! Returns selected object ids
    //! ...
    int processHits( GLint hits, GLuint buffer[] );
    std::string selection( int x, int y, GLCamera &camera, SceneState &s_state );

    //! This draws, assuming a camera matrix has already been set.
    //! ...
    void drawBounds( SceneState &s_state, const int mode = GL_LINES );
    void draw( SceneState &s_state, bool visibleOnly = false, 
                                    bool boundsOnly = false);

protected:
    std::string m_fileName;
    IArchive m_archive;
    IObject m_topObject;

    chrono_t m_minTime;
    chrono_t m_maxTime;
    Box3d m_bounds;

    DrawablePtr m_drawable;
};

} // End namespace ABCOPENGL_VERSION_NS

using namespace ABCOPENGL_VERSION_NS;

} // End namespace AbcOpenGL

#endif
