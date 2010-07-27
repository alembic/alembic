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

#ifndef _Alembic_GLUtil_GLEWInit_h_
#define _Alembic_GLUtil_GLEWInit_h_

#ifndef PLATFORM_DARWIN

#include <Alembic/GLUtil/Foundation.h>

namespace Alembic {
namespace GLUtil {

#ifdef ALEMBIC_GLEW_MX

//-*****************************************************************************
// This maps GL contexts to GLEW contexts.
class GLEWContextMap 
{
public:
    typedef GLXEWContext PlatformContext;
    
    static GLEWContext*     getContext() { return m_current; }
    static PlatformContext* getPlatformContext() { return m_currentPlatform; }
    static void             postCreateGLContext( void* glContext );
    static void             preDestroyGLContext( void* glContext );
    static void             postMakeCurrent( void* glContext );

private:
    static GLEWContext*     m_current;
    static PlatformContext* m_currentPlatform;
};

#endif

//-*****************************************************************************
// Simple GLEW init for our purposes.
void GLEWInit();

} // End namespace GLUtil
} // End namespace Alembic

#endif // PLATFORM_DAWIN

#endif // _Alembic_GLUtil_GLEWInit_h_
