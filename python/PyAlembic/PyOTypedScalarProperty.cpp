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
static void register_(const char* iName)
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
    class_<T, bases<Abc::OScalarProperty> >(
         iName,
         "This class is a typed scalar property writer",
         init<>( "Create an empty property") )
        .def( init<Abc::OCompoundProperty,
                   const std::string&,
                   optional<
                   Abc::Argument,
                   Abc::Argument,
                   Abc::Argument> >(
                  ( arg( "parent" ), arg( "name" ), arg( "argument" ),
                    arg( "argument" ), arg( "argument" ) ),
                    "Create a new typed scalar property with the given parent "
                    "OCompoundProperty, name and optional arguments which can "
                    "be used to override the ErrorHandlingPolicy, to specify "
                    "MetaData, and to specify time sampling or time sampling "
                    "index") )
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
void register_otypedscalarproperty()
{
    register_<Abc::OBoolProperty>   ( "OBoolProperty" );
    register_<Abc::OUcharProperty>  ( "OUcharProperty" );
    register_<Abc::OCharProperty>   ( "OCharProperty" );
    register_<Abc::OUInt16Property> ( "OUInt16Property" );
    register_<Abc::OInt16Property>  ( "OInt16Property" );
    register_<Abc::OUInt32Property> ( "OUInt32Property" );
    register_<Abc::OInt32Property>  ( "OInt32Property" );
    register_<Abc::OUInt64Property> ( "OUInt64Property" );
    register_<Abc::OInt64Property>  ( "OInt64Property" );
    register_<Abc::OHalfProperty>   ( "OHalfProperty" );
    register_<Abc::OFloatProperty>  ( "OFloatProperty" );
    register_<Abc::ODoubleProperty> ( "ODoubleProperty" );
    register_<Abc::OStringProperty> ( "OStringProperty" );
    register_<Abc::OWstringProperty>( "OWstringProperty" );

    register_<Abc::OV2sProperty>( "OV2sProperty" );
    register_<Abc::OV2iProperty>( "OV2iProperty" );
    register_<Abc::OV2fProperty>( "OV2fProperty" );
    register_<Abc::OV2dProperty>( "OV2dProperty" );

    register_<Abc::OV3sProperty>( "OV3sProperty" );
    register_<Abc::OV3iProperty>( "OV3iProperty" );
    register_<Abc::OV3fProperty>( "OV3fProperty" );
    register_<Abc::OV3dProperty>( "OV3dProperty" );

    register_<Abc::OP2sProperty>( "OP2sProperty" );
    register_<Abc::OP2iProperty>( "OP2iProperty" );
    register_<Abc::OP2fProperty>( "OP2fProperty" );
    register_<Abc::OP2dProperty>( "OP2dProperty" );

    register_<Abc::OP3sProperty>( "OP3sProperty" );
    register_<Abc::OP3iProperty>( "OP3iProperty" );
    register_<Abc::OP3fProperty>( "OP3fProperty" );
    register_<Abc::OP3dProperty>( "OP3dProperty" );

    register_<Abc::OBox2sProperty>( "OBox2sProperty" );
    register_<Abc::OBox2iProperty>( "OBox2iProperty" );
    register_<Abc::OBox2fProperty>( "OBox2fProperty" );
    register_<Abc::OBox2dProperty>( "OBox2dProperty" );

    register_<Abc::OBox3sProperty>( "OBox3sProperty" );
    register_<Abc::OBox3iProperty>( "OBox3iProperty" );
    register_<Abc::OBox3fProperty>( "OBox3fProperty" );
    register_<Abc::OBox3dProperty>( "OBox3dProperty" );

    register_<Abc::OM33fProperty>( "OM33fProperty" );
    register_<Abc::OM33dProperty>( "OM33dProperty" );
    register_<Abc::OM44fProperty>( "OM44fProperty" );
    register_<Abc::OM44dProperty>( "OM44dProperty" );

    register_<Abc::OQuatfProperty>( "OQuatfProperty" );
    register_<Abc::OQuatdProperty>( "OQuatdProperty" );

    register_<Abc::OC3hProperty>( "OC3hProperty" );
    register_<Abc::OC3fProperty>( "OC3fProperty" );
    register_<Abc::OC3cProperty>( "OC3cProperty" );

    register_<Abc::OC4hProperty>( "OC4hProperty" );
    register_<Abc::OC4fProperty>( "OC4fProperty" );
    register_<Abc::OC4cProperty>( "OC4cProperty" );

    register_<Abc::ON2fProperty>( "ON3fProperty" );
    register_<Abc::ON2dProperty>( "ON3dProperty" );

    register_<Abc::ON3fProperty>( "ON3fProperty" );
    register_<Abc::ON3dProperty>( "ON3dProperty" );
}
