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

#include <Alembic/AbcGeom/OPolyMesh.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void OPolyMeshSchema::set( const Sample &iSamp,
                           const Abc::OSampleSelector &iSS  )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::set()" );

    // We could add sample integrity checking here.
    if ( iSS.getIndex() == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions() &&
                     iSamp.getIndices() &&
                     iSamp.getCounts(),
                     "Sample 0 must have valid data for all mesh components" );

        m_positions.set( iSamp.getPositions(), iSS );
        m_indices.set( iSamp.getIndices(), iSS );
        m_counts.set( iSamp.getCounts(), iSS );

        if ( iSamp.getUVs() )
        {
            if ( iSamp.getUVs().getIndices() )
            {
                // UVs are indexed
                m_uvs = OV2fArbAttr( *this, "uv", true,
                                     iSamp.getUVs().getScope(),
                                     this->getTimeSamplingType() );
            }
            else
            {
                // UVs are not indexed
                m_uvs = OV2fArbAttr( *this, "uv", false,
                                     iSamp.getUVs().getScope(),
                                     this->getTimeSamplingType() );
            }

            m_uvs.set( iSamp.getUVs(), iSS );
        }
        if ( iSamp.getNormals() )
        {
            if ( iSamp.getNormals().getIndices() )
            {
                // normals are indexed
                m_normals = ON3fArbAttr( *this, "N", true,
                                         iSamp.getNormals().getScope(),
                                         this->getTimeSamplingType() );
            }
            else
            {
                // normals are not indexed
                m_normals = ON3fArbAttr( *this, "N", false,
                                         iSamp.getNormals().getScope(),
                                         this->getTimeSamplingType() );
            }

            m_normals.set( iSamp.getNormals(), iSS );
        }
    }
    else
    {
        SetPropUsePrevIfNull( m_positions, iSamp.getPositions(), iSS );
        SetPropUsePrevIfNull( m_indices, iSamp.getIndices(), iSS );
        SetPropUsePrevIfNull( m_counts, iSamp.getCounts(), iSS );

        if ( m_uvs ) { SetPropUsePrevIfNull( m_uvs, iSamp.getUVs(), iSS ); }
        if ( m_normals ) { SetPropUsePrevIfNull( m_normals, iSamp.getNormals(),
                                                 iSS ); }
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::setFromPrevious( const Abc::OSampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::setFromPrevious" );

    m_positions.setFromPrevious( iSS );
    m_indices.setFromPrevious( iSS );
    m_counts.setFromPrevious( iSS );

    if ( m_uvs ) { m_uvs.setFromPrevious( iSS ); }
    if ( m_normals ) { m_normals.setFromPrevious( iSS ); }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::init( const AbcA::TimeSamplingType &iTst )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::init()" );

    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVertexScope );
    m_positions = Abc::OV3fArrayProperty( *this, "P", mdata, iTst );

    m_indices = Abc::OInt32ArrayProperty( *this, ".faceIndices", iTst );

    m_counts = Abc::OInt32ArrayProperty( *this, ".faceCounts", iTst );

    // UVs and Normals are created on first call to set()

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
