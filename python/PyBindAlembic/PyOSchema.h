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

template<class INFO>
void register_OSchema( py::module_& module_handle, const char* iName )
{
    typedef Abc::OSchema<INFO> OSchema;

    // Overloads
    //
    struct OSchemaOverloads
    {
        static bool matchesMetaData( const AbcA::MetaData& iMetaData,
                                     Abc::SchemaInterpMatching iMatching )
        {
            return OSchema::matches( iMetaData, iMatching );
        }
        static bool matchesHeader( const AbcA::PropertyHeader& iHeader,
                                   Abc::SchemaInterpMatching iMatching )
        {
            return OSchema::matches( iHeader, iMatching );
        }
    };

    // OSchema
    //
    class_<OSchema>(
          module_handle,
          iName,
          "Usually used as a base class, but could also theoretically be used "
          "as a standalone." )
        .def( init<>() )
        .def_static( "getSchemaTitle",
              OSchema::getSchemaTitle,
              "Return the schema title expected of this property."
              "An empty title matches everything." )
        .def( "getSchemaBaseType",
              OSchema::getSchemaBaseType,
              "Return the schema base type expected of this property."
              "An empty base type means it's the root type." )
        .def_static( "getDefaultSchemaName",
              OSchema::getDefaultSchemaName,
              "Return the default name for instances of this schema. "
              "Often something like '.geom'." )
        .def_static( "matches",
              OSchemaOverloads::matchesMetaData,
              arg( "metadata" ),
              arg( "matchingSchema" ) = Abc::kStrictMatching,
              "This will check whether or not a given entity (as represented "
              "by a metadata) strictly matches the interpretation of this "
              "schema object." )
        .def_static( "matches",
              OSchemaOverloads::matchesHeader,
              arg( "header" ),
              arg( "matchingSchema" ) = Abc::kStrictMatching,
              "This will check whether or not a given entity (as represented "
              "by an object header) strictly matches the interpretation of this"
              "schema object, as well as the datatype." )
        ;
}
