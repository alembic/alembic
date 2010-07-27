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

#ifndef _Alembic_TraitsGeom_Particles_h_
#define _Alembic_TraitsGeom_Particles_h_

#include <Alembic/TraitsGeom/Foundation.h>
#include <Alembic/TraitsGeom/Xform.h>
#include <Alembic/TraitsGeom/PolyMesh.h>

namespace Alembic {
namespace TraitsGeom {

//-*****************************************************************************
// Particles are simple. Just IDs and Positions.
//
// IntArray ids()
// V3dArray positions()
// Anything else is considered to be a per-particle attribute
// if it is multi and has the same number of elements as the id array
//
// To achieve bucketing, let SimpleParticle objects be arranged in
// hierarchies. We will know to look for this.
ALEMBIC_STRING_TRAIT( SimpleParticlesProtocolStrait,
                      "AlembicSimpleParticles_v0001" );

ALEMBIC_STRING_TRAIT( IdsStrait, "perVertex|id" );

//-*****************************************************************************
// Property traits
ALEMBIC_PROPERTY_TRAIT(                 IdsTrait,
                                        LongArrayProperty,
                                        IdsStrait );

//-*****************************************************************************
// Amalgams
ALEMBIC_AMALGAM_TRAIT4( SimpleParticlesTrait,
                        BoundsLocalTrait, bounds,
                        XformLocalTrait, xform,
                        IdsTrait, ids,
                        PositionsTrait, positions );

//-*****************************************************************************
// Trait objects
ALEMBIC_TRAIT_OBJECT( SimpleParticles, SimpleParticlesTrait,
                      SimpleParticlesProtocolStrait );

} // End namespace TraitsGeom
} // End namespace Alembic

#endif
