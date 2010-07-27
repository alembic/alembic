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

#ifndef _Alembic_AbcCoreAbstract_CompoundPropertyWriter_h_
#define _Alembic_AbcCoreAbstract_CompoundPropertyWriter_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/BasePropertyWriter.h>
#include <Alembic/AbcCoreAbstract/ForwardDeclarations.h>
#include <Alembic/AbcCoreAbstract/DataType.h>
#include <Alembic/AbcCoreAbstract/MetaData.h>
#include <Alembic/AbcCoreAbstract/TimeSamplingType.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
//! A CompoundProperty is a group of other properties, possibly Simple
//! or possibly Compound. Every object has at one of these.
class CompoundPropertyWriter : public BasePropertyWriter
{
public:
    //! Virtual destructor
    //! ...
    virtual ~CompoundPropertyWriter();

    //-*************************************************************************
    // NEW FUNCTIONS
    //-*************************************************************************

    //! Returns the number of properties that have been created thus far.
    //! May change as more are created.
    virtual size_t getNumProperties() = 0;

    //! It is an error to request for a property by index out of range.
    //! This returns a property that has ALREADY BEEN ADDED.
    //! This will throw an exception on out-of-range access.
    //! There is a possibility it could return a NULL pointer, if the
    //! added property has been closed (deleted).
    virtual BasePropertyWriterPtr getProperty( size_t i ) = 0;
    
    //! Return the name of a sub property that has already been written.
    //! This is so you can check whether or not a name has already been
    //! used amongst the siblings, even if the sub property writers have
    //! been deleted. If you ask for an out-of-range property, it will
    //! throw an exception, since that's a clear programming error.
    virtual std::string getPropertyName( size_t i ) = 0;

    //! Returns an ALREADY ADDED PROPERTY by name. If it can't find
    //! one by name, it returns an empty pointer. This can also happen
    //! if the property was added, but has been closed (deleted).
    virtual BasePropertyWriterPtr getProperty( const std::string & iName ) = 0;

    //! Create and return the requested scalar property.
    //! If a property already exists with the same name, throws
    //! an exception.
    virtual ScalarPropertyWriterPtr
    createScalarProperty( const std::string & iName,
                          const DataType & iDataType,
                          const MetaData & iMetaData,
                          const TimeSamplingType & iTimeSamplingType ) = 0;
    
    //! Create and return the requested array property.
    //! If a property already exists with the same name, throws.
    //! an exception.
    virtual ArrayPropertyWriterPtr
    createArrayProperty( const std::string & iName,
                         const DataType & iDataType,
                         const MetaData & iMetaData,
                         const TimeSamplingType &iTimeSamplingType ) = 0;
    
    //! Create and return the requested compound property.
    //! If a property already exists with the same name, throws
    //! an exception.
    virtual CompoundPropertyWriterPtr
    createCompoundProperty( const std::string & iName,
                            const MetaData & iMetaData ) = 0;

    //-*************************************************************************
    // INHERITED
    //-*************************************************************************
    
    //! Returns kCompoundProperty
    //! ...
    virtual PropertyType getPropertyType() const;

    //! Returns this as a properly cast shared_ptr.
    //! ...
    virtual CompoundPropertyWriterPtr asCompound();

    //-*************************************************************************
    
    //! Inherited from BasePropertyWriter
    //! No implementation herein
    //! virtual const std::string &getName() const = 0;

    //! Inherited from BasePropertyWriter
    //! bool isScalar() const;

    //! Inherited from BasePropertyWriter
    //! bool isArray() const;

    //! Inherited from BasePropertyWriter
    //! bool isCompound() const;

    //! Inherited from BasePropertyWriter
    //! bool isSimple() const;

    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual const MetaData &getMetaData() const = 0;
    
    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual ObjectWriterPtr getObject() = 0;

    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual CompoundPropertyWriterPtr getParent() = 0;
    
    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual void appendMetaData( const MetaData &iAppend ) = 0;
    
    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual SimplePropertyWriterPtr asSimple();
    
    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual ScalarPropertyWriterPtr asScalar();
    
    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual ArrayPropertyWriterPtr asArray();
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif

