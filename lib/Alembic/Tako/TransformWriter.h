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

#ifndef _Alembic_Tako_TransformWriter_h_
#define _Alembic_Tako_TransformWriter_h_

#include <Alembic/Tako/Foundation.h>
#include <Alembic/Tako/HDFWriterNode.h>
#include <Alembic/Tako/TransformOperation.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {
/**
\brief TransformWriter is used to write transforms to the HDF file.
*/
class TransformWriter : public HDFWriterNode
{
    public:
        /**
        \brief Constructor for creating the initial root transform node.
        \param iName The name of the HDF file.

        Creates an HDF file, this will represent the root transform node.
        The root transform can not have a transform stack and no operations
        may be added or written.
        */
        explicit TransformWriter(const char * iName);

        /**
        \brief Constructor for writing transform data and parenting to another node.
        \param iName The name to give the transform group.
        \param iParent The TransformWriter which we will be parenting to.

        Creates an HDF group as a child of iParent with which we will be
        writing out transform data.  It will use iParent hid_t.
        */
        TransformWriter(const char * iName, TransformWriter & iParent);

        /** The destructor. */
        ~TransformWriter();

        /**
        \brief Adds a new transform operation to the vector of operations.
        \param iOp The trasform operation to add.

        Once writeTransformStack has been called, no more operations may
        be pushed on the stack.
        */
        void push(TransformOperationVariant iOp);

        /**
        \brief Clears out the vector of transform operations.
        */
        void clear();

        /**
        \brief Commits the transform stack along with the initial values stored
        to disk.
        \param iFrame The frame to use for any sampled data.
        \param inheritsTransform Used to determine how this nodes transformation
        stack is calculated in context of other transformat nodes.

        Write out the transform stack along with any sampled data at the given
        frame.  inheritsTransform only needs to be written if it is false.
        */
        void writeTransformStack(float iFrame, bool inheritsTransform);

        /**
        \brief A typedef for representing an iterator of the transform stack.

        An iterator that allows const access to TransformOperations in the
        transform stack.  This iterator dereferences to std::vector &lt
        TransformOperationVariant &gt::const_iterator.
        */
        typedef std::vector<TransformOperationVariant>::const_iterator
            const_stack_iterator;

        /**
        \brief Returns an iterator to the first transform operation.
        \return A const_stack_iterator.

        This function returns a const_stack_iterator to the start of the
        internal stack of transform operations.
        */
        const_stack_iterator begin() const;

        /**
        \brief Returns an iterator at the end of the transform operations.
        \return A const_iterator.

        This function returns a const_stack_iterator to the end of the
        internal stack of transform operations.
        */
        const_stack_iterator end() const;

        /**
        \brief Writes out sampled transformation data for a particular frame.
        \param iFrame The frame to write out the sampled data for.
        \param iSamples The samples data to write out.

        Writes out the provided sampled data for the specified frame.
        The size of iSamples must match the number of curves and sampled
        data that exist in the transform stack.
        */
        void writeSamples(float iFrame, const std::vector<double> & iSamples);

        /**
        \brief Get the concatenated matrix made up of all the transform
        operations in this transform.

        \return A 4x4 matrix representing the "local" space for this transform.
        */
        Mat4x4 getMatrix() const;

        /**
        \brief Get the HDF handle for the transforms base group.
        \return HDF handle.
        */
        hid_t getHid() const;
    private:
        // disallow copying
        TransformWriter(const TransformWriter &);
        void operator=(const TransformWriter &);

        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}

using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_TransformWriter_h_
