//-*****************************************************************************
//
// Copyright (c) 2012-2013,
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
void register_icurves(py::module_& module_handle)
{
    // ICurves
    //
    register_ISchemaObject<AbcG::ICurves>( module_handle, "ICurves" );

    // IGeomBaseSchema
    //
    register_IGeomBaseSchema<AbcG::CurvesSchemaInfo>(module_handle,
                                            "IGeomBaseSchema_Curves" );

    struct ICurvesSchemaOverrides
    {
        static AbcG::ICurvesSchema::Sample getValue(AbcG::ICurvesSchema& iCurveSchema, const Abc::ISampleSelector& iSampSelector = Abc::ISampleSelector())
        {
            AbcG::ICurvesSchema::Sample smp;
            iCurveSchema.get( smp, Abc::ISampleSelector() );
            return smp;

        }

    };

    struct ICurvesSchemaSampleOverrides
    {
        static Abc::FloatArraySamplePtr getKnots(AbcG::ICurvesSchema::Sample& iCurveSchemaSample )
        {
            return (iCurveSchemaSample.getKnots());
        }

    };

    // ICurvesSchema
    //
    class_<AbcG::ICurvesSchema, AbcG::IGeomBaseSchema<AbcG::CurvesSchemaInfo> >(
        module_handle,
          "ICurvesSchema",
          "The ICurvesSchema class is a curves schema writer" )
        .def( init<>() )
        .def( init<Abc::ICompoundProperty,
                   const std::string&,
                   const Abc::Argument&,
                   const Abc::Argument& >(),
                   arg( "parent" ), arg( "name" ),
                   arg( "argument" ), arg( "argument" ),
                   "doc")
        .def( init<Abc::ICompoundProperty,
                   const Abc::Argument&,
                   const Abc::Argument& >(),
                   arg( "parent" ), arg( "argument" ),
                   arg( "argument" ),
                   "doc")
        .def( "getNumSamples",
              &AbcG::ICurvesSchema::getNumSamples )
        .def( "getTopologyVariance",
              &AbcG::ICurvesSchema::getTopologyVariance )
        .def( "isConstant",
              &AbcG::ICurvesSchema::isConstant )
        .def( "getTimeSampling",
              &AbcG::ICurvesSchema::getTimeSampling )
        .def( "get",
              &AbcG::ICurvesSchema::get,
              arg( "sample" ), arg( "iSS" ) = Abc::ISampleSelector() )
        .def( "getValue",
              &AbcG::ICurvesSchema::getValue,
              ( arg( "iSampSelector" ) = Abc::ISampleSelector() ),keep_alive<0,1>(), return_value_policy::move )
        .def( "getVelocitiesProperty",
              &AbcG::ICurvesSchema::getVelocitiesProperty )
        .def( "getPositionsProperty",
              &AbcG::ICurvesSchema::getPositionsProperty )
        .def( "getNumVerticesProperty",
              &AbcG::ICurvesSchema::getNumVerticesProperty )
        .def( "getPositionWeightsProperty",
              &AbcG::ICurvesSchema::getPositionWeightsProperty )
        .def( "getUVsParam",
              &AbcG::ICurvesSchema::getUVsParam )
        .def( "getNormalsParam",
              &AbcG::ICurvesSchema::getNormalsParam )
        .def( "getWidthsParam",
              &AbcG::ICurvesSchema::getWidthsParam,
              keep_alive<0,1>(), return_value_policy::copy )
        .def( "getOrdersProperty",
              &AbcG::ICurvesSchema::getOrdersProperty )
        .def( "getKnotsProperty",
              &AbcG::ICurvesSchema::getKnotsProperty )
        .def( "reset", &AbcG::ICurvesSchema::reset )
        .def( "valid", &AbcG::ICurvesSchema::valid )
        .def( ALEMBIC_PYTHON_BOOL_NAME, &AbcG::ICurvesSchema::valid )
        ;

    // ICurvesSchema::Sample
    //
    class_<AbcG::ICurvesSchema::Sample>(
        module_handle,
        "ICurvesSchemaSample" )
        .def( init<>() )
        .def( "getPositions",
              &AbcG::ICurvesSchema::Sample::getPositions,
              keep_alive<0,1>() )
        .def( "getNumCurves",
              &AbcG::ICurvesSchema::Sample::getNumCurves )
        .def( "getCurvesNumVertices",
              &AbcG::ICurvesSchema::Sample::getCurvesNumVertices,
              keep_alive<0,1>() )
        .def( "getType",
              &AbcG::ICurvesSchema::Sample::getType )
        .def( "getWrap",
              &AbcG::ICurvesSchema::Sample::getWrap )
        .def( "getBasis",
              &AbcG::ICurvesSchema::Sample::getBasis )
        .def( "getOrders",
              &AbcG::ICurvesSchema::Sample::getOrders,
              keep_alive<0,1>() )
        .def( "getKnots",
              &AbcG::ICurvesSchema::Sample::getKnots,
              keep_alive<0,1>() )
        .def( "getPositionWeights",
              &AbcG::ICurvesSchema::Sample::getPositionWeights,
              keep_alive<0,1>() )
        .def( "getSelfBounds",
              &AbcG::ICurvesSchema::Sample::getSelfBounds )
        .def( "getVelocities",
              &AbcG::ICurvesSchema::Sample::getVelocities,
              keep_alive<0,1>() )
        .def( "valid",
              &AbcG::ICurvesSchema::Sample::valid )
        .def( "reset",
              &AbcG::ICurvesSchema::Sample::reset )
        .def( ALEMBIC_PYTHON_BOOL_NAME,
              &AbcG::ICurvesSchema::Sample::valid )
        ;
}
