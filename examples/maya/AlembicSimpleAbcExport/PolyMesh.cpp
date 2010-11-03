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

#include "PolyMesh.h"

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
PolyMesh::PolyMesh( Exportable &iExportableParent,
                    MDagPath &iDagPath,
                    MObject &iNode,
                    const std::string &iName,
                    const Abc::TimeSamplingType &iTsmpType,
                    bool iDeforming )
  : Exportable( iDagPath, iNode, iName )
  , m_deforming( iDeforming )
{
    std::cout << "PolyMesh::PolyMesh, iName = " << iName
              << std::endl;
    
    m_polyMesh = Abc::OPolyMesh( iExportableParent.getAlembicObject(),
                                 iName,
                                 iTsmpType );
    m_alembicObject = m_polyMesh;
    m_alembicProperties = m_polyMesh.getSchema();

#if 1
    // Make bounds with the same time sampling as the mesh.
    m_boundsProperty = Abc::OBox3dProperty( m_polyMesh.getSchema(),
                                            "bounds",
                                            iTsmpType );
    Abc::MetaData mdata;
    Abc::SetGeometryScope( mdata, Abc::kFacevaryingScope );
    m_normals = Abc::ON3fArrayProperty( m_polyMesh.getSchema(),
                                        "N",
                                        iTsmpType,
                                        mdata );
    m_sts = Abc::OV2fArrayProperty( m_polyMesh.getSchema(),
                                    "uv0",
                                    iTsmpType,
                                    mdata );
#endif
}

//-*****************************************************************************
bool PolyMesh::valid()
{
    return /*m_boundsProperty.valid() &&*/ m_polyMesh.valid()
        && Exportable::valid();
}

//-*****************************************************************************
Abc::Box3d PolyMesh::writeSample( const Abc::OSampleSelector &iSS )
{
    // First, call base class sample write, which will return bounds
    // of any children.
    Abc::index_t sampleIndex = iSS.getIndex();
    Abc::Box3d bounds = Exportable::writeSample( iSS );

    // If we're not deforming, don't bother with new sample.
    // Do calculate bounds and set them, though.
    if ( sampleIndex != 0 && !m_deforming )
    {
        bounds.extendBy( m_firstSampleSelfBounds );
        m_boundsProperty.set( bounds, iSS );
        return bounds;
    }

    // Make a mesh
    MStatus status;
    MFnMesh mesh( m_dagPath, &status );
    CHECK_MAYA_STATUS;
    mesh.updateSurface();
    mesh.syncObject();

    // Make a sample.
    Abc::OPolyMeshSchema::Sample abcPolyMeshSample;

    //-*************************************************************************
    // WRITE VERTICES
    //-*************************************************************************
    MPointArray vertices;
    mesh.getPoints( vertices );
    size_t npoints = vertices.length();
    std::vector<Abc::V3f> v3fVerts( npoints );

    Abc::Box3d shapeBounds;
    shapeBounds.makeEmpty();
    for ( size_t i = 0; i < npoints; ++i )
    {
        const MPoint &vi = vertices[i];
        Abc::V3f pi( vi.x, vi.y, vi.z );
        v3fVerts[i] = pi;
        shapeBounds.extendBy( Abc::V3d( vi.x, vi.y, vi.z ) );
    }
    if ( sampleIndex == 0 )
    {
        m_firstSampleSelfBounds = shapeBounds;
    }
    bounds.extendBy( shapeBounds );

    // Set the bounds sample.
    m_boundsProperty.set( bounds, iSS );

    // Stuff the positions into the mesh sample.
    abcPolyMeshSample.setPositions( Abc::V3fArraySample( v3fVerts ) );

    //-*************************************************************************
    // OTHER STUFF, FOR FIRST OR LATER VERTICES
    //-*************************************************************************
    std::vector<Abc::int32_t> abcIndices;
    std::vector<Abc::int32_t> abcCounts;
    std::vector<Abc::N3f> abcNormals;
    std::vector<Abc::V2f> abcUvs;

    //-*************************************************************************
    // GET MESH NORMALS & UVS
    //-*************************************************************************
    size_t nnormals = mesh.numNormals();
    MFloatVectorArray meshNorms;
    if ( nnormals > 0 )
    {
        mesh.getNormals( meshNorms, MSpace::kObject );
    }
    
    size_t nuvs = mesh.numUVs();
    MFloatArray meshU;
    MFloatArray meshV;
    if ( nuvs > 0 )
    {
        mesh.getUVs( meshU, meshV );
    }

    //-*************************************************************************
    // LOOP OVER FIRST OR SUBSEQUENT SAMPLES
    //-*************************************************************************
    if ( sampleIndex == 0 )
    {
        // FIRST SAMPLE

        // Loop over polys.
        size_t npolys = mesh.numPolygons();
        abcCounts.resize( npolys );

        Abc::int32_t faceIndex = 0;
        Abc::int32_t faceStartVertexIndex = 0;

        for ( MItMeshPolygon piter( m_dagPath );
              !piter.isDone(); piter.next(), ++faceIndex )
        {
            Abc::int32_t faceCount = piter.polygonVertexCount();
            abcCounts[faceIndex] = faceCount;
            faceStartVertexIndex += faceCount;

            for ( Abc::int32_t faceVertex = 0;
                  faceVertex < faceCount; ++faceVertex )
            {
                abcIndices.push_back( piter.vertexIndex( faceVertex ) );

                if ( nnormals > 0 )
                {
                    size_t normIndex = piter.normalIndex( faceVertex );
                    const MFloatVector &norm = meshNorms[normIndex];
                    Abc::N3f abcNorm( norm[0], norm[1], norm[2] );
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

        // We have now collected abcIndices, abcStarts, abcNormals, and abcUvs.
        // Put them into the sample.
        abcPolyMeshSample.setIndices( Abc::Int32ArraySample( abcIndices ) );
        abcPolyMeshSample.setCounts( Abc::Int32ArraySample( abcCounts ) );

        if ( nnormals > 0 && m_normals )
        {
            m_normals.set( Abc::N3fArraySample( abcNormals ), iSS );
        }
        if ( nuvs > 0 && m_sts )
        {
            m_sts.set( Abc::V2fArraySample( abcUvs ), iSS );
        }
    }
    else if ( ( nnormals > 0 && m_normals ) ||
              ( nuvs > 0 && m_sts ) )
    {
        // SUBSEQUENT SAMPLES
        // Just gathering normals and uvs.
        // (vertices handled above)
        
        // Loop over polys.
        Abc::int32_t faceIndex = 0;
        Abc::int32_t faceStartVertexIndex = 0;
        
        for ( MItMeshPolygon piter( m_dagPath );
              !piter.isDone(); piter.next(), ++faceIndex )
        {
            Abc::int32_t faceCount = piter.polygonVertexCount();
            for ( Abc::int32_t faceVertex = 0;
                  faceVertex < faceCount; ++faceVertex )
            {
                if ( nnormals > 0 )
                {
                    size_t normIndex = piter.normalIndex( faceVertex );
                    const MFloatVector &norm = meshNorms[normIndex];
                    Abc::N3f abcNorm( norm[0], norm[1], norm[2] );
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

        // We have now collected abcNormals, and abcUvs.
        // Put them into the sample.
        if ( nnormals > 0 && m_normals )
        {
            m_normals.set( Abc::N3fArraySample( abcNormals ), iSS );
        }
        if ( nuvs > 0 )
        {
            m_sts.set( Abc::V2fArraySample( abcUvs ), iSS );
        }
    }

    // Set the mesh sample.
    m_polyMesh.getSchema().set( abcPolyMeshSample, iSS );

    return bounds;
}

//-*****************************************************************************
void PolyMesh::close()
{
    m_boundsProperty.reset();
    m_normals.reset();
    m_sts.reset();
    m_polyMesh.reset();
    Exportable::close();
}

} // End namespace AlembicSimpleAbcExport
