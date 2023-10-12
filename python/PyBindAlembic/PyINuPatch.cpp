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

using namespace py;

//-*****************************************************************************
void register_inupatch( py::module_& module_handle )
{
    // INuPatch
    //
    register_ISchemaObject<AbcG::INuPatch>( module_handle, "INuPatch" );

    // IGeomBaseSchema
    //
    register_IGeomBaseSchema<AbcG::NuPatchSchemaInfo>( module_handle,
                                                "IGeomBaseSchema_NuPatch" );

    // INuPatchSchema
    //
    class_<AbcG::INuPatchSchema, AbcG::IGeomBaseSchema<AbcG::NuPatchSchemaInfo> >(
          module_handle,
          "INuPatchSchema",
          "The INuPatchSchema class is a curves schema reader" )
        .def( init<>() )
        .def( init<Abc::ICompoundProperty,
                   const std::string&,
                   const Abc::Argument&,
                   const Abc::Argument&>(),
                    arg( "parent" ), arg( "name" ),
                    arg( "argument" ), arg( "argument" ),
                   "doc")
        .def( init<Abc::ICompoundProperty,
                   const Abc::Argument&,
                   const Abc::Argument&>(),
                    arg( "parent" ), arg( "argument" ),
                    arg( "argument" ),
                   "doc")
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
                arg( "sample" ), arg( "iSS" ) = Abc::ISampleSelector() )
        .def( "getValue",
              &AbcG::INuPatchSchema::getValue,
                arg( "iSampSelector" ) = Abc::ISampleSelector() )
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
        .def( ALEMBIC_PYTHON_BOOL_NAME, &AbcG::INuPatchSchema::valid )
        ;

    // INuPatchSchema::Sample
    //
    class_<AbcG::INuPatchSchema::Sample>(
          module_handle,
          "INuPatchSchemaSample" )
        .def( init<>() )
        .def( "getPositions",
              &AbcG::INuPatchSchema::Sample::getPositions,
              keep_alive<0,1>() )
        .def( "getVelocities",
              &AbcG::INuPatchSchema::Sample::getVelocities,
              keep_alive<0,1>() )
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
              keep_alive<0,1>() )
        .def( "getVKnot",
              &AbcG::INuPatchSchema::Sample::getVKnot,
              keep_alive<0,1>() )
        .def( "getPositionWeights",
              &AbcG::INuPatchSchema::Sample::getPositionWeights,
              keep_alive<0,1>() )
        .def( "getTrimNumLoops",
              &AbcG::INuPatchSchema::Sample::getTrimNumLoops )
        .def( "getTrimNumVertices",
              &AbcG::INuPatchSchema::Sample::getTrimNumVertices,
              keep_alive<0,1>() )
        .def( "getTrimNumCurves",
              &AbcG::INuPatchSchema::Sample::getTrimNumCurves,
              keep_alive<0,1>() )
        .def( "getTrimOrders",
              &AbcG::INuPatchSchema::Sample::getTrimOrders,
              keep_alive<0,1>() )
        .def( "getTrimKnots",
              &AbcG::INuPatchSchema::Sample::getTrimKnots,
              keep_alive<0,1>() )
        .def( "getTrimMins",
              &AbcG::INuPatchSchema::Sample::getTrimMins,
              keep_alive<0,1>() )
        .def( "getTrimMaxes",
              &AbcG::INuPatchSchema::Sample::getTrimMaxes,
              keep_alive<0,1>() )
        .def( "getTrimU",
              &AbcG::INuPatchSchema::Sample::getTrimU,
              keep_alive<0,1>() )
        .def( "getTrimV",
              &AbcG::INuPatchSchema::Sample::getTrimV,
              keep_alive<0,1>() )
        .def( "getTrimW",
              &AbcG::INuPatchSchema::Sample::getTrimW,
              keep_alive<0,1>() )
        .def( "hasTrimCurve",
              &AbcG::INuPatchSchema::Sample::hasTrimCurve )
        .def( "getSelfBounds",
              &AbcG::INuPatchSchema::Sample::getSelfBounds )
        .def( "valid",
              &AbcG::INuPatchSchema::Sample::valid )
        .def( "reset",
              &AbcG::INuPatchSchema::Sample::reset )
        .def( ALEMBIC_PYTHON_BOOL_NAME,
              &AbcG::INuPatchSchema::Sample::valid )
        ;
}
