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

#ifndef _Alembic_TakoSPI_NurbsSurfaceWriter_h_
#define _Alembic_TakoSPI_NurbsSurfaceWriter_h_

#include <Alembic/TakoSPI/Foundation.h>
#include <Alembic/TakoSPI/WriterGlue.h>

namespace Alembic {
namespace TakoSPI {
namespace TAKO_SPI_LIB_VERSION_NS {

class TransformWriterIMPL;

/**
\brief NurbsSurfaceWriterIMPL is used to write Nurbs surfaces to the HDF file.
*/
class NurbsSurfaceWriterIMPL
    : public WriterGlue<TakoAbstractV1::NurbsSurfaceWriter>
{
    protected:
    friend class TransformWriterIMPL;

        /**
        \brief Constructor requires a node name and a parent.
        \param iName The name to give the node.
        \param iParent The transform node to parent this Nurbs surface to.
        */
        NurbsSurfaceWriterIMPL(const std::string & iName,
                               TransformWriterIMPL & iParent);

    public:
        /**
        \brief The destructor.
        */
        virtual ~NurbsSurfaceWriterIMPL();

        /**
        \brief Indicates the start of a frame of data.
        \param iFrame The frame to be written out.  If it is FLT_MAX then the
        data being written out is static.

        Besides their topology and point data, Nurbs surfaces may also have
        trim curve data. Since trime curves are optional, and to keep the
        information manageable, they are broken up into several functions that
        can be called between start and end.  This function is used to
        indicate that we will be writing a particular frames worth of data.
        If iFrame is FLT_MAX then we will be writing out static data.
        */
        virtual void start(float iFrame);

        /**
        \brief Indicates that we are done writing a frames worth of data.

        Another frames worth of data may not be written out till this function
        is called.  If start is called more than once without end being called
        an error will be thrown.
        */
        virtual void end();

        /**
        \brief Writes out just the control vertices (not the topology)
        \param iCV A packed array of control vertices.

        The control vertices  are specified in u,v order where u varies more
        quickly in the array than v.  Each point is packed in x,y,z,w order.
        */
        virtual void write(const std::vector<float> & iCV);

        /**
        \brief Writes the control vertices and the topology for the
        Nurbs surface.

        \param iCV A packed array of control vertices.
        \param iKnotsInU The knots in the U dimension.
        \param iKnotsInV The knots in the V dimension.
        \param iMiscInfo Contains topology info such as form, number of spans,
        and degrees.

        The control vertices  are specified in u,v order where u varies more
        quickly in the array than v.  Each point is packed in x,y,z,w order.
        The size of iKnotsInU should be spans in U + 2 * degrees in U  + 1, and
        the size of iKnotsInV should be spans in V + 2 * degrees in V  + 1.
        */
        virtual void write(const std::vector<float> & iCV,
                           const std::vector<double> & iKnotsInU,
                           const std::vector<double> & iKnotsInV,
                           const NurbsSurfaceMiscInfo & iMiscInfo);

        /**
        \brief Writes the optional trim curves data.
        \param iTc Vector of optional trim curves to write out.

        This function writes out the trim curves control vertices and the
        topology information for each curve.
        */
        virtual void writeTrimCurve(const std::vector<NurbsCurveGrp> & iTc);

        /**
        \brief Writes just the control vertices for the optional trim curves
        data.
        \param iTc Vector of optional trim curves to write out.

        This function only writes out the control vertices for the trim
        curves.  This function can only be called if the topology was written
        out for a previous frame.  The number of control vertices must be the
        same as when the topology was previously written.
        */
        virtual void writeTrimCurveCV(const std::vector<NurbsCurveGrp> & iTc);

    private:
        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}

using namespace TAKO_SPI_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_TakoSPI_NurbsSurfaceWriter_h_
