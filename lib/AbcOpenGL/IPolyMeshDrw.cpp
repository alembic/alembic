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

#include "IPolyMeshDrw.h"

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

//-*****************************************************************************
IPolyMeshDrw::IPolyMeshDrw( IPolyMesh &iPmesh )
  : IObjectDrw( iPmesh, false )
  , m_polyMesh( iPmesh )
{
    // Get out if problems.
    if ( !m_polyMesh.valid() )
    {
        return;
    }

    // set constancy on the mesh draw helper
    m_drwHelper.setConstant( m_polyMesh.getSchema().isConstant() );

    if ( m_polyMesh.getSchema().getNumSamples() > 0 )
    {
        m_polyMesh.getSchema().get( m_samp );
    }

    m_boundsProp = m_polyMesh.getSchema().getSelfBoundsProperty();

    // The object has already set up the min time and max time of
    // all the children.
    // if we have a non-constant time sampling, we should get times
    // out of it.
    TimeSamplingPtr iTsmp = m_polyMesh.getSchema().getTimeSampling();
    if ( !m_polyMesh.getSchema().isConstant() )
    {
        size_t numSamps =  m_polyMesh.getSchema().getNumSamples();
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
IPolyMeshDrw::~IPolyMeshDrw()
{
    // Nothing!
}

//-*****************************************************************************
bool IPolyMeshDrw::valid()
{
    return IObjectDrw::valid() && m_polyMesh.valid();
}

//-*****************************************************************************
void IPolyMeshDrw::setTime( chrono_t iSeconds )
{
    IObjectDrw::setTime( iSeconds );
    if ( !valid() )
    {
        m_drwHelper.makeInvalid();
        return;
    }

    // Use nearest for now.
    ISampleSelector ss( iSeconds, ISampleSelector::kNearIndex );
    IPolyMeshSchema::Sample psamp;

    if ( m_polyMesh.getSchema().isConstant() )
    {
        psamp = m_samp;
    }
    else if ( m_polyMesh.getSchema().getNumSamples() > 0 )
    {
        m_polyMesh.getSchema().get( psamp, ss );
    }

    // Get the stuff.
    P3fArraySamplePtr P = psamp.getPositions();
    Int32ArraySamplePtr indices = psamp.getFaceIndices();
    Int32ArraySamplePtr counts = psamp.getFaceCounts();

    Box3d bounds;
    bounds.makeEmpty();

    if ( m_boundsProp && m_boundsProp.getNumSamples() > 0 )
    {
        bounds = m_boundsProp.getValue( ss );
    }

    // Update the mesh hoo-ha.
    m_drwHelper.update( P, V3fArraySamplePtr(),
                        indices, counts, bounds );

    // The Object update computed child bounds.
    // Extend them by this.
    if ( !m_drwHelper.getBounds().isEmpty() )
    {
        m_bounds.extendBy( m_drwHelper.getBounds() );
    }
}

//-*****************************************************************************
void IPolyMeshDrw::draw( const DrawContext &iCtx )
{
    if ( !valid() )
    {
        return;
    }

    if ( iCtx.boundsOnly() )
        m_drwHelper.drawBounds( iCtx );
    else
        m_drwHelper.draw( iCtx );

    IObjectDrw::draw( iCtx );
}

} // End namespace ABCOPENGL_VERSION_NS
} // End namespace AbcOpenGL
