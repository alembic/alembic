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

#ifndef _Alembic_Tako_NurbsSurfaceReader_h_
#define _Alembic_Tako_NurbsSurfaceReader_h_

#include <Alembic/Tako/Foundation.h>
#include <Alembic/Tako/NurbsDef.h>
#include <Alembic/Tako/HDFReaderNode.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {

class TransformReader;

/**
\brief NurbsSurfaceReader is used to read NURBS surface from the HDF file.
*/
class NurbsSurfaceReader : public HDFReaderNode
{
    public:

        /**
        \brief The hint on the topology type of the NURBS surface.

        This enumeration helps to provide a hint as to whether the surface is
        static, deforming, or the topology is changing.  It also provides the
        hint as to how the trim curve will behave, if the information exists.
        */
        enum
        {
            /** Indicates that the surface is not animated */
            GEOMETRY_STATIC = 0x0000,

            /** Indicates that the surface is deforming */
            GEOMETRY_HOMOGENOUS = 0x0001,

            /**
            Indicates that the topology has changes since the last frame that
            was read.
            */
            GEOMETRY_HETEROGENOUS = 0x0002,

            /** Indicates that the trim curve data is static */
            TRIM_CURVE_STATIC = 0x0010,

            /**
            Indicates that the control vertices of trim curves change through
            time.
            */
            TRIM_CURVE_HOMOGENOUS = 0x0020,

            /**
            Indicates that the topology of trim curves change since the last
            frame that was read.
            */
            TRIM_CURVE_HETEROGENOUS = 0x0030,

            /** Indicates that there was an error while reading the surface.
            This will occur when an illegal frame for the animated geometry is
            attempted to be read.  It will not occur on illegal frame reads of
            trim curve because it may be optional at that frame.
            */
            READ_ERROR = 0xFFFF
        };

        /**
        \brief Constructor requires a node name and a parent.
        \param iName The name of the node to read.
        \param iParent The parent transform node.
        */
        NurbsSurfaceReader(const char * iName, TransformReader & iParent);

        /**
        \brief Destructor
        */
        ~NurbsSurfaceReader();

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
        \brief Attempts to read the NURBS surface at the given frame.
        \param iFrame The frame at which to read the NURBS surface.
        \return What data was read (geometric or trim curves) and hints about
        what data was filled in.
        */
        unsigned int read(float iFrame);

        /**
        The control vertex data for the NURBS surface.  The data is specified
        in u,v order where u varies more quickly in the array than v.  Each
        point is packed in x,y,z,w order.
        */
        std::vector<float> mCV;

        /**
        The knots in the U dimension.  The size of the array will be
        spans in U + 2 * degrees in U  + 1.
        */
        std::vector<double> mKnotsInU;

        /**
        The knots in the V dimension.  The size of the array will be
        spans in V + 2 * degrees in V  + 1.
        */
        std::vector<double> mKnotsInV;

        /**
        Holds a bunch of NURBS surface information such as the form,
        number of spans, and degrees.
        */
        NurbsSurfaceMiscInfo mMiscInfo;

        /**
        Contains the optional Trim Curve data.
        */
        std::vector<NurbsCurveGrp> mTrimCurve;

    private:
        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}

using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_NurbsSurfaceReader_h_
