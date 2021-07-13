//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

using namespace py;

//-*****************************************************************************
template<class T>
static void register_(py::module_& module_handle, const char* iName)
{
    // overloads
    //
    struct Overloads
    {
        static bool matchesMetaData( const AbcA::MetaData& iMetaData,
                                     Abc::SchemaInterpMatching iMatching )
        {
            return T::matches( iMetaData, iMatching );
        }
        static bool matchesHeader( const AbcA::PropertyHeader& iHeader,
                                   Abc::SchemaInterpMatching iMatching )
        {
            return T::matches( iHeader, iMatching );
        }
    };

    // OTypedScalarProperty
    //
    class_<T, Abc::OScalarProperty >(
         module_handle,
         iName,
         "This class is a typed scalar property writer" )
         .def( init<>(), "Create an empty property" )
        .def( init<Abc::OCompoundProperty,
                   const std::string&,
                   Abc::Argument,
                   Abc::Argument,
                   Abc::Argument>(),
                    arg( "parent" ), arg( "name" ), arg( "argument" ),
                    arg( "argument" ), arg( "argument" ),
                    "Create a new typed scalar property with the given parent "
                    "OCompoundProperty, name and optional arguments which can "
                    "be used to override the ErrorHandlingPolicy, to specify "
                    "MetaData, and to specify time sampling or time sampling "
                    "index")
        .def( "getInterpretation",
              &T::getInterpretation,
              "Return the interpretation string expected of this property" )
        .def_static( "matches",
              Overloads::matchesMetaData,
                arg( "metaData" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching ,
              "Return True if the given entity (as represented by a metadata) "
              "strictly matches the interpretation of this typed property" )
        .def_static( "matches",
              Overloads::matchesHeader,
                arg( "propertyHeader" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching ,
              "Return True if the given entity (as represented by a property "
              "header) strictly matches the interepretation of this typed "
              "property, as well as the data type" )
        ;
}

//-*****************************************************************************
void register_otypedscalarproperty( py::module_& module_handle )
{
    register_<Abc::OBoolProperty>   ( module_handle, "OBoolProperty" );
    register_<Abc::OUcharProperty>  ( module_handle, "OUcharProperty" );
    register_<Abc::OCharProperty>   ( module_handle, "OCharProperty" );
    register_<Abc::OUInt16Property> ( module_handle, "OUInt16Property" );
    register_<Abc::OInt16Property>  ( module_handle, "OInt16Property" );
    register_<Abc::OUInt32Property> ( module_handle, "OUInt32Property" );
    register_<Abc::OInt32Property>  ( module_handle, "OInt32Property" );
    register_<Abc::OUInt64Property> ( module_handle, "OUInt64Property" );
    register_<Abc::OInt64Property>  ( module_handle, "OInt64Property" );
    register_<Abc::OHalfProperty>   ( module_handle, "OHalfProperty" );
    register_<Abc::OFloatProperty>  ( module_handle, "OFloatProperty" );
    register_<Abc::ODoubleProperty> ( module_handle, "ODoubleProperty" );
    register_<Abc::OStringProperty> ( module_handle, "OStringProperty" );
    register_<Abc::OWstringProperty>( module_handle, "OWstringProperty" );

    register_<Abc::OV2sProperty>( module_handle, "OV2sProperty" );
    register_<Abc::OV2iProperty>( module_handle, "OV2iProperty" );
    register_<Abc::OV2fProperty>( module_handle, "OV2fProperty" );
    register_<Abc::OV2dProperty>( module_handle, "OV2dProperty" );

    register_<Abc::OV3sProperty>( module_handle, "OV3sProperty" );
    register_<Abc::OV3iProperty>( module_handle, "OV3iProperty" );
    register_<Abc::OV3fProperty>( module_handle, "OV3fProperty" );
    register_<Abc::OV3dProperty>( module_handle, "OV3dProperty" );

    register_<Abc::OP2sProperty>( module_handle, "OP2sProperty" );
    register_<Abc::OP2iProperty>( module_handle, "OP2iProperty" );
    register_<Abc::OP2fProperty>( module_handle, "OP2fProperty" );
    register_<Abc::OP2dProperty>( module_handle, "OP2dProperty" );

    register_<Abc::OP3sProperty>( module_handle, "OP3sProperty" );
    register_<Abc::OP3iProperty>( module_handle, "OP3iProperty" );
    register_<Abc::OP3fProperty>( module_handle, "OP3fProperty" );
    register_<Abc::OP3dProperty>( module_handle, "OP3dProperty" );

    register_<Abc::OBox2sProperty>( module_handle, "OBox2sProperty" );
    register_<Abc::OBox2iProperty>( module_handle, "OBox2iProperty" );
    register_<Abc::OBox2fProperty>( module_handle, "OBox2fProperty" );
    register_<Abc::OBox2dProperty>( module_handle, "OBox2dProperty" );

    register_<Abc::OBox3sProperty>( module_handle, "OBox3sProperty" );
    register_<Abc::OBox3iProperty>( module_handle, "OBox3iProperty" );
    register_<Abc::OBox3fProperty>( module_handle, "OBox3fProperty" );
    register_<Abc::OBox3dProperty>( module_handle, "OBox3dProperty" );

    register_<Abc::OM33fProperty>( module_handle, "OM33fProperty" );
    register_<Abc::OM33dProperty>( module_handle, "OM33dProperty" );
    register_<Abc::OM44fProperty>( module_handle, "OM44fProperty" );
    register_<Abc::OM44dProperty>( module_handle, "OM44dProperty" );

    register_<Abc::OQuatfProperty>( module_handle, "OQuatfProperty" );
    register_<Abc::OQuatdProperty>( module_handle, "OQuatdProperty" );

    register_<Abc::OC3hProperty>( module_handle, "OC3hProperty" );
    register_<Abc::OC3fProperty>( module_handle, "OC3fProperty" );
    register_<Abc::OC3cProperty>( module_handle, "OC3cProperty" );

    register_<Abc::OC4hProperty>( module_handle, "OC4hProperty" );
    register_<Abc::OC4fProperty>( module_handle, "OC4fProperty" );
    register_<Abc::OC4cProperty>( module_handle, "OC4cProperty" );

    register_<Abc::ON2fProperty>( module_handle, "ON3fProperty" );
    register_<Abc::ON2dProperty>( module_handle, "ON3dProperty" );

    register_<Abc::ON3fProperty>( module_handle, "ON3fProperty" );
    register_<Abc::ON3dProperty>( module_handle, "ON3dProperty" );
}
