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
#include <PyISchema.h>
#include <PyISchemaObject.h>

using namespace py;

//-*****************************************************************************
void register_icollections( py::module_& module_handle )
{
    // ICollections
    //
    register_ISchemaObject<AbcC::ICollections>( module_handle, "ICollections" );

    // ISchema
    //
    register_ISchema<AbcC::CollectionsSchemaInfo>( module_handle, "ISchema_Collections"  );

    // Overloads for ICollections
    //
    Abc::IStringArrayProperty
        ( AbcC::ICollectionsSchema::*getCollectionByIndex )( size_t i )
        = &AbcC::ICollectionsSchema::getCollection;
    Abc::IStringArrayProperty
        ( AbcC::ICollectionsSchema::*getCollectionByName )
        ( const std::string & iName) = &AbcC::ICollectionsSchema::getCollection;


    // ICollectionsSchema
    //
    class_<AbcC::ICollectionsSchema, Abc::ISchema<AbcC::CollectionsSchemaInfo> >(
          module_handle,
          "ICollectionsSchema",
          "The ICollectionsSchema class is a collections schema reader." )
        .def( init<>() )
        .def( init<Abc::ICompoundProperty,
                   const std::string&,
                   const Abc::Argument&,
                   const Abc::Argument&>(),
                    arg( "parent" ), arg( "name" ),
                    arg( "argument" ), arg( "argument" ),
                   "Default constructor creates an empty ICollectionSchema.")
        .def( init<Abc::ICompoundProperty,
                   const Abc::Argument&,
                   const Abc::Argument&>(),
                     arg( "parent" ), arg( "argument" ), arg( "argument" ),
                   "This constructor is the same as above, but with default "
                   "schema name used.")
        .def( "getNumCollections",
               &AbcC::ICollectionsSchema::getNumCollections )
        .def( "getCollection", getCollectionByIndex,
                arg( "index" ) )
        .def( "getCollection", getCollectionByName,
                arg( "name" ) )
        .def( "getCollectionName",
              &AbcC::ICollectionsSchema::getCollectionName,
                arg( "index" ) )
        .def( "valid", &AbcC::ICollectionsSchema::valid )
        .def( ALEMBIC_PYTHON_BOOL_NAME, &AbcC::ICollectionsSchema::valid )
        ;
}
