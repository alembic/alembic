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

template<class INFO>
void register_ISchema( const char *iName )
{
    typedef Abc::ISchema<INFO> ISchema;

    // Overloads
    //
    struct ISchemaOverloads
    {
        static bool matchesMetaData( const AbcA::MetaData& iMetaData,
                                     Abc::SchemaInterpMatching iMatching )
        {
            return ISchema::matches( iMetaData, iMatching );
        }
        static bool matchesHeader( const AbcA::PropertyHeader& iHeader,
                                   Abc::SchemaInterpMatching iMatching )
        {
            return ISchema::matches( iHeader, iMatching );
        }
    };

    // ISchema
    //
    class_<ISchema>(
          iName,
          "Usually used as a base class, but could also theoretically be used "
          "as a standalone.",
          init<>() )
        .def( init<Abc::ICompoundProperty,
                   const std::string&,
                   optional<
                   const Abc::Argument&,
                   const Abc::Argument&> >(
                   ( arg( "parent" ), arg( "name" ),
                     arg( "argument" ), arg( "argument" ) ),
                   "doc") )
        .def( init<Abc::ICompoundProperty,
                   optional<
                   const Abc::Argument&,
                   const Abc::Argument&> >(
                   ( arg( "parent" ), arg( "argument" ), arg( "argument" ) ),
                   "doc") )
        .def( init<Abc::ICompoundProperty,
                   const Abc::WrapExistingFlag,
                   optional<
                   const Abc::Argument&,
                   const Abc::Argument&> >(
                   ( arg( "parent" ), arg( "name" ),
                     arg( "argument" ), arg( "argument" ) ),
                   "doc") )
        .def( "getSchemaTitle",
              ISchema::getSchemaTitle,
              "Return the schema title expected of this property."
              "An empty title matches everything." )
        .staticmethod( "getSchemaTitle" )
        .def( "getDefaultSchemaName",
              ISchema::getDefaultSchemaName,
              "Return the default name for instances of this schema. "
              "Often something like '.geom'." )
        .staticmethod( "getDefaultSchemaName" )
        .def( "matches",
              ISchemaOverloads::matchesMetaData,
              ( arg( "metadata" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching ),
              "This will check whether or not a given entity (as represented "
              "by a metadata) strictly matches the interpretation of this "
              "schema object." )
        .def( "matches",
              ISchemaOverloads::matchesHeader,
              ( arg( "header" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching ),
              "This will check whether or not a given entity (as represented "
              "by an object header) strictly matches the interpretation of this"
              "schema object, as well as the datatype." )
        .staticmethod( "matches" )
        ;
}
