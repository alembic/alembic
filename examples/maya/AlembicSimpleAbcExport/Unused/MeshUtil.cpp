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

#include "MeshUtil.h"

namespace AlembicAbcExport {

//-*****************************************************************************
Abc::Box3d WriteMeshRestSample( MDagPath &dagPath,
                                MObject &node,
                                
                                Atg::OMeshTrait &abcMeshTrait,
                                Atg::OSubdTrait &abcSubdTrait,
                                Atg::ONormalsTrait &abcNormalsTrait,
                                Atg::OUvsTrait &abcUvsTrait )
{
    // std::cout << "About to write Mesh Rest Sample" << std::endl;
    
    // Start building our bounds.
    Abc::Box3d allBounds;
    allBounds.makeEmpty();

    // Make a mesh.
    MStatus status;
    MFnMesh mesh( dagPath, &status );
    mesh.updateSurface();
    mesh.syncObject();
    CHECK_MAYA_STATUS;
    if ( !status )
    {
        return allBounds;
    }

    //-*************************************************************************
    // UNIFORM STUFF (Just Subd Stuff for now)
    //-*************************************************************************
    if ( abcSubdTrait )
    {
        abcSubdTrait.scheme().set( ( char )Atg::kCatmullClarkSubd );
        abcSubdTrait.boundary().set( ( char )Atg::kEdgesCornersBoundaryInterp );
    }

    //-*************************************************************************
    // VERTICES
    //-*************************************************************************
    MPointArray vertices;
    mesh.getPoints( vertices );
    size_t npoints = vertices.length();
    
    // Write vertex positions 
    {
        std::vector<Abc::V3f> v3dVerts( npoints );
        for ( size_t i = 0; i < npoints; ++i )
        {
            const MPoint &vi = vertices[i];
            Abc::V3f pi( vi.x, vi.y, vi.z );
            v3dVerts[i] = pi;
            allBounds.extendBy( pi );
        }

        // This will set the rest pose if time is rest time.
        abcMeshTrait.positions().set( &v3dVerts.front(), npoints );
    }

    //-*************************************************************************
    // PREPARE NORMALS AND UVS
    //-*************************************************************************
    size_t nnormals = (( bool )abcNormalsTrait) ? mesh.numNormals() : 0;
    MFloatVectorArray meshNorms;
    std::vector<Abc::V3f> abcNormals;
    if ( nnormals > 0 )
    {
        mesh.getNormals( meshNorms, MSpace::kObject );
    }
    
    size_t nuvs = (( bool )abcUvsTrait) ? mesh.numUVs() : 0;
    std::vector<Abc::V2f> abcUvs;
    MFloatArray meshU;
    MFloatArray meshV;
    if ( nuvs > 0 )
    {
        mesh.getUVs( meshU, meshV );
    }

    //-*************************************************************************
    // BUILD FACE COUNTS, INDICES, NORMALS & UVS
    //-*************************************************************************
    size_t npolys = mesh.numPolygons();

    std::vector<Abc::int32_t> abcFaceCounts( npolys );
    std::vector<Abc::int32_t> abcIndices;
        
    Abc::int32_t faceIndex = 0;
        
    for ( MItMeshPolygon piter( dagPath );
          !piter.isDone(); piter.next(), ++faceIndex ) 
    {
        Abc::int32_t faceCount = piter.polygonVertexCount();
        
        abcFaceCounts[faceIndex] = faceCount;
            
        for ( Abc::int32_t faceVertex = 0;
              faceVertex < faceCount; ++faceVertex )
        {
            abcIndices.push_back( piter.vertexIndex( faceVertex ) );
            
            if ( nnormals > 0 )
            {
                size_t normIndex = piter.normalIndex( faceVertex );
                const MFloatVector &norm = meshNorms[normIndex];
                Abc::V3f abcNorm( norm[0], norm[1], norm[2] );
                abcNormals.push_back( abcNorm );
            }

            if ( nuvs > 0 )
            {
                int uvIndex = 0;
                piter.getUVIndex( faceVertex, uvIndex );
                Abc::V2f abcUv( meshU[uvIndex], meshV[uvIndex] );
                abcUvs.push_back( abcUv );
            }
        }
    }

    // Write 'em.
    abcMeshTrait.faceCounts().set( &abcFaceCounts.front(),
                                   abcFaceCounts.size() );
    abcMeshTrait.vertexIndices().set( &abcIndices.front(),
                                      abcIndices.size() );
    if ( abcNormalsTrait )
    {
        abcNormalsTrait->set( &abcNormals.front(), abcNormals.size() );
    }
    
    if ( abcUvsTrait )
    {
        abcUvsTrait->set( &abcUvs.front(), abcUvs.size() );
    }

    // All done.
    return allBounds;
}

//-*****************************************************************************
Abc::Box3d WriteMeshAnimSample( MDagPath &dagPath,
                                MObject &node,
                                
                                Atg::OMeshTrait &abcMeshTrait,
                                Atg::ONormalsTrait &abcNormalsTrait,

                                const Abc::Time &time )
{
    
    // std::cout << "About to write Mesh Anim Sample at time: "
    //           << time << std::endl;
    
    // Start building our bounds.
    Abc::Box3d allBounds;
    allBounds.makeEmpty();

    // Make a mesh.
    MStatus status;
    MFnMesh mesh( dagPath, &status );
    mesh.updateSurface();
    mesh.syncObject();
    CHECK_MAYA_STATUS;
    if ( !status )
    {
        return allBounds;
    }

    //-*************************************************************************
    // VERTICES
    //-*************************************************************************
    MPointArray vertices;
    mesh.getPoints( vertices );
    size_t npoints = vertices.length();
    
    // Write vertex positions 
    {
        std::vector<Abc::V3f> v3dVerts( npoints );
        for ( size_t i = 0; i < npoints; ++i )
        {
            const MPoint &vi = vertices[i];
            Abc::V3f pi( vi.x, vi.y, vi.z );
            v3dVerts[i] = pi;
            allBounds.extendBy( pi );
        }

        // This will set the rest pose if time is rest time.
        abcMeshTrait.positions().setAnim( time, &v3dVerts.front(), npoints );
    }

    //-*************************************************************************
    // PREPARE NORMALS
    //-*************************************************************************
    size_t nnormals = (( bool )abcNormalsTrait) ? mesh.numNormals() : 0;
    MFloatVectorArray meshNorms;
    std::vector<Abc::V3f> abcNormals;
    if ( nnormals == 0 )
    {
        return allBounds;
    }
    
    mesh.getNormals( meshNorms, MSpace::kObject );
    
    //-*************************************************************************
    // BUILD NORMALS
    //-*************************************************************************
    size_t npolys = mesh.numPolygons();
        
    Abc::int32_t faceIndex = 0;
        
    for ( MItMeshPolygon piter( dagPath );
          !piter.isDone(); piter.next(), ++faceIndex ) 
    {
        Abc::int32_t faceCount = piter.polygonVertexCount();
            
        for ( Abc::int32_t faceVertex = 0;
              faceVertex < faceCount; ++faceVertex )
        {
            size_t normIndex = piter.normalIndex( faceVertex );
            const MFloatVector &norm = meshNorms[normIndex];
            Abc::V3f abcNorm( norm[0], norm[1], norm[2] );
            abcNormals.push_back( abcNorm );
        }
    }

    // Write 'em.
    abcNormalsTrait->setAnim( time, &abcNormals.front(), abcNormals.size() );
}

} // End namespace AlembicAbcExport
