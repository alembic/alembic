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

#include <Alembic/AbcGeom/OCurves.h>
#include <Alembic/AbcGeom/GeometryScope.h>
#include <iostream>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void OCurvesSchema::set( const Sample &iSamp, const Abc::OSampleSelector &iSS  )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCurvesSchema::set()" );

    // We could add sample integrity checking here.
    if ( iSS.getIndex() == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions(),
                     "Sample 0 must have valid data for all mesh components" );

        m_positions.set( iSamp.getPositions(), iSS );

        // set wrap, nVertices, and type
        m_type.set( iSamp.getType(), iSS);
        m_wrap.set( iSamp.getWrap(), iSS);
        m_nVertices.set( iSamp.getCurvesNumVertices(), iSS);

	if ( iSamp.getUBasis() )
	{
	    m_uBasis = Abc::OUcharProperty( this->getPtr(), "uBasis",
                                            this->getTimeSamplingType());
	    m_uBasis.set( iSamp.getUBasis(), iSS);
	}

	if ( iSamp.getVBasis() )
	{
	    m_vBasis = Abc::OUcharProperty( this->getPtr(), "vBasis",
                                            this->getTimeSamplingType());
	    m_vBasis.set( iSamp.getVBasis(), iSS);
	}

        if ( iSamp.getChildBounds().hasVolume() )
        { m_childBounds.set( iSamp.getChildBounds(), iSS ); }

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

        // process uvs
        if ( iSamp.getUVs() )
        {
            m_uvs = Abc::OV2fArrayProperty( this->getPtr(), "uv",
                                            this->getTimeSamplingType() );
            m_uvs.set( iSamp.getUVs(), iSS );
        }

        // process normals
        if ( iSamp.getNormals() )
        {
            m_normals = Abc::OV3fArrayProperty( this->getPtr(), "N",
                                                this->getTimeSamplingType() );
            m_normals.set( iSamp.getNormals(), iSS );
        }

        // process widths
        if ( iSamp.getWidths() )
        {
            m_widths = Abc::OV2fArrayProperty( this->getPtr(), "width",
                                               this->getTimeSamplingType() );

            m_widths.set( iSamp.getWidths(), iSS );
        }

    }
    else
    {
        SetPropUsePrevIfNull( m_positions, iSamp.getPositions(), iSS );
        SetPropUsePrevIfNull( m_childBounds, iSamp.getChildBounds(), iSS );
	SetPropUsePrevIfNull( m_uvs, iSamp.getUVs(), iSS );
	SetPropUsePrevIfNull( m_normals, iSamp.getNormals(), iSS );
	SetPropUsePrevIfNull( m_widths, iSamp.getWidths(), iSS );
	SetPropUsePrevIfNull( m_uBasis, iSamp.getUBasis(), iSS );
	SetPropUsePrevIfNull( m_vBasis, iSamp.getVBasis(), iSS );
        SetPropUsePrevIfNull( m_nVertices, iSamp.getCurvesNumVertices(), iSS );

        // update bounds
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
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OCurvesSchema::setFromPrevious( const Abc::OSampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCurvesSchema::setFromPrevious" );

    m_positions.setFromPrevious( iSS );
    m_type.setFromPrevious( iSS );
    m_wrap.setFromPrevious( iSS );
    m_nVertices.setFromPrevious( iSS );

    m_selfBounds.setFromPrevious( iSS );
    m_childBounds.setFromPrevious( iSS );

    if ( m_uvs ) { m_uvs.setFromPrevious( iSS ); }
    if ( m_normals ) { m_normals.setFromPrevious( iSS ); }
    if ( m_widths ) { m_widths.setFromPrevious( iSS ); }
    if ( m_uBasis ) { m_uBasis.setFromPrevious( iSS ); }
    if ( m_vBasis ) { m_vBasis.setFromPrevious( iSS ); }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OCurvesSchema::init(   const AbcA::TimeSamplingType &iTst)
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCurvesSchema::init()" );

    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVertexScope );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    m_positions = Abc::OV3fArrayProperty( _this, "P", mdata, iTst );
    m_selfBounds = Abc::OBox3dProperty( _this, ".selfBnds", iTst );
    m_childBounds = Abc::OBox3dProperty( _this, ".childBnds", iTst );

    m_type = Abc::OStringProperty( _this, "type", iTst);
    m_nVertices = Abc::OInt32ArrayProperty( _this, "nVertices", iTst);
    m_wrap = Abc::OStringProperty( _this, "wrap", iTst);

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
