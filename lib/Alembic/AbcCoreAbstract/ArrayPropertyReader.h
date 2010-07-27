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

#ifndef _Alembic_AbcCoreAbstract_ArrayPropertyReader_h_
#define _Alembic_AbcCoreAbstract_ArrayPropertyReader_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/SimplePropertyReader.h>
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
class ArrayPropertyReader : public SimplePropertyReader
{
public:
    //! Virtual destructor
    //! ...
    virtual ~ArrayPropertyReader();

    //-*************************************************************************
    // NEW FUNCTIONS
    //-*************************************************************************

    //! It returns a shared pointer to a thing which _is_ the data, in a
    //! locked and retrieved form. This represents the point of demand,
    //! not below here.
    //! Implementations of this library can (and should) utilize the
    //! custom-deleter capabilities of the smart_ptr to add locking and
    //! unlocking access to cache or management code.
    //! It will throw an exception on an out-of-range access.
    virtual ArraySamplePtr getSample( index_t iSample ) = 0;

    //-*************************************************************************
    // INHERITED
    //-*************************************************************************
    
    //! Returns kArrayProperty
    //! ...
    virtual PropertyType getPropertyType() const;

    //! Returns this as a properly cast shared_ptr.
    //! ...
    virtual SimplePropertyReaderPtr asSimple();

    //! Returns this as a properly cast shared_ptr.
    //! ...
    virtual ArrayPropertyReaderPtr asArray();

    //-*************************************************************************

    //! Inherited from SimplePropertyReader
    //! No implementation herein
    //! virtual const DataType &getDataType() const = 0;

    //! Inherited from SimplePropertyReader
    //! No implementation herein
    //! virtual const TimeSamplingType &getTimeSamplingType() const = 0;

    //! Inherited from SimplePropertyReader
    //! No implementation herein
    //! virtual size_t getNumSamples() = 0;

    //-*************************************************************************
    
    //! Inherited from BasePropertyReader
    //! No implementation herein
    //! virtual const std::string &getName() const = 0;

    //! Inherited from BasePropertyReader
    //! bool isScalar() const;

    //! Inherited from BasePropertyReader
    //! bool isArray() const;

    //! Inherited from BasePropertyReader
    //! bool isCompound() const;

    //! Inherited from BasePropertyReader
    //! bool isSimple() const;

    //! Inherited from BasePropertyReader
    //! No implementation herein.
    //! virtual const MetaData &getMetaData() const = 0;
    
    //! Inherited from BasePropertyReader
    //! No implementation herein.
    //! virtual ObjectReaderPtr getObject() = 0;

    //! Inherited from BasePropertyReader
    //! No implementation herein.
    //! virtual CompoundPropertyReaderPtr getParent() = 0;
    
    //! Inherited from BasePropertyReader
    //! No implementation herein.
    //! virtual ScalarPropertyReaderPtr asScalar();
    
    //! Inherited from BasePropertyReader
    //! No implementation herein.
    //! virtual CompoundPropertyReaderPtr asCompound();
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic



#endif
