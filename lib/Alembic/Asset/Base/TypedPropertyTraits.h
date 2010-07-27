//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#ifndef _Alembic_Asset_Base_TypedPropertyTraits_h_
#define _Alembic_Asset_Base_TypedPropertyTraits_h_

#include <Alembic/Util/PlainOldDataType.h>
#include <Alembic/Asset/Base/DataType.h>
#include <Alembic/Asset/Base/FoundationBase.h>
#include <string>

//-*****************************************************************************
// The property traits class maps ValueTypes with their protocol
// strings.

namespace Alembic {
namespace Asset {

//-*****************************************************************************
//-*****************************************************************************
template <class VALUE_TYPE, PlainOldDataType POD, size_t EXTENT>
struct TypedPropertyTraits {};

//-*****************************************************************************
//-*****************************************************************************
#define DECLARE_POD_TRAITS( POD_TYPE , PTDEF )                          \
template <>                                                             \
struct TypedPropertyTraits< POD_TYPE,                                   \
                            PODTraitsFromType< POD_TYPE >::pod_enum, 1 > \
{                                                                       \
    typedef POD_TYPE    value_type;                                     \
    static const char * protocol() throw() { return ""; }               \
    static DataType     dataType() throw()                              \
    { return DataType( PODTraitsFromType< POD_TYPE >::pod_enum, 1 ); }  \
    static value_type   defaultValue() throw()                          \
    { return PODTraitsFromType< POD_TYPE >::default_value(); }          \
};                                                                      \
typedef TypedPropertyTraits< POD_TYPE ,                                 \
                             PODTraitsFromType< POD_TYPE >::pod_enum,   \
                             1 > PTDEF

//-*****************************************************************************
DECLARE_POD_TRAITS( uint8_t, Uint8TPTraits );
DECLARE_POD_TRAITS( int8_t, Int8TPTraits );
DECLARE_POD_TRAITS( uint16_t, Uint16TPTraits );
DECLARE_POD_TRAITS( int16_t, Int16TPTraits );
DECLARE_POD_TRAITS( uint32_t, Uint32TPTraits );
DECLARE_POD_TRAITS( int32_t, Int32TPTraits );
DECLARE_POD_TRAITS( uint64_t, Uint64TPTraits );
DECLARE_POD_TRAITS( int64_t, Int64TPTraits );
DECLARE_POD_TRAITS( float32_t, Float32TPTraits );
DECLARE_POD_TRAITS( float64_t, Float64TPTraits );

#undef DECLARE_POD_TRAITS

//-*****************************************************************************
//-*****************************************************************************
// This macro relies on there being a default constructor for the type
// VAL
#define DECLARE_SPEC_TRAITS( VAL, POD, EXTENT, PROT, PTDEF )            \
template <>                                                             \
struct TypedPropertyTraits< VAL , POD , EXTENT >                        \
{                                                                       \
    typedef VAL         value_type;                                     \
    static const char * protocol() throw() { return ( PROT ) ; }        \
    static DataType     dataType() throw()                              \
    { return DataType( POD, EXTENT ) ; }                                \
    static value_type   defaultValue() throw()                          \
    { value_type v; return v; }                                         \
};                                                                      \
typedef TypedPropertyTraits< VAL , POD , EXTENT > PTDEF

//-*****************************************************************************
DECLARE_SPEC_TRAITS( V2s, kInt16POD, 2, "vec", V2sTPTraits );
DECLARE_SPEC_TRAITS( V2i, kInt32POD, 2, "vec", V2iTPTraits );
DECLARE_SPEC_TRAITS( V2f, kFloat32POD, 2, "vec", V2fTPTraits );
DECLARE_SPEC_TRAITS( V2d, kFloat64POD, 2, "vec", V2dTPTraits );

DECLARE_SPEC_TRAITS( V3s, kInt16POD, 3, "vec", V3sTPTraits );
DECLARE_SPEC_TRAITS( V3i, kInt32POD, 3, "vec", V3iTPTraits );
DECLARE_SPEC_TRAITS( V3f, kFloat32POD, 3, "vec", V3fTPTraits );
DECLARE_SPEC_TRAITS( V3d, kFloat64POD, 3, "vec", V3dTPTraits );

DECLARE_SPEC_TRAITS( Box2s, kInt16POD, 4, "box", Box2sTPTraits );
DECLARE_SPEC_TRAITS( Box2i, kInt32POD, 4, "box", Box2iTPTraits );
DECLARE_SPEC_TRAITS( Box2f, kFloat32POD, 4, "box", Box2fTPTraits );
DECLARE_SPEC_TRAITS( Box2d, kFloat64POD, 4, "box", Box2dTPTraits );

DECLARE_SPEC_TRAITS( Box3s, kInt16POD, 6, "box", Box3sTPTraits );
DECLARE_SPEC_TRAITS( Box3i, kInt32POD, 6, "box", Box3iTPTraits );
DECLARE_SPEC_TRAITS( Box3f, kFloat32POD, 6, "box", Box3fTPTraits );
DECLARE_SPEC_TRAITS( Box3d, kFloat64POD, 6, "box", Box3dTPTraits );

DECLARE_SPEC_TRAITS( M33f, kFloat32POD, 9, "matrix", M33fTPTraits );
DECLARE_SPEC_TRAITS( M33d, kFloat64POD, 9, "matrix", M33dTPTraits );
DECLARE_SPEC_TRAITS( M44f, kFloat32POD, 16, "matrix", M44fTPTraits );
DECLARE_SPEC_TRAITS( M44d, kFloat64POD, 16, "matrix", M44dTPTraits );

DECLARE_SPEC_TRAITS( Quatf, kFloat32POD, 4, "quat", QuatfTPTraits );
DECLARE_SPEC_TRAITS( Quatd, kFloat64POD, 4, "quat", QuatdTPTraits );

#undef DECLARE_SPEC_TRAITS
//-*****************************************************************************

} // End namespace Asset
} // End namespace Alembic

#endif
