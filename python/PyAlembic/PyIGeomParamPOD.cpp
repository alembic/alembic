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

void register_igeomparam_pod()
{
    register_<Abc::BooleanTPTraits>     ( "IBooleanGeomParam" );
    register_<Abc::Uint8TPTraits>       ( "IUcharGeomParam" );
    register_<Abc::Int8TPTraits>        ( "ICharGeomParam" );
    register_<Abc::Uint16TPTraits>      ( "IUInt16GeomParam" );
    register_<Abc::Int16TPTraits>       ( "IInt16GeomParam" );
    register_<Abc::Uint32TPTraits>      ( "IUInt32GeomParam" );
    register_<Abc::Int32TPTraits>       ( "IInt32GeomParam" );
    register_<Abc::Uint64TPTraits>      ( "IUInt64GeomParam" );
    register_<Abc::Int64TPTraits>       ( "IInt64GeomParam" );
    register_<Abc::Float16TPTraits>     ( "IHalfGeomParam" );
    register_<Abc::Float32TPTraits>     ( "IFloatGeomParam" );
    register_<Abc::Float64TPTraits>     ( "IDoubleGeomParam" );
    register_<Abc::StringTPTraits>      ( "IStringGeomParam" );
    register_<Abc::WstringTPTraits>     ( "IWstringGeomParam" );
}
