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

#ifndef _Alembic_AbcCoreAbstract_CompoundPropertyReader_h_
#define _Alembic_AbcCoreAbstract_CompoundPropertyReader_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/BasePropertyReader.h>
#include <Alembic/AbcCoreAbstract/ForwardDeclarations.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
//! A CompoundProperty is a group of other properties, possibly Simple
//! or possibly Compound. Every object has at one of these.
class CompoundPropertyReader : public BasePropertyReader
{
public:
    //! Virtual destructor
    //! ...
    virtual ~CompoundPropertyReader();

    //-*************************************************************************
    // NEW FUNCTIONS
    //-*************************************************************************

    //! Returns the number of properties read from the file
    //! ...
    virtual size_t getNumProperties() = 0;

    //! Returns a property by index.
    //! It is an error to call it with an out-of-range index, and an
    //! exception will be thrown
    virtual BasePropertyReaderPtr getProperty( size_t i ) = 0;

    //! Get the name of a property by index.
    //! ...
    virtual std::string getPropertyName( size_t i ) = 0;

    //! Get a property by name.
    //! That property can be safely upcast.
    virtual BasePropertyReaderPtr getProperty( const std::string &iName ) = 0;

    //-*************************************************************************
    // INHERITED
    //-*************************************************************************
    
    //! Returns kCompoundProperty
    //! ...
    virtual PropertyType getPropertyType() const;

    //! Returns this as a properly cast shared_ptr.
    //! ...
    virtual CompoundPropertyReaderPtr asCompound();

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
    //! virtual SimplePropertyReaderPtr asSimple();
    
    //! Inherited from BasePropertyReader
    //! No implementation herein.
    //! virtual ScalarPropertyReaderPtr asScalar();
    
    //! Inherited from BasePropertyReader
    //! No implementation herein.
    //! virtual ArrayPropertyReaderPtr asArray();
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif
