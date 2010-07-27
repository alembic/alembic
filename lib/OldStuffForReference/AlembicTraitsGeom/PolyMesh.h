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

#ifndef _AlembicTraitsGeom_PolyMesh_h_
#define _AlembicTraitsGeom_PolyMesh_h_

#include <AlembicTraitsGeom/Foundation.h>
#include <AlembicTraitsGeom/Xform.h>

namespace AlembicTraitsGeom {

//-*****************************************************************************
// String Traits - for string literals that differentiate types.
ALEMBIC_STRING_TRAIT( SimplePolyMeshProtocolStrait,
                      "AlembicSimplePolyMesh_v0001" );

ALEMBIC_STRING_TRAIT( PositionsStrait, "perVertex|P" );
ALEMBIC_STRING_TRAIT( NormalsStrait, "perVertexPerFace|N" );
ALEMBIC_STRING_TRAIT( UvsStrait, "perVertexPerFace|uv" );
ALEMBIC_STRING_TRAIT( FaceCountsStrait, "perFace|count" );
ALEMBIC_STRING_TRAIT( VertexIndicesStrait, "perVertexPerFace|index" );

//-*****************************************************************************
// Property traits.
// ----------------
// Vertices (P) non-optional, animateable.
// Face counts, non-optional, non-animateable.
// Indices, non-optional, non-animateable.
// Normals, optional, animateable.
// UVs, optional, non-animateable.
ALEMBIC_PROPERTY_TRAIT(                 PositionsTrait,
                                        V3dArrayProperty,
                                        PositionsStrait );

ALEMBIC_NONANIM_PROPERTY_TRAIT(         FaceCountsTrait,
                                        IntArrayProperty,
                                        FaceCountsStrait );

ALEMBIC_NONANIM_PROPERTY_TRAIT(         VertexIndicesTrait,
                                        IntArrayProperty,
                                        VertexIndicesStrait );

ALEMBIC_OPT_PROPERTY_TRAIT(             NormalsTrait,
                                        V3fArrayProperty,
                                        NormalsStrait );

ALEMBIC_OPT_NONANIM_PROPERTY_TRAIT(     UvsTrait,
                                        V2fArrayProperty,
                                        UvsStrait );

//-*****************************************************************************
// Amalgams

// Mesh Trait
ALEMBIC_AMALGAM_TRAIT3( MeshTrait,
                        PositionsTrait, positions,
                        FaceCountsTrait, faceCounts,
                        VertexIndicesTrait, vertexIndices );

// Simple Poly Mesh Trait
ALEMBIC_AMALGAM_TRAIT5( SimplePolyMeshTrait,
                        BoundsLocalTrait, bounds,
                        XformLocalTrait, xform,
                        MeshTrait, mesh,
                        NormalsTrait, normals,
                        UvsTrait, uvs );

//-*****************************************************************************
// Trait objects
ALEMBIC_TRAIT_OBJECT( SimplePolyMesh, SimplePolyMeshTrait,
                      SimplePolyMeshProtocolStrait );

} // End namespace AlembicTraitsGeom

#endif
