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
#include <PyOSchemaObject.h>
#include <PyOGeomBaseSchema.h>

using namespace boost::python;

//-*****************************************************************************
void register_ofaceset()
{
    // OFaceSet
    //
    register_OSchemaObject<AbcG::OFaceSet>( "OFaceSet" );

    // OGeomBaseSchema
    //
    register_OGeomBaseSchema<AbcG::FaceSetSchemaInfo>(
                                            "OGeomBaseSchema_FaceSet" );

    // overloads for OFaceSetSchema
    //
    void ( AbcG::OFaceSetSchema::*setTimeSamplingByIndex )( AbcU::uint32_t )
        = &AbcG::OFaceSetSchema::setTimeSampling;
    void ( AbcG::OFaceSetSchema::*setTimeSamplingByTimeSamplingPtr )
        ( AbcA::TimeSamplingPtr ) = &AbcG::OFaceSetSchema::setTimeSampling;

    // OFaceSetSchema
    //
    class_<AbcG::OFaceSetSchema,
           bases<AbcG::OGeomBaseSchema<AbcG::FaceSetSchemaInfo> > >(
          "OFaceSetSchema",
          "The OFaceSetSchema class is a faceset schema writer",
          init<>() )
        .def( "getNumSamples",
              &AbcG::OFaceSetSchema::getNumSamples )
        .def( "set",
              &AbcG::OFaceSetSchema::set,
              ( arg( "iSamp" ) ) )
        .def( "setTimeSampling",
              setTimeSamplingByIndex,
              ( arg( "index" ) ) )
        .def( "setTimeSampling",
              setTimeSamplingByTimeSamplingPtr,
              ( arg( "TimeSampling" ) ) )
        .def( "getFaceExclusivity",
              &AbcG::OFaceSetSchema::getFaceExclusivity )
        .def( "setFaceExclusivity",
              &AbcG::OFaceSetSchema::setFaceExclusivity,
              ( arg( "facesExclusive" ) ) )
        .def( "valid", &AbcG::OFaceSetSchema::valid )
        .def( "reset", &AbcG::OFaceSetSchema::reset )
        .def( "__nonzero__", &AbcG::OFaceSetSchema::valid )
        ;

    // OFaceSetSchema::Sample
    //
    class_<AbcG::OFaceSetSchema::Sample>( "OFaceSetSchemaSample", init<>() )
        .def( init<const Abc::Int32ArraySample>(
              ( arg( "iFaceNums" ) ) )
              [with_custodian_and_ward<1,2>()] )
        .def( "getFaces",
              &AbcG::OFaceSetSchema::Sample::getFaces,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setFaces",
              &AbcG::OFaceSetSchema::Sample::setFaces,
              with_custodian_and_ward<1,2>() )
        .def( "getSelfBounds",
              &AbcG::OFaceSetSchema::Sample::getSelfBounds,
              return_value_policy<copy_const_reference>() )
        .def( "setSelfBounds",
              &AbcG::OFaceSetSchema::Sample::setSelfBounds,
              ( arg( "bounds" ) ) )
        .def ("reset", &AbcG::OFaceSetSchema::Sample::reset )
        ;
}
