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

#ifndef _Alembic_AbcCoreAbstract_ArrayPropertyWriter_h_
#define _Alembic_AbcCoreAbstract_ArrayPropertyWriter_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/BasePropertyWriter.h>
#include <Alembic/AbcCoreAbstract/ArraySample.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
//! An Array Property is a Rank N (usually 1-3) property which has a
//! multidimensional array of identically typed values for each
//! sample. This is distinguished from a Simple Property, which has a
//! single element per sample, and requires less sophisticated
//! resource management.
class ArrayPropertyWriter : public BasePropertyWriter
{
public:
    //! Virtual destructor
    //! ...
    virtual ~ArrayPropertyWriter();

    //-*************************************************************************
    // NEW FUNCTIONS
    //-*************************************************************************

    //! Sets a sample at a given sample index, with
    //! a given time. Depending on the time sampling type,
    //! the sampleTime may be ignored, or it may be checked for consistency
    //! to ensure synchronization.
    //!
    //! Samples must always be written starting at index 0, and
    //! moving incrementally forward, writing each subsequent index in order.
    //! An exception will be thrown if the samples are written out of order,
    //! or if the sample times are inconsistent.
    //!
    //! This takes a read-only ArraySample by const reference. The class
    //! will make an internal copy (or the functional equivalent of ),
    //! and will not use that memory block outside the scope of this
    //! function call.
    //!
    //! Arrays of std::string and std::wstring are assumed to be
    //! treated just like regular data elements.
    virtual void setSample( index_t iSampleIndex,
                            chrono_t iSampleTime,
                            const ArraySample & iSamp ) = 0;

    //! Set the next sample to equal the previous sample.
    //! An important feature!
    virtual void setFromPreviousSample( index_t iSampleIndex,
                                        chrono_t iSampleTime ) = 0;

    //! Return the number of samples that have been written so far.
    //! This changes as samples are written.
    virtual size_t getNumSamples() = 0;
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif
