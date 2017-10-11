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
void register_ocurves()
{
    // OCurves
    //
    register_OSchemaObject<AbcG::OCurves>( "OCurves" );

    // OGeomBaseSchema
    //
    register_OGeomBaseSchema<AbcG::CurvesSchemaInfo>(
                                            "OGeomBaseSchema_Curves" );

    // OCurvesSchema
    //
    class_<AbcG::OCurvesSchema,
           bases<AbcG::OGeomBaseSchema<AbcG::CurvesSchemaInfo> > >(
          "OCurvesSchema",
          "The OCurvesSchema class is a curves schema writer",
          init<>() )
        .def( "getTimeSampling",
              &AbcG::OCurvesSchema::getTimeSampling )
        .def( "getNumSamples",
              &AbcG::OCurvesSchema::getNumSamples )
        .def( "set",
              &AbcG::OCurvesSchema::set,
              ( arg( "iSamp" ) ) )
        .def( "setFromPrevious",
              &AbcG::OCurvesSchema::setFromPrevious )
        .def( "reset", &AbcG::OCurvesSchema::reset )
        .def( "valid", &AbcG::OCurvesSchema::valid )
        .def( "__nonzero__", &AbcG::OCurvesSchema::valid )
        ;

    // OCurvesSchema::Sample
    //
    class_<AbcG::OCurvesSchema::Sample>( "OCurvesSchemaSample", init<>() )
        .def( init<Abc::P3fArraySample>()
              [with_custodian_and_ward<1,2>()] )
        .def( init<const Abc::P3fArraySample,
                   const Abc::Int32ArraySample,
                   const AbcG::CurveType,
                   const AbcG::CurvePeriodicity,
                   const AbcG::OFloatGeomParam::Sample,
                   const AbcG::OV2fGeomParam::Sample,
                   const AbcG::ON3fGeomParam::Sample,
                   const AbcG::BasisType>(
                   ( arg( "iPos" ), arg( "iNVertices" ),
                     arg( "iType" ) = AbcG::kCubic,
                     arg( "iWrap" ) = AbcG::kNonPeriodic,
                     arg( "iWidth" ) = AbcG::OFloatGeomParam::Sample(),
                     arg( "iUVs" ) = AbcG::OV2fGeomParam::Sample(),
                     arg( "iNormals" ) = AbcG::ON3fGeomParam::Sample(),
                     arg( "iBasis" ) = AbcG::kBezierBasis ) )
              [with_custodian_and_ward<1,2,
               with_custodian_and_ward<1,3> >()] )
        .def( "getWidths",
              &AbcG::OCurvesSchema::Sample::getWidths,
              return_internal_reference<>() )
        .def( "setWidths",
              &AbcG::OCurvesSchema::Sample::setWidths,
              with_custodian_and_ward<1,2>() )
        .def( "getPositions",
              &AbcG::OCurvesSchema::Sample::getPositions,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setPositions",
              &AbcG::OCurvesSchema::Sample::setPositions,
              with_custodian_and_ward<1,2>() )
        .def( "getPositionWeights",
              &AbcG::OCurvesSchema::Sample::getPositionWeights,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
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
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
       .def( "setCurvesNumVertices",
              &AbcG::OCurvesSchema::Sample::setCurvesNumVertices,
              with_custodian_and_ward<1,2>() )
        .def( "getUVs",
              &AbcG::OCurvesSchema::Sample::getUVs,
              return_internal_reference<>() )
        .def( "setUVs",
              &AbcG::OCurvesSchema::Sample::setUVs,
              with_custodian_and_ward<1,2>() )
        .def( "getSelfBounds",
              &AbcG::OCurvesSchema::Sample::getSelfBounds,
              return_value_policy<copy_const_reference>() )
        .def( "setSelfBounds",
              &AbcG::OCurvesSchema::Sample::setSelfBounds )
        .def( "getVelocities",
              &AbcG::OCurvesSchema::Sample::getVelocities,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
       .def( "setVelocities",
              &AbcG::OCurvesSchema::Sample::setVelocities,
              with_custodian_and_ward<1,2>() )
        .def( "getNormals",
              &AbcG::OCurvesSchema::Sample::getNormals,
              return_internal_reference<>() )
        .def( "setNormals",
              &AbcG::OCurvesSchema::Sample::setNormals,
              with_custodian_and_ward<1,2>() )
        .def( "getBasis",
              &AbcG::OCurvesSchema::Sample::getBasis )
        .def( "setBasis",
              &AbcG::OCurvesSchema::Sample::setBasis )
        .def( "setOrders",
              &AbcG::OCurvesSchema::Sample::setOrders,
              with_custodian_and_ward<1,2>() )
        .def( "getOrders",
              &AbcG::OCurvesSchema::Sample::getOrders,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setKnots",
              &AbcG::OCurvesSchema::Sample::setKnots,
              with_custodian_and_ward<1,2>() )
        .def( "getKnots",
              &AbcG::OCurvesSchema::Sample::getKnots,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "reset",
              &AbcG::OCurvesSchema::Sample::reset )
        ;
}
