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

    // ITypedArrayProperty
    //
    class_<T, bases<Abc::IArrayProperty> >(
        iName,
        "This class is a typed array property reader",
        init<>( "Create an empty property" ) )
        .def( init<Abc::ICompoundProperty,
                   const std::string&,
                   optional<
                   Abc::Argument,
                   Abc::Argument> >(
                  ( arg( "parent" ), arg( "name" ),
                    arg( "argument" ), arg( "argument" ) ),
                    "Create a new typed property with the given parent "
                    "OCompoundProperty, name and optional arguments which can "
                    "be use to override the ErrorHandlingPolicy, to specify "
                    "MetaData" ) )
        .def( "getInterpretation",
              &T::getInterpretation,
              "Return the interpretation string expected of this property" )
        .staticmethod( "getInterpretation" )
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
void register_itypedarrayproperty()
{
    register_<Abc::IBoolArrayProperty>   ( "IBoolArrayProperty" );
    register_<Abc::IUcharArrayProperty>  ( "IUcharArrayProperty" );
    register_<Abc::ICharArrayProperty>   ( "ICharArrayProperty" );
    register_<Abc::IUInt16ArrayProperty> ( "IUInt16ArrayProperty" );
    register_<Abc::IInt16ArrayProperty>  ( "IInt16ArrayProperty" );
    register_<Abc::IUInt32ArrayProperty> ( "IUInt32ArrayProperty" );
    register_<Abc::IInt32ArrayProperty>  ( "IInt32ArrayProperty" );
    register_<Abc::IUInt64ArrayProperty> ( "IUInt64ArrayProperty" );
    register_<Abc::IInt64ArrayProperty>  ( "IInt64ArrayProperty" );
    register_<Abc::IHalfArrayProperty>   ( "IHalfArrayProperty" );
    register_<Abc::IFloatArrayProperty>  ( "IFloatArrayProperty" );
    register_<Abc::IDoubleArrayProperty> ( "IDoubleArrayProperty" );
    register_<Abc::IStringArrayProperty> ( "IStringArrayProperty" );
    register_<Abc::IWstringArrayProperty>( "IWstringArrayProperty" );

    register_<Abc::IV2sArrayProperty>( "IV2sArrayProperty" );
    register_<Abc::IV2iArrayProperty>( "IV2iArrayProperty" );
    register_<Abc::IV2fArrayProperty>( "IV2fArrayProperty" );
    register_<Abc::IV2dArrayProperty>( "IV2dArrayProperty" );

    register_<Abc::IV3sArrayProperty>( "IV3sArrayProperty" );
    register_<Abc::IV3iArrayProperty>( "IV3iArrayProperty" );
    register_<Abc::IV3fArrayProperty>( "IV3fArrayProperty" );
    register_<Abc::IV3dArrayProperty>( "IV3dArrayProperty" );

    register_<Abc::IP2sArrayProperty>( "IP2sArrayProperty" );
    register_<Abc::IP2iArrayProperty>( "IP2iArrayProperty" );
    register_<Abc::IP2fArrayProperty>( "IP2fArrayProperty" );
    register_<Abc::IP2dArrayProperty>( "IP2dArrayProperty" );

    register_<Abc::IP3sArrayProperty>( "IP3sArrayProperty" );
    register_<Abc::IP3iArrayProperty>( "IP3iArrayProperty" );
    register_<Abc::IP3fArrayProperty>( "IP3fArrayProperty" );
    register_<Abc::IP3dArrayProperty>( "IP3dArrayProperty" );

    register_<Abc::IBox2sArrayProperty>( "IBox2sArrayProperty" );
    register_<Abc::IBox2iArrayProperty>( "IBox2iArrayProperty" );
    register_<Abc::IBox2fArrayProperty>( "IBox2fArrayProperty" );
    register_<Abc::IBox2dArrayProperty>( "IBox2dArrayProperty" );

    register_<Abc::IBox3sArrayProperty>( "IBox3sArrayProperty" );
    register_<Abc::IBox3iArrayProperty>( "IBox3iArrayProperty" );
    register_<Abc::IBox3fArrayProperty>( "IBox3fArrayProperty" );
    register_<Abc::IBox3dArrayProperty>( "IBox3dArrayProperty" );

    register_<Abc::IM33fArrayProperty>( "IM33fArrayProperty" );
    register_<Abc::IM33dArrayProperty>( "IM33dArrayProperty" );
    register_<Abc::IM44fArrayProperty>( "IM44fArrayProperty" );
    register_<Abc::IM44dArrayProperty>( "IM44dArrayProperty" );

    register_<Abc::IQuatfArrayProperty>( "IQuatfArrayProperty" );
    register_<Abc::IQuatdArrayProperty>( "IQuatdArrayProperty" );

    register_<Abc::IC3hArrayProperty>( "IC3hArrayProperty" );
    register_<Abc::IC3fArrayProperty>( "IC3fArrayProperty" );
    register_<Abc::IC3cArrayProperty>( "IC3cArrayProperty" );

    register_<Abc::IC4hArrayProperty>( "IC4hArrayProperty" );
    register_<Abc::IC4fArrayProperty>( "IC4fArrayProperty" );
    register_<Abc::IC4cArrayProperty>( "IC4cArrayProperty" );

    register_<Abc::IN2fArrayProperty>( "IN3fArrayProperty" );
    register_<Abc::IN2dArrayProperty>( "IN3dArrayProperty" );

    register_<Abc::IN3fArrayProperty>( "IN3fArrayProperty" );
    register_<Abc::IN3dArrayProperty>( "IN3dArrayProperty" );
}
