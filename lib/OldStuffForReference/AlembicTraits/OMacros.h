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

#ifndef _AlembicTraits_OMacros_h_
#define _AlembicTraits_OMacros_h_

#include <AlembicTraits/Foundation.h>
#include <AlembicTraits/OTraits.h>
#include <AlembicTraits/StringTrait.h>
#include <AlembicTraits/TraitObject.h>

namespace AlembicAsset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// PROPERTY MACROS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
#define ALEMBIC_OPROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE, STRAIT )         \
typedef AlembicAsset::OPropertyTrait< PROPERTY_TYPE, STRAIT > TRAIT_NAME

#define ALEMBIC_OPT_OPROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE, STRAIT )     \
typedef AlembicAsset::OOptional<                                           \
    AlembicAsset::OPropertyTrait< PROPERTY_TYPE, STRAIT > > TRAIT_NAME

#define ALEMBIC_NONANIM_OPROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE, STRAIT ) \
typedef AlembicAsset::ONonAnimated<                                        \
    AlembicAsset::OPropertyTrait< PROPERTY_TYPE, STRAIT > > TRAIT_NAME

#define ALEMBIC_OPT_NONANIM_OPROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE,      \
                                             STRAIT )                        \
typedef AlembicAsset::OOptional<                                           \
    AlembicAsset::ONonAnimated<                                            \
    AlembicAsset::OPropertyTrait< PROPERTY_TYPE, STRAIT > > > TRAIT_NAME


//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// AMALGAM MACROS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
#define ALEMBIC_OAMALGAM_TRAIT2( TRAIT_NAME ,                           \
                                 MEMBER0_TRAIT , MEMBER0_NAME,          \
                                 MEMBER1_TRAIT , MEMBER1_NAME )         \
struct Meta ## TRAIT_NAME                                               \
    : public AlembicAsset::OMetaAmalgamTrait< MEMBER0_TRAIT,          \
                                                MEMBER1_TRAIT >         \
{                                                                       \
    typedef MEMBER0_TRAIT MEMBER0_NAME ## _type;                        \
    typedef MEMBER1_TRAIT MEMBER1_NAME ## _type;                        \
                                                                        \
    MEMBER0_TRAIT & MEMBER0_NAME () throw()                             \
    { return member0(); }                                               \
    const MEMBER0_TRAIT & MEMBER0_NAME () const throw()                 \
    { return member0(); }                                               \
                                                                        \
    MEMBER1_TRAIT & MEMBER1_NAME () throw()                             \
    { return member1(); }                                               \
    const MEMBER1_TRAIT & MEMBER1_NAME () const throw()                 \
    { return member1(); }                                               \
};                                                                      \
typedef AlembicAsset::OAmalgamTrait< Meta ## TRAIT_NAME > TRAIT_NAME

//-*****************************************************************************
#define ALEMBIC_OAMALGAM_TRAIT3( TRAIT_NAME ,                           \
                                 MEMBER0_TRAIT , MEMBER0_NAME,          \
                                 MEMBER1_TRAIT , MEMBER1_NAME,          \
                                 MEMBER2_TRAIT , MEMBER2_NAME )         \
struct Meta ## TRAIT_NAME                                               \
    : public AlembicAsset::OMetaAmalgamTrait< MEMBER0_TRAIT,          \
                                                MEMBER1_TRAIT,          \
                                                MEMBER2_TRAIT >         \
{                                                                       \
    typedef MEMBER0_TRAIT MEMBER0_NAME ## _type;                        \
    typedef MEMBER1_TRAIT MEMBER1_NAME ## _type;                        \
    typedef MEMBER2_TRAIT MEMBER2_NAME ## _type;                        \
                                                                        \
    MEMBER0_TRAIT & MEMBER0_NAME () throw()                             \
    { return member0(); }                                               \
    const MEMBER0_TRAIT & MEMBER0_NAME () const throw()                 \
    { return member0(); }                                               \
                                                                        \
    MEMBER1_TRAIT & MEMBER1_NAME () throw()                             \
    { return member1(); }                                               \
    const MEMBER1_TRAIT & MEMBER1_NAME () const throw()                 \
    { return member1(); }                                               \
                                                                        \
    MEMBER2_TRAIT & MEMBER2_NAME () throw()                             \
    { return member2(); }                                               \
    const MEMBER2_TRAIT & MEMBER2_NAME () const throw()                 \
    { return member2(); }                                               \
};                                                                      \
typedef AlembicAsset::OAmalgamTrait< Meta ## TRAIT_NAME > TRAIT_NAME

//-*****************************************************************************
#define ALEMBIC_OAMALGAM_TRAIT4( TRAIT_NAME ,                           \
                                 MEMBER0_TRAIT , MEMBER0_NAME,          \
                                 MEMBER1_TRAIT , MEMBER1_NAME,          \
                                 MEMBER2_TRAIT , MEMBER2_NAME,          \
                                 MEMBER3_TRAIT , MEMBER3_NAME )         \
struct Meta ## TRAIT_NAME                                               \
    : public AlembicAsset::OMetaAmalgamTrait< MEMBER0_TRAIT,          \
                                                MEMBER1_TRAIT,          \
                                                MEMBER2_TRAIT,          \
                                                MEMBER3_TRAIT >         \
{                                                                       \
    typedef MEMBER0_TRAIT MEMBER0_NAME ## _type;                        \
    typedef MEMBER1_TRAIT MEMBER1_NAME ## _type;                        \
    typedef MEMBER2_TRAIT MEMBER2_NAME ## _type;                        \
    typedef MEMBER3_TRAIT MEMBER3_NAME ## _type;                        \
                                                                        \
    MEMBER0_TRAIT & MEMBER0_NAME () throw()                             \
    { return member0(); }                                               \
    const MEMBER0_TRAIT & MEMBER0_NAME () const throw()                 \
    { return member0(); }                                               \
                                                                        \
    MEMBER1_TRAIT & MEMBER1_NAME () throw()                             \
    { return member1(); }                                               \
    const MEMBER1_TRAIT & MEMBER1_NAME () const throw()                 \
    { return member1(); }                                               \
                                                                        \
    MEMBER2_TRAIT & MEMBER2_NAME () throw()                             \
    { return member2(); }                                               \
    const MEMBER2_TRAIT & MEMBER2_NAME () const throw()                 \
    { return member2(); }                                               \
                                                                        \
    MEMBER3_TRAIT & MEMBER3_NAME () throw()                             \
    { return member3(); }                                               \
    const MEMBER3_TRAIT & MEMBER3_NAME () const throw()                 \
    { return member3(); }                                               \
};                                                                      \
typedef AlembicAsset::OAmalgamTrait< Meta ## TRAIT_NAME > TRAIT_NAME
  
//-*****************************************************************************
#define ALEMBIC_OAMALGAM_TRAIT5( TRAIT_NAME ,                           \
                                 MEMBER0_TRAIT , MEMBER0_NAME,          \
                                 MEMBER1_TRAIT , MEMBER1_NAME,          \
                                 MEMBER2_TRAIT , MEMBER2_NAME,          \
                                 MEMBER3_TRAIT , MEMBER3_NAME,          \
                                 MEMBER4_TRAIT , MEMBER4_NAME )         \
struct Meta ## TRAIT_NAME                                               \
    : public AlembicAsset::OMetaAmalgamTrait< MEMBER0_TRAIT,          \
                                                MEMBER1_TRAIT,          \
                                                MEMBER2_TRAIT,          \
                                                MEMBER3_TRAIT,          \
                                                MEMBER4_TRAIT >         \
{                                                                       \
    typedef MEMBER0_TRAIT MEMBER0_NAME ## _type;                        \
    typedef MEMBER1_TRAIT MEMBER1_NAME ## _type;                        \
    typedef MEMBER2_TRAIT MEMBER2_NAME ## _type;                        \
    typedef MEMBER3_TRAIT MEMBER3_NAME ## _type;                        \
    typedef MEMBER4_TRAIT MEMBER4_NAME ## _type;                        \
                                                                        \
    MEMBER0_TRAIT & MEMBER0_NAME () throw()                             \
    { return member0(); }                                               \
    const MEMBER0_TRAIT & MEMBER0_NAME () const throw()                 \
    { return member0(); }                                               \
                                                                        \
    MEMBER1_TRAIT & MEMBER1_NAME () throw()                             \
    { return member1(); }                                               \
    const MEMBER1_TRAIT & MEMBER1_NAME () const throw()                 \
    { return member1(); }                                               \
                                                                        \
    MEMBER2_TRAIT & MEMBER2_NAME () throw()                             \
    { return member2(); }                                               \
    const MEMBER2_TRAIT & MEMBER2_NAME () const throw()                 \
    { return member2(); }                                               \
                                                                        \
    MEMBER3_TRAIT & MEMBER3_NAME () throw()                             \
    { return member3(); }                                               \
    const MEMBER3_TRAIT & MEMBER3_NAME () const throw()                 \
    { return member3(); }                                               \
                                                                        \
    MEMBER4_TRAIT & MEMBER4_NAME () throw()                             \
    { return member4(); }                                               \
    const MEMBER4_TRAIT & MEMBER4_NAME () const throw()                 \
    { return member4(); }                                               \
};                                                                      \
typedef AlembicAsset::OAmalgamTrait< Meta ## TRAIT_NAME > TRAIT_NAME
   
//-*****************************************************************************
#define ALEMBIC_OAMALGAM_TRAIT6( TRAIT_NAME ,                           \
                                 MEMBER0_TRAIT , MEMBER0_NAME,          \
                                 MEMBER1_TRAIT , MEMBER1_NAME,          \
                                 MEMBER2_TRAIT , MEMBER2_NAME,          \
                                 MEMBER3_TRAIT , MEMBER3_NAME,          \
                                 MEMBER4_TRAIT , MEMBER4_NAME,          \
                                 MEMBER5_TRAIT , MEMBER5_NAME )         \
struct Meta ## TRAIT_NAME                                               \
    : public AlembicAsset::OMetaAmalgamTrait< MEMBER0_TRAIT,          \
                                                MEMBER1_TRAIT,          \
                                                MEMBER2_TRAIT,          \
                                                MEMBER3_TRAIT,          \
                                                MEMBER4_TRAIT,          \
                                                MEMBER5_TRAIT >         \
{                                                                       \
    typedef MEMBER0_TRAIT MEMBER0_NAME ## _type;                        \
    typedef MEMBER1_TRAIT MEMBER1_NAME ## _type;                        \
    typedef MEMBER2_TRAIT MEMBER2_NAME ## _type;                        \
    typedef MEMBER3_TRAIT MEMBER3_NAME ## _type;                        \
    typedef MEMBER4_TRAIT MEMBER4_NAME ## _type;                        \
    typedef MEMBER5_TRAIT MEMBER5_NAME ## _type;                        \
                                                                        \
    MEMBER0_TRAIT & MEMBER0_NAME () throw()                             \
    { return member0(); }                                               \
    const MEMBER0_TRAIT & MEMBER0_NAME () const throw()                 \
    { return member0(); }                                               \
                                                                        \
    MEMBER1_TRAIT & MEMBER1_NAME () throw()                             \
    { return member1(); }                                               \
    const MEMBER1_TRAIT & MEMBER1_NAME () const throw()                 \
    { return member1(); }                                               \
                                                                        \
    MEMBER2_TRAIT & MEMBER2_NAME () throw()                             \
    { return member2(); }                                               \
    const MEMBER2_TRAIT & MEMBER2_NAME () const throw()                 \
    { return member2(); }                                               \
                                                                        \
    MEMBER3_TRAIT & MEMBER3_NAME () throw()                             \
    { return member3(); }                                               \
    const MEMBER3_TRAIT & MEMBER3_NAME () const throw()                 \
    { return member3(); }                                               \
                                                                        \
    MEMBER4_TRAIT & MEMBER4_NAME () throw()                             \
    { return member4(); }                                               \
    const MEMBER4_TRAIT & MEMBER4_NAME () const throw()                 \
    { return member4(); }                                               \
                                                                        \
    MEMBER5_TRAIT & MEMBER5_NAME () throw()                             \
    { return member5(); }                                               \
    const MEMBER5_TRAIT & MEMBER5_NAME () const throw()                 \
    { return member5(); }                                               \
};                                                                      \
typedef AlembicAsset::OAmalgamTrait< Meta ## TRAIT_NAME > TRAIT_NAME

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// TRAIT OBJECT MACROS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

#define ALEMBIC_OTRAIT_OBJECT( TYPE_NAME, TRAIT, STRAIT )               \
typedef AlembicAsset::OTraitObject< TRAIT, STRAIT > TYPE_NAME ;

} // End namespace AlembicAsset

#endif

