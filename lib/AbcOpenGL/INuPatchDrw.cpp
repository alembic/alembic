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

#include "INuPatchDrw.h"

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

//-*****************************************************************************
INuPatchDrw::INuPatchDrw( INuPatch &iNuPatch )
  : IObjectDrw( iNuPatch, false )
  , m_nuPatch( iNuPatch )
{
    // create our nurb renderer.
    nurb = gluNewNurbsRenderer();

    gluNurbsProperty(nurb, GLU_SAMPLING_TOLERANCE, 25.0);
    gluNurbsProperty(nurb, GLU_DISPLAY_MODE, GLU_FILL);

    // Get out if problems.
    if ( !m_nuPatch.valid() )
    {
        return;
    }


    // The object has already set up the min time and max time of
    // all the children.
    // if we have a non-constant time sampling, we should get times
    // out of it.
    TimeSamplingPtr iTsmp = m_nuPatch.getSchema().getTimeSampling();
    if ( !m_nuPatch.getSchema().isConstant() )
    {
        size_t numSamps = m_nuPatch.getSchema().getNumSamples();
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
INuPatchDrw::~INuPatchDrw()
{
    //delete nurb;
}

//-*****************************************************************************
bool INuPatchDrw::valid()
{
    return IObjectDrw::valid() && m_nuPatch.valid();
}

//-*****************************************************************************
void INuPatchDrw::setTime( chrono_t iSeconds )
{
    IObjectDrw::setTime( iSeconds );

    // Use nearest for now.
    ISampleSelector ss( iSeconds, ISampleSelector::kNearIndex );
    INuPatchSchema::Sample nuPatchSample;
    m_nuPatch.getSchema().get( nuPatchSample, ss );

    m_positions = nuPatchSample.getPositions();
    m_uKnot = nuPatchSample.getUKnot();
    m_vKnot = nuPatchSample.getVKnot();
    m_nu = nuPatchSample.getNumU();
    m_nv = nuPatchSample.getNumV();
    m_uOrder = nuPatchSample.getUOrder();
    m_vOrder = nuPatchSample.getVOrder();

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

    // The Object update computed child bounds.
    // Extend them by this.
    /*
    if ( !m_drwHelper.getBounds().isEmpty() )
    {
        m_bounds.extendBy( m_drwHelper.getBounds() );
    }
    */
}


//-*****************************************************************************
void INuPatchDrw::draw( const DrawContext &iCtx )
{

    const V3f * points = m_positions -> get();

    const float *u_knot = m_uKnot -> get();
    const float *v_knot = m_vKnot -> get();
    size_t nknotu = m_uKnot -> size();
    size_t nknotv = m_vKnot -> size();

    glColor3f(1.0, 1.0, 1.0);

    gluBeginSurface(nurb);

        gluNurbsSurface( nurb,
                        nknotu, (GLfloat *) &u_knot[0],
                        nknotv, (GLfloat *) &v_knot[0],
                        3, m_nu*3, // stride
                        (GLfloat *)&points[0][0],
                        m_uOrder, m_vOrder, //orders
                        GL_MAP2_VERTEX_3
                        );

    gluEndSurface(nurb);


    IObjectDrw::draw( iCtx );
}

} // End namespace ABCOPENGL_VERSION_NS
} // End namespace AbcOpenGL
