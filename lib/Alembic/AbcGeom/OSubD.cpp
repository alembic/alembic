//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#include <Alembic/AbcGeom/OSubD.h>
#include <Alembic/AbcGeom/GeometryScope.h>
#include <Alembic/AbcGeom/OFaceSet.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
OSubDSchema::OSubDSchema( AbcA::CompoundPropertyWriterPtr iParent,
                                  const std::string &iName,
                                  const Abc::Argument &iArg0,
                                  const Abc::Argument &iArg1,
                                  const Abc::Argument &iArg2,
                                  const Abc::Argument &iArg3 )
    : OGeomBaseSchema<SubDSchemaInfo>( iParent, iName,
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
OSubDSchema::OSubDSchema( Abc::OCompoundProperty iParent,
                                  const std::string &iName,
                                  const Abc::Argument &iArg0,
                                  const Abc::Argument &iArg1,
                                  const Abc::Argument &iArg2 )
    : OGeomBaseSchema<SubDSchemaInfo>( iParent.getPtr(), iName,
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
    if ( iVal != ABC_GEOM_SUBD_NULL_INT_VALUE )
    {
        iProp.set( iVal );
    }
    else
    {
        iProp.setFromPrevious( );
    }
}

//-*****************************************************************************
void OSubDSchema::set( const Sample &iSamp )
{
    if( m_selectiveExport || iSamp.isPartialSample() )
    {
        selectiveSet( iSamp );
        return;
    }

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::set()" );

    // do we need to create subd scheme?
    if ( iSamp.getSubdivisionScheme() != "catmull-clark" &&
         !m_subdSchemeProperty )
    {
        createSubDSchemeProperty();
    }

    // do we need to create the faceVaryingInterpolateBoundary prop?
    if ( !m_faceVaryingInterpolateBoundaryProperty &&
         iSamp.getFaceVaryingInterpolateBoundary() !=
         ABC_GEOM_SUBD_NULL_INT_VALUE )
    {
        createFaceVaryingInterpolateBoundaryProperty();
    }

    // do we need to create the faceVaryingPropagateCorners prop?
    if ( !m_faceVaryingPropagateCornersProperty &&
         iSamp.getFaceVaryingPropagateCorners() !=
         ABC_GEOM_SUBD_NULL_INT_VALUE )
    {
        createFaceVaryingPropagateCornersProperty();
    }

    // do we need to create the interpolateBoundary prop?
    if ( !m_interpolateBoundaryProperty &&
         iSamp.getInterpolateBoundary() != ABC_GEOM_SUBD_NULL_INT_VALUE )
    {
        createInterpolateBoundaryProperty();
    }

    // do we need to create velocities prop?
    if ( iSamp.getVelocities() && !m_velocitiesProperty )
    {
        createVelocitiesProperty();
    }

    // do we need to create uvs?
    if ( iSamp.getUVs() && !m_uvsParam )
    {
        createUVsProperty( iSamp );
    }

    // We could add sample integrity checking here.
    if ( m_numSamples == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions() &&
                     iSamp.getFaceIndices() &&
                     iSamp.getFaceCounts(),
                     "Sample 0 must have valid data for all mesh components" );

        m_positionsProperty.set( iSamp.getPositions() );
        m_faceIndicesProperty.set( iSamp.getFaceIndices() );
        m_faceCountsProperty.set( iSamp.getFaceCounts() );

        if ( m_velocitiesProperty )
        {
            SetPropUsePrevIfNull( m_velocitiesProperty, iSamp.getVelocities() );
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

        if ( iSamp.getUVs().getVals() )
        {
            m_uvsParam.set( iSamp.getUVs() );
        }

        if ( m_faceVaryingInterpolateBoundaryProperty )
        {
            int32_t value = iSamp.getFaceVaryingInterpolateBoundary();
            if ( value == ABC_GEOM_SUBD_NULL_INT_VALUE )
            {
                value = 0;
            }
            m_faceVaryingInterpolateBoundaryProperty.set( value );
        }

        if ( m_faceVaryingPropagateCornersProperty )
        {
            int32_t value = iSamp.getFaceVaryingPropagateCorners();
            if ( value == ABC_GEOM_SUBD_NULL_INT_VALUE )
            {
                value = 0;
            }
            m_faceVaryingPropagateCornersProperty.set( value );
        }

        if ( m_interpolateBoundaryProperty )
        {
            int32_t value = iSamp.getInterpolateBoundary();
            if ( value == ABC_GEOM_SUBD_NULL_INT_VALUE )
            {
                value = 0;
            }
            m_interpolateBoundaryProperty.set( value );
        }

        if ( m_subdSchemeProperty )
        {
            m_subdSchemeProperty.set( iSamp.getSubdivisionScheme() );
        }

        if ( iSamp.getCreaseIndices() || iSamp.getCreaseLengths() ||
            iSamp.getCreaseSharpnesses() )
        {
            initCreases(0);
        }

        if ( iSamp.getCreaseIndices() )
        {
            m_creaseIndicesProperty.set( iSamp.getCreaseIndices() );
        }

        if ( iSamp.getCreaseLengths() )
        {
            m_creaseLengthsProperty.set( iSamp.getCreaseLengths() );
        }

        if ( iSamp.getCreaseSharpnesses() )
        {
            m_creaseSharpnessesProperty.set( iSamp.getCreaseSharpnesses() );
        }

        if ( iSamp.getCornerIndices() || iSamp.getCornerSharpnesses() )
        {
            initCorners(0);
        }

        if ( iSamp.getCornerIndices() )
        {
            m_cornerIndicesProperty.set( iSamp.getCornerIndices() );
        }

        if ( iSamp.getCornerSharpnesses() )
        {
            m_cornerSharpnessesProperty.set( iSamp.getCornerSharpnesses() );
        }

        if ( iSamp.getHoles() )
        {
            initHoles(0);
        }

        if ( iSamp.getHoles() )
        {
            m_holesProperty.set( iSamp.getHoles() );
        }

    }
    else
    {
        SetPropUsePrevIfNull( m_positionsProperty, iSamp.getPositions() );
        SetPropUsePrevIfNull( m_faceIndicesProperty, iSamp.getFaceIndices() );
        SetPropUsePrevIfNull( m_faceCountsProperty, iSamp.getFaceCounts() );

        if ( m_faceVaryingInterpolateBoundaryProperty )
        {
            SetPropUsePrevIfNull( m_faceVaryingInterpolateBoundaryProperty,
                                  iSamp.getFaceVaryingInterpolateBoundary() );
        }

        if ( m_faceVaryingPropagateCornersProperty )
        {
            SetPropUsePrevIfNull( m_faceVaryingPropagateCornersProperty,
                                  iSamp.getFaceVaryingPropagateCorners() );
        }

        if ( m_interpolateBoundaryProperty )
        {
            SetPropUsePrevIfNull( m_interpolateBoundaryProperty,
                                  iSamp.getInterpolateBoundary() );
        }

        if ( ( iSamp.getCreaseIndices() || iSamp.getCreaseLengths() ||
               iSamp.getCreaseSharpnesses() ) && !m_creaseIndicesProperty )
        {
            initCreases( m_numSamples - 1 );
        }

        if ( m_creaseIndicesProperty )
        {
            SetPropUsePrevIfNull( m_creaseIndicesProperty,
                              iSamp.getCreaseIndices() );
            SetPropUsePrevIfNull( m_creaseLengthsProperty,
                              iSamp.getCreaseLengths() );
            SetPropUsePrevIfNull( m_creaseSharpnessesProperty,
                              iSamp.getCreaseSharpnesses() );
        }

        if ( ( iSamp.getCornerIndices() || iSamp.getCornerSharpnesses() ) &&
             !m_cornerIndicesProperty )
        {
            initCorners( m_numSamples - 1 );
        }

        if ( m_cornerIndicesProperty )
        {
            SetPropUsePrevIfNull( m_cornerIndicesProperty,
                              iSamp.getCornerIndices() );
            SetPropUsePrevIfNull( m_cornerSharpnessesProperty,
                              iSamp.getCornerSharpnesses() );
        }

        if ( iSamp.getHoles() && !m_holesProperty )
        {
            initHoles( m_numSamples - 1);
        }

        if ( m_holesProperty )
        {
            SetPropUsePrevIfNull( m_holesProperty, iSamp.getHoles() );
        }

        if ( m_subdSchemeProperty )
        {
            SetPropUsePrevIfNull( m_subdSchemeProperty,
                                  iSamp.getSubdivisionScheme() );
        }

        if ( m_velocitiesProperty )
        {
            SetPropUsePrevIfNull( m_velocitiesProperty, iSamp.getVelocities() );
        }

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

        if ( m_uvsParam )
        {
            m_uvsParam.set( iSamp.getUVs() );
        }
    }

    m_numSamples++;

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OSubDSchema::createSubDSchemeProperty()
{
    m_subdSchemeProperty = Abc::OStringProperty( this->getPtr(),
        ".scheme", m_timeSamplingIndex );

    std::string scheme = "catmull-clark";
    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_subdSchemeProperty.set( scheme );
    }
}

//-*****************************************************************************
void OSubDSchema::createFaceVaryingInterpolateBoundaryProperty()
{
    m_faceVaryingInterpolateBoundaryProperty = Abc::OInt32Property(
        this->getPtr(), ".faceVaryingInterpolateBoundary",
        m_timeSamplingIndex );

    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_faceVaryingInterpolateBoundaryProperty.set( 0 );
    }
}

//-*****************************************************************************
void OSubDSchema::createFaceVaryingPropagateCornersProperty()
{
    m_faceVaryingPropagateCornersProperty = Abc::OInt32Property(
        this->getPtr(), ".faceVaryingPropagateCorners",
        m_timeSamplingIndex);

    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_faceVaryingPropagateCornersProperty.set( 0 );
    }
}

//-*****************************************************************************
void OSubDSchema::createInterpolateBoundaryProperty()
{
    m_interpolateBoundaryProperty = Abc::OInt32Property( this->getPtr(),
        ".interpolateBoundary", m_timeSamplingIndex );

    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_interpolateBoundaryProperty.set( 0 );
    }
}

//-*****************************************************************************
void OSubDSchema::createVelocitiesProperty()
{
    m_velocitiesProperty = Abc::OV3fArrayProperty( this->getPtr(),
        ".velocities", m_timeSamplingIndex );

    std::vector< V3f > emptyVec;
    const V3fArraySample empty( emptyVec );
    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_velocitiesProperty.set( empty );
    }
}

//-*****************************************************************************
void OSubDSchema::createUVsProperty( const Sample &iSamp )
{
    std::vector<V2f> emptyVals;
    std::vector<Util::uint32_t> emptyIndices;

    OV2fGeomParam::Sample empty;
    AbcA::MetaData mdata;
    SetSourceName( mdata, m_uvSourceName );

    if ( iSamp.getUVs().getIndices() )
    {
        empty = OV2fGeomParam::Sample( Abc::V2fArraySample( emptyVals ),
            Abc::UInt32ArraySample( emptyIndices ),
            iSamp.getUVs().getScope() );

        // UVs are indexed
        m_uvsParam = OV2fGeomParam( this->getPtr(), "uv", true,
                                    empty.getScope(), 1,
                                    this->getTimeSampling(), mdata );
    }
    else
    {
        empty = OV2fGeomParam::Sample( Abc::V2fArraySample( emptyVals ),
                                       iSamp.getUVs().getScope() );

        // UVs are not indexed
        m_uvsParam = OV2fGeomParam( this->getPtr(), "uv", false,
                                    empty.getScope(), 1,
                                    this->getTimeSampling(), mdata );
    }

    // set all the missing samples
    for ( size_t i = 0; i < m_numSamples; ++i )
    {
        m_uvsParam.set( empty );
    }
}

//-*****************************************************************************
void OSubDSchema::createPositionsProperty()
{
    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVertexScope );

    m_positionsProperty = Abc::OP3fArrayProperty( this->getPtr(), "P", mdata,
                                                  m_timeSamplingIndex );

    std::vector<V3f> emptyVec;
    const V3fArraySample empty( emptyVec );
    for ( size_t i = 0 ; i < m_numSamples ; ++i )
    {
        m_positionsProperty.set( empty );
    }

    createSelfBoundsProperty( m_timeSamplingIndex, m_numSamples );
}

//-*****************************************************************************
void OSubDSchema::setFromPrevious()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::setFromPrevious" );

    if( m_positionsProperty )
    {
        m_positionsProperty.setFromPrevious();
    }

    if( m_faceIndicesProperty )
    {
        m_faceIndicesProperty.setFromPrevious();
    }

    if( m_faceCountsProperty )
    {
        m_faceCountsProperty.setFromPrevious();
    }

    if ( m_faceVaryingInterpolateBoundaryProperty )
    {
        m_faceVaryingInterpolateBoundaryProperty.setFromPrevious();
    }

    if ( m_faceVaryingPropagateCornersProperty )
    {
        m_faceVaryingPropagateCornersProperty.setFromPrevious();
    }


    if ( m_interpolateBoundaryProperty )
    {
        m_interpolateBoundaryProperty.setFromPrevious();
    }

    if ( m_creaseIndicesProperty )
    {
        m_creaseIndicesProperty.setFromPrevious();
        m_creaseLengthsProperty.setFromPrevious();
        m_creaseSharpnessesProperty.setFromPrevious();
    }

    if ( m_cornerIndicesProperty )
    {
        m_cornerIndicesProperty.setFromPrevious();
        m_cornerSharpnessesProperty.setFromPrevious();
    }

    if ( m_holesProperty )
    {
        m_holesProperty.setFromPrevious();
    }

    if ( m_subdSchemeProperty )
    {
        m_subdSchemeProperty.setFromPrevious();
    }

    m_selfBoundsProperty.setFromPrevious();

    if ( m_velocitiesProperty ) { m_velocitiesProperty.setFromPrevious(); }

    if ( m_uvsParam ) { m_uvsParam.setFromPrevious(); }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OSubDSchema::setTimeSampling( uint32_t iIndex )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OSubDSchema::setTimeSampling( uint32_t )" );

    m_timeSamplingIndex = iIndex;

    if( m_positionsProperty )
    {
        m_positionsProperty.setTimeSampling( iIndex );
    }

    if( m_faceIndicesProperty )
    {
        m_faceIndicesProperty.setTimeSampling( iIndex );
    }

    if( m_faceCountsProperty )
    {
        m_faceCountsProperty.setTimeSampling( iIndex );
    }

    if( m_selfBoundsProperty )
    {
        m_selfBoundsProperty.setTimeSampling( iIndex );
    }

    if ( m_faceVaryingInterpolateBoundaryProperty )
    {
        m_faceVaryingInterpolateBoundaryProperty.setTimeSampling( iIndex );
    }

    if ( m_faceVaryingPropagateCornersProperty )
    {
        m_faceVaryingPropagateCornersProperty.setTimeSampling( iIndex );
    }

    if ( m_interpolateBoundaryProperty )
    {
        m_interpolateBoundaryProperty.setTimeSampling( iIndex );
    }

    if ( m_subdSchemeProperty )
    {
        m_subdSchemeProperty.setTimeSampling( iIndex );
    }

    if ( m_creaseIndicesProperty )
    {
        m_creaseIndicesProperty.setTimeSampling( iIndex );
    }

    if ( m_creaseLengthsProperty )
    {
        m_creaseLengthsProperty.setTimeSampling( iIndex );
    }

    if ( m_creaseSharpnessesProperty )
    {
        m_creaseSharpnessesProperty.setTimeSampling( iIndex );
    }

    if ( m_cornerIndicesProperty )
    {
        m_cornerIndicesProperty.setTimeSampling( iIndex );
    }

    if ( m_cornerSharpnessesProperty )
    {
        m_cornerSharpnessesProperty.setTimeSampling( iIndex );
    }

    if ( m_holesProperty )
    {
        m_holesProperty.setTimeSampling( iIndex );
    }

    if ( m_velocitiesProperty )
    {
        m_velocitiesProperty.setTimeSampling( iIndex );
    }

    if ( m_uvsParam )
    {
        m_uvsParam.setTimeSampling( iIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OSubDSchema::setTimeSampling( AbcA::TimeSamplingPtr iTime )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OSubDSchema::setTimeSampling( TimeSamplingPtr )" );

    if (iTime)
    {
        uint32_t tsIndex = getObject().getArchive().addTimeSampling(*iTime);
        setTimeSampling( tsIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

namespace {
    OFaceSet g_empty;
}
//-*****************************************************************************
OFaceSet &
OSubDSchema::createFaceSet( const std::string &iFaceSetName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::createFaceSet ()" );

    ABCA_ASSERT( m_faceSets.find (iFaceSetName) == m_faceSets.end (),
                 "faceSet has already been created in SubD." );

    OObject thisObj( getObject() );
    m_faceSets [iFaceSetName] = OFaceSet (thisObj, iFaceSetName);

    return m_faceSets [iFaceSetName];

    ALEMBIC_ABC_SAFE_CALL_END();

    return g_empty;
}

//-*****************************************************************************
void OSubDSchema::init( uint32_t iTsIdx, bool isSparse )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::init()" );

    m_selectiveExport = isSparse;

    m_numSamples = 0;

    m_timeSamplingIndex = iTsIdx;

    if ( m_selectiveExport )
    {
        return;
    }

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    createPositionsProperty();

    m_faceIndicesProperty = Abc::OInt32ArrayProperty( _this, ".faceIndices",
                                                      m_timeSamplingIndex );

    m_faceCountsProperty = Abc::OInt32ArrayProperty( _this, ".faceCounts",
                                                     m_timeSamplingIndex );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void OSubDSchema::initCreases(uint32_t iNumSamples)
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::initCreases()" );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    AbcA::TimeSamplingPtr ts = m_positionsProperty.getTimeSampling();

    m_creaseIndicesProperty = Abc::OInt32ArrayProperty( _this, ".creaseIndices", ts );

    m_creaseLengthsProperty = Abc::OInt32ArrayProperty( _this, ".creaseLengths", ts );

    m_creaseSharpnessesProperty = Abc::OFloatArrayProperty( _this, ".creaseSharpnesses",
        ts );

    std::vector<int32_t> emptyInt;
    std::vector<float32_t> emptyFloat;

    // set the appropriate samples to empty
    for (uint32_t i = 0; i < iNumSamples; ++i)
    {
        m_creaseIndicesProperty.set( Abc::Int32ArraySample( emptyInt ) );
        m_creaseLengthsProperty.set( Abc::Int32ArraySample( emptyInt ) );
        m_creaseSharpnessesProperty.set( Abc::FloatArraySample( emptyFloat ) );
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void OSubDSchema::initCorners(uint32_t iNumSamples)
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::initCorners()" );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    AbcA::TimeSamplingPtr ts = m_positionsProperty.getTimeSampling();

    m_cornerIndicesProperty = Abc::OInt32ArrayProperty( _this, ".cornerIndices", ts );

    m_cornerSharpnessesProperty = Abc::OFloatArrayProperty( _this, ".cornerSharpnesses",
        ts );

    std::vector<int32_t> emptyInt;
    std::vector<float32_t> emptyFloat;

    // set the appropriate samples to empty
    for (uint32_t i = 0; i < iNumSamples; ++i)
    {
        m_cornerIndicesProperty.set( Abc::Int32ArraySample( emptyInt ) );
        m_cornerSharpnessesProperty.set( Abc::FloatArraySample( emptyFloat ) );
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}


//-*****************************************************************************
void OSubDSchema::initHoles(uint32_t iNumSamples)
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::initHoles()" );

    AbcA::TimeSamplingPtr ts = m_positionsProperty.getTimeSampling();

    m_holesProperty = Abc::OInt32ArrayProperty( this->getPtr(), ".holes", ts );

    std::vector<int32_t> emptyInt;

    // set the appropriate samples to empty
    for (uint32_t i = 0; i < iNumSamples; ++i)
    {
        m_holesProperty.set( Abc::Int32ArraySample( emptyInt ) );
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void OSubDSchema::getFaceSetNames (std::vector <std::string> & oFaceSetNames)
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::getFaceSetNames()" );

    for (std::map<std::string, OFaceSet>::const_iterator faceSetIter =
        m_faceSets.begin(); faceSetIter != m_faceSets.end(); ++faceSetIter)
    {
        oFaceSetNames.push_back( faceSetIter->first );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
bool
OSubDSchema::hasFaceSet( const std::string &iFaceSetName )
{
    return (m_faceSets.find (iFaceSetName) != m_faceSets.end ());
}

//-*****************************************************************************
OFaceSet
OSubDSchema::getFaceSet( const std::string &iFaceSetName )
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::getFaceSet()" );

    return m_faceSets [iFaceSetName];

    ALEMBIC_ABC_SAFE_CALL_END();

    OFaceSet empty;
    return empty;
}

//-*****************************************************************************
void OSubDSchema::setUVSourceName(const std::string & iName)
{
    m_uvSourceName = iName;
}

//-*****************************************************************************
void OSubDSchema::selectiveSet( const Sample &iSamp )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::selectiveSet()" );

    // TODO spare create indices and counts?

    if ( iSamp.getPositions() && !m_positionsProperty )
    {
        createPositionsProperty();
    }

    if ( m_positionsProperty )
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

    //! Velocities
    if ( iSamp.getVelocities() && !m_velocitiesProperty )
    {
        createVelocitiesProperty();
    }

    if ( m_velocitiesProperty )
    {
        SetPropUsePrevIfNull( m_velocitiesProperty, iSamp.getVelocities() );
    }

    //! UVs
    if ( iSamp.getUVs().getVals() )
    {
        if( !m_uvsParam )
        {
            createUVsProperty( iSamp );
        }

        // OGeomParam will automatically use SetPropUsePrevIfNull internally
        m_uvsParam.set( iSamp.getUVs() );
    }

    if ( !m_faceVaryingInterpolateBoundaryProperty &&
            iSamp.getFaceVaryingInterpolateBoundary() !=
            ABC_GEOM_SUBD_NULL_INT_VALUE )
    {
       createFaceVaryingInterpolateBoundaryProperty();
    }

    if ( m_faceVaryingInterpolateBoundaryProperty )
    {
        int32_t value = iSamp.getFaceVaryingInterpolateBoundary();
        if ( value == ABC_GEOM_SUBD_NULL_INT_VALUE )
        {
            value = 0;
        }
        m_faceVaryingInterpolateBoundaryProperty.set( value );
    }

    if ( !m_faceVaryingPropagateCornersProperty &&
             iSamp.getFaceVaryingPropagateCorners() !=
             ABC_GEOM_SUBD_NULL_INT_VALUE )
    {
        createFaceVaryingPropagateCornersProperty();
    }

    if ( m_faceVaryingPropagateCornersProperty )
    {
        int32_t value = iSamp.getFaceVaryingPropagateCorners();
        if ( value == ABC_GEOM_SUBD_NULL_INT_VALUE )
        {
            value = 0;
        }
        m_faceVaryingPropagateCornersProperty.set( value );
    }

    if ( !m_interpolateBoundaryProperty &&
             iSamp.getInterpolateBoundary() != ABC_GEOM_SUBD_NULL_INT_VALUE )
    {
        createInterpolateBoundaryProperty();
    }

    if ( m_interpolateBoundaryProperty )
    {
        int32_t value = iSamp.getInterpolateBoundary();
        if ( value == ABC_GEOM_SUBD_NULL_INT_VALUE )
        {
            value = 0;
        }
        m_interpolateBoundaryProperty.set( value );
    }

    if ( iSamp.getSubdivisionScheme() != "catmull-clark" &&
             !m_subdSchemeProperty )
    {
        createSubDSchemeProperty();
    }

    if ( m_subdSchemeProperty )
    {
        m_subdSchemeProperty.set( iSamp.getSubdivisionScheme() );
    }

    if ( ( iSamp.getCreaseIndices() || iSamp.getCreaseLengths() ||
           iSamp.getCreaseSharpnesses() ) &&
        ( !m_creaseIndicesProperty && !m_creaseLengthsProperty &&
          !m_creaseSharpnessesProperty ) )
    {
        initCreases(0);
    }

    if ( iSamp.getCreaseIndices() )
    {
        m_creaseIndicesProperty.set( iSamp.getCreaseIndices() );
    }

    if ( iSamp.getCreaseLengths() )
    {
        m_creaseLengthsProperty.set( iSamp.getCreaseLengths() );
    }

    if ( iSamp.getCreaseSharpnesses() )
    {
        m_creaseSharpnessesProperty.set( iSamp.getCreaseSharpnesses() );
    }

    if ( ( iSamp.getCornerIndices() || iSamp.getCornerSharpnesses() ) &&
         ( !m_cornerIndicesProperty && !m_cornerSharpnessesProperty ) )
    {
        initCorners(0);
    }

    if ( iSamp.getCornerIndices() )
    {
        m_cornerIndicesProperty.set( iSamp.getCornerIndices() );
    }

    if ( iSamp.getCornerSharpnesses() )
    {
        m_cornerSharpnessesProperty.set( iSamp.getCornerSharpnesses() );
    }

    if ( iSamp.getHoles() && !m_holesProperty)
    {
        initHoles(0);
    }

    if ( iSamp.getHoles() )
    {
        m_holesProperty.set( iSamp.getHoles() );
    }

    m_numSamples++;

    ALEMBIC_ABC_SAFE_CALL_END();
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcGeom
} // End namespace Alembic
