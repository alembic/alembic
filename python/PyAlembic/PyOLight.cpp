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
#include <PyOSchema.h>

using namespace boost::python;

//-*****************************************************************************
void register_olight()
{
    // OLight
    //
    register_OSchemaObject<AbcG::OLight>( "OLight" );

    // OSchema
    //
    register_OSchema<AbcG::LightSchemaInfo>( "OSchema_Light" );

    // Overloads
    //
    void ( AbcG::OLightSchema::*setTimeSamplingByIndex )( AbcU::uint32_t )
        = &AbcG::OLightSchema::setTimeSampling;
    void ( AbcG::OLightSchema::*setTimeSamplingByTimeSamplingPtr )
        ( AbcA::TimeSamplingPtr ) = &AbcG::OLightSchema::setTimeSampling;

    // OLightSchema
    //
    class_<AbcG::OLightSchema,
           bases<AbcG::OSchema<AbcG::LightSchemaInfo> > >(
          "OLightSchema",
          "The OLightSchema class is a light schema writer",
          init<>() )
        .def( "getTimeSampling",
              &AbcG::OLightSchema::getTimeSampling )
        .def( "getNumSamples",
              &AbcG::OLightSchema::getNumSamples )
        .def( "setCameraSample",
              &AbcG::OLightSchema::setCameraSample,
              ( arg( "iSamp" ) ) )
        .def( "setFromPrevious",
              &AbcG::OLightSchema::setFromPrevious )
        .def( "setTimeSampling",
              setTimeSamplingByIndex,
              ( arg( "index" ) ),
              "Change the TimeSampling used by this property with the given "
              "index" )
        .def( "setTimeSampling",
              setTimeSamplingByTimeSamplingPtr,
              ( arg( "TimeSampling" ) ),
              "Change the TimeSampling used by this property with the given "
              "TimeSampling" )
        .def( "getUserProperties",
              &AbcG::OLightSchema::getUserProperties )
        .def( "getArbGeomParams",
              &AbcG::OLightSchema::getArbGeomParams )
        .def( "getChildBoundsProperty",
              &AbcG::OLightSchema::getChildBoundsProperty )
        .def( "reset", &AbcG::OLightSchema::reset )
        .def( "valid", &AbcG::OLightSchema::valid )
        .def( "__nonzero__", &AbcG::OLightSchema::valid )
        ;
}
