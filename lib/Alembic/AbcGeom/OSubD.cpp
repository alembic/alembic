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

#include <Alembic/AbcGeom/OSubD.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
inline void SetPropUsePrevIfNull( Abc::OInt32Property iProp, int32_t iVal,
                                  const Abc::OSampleSelector &iSS )
{
    if ( iVal != ABC_GEOM_SUBD_NULL_INT_VALUE )
    {
        iProp.set( iVal, iSS );
    }
    else
    {
        iProp.setFromPrevious( iSS );
    }
}

//-*****************************************************************************
void OSubDSchema::set( const Sample &iSamp,
                       const Abc::OSampleSelector &iSS  )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::set()" );

    // We could add sample integrity checking here.
    if ( iSS.getIndex() == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions() &&
                     iSamp.getFaceIndices() &&
                     iSamp.getFaceCounts(),
                     "Sample 0 must have valid data for all mesh components" );

        m_positions.set( iSamp.getPositions(), iSS );
        m_faceIndices.set( iSamp.getFaceIndices(), iSS );
        m_faceCounts.set( iSamp.getFaceCounts(), iSS );

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
        else
        { m_selfBounds.set( iSamp.getSelfBounds(), iSS ); }

        if ( iSamp.getUVs().getVals() )
        {
            if ( iSamp.getUVs().getIndices() )
            {
                // UVs are indexed
                m_uvs = OV2fGeomParam( this->getPtr(), "uv", true,
                                       iSamp.getUVs().getScope(), 1,
                                       this->getTimeSamplingType() );
            }
            else
            {
                // UVs are not indexed
                m_uvs = OV2fGeomParam( this->getPtr(), "uv", false,
                                       iSamp.getUVs().getScope(), 1,
                                       this->getTimeSamplingType() );
            }

            m_uvs.set( iSamp.getUVs(), iSS );
        }

        if ( iSamp.getFaceVaryingInterpolateBoundary() ==
             ABC_GEOM_SUBD_NULL_INT_VALUE )
        {
            m_faceVaryingInterpolateBoundary.set( 0, iSS );
        }
        else
        {
            m_faceVaryingInterpolateBoundary.set(
                iSamp.getFaceVaryingInterpolateBoundary(), iSS );
        }
        if ( iSamp.getFaceVaryingPropagateCorners() ==
             ABC_GEOM_SUBD_NULL_INT_VALUE )
        {
            m_faceVaryingPropagateCorners.set( 0, iSS );
        }
        else
        {
            m_faceVaryingPropagateCorners.set(
                iSamp.getFaceVaryingPropagateCorners(), iSS );
        }
        if ( iSamp.getInterpolateBoundary() ==
             ABC_GEOM_SUBD_NULL_INT_VALUE )
        {
            m_interpolateBoundary.set( 0, iSS );
        }
        else
        {
            m_interpolateBoundary.set( iSamp.getInterpolateBoundary(), iSS );
        }

        m_subdScheme.set( iSamp.getSubdivisionScheme(), iSS );

        std::vector<int32_t> emptyInt32Array;
        std::vector<float32_t> emptyFloatArray;

        if ( iSamp.getCreaseIndices() || iSamp.getCreaseLengths() ||
            iSamp.getCreaseSharpnesses() )
        {
            initCreases();
        }
        else
        {
            m_nullCreaseSamples.push_back( iSS );
        }

        if ( iSamp.getCreaseIndices() )
        {
            m_creaseIndices.set( iSamp.getCreaseIndices(), iSS );
        }

        if ( iSamp.getCreaseLengths() )
        {
            m_creaseLengths.set( iSamp.getCreaseLengths(), iSS );
        }

        if ( iSamp.getCreaseSharpnesses() )
        {
            m_creaseSharpnesses.set( iSamp.getCreaseSharpnesses(), iSS );
        }

        if ( iSamp.getCornerIndices() || iSamp.getCornerSharpnesses() )
        {
            initCorners();
        }
        else
        {
            m_nullCornerSamples.push_back( iSS );
        }

        if ( iSamp.getCornerIndices() )
        {
            m_cornerIndices.set( iSamp.getCornerIndices(), iSS );
        }

        if ( iSamp.getCornerSharpnesses() )
        {
            m_cornerSharpnesses.set( iSamp.getCornerSharpnesses(), iSS );
        }

        if ( iSamp.getHoles() )
        {
            initHoles();
        }
        else
        {
            m_nullHoleSamples.push_back( iSS );
        }

        if ( iSamp.getHoles() )
        {
            m_holes.set( iSamp.getHoles(), iSS );
        }

    }
    else
    {
        SetPropUsePrevIfNull( m_positions, iSamp.getPositions(), iSS );
        SetPropUsePrevIfNull( m_faceIndices, iSamp.getFaceIndices(), iSS );
        SetPropUsePrevIfNull( m_faceCounts, iSamp.getFaceCounts(), iSS );

        SetPropUsePrevIfNull( m_faceVaryingInterpolateBoundary,
                              iSamp.getFaceVaryingInterpolateBoundary(), iSS );
        SetPropUsePrevIfNull( m_faceVaryingPropagateCorners,
                              iSamp.getFaceVaryingPropagateCorners(), iSS );
        SetPropUsePrevIfNull( m_interpolateBoundary,
                              iSamp.getInterpolateBoundary(), iSS );

        if ( ( iSamp.getCreaseIndices() || iSamp.getCreaseLengths() ||
            iSamp.getCreaseSharpnesses() ) && !m_creaseIndices )
        {
            initCreases();
        }

        if ( m_creaseIndices )
        {
            SetPropUsePrevIfNull( m_creaseIndices,
                              iSamp.getCreaseIndices(), iSS );
            SetPropUsePrevIfNull( m_creaseLengths,
                              iSamp.getCreaseLengths(), iSS );
            SetPropUsePrevIfNull( m_creaseSharpnesses,
                              iSamp.getCreaseSharpnesses(), iSS );
        }
        else
        {
            m_nullCreaseSamples.push_back( iSS );
        }

        if ( ( iSamp.getCornerIndices() || iSamp.getCornerSharpnesses() ) &&
            !m_cornerIndices )
        {
            initCorners();
        }

        if ( m_cornerIndices )
        {
            SetPropUsePrevIfNull( m_cornerIndices,
                              iSamp.getCornerIndices(), iSS );
            SetPropUsePrevIfNull( m_cornerSharpnesses,
                              iSamp.getCornerSharpnesses(), iSS );
        }
        else
        {
            m_nullCornerSamples.push_back( iSS );
        }

        if ( iSamp.getHoles() && !m_holes )
        {
            initHoles();
        }

        if ( m_holes )
        {
            SetPropUsePrevIfNull( m_holes, iSamp.getHoles(), iSS );
        }
        else
        {
            m_nullHoleSamples.push_back( iSS );
        }

        SetPropUsePrevIfNull( m_subdScheme, iSamp.getSubdivisionScheme(),
                              iSS );


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

        if ( m_uvs ) { m_uvs.set( iSamp.getUVs(), iSS ); }
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OSubDSchema::setFromPrevious( const Abc::OSampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::setFromPrevious" );

    m_positions.setFromPrevious( iSS );
    m_faceIndices.setFromPrevious( iSS );
    m_faceCounts.setFromPrevious( iSS );

    m_faceVaryingInterpolateBoundary.setFromPrevious( iSS );
    m_faceVaryingPropagateCorners.setFromPrevious( iSS );
    m_interpolateBoundary.setFromPrevious( iSS );

    if ( m_creaseIndices )
    {
        m_creaseIndices.setFromPrevious( iSS );
        m_creaseLengths.setFromPrevious( iSS );
        m_creaseSharpnesses.setFromPrevious( iSS );
    }
    else
    {
        m_nullCreaseSamples.push_back( iSS );
    }

    if ( m_cornerIndices )
    {
        m_cornerIndices.setFromPrevious( iSS );
        m_cornerSharpnesses.setFromPrevious( iSS );
    }
    else
    {
        m_nullCornerSamples.push_back( iSS );
    }

    if ( m_holes )
    {
        m_holes.setFromPrevious( iSS );
    }
    else
    {
        m_nullHoleSamples.push_back( iSS );
    }

    m_subdScheme.setFromPrevious( iSS );

    m_selfBounds.setFromPrevious( iSS );
    m_childBounds.setFromPrevious( iSS );

    if ( m_uvs ) { m_uvs.setFromPrevious( iSS ); }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
Abc::OCompoundProperty OSubDSchema::getArbGeomParams()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::getArbGeomParams()" );

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
void OSubDSchema::init( const AbcA::TimeSamplingType &iTst )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::init()" );

    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVertexScope );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    m_positions = Abc::OV3fArrayProperty( _this, "P", mdata, iTst );

    m_faceIndices = Abc::OInt32ArrayProperty( _this, ".faceIndices", iTst );

    m_faceCounts = Abc::OInt32ArrayProperty( _this, ".faceCounts", iTst );

    m_faceVaryingInterpolateBoundary =
        Abc::OInt32Property( _this, ".faceVaryingInterpolateBoundary", iTst );

    m_faceVaryingPropagateCorners =
        Abc::OInt32Property( _this, ".faceVaryingPropagateCorners", iTst );

    m_interpolateBoundary =
        Abc::OInt32Property( _this, ".interpolateBoundary", iTst );

    m_subdScheme = Abc::OStringProperty( _this, ".scheme", iTst );

    m_selfBounds = Abc::OBox3dProperty( _this, ".selfBnds", iTst );
    m_childBounds = Abc::OBox3dProperty( _this, ".childBnds", iTst );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void OSubDSchema::initCreases()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::initCreases()" );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    AbcA::TimeSamplingType tst = m_positions.getTimeSamplingType();

    m_creaseIndices = Abc::OInt32ArrayProperty( _this, ".creaseIndices", tst );

    m_creaseLengths = Abc::OInt32ArrayProperty( _this, ".creaseLengths", tst );

    m_creaseSharpnesses = Abc::OFloatArrayProperty( _this, ".creaseSharpnesses",
        tst );

    std::vector<int32_t> emptyInt;
    std::vector<float32_t> emptyFloat;

    // set the appropriate samples to empty
    std::vector < Abc::OSampleSelector >::iterator it =
        m_nullCreaseSamples.begin();
    std::vector < Abc::OSampleSelector >::iterator itEnd =
        m_nullCreaseSamples.end();

    for (; it != itEnd; ++it)
    {
        m_creaseIndices.set( Abc::Int32ArraySample( emptyInt ), *it );
        m_creaseLengths.set( Abc::Int32ArraySample( emptyInt ), *it );
        m_creaseSharpnesses.set( Abc::FloatArraySample( emptyFloat ), *it );
    }

    // we no longer need these for creating empty samples
    m_nullCreaseSamples.clear();

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void OSubDSchema::initCorners()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::initCorners()" );

    AbcA::TimeSamplingType tst = m_positions.getTimeSamplingType();

    m_cornerIndices = Abc::OInt32ArrayProperty( *this, ".cornerIndices", tst );

    m_cornerSharpnesses = Abc::OFloatArrayProperty( *this, ".cornerSharpnesses",
        tst );

    std::vector<int32_t> emptyInt;
    std::vector<float32_t> emptyFloat;

    // set the appropriate samples to empty
    std::vector < Abc::OSampleSelector >::iterator it =
        m_nullCornerSamples.begin();
    std::vector < Abc::OSampleSelector >::iterator itEnd =
        m_nullCornerSamples.end();

    for (; it != itEnd; ++it)
    {
        m_cornerIndices.set( Abc::Int32ArraySample( emptyInt ), *it );
        m_cornerSharpnesses.set( Abc::FloatArraySample( emptyFloat ), *it );
    }

    // we no longer need these for creating empty samples
    m_nullCornerSamples.clear();

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}


//-*****************************************************************************
void OSubDSchema::initHoles()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSubDSchema::initHoles()" );

    AbcA::TimeSamplingType tst = m_positions.getTimeSamplingType();

    m_holes = Abc::OInt32ArrayProperty( *this, ".holes", tst );

    std::vector<int32_t> emptyInt;

    // set the appropriate samples to empty
    std::vector < Abc::OSampleSelector >::iterator it =
        m_nullHoleSamples.begin();
    std::vector < Abc::OSampleSelector >::iterator itEnd =
        m_nullHoleSamples.end();

    for (; it != itEnd; ++it)
    {
        m_holes.set( Abc::Int32ArraySample( emptyInt ), *it );
    }

    // we no longer need these for creating empty samples
    m_nullHoleSamples.clear();

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
