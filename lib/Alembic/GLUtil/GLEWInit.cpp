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

#ifndef PLATFORM_DARWIN

#include <Alembic/GLUtil/GLEWInit.h>

namespace Alembic {
namespace GLUtil {

#ifdef ALEMBIC_GLEW_MX

//-*****************************************************************************
typedef std::map<void *,GLEWContext *> ContextMap;

//-*****************************************************************************
typedef std::map<void *,GLEWContextMap::PlatformContext *> PlatformMap;

//-*****************************************************************************
// Static, global maps.
static ContextMap       g_contextMap;
static PlatformMap      g_platformMap;

//-*****************************************************************************
// Initi static class members.
GLEWContext *GLEWContextMap::m_current = NULL;
GLEWContextMap::PlatformContext *GLEWContextMap::m_currentPlatform = NULL;

//-*****************************************************************************
void GLEWContextMap::postCreateGLContext( void* context )
{
    if ( context == NULL )
    {
        ABC_THROW( "GLEWContextMap::postCreateGLContext called "
                   "with null context" );
    }
    
    if ( g_contextMap.find( context ) != g_contextMap.end() )
    {
        ABC_THROW( "GLEWContextMap::postCreateGLContext called "
                   "with an already known context" );
    }

    GLEWContext *tmp = new GLEWContext;

    GLenum err = glewContextInit( tmp );
    if ( err != GLEW_OK )
    {
	delete tmp;
        ABC_THROW( "glewContextInit() failed: "
                   << ( const char * ) glewGetErrorString( err ) );
    }
    g_contextMap[context] = tmp;

    PlatformContext *tmp2 = new PlatformContext;

    err = glxewContextInit( tmp2 );
    if ( err != GLEW_OK )
    {
	delete tmp2;
        ABC_THROW( "glxewContextInit() failed: "
                   << ( const char * ) glewGetErrorString( err ) );
    }
    g_platformMap[context] = tmp2;
}

//-*****************************************************************************
void GLEWContextMap::preDestroyGLContext( void * context )
{
    ContextMap::iterator i = g_contextMap.find( context );
    PlatformMap::iterator j = g_platformMap.find( context );

    if ( i == g_contextMap.end() ||
         j == g_platformMap.end() )
    {
        ABC_THROW( "GLEWContextMap::preDestroyGLContext called "
                   "with an unregistered context" );
    }

    GLEWContext *tmp = i->second;
    g_contextMap.erase( i );
    delete tmp;

    PlatformContext *tmp2 = j->second;
    g_platformMap.erase( j );
    delete tmp2;

    if ( m_current == tmp ) { m_current = NULL; }
    if ( m_currentPlatform == tmp2 ) { m_currentPlatform = NULL; }
}

//-*****************************************************************************
void GLEWContextMap::postMakeCurrent( void *context )
{
    if ( context == NULL ) 
    {
        m_current = NULL;
        m_currentPlatform = NULL;
        return;
    }

    ContextMap::iterator i = g_contextMap.find( context );
    if ( i == g_contextMap.end() )
    {
        // do nothing with unregistered contices, not all are created
        // in such a way that AlembicGL knows about them.
        return;
    }

    PlatformMap::iterator j = g_platformMap.find( context );
    if ( j == g_platformMap.end() )
    {
        // do nothing with unregistered contices, not all are created
        // in such a way that AlembicGL knows about them.
        return;
    }

    m_current = i->second;
    m_currentPlatform = j->second;
}

#endif

//-*****************************************************************************
//-*****************************************************************************
// The simple GLEW init
//-*****************************************************************************
//-*****************************************************************************
#ifdef ALEMBIC_GLEW_MX

void GLEWInit()
{
    void *context = glXGetCurrentContext();
    GLEWContextMap::postCreateGLContext( context );
    GLEWContextMap::postMakeCurrent( context );
}

} // End namespace AlembicGL

#else

void GLEWInit()
{
    // Nothing
    //void *context = glXGetCurrentContext();
    //GLEWContextMap::postCreateGLContext( context );
    //GLEWContextMap::postMakeCurrent( context );
    glewInit();
}

#endif

} // End namespace GLUtil
} // End namespace Alembic


#ifdef ALEMBIC_GLEW_MX

//-*****************************************************************************
extern "C" GLEWContext *
glewGetContext()
{
    // return AlembicGL::GLEWContextMap::getContext();
    return NULL;
}

//-*****************************************************************************
extern "C" GLXEWContext *
glxewGetContext()
{
    // return AlembicGL::GLEWContextMap::getPlatformContext();
    return NULL;
}

#endif

#endif // PLATFORM_DARWIN
