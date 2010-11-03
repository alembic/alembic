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

#include <AbcClients/WFObjConvert/AbcReader.h>

namespace AbcClients {
namespace WFObjConvert {

//-*****************************************************************************
void AbcReader::parsingEnd( const std::string &iStreamName,
                            size_t iNumLines )
{
    makeCurrentObject();
}

//-*****************************************************************************
void AbcReader::v( index_t iIndex, const V3d &iPoint )
{
    assert( m_vertices.size()+1 == iIndex );

    V3f pt( iPoint.x, iPoint.y, iPoint.z );

    m_vertices.push_back( pt );
}

//-*****************************************************************************
void AbcReader::f( const IndexVec &vIndices,
                   const IndexVec &vtIndices,
                   const IndexVec &vnIndices )
{
    int count = vIndices.size();
    if ( count > 2 )
    {
        m_counts.push_back( count );
        for ( int i = 0; i < count; ++i )
        {
            m_indices.push_back( ( int )( vIndices[i]-1 ) );
        }
    }
}

//-*****************************************************************************
void AbcReader::activeObject( const std::string &iObjectName )
{
    makeCurrentObject();
    m_currentObjectName = iObjectName;
}

//-*****************************************************************************
void AbcReader::makeCurrentObject()
{
    if ( m_vertices.size() > 3 &&
         m_indices.size() > 3 &&
         m_counts.size() > 1 &&
         m_currentObjectName.length() &&
         !m_parentObject.getChildHeader( m_currentObjectName ) )
    {
        OPolyMesh meshObj( m_parentObject, m_currentObjectName );
        OPolyMeshSchema &mesh = meshObj.getSchema();

        OPolyMeshSchema::Sample psamp;
        psamp.setPositions( V3fArraySample( m_vertices ) );
        psamp.setIndices( Int32ArraySample( m_indices ) );
        psamp.setCounts( Int32ArraySample( m_counts ) );

        mesh.set( psamp );
    }

    m_indices.clear();
    m_counts.clear();
    m_currentObjectName = "";
}


} // End namespace WFObjConvert
} // End namespace AbcClients
