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

    // ITypedScalarProperty
    class_<T, Abc::IScalarProperty >(
        module_handle,
        iName,
        "This class is a typed scalar property reader" )
        .def( init<>(), "Create an empty property" )
        .def( init<Abc::ICompoundProperty,
                   const std::string&,
                   Abc::Argument,
                   Abc::Argument>(),
                    arg( "parent" ), arg( "name" ),
                    arg( "argument" ), arg( "argument" ),
                    "Create a new typed property with the given parent "
                    "OCompoundProperty, name and optional arguments which can "
                    "be use to override the ErrorHandlingPolicy, to specify "
                    "MetaData" )
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
void register_itypedscalarproperty( py::module_& module_handle )
{
    register_<Abc::IBoolProperty>   ( module_handle, "IBoolProperty" );
    register_<Abc::IUcharProperty>  ( module_handle, "IUcharProperty" );
    register_<Abc::ICharProperty>   ( module_handle, "ICharProperty" );
    register_<Abc::IUInt16Property> ( module_handle, "IUInt16Property" );
    register_<Abc::IInt16Property>  ( module_handle, "IInt16Property" );
    register_<Abc::IUInt32Property> ( module_handle, "IUInt32Property" );
    register_<Abc::IInt32Property>  ( module_handle, "IInt32Property" );
    register_<Abc::IUInt64Property> ( module_handle, "IUInt64Property" );
    register_<Abc::IInt64Property>  ( module_handle, "IInt64Property" );
    register_<Abc::IHalfProperty>   ( module_handle, "IHalfProperty" );
    register_<Abc::IFloatProperty>  ( module_handle, "IFloatProperty" );
    register_<Abc::IDoubleProperty> ( module_handle, "IDoubleProperty" );
    register_<Abc::IStringProperty> ( module_handle, "IStringProperty" );
    register_<Abc::IWstringProperty>( module_handle, "IWstringProperty" );

    register_<Abc::IV2sProperty>( module_handle, "IV2sProperty" );
    register_<Abc::IV2iProperty>( module_handle, "IV2iProperty" );
    register_<Abc::IV2fProperty>( module_handle, "IV2fProperty" );
    register_<Abc::IV2dProperty>( module_handle, "IV2dProperty" );

    register_<Abc::IV3sProperty>( module_handle, "IV3sProperty" );
    register_<Abc::IV3iProperty>( module_handle, "IV3iProperty" );
    register_<Abc::IV3fProperty>( module_handle, "IV3fProperty" );
    register_<Abc::IV3dProperty>( module_handle, "IV3dProperty" );

    register_<Abc::IP2sProperty>( module_handle, "IP2sProperty" );
    register_<Abc::IP2iProperty>( module_handle, "IP2iProperty" );
    register_<Abc::IP2fProperty>( module_handle, "IP2fProperty" );
    register_<Abc::IP2dProperty>( module_handle, "IP2dProperty" );

    register_<Abc::IP3sProperty>( module_handle, "IP3sProperty" );
    register_<Abc::IP3iProperty>( module_handle, "IP3iProperty" );
    register_<Abc::IP3fProperty>( module_handle, "IP3fProperty" );
    register_<Abc::IP3dProperty>( module_handle, "IP3dProperty" );

    register_<Abc::IBox2sProperty>( module_handle, "IBox2sProperty" );
    register_<Abc::IBox2iProperty>( module_handle, "IBox2iProperty" );
    register_<Abc::IBox2fProperty>( module_handle, "IBox2fProperty" );
    register_<Abc::IBox2dProperty>( module_handle, "IBox2dProperty" );

    register_<Abc::IBox3sProperty>( module_handle, "IBox3sProperty" );
    register_<Abc::IBox3iProperty>( module_handle, "IBox3iProperty" );
    register_<Abc::IBox3fProperty>( module_handle, "IBox3fProperty" );
    register_<Abc::IBox3dProperty>( module_handle, "IBox3dProperty" );

    register_<Abc::IM33fProperty>( module_handle, "IM33fProperty" );
    register_<Abc::IM33dProperty>( module_handle, "IM33dProperty" );
    register_<Abc::IM44fProperty>( module_handle, "IM44fProperty" );
    register_<Abc::IM44dProperty>( module_handle, "IM44dProperty" );

    register_<Abc::IQuatfProperty>( module_handle, "IQuatfProperty" );
    register_<Abc::IQuatdProperty>( module_handle, "IQuatdProperty" );

    register_<Abc::IC3hProperty>( module_handle, "IC3hProperty" );
    register_<Abc::IC3fProperty>( module_handle, "IC3fProperty" );
    register_<Abc::IC3cProperty>( module_handle, "IC3cProperty" );

    register_<Abc::IC4hProperty>( module_handle, "IC4hProperty" );
    register_<Abc::IC4fProperty>( module_handle, "IC4fProperty" );
    register_<Abc::IC4cProperty>( module_handle, "IC4cProperty" );

    register_<Abc::IN2fProperty>( module_handle, "IN3fProperty" );
    register_<Abc::IN2dProperty>( module_handle, "IN3dProperty" );

    register_<Abc::IN3fProperty>( module_handle, "IN3fProperty" );
    register_<Abc::IN3dProperty>( module_handle, "IN3dProperty" );
}
