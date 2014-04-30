//-*****************************************************************************
//
// Copyright (c) 2009-2014,
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

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

//-*****************************************************************************
GLCamera::GLCamera()
  : m_rotation( 0.0, 0.0, 0.0 ),
    m_scale( 1.0, 1.0, 1.0 ),
    m_translation( 0.0, 0.0, 0.0 ),
    m_centerOfInterest( 15.0 ),
    m_fovy( 45.0 ),
    m_clip( 0.0, 1.0 ),
    m_size( 100, 100 ),
    m_aspect( 1.0 )
{
    // Nothing
}

//-*****************************************************************************
static inline void rotateVector( double rx, double ry, V3d &v )
{
    rx = radians( rx );
    const double sinX = sinf( rx );
    const double cosX = cosf( rx );

    const V3d t( v.x,
                 ( v.y * cosX ) - ( v.z * sinX ),
                 ( v.y * sinX ) + ( v.z * cosX ) );

    ry = radians( ry );
    const double sinY = sinf( ry );
    const double cosY = cosf( ry );

    v.x = ( t.x * cosY ) + ( t.z * sinY );
    v.y = t.y;
    v.z = ( t.x * -sinY ) + ( t.z * cosY );
}

//-*****************************************************************************
void GLCamera::frame( const Box3d &bounds )
{
    double r = 0.5f * bounds.size().length();
    double fovyr = radians( m_fovy );

    double g = ( 1.1f * r ) / sinf( fovyr * 0.5f );
    lookAt( bounds.center() + V3d( 0, 0, g ), bounds.center() );
}

//-*****************************************************************************
void GLCamera::autoSetClippingPlanes( const Box3d &bounds )
{
    const double rotX = m_rotation.x;
    const double rotY = m_rotation.y;
    const V3d &eye = m_translation;
    double clipNear = FLT_MAX;
    double clipFar = FLT_MIN;

    V3d v( 0.0, 0.0, -m_centerOfInterest );
    rotateVector( rotX, rotY, v );
    const V3d view = eye + v;
    v.normalize();

    V3d points[8];

    points[0] = V3d( bounds.min.x, bounds.min.y, bounds.min.z );
    points[1] = V3d( bounds.min.x, bounds.min.y, bounds.max.z );
    points[2] = V3d( bounds.min.x, bounds.max.y, bounds.min.z );
    points[3] = V3d( bounds.min.x, bounds.max.y, bounds.max.z );
    points[4] = V3d( bounds.max.x, bounds.min.y, bounds.min.z );
    points[5] = V3d( bounds.max.x, bounds.min.y, bounds.max.z );
    points[6] = V3d( bounds.max.x, bounds.max.y, bounds.min.z );
    points[7] = V3d( bounds.max.x, bounds.max.y, bounds.max.z );

    for( int p = 0; p < 8; ++p )
    {
        V3d dp = points[p] - eye;
        double proj = dp.dot( v );
        clipNear = std::min( proj, clipNear );
        clipFar = std::max( proj, clipFar );
    }

    clipNear -= 0.5f;
    clipFar += 0.5f;
    clipNear = clamp( clipNear, 0.1, 100000.0 );
    clipFar = clamp( clipFar, 0.1, 100000.0 );

    if ( clipFar <= clipNear )
    {
        clipFar = clipNear + 0.1;
    }

    m_clip[0] = clipNear;
    m_clip[1] = clipFar;
}

//-*****************************************************************************
void GLCamera::lookAt( const V3d &eye, const V3d &at )
{
    m_translation = eye;

    const V3d dt = at - eye;

    const double xzLen = sqrt( ( dt.x * dt.x ) +
                               ( dt.z * dt.z ) );

    m_rotation.x = degrees( atan2( dt.y, xzLen ) );

    m_rotation.y = degrees( atan2( dt.x, -dt.z ) );

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

    ::glScaled( 1.0 / m_scale[0], 1.0 / m_scale[1], 1.0 / m_scale[2] );

    // Apply rotation in XYZ order. XformSample::getXRotation etc extract
    // the rotation values out of the xform matrix using XYZ rotation.
    // TODO: revisit using a xform matrix here instead.
    ::glRotated( -m_rotation[0], 1.0, 0.0, 0.0 );
    ::glRotated( -m_rotation[1], 0.0, 1.0, 0.0 );
    ::glRotated( -m_rotation[2], 0.0, 0.0, 1.0 );

    ::glTranslated( -m_translation[0], -m_translation[1], -m_translation[2] );
}

//-*****************************************************************************
M44d GLCamera::transform() const
{
    M44d m;
    M44d tmp;
    m.makeIdentity();

    tmp.setScale( V3d( 1.0 / m_scale[0],
                       1.0 / m_scale[1],
                       1.0 / m_scale[2] )  );
    m = m * tmp;

    tmp.setAxisAngle( V3d( 1.0, 0.0, 0.0 ), radians( -m_rotation[0] ) );
    m = m * tmp;
    tmp.setAxisAngle( V3d( 0.0, 1.0, 0.0 ), radians( -m_rotation[1] ) );
    m = m * tmp;
    tmp.setAxisAngle( V3d( 0.0, 0.0, 1.0 ), radians( -m_rotation[2] ) );
    m = m * tmp;

    tmp.setTranslation( V3d( -m_translation[0],
                             -m_translation[1],
                             -m_translation[2] ) );
    m = m * tmp;

    return m;
}

//-*****************************************************************************
void GLCamera::track( const V2d &point )
{
    // INIT
    const double rotX = m_rotation.x;
    const double rotY = m_rotation.y;

    V3d dS( 1.0, 0.0, 0.0 );
    rotateVector( rotX, rotY, dS );

    V3d dT( 0.0, 1.0, 0.0 );
    rotateVector( rotX, rotY, dT );

    double multS = 2.0 * m_centerOfInterest * tanf( radians( fovy() ) / 2.0 );
    const double multT = multS / double( height() );
    multS /= double( width() );

    // TRACK
    const double s = -multS * point.x;
    const double t = multT * point.y;

    // ALTER
    setTranslation( ( m_translation +
                      ( s * dS ) + ( t * dT ) ) );
}

//-*****************************************************************************
void GLCamera::dolly( const V2d &point,
                      double dollySpeed )
{
    // INIT
    const double rotX = m_rotation.x;
    const double rotY = m_rotation.y;
    const V3d &eye = m_translation;

    V3d v( 0.0, 0.0, -m_centerOfInterest );
    rotateVector( rotX, rotY, v );
    const V3d view = eye + v;
    v.normalize();

    // DOLLY
    const double t = point.x / double( width() );

    // Magic dolly function
    double dollyBy = 1.0 - expf( -dollySpeed * t );

    if (dollyBy > 1.0)
    {
        dollyBy = 1.0;
    }
    else if (dollyBy < -1.0)
    {
        dollyBy = -1.0;
    }

    dollyBy *= m_centerOfInterest;
    const V3d newEye = eye + ( dollyBy * v );

    // ALTER
    setTranslation( newEye );
    v = newEye - view;
    m_centerOfInterest = v.length();
}

//-*****************************************************************************
void GLCamera::rotate( const V2d &point,
                       double rotateSpeed )
{
    // INIT
    double rotX = m_rotation.x;
    double rotY = m_rotation.y;
    const double rotZ = m_rotation.z;
    V3d eye = m_translation;

    V3d v( 0.0, 0.0, -m_centerOfInterest );
    rotateVector( rotX, rotY, v );

    const V3d view = eye + v;

    // ROTATE
    rotY += -rotateSpeed * ( point.x / double( width() ) );
    rotX += -rotateSpeed * ( point.y / double( height() ) );

    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = m_centerOfInterest;
    rotateVector( rotX, rotY, v );

    const V3d newEye = view + v;

    // ALTER
    setTranslation( view + v );
    setRotation( V3d( rotX, rotY, rotZ ) );
}

//-*****************************************************************************
std::string GLCamera::RIB() const
{
    std::ostringstream cameraStream;
    cameraStream << "Format "
                 << ( int )m_size[0]
                 << ( int )m_size[1] << "1\n"
                 << "Clipping "
                 << ( float )m_clip[0]
                 << ( float )m_clip[1] << "\n"
                 << "Projection \"perspective\" \"fov\" ["
                 << ( float )m_fovy << "]\n"
                 << "Scale 1 1 -1\n"
                 << "Scale "
                 << ( float )( 1.0/m_scale[0] ) << " "
                 << ( float )( 1.0/m_scale[1] ) << " "
                 << ( float )( 1.0/m_scale[2] ) << "\n"
                 << "Rotate  "
                 << ( float )( -m_rotation[0] ) << "1 0 0\n"
                 << "Rotate  "
                 << ( float )( -m_rotation[1] ) << "0 1 0\n"
                 << "Rotate  "
                 << ( float )( -m_rotation[2] ) << "0 0 1\n"
                 << "Translate "
                 << ( float )( -m_translation[0] )
                 << ( float )( -m_translation[1] )
                 << ( float )( -m_translation[2] )
                 << "\n";
    return cameraStream.str();
}

} // End namespace ABCOPENGL_VERSION_NS
} // End namespace AbcOpenGL
