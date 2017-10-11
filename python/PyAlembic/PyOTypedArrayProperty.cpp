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

using namespace boost::python;

//-*****************************************************************************
template<class T>
static void register_( const char* iName )
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
    class_<T, bases<Abc::OArrayProperty> >(
        iName,
        "This class is a typed array property writer",
        init<>( "Create an empty property" ) )
        .def( init<Abc::OCompoundProperty,
                   const std::string&,
                   optional<
                   Abc::Argument,
                   Abc::Argument,
                   Abc::Argument> >(
                  ( arg ( "parent" ), arg( "name" ), arg( "argument" ),
                    arg ( "argument" ), arg( "argument" ) ),
                    "Create a new typed property with the given parent "
                    "OCompoundProperty, name and optional arguments which can "
                    "be use to override the ErrorHandlingPolicy, to specify "
                    "MetaData, and to specify time sampling or time sampling "
                    "index" ) )
        .def( "getInterpretation",
              &T::getInterpretation,
              "Return the interpretation string expected of this property" )
        .def( "matches",
              Overloads::matchesMetaData,
              ( arg( "metaData" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching ),
              "Return True if the given entity (as represented by a metadata) "
              "strictly matches the interpretation of this typed property" )
        .def( "matches",
              Overloads::matchesHeader,
              ( arg( "propertyHeader" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching ),
              "Return True if the given entity (as represented by a property "
              "header) strictly matches the interepretation of this typed "
              "property, as well as the data type" )
        .staticmethod( "matches" )
        ;
}

//-*****************************************************************************
void register_otypedarrayproperty()
{
    register_<Abc::OBoolArrayProperty>   ( "OBoolArrayProperty" );
    register_<Abc::OUcharArrayProperty>  ( "OUcharArrayProperty" );
    register_<Abc::OCharArrayProperty>   ( "OCharArrayProperty" );
    register_<Abc::OUInt16ArrayProperty> ( "OUInt16ArrayProperty" );
    register_<Abc::OInt16ArrayProperty>  ( "OInt16ArrayProperty" );
    register_<Abc::OUInt32ArrayProperty> ( "OUInt32ArrayProperty" );
    register_<Abc::OInt32ArrayProperty>  ( "OInt32ArrayProperty" );
    register_<Abc::OUInt64ArrayProperty> ( "OUInt64ArrayProperty" );
    register_<Abc::OInt64ArrayProperty>  ( "OInt64ArrayProperty" );
    register_<Abc::OHalfArrayProperty>   ( "OHalfArrayProperty" );
    register_<Abc::OFloatArrayProperty>  ( "OFloatArrayProperty" );
    register_<Abc::ODoubleArrayProperty> ( "ODoubleArrayProperty" );
    register_<Abc::OStringArrayProperty> ( "OStringArrayProperty" );
    register_<Abc::OWstringArrayProperty>( "OWstringArrayProperty" );

    register_<Abc::OV2sArrayProperty>( "OV2sArrayProperty" );
    register_<Abc::OV2iArrayProperty>( "OV2iArrayProperty" );
    register_<Abc::OV2fArrayProperty>( "OV2fArrayProperty" );
    register_<Abc::OV2dArrayProperty>( "OV2dArrayProperty" );

    register_<Abc::OV3sArrayProperty>( "OV3sArrayProperty" );
    register_<Abc::OV3iArrayProperty>( "OV3iArrayProperty" );
    register_<Abc::OV3fArrayProperty>( "OV3fArrayProperty" );
    register_<Abc::OV3dArrayProperty>( "OV3dArrayProperty" );

    register_<Abc::OP2sArrayProperty>( "OP2sArrayProperty" );
    register_<Abc::OP2iArrayProperty>( "OP2iArrayProperty" );
    register_<Abc::OP2fArrayProperty>( "OP2fArrayProperty" );
    register_<Abc::OP2dArrayProperty>( "OP2dArrayProperty" );

    register_<Abc::OP3sArrayProperty>( "OP3sArrayProperty" );
    register_<Abc::OP3iArrayProperty>( "OP3iArrayProperty" );
    register_<Abc::OP3fArrayProperty>( "OP3fArrayProperty" );
    register_<Abc::OP3dArrayProperty>( "OP3dArrayProperty" );

    register_<Abc::OBox2sArrayProperty>( "OBox2sArrayProperty" );
    register_<Abc::OBox2iArrayProperty>( "OBox2iArrayProperty" );
    register_<Abc::OBox2fArrayProperty>( "OBox2fArrayProperty" );
    register_<Abc::OBox2dArrayProperty>( "OBox2dArrayProperty" );

    register_<Abc::OBox3sArrayProperty>( "OBox3sArrayProperty" );
    register_<Abc::OBox3iArrayProperty>( "OBox3iArrayProperty" );
    register_<Abc::OBox3fArrayProperty>( "OBox3fArrayProperty" );
    register_<Abc::OBox3dArrayProperty>( "OBox3dArrayProperty" );

    register_<Abc::OM33fArrayProperty>( "OM33fArrayProperty" );
    register_<Abc::OM33dArrayProperty>( "OM33dArrayProperty" );
    register_<Abc::OM44fArrayProperty>( "OM44fArrayProperty" );
    register_<Abc::OM44dArrayProperty>( "OM44dArrayProperty" );

    register_<Abc::OQuatfArrayProperty>( "OQuatfArrayProperty" );
    register_<Abc::OQuatdArrayProperty>( "OQuatdArrayProperty" );

    register_<Abc::OC3hArrayProperty>( "OC3hArrayProperty" );
    register_<Abc::OC3fArrayProperty>( "OC3fArrayProperty" );
    register_<Abc::OC3cArrayProperty>( "OC3cArrayProperty" );

    register_<Abc::OC4hArrayProperty>( "OC4hArrayProperty" );
    register_<Abc::OC4fArrayProperty>( "OC4fArrayProperty" );
    register_<Abc::OC4cArrayProperty>( "OC4cArrayProperty" );

    register_<Abc::ON2fArrayProperty>( "ON3fArrayProperty" );
    register_<Abc::ON2dArrayProperty>( "ON3dArrayProperty" );

    register_<Abc::ON3fArrayProperty>( "ON3fArrayProperty" );
    register_<Abc::ON3dArrayProperty>( "ON3dArrayProperty" );
}
