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
#include <PyISchema.h>
#include <PyISchemaObject.h>

using namespace boost::python;

//-*****************************************************************************
void register_ixform()
{
    // IXform
    //
    register_ISchemaObject<AbcG::IXform>( "IXform" );

    // ISchema base class
    //
    register_ISchema<AbcG::XformSchemaInfo>( "ISchema_Xform" );

    // IXformSchema
    //
    class_<AbcG::IXformSchema,
           bases<Abc::ISchema<AbcG::XformSchemaInfo> > >(
           "IXformSchema",
           "The IXformSchema class is a xform schema reader",
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
        .def( "getTimeSampling", 
              &AbcG::IXformSchema::getTimeSampling )
        .def( "isConstant",
              &AbcG::IXformSchema::isConstant )
        .def( "isConstantIdentity",
              &AbcG::IXformSchema::isConstantIdentity )
        .def( "getNumSamples",
              &AbcG::IXformSchema::getNumSamples )
        .def( "getValue",
              &AbcG::IXformSchema::getValue,
              ( arg( "iSS" ) = Abc::ISampleSelector() ) )
        .def( "getChildBoundsProperty",
              &AbcG::IXformSchema::getChildBoundsProperty )
        .def( "getInheritsXforms",
              &AbcG::IXformSchema::getInheritsXforms,
              ( arg( "iSS" ) = Abc::ISampleSelector() ),
              "lightweight get to avoid constructing a sample" )
        .def( "getNumOps",
              &AbcG::IXformSchema::getNumOps )
        .def( "getArbGeomParams",
              &AbcG::IXformSchema::getArbGeomParams )
        .def( "getUserProperties",
              &AbcG::IXformSchema::getUserProperties )
        .def( "valid", &AbcG::IXformSchema::valid )
        .def( "reset", &AbcG::IXformSchema::reset )
        .def( "__nonzero__", &AbcG::IXformSchema::valid )
        ;
}
