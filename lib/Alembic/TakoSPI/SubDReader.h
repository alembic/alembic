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

#ifndef _Alembic_TakoSPI_SubDReader_h_
#define _Alembic_TakoSPI_SubDReader_h_

#include <Alembic/TakoSPI/Foundation.h>
#include <Alembic/TakoSPI/ReaderGlue.h>

namespace Alembic {

namespace TakoSPI {

namespace TAKO_SPI_LIB_VERSION_NS {

class TransformReaderIMPL;

/**
\brief SubDReaderIMPL is used to read sub-division meshes from the HDF file.
*/
class SubDReaderIMPL
    : public ReaderGlue<TakoAbstractV1::SubDReader>
{
    public:
        /**
        \brief Constructor requires a node name and a parent.
        \param iName The name of the node to read.
        \param iParent The polygon meshes' transform parent.
        */
        SubDReaderIMPL(const std::string & iName,
                       TransformReaderIMPL & iParent);

        /** Destructor */
        ~SubDReaderIMPL();

        /**
        Returns the Renderman specific faceVaryingInterpolateBoundary value.
        Since this value is optional, if a value less than 0 is returned then
        the value does not exist.
        */
        virtual int getFaceVaryingInterpolateBoundary();

        /**
        Returns the Renderman specific faceVaryingPropagateCorners value.
        Since this value is optional, if a value less than 0 is returned then
        the value does not exist.
        */
        virtual int getFaceVaryingPropagateCorners();

        /**
        Returns the boundary interpolation value.
        Since this value is optional, if a value less than 0 is returned then
        the value does not exist.
        */
        virtual int getInterpolateBoundary();

        /**
        \brief Gets the list of frame samples.
        \param ioFrames The set of frame samples to be added to.

        This function adds any frames samples it finds for this node to
        ioFrames.  Since ioFrames is a set, duplicates are automatically
        elminiated.
        */
        virtual void getFrames(std::set<float>& ioFrames) const;

        /**
        \brief Returns whether or not this node has any frame samples.
        \return true if this node has at least one frame sample, false if it
        is static.

        This is a convenience functions which returns whether or not the node
        has any frame samples.
        */
        virtual bool hasFrames() const;

        /**
        \brief Attempts to read the SubD at the given frame.
        \param iFrame The frame at which to read the SubD.
        \return Value which indicates what SubD data arrays have changed, or
        whether an error occured while reading the base mesh geometry.
        */
        virtual unsigned int read(float iFrame);

        virtual const std::vector<float> & getPoints()
        { return mPoints; }
        virtual const std::vector<index_t> & getFacePoints()
        { return mFacePoints; }
        virtual const std::vector<index_t> & getFaceList()
        { return mFaceList; }

        virtual const std::vector<index_t> & getHolePolyIndices()
        { return mHolePolyIndices; }

        virtual const std::vector<float> & getCreaseSharpness()
        { return mCreaseSharpness; }
        virtual const std::vector<index_t> & getCreaseIndices()
        { return mCreaseIndices; }
        virtual const std::vector<index_t> & getCreaseLengths()
        { return mCreaseLengths; }

        virtual const std::vector<index_t> & getCornerIndices()
        { return mCornerIndices; }
        virtual const std::vector<float> & getCornerSharpness()
        { return mCornerSharpness; }


    protected:
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

using namespace TAKO_SPI_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_TakoSPI_SubDReader_h_
