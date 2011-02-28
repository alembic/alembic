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

        m_childBounds.set( iSamp.getChildBounds(), iSS );

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

        if ( iSamp.getUVs().getVals() )
        {
            if ( iSamp.getUVs().getIndices() )
            {
                // UVs are indexed
                m_uvs = OV2fGeomParam( *this, "uv", true,
                                       iSamp.getUVs().getScope(), 1,
                                       this->getTimeSamplingType() );
            }
            else
            {
                // UVs are not indexed
                m_uvs = OV2fGeomParam( *this, "uv", false,
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

        emptyInt32Array.resize( 0 );
        emptyFloatArray.resize( 0 );

        if ( iSamp.getCreaseIndices() )
        {
            m_creaseIndices.set( iSamp.getCreaseIndices(), iSS );
        }
        else
        {
            m_creaseIndices.set( Abc::Int32ArraySample( emptyInt32Array ), iSS );
        }

        if ( iSamp.getCreaseLengths() )
        {
            m_creaseLengths.set( iSamp.getCreaseLengths(), iSS );
        }
        else
        {
            m_creaseLengths.set( Abc::Int32ArraySample( emptyInt32Array ), iSS );
        }

        if ( iSamp.getCreaseSharpnesses() )
        {
            m_creaseSharpnesses.set( iSamp.getCreaseSharpnesses(), iSS );
        }
        else
        {
            m_creaseSharpnesses.set( Abc::FloatArraySample( emptyFloatArray ),
                                     iSS );
        }

        if ( iSamp.getCornerIndices() )
        {
            m_cornerIndices.set( iSamp.getCornerIndices(), iSS );
        }
        else
        {
            m_cornerIndices.set( Abc::Int32ArraySample( emptyInt32Array ), iSS );
        }

        if ( iSamp.getCornerSharpnesses() )
        {
            m_cornerSharpnesses.set( iSamp.getCornerSharpnesses(), iSS );
        }
        else
        {
            m_cornerSharpnesses.set( Abc::FloatArraySample( emptyFloatArray ),
                                     iSS );
        }

        if ( iSamp.getHoles() )
        {
            m_holes.set( iSamp.getHoles(), iSS );
        }
        else
        {
            m_holes.set( Abc::Int32ArraySample( emptyInt32Array ), iSS );
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

        SetPropUsePrevIfNull( m_creaseIndices,
                              iSamp.getCreaseIndices(), iSS );
        SetPropUsePrevIfNull( m_creaseLengths,
                              iSamp.getCreaseLengths(), iSS );
        SetPropUsePrevIfNull( m_creaseSharpnesses,
                              iSamp.getCreaseSharpnesses(), iSS );

        SetPropUsePrevIfNull( m_cornerIndices,
                              iSamp.getCornerIndices(), iSS );
        SetPropUsePrevIfNull( m_cornerSharpnesses,
                              iSamp.getCornerSharpnesses(), iSS );

        SetPropUsePrevIfNull( m_holes, iSamp.getHoles(), iSS );

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

    m_creaseIndices.setFromPrevious( iSS );
    m_creaseLengths.setFromPrevious( iSS );
    m_creaseSharpnesses.setFromPrevious( iSS );

    m_cornerIndices.setFromPrevious( iSS );
    m_cornerSharpnesses.setFromPrevious( iSS );

    m_holes.setFromPrevious( iSS );

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
    m_positions = Abc::OV3fArrayProperty( *this, "P", mdata, iTst );

    m_faceIndices = Abc::OInt32ArrayProperty( *this, ".faceIndices", iTst );

    m_faceCounts = Abc::OInt32ArrayProperty( *this, ".faceCounts", iTst );

    m_faceVaryingInterpolateBoundary =
        Abc::OInt32Property( *this, ".faceVaryingInterpolateBoundary", iTst );

    m_faceVaryingPropagateCorners =
        Abc::OInt32Property( *this, ".faceVaryingPropagateCorners", iTst );

    m_interpolateBoundary =
        Abc::OInt32Property( *this, ".interpolateBoundary", iTst );

    m_creaseIndices = Abc::OInt32ArrayProperty( *this, ".creaseIndices", iTst );

    m_creaseLengths = Abc::OInt32ArrayProperty( *this, ".creaseLengths", iTst );

    m_creaseSharpnesses = Abc::OFloatArrayProperty( *this, ".creaseSharpnesses",
                                               iTst );

    m_cornerIndices = Abc::OInt32ArrayProperty( *this, ".cornerIndices", iTst );

    m_cornerSharpnesses = Abc::OFloatArrayProperty( *this, ".cornerSharpnesses",
                                               iTst );

    m_holes = Abc::OInt32ArrayProperty( *this, ".holes", iTst );

    m_subdScheme = Abc::OStringProperty( *this, ".scheme", iTst );

    m_selfBounds = Abc::OBox3dProperty( *this, ".selfBnds", iTst );
    m_childBounds = Abc::OBox3dProperty( *this, ".childBnds", iTst );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
