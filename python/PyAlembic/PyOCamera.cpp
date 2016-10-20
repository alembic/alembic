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
void register_ocamera()
{
    // OCamera
    //
    register_OSchemaObject<AbcG::OCamera>( "OCamera" );

    // OGeomBaseSchema
    //
    register_OGeomBaseSchema<AbcG::CameraSchemaInfo>(
                                            "OGeomBaseSchema_Camera" );

    // Overloads for OCameraSchema
    //
    void ( AbcG::OCameraSchema::*setTimeSamplingByIndex )( AbcU::uint32_t )
        = &AbcG::OCameraSchema::setTimeSampling;
    void ( AbcG::OCameraSchema::*setTimeSamplingByTimeSamplingPtr )
        ( AbcA::TimeSamplingPtr ) = &AbcG::OCameraSchema::setTimeSampling;

    // OCameraSchema
    //
    class_<AbcG::OCameraSchema,
           bases<AbcG::OGeomBaseSchema<AbcG::CameraSchemaInfo> > >(
          "OCameraDSchema",
          "The OCameraSchema class is a camera schema writer.",
          init<>() )
        .def( "getTimeSampling",
              &AbcG::OCameraSchema::getTimeSampling )
        .def( "getNumSamples",
              &AbcG::OCameraSchema::getNumSamples )
        .def( "getUserProperties", &AbcG::OCameraSchema::getUserProperties )
        .def( "getArbGeomParams", &AbcG::OCameraSchema::getArbGeomParams )
        .def( "getChildBoundsProperty",
              &AbcG::OCameraSchema::getChildBoundsProperty )
        .def( "set",
              &AbcG::OCameraSchema::set,
              ( arg( "iSamp" ) ) )
        .def( "setFromPrevious",
              &AbcG::OCameraSchema::setFromPrevious )
        .def( "setTimeSampling",
              setTimeSamplingByIndex,
              ( arg( "index" ) ) )
        .def( "setTimeSampling",
              setTimeSamplingByTimeSamplingPtr,
              ( arg( "timeSampling" ) ) )
        .def( "valid", &AbcG::OCameraSchema::valid )
        .def( "reset", &AbcG::OCameraSchema::reset )
        .def( "__nonzero__", &AbcG::OCameraSchema::valid )
        ;
}
