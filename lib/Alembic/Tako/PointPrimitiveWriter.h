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

#ifndef _Alembic_Tako_PointPrimitiveWriter_h_
#define _Alembic_Tako_PointPrimitiveWriter_h_

#include <Alembic/Tako/Foundation.h>
#include <Alembic/Tako/HDFWriterNode.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {

class TransformWriter;
class PointPrimitiveWriterPrivate;

/**
\brief PointPrimitiveWriter is used to write a point cloud to the hdf file.
*/
class PointPrimitiveWriter : public HDFWriterNode
{
    public:

        /**
        \brief Constructor requires a node name and a parent.
        \param iName The name to give the node.
        \param iParent The transform node to parent this polygon mesh to.
        */
        PointPrimitiveWriter(const char * iName, TransformWriter & iParent);

        /**
        \brief The destructor.
        */
        ~PointPrimitiveWriter();

        /**
        \brief Writes out the point information for a given frame.
        \param iFrame The frame to write the point cloud data for.  If iFrame
        is FLT_MAX then the point cloud is static.
        \param iPosition A packed array of points each points component is
        stored in X, Y, Z order.
        \param iVelocity A packed array of velocity vectors each component is
        stored in X, Y, Z order.
        \param iParticleIds The integer id for a given point in the point cloud.
        \param iConstantwidth The width for all the points in the point cloud.

        This function writes out the point cloud data and is used when the
        width is the same for all the points.
        If iFrame is FLT_MAX then the point cloud is static.
        */
        void write(float iFrame,
                const std::vector<float> & iPosition,
                const std::vector<float> & iVelocity,
                const std::vector<int> & iParticleIds,
                const float iConstantwidth) const;

        /**
        \brief Writes out the point information for a given frame.
        \param iFrame The frame to write the point cloud data for.  If iFrame
        is FLT_MAX then the point cloud is static.
        \param iPosition A packed array of points each points component is
        stored in X, Y, Z order.
        \param iVelocity A packed array of velocity vectors each component is
        stored in X, Y, Z order.
        \param iParticleIds The integer id for a given point in the point cloud.
        \param iWidth A width value for each of the points in the point cloud.

        This function writes out the point cloud data and is used when the
        width differs per point.
        If iFrame is FLT_MAX then the point cloud is static.
        */
        void write(float iFrame,
                const std::vector<float> & iPosition,
                const std::vector<float> & iVelocity,
                const std::vector<int> & iParticleIds,
                const std::vector<float> & iWidth) const;

    private:
        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}

using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_PointPrimitiveWriter_h_
