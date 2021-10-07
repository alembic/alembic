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
#include <PyTypeBindingTraits.h>

using namespace py;

//-*****************************************************************************
template<class TPTraits>
py::object getArrayType()
{
    typedef typename TPTraits::value_type val_type;
    std::vector<val_type> type_data;

    return py::object( py::cast( type_data, return_value_policy::take_ownership ).attr( "__class__" ));
}

//-*****************************************************************************
#define REGISTER_TPTRAITS( module_handle, TPTraits )                                      \
{                                                                                         \
    py::object scope = class_<Abc::TPTraits> ( module_handle, #TPTraits ) .def( init<>() )  \
      .def_static( "interpretation", &Abc::TPTraits::interpretation,                        \
      "Return the interpretation string" )                                                  \
      .def_static( "dataType", &Abc::TPTraits::dataType,                                    \
      "Return the DataType of this property type" )                                         \
      ;                                                                                     \
    scope.attr( "arrayType" ) = getArrayType<Abc::TPTraits>();                            \
}

//-*****************************************************************************
void register_typedpropertytraits(py::module_& module_handle)
{
    // Typed Property Traits
    REGISTER_TPTRAITS( module_handle, BooleanTPTraits )
    REGISTER_TPTRAITS( module_handle, Uint8TPTraits )
    REGISTER_TPTRAITS( module_handle, Int8TPTraits )
    REGISTER_TPTRAITS( module_handle, Uint16TPTraits )
    REGISTER_TPTRAITS( module_handle, Int16TPTraits )
    REGISTER_TPTRAITS( module_handle, Uint32TPTraits )
    REGISTER_TPTRAITS( module_handle, Int32TPTraits )
    REGISTER_TPTRAITS( module_handle, Uint64TPTraits )
    REGISTER_TPTRAITS( module_handle, Int64TPTraits )
    REGISTER_TPTRAITS( module_handle, Float16TPTraits )
    REGISTER_TPTRAITS( module_handle, Float32TPTraits )
    REGISTER_TPTRAITS( module_handle, Float64TPTraits )
    REGISTER_TPTRAITS( module_handle, StringTPTraits )
    REGISTER_TPTRAITS( module_handle, WstringTPTraits )

    REGISTER_TPTRAITS( module_handle, V2sTPTraits )
    REGISTER_TPTRAITS( module_handle, V2iTPTraits )
    REGISTER_TPTRAITS( module_handle, V2fTPTraits )
    REGISTER_TPTRAITS( module_handle, V2dTPTraits )
    REGISTER_TPTRAITS( module_handle, P2sTPTraits )
    REGISTER_TPTRAITS( module_handle, P2iTPTraits )
    REGISTER_TPTRAITS( module_handle, P2fTPTraits )
    REGISTER_TPTRAITS( module_handle, P2dTPTraits )
    REGISTER_TPTRAITS( module_handle, N2fTPTraits )
    REGISTER_TPTRAITS( module_handle, N2dTPTraits )

    REGISTER_TPTRAITS( module_handle, C3cTPTraits )
    REGISTER_TPTRAITS( module_handle, C3hTPTraits )
    REGISTER_TPTRAITS( module_handle, C3fTPTraits )
    REGISTER_TPTRAITS( module_handle, V3sTPTraits )
    REGISTER_TPTRAITS( module_handle, V3iTPTraits )
    REGISTER_TPTRAITS( module_handle, V3fTPTraits )
    REGISTER_TPTRAITS( module_handle, V3dTPTraits )
    REGISTER_TPTRAITS( module_handle, P3sTPTraits )
    REGISTER_TPTRAITS( module_handle, P3iTPTraits )
    REGISTER_TPTRAITS( module_handle, P3fTPTraits )
    REGISTER_TPTRAITS( module_handle, P3dTPTraits )
    REGISTER_TPTRAITS( module_handle, N3fTPTraits )
    REGISTER_TPTRAITS( module_handle, N3dTPTraits )

    REGISTER_TPTRAITS( module_handle, C4cTPTraits )
    REGISTER_TPTRAITS( module_handle, C4hTPTraits )
    REGISTER_TPTRAITS( module_handle, C4fTPTraits )

    REGISTER_TPTRAITS( module_handle, Box2sTPTraits )
    REGISTER_TPTRAITS( module_handle, Box2iTPTraits )
    REGISTER_TPTRAITS( module_handle, Box2fTPTraits )
    REGISTER_TPTRAITS( module_handle, Box2dTPTraits )
    REGISTER_TPTRAITS( module_handle, QuatfTPTraits )
    REGISTER_TPTRAITS( module_handle, QuatdTPTraits )

    REGISTER_TPTRAITS( module_handle, Box3sTPTraits )
    REGISTER_TPTRAITS( module_handle, Box3iTPTraits )
    REGISTER_TPTRAITS( module_handle, Box3fTPTraits )
    REGISTER_TPTRAITS( module_handle, Box3dTPTraits )

    REGISTER_TPTRAITS( module_handle, M33fTPTraits )
    REGISTER_TPTRAITS( module_handle, M33dTPTraits )

    REGISTER_TPTRAITS( module_handle, M44fTPTraits )
    REGISTER_TPTRAITS( module_handle, M44dTPTraits )
}
