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

#ifndef _Alembic_Abc_OCompoundProperty_h_
#define _Alembic_Abc_OCompoundProperty_h_

#include <Alembic/Util/Export.h>
#include <Alembic/Abc/Foundation.h>
#include <Alembic/Abc/Argument.h>
#include <Alembic/Abc/Base.h>
#include <Alembic/Abc/OBaseProperty.h>

namespace Alembic {
namespace Abc {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ALEMBIC_EXPORT OCompoundProperty
    : public OBasePropertyT<AbcA::CompoundPropertyWriterPtr>
{
public:
    //! By convention we always define this_type in Abc classes
    //! Used by unspecified-bool-type conversion below
    typedef OCompoundProperty this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OCompoundProperty function set.
    //! ...
    OCompoundProperty() : OBasePropertyT<AbcA::CompoundPropertyWriterPtr>() {}


    //! Create a new OCompoundProperty named iName as a child of iParent.
    //! The remaining optional arguments can be used to override the
    //! ErrorHandlerPolicy, or to specify MetaData.
    OCompoundProperty( AbcA::CompoundPropertyWriterPtr iParent,
                       const std::string &iName,
                       const Argument &iArg0 = Argument(),
                       const Argument &iArg1 = Argument() );

    //! Create a new OCompoundProperty named iName as a child of iParent.
    //! The remaining optional arguments can be used to override the
    //! ErrorHandlerPolicy, or to specify MetaData.
    OCompoundProperty( OCompoundProperty iParent,
                       const std::string &iName,
                       const Argument &iArg0 = Argument(),
                       const Argument &iArg1 = Argument() );

    //! This attaches an OCompoundProperty wrapper around an existing
    //! CompoundPropertyWriterPtr, with an optional error handling policy.
    OCompoundProperty(
        AbcA::CompoundPropertyWriterPtr iProp,
        const Argument &iArg0 = Argument(),
        const Argument &iArg1 = Argument() );

    // Deprecated in favor of the constructor above
    OCompoundProperty(
        AbcA::CompoundPropertyWriterPtr iProp,
        WrapExistingFlag iWrapFlag = kWrapExisting,
        const Argument &iArg0 = Argument(),
        const Argument &iArg1 = Argument() );

    //! This attaches an OCompoundProperty wrapper around the top
    //! properties of an OObject.
    OCompoundProperty(
        OObject iObject,
        const Argument &iArg0 = Argument(),
        const Argument &iArg1 = Argument() );

    // Deprecated in favor of the constructor above
    OCompoundProperty(
        OObject iObject,
        TopFlag iTopFlag = kTop,
        const Argument &iArg0 = Argument(),
        const Argument &iArg1 = Argument() );

    //! Default copy constructor used
    //! Default assignment operator used.

    //! Destructor
    //! ...
    ~OCompoundProperty();

    //-*************************************************************************
    // COMPOUND PROPERTY WRITER FUNCTIONALITY
    //-*************************************************************************

    //! Returns the number of properties that have been created thus far.
    //! May change as more are created.
    size_t getNumProperties() const;

    //! Return the header of a property that has already been added.
    //! Property is selected by index.
    //! This will throw an exception on out-of-range access.
    const AbcA::PropertyHeader & getPropertyHeader( size_t i ) const;

    //! Return the header of a property that has already been added.
    //! Property is selected by name.
    //! This will return NULL if no property with the given name is found.
    const AbcA::PropertyHeader * getPropertyHeader( const std::string &iName ) const;

    //! Return a wrapped base property version of an ALREADY ADDED property.
    //! Will return an empty pointer if the writer for this property
    //! no longer exists or was never added.
    //! This would primarily be used for introspection.
    OBaseProperty getProperty( size_t i ) const;

    //! Return a wrapped base property version of an ALREADY ADDED property.
    //! Will return an empty pointer if the writer for this property
    //! no longer exists or was never added.
    //! This would primarily be used for introspection.
    OBaseProperty getProperty( const std::string &iName ) const;

    //! Return the parent compound property, handily wrapped in a
    //! OCompoundProperty wrapper.
    OCompoundProperty getParent() const;

private:

    void init( OObject iObject, const Argument &iArg0, const Argument &iArg1 );

    void init( AbcA::CompoundPropertyWriterPtr iParentObject,
               const std::string &iName,
               const Argument &iArg0,
               const Argument &iArg1,
               const Argument &iArg2 = Argument() );
};

//-*****************************************************************************
inline AbcA::CompoundPropertyWriterPtr
GetCompoundPropertyWriterPtr
( OCompoundProperty iPrp ) { return iPrp.getPtr(); }

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Abc
} // End namespace Alembic

#endif
