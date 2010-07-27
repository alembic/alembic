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

#ifndef _Alembic_AbcCoreAbstract_ObjectWriter_h_
#define _Alembic_AbcCoreAbstract_ObjectWriter_h_

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
class ObjectWriter
    : private boost::noncopyable,
      protected boost::enable_shared_from_this<ObjectWriter>
{
public:
    //! Virtual destructor
    //! ...
    virtual ~ObjectWriter();

    //-*************************************************************************
    // NEW FUNCTIONS
    //-*************************************************************************

    //! All objects have a name. This name is unique amongst their siblings
    //! Returned by reference, since it is guaranteed to exist and be
    //! unchanging. This is the name that was given when the object was
    //! created.
    virtual const std::string &getName() const = 0;

    //! The full name of an object is the complete path name all the way
    //! to the root object of the archive. It is guaranteed to be fully
    //! unique within the entire archive.
    virtual const std::string &getFullName() const = 0;

    //! All objects have metadata. This metadata is identical to the
    //! Metadata of the top level compoundProperty "properties".
    //! Because the metadata must exist and be initialized in order to
    //! bootstrap the object, it is guaranteed to exist and is returned
    //! by reference. While MetaData was required to create the object,
    //! additional MetaData may be appended during the writing of the
    //! object. This returns the accumulated MetaData, which may change
    //! as writing occurs. The reference will remain valid, but the
    //! MetaData it points to may change over time.
    virtual const MetaData &getMetaData() const = 0;
    
    //! All objects have a shared link to the root. This may seem
    //! wasteful, but it is essential in order to allow for the flexible,
    //! reference-counted autonomy of the writer objects. Alembic
    //! allows you to keep references to writers wherever you want,
    //! without requiring you to keep track of (or store) the parental
    //! hierarchy directly. In order to make this possible, we have
    //! the ability to walk upwards. This may be stored as a direct
    //! link, or retrieved by walking up the parent chain, which is
    //! a feature of the individual implementations. (it might not be
    //! cheap, basically).
    //! In order to prevent shared_ptr cycles, it is important
    //! that objects only store their children via weak ptrs.
    virtual RootWriterPtr getRoot() = 0;

    //! All objects have a shared link to their parent. This may seem
    //! wasteful, but it is essential in order to allow for the flexible,
    //! reference-counted autonomy of the writer objects. Alembic
    //! allows you to keep references to writers wherever you want,
    //! without requiring you to keep track of (or store) the parental
    //! hierarchy directly. In order to make this possible, we have
    //! the ability to walk upwards.
    //! In order to prevent shared_ptr cycles, it is important
    //! that objects only store their children via weak ptrs.
    virtual ObjectWriterPtr getParent() = 0;

    //! Append metadata to what already exists. Any new metadata must
    //! not overlap with existing metadata. An exception will be thrown
    //! if any MetaData overlaps. The MetaData class provides ample
    //! flexibility for detecting this error in advance.  Some metadata
    //! is guarenteed to exist when an object is created; there is
    //! no need for a "createMetaData" or "setMetaData" method.
    virtual void appendMetaData( const MetaData &iAppend ) = 0;

    //! All objects have a single compound property which is the
    //! root for any properties which are associated with this object.
    //! Guaranteed to exist, even if the compound property itself is
    //! empty. This may change as properties are added.
    virtual CompoundPropertyWriterPtr getProperties() = 0;

    //! Returns the number of objects that are contained as children.
    //! this number may increase (but not decrease) as objects are
    //! created. This is the number of children object writers that
    //! have ALREADY BEEN ADDED. It may increase.
    virtual size_t getNumChildren() = 0;

    //! Returns a pointer to an object writer at the given index,
    //! that has ALREADY BEEN ADDED. It is an error to request a child
    //! with an out of range index, and an exception will be thrown.
    //! In a strange case,
    //! this will return an empty pointer if the writer has been
    //! added in the past, but was closed (deleted). There is really
    //! no distinction between an empty weak pointer and a weak pointer
    //! to an expired object, so we simply return an empty pointer
    //! if the object had been created but no longer exists.
    virtual ObjectWriterPtr getChild( size_t i ) = 0;

    //! Return the name of a child object that has already been written.
    //! This is so you can check whether or not a name has already been
    //! used amongst the siblings, even if the child object writers have
    //! been deleted. If you ask for an out-of-range child, it will
    //! throw an exception, since that's a clear programming error.
    virtual std::string getChildName( size_t i ) = 0;

    //! Returns a pointer to an object writer of the given name iName,
    //! for an object that has ALREADY BEEN ADDED. This will return
    //! an empty pointer if no object of the given name has been added.
    virtual ObjectWriterPtr getChild( const std::string &iName ) = 0;

    //! Create an object with the given name and metadata.
    //! If an object with the given name already exists, an exception will
    //! be thrown, as this is a programming error.
    virtual ObjectWriterPtr createChild( const std::string &iName,
                                         const MetaData &iMetaData ) = 0;

    //! Returns shared pointer to myself.
    //! This is non-virtual
    ObjectWriterPtr asObject();
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif

