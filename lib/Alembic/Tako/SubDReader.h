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

#ifndef _Alembic_Tako_SubDReader_h_
#define _Alembic_Tako_SubDReader_h_

#include <Alembic/Tako/Foundation.h>
#include <Alembic/Tako/HDFReaderNode.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {

class TransformReader;

/**
\brief SubDReader is used to read sub-division meshes from the HDF file.
*/
class SubDReader : public HDFReaderNode
{
    public:

        /**
        \brief The hint on the topology type of the SubD mesh.

        This enumeration helps to provide a hint as to whether the mesh is
        static, deforming, or the topology is changing.  It also provides the
        hint as to how the creases, corners, and hole poly index will behave,
        if they even exist.
        */
        enum {
            /** Indicates that the mesh is not animated*/
            GEOMETRY_STATIC       = 0x0000,

            /** Indicates that the mesh is deforming*/
            GEOMETRY_HOMOGENOUS   = 0x0001,

            /**
            Indicates that the topology has changes since the last frame that
            was read.
            */
            GEOMETRY_HETEROGENOUS = 0x0002,

            /** Indicates that the crease data is static */
            CREASES_STATIC        = 0x0010,

            /**
            Indicates that just the crease sharpness is changing through time.
            */
            CREASES_HOMOGENOUS    = 0x0020,

            /**
            Indicates that the crease sharpness, crease indices and crease
            lengths change through time.
            */
            CREASES_HETEROGENOUS  = 0x0030,

            /** Indicates that the corner data is static */
            CORNERS_STATIC        = 0x0100,

            /**
            Indicates that the corner sharpness is changing through time.
            */
            CORNERS_HOMOGENOUS    = 0x0200,

            /**
            Indicates that the corner sharpness, and corner indices changes
            through time.
            */
            CORNERS_HETEROGENOUS  = 0x0300,

            /** Indicates that the hole polygon indices are static. */
            HOLEPOLY_STATIC       = 0x1000,

            /** Indicates that the hole polygon indices change through time. */
            HOLEPOLY_HETEROGENOUS = 0x3000,

            /** Indicates that there was an error while reading the SubD.  This
            will occur when an illegal frame for the animated geometry is
            attempted to be read.  It will not occur on illegal frame reads of
            creases, corners, or holes because that geometry may be optional
            at that frame.*/
            READ_ERROR            = 0x80000000
        };

        /**
        \brief Constructor requires a node name and a parent.
        \param iName The name of the node to read.
        \param iParent The polygon meshes' transform parent.
        */
        SubDReader(const char * iName, TransformReader & iParent);

        /** Destructor */
        ~SubDReader();

        /**
        Returns the Renderman specific faceVaryingInterpolateBoundary value.
        Since this value is optional, if a value less than 0 is returned then
        the value does not exist.
        */
        int getFaceVaryingInterpolateBoundary();

        /**
        Returns the Renderman specific faceVaryingPropagateCorners value.
        Since this value is optional, if a value less than 0 is returned then
        the value does not exist.
        */
        int getFaceVaryingPropagateCorners();

        /**
        Returns the boundary interpolation value.
        Since this value is optional, if a value less than 0 is returned then
        the value does not exist.
        */
        int getInterpolateBoundary();

        /**
        \brief Gets the list of frame samples.
        \param ioFrames The set of frame samples to be added to.

        This function adds any frames samples it finds for this node to
        ioFrames.  Since ioFrames is a set, duplicates are automatically
        elminiated.
        */
        void getFrames(std::set<float>& ioFrames) const;

        /**
        \brief Returns whether or not this node has any frame samples.
        \return true if this node has at least one frame sample, false if it
        is static.

        This is a convenience functions which returns whether or not the node
        has any frame samples.
        */
        bool hasFrames() const;

        /**
        \brief Attempts to read the SubD at the given frame.
        \param iFrame The frame at which to read the SubD.
        \return Value which indicates what SubD data arrays have changed, or
        whether an error occured while reading the base mesh geometry.
        */
        unsigned int read(float iFrame);

        /**
        The point data for the base mesh.  Each point is packed into the array
        in x,y,z order.
        */
        std::vector<float> mPoints;

        /**
        The logical point index that references the data stored in mPoints.
        These indices are in clockwise order.
        */
        std::vector<index_t> mFacePoints;

        /**
        Physical indices into mFacePoints that determine where the first index
        point is, and where to stop the loop when creating the polygon.
        mFaceList[i+ 1] - mFaceList[i] is the number of points a polygon has.
        */
        std::vector<index_t> mFaceList;

        /**
        Logical index which determines which faces should act as holes.
        */
        std::vector<index_t> mHolePolyIndices;

        /**
        The crease sharpness value for a specified edge.
        The length of mCreaseSharpness will be the same as mCreaseLengths.
        */
        std::vector<float> mCreaseSharpness;

        /**
        Logical index into mPoints which determine which points (and hence the
        edge) make up the crease.
        */
        std::vector<index_t> mCreaseIndices;

        /**
        Defines how many mCreaseIndices determine an edge.
        */
        std::vector<index_t> mCreaseLengths;

        /**
        Logical index into mPoints which determine which points are corners.
        */
        std::vector<index_t> mCornerIndices;

        /**
        The sharpness value for each corner.
        */
        std::vector<float> mCornerSharpness;

    private:
        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}

using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_SubDReader_h_
