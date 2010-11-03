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

#include <FluidSimDemo/DrawHelper.h>

namespace FluidSimDemo {

//-*****************************************************************************
DrawHelper::DrawHelper()
{
    m_quadratic = gluNewQuadric();
    if ( !m_quadratic )
    {
        FSD_THROW( "Could not create quadric for drawing" );
    }
    gluQuadricNormals( m_quadratic, GLU_SMOOTH );
    gluQuadricTexture( m_quadratic, GL_TRUE );
    gluQuadricOrientation( m_quadratic, GLU_OUTSIDE );
}

//-*****************************************************************************
DrawHelper::~DrawHelper()
{
    if ( m_quadratic )
    {
        gluDeleteQuadric( m_quadratic );
        m_quadratic = NULL;
    }
}

//-*****************************************************************************
void DrawHelper::drawSphere() const
{
    gluSphere( const_cast<GLUquadric*>( m_quadratic ),
               1.0f, 32, 32 );
}

//-*****************************************************************************
void DrawHelper::drawCube() const
{
    glBegin( GL_QUADS );
    
    // front face 
    glNormal3f( 0.0f, 0.0f, 1.0f );
    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( -1.0f, -1.0f, 1.0f ); 
    glTexCoord2f( 1.0f, 0.0f );
    glVertex3f( 1.0f, -1.0f, 1.0f );
    glTexCoord2f( 1.0f, 1.0f );
    glVertex3f( 1.0f, 1.0f, 1.0f );
    glTexCoord2f( 0.0f, 1.0f );
    glVertex3f( -1.0f, 1.0f, 1.0f );
    
    // back face 
    glNormal3f( 0.0f, 0.0f, -1.0f );
    glTexCoord2f( 1.0f, 0.0f );
    glVertex3f( -1.0f, -1.0f, -1.0f ); 
    glTexCoord2f( 1.0f, 1.0f );
    glVertex3f( -1.0f, 1.0f, -1.0f );
    glTexCoord2f( 0.0f, 1.0f );
    glVertex3f( 1.0f, 1.0f, -1.0f );
    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( 1.0f, -1.0f, -1.0f );
    // right face 
    glNormal3f( 1.0f, 0.0f, 0.0f );
    glTexCoord2f( 1.0f, 0.0f );
    glVertex3f( 1.0f, -1.0f, -1.0f ); 
    glTexCoord2f( 1.0f, 1.0f );
    glVertex3f( 1.0f, 1.0f, -1.0f );
    glTexCoord2f( 0.0f, 1.0f );
    glVertex3f( 1.0f, 1.0f, 1.0f );
    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( 1.0f, -1.0f, 1.0f );
    
    // left face 
    glNormal3f( -1.0f, 0.0f, 0.0f );
    glTexCoord2f( 1.0f, 0.0f );
    glVertex3f( -1.0f, -1.0f, 1.0f ); 
    glTexCoord2f( 1.0f, 1.0f );
    glVertex3f( -1.0f, 1.0f, 1.0f );
    glTexCoord2f( 0.0f, 1.0f );
    glVertex3f( -1.0f, 1.0f, -1.0f );
    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( -1.0f, -1.0f, -1.0f );
    
    // top face 
    glNormal3f( 0.0f, 1.0f, 0.0f );
    glTexCoord2f( 1.0f, 0.0f );
    glVertex3f( 1.0f, 1.0f, 1.0f ); 
    glTexCoord2f( 1.0f, 1.0f );
    glVertex3f( 1.0f, 1.0f, -1.0f );
    glTexCoord2f( 0.0f, 1.0f );
    glVertex3f( -1.0f, 1.0f, -1.0f );
    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( -1.0f, 1.0f, 1.0f );
    
    // bottom face 
    glNormal3f( 0.0f, -1.0f, 0.0f );
    glTexCoord2f( 1.0f, 0.0f );
    glVertex3f( 1.0f, -1.0f, -1.0f ); 
    glTexCoord2f( 1.0f, 1.0f );
    glVertex3f( 1.0f, -1.0f, 1.0f );
    glTexCoord2f( 0.0f, 1.0f );
    glVertex3f( -1.0f, -1.0f, 1.0f );
    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( -1.0f, -1.0f, -1.0f );
    
    glEnd();
}

//-*****************************************************************************
void DrawHelper::drawCylinder() const
{
    glPushMatrix();
    glTranslatef( 0.0f, -1.0f, 0.0f );
    glRotatef( -90.0, 1.0, 0.0, 0.0 );
    gluCylinder( const_cast<GLUquadric*>( m_quadratic ),
                 1.0f, 1.0f, 2.0f, 32, 32 );
    glPopMatrix();
}

//-*****************************************************************************
void DrawHelper::drawCone() const
{
    glPushMatrix();
    glTranslatef( 0.0f, -1.0f, 0.0f );
    glRotatef( -90.0, 1.0, 0.0, 0.0 );
    gluCylinder( const_cast<GLUquadric*>( m_quadratic ),
                 1.0f, 0.0f, 2.0f, 32, 32 );
    glPopMatrix();
}

} // End namespace FluidSimDemo
