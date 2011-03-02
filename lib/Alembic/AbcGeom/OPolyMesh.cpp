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

        m_childBounds.set( iSamp.getChildBounds(), iSS );

        if ( iSamp.getSelfBounds().isEmpty() )
        {
            // OTypedScalarProperty::set() is not referentially transparent,
            // so we need a a placeholder variable.
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() )
                           );
            m_selfBounds.set( bnds, iSS );
        }
        else { m_selfBounds.set( iSamp.getSelfBounds(), iSS ); }

        if ( iSamp.getUVs().getVals() )
        {
            if ( iSamp.getUVs().getIndices() )
            {
                // UVs are indexed
                m_uvs = OV2fGeomParam( this->getPtr(), "uv", true,
                                       iSamp.getUVs().getScope(), 1,
                                       this->getTimeSamplingType() );
            }
            else
            {
                // UVs are not indexed
                m_uvs = OV2fGeomParam( this->getPtr(), "uv", false,
                                       iSamp.getUVs().getScope(), 1,
                                       this->getTimeSamplingType() );
            }

            m_uvs.set( iSamp.getUVs(), iSS );
        }
        if ( iSamp.getNormals().getVals() )
        {
            if ( iSamp.getNormals().getIndices() )
            {
                // normals are indexed
                m_normals = ON3fGeomParam( this->getPtr(), "N", true,
                                           iSamp.getNormals().getScope(),
                                           1, this->getTimeSamplingType() );
            }
            else
            {
                // normals are not indexed
                m_normals = ON3fGeomParam( this->getPtr(), "N", false,
                                           iSamp.getNormals().getScope(), 1,
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
        SetPropUsePrevIfNull( m_childBounds, iSamp.getChildBounds(), iSS );

        if ( iSamp.getSelfBounds().hasVolume() )
        {
            m_selfBounds.set( iSamp.getSelfBounds(), iSS );
        }
        else if ( iSamp.getPositions() )
        {
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() )
                           );
            m_selfBounds.set( bnds, iSS );
        }
        else
        {
            m_selfBounds.setFromPrevious( iSS );
        }

        // OGeomParam will automatically use SetPropUsePrevIfNull internally
        if ( m_uvs ) { m_uvs.set( iSamp.getUVs(), iSS ); }
        if ( m_normals ) { m_normals.set( iSamp.getNormals(), iSS ); }
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

    m_selfBounds.setFromPrevious( iSS );
    m_childBounds.setFromPrevious( iSS );

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

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    m_positions = Abc::OV3fArrayProperty( _this, "P", mdata, iTst );

    m_indices = Abc::OInt32ArrayProperty( _this, ".faceIndices", iTst );

    m_counts = Abc::OInt32ArrayProperty( _this, ".faceCounts", iTst );

    m_selfBounds = Abc::OBox3dProperty( _this, ".selfBnds", iTst );

    m_childBounds = Abc::OBox3dProperty( _this, ".childBnds", iTst );

    // UVs and Normals are created on first call to set()

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
Abc::OCompoundProperty OPolyMeshSchema::getArbGeomParams()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::getArbGeomParams()" );

    if ( ! m_arbGeomParams )
    {
        m_arbGeomParams = Abc::OCompoundProperty( this->getPtr(),
                                                  ".arbGeomParams" );
    }

    return m_arbGeomParams;

    ALEMBIC_ABC_SAFE_CALL_END();

    Abc::OCompoundProperty ret;
    return ret;
}

} // End namespace AbcGeom
} // End namespace Alembic
