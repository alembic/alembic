//-*****************************************************************************
//
// Copyright (c) 2009-2011,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#ifndef _Alembic_AbcGeom_Tests_NurbsData_h_
#define _Alembic_AbcGeom_Tests_NurbsData_h_

#include <Alembic/AbcGeom/All.h>

using Alembic::Util::int32_t;
using Alembic::Util::float32_t;

extern const int32_t g_nu;
extern const int32_t g_nv;
extern const int32_t g_uOrder;
extern const int32_t g_vOrder;
extern const float32_t g_uKnot[];
extern const float32_t g_vKnot[];

extern const int32_t g_nP;
extern const float32_t g_P[];
extern const float32_t g_Pw[];
extern const int32_t g_numNormals;
extern const float32_t g_normals[];
extern const int32_t g_numVelocities;
extern const float32_t g_velocities[];

// trim curve
extern const int32_t g_trim_nLoops;
extern const int32_t g_trim_nCurves[];
extern const int32_t g_trim_n[];
extern const int32_t g_trim_order[];
extern const float32_t g_trim_knot[];
extern const float32_t g_trim_min[];
extern const float32_t g_trim_max[];
extern const float32_t g_trim_u[];
extern const float32_t g_trim_v[];
extern const float32_t g_trim_w[];

//-*******************************

#endif
