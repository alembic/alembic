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

#include "GLCamera.h"
#include "Foundation.h"

namespace SimpleGtoViewer {

//-*****************************************************************************
GLCamera::GLCamera()
  : m_fovy( 45.0f ),
    m_size( 100, 100 ),
    m_aspect( 1.0f ),
    m_centerOfInterest( 15.0f ),
    m_rotation( 0.0f, 0.0f, 0.0f ),
    m_scale( 1.0f, 1.0f, 1.0f ),
    m_translation( 0.0f, 0.0f, 0.0f )
{
    // Nothing
}

//-*****************************************************************************
static inline void rotateVector( float rx, float ry, V3f &v )
{
    rx = radians( rx );
    const float sinX = sinf( rx );
    const float cosX = cosf( rx );

    const V3f t( v.x,
                 ( v.y * cosX ) - ( v.z * sinX ),
                 ( v.y * sinX ) + ( v.z * cosX ) );

    ry = radians( ry );
    const float sinY = sinf( ry );
    const float cosY = cosf( ry );

    v.x = ( t.x * cosY ) + ( t.z * sinY );
    v.y = t.y;
    v.z = ( t.x * -sinY ) + ( t.z * cosY );
}

//-*****************************************************************************
void GLCamera::frame( const B3f &bounds )
{
    float r = 0.5f * bounds.size().length();
    float fovyr = radians( m_fovy );
    
#if 0
    // Pad framing a little
    m_centerOfInterest = ( 1.1f * r ) / sinf( fovyr * 0.5f );
    
    float rx = radians( m_rotation.x );
    float ry = radians( m_rotation.y );
    V3f I( m_centerOfInterest * cosf( rx ) * cosf( ry ),
             m_centerOfInterest * sinf( rx ),
             -m_centerOfInterest * cosf( rx ) * sinf( ry ) );
    
    V3f at = m_translation + I;
    m_translation = bounds.center() - at;
#else
    float g = ( 1.1f * r ) / sinf( fovyr * 0.5f );
    lookAt( bounds.center() + V3f( 0, 0, g ), bounds.center() );
#endif
}

//-*****************************************************************************
void GLCamera::autoSetClippingPlanes( const B3f &bounds )
{
    const float rotX = m_rotation.x;
    const float rotY = m_rotation.y;
    const V3f &eye = m_translation;
    float clipNear = FLT_MAX;
    float clipFar = FLT_MIN;
	
    V3f v( 0.0f, 0.0f, -m_centerOfInterest );
    rotateVector( rotX, rotY, v );
    const V3f view = eye + v;
    v.normalize();
    
    V3f points[8];
    
    points[0] = V3f( bounds.min.x, bounds.min.y, bounds.min.z );
    points[1] = V3f( bounds.min.x, bounds.min.y, bounds.max.z );
    points[2] = V3f( bounds.min.x, bounds.max.y, bounds.min.z );
    points[3] = V3f( bounds.min.x, bounds.max.y, bounds.max.z );
    points[4] = V3f( bounds.max.x, bounds.min.y, bounds.min.z );
    points[5] = V3f( bounds.max.x, bounds.min.y, bounds.max.z );
    points[6] = V3f( bounds.max.x, bounds.max.y, bounds.min.z );
    points[7] = V3f( bounds.max.x, bounds.max.y, bounds.max.z );
    
    for( int p = 0; p < 8; ++p )
    {
        V3f dp = points[p] - eye;
        float proj = dp.dot( v );
        clipNear = std::min( proj, clipNear );
        clipFar = std::max( proj, clipFar );
    }
    
    clipNear -= 0.5f;
    clipFar += 0.5f;
    clipNear = clamp( clipNear, 0.1f, 100000.0f );
    clipFar = clamp( clipFar, 0.1f, 100000.0f );
    
    assert( clipFar > clipNear );
    
    m_clip[0] = clipNear;
    m_clip[1] = clipFar;
}

//-*****************************************************************************
void GLCamera::lookAt( const V3f &eye, const V3f &at )
{
    m_translation = eye;

    const V3f dt = at - eye;
	
    const float xzLen = sqrtf( ( dt.x * dt.x ) +
                               ( dt.z * dt.z ) );

    m_rotation.x = degrees( atan2f( dt.y, xzLen ) );

    m_rotation.y = degrees( atan2f( dt.x, -dt.z ) );    
    
    m_centerOfInterest = dt.length();
}

//-*****************************************************************************
void GLCamera::apply() const
{
    glViewport( 0, 0, ( GLsizei )m_size[0], ( GLsizei )m_size[1] );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( m_fovy, 
                    ( ( GLdouble )m_size[0] ) /
                    ( ( GLdouble )m_size[1] ), 
                    m_clip[0], 
                    m_clip[1] );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();    

    ::glScalef( 1.0f / m_scale[0], 1.0f / m_scale[1], 1.0f / m_scale[2] );
    ::glRotatef( -m_rotation[0], 1.0f, 0.0f, 0.0f );
    ::glRotatef( -m_rotation[1], 0.0f, 1.0f, 0.0f );
    ::glRotatef( -m_rotation[2], 0.0f, 0.0f, 1.0f );
    ::glTranslatef( -m_translation[0], -m_translation[1], -m_translation[2] );
}

//-*****************************************************************************
M44f GLCamera::transform() const
{
    M44f m;
    M44f tmp;
    m.makeIdentity();
        
    tmp.setScale( V3f( 1.0f / m_scale[0],
                       1.0f / m_scale[1], 
                       1.0f / m_scale[2] )  );
    m = m * tmp;
    
    tmp.setAxisAngle( V3f( 1.0f, 0.0f, 0.0f ), radians( -m_rotation[0] ) );
    m = m * tmp;
    tmp.setAxisAngle( V3f( 0.0f, 1.0f, 0.0f ), radians( -m_rotation[1] ) );
    m = m * tmp;
    tmp.setAxisAngle( V3f( 0.0f, 0.0f, 1.0f ), radians( -m_rotation[2] ) );
    m = m * tmp;
    
    tmp.setTranslation( V3f( -m_translation[0],
                             -m_translation[1],     
                             -m_translation[2] ) ); 
    m = m * tmp;
    
    return m;
}

//-*****************************************************************************
void GLCamera::track( const V2f &point )
{
    // INIT
    const float rotX = m_rotation.x;
    const float rotY = m_rotation.y;

    V3f dS( 1.0f, 0.0f, 0.0f );
    rotateVector( rotX, rotY, dS );

    V3f dT( 0.0f, 1.0f, 0.0f );
    rotateVector( rotX, rotY, dT );

    float multS = 2.0f * m_centerOfInterest * tanf( radians( fovy() ) / 2.0f );
    const float multT = multS / float( height() );
    multS /= float( width() );

    // TRACK
    const float s = -multS * point.x;
    const float t = multT * point.y;

    // ALTER
    setTranslation( ( m_translation +
                      ( s * dS ) + ( t * dT ) ) );
}

//-*****************************************************************************
void GLCamera::dolly( const V2f &point,
                      float dollySpeed )
{
    // INIT
    const float rotX = m_rotation.x;
    const float rotY = m_rotation.y;
    const V3f &eye = m_translation;
	
    V3f v( 0.0f, 0.0f, -m_centerOfInterest );
    rotateVector( rotX, rotY, v );
    const V3f view = eye + v;
    v.normalize();

    // DOLLY
    const float t = point.x / float( width() );

    // Magic dolly function
    float dollyBy = 1.0f - expf( -dollySpeed * t );

    assert( fabsf( dollyBy ) < 1.0f );
    dollyBy *= m_centerOfInterest;
    const V3f newEye = eye + ( dollyBy * v );

    // ALTER
    setTranslation( newEye );
    v = newEye - view;
    m_centerOfInterest = v.length();
}

//-*****************************************************************************
void GLCamera::rotate( const V2f &point,
                       float rotateSpeed )
{
    // INIT
    float rotX = m_rotation.x;
    float rotY = m_rotation.y;
    const float rotZ = m_rotation.z;
    V3f eye = m_translation;

    V3f v( 0.0f, 0.0f, -m_centerOfInterest );
    rotateVector( rotX, rotY, v );

    const V3f view = eye + v;

    // ROTATE
    rotY += -rotateSpeed * ( point.x / float( width() ) );
    rotX += -rotateSpeed * ( point.y / float( height() ) );

    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = m_centerOfInterest;
    rotateVector( rotX, rotY, v );

    const V3f newEye = view + v;

    // ALTER
    setTranslation( view + v );
    setRotation( V3f( rotX, rotY, rotZ ) );
}

} // End namespace SimpleGtoViewer


