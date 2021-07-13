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

// overloads
//
template<class ISCHEMAOBJECT>
struct ISchemaObjectOverloads
{
    static typename ISCHEMAOBJECT::schema_type& getSchema(
                                            ISCHEMAOBJECT &iISchemaObject )
    {
        return iISchemaObject.getSchema();
    }
    static bool matchesMetaData( const AbcA::MetaData& iMetaData,
                                 Abc::SchemaInterpMatching iMatching )
    {
        return ISCHEMAOBJECT::matches( iMetaData, iMatching );
    }
    static bool matchesHeader( const AbcA::ObjectHeader& iHeader,
                               Abc::SchemaInterpMatching iMatching )
    {
        return ISCHEMAOBJECT::matches( iHeader, iMatching );
    }
};

//-*****************************************************************************
template<class ISCHEMAOBJECT>
void register_ISchemaObject( py::module_& module_handle, const char *iName )
{
    // ISCHEMAOBJECT
    //
    class_<ISCHEMAOBJECT, Abc::IObject>(
          module_handle,
          iName,
          "doc" )
        .def( init<>() )
        .def ( init<Abc::IObject,
                    const std::string&,
                        const Abc::Argument&,
                        const Abc::Argument& > () )
        .def ( init<Abc::IObject,
                    const Abc::WrapExistingFlag,
                        const Abc::Argument&,
                        const Abc::Argument&> () )
        .def_static( "getSchemaObjTitle",
              &ISCHEMAOBJECT::getSchemaObjTitle,
              "doc" )
        .def_static( "getSchemaTitle",
              &ISCHEMAOBJECT::getSchemaTitle,
              "doc" )
        .def( "getSchema",
              ISchemaObjectOverloads<ISCHEMAOBJECT>::getSchema,
              "doc",
              rreturn_value_policy::reference_internal )
        .def_static( "matches",
              ISchemaObjectOverloads<ISCHEMAOBJECT>::matchesMetaData,
                arg( "metaData" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching ,
              "" )
        .def_static( "matches",
              ISchemaObjectOverloads<ISCHEMAOBJECT>::matchesHeader,
                arg( "metaData" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching ,
              "" )
        .def( "valid", &ISCHEMAOBJECT::valid )
        .def( "reset", &ISCHEMAOBJECT::reset )
        .def( ALEMBIC_PYTHON_BOOL_NAME, &ISCHEMAOBJECT::valid )
        ;
}
