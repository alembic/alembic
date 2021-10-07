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
#include <PyIGeomParam.h>

void register_igeomparam_pod(py::module_& module_handle)
{
    register_<AbcG::IBoolGeomParam>        ( module_handle, "IBooleanGeomParam" );
    register_<AbcG::IUcharGeomParam>       ( module_handle, "IUcharGeomParam" );
    register_<AbcG::ICharGeomParam>        ( module_handle, "ICharGeomParam" );
    register_<AbcG::IUInt16GeomParam>      ( module_handle, "IUInt16GeomParam" );
    register_<AbcG::IInt16GeomParam>       ( module_handle, "IInt16GeomParam" );
    register_<AbcG::IUInt32GeomParam>      ( module_handle, "IUInt32GeomParam" );
    register_<AbcG::IInt32GeomParam>       ( module_handle, "IInt32GeomParam" );
    register_<AbcG::IUInt64GeomParam>      ( module_handle, "IUInt64GeomParam" );
    register_<AbcG::IInt64GeomParam>       ( module_handle, "IInt64GeomParam" );
    register_<AbcG::IHalfGeomParam>        ( module_handle, "IHalfGeomParam" );
    register_<AbcG::IFloatGeomParam>       ( module_handle, "IFloatGeomParam" );
    register_<AbcG::IDoubleGeomParam>      ( module_handle, "IDoubleGeomParam" );
    register_<AbcG::IStringGeomParam>      ( module_handle, "IStringGeomParam" );
    register_<AbcG::IWstringGeomParam>     ( module_handle, "IWstringGeomParam" );
}
