//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#ifndef _SimpleAbcViewer_Foundation_h_
#define _SimpleAbcViewer_Foundation_h_

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/Util/All.h>

#include <ImathMath.h>
#include <ImathVec.h>
#include <ImathMatrix.h>
#include <ImathBox.h>
#include <ImathQuat.h>
#include <ImathColor.h>
#include <ImathFun.h>
#include <ImathBoxAlgo.h>

#include <boost/format.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <exception>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

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

#ifdef PLATFORM_WINDOWS
#include <GL/wglew.h>
#include <GL/glut.h>
#else
#include <GL/glxew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/glut.h>

#endif // ifdef WIN64

#endif // ifdef PLATFORM_DARWIN

namespace SimpleAbcViewer {

//-*****************************************************************************
namespace Abc = Alembic::AbcGeom;
using namespace Abc;

//-*****************************************************************************
//-*****************************************************************************
// FUNCTIONS
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
template <class T>
inline T degrees( const T &rads ) { return 180.0 * rads / M_PI; }

//-*****************************************************************************
template <class T>
inline T radians( const T &degs ) { return M_PI * degs / 180.0; }

//-*****************************************************************************
template <class T>
inline const T &clamp( const T &x, const T &lo, const T &hi )
{
    return x < lo ? lo : x > hi ? hi : x;
}


//-*****************************************************************************
//-*****************************************************************************
// GL ERROR CHECKING
//-*****************************************************************************
//-*****************************************************************************
inline void GL_CHECK( const std::string &header = "" )
{
    GLenum errCode = glGetError();
    if ( errCode != GL_NO_ERROR )
    {
        std::cerr << "GL ERROR in " << header << ": "
                  << ( const char * )gluErrorString( errCode )
                  << std::endl;

        GLint matrixStackDepth = 0;
        glGetIntegerv( GL_MODELVIEW_STACK_DEPTH, &matrixStackDepth );
        std::cout << "Stack depth: " << ( int )matrixStackDepth
                  << std::endl;
    }
}

#if 0

//-*****************************************************************************
#define GL_NOISY( CODE )                        \
do                                              \
{                                               \
    CODE ;                                      \
    GL_CHECK( BOOST_PP_STRINGIZE( CODE ) );     \
    std::cout << "EXECUTED:"                    \
              << BOOST_PP_STRINGIZE( CODE )     \
              << std::endl;                     \
}                                               \
while( 0 )

#else

//-*****************************************************************************
#define GL_NOISY( CODE )                        \
do                                              \
{                                               \
    CODE ;                                      \
    std::string msg = "Code: ";                 \
    msg += BOOST_PP_STRINGIZE( CODE );          \
    GL_CHECK( msg );                            \
}                                               \
while( 0 )

#endif



} // End namespace SimpleAbcViewer

#endif
