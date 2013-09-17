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
#include <PyISchema.h>

using namespace boost::python;

//-*****************************************************************************
void register_icamera()
{
    // ICamera
    //
    register_ISchemaObject<AbcG::ICamera>( "ICamera" );

    // ISchema
    //
    register_ISchema<AbcG::CameraSchemaInfo>( "ISchema_Camera"  );

    // ICamera
    //
    class_<AbcG::ICameraSchema,
           bases<AbcG::ISchema<AbcG::CameraSchemaInfo> > >(
          "ICameraSchema",
          "The ICameraSchema class is a camera schema reader.",
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
        .def( "isConstant", &AbcG::ICameraSchema::isConstant )
        .def( "getNumSamples",
              &AbcG::ICameraSchema::getNumSamples )
        .def( "getTimeSampling",
              &AbcG::ICameraSchema::getTimeSampling )
        .def( "getArbGeomParams", &AbcG::ICameraSchema::getArbGeomParams )
        .def( "getUserProperties", &AbcG::ICameraSchema::getUserProperties )
        .def( "getChildBoundsProperty",
              &AbcG::ICameraSchema::getChildBoundsProperty )
        .def( "getValue", 
              &AbcG::ICameraSchema::getValue,
              ( arg( "iSS" ) = Abc::ISampleSelector() ) )
        .def( "valid", &AbcG::ICameraSchema::valid )
        .def( "reset", &AbcG::ICameraSchema::reset )
        .def( "__nonzero__", &AbcG::ICameraSchema::valid )
        ;
}
