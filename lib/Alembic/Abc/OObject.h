//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#ifndef _Alembic_Abc_OObject_h_
#define _Alembic_Abc_OObject_h_

#include <Alembic/Util/Export.h>
#include <Alembic/Abc/Foundation.h>
#include <Alembic/Abc/Base.h>
#include <Alembic/Abc/Argument.h>
#include <Alembic/Abc/OArchive.h>

namespace Alembic {
namespace Abc {
namespace ALEMBIC_VERSION_NS {

class OCompoundProperty;

//-*****************************************************************************
class ALEMBIC_EXPORT OObject : public Base
{
public:
    //! By convention, we always define "this_type" in every Abc
    //! class. This convention is relied upon by the unspecified-bool-type
    //! conversion.
    typedef OObject this_type;
    typedef OObject operator_bool_base_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OObject function set.
    //! ...
    OObject() {}

    //! This creates a new object writer.
    //! The first argument is the parent OObject from which the error handler
    //! policy for inheritance is derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, and MetaData
    OObject( OObject iParentObject,
             const std::string &iName,

             const Argument &iArg0 = Argument(),
             const Argument &iArg1 = Argument(),
             const Argument &iArg2 = Argument() );

    //! This attaches an OObject wrapper around an existing
    //! ObjectWriterPtr, with an optional error handling policy.
    //! Arguments can be used to set error handling policy.
    OObject( AbcA::ObjectWriterPtr iPtr,
             const Argument &iArg0 = Argument(),
             const Argument &iArg1 = Argument(),
             const Argument &iArg2 = Argument() )
      : m_object( iPtr )
    {
        // Set the error handling policy
        getErrorHandler().setPolicy(
            GetErrorHandlerPolicy( iPtr, iArg0, iArg1, iArg2 ) );
    }

    // Deprecated in favor of the constructor above
    OObject( AbcA::ObjectWriterPtr iPtr,
             WrapExistingFlag /* iFlag */,
             const Argument &iArg0 = Argument(),
             const Argument &iArg1 = Argument(),
             const Argument &iArg2 = Argument() )
      : m_object( iPtr )
    {
        // Set the error handling policy
        getErrorHandler().setPolicy(
            GetErrorHandlerPolicy( iPtr, iArg0, iArg1, iArg2 ) );
    }

    //! This attaches an OObject wrapper around the top
    //! object in an archive.
    //! Arguments can be used to set error handling policy.
    OObject( OArchive & iArchive,
             const Argument &iArg0 = Argument(),
             const Argument &iArg1 = Argument(),
             const Argument &iArg2 = Argument() )
    {
        init( iArchive, iArg0, iArg1, iArg2 );
    }

    // deprecated in favor of the constructor above
    OObject( OArchive & iArchive,
             TopFlag /* iTop */,
             const Argument &iArg0 = Argument(),
             const Argument &iArg1 = Argument(),
             const Argument &iArg2 = Argument() )
    {
        init( iArchive, iArg0, iArg1, iArg2 );
    }

    //! Default copy constructor used
    //! Default assignment operator used.

    //! Destructor
    //! ...
    virtual ~OObject();

    //-*************************************************************************
    // OBJECT WRITER FUNCTIONALITY
    //-*************************************************************************

    //! Return the object's header.
    //! ...
    const AbcA::ObjectHeader & getHeader() const;

    //! This function returns the object's local name
    //! ...
    const std::string &getName() const
    { return getHeader().getName(); }

    //! This function returns the object's full (unique with the archive)
    //! name
    const std::string &getFullName() const
    { return getHeader().getFullName(); }

    //! This function returns the object's metadata.
    //! ...
    const AbcA::MetaData &getMetaData() const
    { return getHeader().getMetaData(); }

    //! This function returns the object's archive, handily
    //! wrapped in an OArchive wrapper.
    OArchive getArchive();

    //! This function returns the object's parent, handily
    //! wrapped in an OObject wrapper. If the object is the top
    //! level object, the OObject returned will be NULL.
    OObject getParent();

    //! This function returns the number of child objects that
    //! this object has. This may change as new children
    //! are created for writing.
    size_t getNumChildren();

    //! This function returns the headers of each of the child
    //! objects created so far.
    const AbcA::ObjectHeader & getChildHeader( size_t i );

    //! This function returns the header of a named object if it has
    //! been created - even if the object no longer exists.
    const AbcA::ObjectHeader * getChildHeader( const std::string &iName );

    //! This returns the single top-level OCompoundProperty that exists
    //! automatically as part of the object.
    OCompoundProperty getProperties();

    //-*************************************************************************
    // ADVANCED TOOLS
    // Unless you really know why you need to be using these next few
    // functions, they're probably best left alone. These functions attempt
    // to find a pointer to an existing writer, instead of creating a new one.
    //-*************************************************************************

    //! This function returns an OObject wrapped around a pointer
    //! to an already created child. This is distinct from creating a new
    //! OObject as a child. If the writer associated with this child no longer
    //! exists, this function will return an empty OObject.
    OObject getChild( size_t iChildIndex );

    //! This function returns an OObject wrapped around a pointer
    //! to an already created child. This is distinct from creating a new
    //! OObject as a child. If the writer associated with this child no longer
    //! exists, this function will return an empty OObject.
    OObject getChild( const std::string &iChildName );

    //!-************************************************************************
    // PROXY METHODS
    // An OObject can refer to another OObject. When read in, this instance
    // will be represented by the target source hierarchy and its children.
    //!-************************************************************************
    bool addChildInstance( OObject iTarget, const std::string& iName );

    //-*************************************************************************
    // ABC BASE MECHANISMS
    // These functions are used by Abc to deal with errors, rewrapping,
    // and so on.
    //-*************************************************************************

    //! getPtr, as usual, returns a shared ptr to the
    //! underlying AbcCoreAbstract object, in this case the
    //! ObjectWriterPtr.
    AbcA::ObjectWriterPtr getPtr() { return m_object; }

    //! Reset returns this function set to an empty, default
    //! state.
    void reset() { m_object.reset(); Base::reset(); }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( Base::valid() && m_object );
    }

    //! The unspecified-bool-type operator casts the object to "true"
    //! if it is valid, and "false" otherwise.
    ALEMBIC_OPERATOR_BOOL( valid() );

private:

    void init( OArchive & iArchive,
               const Argument &iArg0,
               const Argument &iArg1,
               const Argument &iArg2 );

    void init( AbcA::ObjectWriterPtr iParentObject,
               const std::string &iName,
               ErrorHandler::Policy iParentPolicy,

               const Argument &iArg0,
               const Argument &iArg1,
               const Argument &iARg2 );

protected:
    AbcA::ObjectWriterPtr m_object;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Abc
} // End namespace Alembic

#endif
