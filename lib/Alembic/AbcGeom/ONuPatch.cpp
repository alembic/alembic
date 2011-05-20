//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <Alembic/AbcGeom/ONuPatch.h>
#include <Alembic/AbcGeom/GeometryScope.h>
#include <iostream>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void ONuPatchSchema::set( const ONuPatchSchema::Sample &iSamp  )
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ONuPatchSchema::set()" );

    // TODO: do this the 'simple' way. always create the trim curves
    // properties within the init, add a boolean property hasTrimCurve.

    // We could add sample integrity checking here.
    if ( m_positions.getNumSamples() == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions(),
                     "Sample 0 must have valid data for all mesh components" );

        // set required properties
        m_positions.set( iSamp.getPositions() );
        m_numU.set( iSamp.getNu() );
        m_numV.set( iSamp.getNv() );
        m_uOrder.set( iSamp.getUOrder() );
        m_vOrder.set( iSamp.getVOrder() );
        m_uKnot.set( iSamp.getUKnot() );
        m_vKnot.set( iSamp.getVKnot() );

        // set optional properties
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
                iSamp.getUVs().getScope(), 1, this->getTimeSampling() );
            }
            else
            {
                // normals are not indexed
                m_normals = ON3fGeomParam( this->getPtr(), "N", false,
                                            iSamp.getUVs().getScope(), 1,
                                            this->getTimeSampling() );
            }

            m_normals.set( iSamp.getNormals() );
        }

        m_trimNumLoops.set( iSamp.getTrimNumLoops() );

        m_trimNumCurves.set( iSamp.getTrimNumCurves() );
        m_trimNumVertices.set( iSamp.getTrimNumVertices() );
        m_trimOrder.set( iSamp.getTrimOrder() );
        m_trimKnot.set( iSamp.getTrimKnot() );
        m_trimMin.set( iSamp.getTrimMin() );
        m_trimMax.set( iSamp.getTrimMax() );
        m_trimU.set( iSamp.getTrimU() );
        m_trimV.set( iSamp.getTrimV() );
        m_trimW.set( iSamp.getTrimW() );

        // set bounds
        if ( iSamp.getChildBounds().hasVolume() )
        { m_childBounds.set( iSamp.getChildBounds() ); }

        if ( iSamp.getSelfBounds().isEmpty() )
        {
            // OTypedScalarProperty::set() is not referentially transparent,
            // so we need a a placeholder variable.
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() )
                           );

            m_selfBounds.set( bnds );

        }
        else
        {
            m_selfBounds.set( iSamp.getSelfBounds() );
        }
    }
    else
    {
        // TODO this would all go away, remove the lightweight constructor
        SetPropUsePrevIfNull( m_positions, iSamp.getPositions() );
        SetPropUsePrevIfNull( m_numU, iSamp.getNu() );
        SetPropUsePrevIfNull( m_numV, iSamp.getNv() );
        SetPropUsePrevIfNull( m_uOrder, iSamp.getUOrder() );
        SetPropUsePrevIfNull( m_vOrder, iSamp.getVOrder() );
        SetPropUsePrevIfNull( m_uKnot, iSamp.getUKnot() );
        SetPropUsePrevIfNull( m_vKnot, iSamp.getVKnot() );

        if ( m_uvs )
        {
            m_uvs.set( iSamp.getUVs() );
        }

        if ( m_normals )
        {
            m_normals.set( iSamp.getNormals() );
        }

        // handle trim curves
        if ( m_trimNumLoops )
        {
            SetPropUsePrevIfNull( m_trimNumLoops, iSamp.getTrimNumLoops() );
        }

        if ( m_trimNumCurves )
        {
            SetPropUsePrevIfNull( m_trimNumCurves, iSamp.getTrimNumCurves() );
        }

        if ( m_trimNumVertices )
        {
            SetPropUsePrevIfNull( m_trimNumVertices,
                                    iSamp.getTrimNumVertices() );
        }

        if ( m_trimOrder )
        {
            SetPropUsePrevIfNull( m_trimOrder, iSamp.getTrimOrder() );
        }

        if ( m_trimKnot )
        {
            SetPropUsePrevIfNull( m_trimKnot, iSamp.getTrimKnot() );
        }

        if ( m_trimMin )
        {
            SetPropUsePrevIfNull( m_trimMin, iSamp.getTrimMin() );
        }

        if ( m_trimMax )
        {
            SetPropUsePrevIfNull( m_trimMax, iSamp.getTrimMax() );
        }

        if ( m_trimU )
        {
            SetPropUsePrevIfNull( m_trimU, iSamp.getTrimU() );
        }

        if ( m_trimV )
        {
            SetPropUsePrevIfNull( m_trimV, iSamp.getTrimV() );
        }

        if ( m_trimW )
        {
            SetPropUsePrevIfNull( m_trimW, iSamp.getTrimW() );
        }

        // update bounds
        if ( iSamp.getSelfBounds().hasVolume() )
        {
            m_selfBounds.set( iSamp.getSelfBounds() );
        }
        else if ( iSamp.getPositions() )
        {
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() )
                           );
            m_selfBounds.set( bnds );
        }
        else
        {
            m_selfBounds.setFromPrevious();
        }
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void ONuPatchSchema::setFromPrevious( )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ONuPatchSchema::setFromPrevious" );

    // handle required properites
    m_positions.setFromPrevious();
    m_numU.setFromPrevious();
    m_numV.setFromPrevious();
    m_uOrder.setFromPrevious();
    m_vOrder.setFromPrevious();
    m_uKnot.setFromPrevious();
    m_vKnot.setFromPrevious();

    m_selfBounds.setFromPrevious();
    m_childBounds.setFromPrevious();

    // handle option properties
    if ( m_uvs ) { m_uvs.setFromPrevious(); }
    if ( m_normals ) { m_normals.setFromPrevious(); }
    
    // handle trim curves.
    m_trimNumLoops.setFromPrevious();
    m_trimNumCurves.setFromPrevious();
    m_trimNumVertices.setFromPrevious();
    m_trimOrder.setFromPrevious();
    m_trimKnot.setFromPrevious();
    m_trimMin.setFromPrevious();
    m_trimMax.setFromPrevious();
    m_trimU.setFromPrevious();
    m_trimV.setFromPrevious();
    m_trimW.setFromPrevious();
    
    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void ONuPatchSchema::init( const AbcA::index_t iTsIdx )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ONuPatchSchema::init()" );

    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVertexScope );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    // initialize any required properties
    m_positions = Abc::OV3fArrayProperty( _this, "P", mdata, iTsIdx );
    m_numU = Abc::OUInt64Property( _this, "nu", mdata, iTsIdx );
    m_numV = Abc::OUInt64Property( _this, "nv", mdata, iTsIdx );
    m_uOrder = Abc::OUInt64Property( _this, "uOrder", mdata, iTsIdx );
    m_vOrder = Abc::OUInt64Property( _this, "vOrder", mdata, iTsIdx );
    m_uKnot = Abc::OFloatArrayProperty( _this, "uKnot", mdata, iTsIdx );
    m_vKnot = Abc::OFloatArrayProperty( _this, "vKnot", mdata, iTsIdx );

    // trim curves
    m_trimNumLoops = Abc::OUInt64Property( _this, "trim_nloops", iTsIdx );
    m_trimNumCurves = Abc::OUInt64ArrayProperty( _this, "trim_ncurves",
                                                iTsIdx );
    m_trimNumVertices = Abc::OUInt64ArrayProperty( _this, "trim_n", iTsIdx );
    m_trimOrder = Abc::OUInt64ArrayProperty( _this, "trim_order", iTsIdx );
    m_trimKnot = Abc::OFloatArrayProperty( _this, "trim_knot", iTsIdx );
    m_trimMin = Abc::OFloatArrayProperty( _this, "trim_min", iTsIdx );
    m_trimMax = Abc::OFloatArrayProperty( _this, "trim_max", iTsIdx );
    m_trimU = Abc::OFloatArrayProperty( _this, "trim_u", iTsIdx );
    m_trimV = Abc::OFloatArrayProperty( _this, "trim_v", iTsIdx );
    m_trimW = Abc::OFloatArrayProperty( _this, "trim_w", iTsIdx );

    m_selfBounds = Abc::OBox3dProperty( _this, ".selfBnds", iTsIdx );
    m_childBounds = Abc::OBox3dProperty( _this, ".childBnds", iTsIdx );

    // UVs and Normals are created on first call to set()

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
Abc::OCompoundProperty ONuPatchSchema::getArbGeomParams()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ONuPatchSchema::getArbGeomParams()" );

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
