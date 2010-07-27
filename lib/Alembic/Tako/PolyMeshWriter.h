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

#ifndef _Alembic_Tako_PolyMeshWriter_h_
#define _Alembic_Tako_PolyMeshWriter_h_

#include <Alembic/Tako/Foundation.h>
#include <Alembic/Tako/HDFWriterNode.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {

class TransformWriter;

/**
\brief PolyMeshWriter is used to write polygon meshes to the HDF file.
*/
class PolyMeshWriter : public HDFWriterNode
{
    public:

        /**
        \brief Constructor requires a node name and a parent.
        \param iName The name to give the node.
        \param iParent The transform node to parent this polygon mesh to.
        */
        PolyMeshWriter(const char * iName, TransformWriter & iParent);

        /**
        \brief The destructor.
        */
        ~PolyMeshWriter();

        /**
        \brief Writes just the points (not the topology) for a given frame.
        \param iFrame The frame to write the point data for.
        \param iPoints A packed array of points each points component is stored
        in X, Y, Z order.
        \param iNormals A packed array of normals per face vertex is stored
        in X, Y, Z order.

        This function can only be called if the topology has been written out
        for a previous frame.  The number of points must be the same as when
        the topology was previously written.  This function is used to write
        out changing vertex data.  (TOPOLOGY_HOMOGENOUS)
        */
        void write(float iFrame,
            const std::vector<float> & iPoints,
            const std::vector<float> & iNormals) const;

        /**
        \brief Writes out the point and toplogogy information for a given frame.
        \param iFrame The frame to write the polygonal mesh data for.  If iFrame
        is FLT_MAX then the mesh is static.
        \param iPoints A packed array of points each points component is stored
        in X, Y, Z order.
        \param iNormals A packed array of normals per face vertex is stored
        in X, Y, Z order.
        \param iFacePoints logical index into iPoints which determines the
        point ordering for faces.
        \param iFaceList Determines the start and stop index from iFacePoints
        to define which points to use for a single face.

        This function write out the points and the topology for a polymesh.
        If iFrame is FLT_MAX then the mesh is static.  This function is
        called when a new topology needs to be written out for a particular
        frame.
        */
        void write(float iFrame,
            const std::vector<float> & iPoints,
            const std::vector<float> & iNormals,
            const std::vector<index_t> & iFacePoints,
            const std::vector<index_t> & iFaceList);

    private:
        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}

using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_PolyMeshWriter_h_
