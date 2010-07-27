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

#ifndef _SimpleGtoViewer_Foundation_h_
#define _SimpleGtoViewer_Foundation_h_

#include <Alembic/Util/Util.h>
#include <Alembic/GLUtil/GLUtil.h>

#include <OpenEXR/ImathMath.h>
#include <OpenEXR/ImathVec.h>
#include <OpenEXR/ImathMatrix.h>
#include <OpenEXR/ImathBox.h>
#include <OpenEXR/ImathQuat.h>
#include <OpenEXR/ImathColor.h>
#include <OpenEXR/ImathFun.h>

#include <GtoContainer/Foundation.h>
#include <GtoContainer/StdProperties.h>
#include <GtoContainer/ObjectVector.h>
#include <GtoContainer/Reader.h>
#include <GtoContainer/Property.h>

#include <Gto/Protocols.h>

#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <utility>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

namespace SimpleGtoViewer {

//-*****************************************************************************
// Just typedefs.
//-*****************************************************************************

typedef Imath::V2d V2d;
typedef Imath::V2f V2f;
typedef Imath::V2i V2i;
// typedef Imath::Vec2<half> V2h;

typedef Imath::V3d V3d;
typedef Imath::V3f V3f;
typedef Imath::V3i V3i;
// typedef Imath::Vec3<half> V3h;

typedef Imath::Color3f C3f;
//typedef Imath::Color3h C3h;
typedef Imath::Color3c C3c;

typedef Imath::Color4f C4f;
//typedef Imath::Color4h C4h;
typedef Imath::Color4c C4c;

typedef Imath::Box2d B2d;
typedef Imath::Box2f B2f;
typedef Imath::Box2i B2i;
//typedef Imath::Box< Imath::Vec2< half > > B2h;

typedef Imath::Box3d B3d;
typedef Imath::Box3f B3f;
typedef Imath::Box3i B3i;
//typedef Imath::Box< Imath::Vec3< half > > B3h;

typedef Imath::Quatd Quatd;
typedef Imath::Quatf Quatf;

typedef Imath::M44f M44f;
typedef Imath::M44d M44d;

typedef Imath::M33f M33f;
typedef Imath::M33d M33d;

//-*****************************************************************************
//-*****************************************************************************
// CONVERTERS
//-*****************************************************************************
//-*****************************************************************************

inline V2d& toV2d( GtoContainer::double2 &d ) { return *( ( V2d * )( &d ) ); }
inline V2f& toV2f( GtoContainer::float2 &d ) { return *( ( V2f * )( &d ) ); }
inline V2i& toV2i( GtoContainer::int2 &d ) { return *( ( V2i * )( &d ) ); }
// inline V2h& toV2h( GtoContainer::half2 &d ) { return *( ( V2h * )( &d ) ); }

inline V3d& toV3d( GtoContainer::double3 &d ) { return *( ( V3d * )( &d ) ); }
inline V3f& toV3f( GtoContainer::float3 &d ) { return *( ( V3f * )( &d ) ); }
inline V3i& toV3i( GtoContainer::int3 &d ) { return *( ( V3i * )( &d ) ); }
// inline V3h& toV3h( GtoContainer::half3 &d ) { return *( ( V3h * )( &d ) ); }

inline C3f& toC3f( GtoContainer::float3 &d ) { return *( ( C3f * )( &d ) ); }
inline C3c& toC3c( GtoContainer::byte3 &d ) { return *( ( C3c * )( &d ) ); }

inline C4f& toC4f( GtoContainer::float4 &d ) { return *( ( C4f * )( &d ) ); }
inline C4c& toC4c( GtoContainer::byte4 &d ) { return *( ( C4c * )( &d ) ); }

inline B3d& toB3d( GtoContainer::double6 &d ) { return *( ( B3d * )( &d ) ); }
inline B3f& toB3f( GtoContainer::float6 &d ) { return *( ( B3f * )( &d ) ); }
inline B3i& toB3i( GtoContainer::int6 &d ) { return *( ( B3i * )( &d ) ); }
// inline B3h& toB3h( GtoContainer::half6 &d ) { return *( ( B3h * )( &d ) ); }

inline B2d& toB2d( GtoContainer::double4 &d ) { return *( ( B2d * )( &d ) ); }
inline B2f& toB2f( GtoContainer::float4 &d ) { return *( ( B2f * )( &d ) ); }
inline B2i& toB2i( GtoContainer::int4 &d ) { return *( ( B2i * )( &d ) ); }
// inline B2h& toB2h( GtoContainer::half4 &d ) { return *( ( B2h * )( &d ) ); }

inline Quatd& toQuatd( GtoContainer::double4 &d )
{ return *( ( Quatd * )( &d ) ); }
inline Quatf& toQuatf( GtoContainer::float4 &d )
{ return *( ( Quatf * )( &d ) ); }

inline M44f& toM44f( GtoContainer::float16 &d ) { return *( ( M44f * )( &d ) ); }
inline M44d& toM44d( GtoContainer::double16 &d )
{ return *( ( M44d * )( &d ) ); }

inline M33f& toM33f( GtoContainer::float9 &d ) { return *( ( M33f * )( &d ) ); }
inline M33d& toM33d( GtoContainer::double9 &d )
{ return *( ( M33d * )( &d ) ); }

//-*****************************************************************************
inline const V2d& toV2d( const GtoContainer::double2 &d )
{ return *( ( const V2d * )( &d ) ); }
inline const V2f& toV2f( const GtoContainer::float2 &d )
{ return *( ( const V2f * )( &d ) ); }
inline const V2i& toV2i( const GtoContainer::int2 &d )
{ return *( ( const V2i * )( &d ) ); }
// inline const V2h& toV2h( const GtoContainer::half2 &d )
// { return *( ( const V2h * )( &d ) ); }

inline const V3d& toV3d( const GtoContainer::double3 &d )
{ return *( ( const V3d * )( &d ) ); }
inline const V3f& toV3f( const GtoContainer::float3 &d )
{ return *( ( const V3f * )( &d ) ); }
inline const V3i& toV3i( const GtoContainer::int3 &d )
{ return *( ( const V3i * )( &d ) ); }
// inline const V3h& toV3h( const GtoContainer::half3 &d )
//{ return *( ( const V3h * )( &d ) ); }

inline const C3f& toC3f( const GtoContainer::float3 &d )
{ return *( ( const C3f * )( &d ) ); }
inline const C3c& toC3c( const GtoContainer::byte3 &d )
{ return *( ( const C3c * )( &d ) ); }

inline const C4f& toC4f( const GtoContainer::float4 &d )
{ return *( ( const C4f * )( &d ) ); }
inline const C4c& toC4c( const GtoContainer::byte4 &d )
{ return *( ( const C4c * )( &d ) ); }

inline const B3d& toB3d( const GtoContainer::double6 &d )
{ return *( ( const B3d * )( &d ) ); }
inline const B3f& toB3f( const GtoContainer::float6 &d )
{ return *( ( const B3f * )( &d ) ); }
inline const B3i& toB3i( const GtoContainer::int6 &d )
{ return *( ( const B3i * )( &d ) ); }
// inline const B3h& toB3h( const GtoContainer::half6 &d )
// { return *( ( const B3h * )( &d ) ); }

inline const B2d& toB2d( const GtoContainer::double4 &d )
{ return *( ( const B2d * )( &d ) ); }
inline const B2f& toB2f( const GtoContainer::float4 &d )
{ return *( ( const B2f * )( &d ) ); }
inline const B2i& toB2i( const GtoContainer::int4 &d )
{ return *( ( const B2i * )( &d ) ); }
// inline const B2h& toB2h( const GtoContainer::half4 &d )
// { return *( ( const B2h * )( &d ) ); }

inline const Quatd& toQuatd( const GtoContainer::double4 &d )
{ return *( ( const Quatd * )( &d ) ); }
inline const Quatf& toQuatf( const GtoContainer::float4 &d )
{ return *( ( const Quatf * )( &d ) ); }

inline const M44f& toM44f( const GtoContainer::float16 &d )
{ return *( ( const M44f * )( &d ) ); }
inline const M44d& toM44d( const GtoContainer::double16 &d )
{ return *( ( const M44d * )( &d ) ); }

inline const M33f& toM33f( const GtoContainer::float9 &d )
{ return *( ( const M33f * )( &d ) ); }
inline const M33d& toM33d( const GtoContainer::double9 &d )
{ return *( ( const M33d * )( &d ) ); }

//-*****************************************************************************
inline GtoContainer::double2 &fromV2d( V2d &d )
{ return *( ( GtoContainer::double2 * )( &d ) ); }
inline GtoContainer::float2 &fromV2f( V2f &d )
{ return *( ( GtoContainer::float2 * )( &d ) ); }
inline GtoContainer::int2 &fromV2i( V2i &d )
{ return *( ( GtoContainer::int2 * )( &d ) ); }
// inline GtoContainer::half2 &fromV2h( V2h &d )
// { return *( ( GtoContainer::half2 * )( &d ) ); }

inline GtoContainer::double3 &fromV3d( V3d &d )
{ return *( ( GtoContainer::double3 * )( &d ) ); }
inline GtoContainer::float3 &fromV3f( V3f &d )
{ return *( ( GtoContainer::float3 * )( &d ) ); }
inline GtoContainer::int3 &fromV3i( V3i &d )
{ return *( ( GtoContainer::int3 * )( &d ) ); }
// inline GtoContainer::half3 &fromV3h( V3h &d )
// { return *( ( GtoContainer::half3 * )( &d ) ); }

inline GtoContainer::float3 &fromC3f( C3f &d )
{ return *( ( GtoContainer::float3 * )( &d ) ); }
inline GtoContainer::byte3 &fromC3c( C3c &d )
{ return *( ( GtoContainer::byte3 * )( &d ) ); }

inline GtoContainer::float4 &fromC4f( C4f &d )
{ return *( ( GtoContainer::float4 * )( &d ) ); }
inline GtoContainer::byte4 &fromC4c( C4c &d )
{ return *( ( GtoContainer::byte4 * )( &d ) ); }

inline GtoContainer::double6 &fromB3d( B3d &d )
{ return *( ( GtoContainer::double6 * )( &d ) ); }
inline GtoContainer::float6 &fromB3f( B3f &d )
{ return *( ( GtoContainer::float6 * )( &d ) ); }
inline GtoContainer::int6 &fromB3i( B3i &d )
{ return *( ( GtoContainer::int6 * )( &d ) ); }
// inline GtoContainer::half6 &fromB3h( B3h &d )
// { return *( ( GtoContainer::half6 * )( &d ) ); }

inline GtoContainer::double4 &fromB2d( B2d &d )
{ return *( ( GtoContainer::double4 * )( &d ) ); }
inline GtoContainer::float4 &fromB2f( B2f &d )
{ return *( ( GtoContainer::float4 * )( &d ) ); }
inline GtoContainer::int4 &fromB2i( B2i &d )
{ return *( ( GtoContainer::int4 * )( &d ) ); }
// inline GtoContainer::half4 &fromB2h( B2h &d )
// { return *( ( GtoContainer::half4 * )( &d ) ); }

inline GtoContainer::double4 &fromQuatd( Quatd &d )
{ return *( ( GtoContainer::double4 * )( &d ) ); }
inline GtoContainer::float4 &fromQuatf( Quatf &d )
{ return *( ( GtoContainer::float4 * )( &d ) ); }

//-*****************************************************************************
inline const GtoContainer::double2 &fromV2d( const V2d &d )
{ return *( ( const GtoContainer::double2 * )( &d ) ); }
inline const GtoContainer::float2 &fromV2f( const V2f &d )
{ return *( ( const GtoContainer::float2 * )( &d ) ); }
inline const GtoContainer::int2 &fromV2i( const V2i &d )
{ return *( ( const GtoContainer::int2 * )( &d ) ); }
// inline const GtoContainer::half2 &fromV2h( const V2h &d )
// { return *( ( const GtoContainer::half2 * )( &d ) ); }

inline const GtoContainer::double3 &fromV3d( const V3d &d )
{ return *( ( const GtoContainer::double3 * )( &d ) ); }
inline const GtoContainer::float3 &fromV3f( const V3f &d )
{ return *( ( const GtoContainer::float3 * )( &d ) ); }
inline const GtoContainer::int3 &fromV3i( const V3i &d )
{ return *( ( const GtoContainer::int3 * )( &d ) ); }
// inline const GtoContainer::half3 &fromV3h( const V3h &d )
// { return *( ( const GtoContainer::half3 * )( &d ) ); }

inline const GtoContainer::float3 &fromC3f( const C3f &d )
{ return *( ( const GtoContainer::float3 * )( &d ) ); }
inline const GtoContainer::byte3 &fromC3c( const C3c &d )
{ return *( ( const GtoContainer::byte3 * )( &d ) ); }

inline const GtoContainer::float4 &fromC4f( const C4f &d )
{ return *( ( const GtoContainer::float4 * )( &d ) ); }
inline const GtoContainer::byte4 &fromC4c( const C4c &d )
{ return *( ( const GtoContainer::byte4 * )( &d ) ); }

inline const GtoContainer::double6 &fromB3d( const B3d &d )
{ return *( ( const GtoContainer::double6 * )( &d ) ); }
inline const GtoContainer::float6 &fromB3f( const B3f &d )
{ return *( ( const GtoContainer::float6 * )( &d ) ); }
inline const GtoContainer::int6 &fromB3i( const B3i &d )
{ return *( ( const GtoContainer::int6 * )( &d ) ); }
// inline const GtoContainer::half6 &fromB3h( const B3h &d )
// { return *( ( const GtoContainer::half6 * )( &d ) ); }

inline const GtoContainer::double4 &fromB2d( const B2d &d )
{ return *( ( const GtoContainer::double4 * )( &d ) ); }
inline const GtoContainer::float4 &fromB2f( const B2f &d )
{ return *( ( const GtoContainer::float4 * )( &d ) ); }
inline const GtoContainer::int4 &fromB2i( const B2i &d )
{ return *( ( const GtoContainer::int4 * )( &d ) ); }
// inline const GtoContainer::half4 &fromB2h( const B2h &d )
// { return *( ( const GtoContainer::half4 * )( &d ) ); }

inline const GtoContainer::double4 &fromQuatd( const Quatd &d )
{ return *( ( const GtoContainer::double4 * )( &d ) ); }
inline const GtoContainer::float4 &fromQuatf( const Quatf &d )
{ return *( ( const GtoContainer::float4 * )( &d ) ); }

//-*****************************************************************************
//-*****************************************************************************
// FUNCTIONS
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
template <class T>
inline T degrees( const T &rads ) { return 180.0 * rads / M_PI; }

//-*****************************************************************************
template <class T>
inline T radians( const T &degs ) { return M_PI * degs / 180.0; }

//-*****************************************************************************
template <class T>
inline const T &clamp( const T &x, const T &lo, const T &hi )
{
    return x < lo ? lo : x > hi ? hi : x;
}

} // End namespace SimpleGtoViewer

#endif
