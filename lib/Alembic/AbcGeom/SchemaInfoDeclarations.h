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

#ifndef _Alembic_AbcGeom_SchemaInfoDeclarations_h_
#define _Alembic_AbcGeom_SchemaInfoDeclarations_h_

#include <Alembic/Abc/OSchema.h>
#include <Alembic/Util/Foundation.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//! Declare our schema info for AbcGeom

//-*****************************************************************************
// PolyMesh
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "AbcGeom_PolyMesh_v1",
                                 "AbcGeom_GeomBase_v1",
                                 ".geom",
                                 false,
                                 PolyMeshSchemaInfo );

#define ALEMBIC_ABCGEOM_POLYMESH_SCHEMA (PolyMeshSchemaInfo::title())

//-*****************************************************************************
// NuPatch
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "AbcGeom_NuPatch_v2",
                                 "AbcGeom_GeomBase_v1",
                                 ".geom",
                                 false,
                                 NuPatchSchemaInfo );

#define ALEMBIC_ABCGEOM_NUPATCH_SCHEMA (NuPatchSchemaInfo::title())

//-*****************************************************************************
// Subdivision surface
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "AbcGeom_SubD_v1",
                                 "AbcGeom_GeomBase_v1",
                                 ".geom",
                                 false,
                                 SubDSchemaInfo );

#define ALEMBIC_ABCGEOM_SUBD_SCHEMA (SubDSchemaInfo::title())

//-*****************************************************************************
// SubD and PolyMesh FaceSet
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "AbcGeom_FaceSet_v1",
                                 "AbcGeom_GeomBase_v1",
                                 ".faceset",
                                 false,
                                 FaceSetSchemaInfo );

#define ALEMBIC_ABCGEOM_PARITION_SCHEMA (FaceSetSchemaInfo::title())


//-*****************************************************************************
// Points
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "AbcGeom_Points_v1",
                                 "AbcGeom_GeomBase_v1",
                                 ".geom",
                                 false,
                                 PointsSchemaInfo );

#define ALEMBIC_ABCGEOM_POINTS_SCHEMA (PointsSchemaInfo::title())

//-*****************************************************************************
// Xform
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "AbcGeom_Xform_v3",
                                 "",
                                 ".xform",
                                 true,
                                 XformSchemaInfo );

#define ALEMBIC_ABCGEOM_XFORM_SCHEMA (XformSchemaInfo::title())

//-*****************************************************************************
// Camera
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "AbcGeom_Camera_v1",
                                 "",
                                 ".geom",
                                 true,
                                 CameraSchemaInfo );

#define ALEMBIC_ABCGEOM_CAMERA_SCHEMA (CameraSchemaInfo::title())

//-*****************************************************************************
// Curves
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "AbcGeom_Curve_v2",
                                 "AbcGeom_GeomBase_v1",
                                 ".geom",
                                 false,
                                 CurvesSchemaInfo );

#define ALEMBIC_ABCGEOM_CURVE_SCHEMA (CurvesSchemaInfo::title())

//-*****************************************************************************
// Light
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "AbcGeom_Light_v1",
                                 "",
                                 ".geom",
                                 true,
                                 LightSchemaInfo );

#define ALEMBIC_ABCGEOM_LIGHT_SCHEMA (LightSchemaInfo::title())

//-*****************************************************************************
// GeomBase, meant for convenience IGeomBase only
ALEMBIC_ABC_DECLARE_SCHEMA_INFO( "AbcGeom_GeomBase_v1",
                                 "",
                                 ".geom",
                                 false,
                                 GeomBaseSchemaInfo );

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic


#endif // header guard
