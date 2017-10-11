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

#include <Alembic/AbcGeom/ONuPatch.h>
#include <Alembic/AbcGeom/GeometryScope.h>
#include <iostream>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
ONuPatchSchema::ONuPatchSchema( AbcA::CompoundPropertyWriterPtr iParent,
                                  const std::string &iName,
                                  const Abc::Argument &iArg0,
                                  const Abc::Argument &iArg1,
                                  const Abc::Argument &iArg2,
                                  const Abc::Argument &iArg3 )
    : OGeomBaseSchema<NuPatchSchemaInfo>( iParent, iName,
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
ONuPatchSchema::ONuPatchSchema( Abc::OCompoundProperty iParent,
                                  const std::string &iName,
                                  const Abc::Argument &iArg0,
                                  const Abc::Argument &iArg1,
                                  const Abc::Argument &iArg2 )
    : OGeomBaseSchema<NuPatchSchemaInfo>( iParent.getPtr(), iName,
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
inline void SetPropUsePrevIfNull( Abc::OInt32Property iProp, int32_t iVal )
{
    if ( ! iProp ) { return; }

    if ( iVal != ABC_GEOM_NUPATCH_NULL_INT_VALUE )
    {
        iProp.set( iVal );
    }
    else
    {
        iProp.setFromPrevious();
    }
}

//-*****************************************************************************
void ONuPatchSchema::set( const ONuPatchSchema::Sample &iSamp  )
{
    if( m_selectiveExport || iSamp.isPartialSample() )
    {
        selectiveSet( iSamp );
        return;
    }

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ONuPatchSchema::set()" );

    // do we need to create velocities prop?
    if ( iSamp.getVelocities() && !m_velocitiesProperty )
    {
        createVelocityProperty();
    }

    // do we need to create uvs?
    if ( iSamp.getUVs() && !m_uvsParam )
    {
        createUVsProperty( iSamp );
    }

    // do we need to create normals?
    if ( iSamp.getNormals() && !m_normalsParam )
    {
        createNormalsProperty( iSamp );
    }

    // do we need to create position weights?
    if ( iSamp.getPositionWeights() && !m_positionWeightsProperty)
    {
        createPositionWeightsProperty();
    }

    if ( iSamp.hasTrimCurve() && !m_trimNumLoopsProperty )
    {
        createTrimPropreties();
    }


    // We could add sample integrity checking here.
    if ( m_numSamples == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions(),
                     "Sample 0 must have valid data for all mesh components" );

        // set required properties
        m_positionsProperty.set( iSamp.getPositions() );
        m_numUProperty.set( iSamp.getNu() );
        m_numVProperty.set( iSamp.getNv() );
        m_uOrderProperty.set( iSamp.getUOrder() );
        m_vOrderProperty.set( iSamp.getVOrder() );
        m_uKnotProperty.set( iSamp.getUKnot() );
        m_vKnotProperty.set( iSamp.getVKnot() );

        if ( m_trimNumLoopsProperty )
        {
            m_trimNumLoopsProperty.set( iSamp.getTrimNumLoops() );

            m_trimNumCurvesProperty.set( iSamp.getTrimNumCurves() );
            m_trimNumVerticesProperty.set( iSamp.getTrimNumVertices() );
            m_trimOrderProperty.set( iSamp.getTrimOrder() );
            m_trimKnotProperty.set( iSamp.getTrimKnot() );
            m_trimMinProperty.set( iSamp.getTrimMin() );
            m_trimMaxProperty.set( iSamp.getTrimMax() );
            m_trimUProperty.set( iSamp.getTrimU() );
            m_trimVProperty.set( iSamp.getTrimV() );
            m_trimWProperty.set( iSamp.getTrimW() );
        }

        if ( m_positionWeightsProperty )
        {
            m_positionWeightsProperty.set( iSamp.getPositionWeights() );
        }

        if ( m_velocitiesProperty )
        {
            m_velocitiesProperty.set( iSamp.getVelocities() );
        }

        if ( m_uvsParam )
        {
            m_uvsParam.set( iSamp.getUVs() );
        }

        if ( m_normalsParam )
        {
            m_normalsParam.set( iSamp.getNormals() );
        }

        if ( iSamp.getSelfBounds().isEmpty() )
        {
            // OTypedScalarProperty::set() is not referentially transparent,
            // so we need a a placeholder variable.
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() )
                           );

            m_selfBoundsProperty.set( bnds );

        }
        else
        {
            m_selfBoundsProperty.set( iSamp.getSelfBounds() );
        }
    }
    else
    {
        // TODO this would all go away, remove the lightweight constructor
        SetPropUsePrevIfNull( m_positionsProperty, iSamp.getPositions() );
        SetPropUsePrevIfNull( m_numUProperty, iSamp.getNu() );
        SetPropUsePrevIfNull( m_numVProperty, iSamp.getNv() );
        SetPropUsePrevIfNull( m_uOrderProperty, iSamp.getUOrder() );
        SetPropUsePrevIfNull( m_vOrderProperty, iSamp.getVOrder() );
        SetPropUsePrevIfNull( m_uKnotProperty, iSamp.getUKnot() );
        SetPropUsePrevIfNull( m_vKnotProperty, iSamp.getVKnot() );

        if ( m_uvsParam )
        {
            m_uvsParam.set( iSamp.getUVs() );
        }

        if ( m_normalsParam )
        {
            m_normalsParam.set( iSamp.getNormals() );
        }

        if ( m_positionWeightsProperty )
        {
            SetPropUsePrevIfNull( m_positionWeightsProperty,
                                  iSamp.getPositionWeights() );
        }

        if ( m_velocitiesProperty )
        {
            SetPropUsePrevIfNull( m_velocitiesProperty,
                                  iSamp.getVelocities() );
        }

        // handle trim curves
        if ( m_trimNumLoopsProperty )
        {
            SetPropUsePrevIfNull( m_trimNumLoopsProperty, iSamp.getTrimNumLoops() );
            SetPropUsePrevIfNull( m_trimNumCurvesProperty, iSamp.getTrimNumCurves() );
            SetPropUsePrevIfNull( m_trimNumVerticesProperty,
                                    iSamp.getTrimNumVertices() );
            SetPropUsePrevIfNull( m_trimOrderProperty, iSamp.getTrimOrder() );
            SetPropUsePrevIfNull( m_trimKnotProperty, iSamp.getTrimKnot() );
            SetPropUsePrevIfNull( m_trimMinProperty, iSamp.getTrimMin() );
            SetPropUsePrevIfNull( m_trimMaxProperty, iSamp.getTrimMax() );
            SetPropUsePrevIfNull( m_trimUProperty, iSamp.getTrimU() );
            SetPropUsePrevIfNull( m_trimVProperty, iSamp.getTrimV() );
            SetPropUsePrevIfNull( m_trimWProperty, iSamp.getTrimW() );
        }

        // update bounds
        if ( iSamp.getSelfBounds().hasVolume() )
        {
            m_selfBoundsProperty.set( iSamp.getSelfBounds() );
        }
        else if ( iSamp.getPositions() )
        {
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() )
                           );
            m_selfBoundsProperty.set( bnds );
        }
        else
        {
            m_selfBoundsProperty.setFromPrevious();
        }
    }

    m_numSamples++;

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void ONuPatchSchema::selectiveSet( const Sample &iSamp )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ONuPatchSchema::selectiveSet()" );

    if ( iSamp.getPositions() && !m_positionsProperty )
    {
        createPositionProperties();
    }

    // Assuming all knot properties get created in ::createKnotProperties()
    if( iSamp.hasKnotSampleData() && !m_numUProperty )
    {
        createKnotProperties();
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
    }

    // Assuming all knot properties get created in ::createKnotProperties()
    if( m_numUProperty )
    {
        SetPropUsePrevIfNull( m_numUProperty, iSamp.getNu() );
        SetPropUsePrevIfNull( m_numVProperty, iSamp.getNv() );
        SetPropUsePrevIfNull( m_uOrderProperty, iSamp.getUOrder() );
        SetPropUsePrevIfNull( m_vOrderProperty, iSamp.getVOrder() );
        SetPropUsePrevIfNull( m_uKnotProperty, iSamp.getUKnot() );
        SetPropUsePrevIfNull( m_vKnotProperty, iSamp.getVKnot() );
    }

    // do we need to create velocities prop?
    if ( iSamp.getVelocities() && !m_velocitiesProperty )
    {
        createVelocityProperty();
    }

    if ( m_velocitiesProperty )
    {
        SetPropUsePrevIfNull( m_velocitiesProperty, iSamp.getVelocities() );
    }

    if ( m_velocitiesProperty )
    {
        SetPropUsePrevIfNull( m_velocitiesProperty,
                              iSamp.getVelocities() );
    }

    // do we need to create uvs?
    if ( iSamp.getUVs() && !m_uvsParam )
    {
        createUVsProperty( iSamp );
    }

    if ( m_uvsParam )
    {
        m_uvsParam.set( iSamp.getUVs() );
    }

    // do we need to create normals?
    if ( iSamp.getNormals() && !m_normalsParam )
    {
        createNormalsProperty( iSamp );
    }

    if ( m_normalsParam )
    {
        m_normalsParam.set( iSamp.getNormals() );
    }

    // do we need to create position weights?
    if ( iSamp.getPositionWeights() && !m_positionWeightsProperty)
    {
        createPositionWeightsProperty();
    }

    if ( m_positionWeightsProperty )
    {
        SetPropUsePrevIfNull( m_positionWeightsProperty,
                              iSamp.getPositionWeights() );
    }

    if ( iSamp.hasTrimCurve() && !m_trimNumLoopsProperty )
    {
        createTrimPropreties();
    }

    if ( m_trimNumLoopsProperty )
    {
        SetPropUsePrevIfNull( m_trimNumLoopsProperty, iSamp.getTrimNumLoops() );
        SetPropUsePrevIfNull( m_trimNumCurvesProperty, iSamp.getTrimNumCurves() );
        SetPropUsePrevIfNull( m_trimNumVerticesProperty,
                                iSamp.getTrimNumVertices() );
        SetPropUsePrevIfNull( m_trimOrderProperty, iSamp.getTrimOrder() );
        SetPropUsePrevIfNull( m_trimKnotProperty, iSamp.getTrimKnot() );
        SetPropUsePrevIfNull( m_trimMinProperty, iSamp.getTrimMin() );
        SetPropUsePrevIfNull( m_trimMaxProperty, iSamp.getTrimMax() );
        SetPropUsePrevIfNull( m_trimUProperty, iSamp.getTrimU() );
        SetPropUsePrevIfNull( m_trimVProperty, iSamp.getTrimV() );
        SetPropUsePrevIfNull( m_trimWProperty, iSamp.getTrimW() );
    }

    m_numSamples++;

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void ONuPatchSchema::createPositionProperties()
{
    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVertexScope );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    // initialize any required properties
    m_positionsProperty = Abc::OP3fArrayProperty( _this, "P", mdata, m_timeSamplingIndex );
    
    std::vector<V3f> emptyVec;
    const V3fArraySample empty( emptyVec );
    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_positionsProperty.set( empty );
    }

    createSelfBoundsProperty( m_timeSamplingIndex, m_numSamples );
}

//-*****************************************************************************
void ONuPatchSchema::createKnotProperties()
{
    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVertexScope );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    // initialize any required properties
    m_numUProperty = Abc::OInt32Property( _this, "nu", m_timeSamplingIndex );
    m_numVProperty = Abc::OInt32Property( _this, "nv", m_timeSamplingIndex );
    m_uOrderProperty = Abc::OInt32Property( _this, "uOrder", m_timeSamplingIndex );
    m_vOrderProperty = Abc::OInt32Property( _this, "vOrder", m_timeSamplingIndex );
    m_uKnotProperty = Abc::OFloatArrayProperty( _this, "uKnot", m_timeSamplingIndex );
    m_vKnotProperty = Abc::OFloatArrayProperty( _this, "vKnot", m_timeSamplingIndex );

    const FloatArraySample emptyFloatSample;
    const float floatSample = 0.0f;
    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_numUProperty.set( floatSample );
        m_numVProperty.set( floatSample );
        m_uOrderProperty.set( floatSample );
        m_vOrderProperty.set( floatSample );
        m_uKnotProperty.set( emptyFloatSample );
        m_vKnotProperty.set( emptyFloatSample );
    }
}

//-*****************************************************************************
void ONuPatchSchema::createVelocityProperty()
{
    m_velocitiesProperty = Abc::OV3fArrayProperty( this->getPtr(),
        ".velocities", m_timeSamplingIndex );

    std::vector<V3f> emptyVec;
    const V3fArraySample empty( emptyVec );
    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_velocitiesProperty.set( empty );
    }
}

//-*****************************************************************************
void ONuPatchSchema::createUVsProperty( const Sample &iSamp )
{
    std::vector<V2f> emptyVals;
    std::vector<Util::uint32_t> emptyIndices;

    OV2fGeomParam::Sample empty;

    if ( iSamp.getUVs().getIndices() )
    {
        empty = OV2fGeomParam::Sample( Abc::V2fArraySample( emptyVals ),
            Abc::UInt32ArraySample( emptyIndices ),
            iSamp.getUVs().getScope() );

        // UVs are indexed
        m_uvsParam = OV2fGeomParam( this->getPtr(), "uv", true,
                                    empty.getScope(), 1,
                                    this->getTimeSampling() );
    }
    else
    {
        empty = OV2fGeomParam::Sample( Abc::V2fArraySample( emptyVals ),
                                       iSamp.getUVs().getScope() );

        // UVs are not indexed
        m_uvsParam = OV2fGeomParam( this->getPtr(), "uv", false,
                               empty.getScope(), 1,
                               this->getTimeSampling() );
    }

    // set all the missing samples
    for ( size_t i = 0; i < m_numSamples; ++i )
    {
        m_uvsParam.set( empty );
    }
}

//-*****************************************************************************
void ONuPatchSchema::createNormalsProperty( const Sample &iSamp )
{
    std::vector<V3f> emptyVals;
    std::vector<Util::uint32_t> emptyIndices;

    ON3fGeomParam::Sample empty;

    if ( iSamp.getNormals().getIndices() )
    {
        empty = ON3fGeomParam::Sample( Abc::V3fArraySample( emptyVals ),
            Abc::UInt32ArraySample( emptyIndices ),
            iSamp.getNormals().getScope() );

        // normals are indexed
        m_normalsParam = ON3fGeomParam( this->getPtr(), "N", true,
            empty.getScope(), 1, this->getTimeSampling() );
    }
    else
    {
        empty = ON3fGeomParam::Sample( Abc::V3fArraySample( emptyVals ),
                                       iSamp.getNormals().getScope() );

        // normals are not indexed
        m_normalsParam = ON3fGeomParam( this->getPtr(), "N", false,
                                    empty.getScope(), 1,
                                    this->getTimeSampling() );
    }

    // set all the missing samples
    for ( size_t i = 0; i < m_numSamples; ++i )
    {
        m_normalsParam.set( empty );
    }
}

//-*****************************************************************************
void ONuPatchSchema::createPositionWeightsProperty()
{
    m_positionWeightsProperty = Abc::OFloatArrayProperty( *this, "w",
                                                  this->getTimeSampling() );

    std::vector<float> emptyVec;
    Alembic::Abc::FloatArraySample emptySamp( emptyVec );

    // set all the missing samples
    for ( size_t i = 0; i < m_numSamples; ++i )
    {
        m_positionWeightsProperty.set( emptySamp );
    }
}

//-*****************************************************************************
void ONuPatchSchema::createTrimPropreties()
{
    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    std::vector<Util::int32_t> emptyIntVec;
    std::vector<float> emptyFloatVec;
    Alembic::Abc::Int32ArraySample emptyIntSamp( emptyIntVec );
    Alembic::Abc::FloatArraySample emptyFloatSamp( emptyFloatVec );

    AbcA::TimeSamplingPtr tsPtr = this->getTimeSampling();

    // trim curves
    m_trimNumLoopsProperty = Abc::OInt32Property( _this, "trim_nloops", tsPtr );
    m_trimNumCurvesProperty = Abc::OInt32ArrayProperty( _this, "trim_ncurves",
                                                 tsPtr );
    m_trimNumVerticesProperty = Abc::OInt32ArrayProperty( _this, "trim_n", tsPtr );
    m_trimOrderProperty = Abc::OInt32ArrayProperty( _this, "trim_order", tsPtr );
    m_trimKnotProperty = Abc::OFloatArrayProperty( _this, "trim_knot", tsPtr );
    m_trimMinProperty = Abc::OFloatArrayProperty( _this, "trim_min", tsPtr );
    m_trimMaxProperty = Abc::OFloatArrayProperty( _this, "trim_max", tsPtr );
    m_trimUProperty = Abc::OFloatArrayProperty( _this, "trim_u", tsPtr );
    m_trimVProperty = Abc::OFloatArrayProperty( _this, "trim_v", tsPtr );
    m_trimWProperty = Abc::OFloatArrayProperty( _this, "trim_w", tsPtr );

    // set all the missing samples
    for ( size_t i = 0; i < m_numSamples; ++i )
    {
        m_trimNumLoopsProperty.set( 0 );

        m_trimNumCurvesProperty.set( emptyIntSamp );
        m_trimNumVerticesProperty.set( emptyIntSamp );
        m_trimOrderProperty.set( emptyIntSamp );
        m_trimKnotProperty.set( emptyFloatSamp );
        m_trimMinProperty.set( emptyFloatSamp );
        m_trimMaxProperty.set( emptyFloatSamp );
        m_trimUProperty.set( emptyFloatSamp );
        m_trimVProperty.set( emptyFloatSamp );
        m_trimWProperty.set( emptyFloatSamp );
    }
}

//-*****************************************************************************
void ONuPatchSchema::setFromPrevious( )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ONuPatchSchema::setFromPrevious" );

    // handle required properites
    if( m_positionsProperty ) { m_positionsProperty.setFromPrevious(); }
    if( m_numUProperty ) { m_numUProperty.setFromPrevious(); }
    if( m_numVProperty ) { m_numVProperty.setFromPrevious(); }
    if( m_uOrderProperty ) { m_uOrderProperty.setFromPrevious(); }
    if( m_vOrderProperty ) { m_vOrderProperty.setFromPrevious(); }
    if( m_uKnotProperty ) { m_uKnotProperty.setFromPrevious(); }
    if( m_vKnotProperty ) { m_vKnotProperty.setFromPrevious(); }

    m_selfBoundsProperty.setFromPrevious();

    // handle optional properties
    if ( m_velocitiesProperty ) { m_velocitiesProperty.setFromPrevious(); }
    if ( m_uvsParam ) { m_uvsParam.setFromPrevious(); }
    if ( m_normalsParam ) { m_normalsParam.setFromPrevious(); }
    if ( m_positionWeightsProperty )
    {
        m_positionWeightsProperty.setFromPrevious();
    }

    // handle trim curves.
    if ( m_trimNumLoopsProperty )
    {
        m_trimNumLoopsProperty.setFromPrevious();
        m_trimNumCurvesProperty.setFromPrevious();
        m_trimNumVerticesProperty.setFromPrevious();
        m_trimOrderProperty.setFromPrevious();
        m_trimKnotProperty.setFromPrevious();
        m_trimMinProperty.setFromPrevious();
        m_trimMaxProperty.setFromPrevious();
        m_trimUProperty.setFromPrevious();
        m_trimVProperty.setFromPrevious();
        m_trimWProperty.setFromPrevious();
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void ONuPatchSchema::setTimeSampling( uint32_t iIndex )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "ONuPatchSchema::setTimeSampling( uint32_t )" );

    m_timeSamplingIndex = iIndex;

    if( m_positionsProperty )
    {
        m_positionsProperty.setTimeSampling( iIndex );
        m_numUProperty.setTimeSampling( iIndex );
        m_numVProperty.setTimeSampling( iIndex );
        m_uOrderProperty.setTimeSampling( iIndex );
        m_vOrderProperty.setTimeSampling( iIndex );
        m_uKnotProperty.setTimeSampling( iIndex );
        m_vKnotProperty.setTimeSampling( iIndex );
    }

    if( m_selfBoundsProperty )
    {
        m_selfBoundsProperty.setTimeSampling( iIndex );
    }

    if ( m_velocitiesProperty )
    {
        m_velocitiesProperty.setTimeSampling( iIndex );
    }

    if ( m_uvsParam ) { m_uvsParam.setTimeSampling( iIndex ); }
    if ( m_normalsParam ) { m_normalsParam.setTimeSampling( iIndex ); }
    if ( m_positionWeightsProperty )
    {
        m_positionWeightsProperty.setTimeSampling( iIndex );
    }

    if ( m_trimNumLoopsProperty )
    {
        m_trimNumLoopsProperty.setTimeSampling( iIndex );
        m_trimNumCurvesProperty.setTimeSampling( iIndex );
        m_trimNumVerticesProperty.setTimeSampling( iIndex );
        m_trimOrderProperty.setTimeSampling( iIndex );
        m_trimKnotProperty.setTimeSampling( iIndex );
        m_trimMinProperty.setTimeSampling( iIndex );
        m_trimMaxProperty.setTimeSampling( iIndex );
        m_trimUProperty.setTimeSampling( iIndex );
        m_trimVProperty.setTimeSampling( iIndex );
        m_trimWProperty.setTimeSampling( iIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void ONuPatchSchema::setTimeSampling( AbcA::TimeSamplingPtr iTime )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "ONuPatchSchema::setTimeSampling( TimeSamplingPtr )" );

    if ( iTime )
    {
        uint32_t tsIndex = getObject().getArchive().addTimeSampling( *iTime );
        setTimeSampling( tsIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void ONuPatchSchema::init( const AbcA::index_t iTsIdx, bool isSparse )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ONuPatchSchema::init()" );

    m_selectiveExport = isSparse;

    m_numSamples = 0;

    m_timeSamplingIndex = iTsIdx;

    if ( m_selectiveExport )
    {
        return;
    }

    createPositionProperties();
    createKnotProperties();

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcGeom
} // End namespace Alembic
