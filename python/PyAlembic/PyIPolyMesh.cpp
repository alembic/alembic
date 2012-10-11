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
#include <PyImathStringArray.h>

using namespace boost::python;

//-*****************************************************************************
void register_ipolymesh()
{
    // IPolyMesh
    //
    register_ISchemaObject<AbcG::IPolyMesh>( "IPolyMesh" );

    // IGeomBaseSchema base class for IPolyMeshSchema
    //
    register_IGeomBaseSchema<AbcG::PolyMeshSchemaInfo>(
                                                "IGeomBaseSchema_PolyMesh" );

    // overloads for IPolyMeshSchema
    //
    struct Overloads
    {
        static object getFaceSetNames( AbcG::IPolyMeshSchema& iSchema )
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

    // IPolyMeshSchema
    //
    class_<AbcG::IPolyMeshSchema,
           bases<AbcG::IGeomBaseSchema<AbcG::PolyMeshSchemaInfo> > >(
          "IPolyMeshSchema",
          "The IPolyMeshSchema class is a faceset schema writer",
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
                   ( arg( "parent" ),
                     arg( "argument" ), arg( "argument" ) ),
                   "doc") )
        .def( "getNumSamples",
              &AbcG::IPolyMeshSchema::getNumSamples )
        .def( "getTopologyVariance",
              &AbcG::IPolyMeshSchema::getTopologyVariance )
        .def( "isConstant",
              &AbcG::IPolyMeshSchema::isConstant )
        .def( "getTimeSampling",
              &AbcG::IPolyMeshSchema::getTimeSampling )
        .def( "getValue",
              &AbcG::IPolyMeshSchema::getValue,
              ( arg( "iSampSelector" ) = Abc::ISampleSelector() ) )
        .def( "getUVsParam",
              &AbcG::IPolyMeshSchema::getUVsParam )
        .def( "getNormalsParam",
              &AbcG::IPolyMeshSchema::getNormalsParam )
        .def( "getFaceCountsProperty",
              &AbcG::IPolyMeshSchema::getFaceCountsProperty )
        .def( "getFaceIndicesProperty",
              &AbcG::IPolyMeshSchema::getFaceIndicesProperty )
        .def( "getPositionsProperty",
              &AbcG::IPolyMeshSchema::getPositionsProperty )
        .def( "getVelocitiesProperty",
              &AbcG::IPolyMeshSchema::getVelocitiesProperty )
        .def( "reset", &AbcG::IPolyMeshSchema::reset )
        .def( "valid", &AbcG::IPolyMeshSchema::valid )
        .def( "__nonzero__", &AbcG::IPolyMeshSchema::valid )
        .def( "getFaceSetNames",
              &Overloads::getFaceSetNames,
              ( arg( "oFaceSetNames" ) ) )
        .def( "getFaceSet",
              &AbcG::IPolyMeshSchema::getFaceSet,
              ( arg( "iFaceSetName" ) ),
              with_custodian_and_ward_postcall<0,1>() )
        .def( "hasFaceSet",
              &AbcG::IPolyMeshSchema::hasFaceSet,
              ( arg( "iFaceSetName" ) ) )
        ;

    // IPolyMeshSchema::Sample
    //
    class_<AbcG::IPolyMeshSchema::Sample>( "IPolyMeshSchemaSample", init<>() )
        .def( "getPositions",
              &AbcG::IPolyMeshSchema::Sample::getPositions,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getVelocities",
              &AbcG::IPolyMeshSchema::Sample::getVelocities,
              with_custodian_and_ward_postcall<0,1>() )
         .def( "getFaceIndices",
              &AbcG::IPolyMeshSchema::Sample::getFaceIndices,
              with_custodian_and_ward_postcall<0,1>() )
         .def( "getFaceCounts",
              &AbcG::IPolyMeshSchema::Sample::getFaceCounts,
              with_custodian_and_ward_postcall<0,1>() )
         .def( "getSelfBounds",
              &AbcG::IPolyMeshSchema::Sample::getSelfBounds )
         .def( "valid",
              &AbcG::IPolyMeshSchema::Sample::valid )
         .def( "reset",
              &AbcG::IPolyMeshSchema::Sample::reset )
         .def( "__nonzero__",
              &AbcG::IPolyMeshSchema::Sample::valid )
        ;
}
