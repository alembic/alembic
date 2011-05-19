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

    Alembic::Util::uint8_t basisAndType[4];
    basisAndType[0] = iSamp.getType();
    basisAndType[1] = iSamp.getWrap();
    basisAndType[2] = iSamp.getBasis();

    // repeat so we don't have to change the data layout and bump up
    // the version number
    basisAndType[3] = basisAndType[2];

    // do we need to create child bounds?
    if ( iSamp.getChildBounds().hasVolume() && !m_childBounds)
    {
        m_childBounds = Abc::OBox3dProperty( *this, ".childBnds",
                                             m_positions.getTimeSampling() );
        Abc::Box3d emptyBox;
        emptyBox.makeEmpty();

        size_t numSamples = m_positions.getNumSamples();

        // set all the missing samples
        for ( size_t i = 0; i < numSamples; ++i )
        {
            m_childBounds.set( emptyBox );
        }
    }

    // do we need to create uvs?
    if ( iSamp.getUVs() && !m_uvs )
    {
        m_uvs = Abc::OV2fArrayProperty( this->getPtr(), "uv",
            m_positions.getTimeSampling() );

        Abc::V2fArraySample empty;

        size_t numSamples = m_positions.getNumSamples();

        // set all the missing samples
        for ( size_t i = 0; i < numSamples; ++i )
        {
            m_uvs.set( empty );
        }
    }

    // do we need to create normals?
    if ( iSamp.getNormals() && !m_normals )
    {
        m_normals = Abc::OV3fArrayProperty( this->getPtr(), "N",
            m_positions.getTimeSampling() );

        Abc::V3fArraySample empty;

        size_t numSamples = m_positions.getNumSamples();

        // set all the missing samples
        for ( size_t i = 0; i < numSamples; ++i )
        {
            m_normals.set( empty );
        }
    }

    // do we need to create widths?
    if ( iSamp.getWidths() && !m_widths )
    {
        m_widths = Abc::OFloatArrayProperty( this->getPtr(), "width",
            m_positions.getTimeSampling() );

        Abc::FloatArraySample empty;

        size_t numSamples = m_positions.getNumSamples();

        // set all the missing samples
        for ( size_t i = 0; i < numSamples; ++i )
        {
            m_widths.set( empty );
        }
    }

    // We could add sample integrity checking here.
    if ( m_positions.getNumSamples() == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions(),
                     "Sample 0 must have valid data for all mesh components" );

        m_positions.set( iSamp.getPositions() );
        m_nVertices.set( iSamp.getCurvesNumVertices() );

        m_basisAndType.set( basisAndType );

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
            m_uvs.set( iSamp.getUVs() );
        }

        // process normals
        if ( iSamp.getNormals() )
        {
            m_normals.set( iSamp.getNormals() );
        }

        // process widths
        if ( iSamp.getWidths() )
        {
            m_widths.set( iSamp.getWidths() );
        }

    }
    else
    {
        SetPropUsePrevIfNull( m_positions, iSamp.getPositions() );
        SetPropUsePrevIfNull( m_nVertices, iSamp.getCurvesNumVertices() );

        // if number of vertices were specified, then the basis and type
        // was specified
        if ( m_nVertices )
        {
            m_basisAndType.set( basisAndType );
        }
        else
        {
            m_basisAndType.setFromPrevious();
        }

        if ( m_childBounds )
        { SetPropUsePrevIfNull( m_childBounds, iSamp.getChildBounds() ); }

        if ( m_uvs )
        { SetPropUsePrevIfNull( m_uvs, iSamp.getUVs() ); }

        if ( m_normals )
        { SetPropUsePrevIfNull( m_normals, iSamp.getNormals() ); }

        if ( m_widths )
        { SetPropUsePrevIfNull( m_widths, iSamp.getWidths() ); }

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
    m_nVertices.setFromPrevious();

    m_basisAndType.setFromPrevious();

    m_selfBounds.setFromPrevious();

    if ( m_childBounds ) { m_childBounds.setFromPrevious(); }

    if ( m_uvs ) { m_uvs.setFromPrevious(); }
    if ( m_normals ) { m_normals.setFromPrevious(); }
    if ( m_widths ) { m_widths.setFromPrevious(); }
    
    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OCurvesSchema::init( const AbcA::index_t iTsIdx )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCurvesSchema::init()" );

    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVertexScope );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    m_positions = Abc::OV3fArrayProperty( _this, "P", mdata, iTsIdx );
    m_selfBounds = Abc::OBox3dProperty( _this, ".selfBnds", iTsIdx );

    m_nVertices = Abc::OUInt32ArrayProperty( _this, "nVertices", iTsIdx);

    m_basisAndType = Abc::OScalarProperty( _this, "curveBasisAndType",
        AbcA::DataType( Alembic::Util::kUint8POD, 4 ), iTsIdx );

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
