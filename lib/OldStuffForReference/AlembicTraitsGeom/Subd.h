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

#ifndef _AlembicTraitsGeom_Subd_h_
#define _AlembicTraitsGeom_Subd_h_

#include <AlembicTraitsGeom/Foundation.h>
#include <AlembicTraitsGeom/Subd.h>

namespace AlembicTraitsGeom {

//-*****************************************************************************
enum SubdScheme
{
    kCatmullClarkSubd,
    kLoopSubd
};

//-*****************************************************************************
enum SubdBoundaryInterpolation
{
    kNoBoundaryInterp = 0,
    kEdgesCornersBoundaryInterp = 1,
    kEdgesBoundaryInterp = 2
};

//-*****************************************************************************
// String traits - for string literals that differentiate types.
ALEMBIC_STRING_TRAIT( SimpleSubdProtocolStrait,
                      "AlembicSimpleSubd_v0001" );

ALEMBIC_STRING_TRAIT( SubdSchemeStrait, "subd|scheme" );
ALEMBIC_STRING_TRAIT( SubdBoundaryStrait, "subd|interpolateBoundary" );
ALEMBIC_STRING_TRAIT( SubdHolesStrait, "subd|hole" );
ALEMBIC_STRING_TRAIT( SubdCornersStrait, "subd|corner" );
ALEMBIC_STRING_TRAIT( SubdCreaseCountsStrait, "subd|creaseCount" );
ALEMBIC_STRING_TRAIT( SubdCreaseIndicesStrait, "subd|creaseIndex" );

//-*****************************************************************************
// Property traits.
// ----------------
// SubdScheme, non-optional, non-animateable.
// SubdBoundary, non-optional, non-animateable.
// SubdHoles, optional, non-animateable.
// SubdCorners, optional, non-animateable.
// SubdCreases, optional
//     counts, non-optional, non-animateable.
//     indices, non-optional, non-animateable.
ALEMBIC_NONANIM_PROPERTY_TRAIT(         SubdSchemeTrait,
                                        CharProperty,
                                        SubdSchemeStrait );

ALEMBIC_NONANIM_PROPERTY_TRAIT(         SubdBoundaryTrait,
                                        CharProperty,
                                        SubdBoundaryStrait );

ALEMBIC_OPT_NONANIM_PROPERTY_TRAIT(     SubdHolesTrait,
                                        IntArrayProperty,
                                        SubdHolesStrait );

ALEMBIC_OPT_NONANIM_PROPERTY_TRAIT(     SubdCornersTrait,
                                        IntArrayProperty,
                                        SubdCornersStrait );

ALEMBIC_NONANIM_PROPERTY_TRAIT(         SubdCreaseCountsTrait,
                                        IntArrayProperty,
                                        SubdCreaseCountsStrait );

ALEMBIC_NONANIM_PROPERTY_TRAIT(         SubdCreaseIndicesTrait,
                                        IntArrayProperty,
                                        SubdCreaseIndicesStrait );

//-*****************************************************************************
// Amalgams
// Note that I have to manually create the optional wrapper around the
// amalgam. I'm okay with this. We get a bit macro-heavy otherwise.
ALEMBIC_AMALGAM_TRAIT2( __SubdCreasesTrait,
                        SubdCreaseCountsTrait, count,
                        SubdCreaseIndicesTrait, index );
typedef AlembicAsset::IOptional< I__SubdCreasesTrait > ISubdCreasesTrait;
typedef AlembicAsset::OOptional< O__SubdCreasesTrait > OSubdCreasesTrait;

ALEMBIC_AMALGAM_TRAIT5( SubdTrait,
                        SubdSchemeTrait, scheme,
                        SubdBoundaryTrait, boundary,
                        SubdHolesTrait, holes,
                        SubdCornersTrait, corners,
                        SubdCreasesTrait, creases );

ALEMBIC_AMALGAM_TRAIT5( SimpleSubdTrait,
                        BoundsLocalTrait, bounds,
                        XformLocalTrait, xform,
                        MeshTrait, mesh,
                        SubdTrait, subd,
                        UvsTrait, uvs );

//-*****************************************************************************
// Trait Objects.
ALEMBIC_TRAIT_OBJECT( SimpleSubd, SimpleSubdTrait,
                      SimpleSubdProtocolStrait );

} // End namespace AlembicTraitsGeom

#endif
