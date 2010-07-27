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

#ifndef _Alembic_Tako_PolyMeshReader_h_
#define _Alembic_Tako_PolyMeshReader_h_

#include <Alembic/Tako/Foundation.h>
#include <Alembic/Tako/HDFReaderNode.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {

class TransformReader;

/**
\brief PolyMeshReader is used to read polygon meshes from the HDF file.
*/
class PolyMeshReader : public HDFReaderNode
{
    public:

        /**
        \brief The hint on the topology type of a mesh.

        This enumerated type helps to provide a hint as to whether the mesh is
        static, deforming, or the topology is changing.
        */
        enum TopologyType
        {
            /** Indicates that the mesh is not animated*/
            TOPOLOGY_STATIC,

            /** Indicates that the mesh is deforming*/
            TOPOLOGY_HOMOGENOUS,

            /**
            Indicates that the topology has changes since the last frame that
            was read.
            */
            TOPOLOGY_HETEROGENOUS,

            /**
            Indicates that there was an error when trying to read the geometry.
            This can occur if an illegal frame was attempted to be read.
            */
            READ_ERROR
        };

        /**
        \brief Constructor requires a node name and a parent.
        \param iName The name of the node to read.
        \param iParent The polygon meshes' transform parent.
        */
        PolyMeshReader(const char * iName, TransformReader & iParent);

        /**
        \brief The destructor.
        */
        ~PolyMeshReader();

        /**
        \brief Adds found frames for this transform to a set.
        \param ioSamples The set of frames to be added to.

        Looks for any frame samples that this transform may have and adds them
        to the set ioSamples.
        */
        void getFrames(std::set<float>& ioSamples) const;

        /**
        \brief Returns whether or not this poly mesh has any samples.
        \return true if this poly mesh is sampled.
        */
        bool hasFrames() const;

        /**
        \brief Reads a frames worth of data.
        \param iFrame The frame of data to read.
        \return Indicates what data may have changed.

        The first call to read will fill in mPoints, mFacePoints, and mFaceList.
        It will return TOPOLOGY_STATIC if the data does not depend on the frame,
        it will return TOPOLOGY_HOMOGENOUS if the data DOES depend on the frame,
        and finally it could return READ_ERROR if it wasn't TOPOLOGY_STATIC and
        the frame of data did not exist.  For subsequent calls to read, if
        the geometry was static, nothing is read (even if the arrays have been
        cleared out by the user) and TOPOLOGY_STATIC is returned.  If just
        the points changed, that data is read and TOPOLOGY_HOMOGENOUS is
        returned.  If all the arrays changed on this read, TOPOLOGY_HETEROGENOUS
        is returned.  If the frame of data could not be found, READ_ERROR is
        returned.
        */
        TopologyType read(float iFrame);

        /**
        The list of points packed in X, Y, Z order.  It won't be filled out
        until read is called.
        */
        std::vector<float> mPoints;

        /**
        The list of normals packed in X, Y, Z order.  It won't be filled out
        until read is called.  This data is optional and this vector could be
        empty.
        */
        std::vector<float> mNormals;

        /** Logical index into iPoints which determines the point order for
        the faces.  It won't be filled out until read is called.  These indices
        are in clockwise order.
        */
        std::vector<index_t> mFacePoints;

        /**
        Physical indices into mFacePoints that determine where the first index
        point is, and where to stop the loop when creating the polygon.
        mFaceList[i+ 1] - mFaceList[i] is the number of points a polygon has.
        */
        std::vector<index_t> mFaceList;

    private:
        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}

using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_PolyMeshReader_h_
