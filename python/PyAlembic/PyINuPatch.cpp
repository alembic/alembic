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
void register_inupatch()
{
    // INuPatch
    //
    register_ISchemaObject<AbcG::INuPatch>( "INuPatch" );

    // IGeomBaseSchema
    //
    register_IGeomBaseSchema<AbcG::NuPatchSchemaInfo>(
                                                "IGeomBaseSchema_NuPatch" );

    // INuPatchSchema
    //
    class_<AbcG::INuPatchSchema,
           bases<AbcG::IGeomBaseSchema<AbcG::NuPatchSchemaInfo> > >(
          "INuPatchSchema",
          "The INuPatchSchema class is a curves schema reader",
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
                   ( arg( "parent" ), arg( "argument" ),
                     arg( "argument" ), arg( "argument" ) ),
                   "doc") )
        .def( "getNumSamples",
              &AbcG::INuPatchSchema::getNumSamples )
        .def( "getTopologyVariance",
              &AbcG::INuPatchSchema::getTopologyVariance )
        .def( "isConstant",
              &AbcG::INuPatchSchema::isConstant )
        .def( "getTimeSampling",
              &AbcG::INuPatchSchema::getTimeSampling )
        .def( "get",
              &AbcG::INuPatchSchema::get,
              ( arg( "sample" ), arg( "iSS" ) = Abc::ISampleSelector() ) )
        .def( "getValue",
              &AbcG::INuPatchSchema::getValue,
              ( arg( "iSampSelector" ) = Abc::ISampleSelector() ) )
        .def( "getPositionsProperty",
              &AbcG::INuPatchSchema::getPositionsProperty )
        .def( "getUKnotsProperty",
              &AbcG::INuPatchSchema::getUKnotsProperty )
        .def( "getVKnotsProperty",
              &AbcG::INuPatchSchema::getVKnotsProperty )
        .def( "getVelocitiesProperty",
              &AbcG::INuPatchSchema::getVelocitiesProperty )
        .def( "getPositionWeightsProperty",
              &AbcG::INuPatchSchema::getPositionWeightsProperty )
        .def( "getUVsParam",
              &AbcG::INuPatchSchema::getUVsParam )
        .def( "getNormalsParam",
              &AbcG::INuPatchSchema::getNormalsParam )
        .def( "hasTrimCurve",
              &AbcG::INuPatchSchema::hasTrimCurve )
        .def( "trimCurveTopologyIsHomogenous",
              &AbcG::INuPatchSchema::trimCurveTopologyIsHomogenous )
        .def( "trimCurveTopologyIsConstant",
              &AbcG::INuPatchSchema::trimCurveTopologyIsConstant )
        .def( "reset", &AbcG::INuPatchSchema::reset )
        .def( "valid", &AbcG::INuPatchSchema::valid )
        .def( "__nonzero__", &AbcG::INuPatchSchema::valid )
        ;

    // INuPatchSchema::Sample
    //
    class_<AbcG::INuPatchSchema::Sample>( "INuPatchSchemaSample", init<>() )
        .def( "getPositions",
              &AbcG::INuPatchSchema::Sample::getPositions,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getVelocities",
              &AbcG::INuPatchSchema::Sample::getVelocities,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getNumU",
              &AbcG::INuPatchSchema::Sample::getNumU )
        .def( "getNumV",
              &AbcG::INuPatchSchema::Sample::getNumV )
        .def( "getUOrder",
              &AbcG::INuPatchSchema::Sample::getUOrder )
        .def( "getVOrder",
              &AbcG::INuPatchSchema::Sample::getVOrder )
        .def( "getUKnot",
              &AbcG::INuPatchSchema::Sample::getUKnot,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getVKnot",
              &AbcG::INuPatchSchema::Sample::getVKnot,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getPositionWeights",
              &AbcG::INuPatchSchema::Sample::getPositionWeights,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getTrimNumLoops",
              &AbcG::INuPatchSchema::Sample::getTrimNumLoops )
        .def( "getTrimNumVertices",
              &AbcG::INuPatchSchema::Sample::getTrimNumVertices,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getTrimNumCurves",
              &AbcG::INuPatchSchema::Sample::getTrimNumCurves,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getTrimOrders",
              &AbcG::INuPatchSchema::Sample::getTrimOrders,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getTrimKnots",
              &AbcG::INuPatchSchema::Sample::getTrimKnots,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getTrimMins",
              &AbcG::INuPatchSchema::Sample::getTrimMins,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getTrimMaxes",
              &AbcG::INuPatchSchema::Sample::getTrimMaxes,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getTrimU",
              &AbcG::INuPatchSchema::Sample::getTrimU,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getTrimV",
              &AbcG::INuPatchSchema::Sample::getTrimV,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getTrimW",
              &AbcG::INuPatchSchema::Sample::getTrimW,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "hasTrimCurve",
              &AbcG::INuPatchSchema::Sample::hasTrimCurve )
        .def( "getSelfBounds",
              &AbcG::INuPatchSchema::Sample::getSelfBounds )
        .def( "valid",
              &AbcG::INuPatchSchema::Sample::valid )
        .def( "reset",
              &AbcG::INuPatchSchema::Sample::reset )
        .def( "__nonzero__",
              &AbcG::INuPatchSchema::Sample::valid )
        ;
}
