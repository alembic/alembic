//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#include <Alembic/AbcGeom/Tests/CurvesData.h>

//-*****************************************************************************

const size_t g_numCurves = 2;
const int32_t g_numVerts[] = {8, 4};
const size_t g_totalVerts = 12;

const float32_t g_verts[] = {      // first curve
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,

    // second curve
    1.0f, 0.0f, 1.0f,
    0.5f, 0.25f, 1.0f,
    0.5f, 0.75f, 1.0f,
    1.0f, 1.0f, 1.0f
};

const float32_t g_weights[] = {
  // first curve
  1.0f,
  0.8f,
  0.6f,
  0.4f,
  0.4f,
  0.6f,
  0.8f,
  1.0f,

  // second curve
  1.0f,
  1.0f,
  1.0f,
  1.0f
};

const float32_t g_knots[] = {
  // first curve 8 vertices 4 order so 12 knots
  1.0f,
  2.0f,
  3.0f,
  4.0f,
  5.0f,
  6.0f,
  7.0f,
  8.0f,
  9.0f,
 10.0f,
 11.0f,
 12.0f,

  // second curve 4 vertices 2 order so 6 knots
  0.1f,
  0.1f,
  0.2f,
  0.4f,
  0.6f,
  0.6f
};

const float32_t g_uvs[] = {    0.0, 0.0,
                               0.1, 0.0,
                               0.2, 0.0,
                               0.3, 0.0,
                               0.4, 0.0,
                               0.5, 0.0,
                               0.6, 0.0,
                               0.7, 0.0,
                               0.0, 0.0,
                               0.25, 0.0,
                               0.5, 0.0,
                               1.0, 0.0
};

const float32_t g_widths[] = {    // first curve
    0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6,

    // second curve
    0.25, 0.5, 0.75, 1.0
};

const uint8_t g_orders[] = { 4, 2 };

const size_t g_numWidths = 12;  // vertex varying
