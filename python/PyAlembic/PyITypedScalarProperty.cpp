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

    // ITypedScalarProperty
    class_<T, bases<Abc::IScalarProperty> >(
        iName,
        "This class is a typed scalar property reader",
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
void register_itypedscalarproperty()
{
    register_<Abc::IBoolProperty>   ( "IBoolProperty" );
    register_<Abc::IUcharProperty>  ( "IUcharProperty" );
    register_<Abc::ICharProperty>   ( "ICharProperty" );
    register_<Abc::IUInt16Property> ( "IUInt16Property" );
    register_<Abc::IInt16Property>  ( "IInt16Property" );
    register_<Abc::IUInt32Property> ( "IUInt32Property" );
    register_<Abc::IInt32Property>  ( "IInt32Property" );
    register_<Abc::IUInt64Property> ( "IUInt64Property" );
    register_<Abc::IInt64Property>  ( "IInt64Property" );
    register_<Abc::IHalfProperty>   ( "IHalfProperty" );
    register_<Abc::IFloatProperty>  ( "IFloatProperty" );
    register_<Abc::IDoubleProperty> ( "IDoubleProperty" );
    register_<Abc::IStringProperty> ( "IStringProperty" );
    register_<Abc::IWstringProperty>( "IWstringProperty" );

    register_<Abc::IV2sProperty>( "IV2sProperty" );
    register_<Abc::IV2iProperty>( "IV2iProperty" );
    register_<Abc::IV2fProperty>( "IV2fProperty" );
    register_<Abc::IV2dProperty>( "IV2dProperty" );

    register_<Abc::IV3sProperty>( "IV3sProperty" );
    register_<Abc::IV3iProperty>( "IV3iProperty" );
    register_<Abc::IV3fProperty>( "IV3fProperty" );
    register_<Abc::IV3dProperty>( "IV3dProperty" );

    register_<Abc::IP2sProperty>( "IP2sProperty" );
    register_<Abc::IP2iProperty>( "IP2iProperty" );
    register_<Abc::IP2fProperty>( "IP2fProperty" );
    register_<Abc::IP2dProperty>( "IP2dProperty" );

    register_<Abc::IP3sProperty>( "IP3sProperty" );
    register_<Abc::IP3iProperty>( "IP3iProperty" );
    register_<Abc::IP3fProperty>( "IP3fProperty" );
    register_<Abc::IP3dProperty>( "IP3dProperty" );

    register_<Abc::IBox2sProperty>( "IBox2sProperty" );
    register_<Abc::IBox2iProperty>( "IBox2iProperty" );
    register_<Abc::IBox2fProperty>( "IBox2fProperty" );
    register_<Abc::IBox2dProperty>( "IBox2dProperty" );

    register_<Abc::IBox3sProperty>( "IBox3sProperty" );
    register_<Abc::IBox3iProperty>( "IBox3iProperty" );
    register_<Abc::IBox3fProperty>( "IBox3fProperty" );
    register_<Abc::IBox3dProperty>( "IBox3dProperty" );

    register_<Abc::IM33fProperty>( "IM33fProperty" );
    register_<Abc::IM33dProperty>( "IM33dProperty" );
    register_<Abc::IM44fProperty>( "IM44fProperty" );
    register_<Abc::IM44dProperty>( "IM44dProperty" );

    register_<Abc::IQuatfProperty>( "IQuatfProperty" );
    register_<Abc::IQuatdProperty>( "IQuatdProperty" );

    register_<Abc::IC3hProperty>( "IC3hProperty" );
    register_<Abc::IC3fProperty>( "IC3fProperty" );
    register_<Abc::IC3cProperty>( "IC3cProperty" );

    register_<Abc::IC4hProperty>( "IC4hProperty" );
    register_<Abc::IC4fProperty>( "IC4fProperty" );
    register_<Abc::IC4cProperty>( "IC4cProperty" );

    register_<Abc::IN2fProperty>( "IN3fProperty" );
    register_<Abc::IN2dProperty>( "IN3dProperty" );

    register_<Abc::IN3fProperty>( "IN3fProperty" );
    register_<Abc::IN3dProperty>( "IN3dProperty" );
}
