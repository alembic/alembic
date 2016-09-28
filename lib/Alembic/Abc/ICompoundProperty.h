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

#ifndef _Alembic_Abc_ICompoundProperty_h_
#define _Alembic_Abc_ICompoundProperty_h_

#include <Alembic/Util/Export.h>
#include <Alembic/Abc/Foundation.h>
#include <Alembic/Abc/Argument.h>
#include <Alembic/Abc/Base.h>
#include <Alembic/Abc/IBaseProperty.h>

namespace Alembic {
namespace Abc {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ALEMBIC_EXPORT ICompoundProperty
    : public IBasePropertyT<AbcA::CompoundPropertyReaderPtr>
{
public:
    //! By convention we always define this_type in Abc classes
    //! Used by unspecified-bool-type conversion below
    typedef ICompoundProperty this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty ICompoundProperty function set.
    //! ...
    ICompoundProperty() : IBasePropertyT<AbcA::CompoundPropertyReaderPtr>() {}

    //! This constructor creates a new compound property reader.
    //! The first argument is the parent ICompundProperty, from which the error
    //! handler policy for inheritance is also derived.  The remaining optional
    //! arguments can be used to override the ErrorHandlerPolicy, and that's it.
    ICompoundProperty( const ICompoundProperty & iParent,
                       const std::string & iName,
                       const Argument & iArg0 = Argument() );

    //! This attaches an ICompoundProperty wrapper around an existing
    //! CompoundPropertyReaderPtr, with an optional error handling policy.
    //!
    //! The extra argument is to support ISchema, which is publicly derived
    //! from ICompoundProperty (see ISchema.h).
    ICompoundProperty(
        AbcA::CompoundPropertyReaderPtr iPtr,
        const Argument &iArg0 = Argument(),
        const Argument &iArg1 = Argument() );

    // Deprecated in favor of the constructor above
    ICompoundProperty(
        AbcA::CompoundPropertyReaderPtr iPtr,
        WrapExistingFlag /* iWrapFlag */,
        const Argument &iArg0 = Argument(),
        const Argument &iArg1 = Argument() );

    //! This attaches an ICompoundProperty wrapper around the top
    //! properties of any object, with an optional error handling policy.
    //!
    //! The extra argument is to support ISchema, which is publicly derived
    //! from ICompoundProperty (see ISchema.h).
    ICompoundProperty(
        const IObject & iObject,
        const Argument &iArg0 = Argument(),
        const Argument &iArg1 = Argument() );

    // Deprecated in favor of the constructor above
    ICompoundProperty(
        const IObject & iObject,
        TopFlag /* iTopFlag */,
        const Argument &iArg0 = Argument(),
        const Argument &iArg1 = Argument() );

    //! Default copy constructor used
    //! Default assignment operator used.

    //! Destructor
    //! ...
    ~ICompoundProperty();

    //-*************************************************************************
    // COMPOUND PROPERTY READER FUNCTIONALITY
    //-*************************************************************************

    //! Returns the number of properties contained in this ICompoundProperty
    size_t getNumProperties() const;

    //! Return the header of a child property.
    //! Property is selected by index.
    //! This will throw an exception on out-of-range access.
    const AbcA::PropertyHeader & getPropertyHeader( size_t i ) const;

    //! Return the header of a property name.
    //! This will return a NULL pointer if no header by that name is found.
    const AbcA::PropertyHeader *
    getPropertyHeader( const std::string &iName ) const;

    //! There is no distinction between already added properties
    //! and created properties with an AbcA::CompoundPropertyReader, therefore
    //! we have no need to expose "getProperty". Simply use the appropriate
    //! IScalarProperty, ICompoundProperty, or IArrayProperty
    //! wrappers.

    //! Return the parent compound property, handily wrapped in a
    //! ICompoundProperty wrapper.
    ICompoundProperty getParent() const;

private:

    void init ( const IObject & iObject,
                const Argument &iArg0,
                const Argument &iArg1 );

    void init( AbcA::CompoundPropertyReaderPtr iParentObject,
               const std::string &iName,

               ErrorHandler::Policy iParentPolicy,

               const Argument &iArg0 );
};

//-*****************************************************************************
inline AbcA::CompoundPropertyReaderPtr
GetCompoundPropertyReaderPtr
( ICompoundProperty &iPrp ) { return iPrp.getPtr(); }

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Abc
} // End namespace Alembic

#endif
