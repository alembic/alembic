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

#include <Alembic/AbcGeom/OCurves.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void OCurvesSchema::set( const OCurvesSchema::Sample &iSamp )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCurvesSchema::set()" );

    // We could add sample integrity checking here.
    if ( m_positions.getNumSamples() == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions(),
                     "Sample 0 must have valid data for all mesh components" );

        m_positions.set( iSamp.getPositions() );

        // set wrap, nVertices, and type
        m_type.set( iSamp.getType() );
        m_wrap.set( iSamp.getWrap() );
        m_nVertices.set( iSamp.getCurvesNumVertices() );

	if ( iSamp.getUBasis() )
	{
	    m_uBasis = Abc::OUcharProperty( this->getPtr(), "uBasis",
                                            m_timeSamplingIndex );
	    m_uBasis.set( iSamp.getUBasis() );
	}

	if ( iSamp.getVBasis() )
	{
	    m_vBasis = Abc::OUcharProperty( this->getPtr(), "vBasis",
                                            m_timeSamplingIndex );
	    m_vBasis.set( iSamp.getVBasis() );
	}

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
        else { m_selfBounds.set( iSamp.getSelfBounds() ); }

        // process uvs
        if ( iSamp.getUVs() )
        {
            m_uvs = Abc::OV2fArrayProperty( this->getPtr(), "uv",
                                            m_timeSamplingIndex );
            m_uvs.set( iSamp.getUVs() );
        }

        // process normals
        if ( iSamp.getNormals() )
        {
            m_normals = Abc::OV3fArrayProperty( this->getPtr(), "N",
                                                m_timeSamplingIndex );
            m_normals.set( iSamp.getNormals() );
        }

        // process widths
        if ( iSamp.getWidths() )
        {
            m_widths = Abc::OV2fArrayProperty( this->getPtr(), "width",
                                               m_timeSamplingIndex );
            m_widths.set( iSamp.getWidths() );
        }

    }
    else
    {
        SetPropUsePrevIfNull( m_positions, iSamp.getPositions() );
        SetPropUsePrevIfNull( m_nVertices, iSamp.getCurvesNumVertices() );
        SetPropUsePrevIfNull( m_type, iSamp.getType() );
        SetPropUsePrevIfNull( m_wrap, iSamp.getWrap() );

        if ( m_childBounds )
        { SetPropUsePrevIfNull( m_childBounds, iSamp.getChildBounds() ); }

        if ( m_uvs )
        { SetPropUsePrevIfNull( m_uvs, iSamp.getUVs() ); }

        if ( m_normals )
        { SetPropUsePrevIfNull( m_normals, iSamp.getNormals() ); }

        if ( m_widths )
        { SetPropUsePrevIfNull( m_widths, iSamp.getWidths() ); }

        if ( m_uBasis )
        { m_uBasis.set( iSamp.getUBasis() ); }

        if ( m_vBasis )
	{ m_vBasis.set( iSamp.getVBasis() ); }

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
void OCurvesSchema::setFromPrevious()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCurvesSchema::setFromPrevious" );

    m_positions.setFromPrevious();
    m_type.setFromPrevious();
    m_wrap.setFromPrevious();
    m_nVertices.setFromPrevious();

    m_selfBounds.setFromPrevious();

    if ( m_childBounds ) { m_childBounds.setFromPrevious(); }

    if ( m_uvs ) { m_uvs.setFromPrevious(); }
    if ( m_normals ) { m_normals.setFromPrevious(); }
    if ( m_widths ) { m_widths.setFromPrevious(); }
    if ( m_uBasis ) { m_uBasis.setFromPrevious(); }
    if ( m_vBasis ) { m_vBasis.setFromPrevious(); }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OCurvesSchema::init( const AbcA::index_t iTsIdx )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCurvesSchema::init()" );

    m_timeSamplingIndex = iTsIdx;

    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVertexScope );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    m_positions = Abc::OV3fArrayProperty( _this, "P", mdata, iTsIdx );
    m_selfBounds = Abc::OBox3dProperty( _this, ".selfBnds", iTsIdx );
    m_childBounds = Abc::OBox3dProperty( _this, ".childBnds", iTsIdx );

    m_type = Abc::OStringProperty( _this, "type", iTsIdx );
    m_nVertices = Abc::OInt32ArrayProperty( _this, "nVertices", iTsIdx);
    m_wrap = Abc::OStringProperty( _this, "wrap", iTsIdx );

    // UVs, Normals, and Widths are created on first call to set()

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
Abc::OCompoundProperty OCurvesSchema::getArbGeomParams()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCurvesSchema::getArbGeomParams()" );

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
