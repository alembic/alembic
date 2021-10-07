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

void register_igeomparam_box(py::module_& module_handle)
{
    register_<AbcG::IBox2sGeomParam>  ( module_handle, "IBox2sGeomParam" );
    register_<AbcG::IBox2iGeomParam>  ( module_handle, "IBox2iGeomParam" );
    register_<AbcG::IBox2fGeomParam>  ( module_handle, "IBox2fGeomParam" );
    register_<AbcG::IBox2dGeomParam>  ( module_handle, "IBox2dGeomParam" );

    register_<AbcG::IBox3sGeomParam>  ( module_handle, "IBox3sGeomParam" );
    register_<AbcG::IBox3iGeomParam>  ( module_handle, "IBox3iGeomParam" );
    register_<AbcG::IBox3fGeomParam>  ( module_handle, "IBox3fGeomParam" );
    register_<AbcG::IBox3dGeomParam>  ( module_handle, "IBox3dGeomParam" );
}
