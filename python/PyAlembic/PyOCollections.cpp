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
#include <PyOSchema.h>
#include <PyOSchemaObject.h>

using namespace boost::python;

//-*****************************************************************************
void register_ocollections()
{
    // OCollections
    //
    register_OSchemaObject<AbcC::OCollections>( "OCollections" );

    // OSchema
    //
    register_OSchema<AbcC::CollectionsSchemaInfo>( "OSchema_Collections" );

    // Overloads for ICollections
    //
    Abc::OStringArrayProperty
        ( AbcC::OCollectionsSchema::*getCollectionByIndex )( size_t i )
        = &AbcC::OCollectionsSchema::getCollection;
    Abc::OStringArrayProperty
        ( AbcC::OCollectionsSchema::*getCollectionByName )
        ( const std::string & iName) = &AbcC::OCollectionsSchema::getCollection;

    // OCollectionsSchema
    //
    class_<AbcC::OCollectionsSchema,
           bases<Abc::OSchema<AbcC::CollectionsSchemaInfo> > >(
          "OCollectionsSchema",
          "The OCollectionsSchema class is a collections schema writer.",
          init<>() )
        .def( "createCollection",
              &AbcC::OCollectionsSchema::createCollection,
              ( arg( "name" ),
                arg( "argument" ) = Abc::Argument(),
                arg( "argument" ) = Abc::Argument(),
                arg( "argument" ) = Abc::Argument() ) )
        .def( "getNumCollections", &AbcC::OCollectionsSchema::getNumCollections )
        .def( "getCollection", getCollectionByIndex,
              ( arg( "index" ) ) )
        .def( "getCollection", getCollectionByName,
              ( arg( "name" ) ) )
        .def( "valid", &AbcC::OCollectionsSchema::valid )
        .def( "__nonzero__", &AbcC::OCollectionsSchema::valid )
        ;
}
