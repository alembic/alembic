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

#ifndef _AlembicTako_Transform_h_
#define _AlembicTako_Transform_h_

#include <AlembicTako/Foundation.h>

namespace AlembicTako {

//-*****************************************************************************
// Alembic and Tako are slightly structurally different in how animation
// and time sampling are done. While I believe it would be possible to
// fully map the existing Tako readers and writers onto Alembic Objects
// (And we may yet still do that), the intent of this file is to simply
// create AlembicTrait objects that create an analagous set of properties
// as Tako objects have.
//
// The transforms in Tako are also more complex than just a M44d property,
// but to illustrate I'll create a M44dArrayProperty named "stack".
// Tako also has a boolean property - non-animated, named "inheritsTransform"
//-*****************************************************************************

//-*****************************************************************************
// Make string traits
ALEMBIC_STRING_TRAIT( TransformProtocolStrait, "TakoTransform_v0001" );
ALEMBIC_STRING_TRAIT( TransformStackStrait, "stack" );
ALEMBIC_STRING_TRAIT( TransformInheritStrait, "inheritsTransform" );

//-*****************************************************************************
// Make property traits

// TransformStack is an array of M44f's. This is intended to represent
// Tako's transform stack, which could have any number of transformations
// per object. 
ALEMBIC_PROPERTY_TRAIT( TransformStackTrait,
                        M44fArrayProperty,
                        TransformStackStrait );

// InheritsTransform is a boolean. It is not animateable.  Since
// BoolProperties are not implemented yet, I'll use a CharProperty
ALEMBIC_NONANIM_PROPERTY_TRAIT( TransformInheritTrait,
                                CharProperty,
                                TransformInheritStrait );

//-*****************************************************************************
// Make amalgams

// Combine the stack and inheritsTransform traits into an amalgamated
// TransformTrait.
ALEMBIC_AMALGAM_TRAIT2( TransformTrait,
                        TransformStackTrait, stack,
                        TransformInheritTrait, inheritsTransform );

//-*****************************************************************************
// Make a Trait Object corresponding to the TransformTrait
ALEMBIC_TRAIT_OBJECT( TransformObj,
                      TransformTrait,
                      TransformProtocolStrait );

} // End namespace AlembicTako

#endif
