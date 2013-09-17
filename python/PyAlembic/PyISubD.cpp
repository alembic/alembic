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
#include <PyImathStringArray.h>

using namespace boost::python;

//-*****************************************************************************
void register_isubd()
{
    // ISubD
    //
    register_ISchemaObject<AbcG::ISubD>( "ISubD" );

    // IGeomBaseSchema
    //
    register_IGeomBaseSchema<AbcG::SubDSchemaInfo>( "IGeomBaseSchema_SubD" );

    // Overload for OSubDSchema::getFaceSetNames
    //
    struct Overloads
    {
        static object getFaceSetNames( AbcG::ISubDSchema& iSchema )
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

    // ISubDSchema
    //
    class_<AbcG::ISubDSchema,
           bases<AbcG::IGeomBaseSchema<AbcG::SubDSchemaInfo> > >(
          "ISubDSchema",
          "The ISubDSchema class is a subd schema reader.",
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
                   ( arg( "parent" ), arg( "argument" ), arg( "argument" ) ),
                   "doc") )
        .def( "getTopologyVariance",
              &AbcG::ISubDSchema::getTopologyVariance )
        .def( "isConstant",
              &AbcG::ISubDSchema::isConstant )
        .def( "getNumSamples",
              &AbcG::ISubDSchema::getNumSamples )
        .def( "getTimeSampling",
              &AbcG::ISubDSchema::getTimeSampling )
        .def( "getValue",
              &AbcG::ISubDSchema::getValue,
              ( arg( "iSS" ) = Abc::ISampleSelector() ) )
        .def( "getFaceCountsProperty",
              &AbcG::ISubDSchema::getFaceCountsProperty )
        .def( "getFaceIndicesProperty",
              &AbcG::ISubDSchema::getFaceIndicesProperty )
        .def( "getPositionsProperty",
              &AbcG::ISubDSchema::getPositionsProperty )
        .def( "getFaceVaryingInterpolateBoundaryProperty",
              &AbcG::ISubDSchema::getFaceVaryingInterpolateBoundaryProperty )
        .def( "getFaceVaryingPropagateCornersProperty",
              &AbcG::ISubDSchema::getFaceVaryingPropagateCornersProperty )
        .def( "getInterpolateBoundaryProperty",
              &AbcG::ISubDSchema::getInterpolateBoundaryProperty )
        .def( "getCreaseIndicesProperty",
              &AbcG::ISubDSchema::getCreaseIndicesProperty )
        .def( "getCreaseLengthsProperty",
              &AbcG::ISubDSchema::getCreaseLengthsProperty )
        .def( "getCreaseSharpnessesProperty",
              &AbcG::ISubDSchema::getCreaseSharpnessesProperty )
        .def( "getCornerIndicesProperty",
              &AbcG::ISubDSchema::getCornerIndicesProperty )
        .def( "getCornerSharpnessesProperty",
              &AbcG::ISubDSchema::getCornerSharpnessesProperty )
        .def( "getHolesProperty",
              &AbcG::ISubDSchema::getHolesProperty )
        .def( "getSubdivisionSchemeProperty",
              &AbcG::ISubDSchema::getSubdivisionSchemeProperty )
        .def( "getVelocitiesProperty",
              &AbcG::ISubDSchema::getVelocitiesProperty )
        .def( "getUVsParam",
              &AbcG::ISubDSchema::getUVsParam )
        .def( "getFaceSetNames",
              &Overloads::getFaceSetNames,
              ( arg( "faceSetNames" ) ) )
        .def( "getFaceSet",
              &AbcG::ISubDSchema::getFaceSet,
              ( arg( "faceSetName" ) ) )
        .def( "hasFaceSet",
              &AbcG::ISubDSchema::hasFaceSet,
              ( arg( "faceSetName" ) ) )
        .def( "valid", &AbcG::ISubDSchema::valid )
        .def( "reset", &AbcG::ISubDSchema::reset )
        .def( "__nonzero__", &AbcG::ISubDSchema::valid )
        ;

    // ISubD::Sample
    //
    class_<AbcG::ISubDSchema::Sample>( "Sample", init<>() )
        .def( "getPositions",
              &AbcG::ISubDSchema::Sample::getPositions,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getFaceIndices",
              &AbcG::ISubDSchema::Sample::getFaceIndices,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getFaceCounts",
              &AbcG::ISubDSchema::Sample::getFaceCounts,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getFaceVaryingInterpolateBoundary",
              &AbcG::ISubDSchema::Sample::getFaceVaryingInterpolateBoundary )
        .def( "getFaceVaryingPropagateCorners",
              &AbcG::ISubDSchema::Sample::getFaceVaryingPropagateCorners )
        .def( "getInterpolateBoundary",
              &AbcG::ISubDSchema::Sample::getInterpolateBoundary )
        .def( "getCreaseIndices",
              &AbcG::ISubDSchema::Sample::getCreaseIndices,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getCreaseLengths",
              &AbcG::ISubDSchema::Sample::getCreaseLengths,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getCreaseSharpnesses",
              &AbcG::ISubDSchema::Sample::getCreaseSharpnesses,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getCornerIndices",
              &AbcG::ISubDSchema::Sample::getCornerIndices,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getCornerSharpnesses",
              &AbcG::ISubDSchema::Sample::getCornerSharpnesses,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getHoles",
              &AbcG::ISubDSchema::Sample::getHoles,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getSubdivisionScheme",
              &AbcG::ISubDSchema::Sample::getSubdivisionScheme )
        .def( "getVelocities",
              &AbcG::ISubDSchema::Sample::getVelocities,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getSelfBounds", &AbcG::ISubDSchema::Sample::getSelfBounds )
        .def( "valid", &AbcG::ISubDSchema::Sample::valid )
        .def( "reset", &AbcG::ISubDSchema::Sample::reset )
        ;
}
