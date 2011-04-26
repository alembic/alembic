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
void OPolyMeshSchema::set( const Sample &iSamp )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::set()" );

    // do we need to create child bounds?
    if ( iSamp.getChildBounds().hasVolume() && !m_childBounds)
    {
        m_childBounds = Abc::OBox3dProperty( this->getPtr(), ".childBnds",
            m_positions.getTimeSampling() );
        Abc::Box3d emptyBox;
        emptyBox.makeEmpty();

        // -1 because we just dis an m_positions set above
        size_t numSamples = m_positions.getNumSamples() - 1;

        // set all the missing samples
        for ( size_t i = 0; i < numSamples; ++i )
        {
            m_childBounds.set( emptyBox );
        }
    }

    // We could add sample integrity checking here.
    if ( m_positions.getNumSamples() == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions() &&
                     iSamp.getIndices() &&
                     iSamp.getCounts(),
                     "Sample 0 must have valid data for all mesh components" );

        m_positions.set( iSamp.getPositions() );
        m_indices.set( iSamp.getIndices() );
        m_counts.set( iSamp.getCounts() );

        if (m_childBounds)
        { m_childBounds.set( iSamp.getChildBounds() ); }

        if ( iSamp.getSelfBounds().isEmpty() )
        {
            // OTypedScalarProperty::set() is not referentially transparent,
            // so we need a a placeholder variable.
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() ) );
            m_selfBounds.set( bnds );
        }
        else { m_selfBounds.set( iSamp.getSelfBounds() ); }

        if ( iSamp.getUVs().getVals() )
        {
            if ( iSamp.getUVs().getIndices() )
            {
                // UVs are indexed
                m_uvs = OV2fGeomParam( this->getPtr(), "uv", true,
                                       iSamp.getUVs().getScope(), 1,
                                       this->getTimeSampling() );
            }
            else
            {
                // UVs are not indexed
                m_uvs = OV2fGeomParam( this->getPtr(), "uv", false,
                                       iSamp.getUVs().getScope(), 1,
                                       this->getTimeSampling() );
            }

            m_uvs.set( iSamp.getUVs() );
        }
        if ( iSamp.getNormals().getVals() )
        {
            if ( iSamp.getNormals().getIndices() )
            {
                // normals are indexed
                m_normals = ON3fGeomParam( this->getPtr(), "N", true,
                                           iSamp.getNormals().getScope(),
                                           1, this->getTimeSampling() );
            }
            else
            {
                // normals are not indexed
                m_normals = ON3fGeomParam( this->getPtr(), "N", false,
                                           iSamp.getNormals().getScope(), 1,
                                           this->getTimeSampling() );
            }

            m_normals.set( iSamp.getNormals() );
        }
    }
    else
    {
        SetPropUsePrevIfNull( m_positions, iSamp.getPositions() );
        SetPropUsePrevIfNull( m_indices, iSamp.getIndices() );
        SetPropUsePrevIfNull( m_counts, iSamp.getCounts() );

        if ( m_childBounds )
        {
            SetPropUsePrevIfNull( m_childBounds, iSamp.getChildBounds() );
        }

        if ( iSamp.getSelfBounds().hasVolume() )
        {
            m_selfBounds.set( iSamp.getSelfBounds() );
        }
        else if ( iSamp.getPositions() )
        {
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() ) );
            m_selfBounds.set( bnds );
        }
        else
        {
            m_selfBounds.setFromPrevious();
        }

        // OGeomParam will automatically use SetPropUsePrevIfNull internally
        if ( m_uvs ) { m_uvs.set( iSamp.getUVs() ); }
        if ( m_normals ) { m_normals.set( iSamp.getNormals() ); }
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::setFromPrevious()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::setFromPrevious" );

    m_positions.setFromPrevious();
    m_indices.setFromPrevious();
    m_counts.setFromPrevious();

    m_selfBounds.setFromPrevious();

    if (m_childBounds)
        m_childBounds.setFromPrevious();

    if ( m_uvs ) { m_uvs.setFromPrevious(); }
    if ( m_normals ) { m_normals.setFromPrevious(); }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::setTimeSampling( uint32_t iIndex )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OPolyMeshSchema::setTimeSampling( uint32_t )" );

    m_positions.setTimeSampling( iIndex );
    m_indices.setTimeSampling( iIndex );
    m_counts.setTimeSampling( iIndex );
    m_selfBounds.setTimeSampling( iIndex );

    if ( m_childBounds )
        m_childBounds.setTimeSampling( iIndex );

    if ( m_uvs )
        m_uvs.setTimeSampling( iIndex );

    if ( m_normals )
        m_normals.setTimeSampling( iIndex );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::setTimeSampling( AbcA::TimeSamplingPtr iTime )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OPolyMeshSchema::setTimeSampling( TimeSamplingPtr )" );

    if (iTime)
    {
        uint32_t tsIndex = getObject().getArchive().addTimeSampling( *iTime );
        setTimeSampling( tsIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::init( uint32_t iTsIdx )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::init()" );

    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVertexScope );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    m_positions = Abc::OV3fArrayProperty( _this, "P", mdata, iTsIdx );

    m_indices = Abc::OInt32ArrayProperty( _this, ".faceIndices", iTsIdx );

    m_counts = Abc::OInt32ArrayProperty( _this, ".faceCounts", iTsIdx );

    m_selfBounds = Abc::OBox3dProperty( _this, ".selfBnds", iTsIdx );

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
