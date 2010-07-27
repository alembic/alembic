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

#include "PointPrimitiveDrw.h"


namespace SimpleAbcViewer {

//-*****************************************************************************
PointPrimitiveDrw::PointPrimitiveDrw( Atk::PointPrimitiveReaderPtr i_ppReader )
  : Drawable(),
    m_ppReader( i_ppReader )
{
    makeInvalid();
}

//-*****************************************************************************
PointPrimitiveDrw::~PointPrimitiveDrw()
{
    // Nothing
}

//-*****************************************************************************
bool PointPrimitiveDrw::valid() const
{
    return m_valid && ( ( bool ) m_points );
}

//-*****************************************************************************
bool PointPrimitiveDrw::readFrame( float i_frame )
{
    makeInvalid();
    
    if ( !m_ppReader )
    {
        return m_valid;
    }

    // Read frames.
    unsigned int stat = m_ppReader->read( i_frame );
    if ( stat == Atk::PointPrimitiveReader::READ_ERROR )
    {
        return m_valid;
    }

    // Get points.
    m_numPoints = m_ppReader->mPosition.size();
    m_points = ( const GLfloat * )&( m_ppReader->mPosition.front() );

    // Get colors.
    Atk::PropertyPair colorProp;
    m_colors = NULL;
    if ( m_ppReader->readProperty( "Cs", i_frame, colorProp ) !=
         Atk::PROPERTY_NONE )
    {
        std::vector<float> & colorVec = boost::get< std::vector<float> >
            ( colorProp.first );
        if ( colorVec.size() == m_ppReader->mPosition.size() )
        {
            m_colors = ( const GLfloat * )&( colorVec.front() );
        }
    }

    // Compute bounds.
    // Could probably avoid doing this every frame if it's static.
    m_bounds.makeEmpty();
    const V3f *pts = ( const V3f * )m_points;
    for ( size_t i = 0; i < m_numPoints; ++i )
    {
        const V3f &p = pts[i];
        m_bounds.extendBy( V3d( p.x, p.y, p.z ) );
    }

    // All good.
    m_valid = true;

    return m_valid;
}

//-*****************************************************************************
void PointPrimitiveDrw::getFrames( std::set<float> & i_sampleFrames ) const
{
    if ( m_valid && m_ppReader )
    {
        m_ppReader->getFrames( i_sampleFrames );
    }
}

//-*****************************************************************************
bool PointPrimitiveDrw::hasFrames() const
{
    if ( !m_valid || !m_ppReader ) { return false; }
    else { return m_ppReader->hasFrames(); }
}

//-*****************************************************************************
Box3d PointPrimitiveDrw::getBounds() const
{
    return m_bounds;
}

//-*****************************************************************************
void PointPrimitiveDrw::draw( const DrawContext & i_ctx ) const
{
    if ( !m_valid || !m_points )
    {
        return;
    }
    
    GL_NOISY( glPushAttrib( GL_POINT_BIT | GL_LIGHTING_BIT |
                            GL_ENABLE_BIT ) );
    
    GL_NOISY( glEnable( GL_POINT_SMOOTH ) );
    GL_NOISY( glDisable( GL_LIGHTING ) );
    GL_NOISY( glPointSize( 2.5f ) );
        
#ifdef SIMPLE_ABC_VIEWER_NO_GL_CLIENT_STATE
//#if 0
    if ( m_colors )
    {
        GL_NOISY( glEnableClientState( GL_COLOR_ARRAY ) );
        GL_NOISY(
            glColorPointer( 3, GL_FLOAT, 0,
                            ( const GLvoid * )m_colors ) );
    }
        
    GL_NOISY( glEnableClientState( GL_VERTEX_ARRAY ) );
    
    GL_NOISY( glVertexPointer( 3, GL_FLOAT, 0,
                               ( const GLvoid * )m_points ) );
    
    GL_NOISY( glDrawArrays( GL_POINTS, 0,
                            ( GLsizei )m_numPoints ) );
    
    if ( m_colors )
    {
        GL_NOISY( glDisableClientState( GL_COLOR_ARRAY ) );
    }
    
    GL_NOISY( glDisableClientState( GL_VERTEX_ARRAY ) );
#else
    glBegin( GL_POINTS );
    
    for ( size_t i = 0; i < m_numPoints; ++i )
    {
        if ( m_colors )
        {
            glColor3fv( m_colors + 3*i );
        }
        glVertex3fv( m_points + 3*i );
    }
    
    glEnd();
    
#endif
    
    GL_NOISY( glPopAttrib() );
}

//-*****************************************************************************
void PointPrimitiveDrw::makeInvalid()
{
    m_valid = false;
    m_numPoints = 0;
    m_points = NULL;
    m_colors = NULL;
    m_bounds.makeEmpty();
}

} // End namespace SimpleAbcViewer
