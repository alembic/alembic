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
#include <PyISchemaObject.h>
#include <PyIGeomBaseSchema.h>

using namespace boost::python;

//-*****************************************************************************
void register_igeombase()
{
    // IGeomBaseObject
    register_ISchemaObject<AbcG::IGeomBaseObject>( "IGeomBaseObject" );

    // IGeomBaseSchema
    //
    register_IGeomBaseSchema<AbcG::GeomBaseSchemaInfo>(
                                                "IGeomBaseSchema_GeomBase" );
    // Overloads
    //
    struct Overloads
    {
        static bool matchesMetaData( const AbcA::MetaData& iMetaData,
                                     Abc::SchemaInterpMatching iMatching )
        {
            return AbcG::IGeomBase::matches( iMetaData, iMatching );
        }
        static bool matchesHeader( const AbcA::PropertyHeader& iHeader,
                                   Abc::SchemaInterpMatching iMatching )
        {
            return AbcG::IGeomBase::matches( iHeader, iMatching );
        }
    };

    // IGeomBase
    //
    class_<AbcG::IGeomBase,
           bases<AbcG::IGeomBaseSchema<AbcG::GeomBaseSchemaInfo> > >(
          "IGeomBase",
          "doc",
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
        .def( "getNumSamples",
              &AbcG::IGeomBase::getNumSamples )
        .def( "isConstant",
              &AbcG::IGeomBase::isConstant )
        .def( "getTimeSampling",
              &AbcG::IGeomBase::getTimeSampling )
        .def( "get",
              &AbcG::IGeomBase::get,
              ( arg( "oSample" ), arg( "iSS" ) = Abc::ISampleSelector() ) )
        .def( "getValue",
              &AbcG::IGeomBase::getValue,
              ( arg( "iSS" ) = Abc::ISampleSelector() ) )
        .def( "getArbGeomParams",
              &AbcG::IGeomBase::getArbGeomParams )
        .def( "getUserProperties",
              &AbcG::IGeomBase::getUserProperties )
        .def( "valid", &AbcG::IGeomBase::valid )
        .def( "reset", &AbcG::IGeomBase::reset )
        .def( "__nonzero__", &AbcG::IGeomBase::valid )
        .def( "matches",
              Overloads::matchesMetaData,
              ( arg( "metadata" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching ) )
        .def( "matches",
              Overloads::matchesHeader,
              ( arg( "header" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching ) )
        .staticmethod( "matches" )
        ;

    // IGeomBase::Sample
    //
    class_<AbcG::IGeomBase::Sample>( "IGeomBaseSample", init<>() )
    .def( "getSelfBounds",
          &AbcG::IGeomBase::Sample::getSelfBounds )
    .def( "reset",
          &AbcG::IGeomBase::Sample::reset )
    ;
}
