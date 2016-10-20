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
void register_opolymesh()
{
    // OPolyMesh
    //
    register_OSchemaObject<AbcG::OPolyMesh>( "OPolyMesh" );

    // OGeomBaseSchema
    //
    register_OGeomBaseSchema<AbcG::PolyMeshSchemaInfo>(
                                                "OGeomBaseSchema_PolyMesh" );

    // overloads for OPolyMeshSchema
    //
    void ( AbcG::OPolyMeshSchema::*setTimeSamplingByIndex )( AbcU::uint32_t )
        = &AbcG::OPolyMeshSchema::setTimeSampling;
    void ( AbcG::OPolyMeshSchema::*setTimeSamplingByTimeSamplingPtr )
        ( AbcA::TimeSamplingPtr ) = &AbcG::OPolyMeshSchema::setTimeSampling;

    struct Overloads
    {
        static object getFaceSetNames( AbcG::OPolyMeshSchema& iSchema )
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

    // OPolyMeshSchema
    //
    class_<AbcG::OPolyMeshSchema,
           bases<AbcG::OGeomBaseSchema<AbcG::PolyMeshSchemaInfo> > >(
          "OPolyMeshSchema",
          "The OPolyMeshSchema class is a faceset schema writer",
          init<>() )
        .def( "getTimeSampling",
              &AbcG::OPolyMeshSchema::getTimeSampling )
        .def( "getNumSamples",
              &AbcG::OPolyMeshSchema::getNumSamples )
        .def( "set",
              &AbcG::OPolyMeshSchema::set,
              ( arg( "iSamp" ) ) )
        .def( "setFromPrevious",
              &AbcG::OPolyMeshSchema::setFromPrevious )
        .def( "setTimeSampling",
              setTimeSamplingByIndex,
              ( arg( "index" ) ) )
        .def( "setTimeSampling",
              setTimeSamplingByTimeSamplingPtr,
              ( arg( "TimeSampling" ) ) )
        .def( "setUVSourceName",
              &AbcG::OPolyMeshSchema::setUVSourceName )
        .def( "reset", &AbcG::OPolyMeshSchema::reset )
        .def( "valid", &AbcG::OPolyMeshSchema::valid )
        .def( "__nonzero__", &AbcG::OPolyMeshSchema::valid )
        .def( "createFaceSet",
              &AbcG::OPolyMeshSchema::createFaceSet,
              ( arg( "iFaceSetName" ) ),
              return_internal_reference<>() )
        .def( "getFaceSetNames",
              &Overloads::getFaceSetNames,
              ( arg( "oFaceSetNames" ) ) )
        .def( "getFaceSet",
              &AbcG::OPolyMeshSchema::getFaceSet,
              ( arg( "iFaceSetName" ) ) )
        .def( "hasFaceSet",
              &AbcG::OPolyMeshSchema::hasFaceSet,
              ( arg( "iFaceSetName" ) ) )

        ;
    // OPolyMeshSchema::Sample
    //
    class_<AbcG::OPolyMeshSchema::Sample>( "OPolyMeshSchemaSample", init<>() )
        .def( init<Abc::P3fArraySample>()
              [with_custodian_and_ward<1,2>()] )
        .def( init<const Abc::P3fArraySample,
                   const Abc::Int32ArraySample,
                   const Abc::Int32ArraySample,
                   const AbcG::OV2fGeomParam::Sample,
                   const AbcG::ON3fGeomParam::Sample>(
                   ( arg( "iPos" ), arg( "iInd" ), arg( "iCnt" ),
                     arg( "iUVs" ) = AbcG::OV2fGeomParam::Sample(),
                     arg( "iNormals" ) = AbcG::ON3fGeomParam::Sample() ) )
              [with_custodian_and_ward<1,2,
               with_custodian_and_ward<1,3,
               with_custodian_and_ward<1,4,
               with_custodian_and_ward<1,5,
               with_custodian_and_ward<1,6> > > > >()] )
        .def( "getPositions",
              &AbcG::OPolyMeshSchema::Sample::getPositions,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setPositions",
              &AbcG::OPolyMeshSchema::Sample::setPositions,
              with_custodian_and_ward<1,2>() )
        .def( "getVelocities",
              &AbcG::OPolyMeshSchema::Sample::getVelocities,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setVelocities",
              &AbcG::OPolyMeshSchema::Sample::setVelocities,
              with_custodian_and_ward<1,2>() )
        .def( "getFaceIndices",
              &AbcG::OPolyMeshSchema::Sample::getFaceIndices,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setFaceIndices",
              &AbcG::OPolyMeshSchema::Sample::setFaceIndices,
              with_custodian_and_ward<1,2>() )
        .def( "getFaceCounts",
              &AbcG::OPolyMeshSchema::Sample::getFaceCounts,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >() )
        .def( "setFaceCounts",
              &AbcG::OPolyMeshSchema::Sample::setFaceCounts,
              with_custodian_and_ward<1,2>() )
        .def( "getSelfBounds",
              &AbcG::OPolyMeshSchema::Sample::getSelfBounds,
              return_value_policy<copy_const_reference>() )
        .def( "setSelfBounds",
              &AbcG::OPolyMeshSchema::Sample::setSelfBounds )
        .def( "getUVs",
              &AbcG::OPolyMeshSchema::Sample::getUVs,
              return_internal_reference<>() )
        .def( "setUVs",
              &AbcG::OPolyMeshSchema::Sample::setUVs,
              with_custodian_and_ward<1,2>() )
        .def( "getNormals",
              &AbcG::OPolyMeshSchema::Sample::getNormals,
              return_internal_reference<>() )
        .def( "setNormals",
              &AbcG::OPolyMeshSchema::Sample::setNormals,
              with_custodian_and_ward<1,2>() )
        .def( "reset",
              &AbcG::OPolyMeshSchema::Sample::reset )
        ;
}
