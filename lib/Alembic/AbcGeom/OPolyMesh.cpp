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

#include <Alembic/AbcGeom/OPolyMesh.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
OPolyMeshSchema::OPolyMeshSchema( AbcA::CompoundPropertyWriterPtr iParent,
                                  const std::string &iName,
                                  const Abc::Argument &iArg0,
                                  const Abc::Argument &iArg1,
                                  const Abc::Argument &iArg2,
                                  const Abc::Argument &iArg3 )
    : OGeomBaseSchema<PolyMeshSchemaInfo>( iParent, iName,
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
OPolyMeshSchema::OPolyMeshSchema( Abc::OCompoundProperty iParent,
                                  const std::string &iName,
                                  const Abc::Argument &iArg0,
                                  const Abc::Argument &iArg1,
                                  const Abc::Argument &iArg2 )
    : OGeomBaseSchema<PolyMeshSchemaInfo>( iParent.getPtr(), iName,
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
void OPolyMeshSchema::set( const Sample &iSamp )
{
    if( m_selectiveExport || iSamp.isPartialSample() )
    {
        selectiveSet( iSamp );
        return;
    }

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::set()" );

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

    // do we need to create normals?
    if ( iSamp.getNormals() && !m_normalsParam )
    {
        createNormalsProperty( iSamp );
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
        m_indicesProperty.set( iSamp.getFaceIndices() );
        m_countsProperty.set( iSamp.getFaceCounts() );

        if ( m_velocitiesProperty )
        { SetPropUsePrevIfNull( m_velocitiesProperty, iSamp.getVelocities() ); }

        if ( iSamp.getSelfBounds().isEmpty() )
        {
            // OTypedScalarProperty::set() is not referentially transparent,
            // so we need a a placeholder variable.
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() ) );
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
        if ( iSamp.getNormals().getVals() )
        {
            m_normalsParam.set( iSamp.getNormals() );
        }
    }
    else
    {
        SetPropUsePrevIfNull( m_positionsProperty, iSamp.getPositions() );
        SetPropUsePrevIfNull( m_indicesProperty, iSamp.getFaceIndices() );
        SetPropUsePrevIfNull( m_countsProperty, iSamp.getFaceCounts() );

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
                ComputeBoundsFromPositions( iSamp.getPositions() ) );
            m_selfBoundsProperty.set( bnds );
        }
        else
        {
            m_selfBoundsProperty.setFromPrevious();
        }

        // OGeomParam will automatically use SetPropUsePrevIfNull internally
        if ( m_uvsParam ) { m_uvsParam.set( iSamp.getUVs() ); }
        if ( m_normalsParam ) { m_normalsParam.set( iSamp.getNormals() ); }
    }

    m_numSamples++;

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::createPositionsProperty()
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
void OPolyMeshSchema::createVelocitiesProperty()
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
void OPolyMeshSchema::createUVsProperty( const Sample &iSamp )
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
                               m_timeSamplingIndex, mdata );
    }
    else
    {
        empty = OV2fGeomParam::Sample( Abc::V2fArraySample( emptyVals ),
                                       iSamp.getUVs().getScope() );

        // UVs are not indexed
        m_uvsParam = OV2fGeomParam( this->getPtr(), "uv", false,
                               empty.getScope(), 1,
                               m_timeSamplingIndex , mdata );
    }

    // set all the missing samples
    for ( size_t i = 0; i < m_numSamples; ++i )
    {
        m_uvsParam.set( empty );
    }
}

//-*****************************************************************************
void OPolyMeshSchema::createNormalsProperty( const Sample &iSamp )
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
            empty.getScope(), 1, m_timeSamplingIndex );
    }
    else
    {
        empty = ON3fGeomParam::Sample( Abc::V3fArraySample( emptyVals ),
                                       iSamp.getNormals().getScope() );

        // normals are not indexed
        m_normalsParam = ON3fGeomParam( this->getPtr(), "N", false,
                                    empty.getScope(), 1,
                                    m_timeSamplingIndex );
    }

    // set all the missing samples
    for ( size_t i = 0; i < m_numSamples; ++i )
    {
        m_normalsParam.set( empty );
    }
}

//-*****************************************************************************
void OPolyMeshSchema::setFromPrevious()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::setFromPrevious" );

    if( m_positionsProperty ) m_positionsProperty.setFromPrevious();
    if( m_indicesProperty ) m_indicesProperty.setFromPrevious();
    if( m_countsProperty )m_countsProperty.setFromPrevious();

    if( m_selfBoundsProperty ) m_selfBoundsProperty.setFromPrevious();

    if ( m_velocitiesProperty ) { m_velocitiesProperty.setFromPrevious(); }
    if ( m_uvsParam ) { m_uvsParam.setFromPrevious(); }
    if ( m_normalsParam ) { m_normalsParam.setFromPrevious(); }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::setTimeSampling( uint32_t iIndex )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OPolyMeshSchema::setTimeSampling( uint32_t )" );

    m_timeSamplingIndex = iIndex;

    if( m_positionsProperty )
    {
        m_positionsProperty.setTimeSampling( iIndex );
    }

    if( m_indicesProperty )
    {
        m_indicesProperty.setTimeSampling( iIndex );
    }

    if( m_countsProperty )
    {
        m_countsProperty.setTimeSampling( iIndex );
    }

    if( m_selfBoundsProperty )
    {
        m_selfBoundsProperty.setTimeSampling( iIndex );
    }

    if ( m_velocitiesProperty )
    {
        m_velocitiesProperty.setTimeSampling( iIndex );
    }

    if ( m_uvsParam )
    {
        m_uvsParam.setTimeSampling( iIndex );
    }

    if ( m_normalsParam )
    {
        m_normalsParam.setTimeSampling( iIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::setTimeSampling( AbcA::TimeSamplingPtr iTime )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OPolyMeshSchema::setTimeSampling( TimeSamplingPtr )" );

    if ( iTime )
    {
        uint32_t tsIndex = getObject().getArchive().addTimeSampling( *iTime );
        setTimeSampling( tsIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::init( uint32_t iTsIdx, bool isSparse )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::init()" );

    m_selectiveExport = isSparse;

    m_numSamples = 0;

    m_timeSamplingIndex = iTsIdx;

    if ( m_selectiveExport )
    {
        return;
    }

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    createPositionsProperty();

    m_indicesProperty = Abc::OInt32ArrayProperty( _this, ".faceIndices",
                                                  m_timeSamplingIndex );

    m_countsProperty = Abc::OInt32ArrayProperty( _this, ".faceCounts",
                                                 m_timeSamplingIndex );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
bool
OPolyMeshSchema::hasFaceSet( const std::string &iFaceSetName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::hasFaceSet ()" );

    return (m_faceSets.find (iFaceSetName) != m_faceSets.end ());

    ALEMBIC_ABC_SAFE_CALL_END();

    return false;
}

namespace {
    OFaceSet g_emptyFaceSet;
}

//-*****************************************************************************
OFaceSet &
OPolyMeshSchema::createFaceSet( const std::string &iFaceSetName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::createFaceSet ()" );

    ABCA_ASSERT( m_faceSets.find (iFaceSetName) == m_faceSets.end (),
                 "faceSet has already been created in polymesh." );

    OObject thisObj( getObject() );
    m_faceSets[iFaceSetName] = OFaceSet( thisObj, iFaceSetName );

    return m_faceSets[iFaceSetName];

    ALEMBIC_ABC_SAFE_CALL_END();

    return g_emptyFaceSet;
}

//-*****************************************************************************
void OPolyMeshSchema::getFaceSetNames (std::vector <std::string> & oFaceSetNames)
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::getFaceSetNames()" );
    for (std::map<std::string, OFaceSet>::const_iterator faceSetIter =
        m_faceSets.begin(); faceSetIter != m_faceSets.end(); ++faceSetIter)
    {
        oFaceSetNames.push_back( faceSetIter->first );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
OFaceSet
OPolyMeshSchema::getFaceSet( const std::string &iFaceSetName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::getFaceSet()" );

    return m_faceSets[iFaceSetName];

    ALEMBIC_ABC_SAFE_CALL_END();

    OFaceSet empty;
    return empty;
}

//-*****************************************************************************
void OPolyMeshSchema::setUVSourceName(const std::string & iName)
{
    m_uvSourceName = iName;
}

//-*****************************************************************************
void OPolyMeshSchema::selectiveSet( const Sample &iSamp )
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

    //! Normals
    if ( iSamp.getNormals().getVals() )
    {
        if( !m_normalsParam  )
        {
            createNormalsProperty( iSamp );
        }

        // OGeomParam will automatically use SetPropUsePrevIfNull internally
        m_normalsParam.set( iSamp.getNormals() );
    }

    m_numSamples++;

    ALEMBIC_ABC_SAFE_CALL_END();
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcGeom
} // End namespace Alembic
