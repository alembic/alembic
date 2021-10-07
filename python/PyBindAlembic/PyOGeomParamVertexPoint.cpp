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
#include <PyOGeomParam.h>

void register_ogeomparam_vertex_point(py::module_& module_handle)
{
    register_<Abc::V2sTPTraits>         ( module_handle, "OV2sGeomParam" );
    register_<Abc::V2iTPTraits>         ( module_handle, "OV2iGeomParam" );
    register_<Abc::V2fTPTraits>         ( module_handle, "OV2fGeomParam" );
    register_<Abc::V2dTPTraits>         ( module_handle, "OV2dGeomParam" );

    register_<Abc::V3sTPTraits>         ( module_handle, "OV3sGeomParam" );
    register_<Abc::V3iTPTraits>         ( module_handle, "OV3iGeomParam" );
    register_<Abc::V3fTPTraits>         ( module_handle, "OV3fGeomParam" );
    register_<Abc::V3dTPTraits>         ( module_handle, "OV3dGeomParam" );

    register_<Abc::P2sTPTraits>         ( module_handle, "OP2sGeomParam" );
    register_<Abc::P2iTPTraits>         ( module_handle, "OP2iGeomParam" );
    register_<Abc::P2fTPTraits>         ( module_handle, "OP2fGeomParam" );
    register_<Abc::P2dTPTraits>         ( module_handle, "OP2dGeomParam" );

    register_<Abc::P3sTPTraits>         ( module_handle, "OP3sGeomParam" );
    register_<Abc::P3iTPTraits>         ( module_handle, "OP3iGeomParam" );
    register_<Abc::P3fTPTraits>         ( module_handle, "OP3fGeomParam" );
    register_<Abc::P3dTPTraits>         ( module_handle, "OP3dGeomParam" );
}
