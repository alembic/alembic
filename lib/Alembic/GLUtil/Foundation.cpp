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

#include <Alembic/GLUtil/Foundation.h>
#include <Alembic/GLUtil/GLEWInit.h>

namespace Alembic {
namespace GLUtil {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// FOR MAC STUFF
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
#ifdef PLATFORM_DARWIN

//-*****************************************************************************
void InitGL()
{
#if 0
    GLenum err = glewInit();
    if ( err != GLEW_OK )
    {
        ABC_THROW( "Alembic::GLUtil::InitGL() glewInit failed. Error: "
                   << glewGetErrorString( err ) );
    }
#endif
}

#else


//-*****************************************************************************
void InitGL()
{
    Alembic::GLUtil::GLEWInit();
}


#endif

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// ERROR CHECKING FUNCTIONS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// Checks framebuffer status.
// Copied directly out of the spec, modified to deliver a return value.
void CheckGLFramebufferStatus()
{
    GLenum status;
    status = ( GLenum )glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
    switch ( status )
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        return;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        ABC_THROW(
            "Framebuffer incomplete, incomplete attachment" );
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        ABC_THROW(
            "Unsupported framebuffer format" );
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        ABC_THROW(
            "Framebuffer incomplete, missing attachment" );
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        ABC_THROW(
            "Framebuffer incomplete, attached images "
            "must have same dimensions" );
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        ABC_THROW(
            "Framebuffer incomplete, attached images "
            "must have same format" );
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        ABC_THROW(
            "Framebuffer incomplete, missing draw buffer" );
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        ABC_THROW(
            "Framebuffer incomplete, missing read buffer" );
        break;
    }
    ABC_THROW( "Unknown GL Framebuffer error" );
}


//-*****************************************************************************
// Checks for OpenGL errors.
// Extremely useful debugging function: When developing,
// make sure to call this after almost every GL call.
void CheckGLErrors( const std::string &label )
{
    GLenum errCode;
    if ( ( errCode = glGetError() ) != GL_NO_ERROR )
    {
        ABC_THROW( "OpenGL Error: "
                   << ( const char * )gluErrorString( errCode )
                   << " (Label: " << label
                   << ")" );
    }
}

} // End namespace GLUtil
} // End namespace Alembic


