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

#include <Alembic/AbcGeom/OFaceSet.h>
#include <Alembic/AbcGeom/GeometryScope.h>
#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/OSubD.h>

namespace Alembic {
namespace AbcGeom {


//-*****************************************************************************
void OFaceSetSchema::init( uint32_t iTimeSamplingID )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OFaceSetSchema::init()" );

    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVertexScope );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    m_facesProperty = Abc::OInt32ArrayProperty( _this, ".faces",
        iTimeSamplingID );
    // NYI - replace this with library utility funtions that generically
    // store visibility.
    m_visibilityProperty = Abc::OBoolProperty( _this, ".visibility",
        iTimeSamplingID );

    m_selfBoundsProperty = Abc::OBox3dProperty( _this, ".selfBnds",
        iTimeSamplingID );

    m_facesExclusiveProperty = Abc::OBoolProperty( _this, ".isExclusive",
                                                   iTimeSamplingID );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void OFaceSetSchema::setTimeSampling( uint32_t iTimeSamplingID )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OFaceSetSchema::setTimeSampling( uint32_t iTimeSamplingID )" );

    m_facesProperty.setTimeSampling( iTimeSamplingID );
    m_visibilityProperty.setTimeSampling( iTimeSamplingID );
    m_selfBoundsProperty.setTimeSampling( iTimeSamplingID );
    m_childBoundsProperty.setTimeSampling( iTimeSamplingID );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OFaceSetSchema::setTimeSampling( AbcA::TimeSamplingPtr iTimeSampling )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OFaceSetSchema::setTimeSampling( AbcA::TimeSamplingPtr iTimeSampling )" );

    if (iTimeSampling)
    {
        uint32_t iTimeSamplingID;
        iTimeSamplingID = getObject().getArchive().addTimeSampling(
            *iTimeSampling );
        setTimeSampling( iTimeSamplingID );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
Abc::Box3d computeBoundsFromPositionsByFaces (const Int32ArraySample & faces,
    const Int32ArraySample & meshFaceCounts,
    const Int32ArraySample & vertexIndices,
    const V3fArraySample & meshP)
{
    Abc::Box3d     bounds;
    size_t numFaceSetFaces = faces.size ();

    size_t numFaces = meshFaceCounts.size ();
    size_t numIndices = vertexIndices.size ();
    size_t numPoints = meshP.size ();
    if ( numFaces < 1 ||
         numIndices < 1 ||
         numPoints < 1 ||
         numFaceSetFaces < 1 )
    {
        return bounds;
    }
    // Create ordered list of face numbers in faceset because
    // the list of face numbers in the faceset might be in any
    // order, so we build an ordered vec.
    std::vector <int32_t> faceSetFaceNums (faces.get (),
        faces.get() + numFaceSetFaces);
    std::sort (faceSetFaceNums.begin (), faceSetFaceNums.end ());
    std::vector <int32_t>::iterator curFaceSetFaceIter = faceSetFaceNums.begin ();
    std::vector <int32_t>::iterator faceSetFaceIterEnd = faceSetFaceNums.end ();

    // Run through faces of the polymesh. If the face is in
    // our faceset we get the face's count of vertex indices
    // and extend our bounds by those verts.
    // We stop our iteration once we've reached all faces in
    // our faceset.
    size_t curFaceSetFaceNum = *curFaceSetFaceIter;
    size_t faceIndex;
    size_t vertIndex;
    size_t vertexNum;
    size_t vertIndexBegin = 0;
    size_t vertIndexEnd = 0;
    V3f vertex;
    for ( faceIndex = 0; faceIndex < numFaces &&
        curFaceSetFaceIter != faceSetFaceIterEnd; faceIndex++)
    {
        vertIndexBegin = vertIndexEnd;
        vertIndexEnd = vertIndexBegin + meshFaceCounts[faceIndex];
        ABCA_ASSERT( vertIndexEnd < numIndices,
                     "Face in mesh has count of vertices that is greater "
                     "than total number of vertex defined in mesh.");

        if (faceIndex == curFaceSetFaceNum)
        {
            // This face is in our faceset
            for (vertIndex = vertIndexBegin; vertIndex < vertIndexEnd;
                vertIndex++)
            {
                vertexNum = vertexIndices[vertIndex];
                vertex = meshP[vertexNum];
                bounds.extendBy (vertex);
            }
            curFaceSetFaceIter++;
            if (curFaceSetFaceIter != faceSetFaceIterEnd)
            {
                // There are more faces in this faceset, so
                // get the next one.
                curFaceSetFaceNum = *curFaceSetFaceIter;
            }
        }
    }
    return bounds;
}

//-*****************************************************************************
void OFaceSetSchema::set( const Sample &iSamp )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OFaceSetSchema::set()" );

    Abc::Box3d emptyBox;
    emptyBox.makeEmpty();
    // do we need to create child bounds?
    if ( iSamp.getChildBounds().hasVolume() && !m_childBoundsProperty)
    {
        m_childBoundsProperty = Abc::OBox3dProperty( this->getPtr(),
            ".childBnds", m_facesProperty.getTimeSampling() );

        // -1 because we just dis an m_positions set above
        size_t numSamples = m_facesProperty.getNumSamples() - 1;

        // set all the missing samples
        for ( size_t i = 0; i < numSamples; ++i )
        {
            m_childBoundsProperty.set( emptyBox );
        }
    }

    // We could add sample integrity checking here.
    if ( m_facesProperty.getNumSamples () == 0 )
    {
        // First sample must provide faces
        ABCA_ASSERT( iSamp.getFaces() ,
                     "Sample 0 must provide the faces that make up the faceset." );
        m_facesProperty.set( iSamp.getFaces() );
        m_visibilityProperty.set( iSamp.isVisible() );

        if (m_childBoundsProperty)
        {
            m_childBoundsProperty.set( iSamp.getChildBounds() );
        }
    }
    else
    {
        SetPropUsePrevIfNull( m_facesProperty, iSamp.getFaces() );

        if ( m_childBoundsProperty )
        {
            SetPropUsePrevIfNull( m_childBoundsProperty,
                iSamp.getChildBounds() );
        }
    }

    // We've now set the sample for the m_faces property.
    if ( iSamp.getSelfBounds().hasVolume() )
    {
        // Caller explicity set bounds for this sample of the faceset.
        m_selfBoundsProperty.set( iSamp.getSelfBounds() );
    }
    else
    {
        /* NYI
        // The sample doesn't specify a self bounds, so attempt to
        // examine the faces of the parent mesh/subd
        // and compute bouding volume.
        Abc::Box3d faceSetBounds;
        faceSetBounds.makeEmpty( );
        size_t this_sample_num = m_facesProperty.getNumSamples ();

        OObject parentObject = this->getParent( ).getObject( ).getParent( );
        Alembic::AbcGeom::OSubD parentMeshObject( parentObject, Alembic::Abc::kWrapExisting );
        OSubDSchema &parentMesh = parentMeshObject.getSchema();
        AbcA::TimeSamplingPtr tsPtr = parentMesh.m_faceCounts.getTimeSampling();
        // check that our faceSet is using the same time sampling
        if ((tsPtr == m_selfBoundsProperty.getTimeSampling()) &&
            (parentMesh.m_faceCounts.getNumSamples () >=
            this_sample_num))
        {
            OSubDSchema::Sample meshSample;
            parentMesh.get( meshSample, this_sample_num );
            faceSetBounds = computeBoundsFromPositionsByFaces (iSamp.getFaces(),
                meshSample.getFaceCounts (), meshSample.getFaceIndices (),
                meshSample.getPositions ());
            m_selfBoundsProperty.set( faceSetBounds );
        }
        else
        {
            m_selfBoundsProperty.set( emptyBox );
        }
        */
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
