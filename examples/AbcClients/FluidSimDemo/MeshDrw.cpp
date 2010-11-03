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

#include <FluidSimDemo/MeshDrw.h>

namespace FluidSimDemo {

//-*****************************************************************************
MeshDrw::MeshDrw( IObjectPtr iObject,
                  CollisionObjects &cobjs )
  : TransformDrw( iObject, cobjs )
{
    
    if ( m_object )
    {
        m_meshTrait = Atg::IMeshTrait( *m_object );
        m_normalsTrait = Atg::INormalsTrait( *m_object );
    }

    if ( m_meshTrait )
    {
        m_indices = m_meshTrait.vertexIndices().get();
        if ( !m_indices || m_indices.size() < 3 )
        {
            m_valid = false;
            return;
        }
        
        m_counts = m_meshTrait.faceCounts().get();
        if ( !m_counts )
        {
            m_valid = false;
            return;
        }
    }

    m_valid = true;
}

//-*****************************************************************************
void MeshDrw::readShape( float i_frame,
                         const M44d &iParentXform )
{
    if ( !m_meshTrait || !m_valid || !m_meshTrait.positions() )
    {
        m_shapeBounds.makeEmpty();
        return;
    }
    
    Abc::seconds_t secs = i_frame / 24.0f;

    if ( m_meshTrait.positions().isAnimated() )
    {
        m_points = m_meshTrait.positions().getAnimLowerBound( secs );
        m_shapeBounds.makeEmpty();
        for ( size_t p = 0; p < m_points.size(); ++p )
        {
            m_shapeBounds.extendBy( m_points[p] );
        }    
    }
    else
    {
        if ( !m_points )
        {
            m_points = m_meshTrait.positions().get();
            for ( size_t p = 0; p < m_points.size(); ++p )
            {
                m_shapeBounds.extendBy( m_points[p] );
            }
        }
    }

    if ( m_normalsTrait )
    {
        if ( m_normalsTrait.isAnimated() )
        {
            m_normals = m_normalsTrait.getAnimLowerBound( secs );
        }
        else
        {
            if ( !m_normals )
            {
                m_normals = m_normalsTrait.get();
            }
        }
    }

    if ( !m_points )
    {
        m_shapeBounds.makeEmpty();
        m_valid = false;
        return;
    }
    
    // Update mesh - either completely, or partially.
    if ( m_triangles.empty() )
    {
        createTriangles();
    }

    // Now figure out normals.
    // Now see if we need to calculate normals.
    size_t numPoints = m_points.size();
    if ( m_customNormals.empty() &&
         ( !m_normals || m_normals.size() != numPoints ) )
    {
        // Make some custom normals.
        m_normals.release();
        m_customNormals.resize( numPoints );

        std::fill( m_customNormals.begin(),
                   m_customNormals.end(),
                   V3f( 0.0f, 0.0f, 0.0f ) );
        
        V3f *cNptr = ( V3f * )&(m_customNormals.front());

        //std::cout << "Recalcing normals for object: "
        //          << m_host.name() << std::endl;
        
        for ( size_t tidx = 0; tidx < m_triangles.size(); ++tidx )
        {
            const Tri &tri = m_triangles[tidx];
            
            const V3f &A = m_points[tri[0]];
            const V3f &B = m_points[tri[1]];
            const V3f &C = m_points[tri[2]];
            
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
void MeshDrw::drawShape( const DrawContext & i_ctx ) const
{
#if 0
    if ( !i_ctx.drawGeometry() )
    {
        return;
    }
#endif
    
    // Bail if invalid.
    if ( !m_valid ||
         m_triangles.size() < 1 ||
         m_points.size() < 3 )
    {
        return;
    }

    const V3f *points = m_points.get();
    const V3f *normals = NULL;
    if ( m_normals.size() == m_points.size() )
    {
        normals = m_normals.get();
    }
    else if ( m_customNormals.size() == m_points.size() )
    {
        normals = ( const V3f * )&(m_customNormals.front());
    }

#ifndef SIMPLE_ABC_VIEWER_NO_GL_CLIENT_STATE
// #if 0
    {
        glEnableClientState( GL_VERTEX_ARRAY );
        if ( normals )
        {
            glEnableClientState( GL_NORMAL_ARRAY );
            glNormalPointer( GL_FLOAT, 0,
                             ( const GLvoid * )normals );
        }
        
        glVertexPointer( 3, GL_FLOAT, 0,
                         ( const GLvoid * )points );
        
        glDrawElements( GL_TRIANGLES,
                        ( GLsizei )m_triangles.size() * 3,
                        GL_UNSIGNED_INT,
                        ( const GLvoid * )&(m_triangles[0]) );
        
        if ( normals )
        {
            glDisableClientState( GL_NORMAL_ARRAY );
        }
        glDisableClientState( GL_VERTEX_ARRAY );
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
void MeshDrw::createTriangles()
{
    // Make invalid to start.
    m_valid = false;

    // Set the dudes.
    if ( m_points.size() < 3 ||
         m_indices.size() < 3 ||
         m_counts.size() < 1 )
    {
        // Invalid.
        return;
    }

    // Get the number of each thing.
    size_t numFaces = m_counts.size();
    size_t numIndices = m_indices.size();
    size_t numPoints = m_points.size();
    if ( numFaces < 1 ||
         numIndices < 1 ||
         numPoints < 1 )
    {
        // Invalid.
        return;
    }

    // Make triangles.
    size_t faceIndexBegin = 0;
    for ( size_t face = 0; face < numFaces; ++face )
    {
        size_t count = m_counts[face];
        size_t faceIndexEnd = faceIndexBegin + count;

        // Check this face is valid
        if ( faceIndexEnd > numIndices ||
             faceIndexEnd < faceIndexBegin )
        {
            // Invalid
            return;
        }

        // Checking indices are valid.
        for ( size_t fidx = faceIndexBegin; fidx < faceIndexEnd; ++fidx )
        {
            if ( m_indices[fidx] >= numPoints )
            {
                // Invalid;
                return;
            }
        }

        // Make triangles to fill this face.
        if ( count > 2 && faceIndexEnd <= numIndices )
        {
            m_triangles.push_back(
                Tri( ( unsigned int )m_indices[faceIndexBegin+0],
                     ( unsigned int )m_indices[faceIndexBegin+1],
                     ( unsigned int )m_indices[faceIndexBegin+2] ) );
            for ( size_t c = 3; c < count; ++c )
            {
                m_triangles.push_back(
                    Tri( ( unsigned int )m_indices[faceIndexBegin+0],
                         ( unsigned int )m_indices[faceIndexBegin+c-1],
                         ( unsigned int )m_indices[faceIndexBegin+c]
                         ) );
            }
        }

        faceIndexBegin += count;
    }

    // Cool, we made triangles.
    // Pretend the mesh is made...
    m_valid = true;
}

} // End namespace FluidSimDemo
