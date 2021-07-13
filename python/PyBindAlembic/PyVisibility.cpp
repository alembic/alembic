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

using namespace py;

//-*****************************************************************************
void register_visibility( py::module_& module_handle )
{
    // ObjectVisibility
    //
    enum_<AbcG::ObjectVisibility>( module_handle, "ObjectVisibility" )
        .value( "kVisibilityDeferred", AbcG::kVisibilityDeferred )
        .value( "kVisibilityHidden", AbcG::kVisibilityHidden )
        .value( "kkVisibilityVisible", AbcG::kVisibilityVisible )
        .export_values()
        ;

    // For Writer code
    // Overloads
    AbcG::OVisibilityProperty ( *CreateVisibilityPropertyByIndex )
        ( Abc::OObject&, AbcU::uint32_t ) =
            &AbcG::CreateVisibilityProperty;
    AbcG::OVisibilityProperty ( *CreateVisibilityPropertyByTimeSamplingPtr )
        ( Abc::OObject&, AbcA::TimeSamplingPtr ) =
            &AbcG::CreateVisibilityProperty;

    module_handle.def( "CreateVisibilityProperty",
         CreateVisibilityPropertyByIndex,
         arg( "iObject" ), arg( "iTimeSamplingIndex" ) );
    module_handle.def( "CreateVisibilityProperty",
         CreateVisibilityPropertyByTimeSamplingPtr,
         arg( "IObject" ), arg( "iTimeSamplingPtr" ) );

    // For Reader code
    module_handle.def( "GetVisibilityProperty",
         AbcG::GetVisibilityProperty,
         arg( "schemaObject" ) );
    module_handle.def( "GetVisibility",
         AbcG::GetVisibility,
         arg( "schemaObject" ), arg( "iSS" ) = Abc::ISampleSelector() );
    module_handle.def( "IsAncestorInvisible",
         AbcG::IsAncestorInvisible,
         arg( "schemaObject" ), arg( "iSS" ) = Abc::ISampleSelector() );
}
