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

#ifndef _Alembic_Traits_IMacros_h_
#define _Alembic_Traits_IMacros_h_

#include <Alembic/Traits/Foundation.h>
#include <Alembic/Traits/ITraits.h>
#include <Alembic/Traits/StringTrait.h>
#include <Alembic/Traits/TraitObject.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// PROPERTY MACROS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
#define ALEMBIC_IPROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE, STRAIT )         \
typedef Alembic::Asset::IPropertyTrait< PROPERTY_TYPE, STRAIT > TRAIT_NAME

#define ALEMBIC_OPT_IPROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE, STRAIT )     \
typedef Alembic::Asset::IOptional<                                           \
    Alembic::Asset::IPropertyTrait< PROPERTY_TYPE, STRAIT > > TRAIT_NAME

#define ALEMBIC_NONANIM_IPROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE, STRAIT ) \
typedef Alembic::Asset::INonAnimated<                                        \
    Alembic::Asset::IPropertyTrait< PROPERTY_TYPE, STRAIT > > TRAIT_NAME

#define ALEMBIC_OPT_NONANIM_IPROPERTY_TRAIT( TRAIT_NAME, PROPERTY_TYPE,      \
                                             STRAIT )                        \
typedef Alembic::Asset::IOptional<                                           \
    Alembic::Asset::INonAnimated<                                            \
    Alembic::Asset::IPropertyTrait< PROPERTY_TYPE, STRAIT > > > TRAIT_NAME

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// AMALGAM MACROS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
#define ALEMBIC_IAMALGAM_TRAIT2( TRAIT_NAME ,                           \
                                 MEMBER0_TRAIT , MEMBER0_NAME,          \
                                 MEMBER1_TRAIT , MEMBER1_NAME )         \
struct Meta ## TRAIT_NAME                                               \
    : public Alembic::Asset::IMetaAmalgamTrait< MEMBER0_TRAIT,          \
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
typedef Alembic::Asset::IAmalgamTrait< Meta ## TRAIT_NAME > TRAIT_NAME

//-*****************************************************************************
#define ALEMBIC_IAMALGAM_TRAIT3( TRAIT_NAME ,                           \
                                 MEMBER0_TRAIT , MEMBER0_NAME,          \
                                 MEMBER1_TRAIT , MEMBER1_NAME,          \
                                 MEMBER2_TRAIT , MEMBER2_NAME )         \
struct Meta ## TRAIT_NAME                                               \
    : public Alembic::Asset::IMetaAmalgamTrait< MEMBER0_TRAIT,          \
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
typedef Alembic::Asset::IAmalgamTrait< Meta ## TRAIT_NAME > TRAIT_NAME

//-*****************************************************************************
#define ALEMBIC_IAMALGAM_TRAIT4( TRAIT_NAME ,                           \
                                 MEMBER0_TRAIT , MEMBER0_NAME,          \
                                 MEMBER1_TRAIT , MEMBER1_NAME,          \
                                 MEMBER2_TRAIT , MEMBER2_NAME,          \
                                 MEMBER3_TRAIT , MEMBER3_NAME )         \
struct Meta ## TRAIT_NAME                                               \
    : public Alembic::Asset::IMetaAmalgamTrait< MEMBER0_TRAIT,          \
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
typedef Alembic::Asset::IAmalgamTrait< Meta ## TRAIT_NAME > TRAIT_NAME
  
//-*****************************************************************************
#define ALEMBIC_IAMALGAM_TRAIT5( TRAIT_NAME ,                           \
                                 MEMBER0_TRAIT , MEMBER0_NAME,          \
                                 MEMBER1_TRAIT , MEMBER1_NAME,          \
                                 MEMBER2_TRAIT , MEMBER2_NAME,          \
                                 MEMBER3_TRAIT , MEMBER3_NAME,          \
                                 MEMBER4_TRAIT , MEMBER4_NAME )         \
struct Meta ## TRAIT_NAME                                               \
    : public Alembic::Asset::IMetaAmalgamTrait< MEMBER0_TRAIT,          \
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
typedef Alembic::Asset::IAmalgamTrait< Meta ## TRAIT_NAME > TRAIT_NAME
   
//-*****************************************************************************
#define ALEMBIC_IAMALGAM_TRAIT6( TRAIT_NAME ,                           \
                                 MEMBER0_TRAIT , MEMBER0_NAME,          \
                                 MEMBER1_TRAIT , MEMBER1_NAME,          \
                                 MEMBER2_TRAIT , MEMBER2_NAME,          \
                                 MEMBER3_TRAIT , MEMBER3_NAME,          \
                                 MEMBER4_TRAIT , MEMBER4_NAME,          \
                                 MEMBER5_TRAIT , MEMBER5_NAME )         \
struct Meta ## TRAIT_NAME                                               \
    : public Alembic::Asset::IMetaAmalgamTrait< MEMBER0_TRAIT,          \
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
typedef Alembic::Asset::IAmalgamTrait< Meta ## TRAIT_NAME > TRAIT_NAME

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// TRAIT OBJECT MACROS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

#define ALEMBIC_ITRAIT_OBJECT( TYPE_NAME, TRAIT, STRAIT )               \
typedef Alembic::Asset::ITraitObject< TRAIT, STRAIT > TYPE_NAME ;

} // End namespace Asset
} // End namespace Alembic

#endif

