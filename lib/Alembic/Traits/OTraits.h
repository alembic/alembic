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

#ifndef _Alembic_Traits_OTraits_h_
#define _Alembic_Traits_OTraits_h_

#include <Alembic/Traits/Foundation.h>
#include <Alembic/Traits/StringTrait.h>

namespace Alembic {
namespace Asset {

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
class OEmptyTrait
{
public:
    typedef OEmptyTrait this_type;

    // Default constructor
    OEmptyTrait() throw() {}

    // Copy constructor
    OEmptyTrait( const OEmptyTrait &copy ) throw() {}

    // Assignment operator
    this_type &operator=( const this_type &copy ) throw() { return *this; }

    // Object constructor (no exceptions)
    explicit OEmptyTrait( const OObject &parent ) throw() {}

    // Object constructor (exceptions)
    OEmptyTrait( const OObject &parent, ThrowExceptionFlag tf ) {}

    virtual ~OEmptyTrait() throw() {}

    // Underlying object (probably an IProperty or amalgam of) exists and
    // is valid.
    bool valid() const throw() { return true; }
    
    // Is animated
    bool isAnimated() const throw() { return false; }

    // Make animated.
    void makeAnimated( const TimeSamplingInfo &tinfo ) throw() {}

    // Is an optional trait
    bool isOptional() const throw() { return false; }

    // Optional helpers -
    void setOptionalParent( const OObject &parent ) throw() {}
    void addOptional() throw() {}
    void addOptional( ThrowExceptionFlag tf ) {}

    // Overloading unspecified bool type for boolean conversion
    ALEMBIC_OPERATOR_BOOL( valid() );

    // Close and release, with & without exceptions
    void close() throw() {}
    void close( ThrowExceptionFlag tf ) {}
    void release() throw() {}
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// OPROPERTY TRAITS (works equally well for singular or multiple)
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// Output Property Traits. Properties need names to behave like a trait, which
// is to say - standard set of constructors & destructors, etc.
// This also assumes that we're dealing with typed properties that already
// have had their datatype and protocol set.
template <class OPROPERTY, class STRAIT>
class OPropertyTrait : public OPROPERTY
{
public:
    typedef OPropertyTrait<OPROPERTY,STRAIT> this_type;
    typedef OPROPERTY property_type;
    typedef STRAIT strait_type;

    OPropertyTrait() throw() : OPROPERTY()
    {
        //std::cout << "Default constructor for OPropertyTrait: "
        //          << STRAIT::c_str() << std::endl;
    }
    OPropertyTrait( const this_type &copy ) throw()
      : OPROPERTY( copy )
    {
        //std::cout << "Copy constructor for OPropertyTrait: "
        //          << STRAIT::c_str() << std::endl;
    }
    this_type &operator=( const this_type &copy ) throw()
    {
        OPROPERTY::operator=( copy );
        return *this;
    }

    explicit OPropertyTrait( const OObject &parent ) throw()
      : OPROPERTY( parent, STRAIT::c_str() ) {}

    OPropertyTrait( const OObject &parent, ThrowExceptionFlag tf )
      : OPROPERTY( parent, STRAIT::c_str(), tf ) {}

    virtual ~OPropertyTrait() throw() {}

    bool isOptional() const throw() { return false; }
    
    // Optional helpers -
    void setOptionalParent( const OObject &parent ) throw() {}
    void addOptional() throw() {}
    void addOptional( ThrowExceptionFlag tf ) {}
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// OUTPUT PROPERTY TRAIT MODIFIERS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// The non-animated modifier is not that big a deal for output properties
// because the animation is read automagically from the file. However, this
// can be a placeholder for later, perhaps, and a good spot to hold warnings.
// Still, in the interest of maintaining parity without Output Properties
// we have it here.
template <class OPROP_TRAIT>
class ONonAnimated : public OPROP_TRAIT
{
public:
    typedef ONonAnimated<OPROP_TRAIT> this_type;
    typedef OPROP_TRAIT property_trait_type;

    ONonAnimated() throw() : OPROP_TRAIT() {}
    ONonAnimated( const this_type &copy ) throw() : OPROP_TRAIT( copy ) {}
    this_type &operator=( const this_type &copy ) throw()
    {
        OPROP_TRAIT::operator=( copy );
        return *this;
    }
    explicit ONonAnimated( const OObject &parent ) throw()
      : OPROP_TRAIT( parent ) {}
    ONonAnimated( const OObject &parent, ThrowExceptionFlag tf )
      : OPROP_TRAIT( parent, tf ) {}

    virtual ~ONonAnimated() throw() {}

    // Overload isAnimated to always return false.
    bool isAnimated() const throw() { return false; }

    // Overload make animated to do nothing.
    // Could possibly print an error if we wanted to.
    void makeAnimated( const TimeSamplingInfo &tinfo ) throw() {}
};

//-*****************************************************************************
// Optional properties have their boolean cast remapped to "exists".
// valid will return true always. Also, constructors never throw.
template <class OPROP_TRAIT>
class OOptional
{
public:
    typedef OOptional<OPROP_TRAIT> this_type;
    typedef OPROP_TRAIT property_trait_type;

    OOptional() throw()
      : m_savedParent(),
        m_optionalTrait() {}
    
    OOptional( const this_type &copy ) throw()
      : m_savedParent( copy.m_savedParent ),
        m_optionalTrait( copy.m_optionalTrait ) {}
    
    this_type &operator=( const this_type &copy ) throw()
    {
        m_savedParent = copy.m_savedParent;
        m_optionalTrait = copy.m_optionalTrait;
        return *this;
    }
    
    explicit OOptional( const OObject &parent ) throw()
      : m_savedParent( parent ),
        m_optionalTrait() {}
    
    OOptional( const OObject &parent, ThrowExceptionFlag tf )
      : m_savedParent( parent ),
        m_optionalTrait() {}

    virtual ~OOptional() throw() {}
    
    virtual bool valid() const throw()
    {
        return ( bool )m_optionalTrait;
    }

    virtual void close() throw()
    {
        if ( m_optionalTrait )
        {
            m_optionalTrait.close();
        }
        this->release();
    }
    virtual void close( ThrowExceptionFlag tf )
    {
        if ( m_optionalTrait )
        {
            m_optionalTrait.close( tf );
        }
        this->release();
    }
    virtual void release() throw()
    {
        m_savedParent.release();
        m_optionalTrait.release();
    }

    // Overload isOptional to always return true.
    bool isOptional() const throw() { return true; }
    
    // Optional helpers -
    void setOptionalParent( const OObject &parent ) throw()
    {
        m_savedParent = parent;
    }
    void addOptional() throw()
    {
        try
        {
            if ( m_savedParent && !m_optionalTrait )
            {
                m_optionalTrait = OPROP_TRAIT( m_savedParent );
            }
        }
        catch ( ... )
        {
            if ( m_optionalTrait )
            {
                m_optionalTrait.close();
            }
            this->release();
        }
    }
    
    void addOptional( ThrowExceptionFlag tf )
    {
        AAST_ASSERT( m_savedParent,
                     "Tried to add optional property with null parent." );
        AAST_ASSERT( !m_optionalTrait.valid(),
                     "Multiple add attempts with optional trait." );

        //std::cout << "\n\nOOptional:: About to add optional: " << std::endl;
        m_optionalTrait = OPROP_TRAIT( m_savedParent, tf );
        //std::cout << "Added optional trait: " << m_optionalTrait.name()
        //          << std::endl;
    }

    // Cast to prop trait.
    OPROP_TRAIT& operator*() throw() { return m_optionalTrait; }
    const OPROP_TRAIT& operator*() const throw() { return m_optionalTrait; }
    OPROP_TRAIT* operator->() throw() { return &m_optionalTrait; }
    const OPROP_TRAIT* operator->() const throw() { return &m_optionalTrait; }

    OPROP_TRAIT& trait() throw() { return m_optionalTrait; }
    const OPROP_TRAIT& trait() const throw() { return m_optionalTrait; }
    
    // Add operator bool and operator!
    ALEMBIC_OPERATOR_BOOL( this->valid() );

protected:
    OObject m_savedParent;
    OPROP_TRAIT m_optionalTrait;
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
template <class META> class OAmalgamTrait;

//-*****************************************************************************
template <class MEMBER0_TRAIT,
          class MEMBER1_TRAIT,
          class MEMBER2_TRAIT = OEmptyTrait,
          class MEMBER3_TRAIT = OEmptyTrait,
          class MEMBER4_TRAIT = OEmptyTrait,
          class MEMBER5_TRAIT = OEmptyTrait,
          class MEMBER6_TRAIT = OEmptyTrait,
          class MEMBER7_TRAIT = OEmptyTrait,
          class MEMBER8_TRAIT = OEmptyTrait,
          class MEMBER9_TRAIT = OEmptyTrait>
struct OMetaAmalgamTrait
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
    
    typedef OMetaAmalgamTrait<MEMBER0_TRAIT,
                              MEMBER1_TRAIT,
                              MEMBER2_TRAIT,
                              MEMBER3_TRAIT,
                              MEMBER4_TRAIT,
                              MEMBER5_TRAIT,
                              MEMBER6_TRAIT,
                              MEMBER7_TRAIT,
                              MEMBER8_TRAIT,
                              MEMBER9_TRAIT> this_type;
    
    typedef OAmalgamTrait<this_type> amalgam_type;

    OMetaAmalgamTrait() throw() {}

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
// OUTPUT AMALGAM
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
class OAmalgamTrait : public META_TRAIT
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
    typedef OAmalgamTrait<META_TRAIT> this_type;

    OAmalgamTrait() throw()
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

    OAmalgamTrait( const this_type &copy ) throw()
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

    OAmalgamTrait( const OObject &parent ) throw()
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

    OAmalgamTrait( const OObject &parent,
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

    virtual ~OAmalgamTrait() throw() {}

    bool valid() const throw()
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

    void makeAnimated( const TimeSamplingInfo &tinfo )
    {
        m_member0.makeAnimated( tinfo );
        m_member1.makeAnimated( tinfo );
        m_member2.makeAnimated( tinfo );
        m_member3.makeAnimated( tinfo );
        m_member4.makeAnimated( tinfo );
        m_member5.makeAnimated( tinfo );
        m_member6.makeAnimated( tinfo );
        m_member7.makeAnimated( tinfo );
        m_member8.makeAnimated( tinfo );
        m_member9.makeAnimated( tinfo );
    }

    // Even if the members of an amalgam are optional, it is not.
    bool isOptional() const throw() { return false; }

    void setOptionalParent( const OObject &parent )
    {
        m_member0.setOptionalParent( parent );
        m_member1.setOptionalParent( parent );
        m_member2.setOptionalParent( parent );
        m_member3.setOptionalParent( parent );
        m_member4.setOptionalParent( parent );
        m_member5.setOptionalParent( parent );
        m_member6.setOptionalParent( parent );
        m_member7.setOptionalParent( parent );
        m_member8.setOptionalParent( parent );
        m_member9.setOptionalParent( parent );
    }

    // Optionals must be added individually.
    void addOptional() throw() {}
    void addOptional( ThrowExceptionFlag tf ) {}

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

    ALEMBIC_OPERATOR_BOOL( valid() );

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

} // End namespace Asset
} // End namespace Alembic

#endif
