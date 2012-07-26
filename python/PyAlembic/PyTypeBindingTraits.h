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

#ifndef _PyAlembic_PyTypeBindingTraits_h_
#define _PyAlembic_PyTypeBindingTraits_h_

#include <Alembic/Util/PlainOldDataType.h>
#include <Foundation.h>

#include <PyImathFixedArray.h>
#include <PyImathExport.h>
#include <PyImathVec.h>
#include <PyImathMatrix.h>
#include <PyImathColor.h>
#include <PyImathStringArray.h>
#include <PyImathBox.h>

// Unsupported POD and array types in PyImath
// bool8, int64, uint64, half, C3h, C4h

namespace {

using namespace PyImath;

//-*****************************************************************************
// Binding Traits from alembic TPTraits to python fixedarrays
//-*****************************************************************************
template<class TPTraits>
struct TypeBindingTraits;

#define DECLARE_TYPE_BINDING_TRAITS( TPTraits, ArrayType, MemCopyable ) \
template<>                                                              \
struct TypeBindingTraits< TPTraits >                                    \
{                                                                       \
    typedef TPTraits::value_type        native_value_type;              \
    typedef ArrayType::BaseType         python_value_type;              \
    typedef ArrayType                   python_array_type;              \
    static const bool                   memCopyable = MemCopyable;      \
};

//-*****************************************************************************
// Actual specializations
DECLARE_TYPE_BINDING_TRAITS( Abc::BooleanTPTraits, BoolArray,          false )
DECLARE_TYPE_BINDING_TRAITS( Abc::Uint8TPTraits,   UnsignedCharArray,  true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Int8TPTraits,    SignedCharArray,    true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Uint16TPTraits,  UnsignedShortArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Int16TPTraits,   ShortArray,         true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Uint32TPTraits,  UnsignedIntArray,   true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Int32TPTraits,   IntArray,           true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Uint64TPTraits,  IntArray,           false )
DECLARE_TYPE_BINDING_TRAITS( Abc::Int64TPTraits,   IntArray,           false )
DECLARE_TYPE_BINDING_TRAITS( Abc::Float16TPTraits, FloatArray,         false )
DECLARE_TYPE_BINDING_TRAITS( Abc::Float32TPTraits, FloatArray,         true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Float64TPTraits, DoubleArray,        true )
DECLARE_TYPE_BINDING_TRAITS( Abc::StringTPTraits,  StringArray,        false )
DECLARE_TYPE_BINDING_TRAITS( Abc::WstringTPTraits, WstringArray,       false )

DECLARE_TYPE_BINDING_TRAITS( Abc::V2sTPTraits, V2sArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::V2iTPTraits, V2iArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::V2fTPTraits, V2fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::V2dTPTraits, V2dArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::P2sTPTraits, V2sArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::P2iTPTraits, V2iArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::P2fTPTraits, V2fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::P2dTPTraits, V2dArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::N2fTPTraits, V2fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::N2dTPTraits, V2dArray, true )

DECLARE_TYPE_BINDING_TRAITS( Abc::C3cTPTraits, C3cArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::C3hTPTraits, C3fArray, false )
DECLARE_TYPE_BINDING_TRAITS( Abc::C3fTPTraits, C3fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::V3sTPTraits, V3sArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::V3iTPTraits, V3iArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::V3fTPTraits, V3fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::V3dTPTraits, V3dArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::P3sTPTraits, V3sArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::P3iTPTraits, V3iArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::P3fTPTraits, V3fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::P3dTPTraits, V3dArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::N3fTPTraits, V3fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::N3dTPTraits, V3dArray, true )

DECLARE_TYPE_BINDING_TRAITS( Abc::C4cTPTraits, C4cArray, true  )
DECLARE_TYPE_BINDING_TRAITS( Abc::C4hTPTraits, C4fArray, false )
DECLARE_TYPE_BINDING_TRAITS( Abc::C4fTPTraits, C4fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Box2sTPTraits, Box2sArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Box2iTPTraits, Box2iArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Box2fTPTraits, Box2fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Box2dTPTraits, Box2dArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::QuatfTPTraits, QuatfArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::QuatdTPTraits, QuatdArray, true )

DECLARE_TYPE_BINDING_TRAITS( Abc::Box3sTPTraits, Box3sArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Box3iTPTraits, Box3iArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Box3fTPTraits, Box3fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::Box3dTPTraits, Box3dArray, true )

DECLARE_TYPE_BINDING_TRAITS( Abc::M33fTPTraits, M33fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::M33dTPTraits, M33dArray, true )

DECLARE_TYPE_BINDING_TRAITS( Abc::M44fTPTraits, M44fArray, true )
DECLARE_TYPE_BINDING_TRAITS( Abc::M44dTPTraits, M44dArray, true )

//-*****************************************************************************
// POD TPTraits from Enums
//-*****************************************************************************
template <AbcU::PlainOldDataType PENUM>
struct PODTPTraitsFromEnum;

#define DECLARE_PODTPTRAITS_FROM_ENUM( PEnum, TPTraits ) \
template<>                                            \
struct PODTPTraitsFromEnum< PEnum >                   \
{                                                     \
    typedef TPTraits        traits_type;              \
};

//-*****************************************************************************
// Actual specializations
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kBooleanPOD , Abc::BooleanTPTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kUint8POD   , Abc::Uint8PODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kInt8POD    , Abc::Int8PODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kUint16POD  , Abc::Uint16PODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kInt16POD   , Abc::Int16PODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kUint32POD  , Abc::Uint32PODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kInt32POD   , Abc::Int32PODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kUint64POD  , Abc::Uint64PODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kInt64POD   , Abc::Int64PODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kFloat16POD , Abc::Float16PODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kFloat32POD , Abc::Float32PODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kFloat64POD , Abc::Float64PODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kStringPOD  , Abc::StringPODTraits );
DECLARE_PODTPTRAITS_FROM_ENUM( AbcU::kWstringPOD , Abc::WstringPODTraits );

} // namespace

#endif
