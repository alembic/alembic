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

using namespace py;

//-*****************************************************************************
void register_opolymesh( py::module_& module_handle )
{
    // OPolyMesh
    //
    register_OSchemaObject<AbcG::OPolyMesh>(  module_handle, "OPolyMesh" );

    // OGeomBaseSchema
    //
    register_OGeomBaseSchema<AbcG::PolyMeshSchemaInfo>( module_handle,
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
            std::vector<std::string> faceSetNames;
            iSchema.getFaceSetNames( faceSetNames );

            return object( py::cast(faceSetNames) );
        }

        static object init( AbcG::OPolyMeshSchema& iSchema )
        {
            std::vector<std::string> faceSetNames;
            iSchema.getFaceSetNames( faceSetNames );

            return object( py::cast(faceSetNames) );
        }

        // these overrides are just to avoid/get-around Imath classes ( V3d, V3f etc ).

        static void setPositions( AbcG::OPolyMeshSchema::Sample& oPolySchemaSample,  py::array positionsArray )
        {
            AbcU::Dimensions dims(positionsArray.shape()[0]);
            size_t array_length = positionsArray.size() / 3;
            std::vector<Abc::V3f> prop_array(array_length);
            std::memcpy(prop_array.data(), positionsArray.data(), array_length * sizeof(Abc::V3f));
            Abc::P3fArraySample p3fsample(prop_array);
            oPolySchemaSample.setPositions(p3fsample);
            Abc::P3fArraySample getP3fsample(oPolySchemaSample.getPositions());
        }

        static void setNormals( AbcG::OPolyMeshSchema::Sample& oPolySchemaSample,  py::array positionsArray )
        {
            AbcU::Dimensions dims(positionsArray.shape()[0]);
            size_t array_length = positionsArray.size() / 3;
            std::vector<Abc::N3f> prop_array(array_length);
            std::memcpy(prop_array.data(), positionsArray.data(), array_length * sizeof(Abc::N3f));
            Abc::N3fArraySample n3f_array_sample(prop_array);
            AbcG::ON3fGeomParam::Sample n3fsample(n3f_array_sample, AbcG::GeometryScope());
            oPolySchemaSample.setNormals(n3fsample);
        }

        static void setUVs( AbcG::OPolyMeshSchema::Sample& oPolySchemaSample,  py::array uvArray, AbcG::GeometryScope kGeoScope )
        {
            AbcU::Dimensions dims(uvArray.shape()[0]);
            size_t array_length = uvArray.size() / 2;
            std::vector<Abc::V2f> prop_array(array_length);
            std::memcpy(prop_array.data(), uvArray.data(), array_length * sizeof(Abc::V2f));
            Abc::V2fArraySample v2f_array_sample(prop_array);
            AbcG::OV2fGeomParam::Sample v2fparam(v2f_array_sample, kGeoScope);
            oPolySchemaSample.setUVs(v2fparam);
        }

        static void setSelfBounds( AbcG::OPolyMeshSchema::Sample& oPolySchemaSample,  py::array boundsArray )
        {
            size_t array_length = boundsArray.size() / 3;
            AbcU::Dimensions dims(boundsArray.shape()[0]);
            std::vector<Abc::V3d> prop_array(array_length);
            std::memcpy(prop_array.data(), boundsArray.data(),  array_length * sizeof(Abc::V3d));
            Abc::TypedArraySample<Abc::V3dTPTraits> array_sample(prop_array.data(), dims);
            Abc::Box3d BBox = Abc::Box3d(prop_array[0], prop_array[1]);
            oPolySchemaSample.setSelfBounds(BBox);
        }

        static void setFaceIndices( AbcG::OPolyMeshSchema::Sample& oPolySchemaSample,  py::array faceIndicesArray )
        {
            AbcU::Dimensions dims(faceIndicesArray.shape()[0]);
            std::vector<Abc::int32_t> prop_array(faceIndicesArray.size());
            std::memcpy(prop_array.data(), faceIndicesArray.data(),  faceIndicesArray.size() * sizeof(Abc::int32_t));
            Abc::TypedArraySample<Abc::Int32TPTraits> array_sample(prop_array.data(), dims);
            oPolySchemaSample.setFaceIndices(array_sample);
        }

        static void setFaceCounts( AbcG::OPolyMeshSchema::Sample& oPolySchemaSample,  py::array faceCountsArray )
        {
            AbcU::Dimensions dims(faceCountsArray.shape()[0]);
            std::vector<Abc::int32_t> prop_array(faceCountsArray.size());
            std::memcpy(prop_array.data(), faceCountsArray.data(),  faceCountsArray.size() * sizeof(Abc::int32_t));
            Abc::Int32ArraySample int32samp(prop_array);
            oPolySchemaSample.setFaceCounts(int32samp);
        }
    };

    // OPolyMeshSchema
    //
    class_<AbcG::OPolyMeshSchema, AbcG::OGeomBaseSchema<AbcG::PolyMeshSchemaInfo> >(
          module_handle,
          "OPolyMeshSchema",
          "The OPolyMeshSchema class is a faceset schema writer" )
        .def( init<>() )
        .def( "getTimeSampling",
              &AbcG::OPolyMeshSchema::getTimeSampling )
        .def( "getNumSamples",
              &AbcG::OPolyMeshSchema::getNumSamples )
        .def( "set",
              &AbcG::OPolyMeshSchema::set,
              arg( "iSamp" ) )
        .def( "setFromPrevious",
              &AbcG::OPolyMeshSchema::setFromPrevious )
        .def( "setTimeSampling",
              setTimeSamplingByIndex,
              arg( "index" ) )
        .def( "setTimeSampling",
              setTimeSamplingByTimeSamplingPtr,
              arg( "TimeSampling" ) )
        .def( "setUVSourceName",
              &AbcG::OPolyMeshSchema::setUVSourceName )
        .def( "reset", &AbcG::OPolyMeshSchema::reset )
        .def( "valid", &AbcG::OPolyMeshSchema::valid )
        .def( ALEMBIC_PYTHON_BOOL_NAME, &AbcG::OPolyMeshSchema::valid )
        .def( "createFaceSet",
              &AbcG::OPolyMeshSchema::createFaceSet,
              arg( "iFaceSetName" ),
              return_value_policy::reference_internal )
        .def_static( "getFaceSetNames",
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
    class_<AbcG::OPolyMeshSchema::Sample>(
          module_handle,
          "OPolyMeshSchemaSample" )
        .def( init<>() )
        .def( init<Abc::P3fArraySample>(),
              keep_alive<1,2>() )
        .def( init<const Abc::P3fArraySample,
                   const Abc::Int32ArraySample,
                   const Abc::Int32ArraySample >(),
                   arg( "iPos" ), arg( "iInd" ), arg( "iCnt" ),
               keep_alive<1,2>(),
               keep_alive<1,3>(),
               keep_alive<1,4>() )
        .def( init<const Abc::P3fArraySample,
                   const Abc::Int32ArraySample,
                   const Abc::Int32ArraySample,
                   const AbcG::OV2fGeomParam::Sample,
                   const AbcG::ON3fGeomParam::Sample>(),
                   arg( "iPos" ), arg( "iInd" ), arg( "iCnt" ),
                   arg( "iUVs" ) = AbcG::OV2fGeomParam::Sample(),
                   arg( "iNormals" ) = AbcG::ON3fGeomParam::Sample(),
               keep_alive<1,2>(),
               keep_alive<1,3>(),
               keep_alive<1,4>(),
               keep_alive<1,5>(),
               keep_alive<1,6>() )
        .def( "getPositions",
              &AbcG::OPolyMeshSchema::Sample::getPositions,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "setPositions",
              &AbcG::OPolyMeshSchema::Sample::setPositions,
              keep_alive<1,2>() )
        .def( "setPositions",
              Overloads::setPositions,
              arg("positionsArray"),
              keep_alive<1,2>() )
        .def( "getVelocities",
              &AbcG::OPolyMeshSchema::Sample::getVelocities,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "setVelocities",
              &AbcG::OPolyMeshSchema::Sample::setVelocities,
              keep_alive<1,2>() )
        .def( "getFaceIndices",
              &AbcG::OPolyMeshSchema::Sample::getFaceIndices,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "setFaceIndices",
              &AbcG::OPolyMeshSchema::Sample::setFaceIndices,
              keep_alive<1,2>() )
        .def( "setFaceIndices",
              Overloads::setFaceIndices,
              arg("faceIndicesArray"),
              keep_alive<1,2>() )
        .def( "getFaceCounts",
              &AbcG::OPolyMeshSchema::Sample::getFaceCounts,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "setFaceCounts",
              &AbcG::OPolyMeshSchema::Sample::setFaceCounts,
              keep_alive<1,2>() )
        .def( "setFaceCounts",
              Overloads::setFaceCounts, arg("faceCountsArray"),
              keep_alive<1,2>() )
        .def( "getSelfBounds",
              &AbcG::OPolyMeshSchema::Sample::getSelfBounds,
              return_value_policy::copy )
        .def( "setSelfBounds",
              &AbcG::OPolyMeshSchema::Sample::setSelfBounds )
        .def( "setSelfBounds",
              Overloads::setSelfBounds, arg("boundsArray"), keep_alive<1,2>() )
        .def( "getUVs",
              &AbcG::OPolyMeshSchema::Sample::getUVs,
              return_value_policy::copy,
              keep_alive<0,1>() )
        .def( "setUVs",
              &AbcG::OPolyMeshSchema::Sample::setUVs,
              keep_alive<1,2>() )
        .def( "setUVs",
              Overloads::setUVs, "uvArray", "kGeoScope",
              keep_alive<1,2>() )
        .def( "getNormals",
              &AbcG::OPolyMeshSchema::Sample::getNormals,
              return_value_policy::automatic )
        .def( "setNormals",
              &AbcG::OPolyMeshSchema::Sample::setNormals,
              keep_alive<1,2>() )
        .def( "setNormals",
              Overloads::setNormals,
              arg("normalsArray"),
              keep_alive<1,2>() )
        .def( "reset",
              &AbcG::OPolyMeshSchema::Sample::reset )
        ;
}
