//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks Inc. and
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
// Industrial Light & Magic, nor the names of their contributors may be used
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

#include <Alembic/TakoAbc/SubDWriter.h>

namespace Alembic {
namespace TakoAbc {

//-*****************************************************************************
//-*****************************************************************************
// TESTS
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
static void verifyGeometryLists( index_t iNumValues,
                                 ConstIndexVectorPtr iFacePoints,
                                 ConstIndexVectorPtr iFaceList )
{
    // make sure there are enough values for at least 1 polygon and
    // make sure the vectors of points are correctly aligned
    if ( iNumValues < 9 || iNumValues % 3 != 0 )
    {
        ABCA_THROW( std::logic_error, "Bad point count" );
    }
    
    if ( iFacePoints->size() < 3 )
    {
        ABCA_THROW( std::logic_error, "Not enough face points!" );
    }
    
    std::vector<index_t>::const_iterator i, end;
    for ( i = iFacePoints->begin(), end = iFacePoints->end(); i != end; ++i )
    {
        if ( *i > iNumValues/3 )
        {
            ABCA_THROW( std::logic_error, "Bad face point!" );
        }
    }
    
    if ( iFaceList->empty() )
    {
        ABCA_THROW( std::logic_error, "Bad face index!" );
    }
    
    i = iFaceList->begin();
    index_t lastIndex = *i;
    index_t maxIndex = iFacePoints->size();
    i++;
    for ( end = iFaceList->end(); i != end; ++i )
    {
        index_t curIndex = *i;
        if ( curIndex > maxIndex || lastIndex + 2 >= curIndex )
        {
            ABCA_THROW( std::logic_error, "Bad face index!" );
        }
        
        lastIndex = curIndex;
    }
}

//-*****************************************************************************
static void verifyCreases( index_t iNumGeoPoints,
                           index_t iNumValues,
                           ConstIndexVectorPtr iIndices,
                           ConstIndexVectorPtr iLengths )
{
    if ( iNumValues != iLengths->size() )
    {
        ABCA_THROW( std::logic_error, 
                    "Size of crease lengths and sharpness do not match!" );
    }

    std::vector<index_t>::const_iterator i, end;
    for ( i = iIndices->begin(), end = iIndices->end(); i != end; ++i )
    {
        if ( *i > iNumGeoPoints )
        {
            ABCA_THROW( std::logic_error, "Bad crease index!" );
        }
    }

    index_t total = 0;
    for ( i = iLengths->begin(), end = iLengths->end(); i != end; ++i )
    {
        // need 2 points to form an edge
        if ( *i < 2 )
        {
            ABCA_THROW( std::logic_error, "Bad crease length!" );
        }

        total += *i;
    }

    if ( total != iIndices->size() )
    {
        ABCA_THROW( std::logic_error, 
                    "Total crease lengths don't cover all of the "
                    "crease indeces." );
    }
}

//-*****************************************************************************
static void verifyCorners( index_t iNumGeoPoints,
                           index_t iNumValues,
                           ConstIndexVectorPtr iIndices )
{
    if ( iNumValues != iIndices->size() )
    {
        ABCA_THROW( std::logic_error, 
                    "Size of corner indices and sharpness do not match!" );
    }

    std::vector<index_t>::const_iterator i, end;
    for ( i = iIndices->begin(), end = iIndices->end(); i != end; ++i )
    {
        if ( *i > iNumGeoPoints )
        {
            ABCA_THROW( std::logic_error, "Bad corner index!" );
        }
    }
}

//-*****************************************************************************
static void verifyHolePolyIndices( index_t iNumFacePoints,
                                   ConstIndexVectorPtr iIndices )
{
    std::vector<index_t>::const_iterator i, end;
    for ( i = iIndices->begin(), end = iIndices->end(); i != end; ++i )
    {
        if ( *i > iNumFacePoints )
        {
            ABCA_THROW( std::logic_error, "Bad hole poly index!" );
        }
    }
}


//-*****************************************************************************
//-*****************************************************************************
// SUBD WRITER
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// Need to squirrel away the current frame we're writing.
// The SubD writer (unlike the PolyMeshWriter) is kinda a state machine.
void SubDWriter::start( float iFrame )
{
    // Turn iFrame into an WriteSampleSelector.
    // Let's assume that we have some sort of class function that
    // knows how to do this. It will be same for all the writers.
    m_sampleBeingWritten = this->takoFrameToSampleSelector( iFrame );
}

//-*****************************************************************************
void SubDWriter::writeGeometry( ConstFloatVectorPtr iPoints )
{
    m_points->set( iPoints );
}

//-*****************************************************************************
void SubDWriter::writeGeometry( ConstFloatVectorPtr iPoints,
                                ConstIndexVectorPtr iFacePoints,
                                ConstIndexVectorPtr iFaceList )
{
    m_points->set( iPoints );
    m_facePoints->set( iFacePoints );
    m_faceList->set( iFaceList );
}

//-*****************************************************************************
void SubDWriter::end()
{
    // check that everything is rad...
    // Again, we could make this way more efficient.
#if DEBUG
    verifyGeometryLists( m_points->get()->size(),
                         m_facePoints->get(),
                         m_faceList->get() );
    if ( m_creaseSharpness->get() )
    {
        verifyCreases( m_points->get()->size(),
                       m_creaseSharpness->get()->size(),
                       m_creaseIndices->get(),
                       m_creaseLengths->get() );
    }
    if ( m_cornerSharpness->get() )
    {
        verifyCorners( m_points->get()->size(),
                       m_cornerSharpness->get()->size(),
                       m_cornerIndices->get() );
    }
#endif

    // Write the static stuff.
    if ( !m_firstSampleWritten )
    {
        m_faceVaryingInterpolateBoundary->write();
        m_faceVaryingPropagateCorners->write();
        m_interpolateBoundary->write();
        
        m_firstSampleWritten = true;
    }

    // These automatically detect changes and will be static on their own
    // if need be. This is also reasonably cheap. Later we can optimize
    // this by tracking changes at this class level, if we want.
    m_pointsSmp->write( m_sampleBeingWritten );
    m_facePointsSmp->write( m_sampleBeingWritten );
    m_faceListSmp->write( m_sampleBeingWritten );
    m_creaseSharpnessSmp->write( m_sampleBeingWritten );
}

//-*****************************************************************************





} // End namespace TakoAbc
} // End namespace Alembic
