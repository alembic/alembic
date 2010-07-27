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

#ifndef _AlembicTako_PointPrimitive_h_
#define _AlembicTako_PointPrimitive_h_

#include <AlembicTako/Foundation.h>

namespace AlembicTako {

//-*****************************************************************************
// See Transform.h for commentary on Alembic vs. Tako structures.
//-*****************************************************************************

//-*****************************************************************************
// Make string traits.
ALEMBIC_STRING_TRAIT( PointPrimitiveProtocolStrait, "TakoPointPrimitive_v0001" );
ALEMBIC_STRING_TRAIT( PointPrimitivePositionStrait, ".position" );
ALEMBIC_STRING_TRAIT( PointPrimitiveVelocityStrait, ".velocity" );
ALEMBIC_STRING_TRAIT( PointPrimitiveIdStrait, ".particleIds" );
ALEMBIC_STRING_TRAIT( PointPrimitiveWidthStrait, ".width" );
ALEMBIC_STRING_TRAIT( PointPrimitiveConstantWidthStrait, ".constantWidth" );

//-*****************************************************************************
// Make property traits.
ALEMBIC_PROPERTY_TRAIT( PointPrimitivePositionTrait,
                        V3fArrayProperty,
                        PointPrimitivePositionStrait );

ALEMBIC_PROPERTY_TRAIT( PointPrimitiveVelocityTrait,
                        V3fArrayProperty,
                        PointPrimitiveVelocityStrait );

ALEMBIC_PROPERTY_TRAIT( PointPrimitiveIdTrait,
                        IntArrayProperty,
                        PointPrimitiveIdStrait );

// Width is an optional property.
ALEMBIC_OPT_PROPERTY_TRAIT( PointPrimitiveWidthTrait,
                            FloatArrayProperty,
                            PointPrimitiveWidthStrait );

// Constant width is also optional.
ALEMBIC_OPT_PROPERTY_TRAIT( PointPrimitiveConstantWidthTrait,
                            FloatProperty,
                            PointPrimitiveConstantWidthStrait );

//-*****************************************************************************
// Make amalgams.
ALEMBIC_AMALGAM_TRAIT5( PointPrimitiveTrait,
                        PointPrimitivePositionTrait, position,
                        PointPrimitiveVelocityTrait, velocity,
                        PointPrimitiveIdTrait, particleIds,
                        PointPrimitiveWidthTrait, width,
                        PointPrimitiveConstantWidthTrait, constantWidth );

//-*****************************************************************************
// Make trait objects.
ALEMBIC_TRAIT_OBJECT( PointPrimitiveObj,
                      PointPrimitiveTrait,
                      PointPrimitiveProtocolStrait );


} // End namespace AlembicTako

#endif
