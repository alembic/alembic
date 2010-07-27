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

#ifndef _Alembic_TakoSPI_TransformReader_h_
#define _Alembic_TakoSPI_TransformReader_h_

#include <Alembic/TakoSPI/Foundation.h>
#include <Alembic/TakoSPI/ReaderGlue.h>

namespace Alembic {

namespace TakoSPI {

namespace TAKO_SPI_LIB_VERSION_NS {

/**
\brief Node which holds a series of TransformOperation, and a list of child
nodes.

TransformReader is used to read transformational data and to traverse the node
hierarchy.
*/
class TransformReaderIMPL
    : public ReaderGlue<TakoAbstractV1::TransformReader>
{
    protected:
    friend class SceneReader;
        /**
        \brief Constructor used to open and read the root node of an HDF file.
        \param iName The name of the HDF file to read.

        Opens an HDF file and reads data off of the implicit root node "/".
        The root node does not have any transform operations but may have
        data in it's property map.
        */
        explicit TransformReaderIMPL(const std::string & iName);

        /**
        \brief Constructor used to read the node.
        \param iName The name of the node to read in as a transform.
        \param iParent The parent transform.
        */
        TransformReaderIMPL(const std::string & iName,
                            TransformReaderIMPL & iParent);

    public:

        /**
        \brief The destructor
        */
        virtual ~TransformReaderIMPL();

        /**
        \brief Reads the transform data.
        \param iFrame The sample to read.
        \return Hint on whether any sampled transform data was read.

        */
        virtual SampleType read(float iFrame);

        /**
        \brief Gets the sampled data for a particular frame.
        \param iFrame The sample to read.
        \param oVec The sampled transform data for that frame.
        \return Hint on whether any sampled transform data was read.

        Convenience function which fills oVec with sampled transform data, if it
        exists for a given frame.  This sampled data is only read into oVec, it
        does not change the transform data stored in the stack.
        */
        virtual SampleType getSamples(float iFrame,
                                      std::vector<double> & oVec);


        /**
        \brief Returns an iterator to the start of the transform operation
        stack.
        \return A const_stack_iterator.

        This function returns a const_stack_iterator to the start of the
        internal stack of transform operations.
        */
        virtual const_stack_iterator begin() const;

        /**
        \brief Returns an iterator to the end of the transform operation
        stack.
        \return A const_stack_iterator.

        This function returns a const_stack_iterator to the end of the
        internal stack of transform operations.
        */
        virtual const_stack_iterator end() const;

        /**
        \brief Calculates a 4x4 matrix based on the transorm operation stack.

        */
        virtual Mat4x4 getMatrix() const;

        /**
        \brief Return the number of children the transform node has.
        \return Number of child nodes the transform has.
        */
        virtual size_t numChildren() const;

        /**
        \brief Gets a child node at the specified index.
        \param iIndex The index of the child node.
        \return A shared ChildNodePtr.

        Gets a child node at the specified index, if index is beyond the
        range of available children, the returned shared pointer will be NULL.
        */
        virtual ChildNodePtr getChild(size_t iIndex);
    
        /**
        \brief Gets a child node at the specified index.
        \param iIndex The index of the child node.
        \return A shared ChildNodePtr.

        Gets a child node at the specified index, if index is beyond the
        range of available children, the returned shared pointer will be NULL.
        */
        virtual ChildNodePtr getChild(const std::string &iName );
     

        /**
        \brief Adds found frames for this transform to a set.
        \param ioSamples The set of frames to be added to.

        Looks for any frame samples that this transform may have and adds them
        to the set ioSamples.
        */
        virtual void getFrames(std::set<float>& ioSamples) const;

        /**
        \brief Returns whether or not this transform has any samples.
        \return true if this transform is sampled.
        */
        virtual bool hasFrames() const;

        /**
        \brief Indicates whether this transform should inherit parent
        transforms.
        \return A bool indicating inherit transform status.

        This flag does not actually effect the calculation of the matrix for
        this node as getMatrix only uses what is in the local transform stack.
        This flag is meant to be used when examining the matrix in conjunction
        with other transform nodes.
        */
        virtual bool inheritsTransform() const;

        /**
        \brief Returns the HDF5 handle for this transforms top-level group node.
        \return An HDF5 handle.

        This function was provided so that a handle could be easily passed
        as a parent to child nodes, and to allow indirect access to the HDF
        file handle.  Please use this function with care.
        */
        hid_t getHid() const;

    private:

        class PrivateData;
        boost::scoped_ptr< PrivateData > mData;
};
}

using namespace TAKO_SPI_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_TakoSPI_TransformReader_h_
