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

#ifndef _Alembic_AbcCoreAbstract_ScalarPropertyReader_h_
#define _Alembic_AbcCoreAbstract_ScalarPropertyReader_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/SimplePropertyReader.h>
#include <Alembic/AbcCoreAbstract/ReadOnlyData.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
//! A Scalar Property is a Rank 0 property which has a single value for each
//! sample. This is distinguished from an Array Property, which has a
//! variable number of elements per sample, and requires more sophisticated
//! resource management.
class ScalarPropertyReader : public SimplePropertyReader
{
public:
    //! Virtual destructor
    //! ...
    virtual ~ScalarPropertyReader();

    //-*************************************************************************
    // NEW FUNCTIONS
    //-*************************************************************************

    //! Returns the single sample value for the requested sample
    //! as a shared pointer, which may have internal resource management.
    //! It will throw an exception if you request an out of range sample.
    //! If it cannot load the sample, and empty sample is returned.
    virtual ReadOnlyBytesPtr getSample( index_t iSample ) = 0;

    //-*************************************************************************
    // INHERITED
    //-*************************************************************************
    
    //! Returns kScalarProperty
    //! ...
    virtual PropertyType getPropertyType() const;

    //! Returns this as a properly cast shared_ptr.
    //! ...
    virtual SimplePropertyReaderPtr asSimple();

    //! Returns this as a properly cast shared_ptr.
    //! ...
    virtual ScalarPropertyReaderPtr asScalar();

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
    //! There's actually no enum for kSimpleProperty, since
    //! this is just an intermediate type
    //! virtual PropertyType getPropertyType() const = 0;

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
    //! virtual ArrayPropertyReaderPtr asArray();
    
    //! Inherited from BasePropertyReader
    //! No implementation herein.
    //! virtual CompoundPropertyReaderPtr asCompound();
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic



#endif
