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

#ifndef _Alembic_Tako_SubDWriter_h_
#define _Alembic_Tako_SubDWriter_h_

#include <Alembic/Tako/Foundation.h>
#include <Alembic/Tako/HDFWriterNode.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {

class TransformWriter;

/**
\brief SubDWriter is used to write SubD meshes to the HDF file.
*/
class SubDWriter : public HDFWriterNode
{
    public:

        /**
        \brief Constructor requires a node name and a parent.
        \param iName The name to give the node.
        \param iParent The transform node to parent this SubD mesh to.
        */
        SubDWriter(const char * iName, TransformWriter & iParent);

        /**
        \brief The destructor.
        */
        ~SubDWriter();

        /**
        \brief Indicates the start of a frame of data.
        \param iFrame The frame to be written out.  If it is FLT_MAX then the
        data being written out is static.

        SubD meshes have several pieces of information besides their topology.
        Since most of this information is optional, and to keep the
        information manageable, they are broken up into several functions that
        can be called between start and end.  This function is used to
        indicate that we will be writing a particular frames worth of data.
        If iFrame is FLT_MAX then we will be writing out static data.
        */
        void start(float iFrame);

        /**
        \brief Indicates that we are done writing a frames worth of data.

        Another frames worth of data may not be written out till this function
        is called.  If start is called more than once without end being called
        an error will be thrown.
        */
        void end();

        /**
        \brief Writes just the points (not the topology).
        \param iPoints A packed array of points each point's components are
        stored in X, Y, Z order.

        This function can only be called if the topology has been written out
        for a previous frame.  The number of points must be the same as when
        the topology was previously written.  This function is used to write
        out changing vertex data.  (GEOMETRY_HOMOGENOUS)
        */
        void writeGeometry(const std::vector<float> & iPoints) const;

        /**
        \brief Writes out the point and toplogogy information.
        \param iPoints A packed array of points each point's components are
        stored in X, Y, Z order.
        \param iFacePoints logical index into iPoints which determines the
        point ordering for faces.
        \param iFaceList Determines the start and stop index from iFacePoints
        to define which points to use for a single face.

        This function write out the points and the topology for a SubD.
        This function is called when a new topology needs to be written out
        for a particular frame.
        */
        void writeGeometry(
            const std::vector<float> & iPoints,
            const std::vector<index_t> & iFacePoints,
            const std::vector<index_t> & iFaceList);

        /**
        \brief Writes out the crease sharpness information.
        \param iSharpness The sharpness value for each of the creases.

        This function can only be called if the crease topology has been
        written out for a previous frame.  The number of sharpness values
        must be the same as when the topology was previously written.  This
        function is used to write out varying sharpness data.
        (CREASES_HOMOGENOUS)
        */
        void writeCreases(
            const std::vector<float> & iSharpness) const;

        /**
        \brief Writes out the crease information.
        \param iSharpness The sharpness value for each of the creases.
        \param iIndices logical index into iPoints which determines what
        points will be part of a crease.
        \param iLengths Determines how many iIndices there are in a crease.
        The size of this array needs to be the same size as iSharpness.

        This function writes out the sharpness and topological information
        for creases.
        */
        void writeCreases(
            const std::vector<float> & iSharpness,
            const std::vector<index_t> & iIndices,
            const std::vector<index_t> & iLengths);

        /**
        \brief Writes out corner sharpness information.
        \param iSharpness The sharpness value for each corner.

        This function can only be called if the corner topology has been
        written out for a previous frame.  The number of sharpness values
        must be the same as when the topology was previously written.  This
        function is used to write out varying sharpness data.
        (CORNERS_HOMOGENOUS)
        */
        void writeCorners(const std::vector<float> & iSharpness) const;

        /**
        \brief Writes out the corner information.
        \param iSharpness The sharpness value for each corner.
        \param iIndices logical index into iPoints which determines which
        points are marked as corners.

        This function writes out the sharpness and topological information for
        corners.
        */
        void writeCorners(
            const std::vector<float> & iSharpness,
            const std::vector<index_t> & iIndices);

        /**
        \brief Writes out which faces are holes.
        \param iIndices logical index into iFaceLIst which determines which
        faces are holes.
        */
        void writeHolePolyIndices(const std::vector<index_t> & iIndices);

        /**
        \brief Writes out a Renderman specific flag.
        \param iValue If this is less than 0 then the flag will
        not be written out.
        */
        void writeFaceVaryingInterpolateBoundary(int iValue) const;

        /**
        \brief Writes out a Renderman specific flag.
        \param iValue If this is less than 0 then the flag will
        not be written out.
        */
        void writeFaceVaryingPropagateCorners(int iValue) const;

        /**
        \brief Writes out a Renderman specific flag.
        \param iValue If this is less than 0 then the flag will
        not be written out.
        */
        void writeInterpolateBoundary(int iValue) const;

    private:
        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}


using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_SubDWriter_h_
