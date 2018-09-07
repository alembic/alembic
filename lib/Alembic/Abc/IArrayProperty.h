//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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
#ifndef Alembic_Abc_IArrayProperty_h
#define Alembic_Abc_IArrayProperty_h

#include <Alembic/Util/Export.h>
#include <Alembic/Abc/Foundation.h>
#include <Alembic/Abc/Base.h>
#include <Alembic/Abc/Argument.h>
#include <Alembic/Abc/ISampleSelector.h>
#include <Alembic/Abc/IBaseProperty.h>
#include <Alembic/Abc/ICompoundProperty.h>

namespace Alembic {
namespace Abc {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ALEMBIC_EXPORT IArrayProperty
    : public IBasePropertyT<AbcA::ArrayPropertyReaderPtr>
{
public:
    //! By convention we always define this_type in Abc classes
    //! Used by unspecified-bool-type conversion below
    typedef IArrayProperty this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty IArrayProperty function set.
    //! ...
    IArrayProperty() : IBasePropertyT<AbcA::ArrayPropertyReaderPtr>() {}

   //! This constructor creates a new array property reader.
    //! The first argument is the parent ICompundProperty, from which the error
    //! handler policy for inheritance is also derived.  The remaining optional
    //! arguments can be used to override the ErrorHandlerPolicy, and that's it.
    IArrayProperty( const ICompoundProperty & iParent,
                    const std::string &iName,

                    const Argument &iArg0 = Argument(),
                    const Argument &iArg1 = Argument() );

    //! This attaches an IArrayProperty wrapper around an existing
    //! ArrayPropertyReaderPtr, with an optional error handling policy.
    IArrayProperty(
        //! The pointer
        //! ...
        AbcA::ArrayPropertyReaderPtr iPtr,

        //! Optional error handling policy
        //! ...
        const Argument &iArg0 = Argument() )
      : IBasePropertyT<AbcA::ArrayPropertyReaderPtr>( iPtr,
            GetErrorHandlerPolicy( iPtr, iArg0 ) ) {};

    // Deprecated in favor of the constructor above
    IArrayProperty(
        AbcA::ArrayPropertyReaderPtr iPtr,
        WrapExistingFlag iWrapFlag,
        const Argument &iArg0 = Argument() )
      : IBasePropertyT<AbcA::ArrayPropertyReaderPtr>( iPtr,
            GetErrorHandlerPolicy( iPtr, iArg0 ) ) {};

    //! Default copy constructor used
    //! Default assignment operator used.

    //! Destructor
    //! ...
    ~IArrayProperty();

    //-*************************************************************************
    // ARRAY PROPERTY READER FUNCTIONALITY
    //-*************************************************************************

    //! Return the number of samples contained in the property.
    //! This can be any number, including zero.
    //! This returns the number of samples that were written, independently
    //! of whether or not they were constant.
    size_t getNumSamples() const;

    //! Ask if we're constant - no change in value amongst samples,
    //! regardless of the time sampling.
    bool isConstant() const;

    //! Ask if we are like a scalar - we have 1 and only 1 DataType per sample.
    bool isScalarLike() const;

    //! Time information.
    AbcA::TimeSamplingPtr getTimeSampling() const;

    //! Get a sample into the address of a datum.
    //! ...
    void get( AbcA::ArraySamplePtr& oSample,
              const ISampleSelector &iSS = ISampleSelector() ) const;

    //! Get a sample into the address of a datum as a particular POD type.
    void getAs( void *oSample, AbcA::PlainOldDataType iPod,
                const ISampleSelector &iSS = ISampleSelector() );

    //! Get a sample into the address of a datum as the POD type of this
    //! array property.
    void getAs( void *oSample,
                const ISampleSelector &iSS = ISampleSelector() );

    //! Get a key from an address of a datum.
    //! ...
    bool getKey( AbcA::ArraySampleKey& oKey,
                 const ISampleSelector &iSS = ISampleSelector() ) const;

    //! Get the dimensions of the datum.
    void getDimensions( Util::Dimensions & oDim,
                        const ISampleSelector &iSS = ISampleSelector() ) const;

    //! Return the parent compound property, handily wrapped in a
    //! ICompoundProperty wrapper.
    ICompoundProperty getParent() const;

private:
    void init( AbcA::CompoundPropertyReaderPtr iParentObject,
               const std::string &iName,

               ErrorHandler::Policy iParentPolicy,

               const Argument &iArg0,
               const Argument &iArg1 );
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Abc
} // End namespace Alembic

#endif
