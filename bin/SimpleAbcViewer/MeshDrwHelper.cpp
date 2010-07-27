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

#include "MeshDrwHelper.h"

namespace SimpleAbcViewer {

//-*****************************************************************************
MeshDrwHelper::MeshDrwHelper()
{
    makeInvalid();
}

//-*****************************************************************************
MeshDrwHelper::~MeshDrwHelper()
{
    makeInvalid();
}

//-*****************************************************************************
void MeshDrwHelper::updateMesh( const FloatArray * i_P,
                                const FloatArray * i_N,
                                const IndexArray * i_indices,
                                const IndexArray * i_starts )
{
    // Make invalid to start.
    makeInvalid();

    // Set the dudes.
    m_inputP = i_P;
    m_inputN = i_N;
    m_inputIndices = i_indices;
    m_inputStarts = i_starts;

    // Check stuff.
    if ( !m_inputP ||
         !m_inputIndices ||
         !m_inputStarts )
    {
        makeInvalid();
        return;
    }

    // Get the number of each thing.
    size_t numFaces = m_inputStarts->size();
    size_t numIndices = m_inputIndices->size();
    size_t numPoints = m_inputP->size() / 3;
    if ( numFaces < 1 ||
         numIndices < 1 ||
         numPoints < 1 )
    {
        // Invalid.
        makeInvalid();
        return;
    }

    // Make triangles.
    ssize_t faceIndexBegin = 0;
    ssize_t faceIndexEnd = 0;
    for ( size_t face = 0; face < numFaces; ++face )
    {
        faceIndexBegin = faceIndexEnd;
        faceIndexEnd = ( face == numFaces-1 ) ?
            numIndices : (*m_inputStarts)[face+1];

        // Check this face is valid
        if ( faceIndexEnd > numIndices ||
             faceIndexEnd < faceIndexBegin )
        {
            makeInvalid();
            return;
        }

        // Checking indices are valid.
        for ( ssize_t fidx = faceIndexBegin;
              fidx < faceIndexEnd; ++fidx )
        {
            if ( (*m_inputIndices)[fidx] >= numPoints )
            {
                makeInvalid();
                return;
            }
        }

        // Make triangles to fill this face.
        ssize_t count = faceIndexEnd - faceIndexBegin;
        if ( count > 2 && faceIndexEnd <= numIndices )
        {
            m_triangles.push_back(
                Tri( ( unsigned int )(*m_inputIndices)[faceIndexBegin+0],
                     ( unsigned int )(*m_inputIndices)[faceIndexBegin+1],
                     ( unsigned int )(*m_inputIndices)[faceIndexBegin+2] ) );
            for ( size_t c = 3; c < count; ++c )
            {
                m_triangles.push_back(
                    Tri( ( unsigned int )(*m_inputIndices)[faceIndexBegin+0],
                         ( unsigned int )(*m_inputIndices)[faceIndexBegin+c-1],
                         ( unsigned int )(*m_inputIndices)[faceIndexBegin+c]
                         ) );
            }
        }
    }

    // Because tako stores triangles flipped, reverse the triangles.
    for ( std::vector<Tri>::iterator titer = m_triangles.begin();
          titer != m_triangles.end(); ++titer )
    {
        Tri &tri = (*titer);
        std::swap( tri.y, tri.z );
    }

    // Cool, we made triangles.
    // Pretend the mesh is made...
    m_valid = true;

    // And now update just the P and N, which will update bounds
    // and calculate new normals if necessary.
    updateMesh( i_P, i_N );

    // And that's it.
}

//-*****************************************************************************
void MeshDrwHelper::updateMesh( const FloatArray * i_P,
                                const FloatArray * i_N )
{
    // Check validity.
    if ( !m_valid ||
         !i_P ||
         !m_inputP ||
         ( i_P->size() != m_inputP->size() ) )
    {
        makeInvalid();
        return;
    }

    // Okay, all is well with P...
    m_inputP = i_P;

    // Now get bounds.
    m_bounds.makeEmpty();
    size_t numPoints = m_inputP->size() / 3;
    const V3f *Pptr = ( const V3f * )&((*m_inputP)[0]);
    for ( size_t p = 0; p < numPoints; ++p )
    {
        const V3f &P = Pptr[p];
        m_bounds.extendBy( V3d( P.x, P.y, P.z ) );
    }

    // Now see if we need to calculate normals.
    m_inputN = i_N;
    m_customNormals.clear();
    if ( !m_inputN || m_inputN->size() != numPoints )
    {
        // Make some custom normals.
        m_inputN = NULL;
        m_customNormals.resize( numPoints*3 );

        std::fill( m_customNormals.begin(),
                   m_customNormals.end(),
                   0.0f );
        
        V3f *cNptr = ( V3f * )&(m_customNormals[0]);

        //std::cout << "Recalcing normals for object: "
        //          << m_host.name() << std::endl;
        
        for ( size_t tidx = 0; tidx < m_triangles.size(); ++tidx )
        {
            const Tri &tri = m_triangles[tidx];
            
            const V3f &A = Pptr[tri[0]];
            const V3f &B = Pptr[tri[1]];
            const V3f &C = Pptr[tri[2]];
            
            V3f AB = B - A;
            V3f AC = C - A;
            
            V3f wN = AB.cross( AC );
            cNptr[tri[0]] += wN;
            cNptr[tri[1]] += wN;
            cNptr[tri[2]] += wN;
        }

        // Normalize normals.
        for ( size_t nidx = 0; nidx < numPoints; ++nidx )
        {
            cNptr[nidx].normalize();
        }
    }
}

//-*****************************************************************************
void MeshDrwHelper::draw( const DrawContext & i_ctx ) const
{
    // Bail if invalid.
    if ( !m_valid ||
         m_triangles.size() < 1 ||
         !m_inputP )
    {
        return;
    }

    const V3f *points = ( const V3f * )&( (*m_inputP)[0] );
    const V3f *normals = NULL;
    if ( m_inputN != NULL && m_inputN->size() == m_inputP->size() )
    {
        normals = ( const V3f * )&( (*m_inputN)[0] );
    }
    else if ( m_customNormals.size() == m_inputP->size() )
    {
        normals = ( const V3f * )&( m_customNormals[0] );
    }

#ifdef SIMPLE_ABC_VIEWER_NO_GL_CLIENT_STATE
//#if 0    
    {
        GL_NOISY( glEnableClientState( GL_VERTEX_ARRAY ) );
        if ( normals )
        {
            GL_NOISY( glEnableClientState( GL_NORMAL_ARRAY ) );
            GL_NOISY( glNormalPointer( GL_FLOAT, 0,
                                       ( const GLvoid * )normals ) );
        }
        
        GL_NOISY( glVertexPointer( 3, GL_FLOAT, 0,
                                   ( const GLvoid * )points ) );
        
        GL_NOISY( glDrawElements( GL_TRIANGLES,
                                  ( GLsizei )m_triangles.size() * 3,
                                  GL_UNSIGNED_INT,
                                  ( const GLvoid * )&(m_triangles[0]) ) );
        
        if ( normals )
        {
            GL_NOISY( glDisableClientState( GL_NORMAL_ARRAY ) );
        }
        GL_NOISY( glDisableClientState( GL_VERTEX_ARRAY ) );
    }
#else
    glBegin( GL_TRIANGLES );
    
    for ( size_t i = 0; i < m_triangles.size(); ++i )
    {
        const Tri &tri = m_triangles[i];
        const V3f &vertA = points[tri[0]];
        const V3f &vertB = points[tri[1]];
        const V3f &vertC = points[tri[2]];
        
        if ( normals )
        {
            const V3f &normA = normals[tri[0]];
            glNormal3fv( ( const GLfloat * )&normA );
            glVertex3fv( ( const GLfloat * )&vertA );
            
            const V3f &normB = normals[tri[1]];
            glNormal3fv( ( const GLfloat * )&normB );
            glVertex3fv( ( const GLfloat * )&vertB );
            
            const V3f &normC = normals[tri[2]];
            glNormal3fv( ( const GLfloat * )&normC );
            glVertex3fv( ( const GLfloat * )&vertC );
        }
        else
        {
            glVertex3fv( ( const GLfloat * )&vertA );
            
            glVertex3fv( ( const GLfloat * )&vertB );
            
            glVertex3fv( ( const GLfloat * )&vertC );
        }
	
    }
    
    glEnd();

#endif
}

//-*****************************************************************************
void MeshDrwHelper::makeInvalid()
{
    m_inputP = NULL;
    m_inputN = NULL;
    m_inputIndices = NULL;
    m_inputStarts = NULL;
    m_valid = false;
    m_triangles.clear();
    m_customNormals.clear();
    m_bounds.makeEmpty();
}

} // End namespace SimpleAbcViewer
