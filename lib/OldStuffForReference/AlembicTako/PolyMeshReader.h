//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#ifndef _AlembicTako_PolyMeshReader_h_
#define _AlembicTako_PolyMeshReader_h_

#include <AlembicTako/Foundation.h>
#include <AlembicTako/HDFReaderNode.h>
#include <AlembicTako/TransformReader.h>
#include <AlembicTako/PolyMesh.h>

namespace AlembicTako {

//-*****************************************************************************
class PolyMeshReader : public HDFReaderNode
{
public:
    // Make as child of reader.
    PolyMeshReader( const std::string &nodeName, TransformReader &parent )
      : HDFReaderNode(),
        m_polyMesh( parent.alembicObject(), nodeName,
                    AlembicAsset::kThrowException )
    {
        HDFReaderNode::init( m_polyMesh );
    }

    // Return type for read function.
    enum TopologyType
    {
        TOPOLOGY_STATIC,
        TOPOLOGY_HOMOGENOUS,
        TOPOLOGY_HETEROGENOUS,
        READ_ERROR
    };

    TopologyType read( float iFrame );

    const AlembicAsset::IV3fSample &points() const { return m_points; }
    const AlembicAsset::IV3fSample &normals() const { return m_normals; }
    const AlembicAsset::IIntSample &facePoints() const { return m_facePoints; }
    const AlembicAsset::IIntSample &faceList() const { return m_faceList; }

protected:
    IPolyMeshObj m_polyMesh;

    AlembicAsset::IV3fSample m_points;
    AlembicAsset::IV3fSample m_normals;
    AlembicAsset::IIntSample m_facePoints;
    AlembicAsset::IIntSample m_faceList;
};


} // End namespace AlembicTako

#endif
