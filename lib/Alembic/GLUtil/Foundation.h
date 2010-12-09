//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#ifndef _Alembic_GLUtil_Foundation_h_
#define _Alembic_GLUtil_Foundation_h_

#include <Alembic/Util/All.h>

#include <boost/algorithm/string.hpp>

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>
#include <map>
#include <utility>

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MAC INCLUDES
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
#ifdef PLATFORM_DARWIN

//#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// NON-MAC INCLUDES (Configured for multiple-context GLEW)
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
#else


#ifdef ALEMBIC_GLEW_MX

#define GLEW_MX
#include <GL/glew.h>
extern "C" GLEWContext *glewGetContext();

#else

#include <GL/glew.h>

#endif // ifdef ALEMBIC_GLEW_MX

#include <GL/gl.h>

#ifdef WIN32
#include <GL/wglew.h>
#endif
#ifdef WIN64
#include <GL/wglew.h>

#else

#include <GL/glxew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/glut.h>

#endif // ifdef WIN64

#endif // ifdef PLATFORM_DARWIN

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// FUNCTIONS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

namespace Alembic {
namespace GLUtil {

//-*****************************************************************************
// Initializes GLEW, but not GLUT. Single context on mac.
void InitGL();

// Some error checking functions.
void CheckGLErrors( const std::string &label = "" );
void CheckGLFramebufferStatus();

// Glut ones we'll just pass along.

} // End namespace GLUtil
} // End namespace Alembic

#endif
