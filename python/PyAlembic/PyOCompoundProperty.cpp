//-*****************************************************************************
//
// Copyright (c) 2012,
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

#include <Foundation.h>
#include <PyOBaseProperty.h>

using namespace boost::python;

//-*****************************************************************************
void register_ocompoundproperty()
{
    // OBaseProperty
    register_OBaseProperty<AbcA::CompoundPropertyWriterPtr>(
                                                    "OBaseProperty_Compound" );

    // overloads
    //
    const AbcA::PropertyHeader &
        ( Abc::OCompoundProperty::*getHeaderByIndex )( size_t ) const = \
        &Abc::OCompoundProperty::getPropertyHeader;
    const AbcA::PropertyHeader *
        ( Abc::OCompoundProperty::*getHeaderByName )( const std::string& ) const = \
        &Abc::OCompoundProperty::getPropertyHeader;

    // OCompoundProperty
    //
    class_<Abc::OCompoundProperty,
           bases<Abc::OBasePropertyT<AbcA::CompoundPropertyWriterPtr> > >(
        "OCompoundProperty",
        "Tthe OCompoundProperty class is a compound property writer",
        init<>( "Create an empty OCompoundProperty" ) )
        .def (init<Abc::OCompoundProperty, const std::string&,
                   optional<
                   const Abc::Argument&,
                   const Abc::Argument&> >(
                  ( arg( "parent"  ), arg( "name" ), arg( "argument" ),
                    arg( "argument" ) ),
                  "Create a new OCompoundProperty with the given parent "
                  "OObject, name and optional arguments which can be used to "
                  "override the ErrorHandlerPolicy, to specify MetaData" ) )
        .def( "getNumProperties",
              &Abc::OCompoundProperty::getNumProperties,
              "Return the number of child properties that have been createtd"
              " thus far\n May change as more are created" )
        .def( "getPropertyHeader",
              getHeaderByIndex,
              ( arg( "index" ) ),
              "Return the header of a child property with the given index",
              return_internal_reference<1>() )
        .def( "getPropertyHeader",
              getHeaderByName,
              ( arg( "name" ) ),
              "Return the header of a child property with the given name",
              return_value_policy<reference_existing_object>() )
        .def( "getParent",
              &Abc::OCompoundProperty::getParent,
              "Return the parent OCompoundProperty" )
        ;
}
