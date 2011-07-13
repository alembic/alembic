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

#include <Alembic/AbcGeom/Tests/NurbsData.h>

//-*****************************************************************************

const int32_t g_nu = 4;
const int32_t g_nv = 4;
const int32_t g_uOrder = 4;
const int32_t g_vOrder = 4;

const float32_t g_uKnot[] = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
const float32_t g_vKnot[] = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};

const int32_t g_nP = 16;
const float32_t g_P[] = {   0.0f, 0.0f, -3.0f,
                                        0.0f, 1.0f, -3.0f,
                                        0.0f, 2.0f, -3.0f,
                                        0.0f, 3.0f, -3.0f,
                                        1.0f, 0.0f, -3.0f,
                                        1.0f, 1.0f, 3.0f,
                                        1.0f, 2.0f, 3.0f,
                                        1.0f, 3.0f, -3.0f,
                                        2.0f, 0.0f, -3.0f,
                                        2.0f, 1.0f, 3.0f,
                                        2.0f, 2.0f, 3.0f,
                                        2.0f, 3.0f, -3.0f,
                                        3.0f, 0.0f, -3.0f,
                                        3.0f, 1.0f, -3.0f,
                                        3.0f, 2.0f, -3.0f,
                                        3.0f, 3.0f, -3.0f};
const float32_t g_Pw[] = { 1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 0.5f,
                           1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 0.5f };

// trim curve
const int32_t g_trim_nLoops = 1;
const int32_t g_trim_nCurves[] = { 1 };
const int32_t g_trim_order[] = { 3 };
const float32_t g_trim_knot[] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 2.0f, 2.0f,
                                        3.0f, 3.0f, 4.0f, 4.0f, 4.0f };
const float32_t g_trim_min[] = { 0.0f };
const float32_t g_trim_max[] = { 1.0f };
const int32_t g_trim_n[] = { 9 };
const float32_t g_trim_u[] = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
const float32_t g_trim_v[] = { 0.5f, 1.0f, 2.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f };
const float32_t g_trim_w[] = { 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f };
