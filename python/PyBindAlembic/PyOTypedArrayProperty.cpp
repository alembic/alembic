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

    // OTypedArrayProperty
    //
    class_<T, Abc::OArrayProperty>(
        module_handle,
        iName,
        "This class is a typed array property writer" )
        .def( init<>(), "Create an empty property" )
        .def( init<Abc::OCompoundProperty,
                   const std::string&,
                   Abc::Argument,
                   Abc::Argument,
                   Abc::Argument>(),
                    arg ( "parent" ), arg( "name" ), arg( "argument" ),
                    arg ( "argument" ), arg( "argument" ),
                    "Create a new typed property with the given parent "
                    "OCompoundProperty, name and optional arguments which can "
                    "be use to override the ErrorHandlingPolicy, to specify "
                    "MetaData, and to specify time sampling or time sampling "
                    "index" )
        .def( "getInterpretation",
              &T::getInterpretation,
              "Return the interpretation string expected of this property" )
        .def_static( "matches",
              Overloads::matchesMetaData,
                arg( "metaData" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching,
              "Return True if the given entity (as represented by a metadata) "
              "strictly matches the interpretation of this typed property" )
        .def_static( "matches",
              Overloads::matchesHeader,
                arg( "propertyHeader" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching,
              "Return True if the given entity (as represented by a property "
              "header) strictly matches the interepretation of this typed "
              "property, as well as the data type" )
        ;
}

//-*****************************************************************************
void register_otypedarrayproperty( py::module_& module_handle )
{
    register_<Abc::OBoolArrayProperty>   ( module_handle, "OBoolArrayProperty" );
    register_<Abc::OUcharArrayProperty>  ( module_handle, "OUcharArrayProperty" );
    register_<Abc::OCharArrayProperty>   ( module_handle, "OCharArrayProperty" );
    register_<Abc::OUInt16ArrayProperty> ( module_handle, "OUInt16ArrayProperty" );
    register_<Abc::OInt16ArrayProperty>  ( module_handle, "OInt16ArrayProperty" );
    register_<Abc::OUInt32ArrayProperty> ( module_handle, "OUInt32ArrayProperty" );
    register_<Abc::OInt32ArrayProperty>  ( module_handle, "OInt32ArrayProperty" );
    register_<Abc::OUInt64ArrayProperty> ( module_handle, "OUInt64ArrayProperty" );
    register_<Abc::OInt64ArrayProperty>  ( module_handle, "OInt64ArrayProperty" );
    register_<Abc::OHalfArrayProperty>   ( module_handle, "OHalfArrayProperty" );
    register_<Abc::OFloatArrayProperty>  ( module_handle, "OFloatArrayProperty" );
    register_<Abc::ODoubleArrayProperty> ( module_handle, "ODoubleArrayProperty" );
    register_<Abc::OStringArrayProperty> ( module_handle, "OStringArrayProperty" );
    register_<Abc::OWstringArrayProperty>( module_handle, "OWstringArrayProperty" );

    register_<Abc::OV2sArrayProperty>( module_handle, "OV2sArrayProperty" );
    register_<Abc::OV2iArrayProperty>( module_handle, "OV2iArrayProperty" );
    register_<Abc::OV2fArrayProperty>( module_handle, "OV2fArrayProperty" );
    register_<Abc::OV2dArrayProperty>( module_handle, "OV2dArrayProperty" );

    register_<Abc::OV3sArrayProperty>( module_handle, "OV3sArrayProperty" );
    register_<Abc::OV3iArrayProperty>( module_handle, "OV3iArrayProperty" );
    register_<Abc::OV3fArrayProperty>( module_handle, "OV3fArrayProperty" );
    register_<Abc::OV3dArrayProperty>( module_handle, "OV3dArrayProperty" );

    register_<Abc::OP2sArrayProperty>( module_handle, "OP2sArrayProperty" );
    register_<Abc::OP2iArrayProperty>( module_handle, "OP2iArrayProperty" );
    register_<Abc::OP2fArrayProperty>( module_handle, "OP2fArrayProperty" );
    register_<Abc::OP2dArrayProperty>( module_handle, "OP2dArrayProperty" );

    register_<Abc::OP3sArrayProperty>( module_handle, "OP3sArrayProperty" );
    register_<Abc::OP3iArrayProperty>( module_handle, "OP3iArrayProperty" );
    register_<Abc::OP3fArrayProperty>( module_handle, "OP3fArrayProperty" );
    register_<Abc::OP3dArrayProperty>( module_handle, "OP3dArrayProperty" );

    register_<Abc::OBox2sArrayProperty>( module_handle, "OBox2sArrayProperty" );
    register_<Abc::OBox2iArrayProperty>( module_handle, "OBox2iArrayProperty" );
    register_<Abc::OBox2fArrayProperty>( module_handle, "OBox2fArrayProperty" );
    register_<Abc::OBox2dArrayProperty>( module_handle, "OBox2dArrayProperty" );

    register_<Abc::OBox3sArrayProperty>( module_handle, "OBox3sArrayProperty" );
    register_<Abc::OBox3iArrayProperty>( module_handle, "OBox3iArrayProperty" );
    register_<Abc::OBox3fArrayProperty>( module_handle, "OBox3fArrayProperty" );
    register_<Abc::OBox3dArrayProperty>( module_handle, "OBox3dArrayProperty" );

    register_<Abc::OM33fArrayProperty>( module_handle, "OM33fArrayProperty" );
    register_<Abc::OM33dArrayProperty>( module_handle, "OM33dArrayProperty" );
    register_<Abc::OM44fArrayProperty>( module_handle, "OM44fArrayProperty" );
    register_<Abc::OM44dArrayProperty>( module_handle, "OM44dArrayProperty" );

    register_<Abc::OQuatfArrayProperty>( module_handle, "OQuatfArrayProperty" );
    register_<Abc::OQuatdArrayProperty>( module_handle, "OQuatdArrayProperty" );

    register_<Abc::OC3hArrayProperty>( module_handle, "OC3hArrayProperty" );
    register_<Abc::OC3fArrayProperty>( module_handle, "OC3fArrayProperty" );
    register_<Abc::OC3cArrayProperty>( module_handle, "OC3cArrayProperty" );

    register_<Abc::OC4hArrayProperty>( module_handle, "OC4hArrayProperty" );
    register_<Abc::OC4fArrayProperty>( module_handle, "OC4fArrayProperty" );
    register_<Abc::OC4cArrayProperty>( module_handle, "OC4cArrayProperty" );

    register_<Abc::ON2fArrayProperty>( module_handle, "ON3fArrayProperty" );
    register_<Abc::ON2dArrayProperty>( module_handle, "ON3dArrayProperty" );

    register_<Abc::ON3fArrayProperty>( module_handle, "ON3fArrayProperty" );
    register_<Abc::ON3dArrayProperty>( module_handle, "ON3dArrayProperty" );
}
