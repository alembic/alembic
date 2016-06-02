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
void OPolyMeshSchema::set( const Sample &iSamp )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::set()" );

    size_t previousSampleCount = getNumSamples();

    if ( iSamp.getVelocities() )
    {
       // do we need to create velocities prop?
       if(!m_velocitiesProperty)
       {
           m_velocitiesProperty = Abc::OV3fArrayProperty( this->getPtr(),
               ".velocities", m_positionsProperty.getTimeSampling() );

           std::vector<V3f> emptyVec;
           const V3fArraySample empty( emptyVec );
           for ( size_t i = 0 ; i < previousSampleCount ; ++i )
           {
               m_velocitiesProperty.set( empty );
           }
       }

       SetPropUsePrevIfNull( m_velocitiesProperty, iSamp.getVelocities() );
    }
    else if(m_velocitiesProperty)
    {
       SetPropUsePrevIfNull( m_velocitiesProperty, iSamp.getVelocities() );
    }


    if( iSamp.getUVs() )
    {
       // do we need to create uvs?
       if ( !m_uvsParam )
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
                                      m_tsIndex, mdata );
           }
           else
           {
               empty = OV2fGeomParam::Sample( Abc::V2fArraySample( emptyVals ),
                                              iSamp.getUVs().getScope() );

               // UVs are not indexed
               m_uvsParam = OV2fGeomParam( this->getPtr(), "uv", false,
                                      empty.getScope(), 1,
                                      m_tsIndex , mdata );
           }

           // set all the missing samples
           for ( size_t i = 0; i < previousSampleCount; ++i )
           {
               m_uvsParam.set( empty );
           }
       }

       m_uvsParam.set( iSamp.getUVs() );
    }
    else if( m_uvsParam )
    {
       // OGeomParam will automatically use SetPropUsePrevIfNull internally
       m_uvsParam.set( iSamp.getUVs() );
    }

    if(iSamp.getNormals())
    {
       // do we need to create normals?
       if (!m_normalsParam )
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
                   empty.getScope(), 1, m_tsIndex );
           }
           else
           {
               empty = ON3fGeomParam::Sample( Abc::V3fArraySample( emptyVals ),
                                              iSamp.getNormals().getScope() );

               // normals are not indexed
               m_normalsParam = ON3fGeomParam( this->getPtr(), "N", false,
                                           empty.getScope(), 1,
                                           m_tsIndex );
           }

           // set all the missing samples
           for ( size_t i = 0; i < previousSampleCount; ++i )
           {
               m_normalsParam.set( empty );
           }

           m_normalsParam.set( iSamp.getNormals() );
       }
    }
    else if(m_normalsParam)
    {
       // OGeomParam will automatically use SetPropUsePrevIfNull internally
       m_normalsParam.set( iSamp.getNormals() );
    }

    if(iSamp.getPositions() && iSamp.getPositions().size())
    {
       if( !m_selfBoundsProperty )
       {
           createSelfBoundsProperty(m_tsIndex);

           Abc::Box3d emptyBounds;

           for ( size_t i = 0; i < previousSampleCount; ++i )
           {
               m_selfBoundsProperty.set( emptyBounds );
           }
       }

       if(!m_positionsProperty)
       {
           AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

           m_positionsProperty = Abc::OP3fArrayProperty( _this, "P", m_metaData, m_tsIndex );

           m_indicesProperty = Abc::OInt32ArrayProperty( _this, ".faceIndices", m_tsIndex );

           m_countsProperty = Abc::OInt32ArrayProperty( _this, ".faceCounts", m_tsIndex );

           // set all the missing samples
           const OPolyMeshSchema::Sample emptySample;

           for ( size_t i = 0; i < previousSampleCount; ++i )
           {
               setPositionSample(emptySample);
           }
       }

       setPositionSample(iSamp);
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::setPositionSample(const OPolyMeshSchema::Sample &iSamp)
{
   // We could add sample integrity checking here.
   if ( m_positionsProperty.getNumSamples() == 0 )
   {
       // First sample must be valid on all points.
       ABCA_ASSERT( ( m_positionsProperty.getNumSamples() == 0 ) &&
                    iSamp.getPositions() &&
                    iSamp.getFaceIndices() &&
                    iSamp.getFaceCounts(),
                    "Sample 0 must have valid data for all mesh components" );

       m_positionsProperty.set( iSamp.getPositions() );
       m_indicesProperty.set( iSamp.getFaceIndices() );
       m_countsProperty.set( iSamp.getFaceCounts() );

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
       SetPropUsePrevIfNull( m_indicesProperty, iSamp.getFaceIndices() );
       SetPropUsePrevIfNull( m_countsProperty, iSamp.getFaceCounts() );

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
}

//-*****************************************************************************
void OPolyMeshSchema::setFromPrevious()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::setFromPrevious" );

    if( m_positionsProperty )
    {
       m_positionsProperty.setFromPrevious();
    }

    if(m_indicesProperty)
    {
       m_indicesProperty.setFromPrevious();
    }

    if(m_countsProperty)
    {
       m_countsProperty.setFromPrevious();
    }

    m_selfBoundsProperty.setFromPrevious();

    if ( m_velocitiesProperty )
    {
       m_velocitiesProperty.setFromPrevious();
    }

    if ( m_uvsParam )
    {
       m_uvsParam.setFromPrevious();
    }

    if ( m_normalsParam )
    {
       m_normalsParam.setFromPrevious();
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPolyMeshSchema::setTimeSampling( uint32_t iIndex )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OPolyMeshSchema::setTimeSampling( uint32_t )" );

    m_tsIndex = iIndex;

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
void OPolyMeshSchema::init( uint32_t iTsIdx )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::init()" );

    SetGeometryScope( m_metaData, kVertexScope );

    m_tsIndex = iTsIdx;

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


//-*****************************************************************************
OFaceSet &
OPolyMeshSchema::createFaceSet( const std::string &iFaceSetName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPolyMeshSchema::createFaceSet ()" );

    ABCA_ASSERT( m_faceSets.find (iFaceSetName) == m_faceSets.end (),
                 "faceSet has already been created in polymesh." );

    m_faceSets[iFaceSetName] = OFaceSet( getObject(), iFaceSetName );

    return m_faceSets[iFaceSetName];

    ALEMBIC_ABC_SAFE_CALL_END();

    static OFaceSet emptyFaceSet;
    return emptyFaceSet;
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
size_t OPolyMeshSchema::getNumSamples() const
{
   // We allow for any of these properties to be used individually,
   // so look for a valid one to find the number of samples

   if(m_positionsProperty)
   {
       return m_positionsProperty.getNumSamples();
   }

   if(m_velocitiesProperty)
   {
       return m_velocitiesProperty.getNumSamples();
   }

   if(m_uvsParam)
   {
       return m_uvsParam.getNumSamples();
   }

   if(m_normalsParam)
   {
       return m_normalsParam.getNumSamples();
   }

   return 0;
}

//-*****************************************************************************
AbcA::TimeSamplingPtr OPolyMeshSchema::getTimeSampling() const
{
   // We allow for any of these properties to be used individually,
   // so look for a valid one to get the time sampling

   if(m_positionsProperty)
   {
       return m_positionsProperty.getTimeSampling();
   }

   if(m_velocitiesProperty)
   {
       return m_velocitiesProperty.getTimeSampling();
   }

   if(m_uvsParam)
   {
       return m_uvsParam.getTimeSampling();
   }

   if(m_normalsParam)
   {
       return m_normalsParam.getTimeSampling();
   }

   ABCA_ASSERT(0, "PolyMesh time sampling is invalid because we haven't set any data to be sampled");
   return AbcA::TimeSamplingPtr();
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcGeom
} // End namespace Alembic
