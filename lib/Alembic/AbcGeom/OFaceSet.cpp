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

#include <algorithm>

#include <Alembic/AbcGeom/OFaceSet.h>
#include <Alembic/AbcGeom/GeometryScope.h>
#include <Alembic/AbcGeom/Foundation.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
OFaceSetSchema::OFaceSetSchema( AbcA::CompoundPropertyWriterPtr iParent,
                                const std::string &iName,
                                const Abc::Argument &iArg0,
                                const Abc::Argument &iArg1,
                                const Abc::Argument &iArg2,
                                const Abc::Argument &iArg3 )
      : OGeomBaseSchema<FaceSetSchemaInfo>( iParent, iName,
                                            iArg0, iArg1, iArg2, iArg3 )
{
    init( iParent, iArg0, iArg1, iArg2, iArg3 );
}

//-*****************************************************************************
OFaceSetSchema::OFaceSetSchema( Abc::OCompoundProperty iParent,
                                const std::string &iName,
                                const Abc::Argument &iArg0,
                                const Abc::Argument &iArg1,
                                const Abc::Argument &iArg2 )
      : OGeomBaseSchema<FaceSetSchemaInfo>( iParent.getPtr(), iName,
                                            GetErrorHandlerPolicy( iParent ),
                                            iArg0, iArg1, iArg2 )
{
    init( iParent.getPtr(), iArg0, iArg1, iArg2, Abc::Argument() );
}

//-*****************************************************************************
void OFaceSetSchema::init( AbcA::CompoundPropertyWriterPtr iParent,
                           const Abc::Argument &iArg0,
                           const Abc::Argument &iArg1,
                           const Abc::Argument &iArg2,
                           const Abc::Argument &iArg3 )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OFaceSetSchema::init()" );

    AbcA::TimeSamplingPtr tsPtr =
        Abc::GetTimeSampling( iArg0, iArg1, iArg2, iArg3 );
    uint32_t timeSamplingID =
        Abc::GetTimeSamplingIndex( iArg0, iArg1, iArg2, iArg3 );

    // Add or find the timeSamplingID to use for our properties.
    if (tsPtr)
    {
        timeSamplingID = iParent->getObject()->getArchive()->addTimeSampling(
            *tsPtr);
    }

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    m_facesProperty = Abc::OInt32ArrayProperty( _this, ".faces",
                                                timeSamplingID );

    m_facesExclusive = kFaceSetNonExclusive;

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void OFaceSetSchema::setTimeSampling( uint32_t iTimeSamplingID )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OFaceSetSchema::setTimeSampling( uint32_t iTimeSamplingID )" );

    m_facesProperty.setTimeSampling( iTimeSamplingID );
    m_selfBoundsProperty.setTimeSampling( iTimeSamplingID );

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
    const P3fArraySample & meshP)
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
        ABCA_ASSERT( vertIndexEnd <= numIndices,
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

    // We could add sample integrity checking here.
    if ( m_facesProperty.getNumSamples () == 0 )
    {
        // First sample must provide faces
        ABCA_ASSERT( iSamp.getFaces() ,
                     "Sample 0 must provide the faces that make up the faceset." );
        m_facesProperty.set( iSamp.getFaces() );
    }
    else
    {
        SetPropUsePrevIfNull( m_facesProperty, iSamp.getFaces() );
    }

    // We've now set the sample for the m_faces property.
    if ( iSamp.getSelfBounds().hasVolume() )
    {
        // Caller explicity set bounds for this sample of the faceset.
        m_selfBoundsProperty.set( iSamp.getSelfBounds() );
    }
    else
    {
        m_selfBoundsProperty.set( iSamp.getSelfBounds() );
        // NYI compute self bounds via parent mesh's faces
    }

    if (m_facesExclusive != kFaceSetNonExclusive)
    {
        // The user has changed the exclusivity hint from the
        // default so we'll create a property now and store.
        _recordExclusivityHint();
    }
    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OFaceSetSchema::setFaceExclusivity( FaceSetExclusivity iFacesExclusive )
{
    if (m_facesExclusive != iFacesExclusive)
    {
        // The user has changed the exclusivity hint.
        m_facesExclusive = iFacesExclusive;
        _recordExclusivityHint();
    }
}


//-*****************************************************************************
void OFaceSetSchema::_recordExclusivityHint()
{
    // Instead of always just setting the exclusive hint property
    // we have a bit more logic here so that we only create
    // the property and set when the client code needs to
    // record a non-default value for faceset's.
    if (!m_facesExclusiveProperty)
    {
        m_facesExclusiveProperty = Abc::OUInt32Property( this->getPtr(),
            ".facesExclusive", m_facesProperty.getTimeSampling() );
    }
    m_facesExclusiveProperty.set (m_facesExclusive);
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcGeom
} // End namespace Alembic
