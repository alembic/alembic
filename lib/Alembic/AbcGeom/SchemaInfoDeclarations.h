//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
//! With properties, specific flavors of properties are expressed via the
//! TypedScalarProperty and the TypedArrayProperty. Compound Properties
//! are more complex, and the specific flavors require a more complex
//! treatment - That's what Schemas are. The CompoundProperty equivalent
//! of a TypedArrayProperty or a TypedScalarProperty.
//!
//! A Schema is a collection of grouped properties which implement some
//! complex object, such as a poly mesh. In the simpelest, standard case,
//! there will be a compound property at the top with a certain name, and
//! inside the compound property will be some number of additional properties
//! that implement the object. In the case of a poly mesh, these properties
//! would include a list of vertices (a V3fArray), a list of indices
//! (an Int32Array), and a list of "per-face counts" (also an Int32Array).


//-*****************************************************************************
//! Here is a macro for declaring SCHEMA_INFO
//! It takes three arguments
//! - the SchemaTitle( a string ),
//! - the DefaultSchemaName( a string )
//! - the name of the SchemaInfo Type to be declared.
//! - for example:
//! ALEMBIC_ABCGEOM_DECLARE_SCHEMA_INFO( "AbcGeom_PolyMesh_v1",
//!                                      ".geom",
//!                                      PolyMeshSchemaInfo );
#define ALEMBIC_ABCGEOM_DECLARE_SCHEMA_INFO( STITLE, SDFLT, STDEF )   \
struct STDEF                                                            \
{                                                                       \
    static const char * title() { return ( STITLE ) ; }                 \
    static const char * defaultName() { return ( SDFLT ); }             \
}

//-*****************************************************************************
//! Now to declare schema traits using the above macro.  The SCHEMA_INFO are
//! basically a collection of information about the scheme, acting as an
//! elaborate typeid

//-*****************************************************************************
// Curve
ALEMBIC_ABCGEOM_DECLARE_SCHEMA_INFO( "AbcGeom_Curve_v1",
                                     ".geom",
                                     CurvesSchemaInfo );

#define ALEMBIC_ABCGEOM_CURVE_SCHEMA (CurvesSchemaInfo::title())


//-*****************************************************************************
// PolyMesh
ALEMBIC_ABCGEOM_DECLARE_SCHEMA_INFO( "AbcGeom_PolyMesh_v1",
                                     ".geom",
                                     PolyMeshSchemaInfo );

#define ALEMBIC_ABCGEOM_POLYMESH_SCHEMA (PolyMeshSchemaInfo::title())

//-*****************************************************************************
// Subdivision surface
ALEMBIC_ABCGEOM_DECLARE_SCHEMA_INFO( "AbcGeom_SubD_v1",
                                     ".geom",
                                     SubDSchemaInfo );

#define ALEMBIC_ABCGEOM_SUBD_SCHEMA (SubDSchemaInfo::title())

//-*****************************************************************************
// Points
ALEMBIC_ABCGEOM_DECLARE_SCHEMA_INFO( "AbcGeom_Points_v1",
                                     ".geom",
                                     PointsSchemaInfo );

#define ALEMBIC_ABCGEOM_POINTS_SCHEMA (PointsSchemaInfo::title())

//-*****************************************************************************
// SimpleXform
ALEMBIC_ABCGEOM_DECLARE_SCHEMA_INFO( "AbcGeom_SimpleXform_v1",
                                     ".xform",
                                     SimpleXformSchemaInfo );

#define ALEMBIC_ABCGEOM_SIMPLEXFORM_SCHEMA (SimpleXformSchemaInfo::title())

//-*****************************************************************************
// Xform
ALEMBIC_ABCGEOM_DECLARE_SCHEMA_INFO( "AbcGeom_Xform_v2",
                                     ".xform",
                                     XformSchemaInfo );

#define ALEMBIC_ABCGEOM_XFORM_SCHEMA (XformSchemaInfo::title())

}
}


#endif // header guard
