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
void register_ifaceset()
{
    // IFaceSet
    //
    register_ISchemaObject<AbcG::IFaceSet>( "IFaceSet" );

    // IGeomBaseSchema base class
    //
    register_IGeomBaseSchema<AbcG::FaceSetSchemaInfo>(
                                            "IGeomBaseSchema_FaceSet" );

    // IFaceSetSchema
    //
    class_<AbcG::IFaceSetSchema,
           bases<AbcG::IGeomBaseSchema<AbcG::FaceSetSchemaInfo> > >(
          "IFaceSetSchema",
          "The IFaceSetSchema class is a faceset schema writer",
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
        .def( "isConstant",
              &AbcG::IFaceSetSchema::isConstant )
        .def( "getNumSamples",
              &AbcG::IFaceSetSchema::getNumSamples )
        .def( "getValue",
              &AbcG::IFaceSetSchema::getValue,
              ( arg( "iSS" ) = Abc::ISampleSelector() ) )
        .def( "getFaceExclusivity",
              &AbcG::IFaceSetSchema::getFaceExclusivity )
        .def( "getTimeSampling",
              &AbcG::IFaceSetSchema::getTimeSampling )
        .def( "getFacesProperty",
              &AbcG::IFaceSetSchema::getFacesProperty )
        .def( "valid", &AbcG::IFaceSetSchema::valid )
        .def( "reset", &AbcG::IFaceSetSchema::reset )
        .def( "__nonzero__", &AbcG::IFaceSetSchema::valid )
        ;

    // IFaceSetSchema::Sample
    //
    class_<AbcG::IFaceSetSchema::Sample>( "IFaceSetSchemaSample", init<>() )
        .def( "getFaces",
              &AbcG::IFaceSetSchema::Sample::getFaces,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getSelfBounds",
              &AbcG::IFaceSetSchema::Sample::getSelfBounds )
        .def( "valid", &AbcG::IFaceSetSchema::Sample::valid )
        .def( "reset", &AbcG::IFaceSetSchema::Sample::reset )
        .def( "__nonzero__", &AbcG::IFaceSetSchema::Sample::valid )
        ;
}
