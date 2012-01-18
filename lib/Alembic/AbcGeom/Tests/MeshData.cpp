//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <Alembic/AbcGeom/Tests/MeshData.h>

//-*****************************************************************************
// 8 vertices of a unit cube
const size_t g_numVerts = 8;
const Abc::float32_t g_verts[] = { -1.0f, -1.0f, -1.0f,
                                          1.0f, -1.0f, -1.0f,
                                          -1.0f, 1.0f, -1.0f,
                                          1.0f, 1.0f, -1.0f,
                                          -1.0f, -1.0f, 1.0f,
                                          1.0f, -1.0f, 1.0f,
                                          -1.0f, 1.0f, 1.0f,
                                          1.0f, 1.0f, 1.0f };

//-*****************************************************************************
// optional velocities to go with the vertices
const Abc::float32_t g_veloc[] = { -0.5f, -0.5f, -0.5f,
    0.25f, -0.5f, -0.5f,
    -0.5f, 0.25f, -0.5f,
    0.25f, 0.25f, -0.5f,
    -0.5f, -0.5f, 0.25f,
    0.25f, -0.5f, 0.25f,
    -0.5f, 0.25f, 0.25f,
    0.25f, 0.25f, 0.25f };

//-*****************************************************************************
// Indices of the cube vertices
const size_t g_numIndices = 24;
const Abc::int32_t g_indices[] = {
    // LEFT
    0, 4, 6, 2,
    // RIGHT
    5, 1, 3, 7,
    // BOTTOM
    0, 1, 5, 4,
    // TOP,
    6, 7, 3, 2,
    // BACK
    1, 0, 2, 3,
    // FRONT
    4, 5, 7, 6 };

const Abc::uint32_t g_uindices[] = {
    // LEFT
    0, 4, 6, 2,
    // RIGHT
    5, 1, 3, 7,
    // BOTTOM
    0, 1, 5, 4,
    // TOP,
    6, 7, 3, 2,
    // BACK
    1, 0, 2, 3,
    // FRONT
    4, 5, 7, 6 };

//-*****************************************************************************
// "Face Counts" - number of vertices in each face.
const size_t g_numCounts = 6;
const Abc::int32_t g_counts[] = { 4, 4, 4, 4, 4, 4 };

//-*****************************************************************************
// Normals of the cube vertices.
// Normals in AbcGeom's PolyMesh class are per-vertex, per-face,
// without indices. This is basically the storage that matches renderman's
// "facevarying" detail type.
const size_t g_numNormals = g_numIndices;
const Abc::float32_t g_normals[] = {
    // LEFT
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    // RIGHT
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    // BOTTOM
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    // TOP
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    // BACK
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    // FRONT
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f };

//-*****************************************************************************
// UVs of the cube vertices.
// UVS, like Normals, are per-vertex, per-face, without indices.
const size_t g_numUVs = g_numIndices;
const Abc::float32_t g_uvs[] = {
    // LEFT
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    // RIGHT
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    // BOTTOM
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    // TOP
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    // BACK
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    // FRONT
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f };

//-*******************************

