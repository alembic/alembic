//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks Inc. and
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
// Industrial Light & Magic, nor the names of their contributors may be used
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

#ifndef _Alembic_AbcCoreAbstract_ObjectReader_h_
#define _Alembic_AbcCoreAbstract_ObjectReader_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/ForwardDeclarations.h>
#include <Alembic/AbcCoreAbstract/MetaData.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
//! An Object consists of a list of children objects, which may be empty,
//! and a single compound property which is the root of any properties
//! which the object may contain.
//! Objects have MetaData, which is identical to the MetaData of the root
//! Compound Property.
class ObjectReader
    : private boost::noncopyable,
      public boost::enable_shared_from_this<ObjectReader>
{
public:
    //! Virtual destructor
    //! ...
    virtual ~ObjectReader();

    //-*************************************************************************
    // NEW FUNCTIONS
    //-*************************************************************************

    //! All objects have a name. This name is unique amongst their siblings
    //! Returned by reference, since it is guaranteed to exist and be
    //! unchanging.
    virtual const std::string &getName() const = 0;
    
    //! The full name of an object is the complete path name all the way
    //! to the root object of the archive. It is guaranteed to be fully
    //! unique within the entire archive.
    virtual const std::string &getFullName() const = 0;

    //! All objects have metadata. This metadata is identical to the
    //! Metadata of the top level compoundProperty "properties".
    //! Because the metadata must exist and be initialized in order to
    //! bootstrap the object, it is guaranteed to exist and is returned
    //! by reference.
    virtual const MetaData &getMetaData() const = 0;

    //! All objects have a shared link to the root. This may seem
    //! wasteful, but it is essential in order to allow for the flexible,
    //! reference-counted autonomy of the reader objects. Alembic
    //! allows you to keep references to readers wherever you want,
    //! without requiring you to keep track of (or store) the parental
    //! hierarchy directly. In order to make this possible, we have
    //! the ability to walk upwards. This may be stored as a direct
    //! link, or retrieved by walking up the parent chain, which is
    //! a feature of the individual implementations. (it might not be
    //! cheap, basically).
    //! In order to prevent shared_ptr cycles, it is important
    //! that objects only store their children via weak ptrs.
    virtual RootReaderPtr getRoot() = 0;

    //! All objects have a shared link to their parent. This may seem
    //! wasteful, but it is essential in order to allow for the flexible,
    //! reference-counted autonomy of the reader objects. Alembic
    //! allows you to keep references to readers wherever you want,
    //! without requiring you to keep track of (or store) the parental
    //! hierarchy directly. In order to make this possible, we have
    //! the ability to walk upwards.
    //! In order to prevent shared_ptr cycles, it is important
    //! that objects only store their children via weak ptrs.
    virtual ObjectReaderPtr getParent() = 0;
    
    //! All objects have one and only one compound property which
    //! is the root for any properties which are associated with this object.
    //! Guaranteed to exist, even if the compound property itself is empty.
    virtual CompoundPropertyReaderPtr getProperties() = 0;

    //! Returns the number of objects that are contained as children.
    //! Objects do not have to have children, this may return zero.
    virtual size_t getNumChildren() = 0;

    //! Return a pointer to the object reader corresponding to the child
    //! at index i.
    //! It is an error to ask for an out-of-range child, and it will throw
    //! an exception in this case.
    virtual ObjectReaderPtr getChild( size_t i ) = 0;

    //! This function is somewhat unnecessary. ObjectReaders by themselves,
    //! before any children or properties have been read, are extremely
    //! lightweight structures consisting of a name and metadata, and that's
    //! about it. So, asking for the child and getting its name is basically
    //! the same as this function. However, in the interest of maintaining
    //! parity with the writer where possible, we include this function.
    //! No such function for metadata exists, if that is required, get
    //! the object reader.
    //! It is an error to call this out of range, and an exception will be
    //! thrown.
    virtual std::string getChildName( size_t i ) = 0;

    //! Return a pointer to the object reader corresponding to the child
    //! with the given name iName
    //! Will return an empty pointer if not found.
    virtual ObjectReaderPtr getChild( const std::string & iName ) = 0;

    //! Returns shared pointer to myself.
    //! This is non-virtual
    ObjectReaderPtr asObject();
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif


