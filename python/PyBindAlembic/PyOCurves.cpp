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
void register_ocurves( py::module_& module_handle )
{
    // OCurves
    //
    register_OSchemaObject<AbcG::OCurves>( module_handle, "OCurves" );

    // OGeomBaseSchema
    //
    register_OGeomBaseSchema<AbcG::CurvesSchemaInfo>( module_handle,
                                            "OGeomBaseSchema_Curves" );

    struct OCurvesSchemaSampleOverrides
    {
        static void setCurvesNumVertices(AbcG::OCurvesSchema::Sample& curveSamp, py::array nVerts)
        {
            AbcU::Dimensions dims(nVerts.shape()[0]);
            size_t array_length = nVerts.size();
            std::vector<Abc::int32_t> array_vals(array_length);
            std::memcpy(array_vals.data(), nVerts.data(), array_length * sizeof(Abc::int32_t));
            Abc::Int32ArraySample array_sample(array_vals);
            curveSamp.setCurvesNumVertices(array_sample);
            Abc::Int32ArraySample cSamp = curveSamp.getCurvesNumVertices();
        }

        static void setPositions(AbcG::OCurvesSchema::Sample& curveSamp, py::array verts)
        {
            AbcU::Dimensions dims(verts.shape()[0]);
            size_t array_length = verts.size() / 3;
            std::vector<Abc::V3f> array_vals(array_length);
            std::memcpy(array_vals.data(), verts.data(), array_length * sizeof(Abc::V3f));
            Abc::P3fArraySample array_sample(array_vals);
            curveSamp.setPositions(array_sample);
        }
    };

    // OCurvesSchema
    //
    class_<AbcG::OCurvesSchema, AbcG::OGeomBaseSchema<AbcG::CurvesSchemaInfo> >(
        module_handle,
          "OCurvesSchema",
          "The OCurvesSchema class is a curves schema writer" )
        .def( init<>() )
        .def( "getTimeSampling",
              &AbcG::OCurvesSchema::getTimeSampling )
        .def( "getNumSamples",
              &AbcG::OCurvesSchema::getNumSamples )
        .def( "set",
              &AbcG::OCurvesSchema::set,
              arg( "iSamp" ) )
        .def( "setFromPrevious",
              &AbcG::OCurvesSchema::setFromPrevious )
        .def( "reset", &AbcG::OCurvesSchema::reset )
        .def( "valid", &AbcG::OCurvesSchema::valid )
        .def( ALEMBIC_PYTHON_BOOL_NAME, &AbcG::OCurvesSchema::valid )
        ;

    // OCurvesSchema::Sample
    //
    class_<AbcG::OCurvesSchema::Sample>(
        module_handle,
        "OCurvesSchemaSample" )
        .def( init<>() )
        .def( init<Abc::P3fArraySample>(),
              keep_alive<1,2>() )
        .def( init<const Abc::P3fArraySample,
                   const Abc::Int32ArraySample,
                   const AbcG::CurveType,
                   const AbcG::CurvePeriodicity,
                   const AbcG::OFloatGeomParam::Sample,
                   const AbcG::OV2fGeomParam::Sample,
                   const AbcG::ON3fGeomParam::Sample,
                   const AbcG::BasisType>(),
                     arg( "iPos" ), arg( "iNVertices" ),
                     arg( "iType" ) = AbcG::kCubic,
                     arg( "iWrap" ) = AbcG::kNonPeriodic,
                     arg( "iWidth" ) = AbcG::OFloatGeomParam::Sample(),
                     arg( "iUVs" ) = AbcG::OV2fGeomParam::Sample(),
                     arg( "iNormals" ) = AbcG::ON3fGeomParam::Sample(),
                     arg( "iBasis" ) = AbcG::kBezierBasis ,
                  keep_alive<1,2>(),
                  keep_alive<1,3>() )
        .def( "getWidths",
              &AbcG::OCurvesSchema::Sample::getWidths,
              return_value_policy::reference_internal )
        .def( "setWidths",
              &AbcG::OCurvesSchema::Sample::setWidths,
              keep_alive<1,2>() )
        .def( "getPositions",
              &AbcG::OCurvesSchema::Sample::getPositions,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "setPositions",
              OCurvesSchemaSampleOverrides::setPositions, arg("verts"),
              keep_alive<1,2>() )
        .def( "setPositions",
              &AbcG::OCurvesSchema::Sample::setPositions,
              keep_alive<1,2>() )
        .def( "getPositionWeights",
              &AbcG::OCurvesSchema::Sample::getPositionWeights,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "setPositionWeights",
              &AbcG::OCurvesSchema::Sample::setPositionWeights,
              keep_alive<1,2>() )
        .def( "getType",
              &AbcG::OCurvesSchema::Sample::getType )
        .def( "setType",
              &AbcG::OCurvesSchema::Sample::setType )
        .def( "getWrap",
              &AbcG::OCurvesSchema::Sample::getWrap )
        .def( "setWrap",
              &AbcG::OCurvesSchema::Sample::setWrap )
        .def( "getNumCurves",
              &AbcG::OCurvesSchema::Sample::getNumCurves )
        .def( "getCurvesNumVertices",
              &AbcG::OCurvesSchema::Sample::getCurvesNumVertices,
              return_value_policy::copy,
              keep_alive<0,1>() )
       .def( "setCurvesNumVertices",
              &AbcG::OCurvesSchema::Sample::setCurvesNumVertices,
              keep_alive<1,2>() )
       .def( "setCurvesNumVertices",
              OCurvesSchemaSampleOverrides::setCurvesNumVertices, arg("nVerts"),
              keep_alive<1,2>() )
        .def( "getUVs",
              &AbcG::OCurvesSchema::Sample::getUVs,
              return_value_policy::reference_internal )
        .def( "setUVs",
              &AbcG::OCurvesSchema::Sample::setUVs,
              keep_alive<1,2>() )
        .def( "getSelfBounds",
              &AbcG::OCurvesSchema::Sample::getSelfBounds,
              return_value_policy::copy )
        .def( "setSelfBounds",
              &AbcG::OCurvesSchema::Sample::setSelfBounds )
        .def( "getVelocities",
              &AbcG::OCurvesSchema::Sample::getVelocities,
              return_value_policy::copy,
              keep_alive<0,1>() )
       .def( "setVelocities",
              &AbcG::OCurvesSchema::Sample::setVelocities,
              keep_alive<1,2>() )
        .def( "getNormals",
              &AbcG::OCurvesSchema::Sample::getNormals,
              return_value_policy::reference_internal )
        .def( "setNormals",
              &AbcG::OCurvesSchema::Sample::setNormals,
              keep_alive<1,2>() )
        .def( "getBasis",
              &AbcG::OCurvesSchema::Sample::getBasis )
        .def( "setBasis",
              &AbcG::OCurvesSchema::Sample::setBasis )
        .def( "setOrders",
              &AbcG::OCurvesSchema::Sample::setOrders,
              keep_alive<1,2>() )
        .def( "getOrders",
              &AbcG::OCurvesSchema::Sample::getOrders,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "setKnots",
              &AbcG::OCurvesSchema::Sample::setKnots,
              keep_alive<1,2>() )
        .def( "getKnots",
              &AbcG::OCurvesSchema::Sample::getKnots,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "reset",
              &AbcG::OCurvesSchema::Sample::reset )
        ;
}
