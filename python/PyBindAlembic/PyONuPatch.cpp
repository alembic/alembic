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

using namespace py;

//-*****************************************************************************
void register_onupatch( py::module_& module_handle )
{
    // ONuPatch
    //
    register_OSchemaObject<AbcG::ONuPatch>( module_handle, "ONuPatch" );

    // OGeomBaseSchema
    //
    register_OGeomBaseSchema<AbcG::NuPatchSchemaInfo>( module_handle,
                                                "OGeomBaseSchema_NuPatch" );

    struct ONuPatchSchemaSampleOverrides
    {
        static void setPositions(AbcG::ONuPatchSchema::Sample& nuPatchSamp, py::array verts)
        {
            AbcU::Dimensions dims(verts.shape()[0]);
            size_t array_length = verts.size() / 3;
            std::vector<Abc::V3f> array_vals(array_length);
            std::memcpy(array_vals.data(), verts.data(), array_length * sizeof(Abc::V3f));
            Abc::P3fArraySample array_sample(array_vals);
            nuPatchSamp.setPositions(array_sample);
        }
    };

    // ONuPatchSchema
    //
    class_<AbcG::ONuPatchSchema, AbcG::OGeomBaseSchema<AbcG::NuPatchSchemaInfo> >(
          module_handle,
          "ONuPatchSchema",
          "The ONuPatchSchema class is a nupatch schema writer" )
        .def( init<>() )
        .def( "getTimeSampling",
              &AbcG::ONuPatchSchema::getTimeSampling )
        .def( "getNumSamples",
              &AbcG::ONuPatchSchema::getNumSamples )
        .def( "set",
              &AbcG::ONuPatchSchema::set,
                arg( "iSamp" ) )
        .def( "setFromPrevious",
              &AbcG::ONuPatchSchema::setFromPrevious )
        .def( "reset", &AbcG::ONuPatchSchema::reset )
        .def( "valid", &AbcG::ONuPatchSchema::valid )
        .def( ALEMBIC_PYTHON_BOOL_NAME, &AbcG::ONuPatchSchema::valid )
        ;

    // ONuPatchSchema::Sample
    //
    class_<AbcG::ONuPatchSchema::Sample>(
          module_handle,
          "ONuPatchSchemaSample" )
        .def( init<>() )
        .def( init<const Abc::P3fArraySample,
                   const AbcU::int32_t,
                   const AbcU::int32_t,
                   const AbcU::int32_t,
                   const AbcU::int32_t,
                   const Abc::FloatArraySample,
                   const Abc::FloatArraySample>(),
                    arg( "iPos" ), arg( "iNumU" ), arg( "iNumV" ),
                    arg( "iUOrder" ), arg( "iVOrder" ),
                    arg( "iUKnot " ), arg( "iVKnot" ),
                    keep_alive<1,2>(),
                    keep_alive<1,7>() )
        .def( init<const Abc::P3fArraySample,
                   const AbcU::int32_t,
                   const AbcU::int32_t,
                   const AbcU::int32_t,
                   const AbcU::int32_t,
                   const Abc::FloatArraySample,
                   const Abc::FloatArraySample,
                   const AbcG::ON3fGeomParam::Sample,
                   const AbcG::OV2fGeomParam::Sample,
                   const Abc::FloatArraySample>(),
                    arg( "iPos" ), arg( "iNumU" ), arg( "iNumV" ),
                    arg( "iUOrder" ), arg( "iVOrder" ),
                    arg( "iUKnot " ), arg( "iVKnot" ),
                    arg( "iNormals" ) = AbcG::ON3fGeomParam::Sample(),
                    arg( "iUVs" ) = AbcG::OV2fGeomParam::Sample(),
                    arg( "iPosWeight" ) = Abc::FloatArraySample(),
                    keep_alive<1,2>(),
                    keep_alive<1,7>(),
                    keep_alive<1,8>(),
                    keep_alive<1,11>() )
        .def( "getPositions",
              &AbcG::ONuPatchSchema::Sample::getPositions,
              return_value_policy::copy )
        .def( "setPositions",
              &AbcG::ONuPatchSchema::Sample::setPositions,
              keep_alive<1,2>() )
        .def( "setPositions",
              ONuPatchSchemaSampleOverrides::setPositions, arg("verts"),
              keep_alive<1,2>() )
        .def( "getPositionWeights",
              &AbcG::ONuPatchSchema::Sample::getPositionWeights,
              return_value_policy::copy )
        .def( "setPositionWeights",
              &AbcG::ONuPatchSchema::Sample::setPositionWeights,
              keep_alive<1,2>() )
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
              return_value_policy::copy )
        .def( "setUKnot",
              &AbcG::ONuPatchSchema::Sample::setUKnot,
              keep_alive<1,2>() )
        .def( "getVKnot",
              &AbcG::ONuPatchSchema::Sample::getVKnot,
              return_value_policy::copy )
        .def( "setVKnot",
              &AbcG::ONuPatchSchema::Sample::setVKnot,
              keep_alive<1,2>() )
        .def( "getUVs",
              &AbcG::ONuPatchSchema::Sample::getUVs,
              return_value_policy::reference_internal )
        .def( "setUVs",
              &AbcG::ONuPatchSchema::Sample::setUVs,
              keep_alive<1,2>() )
        .def( "getSelfBounds",
              &AbcG::ONuPatchSchema::Sample::getSelfBounds,
              return_value_policy::copy )
        .def( "setSelfBounds",
              &AbcG::ONuPatchSchema::Sample::setSelfBounds )
        .def( "getVelocities",
              &AbcG::ONuPatchSchema::Sample::getVelocities,
              return_value_policy::copy )
        .def( "setVelocities",
              &AbcG::ONuPatchSchema::Sample::setVelocities,
              keep_alive<1,2>() )
        .def( "getNormals",
              &AbcG::ONuPatchSchema::Sample::getNormals,
              return_value_policy::reference_internal )
        .def( "setNormals",
              &AbcG::ONuPatchSchema::Sample::setNormals,
              keep_alive<1,2>() )
        .def( "setTrimCurve",
              &AbcG::ONuPatchSchema::Sample::setTrimCurve,
              keep_alive<1,3>(),
              keep_alive<1,4>(),
              keep_alive<1,5>(),
              keep_alive<1,6>(),
              keep_alive<1,7>(),
              keep_alive<1,8>(),
              keep_alive<1,9>(),
              keep_alive<1,10>(),
              keep_alive<1,11>())
        .def( "getTrimNumLoops",
              &AbcG::ONuPatchSchema::Sample::getTrimNumLoops )
        .def( "getTrimNumCurves",
              &AbcG::ONuPatchSchema::Sample::getTrimNumCurves,
              return_value_policy::copy )
        .def( "getTrimNumVertices",
              &AbcG::ONuPatchSchema::Sample::getTrimNumVertices,
              return_value_policy::copy )
        .def( "getTrimOrder",
              &AbcG::ONuPatchSchema::Sample::getTrimOrder,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "getTrimKnot",
              &AbcG::ONuPatchSchema::Sample::getTrimKnot,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "getTrimMin",
              &AbcG::ONuPatchSchema::Sample::getTrimMin,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "getTrimMax",
              &AbcG::ONuPatchSchema::Sample::getTrimMax,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "getTrimU",
              &AbcG::ONuPatchSchema::Sample::getTrimU,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "getTrimV",
              &AbcG::ONuPatchSchema::Sample::getTrimV,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "getTrimW",
              &AbcG::ONuPatchSchema::Sample::getTrimW,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "hasTrimCurve",
              &AbcG::ONuPatchSchema::Sample::hasTrimCurve )
        .def( "reset",
              &AbcG::ONuPatchSchema::Sample::reset )
        ;
}
