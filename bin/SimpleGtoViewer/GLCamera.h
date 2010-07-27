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

#ifndef _SimpleGtoViewer_GLCamera_h_
#define _SimpleGtoViewer_GLCamera_h_

#include "Foundation.h"

namespace SimpleGtoViewer {

//-*****************************************************************************
class GLCamera
{
public:
    GLCamera();
    ~GLCamera() {};
    
    // Executes OpenGL commands to show the current camera
    void apply() const;

    //-*************************************************************************
    // LOCAL TRANSFORM: Get, Set
    M44f transform() const;
    
    const V3f &rotation() const { return m_rotation; }
    void setRotation( const V3f &r ) { m_rotation = r; }

    const V3f &scale() const { return m_scale; }
    void setScale( const V3f &s ) { m_scale = s; }

    const V3f &translation() const { return m_translation; }
    void setTranslation( const V3f &t ) { m_translation = t; }

    float centerOfInterest() const { return m_centerOfInterest; }
    void setCenterOfInterest( float coi )
    {
        m_centerOfInterest = std::max( coi, 0.1f );
    }

    float fovy() const { return m_fovy; }
    void setFovy( float fvy ) { m_fovy = fvy; }

    int width() const { return m_size.x; }
    int height() const { return m_size.y; }
    
    void autoSetClippingPlanes( const B3f &bounds );
    void setClippingPlanes( float near, float far )
    {
        m_clip.x = near;
        m_clip.y = far;
    }

    //-*************************************************************************
    // UI Actions
    void track( const V2f &point );
    void dolly( const V2f &point, 
                float dollySpeed = 5.0f );
    void rotate( const V2f &point,
                 float rotateSpeed = 400.0f );

    void frame( const B3f &bounds );

    void lookAt( const V3f &eye, const V3f &at );

    void setSize( int w, int h )
    {
        m_size.x = w;
        m_size.y = h;
    }
    void setSize( const V2i &sze )
    {
        m_size = sze;
    }
        
protected:
    //*************************************************************************
    // DATA
    V3f m_rotation;
    V3f m_scale;
    V3f m_translation;
   
    float m_centerOfInterest;
    float m_fovy;
    V2f m_clip;
    V2i m_size;
    float m_aspect;
};

} // End namespace SimpleGtoViewer

#endif  
