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
#ifndef _Alembic_Abc_OArrayProperty_h_
#define _Alembic_Abc_OArrayProperty_h_

#include <Alembic/Abc/Foundation.h>
#include <Alembic/Abc/Base.h>
#include <Alembic/Abc/OArgument.h>
#include <Alembic/Abc/OSampleSelector.h>
#include <Alembic/Abc/OBaseProperty.h>
#include <Alembic/Abc/OCompoundProperty.h>

namespace Alembic {
namespace Abc {

//-*****************************************************************************
class OArrayProperty
    : public OBasePropertyT<AbcA::ArrayPropertyWriterPtr>
{
public:
    //! By convention we always define this_type in Abc classes
    //! Used by unspecified-bool-type conversion below
    typedef OArrayProperty this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OArrayProperty function set.
    //! ...
    OArrayProperty() : OBasePropertyT<AbcA::ArrayPropertyWriterPtr>() {}

    //! This templated, explicit function creates a new scalar property writer.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to a CompoundPropertyWriterPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, and to specify time sampling type.
    template <class OBJECT_PTR>
    OArrayProperty( OBJECT_PTR iParentObject,
                    const std::string &iName,
                    const AbcA::DataType &iDataType,

                    const OArgument &iArg0 = OArgument(),
                    const OArgument &iArg1 = OArgument(),
                    const OArgument &iArg2 = OArgument() );

    //! This attaches an OArrayProperty wrapper around an existing
    //! ArrayPropertyWriterPtr, with an optional error handling policy.
    OArrayProperty(
        //! The pointer
        //! ...
        AbcA::ArrayPropertyWriterPtr iPtr,

        //! The flag indicating that wrapping is intended.
        //! Even though it's nonambiguous here, we use it anyway
        //! for readability
        WrapExistingFlag iWrapFlag,

        //! Optional error handling policy
        //! ...
        ErrorHandler::Policy iPolicy = ErrorHandler::kThrowPolicy )
      : OBasePropertyT<AbcA::ArrayPropertyWriterPtr>( iPtr,
                                                      iWrapFlag,
                                                      iPolicy ) {}

    //! Default copy constructor used
    //! Default assignment operator used.

    //! Destructor
    //! ...
    ~OArrayProperty();

    //-*************************************************************************
    // ARRAY PROPERTY WRITER FUNCTIONALITY
    //-*************************************************************************

    //! Get the number of samples written so far.
    //! May change over time as more samples are written.
    size_t getNumSamples();

    //! Set a sample from the address of a datum.
    //! ...
    void set( const AbcA::ArraySample &iSample,
              const OSampleSelector &iSS = OSampleSelector() );

    //! Set a sample from the previous sample.
    //! ...
    void setFromPrevious( const OSampleSelector &iSS );

    //! Return the parent compound property, handily wrapped in a
    //! OCompoundProperty wrapper.
    OCompoundProperty getParent();

private:
    void init( AbcA::CompoundPropertyWriterPtr iParentObject,
               const std::string &iName,
               const AbcA::DataType &iDataType,

               ErrorHandler::Policy iParentPolicy,

               const OArgument &iArg0,
               const OArgument &iArg1,
               const OArgument &iArg2 );
};

//-*****************************************************************************
// TEMPLATE AND INLINE FUNCTIONS
//-*****************************************************************************

//-*****************************************************************************
template <class CPROP_PTR>
inline OArrayProperty::OArrayProperty( CPROP_PTR iParentProp,
                                       const std::string &iName,
                                       const AbcA::DataType &iDataType,
                                       const OArgument &iArg0,
                                       const OArgument &iArg1,
                                       const OArgument &iArg2 )
{
    init( GetCompoundPropertyWriterPtr( iParentProp ),
          iName, iDataType,

          GetErrorHandlerPolicy( iParentProp ),
          iArg0, iArg1, iArg2 );
}

} // End namespace Abc
} // End namespace Alembic

#endif
