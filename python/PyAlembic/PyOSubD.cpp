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
#include <PyOSchemaObject.h>
#include <PyOGeomBaseSchema.h>
#include <PyImathStringArray.h>

using namespace boost::python;

//-*****************************************************************************
void register_osubd()
{
    // OSubD
    //
    register_OSchemaObject<AbcG::OSubD>( "OSubD" );

    // OGeomBaseSchema
    //
    register_OGeomBaseSchema<AbcG::SubDSchemaInfo>( "OGeomBaseSchema_SubD" );

    // Overloads for OSubDSchema
    //
    void ( AbcG::OSubDSchema::*setTimeSamplingByIndex )( AbcU::uint32_t )
        = &AbcG::OSubDSchema::setTimeSampling;
    void ( AbcG::OSubDSchema::*setTimeSamplingByTimeSamplingPtr )
        ( AbcA::TimeSamplingPtr ) = &AbcG::OSubDSchema::setTimeSampling;

    // Overload for OSubDSchema::getFaceSetNames
    //
    struct Overloads
    {
        static object getFaceSetNames( AbcG::OSubDSchema& iSchema )
        {
            using PyImath::StringArray;

            std::vector<std::string> faceSetNames;
            iSchema.getFaceSetNames( faceSetNames );

            manage_new_object::apply<StringArray *>::type converter;

            return object( handle<>( converter(
                                        StringArray::createFromRawArray(
                                            &faceSetNames.front(),
                                            faceSetNames.size() ) ) ) );
        }
    };

    // OSubDSchema
    //
    class_<AbcG::OSubDSchema,
           bases<AbcG::OGeomBaseSchema<AbcG::SubDSchemaInfo> > >(
          "OSubDSchema",
          "The OSubDSchema class is a subd schema writer.",
          init<>() )
        .def( "getTimeSampling",
              &AbcG::OSubDSchema::getTimeSampling )
        .def( "getNumSamples",
              &AbcG::OSubDSchema::getNumSamples )
        .def( "set",
              &AbcG::OSubDSchema::set,
              ( arg( "iSamp" ) ) )
        .def( "setFromPrevious",
              &AbcG::OSubDSchema::setFromPrevious )
        .def( "setTimeSampling",
              setTimeSamplingByIndex,
              ( arg( "index" ) ) )
        .def( "setTimeSampling",
              setTimeSamplingByTimeSamplingPtr,
              ( arg( "timeSampling" ) ) )
        .def( "createFaceSet",
              &AbcG::OSubDSchema::createFaceSet,
              ( arg( "faceSetName" ) ),
              return_internal_reference<>() )
        .def( "getFaceSetNames",
              &Overloads::getFaceSetNames,
              ( arg( "faceSetNames" ) ) )
        .def( "getFaceSet",
              &AbcG::OSubDSchema::getFaceSet,
              ( arg( "faceSetName" ) ) )
        .def( "hasFaceSet",
              &AbcG::OSubDSchema::hasFaceSet,
              ( arg( "faceSetName" ) ) )
        .def( "setUVSourceName",
              &AbcG::OSubDSchema::setUVSourceName )
        .def( "valid", &AbcG::OSubDSchema::valid )
        .def( "reset", &AbcG::OSubDSchema::reset )
        .def( "__nonzero__", &AbcG::OSubDSchema::valid )
        ;

    // Overloads for OSubDSchema::Sample
    //
    void ( AbcG::OSubDSchema::Sample::*setCreasesIndicesLengths )
        ( const Abc::Int32ArraySample&, const Abc::Int32ArraySample& )
        = &AbcG::OSubDSchema::Sample::setCreases;
    void ( AbcG::OSubDSchema::Sample::*setCreasesIndicesLengthsSharpness )
        ( const Abc::Int32ArraySample&, const Abc::Int32ArraySample&,
          const Abc::FloatArraySample& )
        = &AbcG::OSubDSchema::Sample::setCreases;

    // OSubDSchema::Sample
    //
    class_<AbcG::OSubDSchema::Sample>( "OSubDSchemaSample", init<>() )
        .def( init<const Abc::P3fArraySample,
                   const Abc::Int32ArraySample,
                   const Abc::Int32ArraySample,
                   const Abc::Int32ArraySample,
                   const Abc::Int32ArraySample,
                   const Abc::FloatArraySample,
                   const Abc::Int32ArraySample,
                   const Abc::FloatArraySample,
                   const Abc::Int32ArraySample>(
                   ( arg( "positions" ), arg( "faceIndices" ),
                     arg( "faceCounts" ),
                     arg( "creaseIndices" ) = Abc::Int32ArraySample(),
                     arg( "creaseLengths" ) = Abc::Int32ArraySample(),
                     arg( "creaseSharpnesses" ) = Abc::FloatArraySample(),
                     arg( "cornerIndices" ) = Abc::Int32ArraySample(),
                     arg( "cornerSharpnesses" ) = Abc::FloatArraySample(),
                     arg( "holes" ) = Abc::Int32ArraySample() ) )
              [with_custodian_and_ward<1,2,
               with_custodian_and_ward<1,3,
               with_custodian_and_ward<1,4,
               with_custodian_and_ward<1,5,
               with_custodian_and_ward<1,6,
               with_custodian_and_ward<1,7,
               with_custodian_and_ward<1,8,
               with_custodian_and_ward<1,9,
               with_custodian_and_ward<1,10> > > > > > > > >()] )
        .def( "getPositions",
              &AbcG::OSubDSchema::Sample::getPositions,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setPositions",
              &AbcG::OSubDSchema::Sample::setPositions,
              ( arg( "positions" ) ),
              with_custodian_and_ward<1,2>() )
        .def( "getFaceIndices",
              &AbcG::OSubDSchema::Sample::getFaceIndices,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setFaceIndices",
              &AbcG::OSubDSchema::Sample::setFaceIndices,
              ( arg( "faceIndices" ) ),
              with_custodian_and_ward<1,2>() )
        .def( "getFaceCounts",
              &AbcG::OSubDSchema::Sample::getFaceCounts,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setFaceCounts",
              &AbcG::OSubDSchema::Sample::setFaceCounts,
              ( arg( "faceCounts" ) ),
              with_custodian_and_ward<1,2>() )
        .def( "getFaceVaryingInterpolateBoundary",
              &AbcG::OSubDSchema::Sample::getFaceVaryingInterpolateBoundary )
        .def( "setFaceVaryingInterpolateBoundary",
              &AbcG::OSubDSchema::Sample::setFaceVaryingInterpolateBoundary,
              ( arg( "faceVaryingPropagateCorners" ) ) )
        .def( "getFaceVaryingPropagateCorners",
              &AbcG::OSubDSchema::Sample::getFaceVaryingPropagateCorners )
        .def( "setFaceVaryingPropagateCorners",
              &AbcG::OSubDSchema::Sample::setFaceVaryingPropagateCorners,
              ( arg( "faceVaryingPropagateCorners" ) ) )
        .def( "getInterpolateBoundary",
              &AbcG::OSubDSchema::Sample::getInterpolateBoundary )
        .def( "setInterpolateBoundary",
              &AbcG::OSubDSchema::Sample::setInterpolateBoundary,
              ( arg( "interpolateBoundary" ) ) )
        .def( "getCreaseIndices",
              &AbcG::OSubDSchema::Sample::getCreaseIndices,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setCreaseIndices",
              &AbcG::OSubDSchema::Sample::setCreaseIndices,
              ( arg( "creaseIndices" ) ),
              with_custodian_and_ward<1,2>() )
        .def( "getCreaseLengths",
              &AbcG::OSubDSchema::Sample::getCreaseLengths,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setCreaseLengths",
              &AbcG::OSubDSchema::Sample::setCreaseLengths,
              ( arg( "creaseLengths" ) ),
              with_custodian_and_ward<1,2>() )
        .def( "getCreaseSharpnesses",
              &AbcG::OSubDSchema::Sample::getCreaseSharpnesses,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setCreaseSharpnesses",
              &AbcG::OSubDSchema::Sample::setCreaseSharpnesses,
              ( arg( "creaseSharpnesses" ) ),
              with_custodian_and_ward<1,2>() )
        .def( "setCreases", setCreasesIndicesLengths,
              ( arg( "creaseIndices" ), arg( "creaseLengths" ) ),
              with_custodian_and_ward<1,2,
              with_custodian_and_ward<1,3> >() )
        .def( "setCreases", setCreasesIndicesLengthsSharpness,
              ( arg( "creaseIndices" ), arg( "creaseLengths" ),
                arg( "creaseSharpnesses" ) ),
              with_custodian_and_ward<1,2,
              with_custodian_and_ward<1,3,
              with_custodian_and_ward<1,4> > >() )
        .def( "getCornerIndices",
              &AbcG::OSubDSchema::Sample::getCornerIndices,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setCornerIndices", &AbcG::OSubDSchema::Sample::setCornerIndices,
              ( arg( "cornerIndices" ) ),
              with_custodian_and_ward<1,2>() )
        .def( "getCornerSharpnesses",
              &AbcG::OSubDSchema::Sample::getCornerSharpnesses,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setCornerSharpnesses",
              &AbcG::OSubDSchema::Sample::setCornerSharpnesses,
              ( arg( "cornerSharpnesses" ) ),
              with_custodian_and_ward<1,2>() )
        .def( "setCorners", &AbcG::OSubDSchema::Sample::setCorners,
              ( arg( "cornerIndices" ), arg( "cornerSharpnesses" ) ),
              with_custodian_and_ward<1,2,
              with_custodian_and_ward<1,3> >() )
        .def( "getHoles",
              &AbcG::OSubDSchema::Sample::getHoles,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setHoles", &AbcG::OSubDSchema::Sample::setHoles,
              ( arg( "holes" ) ),
              ( arg( "cornerSharpnesses" ) ),
              with_custodian_and_ward<1,2,
              with_custodian_and_ward<1,3> >() )
        .def( "getSubdivisionScheme",
              &AbcG::OSubDSchema::Sample::getSubdivisionScheme )
        .def( "setSubdivisionScheme",
              &AbcG::OSubDSchema::Sample::setSubdivisionScheme,
              ( arg( "subdScheme" ) ) )
        .def( "getSelfBounds",
              &AbcG::OSubDSchema::Sample::getSelfBounds,
              return_value_policy<copy_const_reference>() )
        .def( "setSelfBounds",
              &AbcG::OSubDSchema::Sample::setSelfBounds,
              ( arg( "selfBounds" ) ) )
        .def( "getVelocities",
              &AbcG::OSubDSchema::Sample::getVelocities,
              return_internal_reference<>()  )
        .def( "setVelocities", &AbcG::OSubDSchema::Sample::setVelocities,
              ( arg( "velocities" ) ),
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "getUVs", &AbcG::OSubDSchema::Sample::getUVs,
              return_internal_reference<>() )
        .def( "setUVs", &AbcG::OSubDSchema::Sample::setUVs,
              ( arg( "uvs" ) ) )
        .def( "reset", &AbcG::OSubDSchema::Sample::reset )
        ;
}
