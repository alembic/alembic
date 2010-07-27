//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks Inc. and
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
// Industrial Light & Magic, nor the names of their contributors may be used
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

#include <FluidSimDemo/MeshDrw.h>

namespace FluidSimDemo {


//-*****************************************************************************
// TEMPLATE IMPLEMENTATIONS
//-*****************************************************************************
MeshDrw::MeshDrw( IObjectPtr iObject,
                  CollisionObjects &cobjs )
  : TransformDrw( iObject, cobjs )
{
    
    if ( m_object )
    {
        m_meshTrait = Atg::IMeshTrait( *m_object );
        m_normalsTrait = Atg::INormalsTrait( *m_object );
    }

    if ( m_meshTrait )
    {
        Abc::IIntSample indices = m_meshTrait.vertexIndices().get();
        if ( !indices )
        {
            m_valid = false;
            return;
        }

        size_t numIndices = indices.size();
        if ( numIndices < 3 )
        {
            m_valid = false;
            return;
        }

        m_indices.insert( m_indices.begin(),
                          indices.get(),
                          indices.get() + numIndices );

        
        Abc::IIntSample counts = m_meshTrait.faceCounts().get();
        if ( !counts )
        {
            m_valid = false;
            return;
        }

        size_t numCounts = counts.size();
        if ( numCounts < 1 )
        {
            m_valid = false;
            return;
        }

        m_starts.resize( numCounts );
        m_starts[0] = 0;
        for ( size_t i = 1; i < numCounts; ++i )
        {
            m_starts[i] = m_starts[i-1] + counts[i-1];
        }
    }
}

//-*****************************************************************************
void MeshDrw::readShape( float i_frame,
                         const M44d &iParentXform )
{
    if ( !m_meshTrait || !m_valid || !m_meshTrait.positions() )
    {
        m_shapeBounds.makeEmpty();
        return;
    }
    
    Abc::seconds_t secs = i_frame / 24.0f;

    if ( m_meshTrait.positions().isAnimated() )
    {
        m_points = m_meshTrait.positions().getAnimLowerBound( secs );
        m_shapeBounds.makeEmpty();
        for ( size_t p = 0; p < m_points.size(); ++p )
        {
            m_shapeBounds.extendBy( m_points[p] );
        }    
    }
    else
    {
        if ( !m_points )
        {
            m_points = m_meshTrait.positions().get();
            for ( size_t p = 0; p < m_points.size(); ++p )
            {
                m_shapeBounds.extendBy( m_points[p] );
            }
        }
    }

    if ( !m_points )
    {
        m_shapeBounds.makeEmpty();
        m_valid = false;
        return;
    }
    
    // Update mesh - either completely, or partially.
    MeshDrwHelper::ConstFloatArray Parray( ( const float * )m_points.get(),
                                      ( const float * )m_points.get() + 3 * m_points.size() );
    if ( !m_helper.first )
    {
        MeshDrwHelper::ConstIndexArray Iarray( &m_indices.front(),
                                          &m_indices.front() + m_indices.size() );
        MeshDrwHelper::ConstIndexArray Sarray( &m_starts.front(),
                                          &m_starts.front() + m_starts.size() );
            
        m_helper.updateMesh( &Parray,
                             ( MeshDrwHelper::ConstFloatArray * )NULL,
                             &Iarray,
                             &Sarray );
    }
    else
    {
        m_helper.updateMesh( &Parray,
                           
                             ( MeshDrwHelper::ConstFloatArray * )NULL );
    }
}

//-*****************************************************************************
void MeshDrw::drawShape( const DrawContext & i_ctx ) const
{
    if ( m_valid )
    {
        m_helper.draw( i_ctx );
    }
}


} // End namespace FluidSimDemo
