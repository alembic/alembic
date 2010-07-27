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


#ifndef _Alembic_TakoSPI_PointPrimitiveReader_h_
#define _Alembic_TakoSPI_PointPrimitiveReader_h_

#include <Alembic/TakoSPI/Foundation.h>
#include <Alembic/TakoSPI/ReaderGlue.h>

namespace Alembic {

namespace TakoSPI {

namespace TAKO_SPI_LIB_VERSION_NS {

class TransformReaderIMPL;

/**
\brief PointPrimitiveReaderIMPL is used to read point primitive data from the HDF
file.
*/
class PointPrimitiveReaderIMPL
    : public ReaderGlue<TakoAbstractV1::PointPrimitiveReader>
{
    protected:
    friend class TransformReaderIMPL;

        /**
        \brief Constructor requires a node name and a parent node.
        \param iName The name of the node to read.
        \param iParent The parent transform node.
        */
        PointPrimitiveReaderIMPL( const std::string & iName,
                                  TransformReaderIMPL & iParent);

    public:
        /**
        \brief Destructor
        */
        virtual ~PointPrimitiveReaderIMPL();


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
        \brief Attempts to read the point primitive at the given frame.
        \param iFrame The frame at which to read the point primitive.
        \return Whether the static or animated data was read, or whether
        there was an error reading the data.
        */
        virtual unsigned int read(float iFrame);

        virtual const std::vector<float> & getPosition() { return mPosition; }
        virtual const std::vector<float> & getVelocity() { return mVelocity; }
        virtual const std::vector<int> & getParticleIds() { return mParticleIds; }
        virtual const std::vector<float> & getWidth() { return mWidth; }
        virtual float getConstantWidth() { return mConstantWidth; }

    protected:
        /**
        All of the information about the point primitive for the frame at which it was
        read.
        */
        std::vector<float> mPosition;
        std::vector<float> mVelocity;
        std::vector<int> mParticleIds;
        std::vector<float>  mWidth;
        float mConstantWidth;

    private:
        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}

using namespace TAKO_SPI_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_TakoSPI_PointPrimitiveReaderIMPL_h_
