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

#include "ICurvesDrw.h"

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

//-*****************************************************************************
ICurvesDrw::ICurvesDrw( ICurves &iCurves )
  : IObjectDrw( iCurves, false )
  , m_curves( iCurves )
{
    // Get out if problems.
    if ( !m_curves.getSchema().valid() ||
         m_curves.getSchema().getNumSamples() < 1 )
    {
        return;
    }

    // The object has already set up the min time and max time of
    // all the children.
    // if we have a non-constant time sampling, we should get times
    // out of it.
    TimeSamplingPtr iTsmp = m_curves.getSchema().getTimeSampling();
    if ( ! iCurves.getSchema().isConstant() )
    {
        size_t numSamps = iCurves.getSchema().getNumSamples();
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
ICurvesDrw::~ICurvesDrw()
{
    // Nothing!
}

//-*****************************************************************************
bool ICurvesDrw::valid()
{
    return IObjectDrw::valid() && m_curves.valid();
}

//-*****************************************************************************
void ICurvesDrw::setTime( chrono_t iSeconds )
{
    IObjectDrw::setTime( iSeconds );

    // Use nearest for now.
    ISampleSelector ss( iSeconds, ISampleSelector::kNearIndex );
    ICurvesSchema::Sample curvesSample;
    if ( m_curves.getSchema().getNumSamples() > 0 )
    { m_curves.getSchema().get( curvesSample, ss ); }
    else
    { return; }

    m_positions = curvesSample.getPositions();
    m_nCurves = curvesSample.getNumCurves();

    m_nVertices = curvesSample.getCurvesNumVertices();

    m_bounds.makeEmpty();

    m_bounds.extendBy( curvesSample.getSelfBounds() );
}


//-*****************************************************************************
void ICurvesDrw::draw( const DrawContext &iCtx )
{
    if ( ! ( m_positions && m_nVertices ) ) { return; }

    const V3f *points = m_positions->get();
    const Alembic::Util::int32_t *nVertices = m_nVertices->get();

    glDisable( GL_LIGHTING );

    glColor3f( 1.0, 1.0, 1.0 );
    glEnable( GL_POINT_SMOOTH );
    glPointSize( 1.0 );
    glLineWidth( 1.0 );

    for ( size_t currentCurve = 0, currentVertex = 0 ; currentCurve < m_nCurves ;
          ++currentCurve )
    {

        m_curvePoints.clear();
        for ( size_t currentCurveVertex = 0 ;
              currentCurveVertex < ( size_t ) ( nVertices[currentCurve] );
              ++currentCurveVertex, ++currentVertex )
        {
            m_curvePoints.push_back(&points[currentVertex]);
        }

        glMap1f( GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4,
                 (const GLfloat *)m_curvePoints[0] );
        glEnable( GL_MAP1_VERTEX_3 );

        glBegin( GL_LINE_STRIP );
        for ( size_t currentSegment = 0 ; currentSegment < 30 ;
              ++currentSegment )
        {
            glEvalCoord1f(
                static_cast<GLfloat>( currentSegment ) / static_cast<GLfloat>( 30.0f ) );
        }
        glEnd();
    }

    glEnable( GL_LIGHTING );

    IObjectDrw::draw( iCtx );
}

} // End namespace ABCOPENGL_VERSION_NS
} // End namespace AbcOpenGL
