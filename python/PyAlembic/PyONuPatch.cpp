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
void register_onupatch()
{
    // ONuPatch
    //
    register_OSchemaObject<AbcG::ONuPatch>( "ONuPatch" );

    // OGeomBaseSchema
    //
    register_OGeomBaseSchema<AbcG::NuPatchSchemaInfo>(
                                                "OGeomBaseSchema_NuPatch" );

    // ONuPatchSchema
    //
    class_<AbcG::ONuPatchSchema,
           bases<AbcG::OGeomBaseSchema<AbcG::NuPatchSchemaInfo> > >(
          "ONuPatchSchema",
          "The ONuPatchSchema class is a nupatch schema writer",
          init<>() )
        .def( "getTimeSampling",
              &AbcG::ONuPatchSchema::getTimeSampling )
        .def( "getNumSamples",
              &AbcG::ONuPatchSchema::getNumSamples )
        .def( "set",
              &AbcG::ONuPatchSchema::set,
              ( arg( "iSamp" ) ) )
        .def( "setFromPrevious",
              &AbcG::ONuPatchSchema::setFromPrevious )
        .def( "reset", &AbcG::ONuPatchSchema::reset )
        .def( "valid", &AbcG::ONuPatchSchema::valid )
        .def( "__nonzero__", &AbcG::ONuPatchSchema::valid )
        ;

    // ONuPatchSchema::Sample
    //
    class_<AbcG::ONuPatchSchema::Sample>( "ONuPatchSchemaSample", init<>() )
        .def( init<const Abc::P3fArraySample,
                   const AbcU::int32_t,
                   const AbcU::int32_t,
                   const AbcU::int32_t,
                   const AbcU::int32_t,
                   const Abc::FloatArraySample,
                   const Abc::FloatArraySample,
                   const AbcG::ON3fGeomParam::Sample,
                   const AbcG::OV2fGeomParam::Sample,
                   const Abc::FloatArraySample>(
                   ( arg( "iPos" ), arg( "iNumU" ), arg( "iNumV" ),
                     arg( "iUOrder" ), arg( "iVOrder" ),
                     arg( "iUKnot " ), arg( "iVKnot" ),
                     arg( "iNormals" ) = AbcG::ON3fGeomParam::Sample(),
                     arg( "iUVs" ) = AbcG::OV2fGeomParam::Sample(),
                     arg( "iPosWeight" ) = Abc::FloatArraySample() ) )
              [with_custodian_and_ward<1,2,
               with_custodian_and_ward<1,7,
               with_custodian_and_ward<1,8,
               with_custodian_and_ward<1,11> > > >()] )
        .def( "getPositions",
              &AbcG::ONuPatchSchema::Sample::getPositions,
              return_value_policy<copy_const_reference,
                                  return_internal_reference<> >() )
        .def( "setPositions",
              &AbcG::ONuPatchSchema::Sample::setPositions,
              with_custodian_and_ward<1,2>() )
        .def( "getPositionWeights",
              &AbcG::ONuPatchSchema::Sample::getPositionWeights,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setPositionWeights",
              &AbcG::ONuPatchSchema::Sample::setPositionWeights,
              with_custodian_and_ward<1,2>() )
        .def( "getNv",
              &AbcG::ONuPatchSchema::Sample::getNv )
        .def( "setNv",
              &AbcG::ONuPatchSchema::Sample::setNv )
        .def( "getUOrder",
              &AbcG::ONuPatchSchema::Sample::getUOrder )
        .def( "setUOrder",
              &AbcG::ONuPatchSchema::Sample::setUOrder )
        .def( "getVOrder",
              &AbcG::ONuPatchSchema::Sample::getVOrder )
        .def( "setVOrder",
              &AbcG::ONuPatchSchema::Sample::setVOrder )
        .def( "getUKnot",
              &AbcG::ONuPatchSchema::Sample::getUKnot,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setUKnot",
              &AbcG::ONuPatchSchema::Sample::setUKnot,
              with_custodian_and_ward<1,2>() )
        .def( "getVKnot",
              &AbcG::ONuPatchSchema::Sample::getVKnot,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setVKnot",
              &AbcG::ONuPatchSchema::Sample::setVKnot,
              with_custodian_and_ward<1,2>() )
        .def( "getUVs",
              &AbcG::ONuPatchSchema::Sample::getUVs,
              return_internal_reference<>() )
        .def( "setUVs",
              &AbcG::ONuPatchSchema::Sample::setUVs,
              with_custodian_and_ward<1,2>() )
        .def( "getSelfBounds",
              &AbcG::ONuPatchSchema::Sample::getSelfBounds,
              return_value_policy<copy_const_reference>() )
        .def( "setSelfBounds",
              &AbcG::ONuPatchSchema::Sample::setSelfBounds )
        .def( "getVelocities",
              &AbcG::ONuPatchSchema::Sample::getVelocities,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setVelocities",
              &AbcG::ONuPatchSchema::Sample::setVelocities,
              with_custodian_and_ward<1,2>() )
        .def( "getNormals",
              &AbcG::ONuPatchSchema::Sample::getNormals,
              return_internal_reference<>() )
        .def( "setNormals",
              &AbcG::ONuPatchSchema::Sample::setNormals,
              with_custodian_and_ward<1,2>() )
        .def( "setTrimCurve",
              &AbcG::ONuPatchSchema::Sample::setTrimCurve,
              with_custodian_and_ward<1,3,
              with_custodian_and_ward<1,4,
              with_custodian_and_ward<1,5,
              with_custodian_and_ward<1,6,
              with_custodian_and_ward<1,7,
              with_custodian_and_ward<1,8,
              with_custodian_and_ward<1,9,
              with_custodian_and_ward<1,10,
              with_custodian_and_ward<1,11> > > > > > > > >() )
        .def( "getTrimNumLoops",
              &AbcG::ONuPatchSchema::Sample::getTrimNumLoops )
        .def( "getTrimNumCurves",
              &AbcG::ONuPatchSchema::Sample::getTrimNumCurves,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "getTrimNumVertices",
              &AbcG::ONuPatchSchema::Sample::getTrimNumVertices,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "getTrimOrder",
              &AbcG::ONuPatchSchema::Sample::getTrimOrder,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "getTrimKnot",
              &AbcG::ONuPatchSchema::Sample::getTrimKnot,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "getTrimMin",
              &AbcG::ONuPatchSchema::Sample::getTrimMin,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "getTrimMax",
              &AbcG::ONuPatchSchema::Sample::getTrimMax,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "getTrimU",
              &AbcG::ONuPatchSchema::Sample::getTrimU,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "getTrimV",
              &AbcG::ONuPatchSchema::Sample::getTrimV,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "getTrimW",
              &AbcG::ONuPatchSchema::Sample::getTrimW,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "hasTrimCurve",
              &AbcG::ONuPatchSchema::Sample::hasTrimCurve )
        .def( "reset",
              &AbcG::ONuPatchSchema::Sample::reset )
        ;
}
