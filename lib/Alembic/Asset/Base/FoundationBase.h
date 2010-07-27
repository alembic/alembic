//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#ifndef _Alembic_Asset_Base_FoundationBase_h_
#define _Alembic_Asset_Base_FoundationBase_h_

//-*****************************************************************************
// No HDF5 Stuff included in base, only in Body.
//-*****************************************************************************
#include <Alembic/MD5Hash/MapUtil.h>
#include <Alembic/MD5Hash/MD5.h>
#include <Alembic/Util/Util.h>

#include <boost/cstdint.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/static_assert.hpp>

//#include <boost/detail/endian.hpp>
//#include <boost/spirit/home/support/detail/integer/endian.hpp>

#include <ImathVec.h>
#include <ImathMatrix.h>
#include <ImathQuat.h>
#include <ImathBox.h>

#include <algorithm>
#include <vector>
#include <string>
#include <set>
#include <map>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
//-*****************************************************************************
// Easiest way.
//-*****************************************************************************
//-*****************************************************************************
#if 0
typedef Alembic::Util::byte_t           byte_t;
typedef Alembic::Util::bool_t           bool_t;
typedef Alembic::Util::uint8_t          uint8_t;
typedef Alembic::Util::int8_t           int8_t;
typedef Alembic::Util::uint16_t         uint16_t;
typedef Alembic::Util::int16_t          int16_t;
typedef Alembic::Util::uint32_t         uint32_t;
typedef Alembic::Util::int32_t          int32_t;
typedef Alembic::Util::uint64_t         uint64_t;
typedef Alembic::Util::int64_t          int64_t;
typedef Alembic::Util::float16_t        float16_t;
typedef Alembic::Util::float32_t        float32_t;
typedef Alembic::Util::float64_t        float64_t;

// Last, but not least, cstr_t.
typedef Alembic::Util::cstr_t           cstr_t;

//-*****************************************************************************
typedef Alembic::Util::Dimensions Dimensions;
#else
using namespace ::Alembic::Util;
#endif

//-*****************************************************************************
typedef std::vector<uint8_t> Bytes;
typedef std::vector<seconds_t> SecondsArray;

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// IMPORTED IMATH TYPES
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
typedef Imath::V2s V2s;
typedef Imath::V2i V2i;
typedef Imath::V2f V2f;
typedef Imath::V2d V2d;

typedef Imath::V3s V3s;
typedef Imath::V3i V3i;
typedef Imath::V3f V3f;
typedef Imath::V3d V3d;

typedef Imath::Box2s Box2s;
typedef Imath::Box2i Box2i;
typedef Imath::Box2f Box2f;
typedef Imath::Box2d Box2d;

typedef Imath::Box3s Box3s;
typedef Imath::Box3i Box3i;
typedef Imath::Box3f Box3f;
typedef Imath::Box3d Box3d;

typedef Imath::M33f M33f;
typedef Imath::M33d M33d;
typedef Imath::M44f M44f;
typedef Imath::M44d M44d;

typedef Imath::Quatf Quatf;
typedef Imath::Quatd Quatd;

} // End namespace Asset
} // End namespace Alembic

#endif
