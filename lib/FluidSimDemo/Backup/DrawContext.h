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

#ifndef _FluidSimDemo_DrawContext_h_
#define _FluidSimDemo_DrawContext_h_

#include <FluidSimDemo/Foundation.h>

namespace FluidSimDemo {

//-*****************************************************************************
class DrawContext
{
public:
    DrawContext()
      : m_drawInitStateObjects( false ),
        m_drawCollisionObjects( false ),
        m_drawGeometry( true )
    {
        m_transform.makeIdentity();
    }

    const M44d &transform() const { return m_transform; }
    void setTransform( const M44d & i_xf ) { m_transform = i_xf; }

    const M44d &cameraTransform() const { return m_cameraTransform; }
    void setCameraTransform( const M44d & i_xf )
    { m_cameraTransform = i_xf; }

    bool drawInitStateObjects() const { return m_drawInitStateObjects; }
    bool drawCollisionObjects() const { return m_drawCollisionObjects; }
    bool drawGeometry() const { return m_drawGeometry; }

    void setDrawInitStateObjects( bool tf ) { m_drawInitStateObjects = tf; }
    void setDrawCollisionObjects( bool tf ) { m_drawCollisionObjects = tf; }
    void setDrawGeometry( bool tf ) { m_drawGeometry = tf; }

protected:
    M44d m_transform;
    M44d m_cameraTransform;
    bool m_drawInitStateObjects;
    bool m_drawCollisionObjects;
    bool m_drawGeometry;
};

} // End namespace FluidSimDemo

#endif
