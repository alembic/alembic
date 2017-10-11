//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
OPointsSchema::OPointsSchema( AbcA::CompoundPropertyWriterPtr iParent,
                                  const std::string &iName,
                                  const Abc::Argument &iArg0,
                                  const Abc::Argument &iArg1,
                                  const Abc::Argument &iArg2,
                                  const Abc::Argument &iArg3 )
    : OGeomBaseSchema<PointsSchemaInfo>( iParent, iName,
                                           iArg0, iArg1, iArg2, iArg3 )
{

    AbcA::TimeSamplingPtr tsPtr =
        Abc::GetTimeSampling( iArg0, iArg1, iArg2, iArg3 );
    uint32_t tsIndex =
        Abc::GetTimeSamplingIndex( iArg0, iArg1, iArg2, iArg3 );

    // if we specified a valid TimeSamplingPtr, use it to determine the
    // index otherwise we'll use the index, which defaults to the intrinsic
    // 0 index
    if ( tsPtr )
    {
        tsIndex = iParent->getObject()->getArchive()->addTimeSampling(*tsPtr);
    }

    // Meta data and error handling are eaten up by
    // the super type, so all that's left is time sampling.
    init( tsIndex, Abc::IsSparse( iArg0, iArg1, iArg2, iArg3 ) );
}

//-*****************************************************************************
OPointsSchema::OPointsSchema( Abc::OCompoundProperty iParent,
                                  const std::string &iName,
                                  const Abc::Argument &iArg0,
                                  const Abc::Argument &iArg1,
                                  const Abc::Argument &iArg2 )
    : OGeomBaseSchema<PointsSchemaInfo>( iParent.getPtr(), iName,
                                           GetErrorHandlerPolicy( iParent ),
                                           iArg0, iArg1, iArg2 )
{
    AbcA::TimeSamplingPtr tsPtr =
        Abc::GetTimeSampling( iArg0, iArg1, iArg2 );
    uint32_t tsIndex =
        Abc::GetTimeSamplingIndex( iArg0, iArg1, iArg2 );

    // if we specified a valid TimeSamplingPtr, use it to determine the
    // index otherwise we'll use the index, which defaults to the intrinsic
    // 0 index
    if ( tsPtr )
    {
        tsIndex = iParent.getPtr()->getObject()->getArchive()->addTimeSampling(
            *tsPtr);
    }

    // Meta data and error handling are eaten up by
    // the super type, so all that's left is time sampling.
    init( tsIndex, Abc::IsSparse( iArg0, iArg1, iArg2 ) );
}

//-*****************************************************************************
void OPointsSchema::set( const Sample &iSamp )
{
    if( m_selectiveExport || iSamp.isPartialSample() )
    {
        selectiveSet( iSamp );
        return;
    }

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::set()" );

    // do we need to create velocities prop?
    if ( iSamp.getVelocities() && !m_velocitiesProperty )
    {
        createVelocityProperty();
    }

    // do we need to create widths prop?
    if ( iSamp.getWidths() && !m_widthsParam )
    {
       createWidthsProperty( iSamp );
    }

    // We could add sample integrity checking here.
    if ( m_numSamples == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions() &&
                     iSamp.getIds(),
                     "Sample 0 must have valid data for points and ids" );
        m_positionsProperty.set( iSamp.getPositions() );
        m_idsProperty.set( iSamp.getIds() );

        if ( m_velocitiesProperty )
        { m_velocitiesProperty.set( iSamp.getVelocities() ); }

        if ( m_widthsParam )
        { m_widthsParam.set( iSamp.getWidths() ); }

        if ( iSamp.getSelfBounds().isEmpty() )
        {
            // OTypedScalarProperty::set() is not referentially transparent,
            // so we need a a placeholder variable.
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() ) );
            m_selfBoundsProperty.set( bnds );
        }
        else { m_selfBoundsProperty.set( iSamp.getSelfBounds() ); }
    }
    else
    {
        SetPropUsePrevIfNull( m_positionsProperty, iSamp.getPositions() );
        SetPropUsePrevIfNull( m_idsProperty, iSamp.getIds() );
        SetPropUsePrevIfNull( m_velocitiesProperty, iSamp.getVelocities() );

        if ( iSamp.getSelfBounds().hasVolume() )
        {
            m_selfBoundsProperty.set( iSamp.getSelfBounds() );
        }
        else if ( iSamp.getPositions() )
        {
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() ) );
            m_selfBoundsProperty.set( bnds );
        }
        else
        {
            m_selfBoundsProperty.setFromPrevious();
        }

        if ( m_widthsParam )
        { m_widthsParam.set( iSamp.getWidths() ); }
    }

    m_numSamples++;

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPointsSchema::selectiveSet( const Sample &iSamp )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::set()" );

    if ( iSamp.getPositions() && !m_positionsProperty )
    {
        createPositionProperty();
    }

    if( m_positionsProperty )
    {
        SetPropUsePrevIfNull( m_positionsProperty, iSamp.getPositions() );

        if ( iSamp.getSelfBounds().hasVolume() )
        {
            m_selfBoundsProperty.set( iSamp.getSelfBounds() );
        }
        else if ( iSamp.getPositions() )
        {
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() ) );
            m_selfBoundsProperty.set( bnds );
        }
        else
        {
            m_selfBoundsProperty.setFromPrevious();
        }
    }

    if ( iSamp.getIds() && !m_idsProperty)
    {
        createIdProperty();
    }

    if( m_idsProperty )
    {
        SetPropUsePrevIfNull( m_idsProperty, iSamp.getIds() );
    }

    if ( iSamp.getVelocities() && !m_velocitiesProperty )
    {
        createVelocityProperty();
    }

    if( m_velocitiesProperty )
    {
        m_velocitiesProperty.set( iSamp.getVelocities() );
    }

    if ( iSamp.getWidths() && !m_widthsParam )
    {
       createWidthsProperty( iSamp );
    }

    if( m_widthsParam )
    {
        m_widthsParam.set( iSamp.getWidths() );
    }

    m_numSamples++;

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPointsSchema::setFromPrevious()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::setFromPrevious" );

    if ( m_positionsProperty ) { m_positionsProperty.setFromPrevious(); }
    if ( m_idsProperty ) { m_idsProperty.setFromPrevious(); }
    if ( m_selfBoundsProperty ) { m_selfBoundsProperty.setFromPrevious(); }
    if ( m_velocitiesProperty ) { m_velocitiesProperty.setFromPrevious(); }
    if ( m_widthsParam ) { m_widthsParam.setFromPrevious(); }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPointsSchema::setTimeSampling( uint32_t iIndex )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OPointsSchema::setTimeSampling( uint32_t )" );

    m_timeSamplingIndex = iIndex;

    if ( m_positionsProperty ) { m_positionsProperty.setTimeSampling( iIndex ); }
    if ( m_idsProperty ) { m_idsProperty.setTimeSampling( iIndex ); }
    if ( m_selfBoundsProperty ) { m_selfBoundsProperty.setTimeSampling( iIndex ); }
    if ( m_widthsParam ) { m_widthsParam.setTimeSampling( iIndex ); }
    if ( m_velocitiesProperty ) { m_velocitiesProperty.setTimeSampling( iIndex ); }

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
void OPointsSchema::init( uint32_t iTsIdx, bool isSparse )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::init()" );

    m_selectiveExport = isSparse;

    m_numSamples = 0;

    m_timeSamplingIndex = iTsIdx;

    if ( m_selectiveExport )
    {
        return;
    }

    createPositionProperty();
    createIdProperty();

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void OPointsSchema::createPositionProperty()
{
    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVaryingScope );
    
    m_positionsProperty = Abc::OP3fArrayProperty( this->getPtr(), "P", mdata, m_timeSamplingIndex );

    std::vector<V3f> emptyV3Vec;
    const V3fArraySample emptyV3ArraySamp( emptyV3Vec );
    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_positionsProperty.set( emptyV3ArraySamp );
    }

    createSelfBoundsProperty( m_timeSamplingIndex, m_numSamples );
}

//-*****************************************************************************
void OPointsSchema::createIdProperty()
{
    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVaryingScope );

    m_idsProperty = Abc::OUInt64ArrayProperty( this->getPtr(), ".pointIds", mdata,
        m_timeSamplingIndex );

    std::vector<int64_t> emptyInt64Vec;
    const Int64ArraySample emptyInt64ArraySamp( emptyInt64Vec );
    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_idsProperty.set( emptyInt64ArraySamp );
    }
}

//-*****************************************************************************
void OPointsSchema::createVelocityProperty()
{
    m_velocitiesProperty = Abc::OV3fArrayProperty( this->getPtr(), ".velocities",
                                           this->getTimeSampling() );

    std::vector<V3f> emptyVec;
    const V3fArraySample empty( emptyVec );
    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_velocitiesProperty.set( empty );
    }
}

//-*****************************************************************************
void OPointsSchema::createWidthsProperty( const Sample &iSamp )
{
    std::vector<float> emptyVals;
    std::vector<Util::uint32_t> emptyIndices;
    OFloatGeomParam::Sample empty;

    if ( iSamp.getWidths().getIndices() )
    {
       empty = OFloatGeomParam::Sample( Abc::FloatArraySample( emptyVals ),
           Abc::UInt32ArraySample( emptyIndices ),
           iSamp.getWidths().getScope() );

       // widths are indexed which is wasteful, but technically ok
       m_widthsParam = OFloatGeomParam( this->getPtr(), ".widths", true,
                                        iSamp.getWidths().getScope(),
                                        1, this->getTimeSampling() );
    }
    else
    {
       empty = OFloatGeomParam::Sample( Abc::FloatArraySample( emptyVals ),
                                        iSamp.getWidths().getScope() );

       // widths are not indexed
       m_widthsParam = OFloatGeomParam( this->getPtr(), ".widths", false,
                                        iSamp.getWidths().getScope(), 1,
                                        this->getTimeSampling() );
    }

    // set all the missing samples
    for ( size_t i = 0; i < m_numSamples; ++i )
    {
       m_widthsParam.set( empty );
    }
}


} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcGeom
} // End namespace Alembic
