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

#include <Alembic/AbcGeom/OPoints.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void OPointsSchema::set( const Sample &iSamp )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::set()" );

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

    // do we need to create velocities prop?
    if ( iSamp.getVelocities() && !m_velocities )
    {
        m_velocities = Abc::OV3fArrayProperty( this->getPtr(), ".velocities",
                                               m_positions.getTimeSampling() );

        const V3fArraySample empty;
        const size_t numSamps = m_positions.getNumSamples();
        for ( size_t i = 0 ; i < numSamps ; ++i )
        {
            m_velocities.set( empty );
        }
    }

    // We could add sample integrity checking here.
    if ( m_positions.getNumSamples() == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions() &&
                     iSamp.getIds(),
                     "Sample 0 must have valid data for points and ids" );
        m_positions.set( iSamp.getPositions() );
        m_ids.set( iSamp.getIds() );

        if ( m_velocities )
        { m_velocities.set( iSamp.getVelocities() ); }

        if ( m_childBounds )
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
    }
    else
    {
        SetPropUsePrevIfNull( m_positions, iSamp.getPositions() );
        SetPropUsePrevIfNull( m_ids, iSamp.getIds() );
        SetPropUsePrevIfNull( m_velocities, iSamp.getVelocities() );

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
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPointsSchema::setFromPrevious()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::setFromPrevious" );

    m_positions.setFromPrevious();
    m_ids.setFromPrevious();

    m_selfBounds.setFromPrevious();

    if ( m_childBounds )
    {
        m_childBounds.setFromPrevious();
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPointsSchema::setTimeSampling( uint32_t iIndex )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OPointsSchema::setTimeSampling( uint32_t )" );

    m_positions.setTimeSampling( iIndex );
    m_ids.setTimeSampling( iIndex );
    m_selfBounds.setTimeSampling( iIndex );

    if ( m_childBounds )
    {
        m_childBounds.setTimeSampling( iIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPointsSchema::setTimeSampling( AbcA::TimeSamplingPtr iTime )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OPointsSchema::setTimeSampling( TimeSamplingPtr )" );

    if (iTime)
    {
        uint32_t tsIndex = getObject().getArchive().addTimeSampling( *iTime );
        setTimeSampling( tsIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
Abc::OCompoundProperty OPointsSchema::getArbGeomParams()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::getArbGeomParams()" );

    if ( ! m_arbGeomParams )
    {
        m_arbGeomParams = Abc::OCompoundProperty( *this, ".arbGeomParams" );
    }

    return m_arbGeomParams;

    ALEMBIC_ABC_SAFE_CALL_END();

    Abc::OCompoundProperty ret;
    return ret;
}

//-*****************************************************************************
void OPointsSchema::init( uint32_t iTsIdx )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::init()" );

    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVaryingScope );
    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    m_positions = Abc::OV3fArrayProperty( _this, "P", mdata, iTsIdx );

    m_ids = Abc::OUInt64ArrayProperty( _this, ".pointIds", mdata, iTsIdx );

    m_selfBounds = Abc::OBox3dProperty( _this, ".selfBnds", iTsIdx );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
