//-*****************************************************************************
//
// Copyright (c) 2012-2014,
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
#include <PyISchema.h>
#include <PyIGeomBaseSchema.h>
#include <PyISchemaObject.h>

using namespace py;

void register_ipoints(py::module_& module_handle)
{
    // IPoints
    //
    register_ISchemaObject<AbcG::IPoints>( module_handle, "IPoints" );

    // IGeomBaseSchema
    //
    register_IGeomBaseSchema<AbcG::PointsSchemaInfo>( module_handle, "IGeomBaseSchema_Points" );

    // IPointsSchema
    //
    class_<AbcG::IPointsSchema, AbcG::IGeomBaseSchema<AbcG::PointsSchemaInfo> >(
        module_handle,
          "IPointsSchema",
          "The IPointsSchema class is a points schema reader." )
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
                   arg( "parent" ), arg( "argument" ), arg( "argument" ),
                   "doc")
        .def( "isConstant",
              &AbcG::IPointsSchema::isConstant )
        .def( "getNumSamples",
              &AbcG::IPointsSchema::getNumSamples )
        .def( "getPositionsProperty",
              &AbcG::IPointsSchema::getPositionsProperty )
        .def( "getVelocitiesProperty",
              &AbcG::IPointsSchema::getVelocitiesProperty )
        .def( "getIdsProperty",
              &AbcG::IPointsSchema::getIdsProperty )
        .def( "getWidthsParam",
              &AbcG::IPointsSchema::getWidthsParam )
        .def( "getValue",
              &AbcG::IPointsSchema::getValue,
              arg( "iSS" ) = Abc::ISampleSelector() )
        .def( "getTimeSampling",
              &AbcG::IPointsSchema::getTimeSampling )
        .def( "valid", &AbcG::IPointsSchema::valid )
        .def( "reset", &AbcG::IPointsSchema::reset )
        .def( ALEMBIC_PYTHON_BOOL_NAME, &AbcG::IPointsSchema::valid )
        ;

    // IPoints::Sample
    //
    class_<AbcG::IPointsSchema::Sample>(
        module_handle,
        "IPointsSchemaSample" )
        .def( init<>() )
        .def( "getPositions",
              &AbcG::IPointsSchema::Sample::getPositions,
              keep_alive<0,1>() )
        .def( "getIds",
              &AbcG::IPointsSchema::Sample::getIds,
              keep_alive<0,1>() )
        .def( "getVelocities",
              &AbcG::IPointsSchema::Sample::getVelocities,
              keep_alive<0,1>() )
        .def( "getSelfBounds",
              &AbcG::IPointsSchema::Sample::getSelfBounds )
        .def( "valid", &AbcG::IPointsSchema::Sample::valid )
        .def( "reset", &AbcG::IPointsSchema::Sample::reset )
        ;
}
