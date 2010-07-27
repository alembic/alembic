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

#ifndef _AlembicTraits_Macros_h_
#define _AlembicTraits_Macros_h_

#include <AlembicTraits/Foundation.h>
#include <AlembicTraits/IMacros.h>
#include <AlembicTraits/OMacros.h>

namespace AlembicAsset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// PROPERTY MACROS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// These macros assume that you're using properties from AlembicAsset,
// and that they live in the namespaces AlembicAsset::Input:: and
// AlembicAsset::Output, respectively.
#define ALEMBIC_PROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE, STRAIT )     \
ALEMBIC_IPROPERTY_TRAIT( I ## TRAIT_NAME ,                              \
                         AlembicAsset::I ## PROPERTY_TYPE ,           \
                         STRAIT );                                      \
ALEMBIC_OPROPERTY_TRAIT( O ## TRAIT_NAME ,                              \
                         AlembicAsset::O ## PROPERTY_TYPE ,           \
                         STRAIT )

#define ALEMBIC_OPT_PROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE, STRAIT )  \
ALEMBIC_OPT_IPROPERTY_TRAIT( I ## TRAIT_NAME ,                           \
                             AlembicAsset::I ## PROPERTY_TYPE ,        \
                             STRAIT );                                   \
ALEMBIC_OPT_OPROPERTY_TRAIT( O ## TRAIT_NAME ,                           \
                             AlembicAsset::O ## PROPERTY_TYPE ,        \
                             STRAIT )


#define ALEMBIC_NONANIM_PROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE, STRAIT ) \
ALEMBIC_NONANIM_IPROPERTY_TRAIT( I ## TRAIT_NAME ,                          \
                             AlembicAsset::I ## PROPERTY_TYPE ,           \
                             STRAIT );                                      \
ALEMBIC_NONANIM_OPROPERTY_TRAIT( O ## TRAIT_NAME ,                          \
                             AlembicAsset::O ## PROPERTY_TYPE ,           \
                             STRAIT )


#define ALEMBIC_OPT_NONANIM_PROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE,         \
                                            STRAIT )                           \
ALEMBIC_OPT_NONANIM_IPROPERTY_TRAIT( I ## TRAIT_NAME ,                         \
                                     AlembicAsset::I ## PROPERTY_TYPE ,      \
                                     STRAIT );                                 \
ALEMBIC_OPT_NONANIM_OPROPERTY_TRAIT( O ## TRAIT_NAME ,                         \
                                     AlembicAsset::O ## PROPERTY_TYPE,       \
                                     STRAIT )

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// AMALGAM MACROS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
#define ALEMBIC_AMALGAM_TRAIT2( TRAIT_NAME,                     \
                                MEMBER0_TRAIT, MEMBER0_NAME,    \
                                MEMBER1_TRAIT, MEMBER1_NAME )   \
ALEMBIC_IAMALGAM_TRAIT2( I ## TRAIT_NAME ,                      \
                         I ## MEMBER0_TRAIT , MEMBER0_NAME,     \
                         I ## MEMBER1_TRAIT , MEMBER1_NAME );   \
ALEMBIC_OAMALGAM_TRAIT2( O ## TRAIT_NAME ,                      \
                         O ## MEMBER0_TRAIT , MEMBER0_NAME,     \
                         O ## MEMBER1_TRAIT , MEMBER1_NAME )

//-*****************************************************************************
#define ALEMBIC_AMALGAM_TRAIT3( TRAIT_NAME,                     \
                                MEMBER0_TRAIT, MEMBER0_NAME,    \
                                MEMBER1_TRAIT, MEMBER1_NAME,    \
                                MEMBER2_TRAIT, MEMBER2_NAME )   \
ALEMBIC_IAMALGAM_TRAIT3( I ## TRAIT_NAME ,                      \
                         I ## MEMBER0_TRAIT , MEMBER0_NAME,     \
                         I ## MEMBER1_TRAIT , MEMBER1_NAME,     \
                         I ## MEMBER2_TRAIT , MEMBER2_NAME );   \
ALEMBIC_OAMALGAM_TRAIT3( O ## TRAIT_NAME ,                      \
                         O ## MEMBER0_TRAIT , MEMBER0_NAME,     \
                         O ## MEMBER1_TRAIT , MEMBER1_NAME,     \
                         O ## MEMBER2_TRAIT , MEMBER2_NAME )

//-*****************************************************************************
#define ALEMBIC_AMALGAM_TRAIT4( TRAIT_NAME,                     \
                                MEMBER0_TRAIT, MEMBER0_NAME,    \
                                MEMBER1_TRAIT, MEMBER1_NAME,    \
                                MEMBER2_TRAIT, MEMBER2_NAME,    \
                                MEMBER3_TRAIT, MEMBER3_NAME )   \
ALEMBIC_IAMALGAM_TRAIT4( I ## TRAIT_NAME ,                      \
                         I ## MEMBER0_TRAIT , MEMBER0_NAME,     \
                         I ## MEMBER1_TRAIT , MEMBER1_NAME,     \
                         I ## MEMBER2_TRAIT , MEMBER2_NAME,     \
                         I ## MEMBER3_TRAIT , MEMBER3_NAME );   \
ALEMBIC_OAMALGAM_TRAIT4( O ## TRAIT_NAME ,                      \
                         O ## MEMBER0_TRAIT , MEMBER0_NAME,     \
                         O ## MEMBER1_TRAIT , MEMBER1_NAME,     \
                         O ## MEMBER2_TRAIT , MEMBER2_NAME,     \
                         O ## MEMBER3_TRAIT , MEMBER3_NAME )

//-*****************************************************************************
#define ALEMBIC_AMALGAM_TRAIT5( TRAIT_NAME,                     \
                                MEMBER0_TRAIT, MEMBER0_NAME,    \
                                MEMBER1_TRAIT, MEMBER1_NAME,    \
                                MEMBER2_TRAIT, MEMBER2_NAME,    \
                                MEMBER3_TRAIT, MEMBER3_NAME,    \
                                MEMBER4_TRAIT, MEMBER4_NAME )   \
ALEMBIC_IAMALGAM_TRAIT5( I ## TRAIT_NAME ,                      \
                         I ## MEMBER0_TRAIT , MEMBER0_NAME,     \
                         I ## MEMBER1_TRAIT , MEMBER1_NAME,     \
                         I ## MEMBER2_TRAIT , MEMBER2_NAME,     \
                         I ## MEMBER3_TRAIT , MEMBER3_NAME,     \
                         I ## MEMBER4_TRAIT , MEMBER4_NAME );   \
ALEMBIC_OAMALGAM_TRAIT5( O ## TRAIT_NAME ,                      \
                         O ## MEMBER0_TRAIT , MEMBER0_NAME,     \
                         O ## MEMBER1_TRAIT , MEMBER1_NAME,     \
                         O ## MEMBER2_TRAIT , MEMBER2_NAME,     \
                         O ## MEMBER3_TRAIT , MEMBER3_NAME,     \
                         O ## MEMBER4_TRAIT , MEMBER4_NAME )

//-*****************************************************************************
#define ALEMBIC_AMALGAM_TRAIT6( TRAIT_NAME,                     \
                                MEMBER0_TRAIT, MEMBER0_NAME,    \
                                MEMBER1_TRAIT, MEMBER1_NAME,    \
                                MEMBER2_TRAIT, MEMBER2_NAME,    \
                                MEMBER3_TRAIT, MEMBER3_NAME,    \
                                MEMBER4_TRAIT, MEMBER4_NAME,    \
                                MEMBER5_TRAIT, MEMBER5_NAME )   \
ALEMBIC_IAMALGAM_TRAIT6( I ## TRAIT_NAME ,                      \
                         I ## MEMBER0_TRAIT , MEMBER0_NAME,     \
                         I ## MEMBER1_TRAIT , MEMBER1_NAME,     \
                         I ## MEMBER2_TRAIT , MEMBER2_NAME,     \
                         I ## MEMBER3_TRAIT , MEMBER3_NAME,     \
                         I ## MEMBER4_TRAIT , MEMBER4_NAME,     \
                         I ## MEMBER5_TRAIT , MEMBER5_NAME );   \
ALEMBIC_OAMALGAM_TRAIT6( O ## TRAIT_NAME ,                      \
                         O ## MEMBER0_TRAIT , MEMBER0_NAME,     \
                         O ## MEMBER1_TRAIT , MEMBER1_NAME,     \
                         O ## MEMBER2_TRAIT , MEMBER2_NAME,     \
                         O ## MEMBER3_TRAIT , MEMBER3_NAME,     \
                         O ## MEMBER4_TRAIT , MEMBER4_NAME,     \
                         O ## MEMBER5_TRAIT , MEMBER5_NAME )



//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// TRAIT OBJECT MACROS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

#define ALEMBIC_TRAIT_OBJECT( TYPE_NAME, TRAIT, STRAIT )        \
ALEMBIC_ITRAIT_OBJECT( I ## TYPE_NAME, I ## TRAIT, STRAIT );    \
ALEMBIC_OTRAIT_OBJECT( O ## TYPE_NAME, O ## TRAIT, STRAIT )


} // End namespace AlembicAsset

#endif
