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
#include <PyIGeomParam.h>

void register_igeomparam_matrix_quad_color_normal()
{
    register_<Abc::M33fTPTraits>        ( "IM33fGeomParam" );
    register_<Abc::M33dTPTraits>        ( "IM33dGeomParam" );
    register_<Abc::M44fTPTraits>        ( "IM44fGeomParam" );
    register_<Abc::M44dTPTraits>        ( "IM44dGeomParam" );

    register_<Abc::QuatfTPTraits>       ( "IQuatfGeomParam" );
    register_<Abc::QuatdTPTraits>       ( "IQuatdGeomParam" );

    register_<Abc::C3hTPTraits>         ( "IC3hGeomParam" );
    register_<Abc::C3fTPTraits>         ( "IC3fGeomParam" );
    register_<Abc::C3cTPTraits>         ( "IC3cGeomParam" );

    register_<Abc::C4hTPTraits>         ( "IC4hGeomParam" );
    register_<Abc::C4fTPTraits>         ( "IC4fGeomParam" );
    register_<Abc::C4cTPTraits>         ( "IC4cGeomParam" );

    register_<Abc::N2fTPTraits>         ( "IN2fGeomParam" );
    register_<Abc::N2dTPTraits>         ( "IN2dGeomParam" );

    register_<Abc::N3fTPTraits>         ( "IN3fGeomParam" );
    register_<Abc::N3dTPTraits>         ( "IN3dGeomParam" );
}
