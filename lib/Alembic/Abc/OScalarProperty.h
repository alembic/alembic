//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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
#ifndef _Alembic_Abc_OScalarProperty_h_
#define _Alembic_Abc_OScalarProperty_h_

#include <Alembic/Util/Export.h>
#include <Alembic/Abc/Foundation.h>
#include <Alembic/Abc/Base.h>
#include <Alembic/Abc/Argument.h>
#include <Alembic/Abc/OBaseProperty.h>
#include <Alembic/Abc/OCompoundProperty.h>

namespace Alembic {
namespace Abc {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ALEMBIC_EXPORT OScalarProperty
    : public OBasePropertyT<AbcA::ScalarPropertyWriterPtr>
{
public:
    //! By convention we always define this_type in Abc classes
    //! Used by unspecified-bool-type conversion below
    typedef OScalarProperty this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OScalarProperty function set.
    //! ...
    OScalarProperty() : OBasePropertyT<AbcA::ScalarPropertyWriterPtr>() {}

    //! This templated, explicit function creates a new scalar property writer.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to a CompoundPropertyWriterPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, and to specify time sampling or time sampling index.
    template <class OBJECT_PTR>
    OScalarProperty( OBJECT_PTR iParentObject,
                     const std::string &iName,
                     const AbcA::DataType &iDataType,

                     const Argument &iArg0 = Argument(),
                     const Argument &iArg1 = Argument(),
                     const Argument &iArg2 = Argument() );

    //! This attaches an OScalarProperty wrapper around an existing
    //! ScalarPropertyWriterPtr, arguments are there to specify
    //! ErrorHandling policy,
    OScalarProperty(

        //! The pointer
        //! ...
        AbcA::ScalarPropertyWriterPtr iPtr,

        //! The flag indicating that wrapping is intended.
        //! Even though it's nonambiguous here, we use it anyway
        //! for readability
        WrapExistingFlag iWrapFlag,

        //! Error Handling Policy
        //! ..
        ErrorHandler::Policy iPolicy = ErrorHandler::kThrowPolicy )
      : OBasePropertyT<AbcA::ScalarPropertyWriterPtr>( iPtr,
                                                       iWrapFlag,
                                                       iPolicy ) {}

    //! Default copy constructor used
    //! Default assignment operator used.

    //! Destructor
    //! ...
    ~OScalarProperty();

    //-*************************************************************************
    // SCALAR PROPERTY WRITER FUNCTIONALITY
    //-*************************************************************************

    //! Return the number of samples contained in the property.
    //! This can be any number, including zero.
    //! This returns the number of samples that were written, independently
    //! of whether or not they were constant.
    //! It make change over time as more samples are written.
    size_t getNumSamples() const;

    //! Set a sample from the address of a datum.
    //! ...
    void set( const void *iSample );

    //! Set a sample from the previous sample.
    //! ...
    void setFromPrevious( );

    //! Changes the TimeSampling used by this property.
    //! If the TimeSampling is changed to Acyclic and the number of samples
    //! currently set is more than the number of times provided in the Acyclic
    //! TimeSampling, an exception will be thrown.
    void setTimeSampling( uint32_t iIndex );

    //! Changes the TimeSampling used by this property.
    //! If the TimeSampling is changed to Acyclic and the number of samples
    //! currently set is more than the number of times provided in the Acyclic
    //! TimeSampling, an exception will be thrown.
    void setTimeSampling( AbcA::TimeSamplingPtr iTime );

    //! Return the parent compound property, handily wrapped in a
    //! OCompoundProperty wrapper.
    OCompoundProperty getParent() const;

private:
    void init( AbcA::CompoundPropertyWriterPtr iParentObject,
               const std::string &iName,
               const AbcA::DataType &iDataType,

               ErrorHandler::Policy iParentPolicy,

               const Argument &iArg0,
               const Argument &iArg1,
               const Argument &iArg2 );
};

//-*****************************************************************************
// TEMPLATE AND INLINE FUNCTIONS
//-*****************************************************************************

//-*****************************************************************************
template <class CPROP_PTR>
inline OScalarProperty::OScalarProperty( CPROP_PTR iParentProp,
                                         const std::string &iName,
                                         const AbcA::DataType &iDataType,
                                         const Argument &iArg0,
                                         const Argument &iArg1,
                                         const Argument &iArg2 )
{
    init( GetCompoundPropertyWriterPtr( iParentProp ),
          iName, iDataType,

          GetErrorHandlerPolicy( iParentProp ),
          iArg0, iArg1, iArg2 );
}

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Abc
} // End namespace Alembic

#endif
