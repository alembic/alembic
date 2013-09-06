//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#include "IPointsDrw.h"

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

//-*****************************************************************************
IPointsDrw::IPointsDrw( IPoints &iPmesh )
  : IObjectDrw( iPmesh, false )
  , m_points( iPmesh )
{
    // Get out if problems.
    if ( !m_points.valid() )
    {
        return;
    }

    // Try to create colors, if possible.
    IPointsSchema &pointsSchema = m_points.getSchema();
    const PropertyHeader *phead = pointsSchema.getPropertyHeader( "Cs" );
    if ( phead && IC3fArrayProperty::matches( *phead ) )
    {
        m_colorProp = IC3fArrayProperty( pointsSchema, "Cs" );
    }

    phead = pointsSchema.getPropertyHeader( "N" );
    if ( phead && IN3fArrayProperty::matches( *phead ) )
    {
        m_normalProp = IN3fArrayProperty( pointsSchema, "N" );
    }

    // The object has already set up the min time and max time of
    // all the children.
    // if we have a non-constant time sampling, we should get times
    // out of it.
    TimeSamplingPtr iTsmp = m_points.getSchema().getTimeSampling();
    if ( !m_points.getSchema().isConstant() )
    {
        size_t numSamps =  m_points.getSchema().getNumSamples();
        if ( numSamps > 0 )
        {
            chrono_t minTime = iTsmp->getSampleTime( 0 );
            m_minTime = std::min( m_minTime, minTime );
            chrono_t maxTime = iTsmp->getSampleTime( numSamps-1 );
            m_maxTime = std::max( m_maxTime, maxTime );
        }
    }
}

//-*****************************************************************************
IPointsDrw::~IPointsDrw()
{
    // Nothing!
}

//-*****************************************************************************
bool IPointsDrw::valid()
{
    return IObjectDrw::valid() && m_points.valid();
}

//-*****************************************************************************
void IPointsDrw::setTime( chrono_t iSeconds )
{
    IObjectDrw::setTime( iSeconds );
    if ( !valid() )
    {
        return;
    }

    // Use nearest for now.
    ISampleSelector ss( iSeconds, ISampleSelector::kNearIndex );
    IPointsSchema::Sample psamp;
    m_points.getSchema().get( psamp, ss );
    m_positions = psamp.getPositions();

    // Update bounds from positions
    m_bounds.makeEmpty();
    if ( m_positions )
    {
        size_t numPoints = m_positions->size();
        for ( size_t p = 0; p < numPoints; ++p )
        {
            m_bounds.extendBy( (*m_positions)[p] );
        }
    }

    // If we have a color prop, update it
    if ( m_colorProp )
    {
        m_colors = m_colorProp.getValue( ss );
    }

    if ( m_normalProp )
    {
        m_normals = m_normalProp.getValue( ss );
    }
}

//-*****************************************************************************
void IPointsDrw::draw( const DrawContext &iCtx )
{
    if ( !valid() )
    {
        return;
    }

    if ( !m_positions || m_positions->size() == 0 )
    {
        IObjectDrw::draw( iCtx );
        return;
    }

    size_t numPoints = m_positions->size();

    const V3f *points = m_positions->get();
    const C3f *colors = NULL;
    if ( m_colors  && ( m_colors->size() == numPoints ) )
    {
        colors = m_colors->get();
    }
    const N3f *normals = NULL;
    if ( m_normals && ( m_normals->size() == numPoints ) )
    {
        normals = m_normals->get();
    }

    if ( !normals )
    {
        glDisable( GL_LIGHTING );
    }

    if ( !colors )
    {
        glColor3f( 1.0f, 1.0f, 1.0f );
    }

    glEnable( GL_POINT_SMOOTH );
    glPointSize( iCtx.getPointSize() );

#ifndef SIMPLE_ABC_VIEWER_NO_GL_CLIENT_STATE
//#if 0
    {
        GL_NOISY( glEnableClientState( GL_VERTEX_ARRAY ) );
        if ( colors )
        {
            GL_NOISY( glEnableClientState( GL_COLOR_ARRAY ) );
            GL_NOISY( glColorPointer( 3, GL_FLOAT, 0,
                                      ( const GLvoid * )colors ) );
        }
        if ( normals )
        {
            GL_NOISY( glEnableClientState( GL_NORMAL_ARRAY ) );
            GL_NOISY( glNormalPointer( GL_FLOAT, 0,
                                       ( const GLvoid * )normals ) );
        }

        GL_NOISY( glVertexPointer( 3, GL_FLOAT, 0,
                                   ( const GLvoid * )points ) );

        GL_NOISY( glDrawArrays( GL_POINTS,
                                0, ( GLsizei )( numPoints ) ) );

        if ( colors )
        {
            GL_NOISY( glDisableClientState( GL_COLOR_ARRAY ) );
        }
        if ( normals )
        {
            GL_NOISY( glDisableClientState( GL_NORMAL_ARRAY ) );
        }
        GL_NOISY( glDisableClientState( GL_VERTEX_ARRAY ) );
    }
#else
    glBegin( GL_POINTS );

    for ( size_t i = 0; i < numPoints; ++i )
    {
        const V3f &vert = (*m_positions)[i];

        if ( colors )
        {
            const C3f &col = colors[i];
            glColor3fv( ( const GLfloat * )&col );
        }

        if ( normals )
        {
            const N3f &norm = normals[i];
            glNormal3fv( ( const GLfloat * )&norm );
        }

        glVertex3fv( ( const GLfloat * )&vert );
    }

    glEnd();

#endif

    glEnable( GL_LIGHTING );

    IObjectDrw::draw( iCtx );
}

} // End namespace ABCOPENGL_VERSION_NS
} // End namespace AbcOpenGL
