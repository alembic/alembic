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

#ifndef _SimpleFsdViewer_Foundation_h_
#define _SimpleFsdViewer_Foundation_h_

#include <Alembic/Tako/Tako.h>
#include <Alembic/GLUtil/GLUtil.h>
#include <Alembic/Util/Util.h>

#include <FluidSimDemo/SceneReader.h>

#include <OpenEXR/ImathMath.h>
#include <OpenEXR/ImathVec.h>
#include <OpenEXR/ImathMatrix.h>
#include <OpenEXR/ImathBox.h>
#include <OpenEXR/ImathQuat.h>
#include <OpenEXR/ImathColor.h>
#include <OpenEXR/ImathFun.h>
#include <OpenEXR/ImathBoxAlgo.h>

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

namespace SimpleFsdViewer {

//-*****************************************************************************
namespace Abc = Alembic;
namespace Atk = Alembic::Tako;

//-*****************************************************************************
typedef Imath::V3f V3f;
typedef Imath::V3d V3d;
typedef Imath::V2d V2d;
typedef Imath::M44f M44f;
typedef Imath::M44d M44d;
typedef Imath::Box3f Box3f;
typedef Imath::Box3d Box3d;
typedef Imath::V2i V2i;

typedef Atk::index_t index_t;

typedef Imath::Vec3<unsigned short> V3us;
typedef Imath::Vec3<unsigned int> V3ui;

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



} // End namespace SimpleFsdViewer
#endif
