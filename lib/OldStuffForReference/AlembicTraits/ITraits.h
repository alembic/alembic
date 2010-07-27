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

#ifndef _AlembicTraits_ITraits_h_
#define _AlembicTraits_ITraits_h_

#include <AlembicTraits/Foundation.h>
#include <AlembicTraits/StringTrait.h>

namespace AlembicAsset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// EMPTY TRAIT
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// This is a trait that does nothing. It facilitates the amalgam traits below
// by offering a usable default template argument. It also serves as a pretty
// good interface description of a trait. The functions listed below are those
// which a trait should implement.
class IEmptyTrait
{
public:
    typedef IEmptyTrait this_type;

    // Default constructor
    IEmptyTrait() throw() {}

    // Copy constructor
    IEmptyTrait( const IEmptyTrait &copy ) throw() {}

    // Assignment operator
    this_type &operator=( const this_type &copy ) throw() { return *this; }

    // Object constructor (no exceptions)
    explicit IEmptyTrait( const IObject &parent ) throw() {}

    // Object constructor (exceptions)
    IEmptyTrait( const IObject &parent, ThrowExceptionFlag tf ) throw() {}

    virtual ~IEmptyTrait() throw() {}

    // Underlying object (probably an IProperty or amalgam of) exists and
    // is valid.
    bool valid() const throw() { return true; }
    
    // Is animated
    bool isAnimated() const throw() { return false; }

    // Is an optional trait
    bool isOptional() const throw() { return true; }

    // Overloading unspecified bool type for boolean conversion
    ALEMBIC_OPERATOR_BOOL_NOTHROW( valid() );

    // Close and release, with & without exceptions
    void close() throw() {}
    void close( ThrowExceptionFlag tf ) {}
    void release() throw() {}
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// IPROPERTY TRAITS (works equally well for singular or multiple)
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// Input Property Traits. Properties need names to behave like a trait, which
// is to say - standard set of constructors & destructors, etc.
// This also assumes that we're dealing with typed properties that already
// have had their datatype and protocol set.
template <class IPROPERTY, class STRAIT>
class IPropertyTrait : public IPROPERTY
{
public:
    typedef IPropertyTrait<IPROPERTY,STRAIT> this_type;
    typedef IPROPERTY property_type;
    typedef STRAIT strait_type;

    IPropertyTrait() throw() : IPROPERTY() {}
    IPropertyTrait( const this_type &copy ) throw() : IPROPERTY( copy ) {}
    this_type &operator=( const this_type &copy ) throw()
    {
        IPROPERTY::operator=( copy );
        return *this;
    }

    explicit IPropertyTrait( const IObject &parent ) throw()
      : IPROPERTY( parent, STRAIT::c_str() ) {}

    IPropertyTrait( const IObject &parent, ThrowExceptionFlag tf )
      : IPROPERTY( parent, STRAIT::c_str(), tf ) {}

    virtual ~IPropertyTrait() throw() {}

    virtual bool isOptional() const throw() { return false; }
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// INPUT PROPERTY TRAIT MODIFIERS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// The non-animated modifier is not that big a deal for input properties
// because the animation is read automagically from the file. However, this
// can be a placeholder for later, perhaps, and a good spot to hold warnings.
// Still, in the interest of maintaining parity without Output Properties
// we have it here.
template <class IPROP_TRAIT>
class INonAnimated : public IPROP_TRAIT
{
public:
    typedef INonAnimated<IPROP_TRAIT> this_type;
    typedef IPROP_TRAIT property_trait_type;

    INonAnimated() throw() : IPROP_TRAIT() {}
    INonAnimated( const this_type &copy ) throw() : IPROP_TRAIT( copy ) {}
    this_type &operator=( const this_type &copy ) throw()
    {
        IPROP_TRAIT::operator=( copy );
        return *this;
    }
    explicit INonAnimated( const IObject &parent ) throw()
      : IPROP_TRAIT( parent ) {}
    INonAnimated( const IObject &parent, ThrowExceptionFlag tf )
      : IPROP_TRAIT( parent, tf ) {}

    virtual ~INonAnimated() throw() {}

    // Overload isAnimated to always return false.
    bool isAnimated() const throw() { return false; }
};

//-*****************************************************************************
// Optional properties have their boolean cast remapped to "exists".
// valid will return true always. Also, constructors never throw.
template <class IPROP_TRAIT>
class IOptional : public IPROP_TRAIT
{
public:
    typedef IOptional<IPROP_TRAIT> this_type;
    typedef IPROP_TRAIT property_trait_type;

    IOptional() throw() : IPROP_TRAIT() {}
    IOptional( const this_type &copy ) throw() : IPROP_TRAIT( copy ) {}
    this_type &operator=( const this_type &copy ) throw()
    {
        IPROP_TRAIT::operator=( copy );
        return *this;
    }
    explicit IOptional( const IObject &parent ) throw()
      : IPROP_TRAIT( parent ) {}
    IOptional( const IObject &parent, ThrowExceptionFlag tf ) throw()
      : IPROP_TRAIT( parent ) {}

    virtual ~IOptional() throw() {}

    // Overload isOptional to always return true.
    bool isOptional() const throw() { return true; }
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// AMALGAMATION!!!!
// Amalgamate Individual Traits in a way that maps all the functions of a
// trait to all the individual parts.
// 
// We support up to 10 individual traits - any more than that and you have
// to hand-roll your own trait. Traits are specified by creating a
// MetaAmalgamTrait.
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
template <class META> class IAmalgamTrait;

//-*****************************************************************************
template <class MEMBER0_TRAIT,
          class MEMBER1_TRAIT,
          class MEMBER2_TRAIT = IEmptyTrait,
          class MEMBER3_TRAIT = IEmptyTrait,
          class MEMBER4_TRAIT = IEmptyTrait,
          class MEMBER5_TRAIT = IEmptyTrait,
          class MEMBER6_TRAIT = IEmptyTrait,
          class MEMBER7_TRAIT = IEmptyTrait,
          class MEMBER8_TRAIT = IEmptyTrait,
          class MEMBER9_TRAIT = IEmptyTrait>
struct IMetaAmalgamTrait
{
    typedef MEMBER0_TRAIT member0_trait_type;
    typedef MEMBER1_TRAIT member1_trait_type;
    typedef MEMBER2_TRAIT member2_trait_type;
    typedef MEMBER3_TRAIT member3_trait_type;
    typedef MEMBER4_TRAIT member4_trait_type;
    typedef MEMBER5_TRAIT member5_trait_type;
    typedef MEMBER6_TRAIT member6_trait_type;
    typedef MEMBER7_TRAIT member7_trait_type;
    typedef MEMBER8_TRAIT member8_trait_type;
    typedef MEMBER9_TRAIT member9_trait_type;
    
    typedef IMetaAmalgamTrait<MEMBER0_TRAIT,
                              MEMBER1_TRAIT,
                              MEMBER2_TRAIT,
                              MEMBER3_TRAIT,
                              MEMBER4_TRAIT,
                              MEMBER5_TRAIT,
                              MEMBER6_TRAIT,
                              MEMBER7_TRAIT,
                              MEMBER8_TRAIT,
                              MEMBER9_TRAIT> this_type;
    
    typedef IAmalgamTrait<this_type> amalgam_type;

    IMetaAmalgamTrait() throw() {}

    member0_trait_type &member0() throw()
    { return static_cast<amalgam_type *>( this )->__member0(); }
    const member0_trait_type &member0() const throw()
    { return static_cast<const amalgam_type *>( this )->__member0(); }
    
    member1_trait_type &member1() throw()
    { return static_cast<amalgam_type *>( this )->__member1(); }
    const member1_trait_type &member1() const throw()
    { return static_cast<const amalgam_type *>( this )->__member1(); }
    
    member2_trait_type &member2() throw()
    { return static_cast<amalgam_type *>( this )->__member2(); }
    const member2_trait_type &member2() const throw()
    { return static_cast<const amalgam_type *>( this )->__member2(); }
    
    member3_trait_type &member3() throw()
    { return static_cast<amalgam_type *>( this )->__member3(); }
    const member3_trait_type &member3() const throw()
    { return static_cast<const amalgam_type *>( this )->__member3(); }
    
    member4_trait_type &member4() throw()
    { return static_cast<amalgam_type *>( this )->__member4(); }
    const member4_trait_type &member4() const throw()
    { return static_cast<const amalgam_type *>( this )->__member4(); }
    
    member5_trait_type &member5() throw()
    { return static_cast<amalgam_type *>( this )->__member5(); }
    const member5_trait_type &member5() const throw()
    { return static_cast<const amalgam_type *>( this )->__member5(); }
    
    member6_trait_type &member6() throw()
    { return static_cast<amalgam_type *>( this )->__member6(); }
    const member6_trait_type &member6() const throw()
    { return static_cast<const amalgam_type *>( this )->__member6(); }
    
    member7_trait_type &member7() throw()
    { return static_cast<amalgam_type *>( this )->__member7(); }
    const member7_trait_type &member7() const throw()
    { return static_cast<const amalgam_type *>( this )->__member7(); }
    
    member8_trait_type &member8() throw()
    { return static_cast<amalgam_type *>( this )->__member8(); }
    const member8_trait_type &member8() const throw()
    { return static_cast<const amalgam_type *>( this )->__member8(); }
    
    member9_trait_type &member9() throw()
    { return static_cast<amalgam_type *>( this )->__member9(); }
    const member9_trait_type &member9() const throw()
    { return static_cast<const amalgam_type *>( this )->__member9(); }
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// INPUT AMALGAM
// For now, supports up to ten amalgamated members. Anything more than that
// and you just have to make your own, hand-rolled amalgamated trait.
// I love that I sometimes get to write sentences like that, and that they
// have contextual meaning.
// Amalgams need at least two template arguments, the rest can be defaulted
// to the empty trait.
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
template <class META_TRAIT>
class IAmalgamTrait : public META_TRAIT
{
public:
    typedef META_TRAIT meta_trait_type;
    typedef typename META_TRAIT::member0_trait_type member0_trait_type;
    typedef typename META_TRAIT::member1_trait_type member1_trait_type;
    typedef typename META_TRAIT::member2_trait_type member2_trait_type;
    typedef typename META_TRAIT::member3_trait_type member3_trait_type;
    typedef typename META_TRAIT::member4_trait_type member4_trait_type;
    typedef typename META_TRAIT::member5_trait_type member5_trait_type;
    typedef typename META_TRAIT::member6_trait_type member6_trait_type;
    typedef typename META_TRAIT::member7_trait_type member7_trait_type;
    typedef typename META_TRAIT::member8_trait_type member8_trait_type;
    typedef typename META_TRAIT::member9_trait_type member9_trait_type;
    typedef IAmalgamTrait<META_TRAIT> this_type;

    IAmalgamTrait() throw()
      : m_member0(),
        m_member1(),
        m_member2(),
        m_member3(),
        m_member4(),
        m_member5(),
        m_member6(),
        m_member7(),
        m_member8(),
        m_member9() {}

    IAmalgamTrait( const this_type &copy ) throw()
      : m_member0( copy.m_member0 ),
        m_member1( copy.m_member1 ),
        m_member2( copy.m_member2 ),
        m_member3( copy.m_member3 ),
        m_member4( copy.m_member4 ),
        m_member5( copy.m_member5 ),
        m_member6( copy.m_member6 ),
        m_member7( copy.m_member7 ),
        m_member8( copy.m_member8 ),
        m_member9( copy.m_member9 ) {}

    this_type &operator=( const this_type &copy ) throw()
    {
        m_member0 = copy.m_member0;
        m_member1 = copy.m_member1;
        m_member2 = copy.m_member2;
        m_member3 = copy.m_member3;
        m_member4 = copy.m_member4;
        m_member5 = copy.m_member5;
        m_member6 = copy.m_member6;
        m_member7 = copy.m_member7;
        m_member8 = copy.m_member8;
        m_member9 = copy.m_member9;
        return *this;
    }

    IAmalgamTrait( const IObject &parent ) throw()
      : m_member0( parent ),
        m_member1( parent ),
        m_member2( parent ),
        m_member3( parent ),
        m_member4( parent ),
        m_member5( parent ),
        m_member6( parent ),
        m_member7( parent ),
        m_member8( parent ),
        m_member9( parent ) {}

    IAmalgamTrait( const IObject &parent,
                   ThrowExceptionFlag tf )
      : m_member0( parent, tf ),
        m_member1( parent, tf ),
        m_member2( parent, tf ),
        m_member3( parent, tf ),
        m_member4( parent, tf ),
        m_member5( parent, tf ),
        m_member6( parent, tf ),
        m_member7( parent, tf ),
        m_member8( parent, tf ),
        m_member9( parent, tf ) {}

    virtual ~IAmalgamTrait() throw() {}

    virtual bool valid() const throw()
    {
        return ( ( m_member0.valid() || m_member0.isOptional() ) &&
                 ( m_member1.valid() || m_member1.isOptional() ) &&
                 ( m_member2.valid() || m_member2.isOptional() ) &&
                 ( m_member3.valid() || m_member3.isOptional() ) &&
                 ( m_member4.valid() || m_member4.isOptional() ) &&
                 ( m_member5.valid() || m_member5.isOptional() ) &&
                 ( m_member6.valid() || m_member6.isOptional() ) &&
                 ( m_member7.valid() || m_member7.isOptional() ) &&
                 ( m_member8.valid() || m_member8.isOptional() ) &&
                 ( m_member9.valid() || m_member9.isOptional() ) );
    }

    bool isAnimated() const throw()
    {
        return ( m_member0.isAnimated() ||
                 m_member1.isAnimated() ||
                 m_member2.isAnimated() ||
                 m_member3.isAnimated() ||
                 m_member4.isAnimated() ||
                 m_member5.isAnimated() ||
                 m_member6.isAnimated() ||
                 m_member7.isAnimated() ||
                 m_member8.isAnimated() ||
                 m_member9.isAnimated() );
    }

    virtual bool isOptional() const throw() { return false; }

    void close() throw()
    {
        m_member0.close();
        m_member1.close();
        m_member2.close();
        m_member3.close();
        m_member4.close();
        m_member5.close();
        m_member6.close();
        m_member7.close();
        m_member8.close();
        m_member9.close();
    }
    
    void close( ThrowExceptionFlag tf )
    {
        m_member0.close( tf );
        m_member1.close( tf );
        m_member2.close( tf );
        m_member3.close( tf );
        m_member4.close( tf );
        m_member5.close( tf );
        m_member6.close( tf );
        m_member7.close( tf );
        m_member8.close( tf );
        m_member9.close( tf );
    }

    void release() throw()
    {
        m_member0.release();
        m_member1.release();
        m_member2.release();
        m_member3.release();
        m_member4.release();
        m_member5.release();
        m_member6.release();
        m_member7.release();
        m_member8.release();
        m_member9.release();
    }
    
    member0_trait_type &__member0() throw()
    { return m_member0; }
    const member0_trait_type &__member0() const throw()
    { return m_member0; }
    
    member1_trait_type &__member1() throw()
    { return m_member1; }
    const member1_trait_type &__member1() const throw()
    { return m_member1; }
    
    member2_trait_type &__member2() throw()
    { return m_member2; }
    const member2_trait_type &__member2() const throw()
    { return m_member2; }
    
    member3_trait_type &__member3() throw()
    { return m_member3; }
    const member3_trait_type &__member3() const throw()
    { return m_member3; }
    
    member4_trait_type &__member4() throw()
    { return m_member4; }
    const member4_trait_type &__member4() const throw()
    { return m_member4; }
    
    member5_trait_type &__member5() throw()
    { return m_member5; }
    const member5_trait_type &__member5() const throw()
    { return m_member5; }
    
    member6_trait_type &__member6() throw()
    { return m_member6; }
    const member6_trait_type &__member6() const throw()
    { return m_member6; }
    
    member7_trait_type &__member7() throw()
    { return m_member7; }
    const member7_trait_type &__member7() const throw()
    { return m_member7; }
    
    member8_trait_type &__member8() throw()
    { return m_member8; }
    const member8_trait_type &__member8() const throw()
    { return m_member8; }
    
    member9_trait_type &__member9() throw()
    { return m_member9; }
    const member9_trait_type &__member9() const throw()
    { return m_member9; }

    ALEMBIC_OPERATOR_BOOL_NOTHROW( valid() );

protected:
    member0_trait_type m_member0;
    member1_trait_type m_member1;
    member2_trait_type m_member2;
    member3_trait_type m_member3;
    member4_trait_type m_member4;
    member5_trait_type m_member5;
    member6_trait_type m_member6;
    member7_trait_type m_member7;
    member8_trait_type m_member8;
    member9_trait_type m_member9;
};

} // End namespace AlembicAsset

#endif
