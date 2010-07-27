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

#ifndef _AlembicTraits_TraitObject_h_
#define _AlembicTraits_TraitObject_h_

#include <AlembicTraits/ITraits.h>
#include <AlembicTraits/OTraits.h>

namespace AlembicAsset {

//-*****************************************************************************
template <class TRAIT, class STRAIT>
class ITraitObject : public IObject
{
public:
    typedef TRAIT trait_type;
    typedef STRAIT strait_type;
    typedef ITraitObject<TRAIT,STRAIT> this_type;
    
    ITraitObject() throw() : IObject(), m_trait() {}

    //-*************************************************************************
    // CONSTRUCTION FROM PARENT
    //-*************************************************************************

    //-*************************************************************************
    // Construction with explicit name and protocol
    // If no context passed, parent's will be used.
    // NO EXCEPTIONS
    ITraitObject( const IParentObject &parent,
                  const std::string &nme,
                  const std::string &prot,
                  const IContext &ctx = IContext() ) throw()
      : IObject( parent, nme, prot, ctx ),
        m_trait( *this ) {}
    
    // EXCEPTIONS
    ITraitObject( const IParentObject &parent,
                  const std::string &nme,
                  const std::string &prot,
                  ThrowExceptionFlag tf,
                  const IContext &ctx = IContext() )
      : IObject( parent, nme, tf, prot, ctx ),
        m_trait( *this, tf ) {}

    //-*************************************************************************
    // Construction with explicit name and protocol
    // If no context passed, parent's will be used.
    // NO EXCEPTIONS
    ITraitObject( const IParentObject &parent,
                  const std::string &nme,
                  const IContext &ctx = IContext() ) throw()
      : IObject( parent, nme, STRAIT::c_str(), ctx ),
        m_trait( *this ) {}
    
    // EXCEPTIONS
    ITraitObject( const IParentObject &parent,
                  const std::string &nme,
                  ThrowExceptionFlag tf,
                  const IContext &ctx = IContext() )
      : IObject( parent, nme, tf, STRAIT::c_str(), ctx ),
        m_trait( *this, tf ) {}

    //-*************************************************************************
    // Construction with property info
    // NO EXCEPTIONS
    ITraitObject( const IParentObject &parent,
                  ObjectInfo oinfo,
                  const IContext &ctx = IContext() ) throw()
      : IObject( parent, oinfo, ctx ),
        m_trait( *this ) {}
    
    // EXCEPTIONS
    ITraitObject( const IParentObject &parent,
                  ObjectInfo oinfo,
                  ThrowExceptionFlag flag,
                  const IContext &ctx = IContext() )
      : IObject( parent, oinfo, flag, ctx ),
        m_trait( *this, flag ) {}

    //-*************************************************************************
    // Construction from an asset with a given child index
    // NO EXCEPTIONS
    ITraitObject( const IParentObject &parent,
                  size_t idx,
                  const IContext &ctx = IContext() ) throw()
      : IObject( parent, idx, ctx ),
        m_trait( *this ) {}

    // EXCEPTIONS
    ITraitObject( const IParentObject &parent,
                  size_t idx,
                  ThrowExceptionFlag flag,
                  const IContext &ctx = IContext() )
      : IObject( parent, idx, flag, ctx ),
        m_trait( *this, flag ) {}

    //-*************************************************************************
    // COPY CONSTRUCTION, ASSIGNMENT
    //-*************************************************************************
    ITraitObject( const this_type &copy ) throw()
      : IObject( copy ),
        m_trait( copy.m_trait ) {}

    this_type &operator=( const this_type &copy ) throw()
    {
        IObject::operator=( copy );
        m_trait = copy.m_trait;
        return *this;
    }

    //-*************************************************************************
    //-*************************************************************************
    // CLOSE & RELEASE
    //-*************************************************************************
    //-*************************************************************************
    virtual void close() throw()
    {
        if ( m_trait )
        {
            m_trait.close();
        }
        IObject::close();
    }
    virtual void close( ThrowExceptionFlag tf )
    {
        if ( m_trait )
        {
            m_trait.close( tf );
        }
        IObject::close( tf );
    }
    virtual void release() throw()
    {
        m_trait.release();
        IObject::release();
    }

    //-*************************************************************************
    //-*************************************************************************
    // CASTING
    //-*************************************************************************
    //-*************************************************************************
    TRAIT& operator*() throw() { return m_trait; }
    const TRAIT& operator*() const throw() { return m_trait; }

    TRAIT* operator->() throw() { return &m_trait; }
    const TRAIT* operator->() const throw() { return &m_trait; }

    TRAIT& trait() throw() { return m_trait; }
    const TRAIT& trait() const throw() { return m_trait; }

protected:
    TRAIT m_trait;
};

//-*****************************************************************************
template <class TRAIT, class STRAIT>
class OTraitObject : public OObject
{
public:
    typedef TRAIT trait_type;
    typedef STRAIT strait_type;
    typedef OTraitObject<TRAIT,STRAIT> this_type;
    
    OTraitObject() throw() : OObject(), m_trait()
    {
        m_trait.setOptionalParent( *this );
    }

    //-*************************************************************************
    // Construction with explicit name and protocol
    // If no context passed, parent's will be used.
    // NO EXCEPTIONS
    OTraitObject( const OParentObject &parent,
                  const std::string &nme,
                  const std::string &prot ) throw()
      : OObject( parent, nme, prot ),
        m_trait( *this )
    {
        m_trait.setOptionalParent( *this );
    }
    
    // EXCEPTIONS
    OTraitObject( const OParentObject &parent,
                  const std::string &nme,
                  const std::string &prot,
                  ThrowExceptionFlag tf )
      : OObject( parent, nme, prot, tf ),
        m_trait( *this, tf )
    {
        m_trait.setOptionalParent( *this );
    }

    //-*************************************************************************
    // Construction with explicit name and protocol
    // If no context passed, parent's will be used.
    // NO EXCEPTIONS
    OTraitObject( const OParentObject &parent,
                  const std::string &nme ) throw()
      : OObject( parent, nme, STRAIT::c_str() ),
        m_trait( *this )
    {
        m_trait.setOptionalParent( *this );
    }
    
    // EXCEPTIONS
    OTraitObject( const OParentObject &parent,
                  const std::string &nme,
                  ThrowExceptionFlag tf )
      : OObject( parent, nme, STRAIT::c_str(), tf ),
        m_trait( *this, tf )
    {
        m_trait.setOptionalParent( *this );
    }

    //-*************************************************************************
    // COPY CONSTRUCTION, ASSIGNMENT
    //-*************************************************************************
    OTraitObject( const this_type &copy ) throw()
      : OObject( copy ),
        m_trait( copy.m_trait ) {}

    this_type &operator=( const this_type &copy ) throw()
    {
        OObject::operator=( copy );
        m_trait = copy.m_trait;
        return *this;
    }

    //-*************************************************************************
    //-*************************************************************************
    // CLOSE & RELEASE
    //-*************************************************************************
    //-*************************************************************************
    virtual void close() throw()
    {
        if ( m_trait )
        {
            m_trait.close();
        }
        OObject::close();
    }
    virtual void close( ThrowExceptionFlag tf )
    {
        if ( m_trait )
        {
            m_trait.close( tf );
        }
        OObject::close( tf );
    }
    virtual void release() throw()
    {
        m_trait.release();
        OObject::release();
    }

    //-*************************************************************************
    //-*************************************************************************
    // CASTING
    //-*************************************************************************
    //-*************************************************************************
    TRAIT& operator*() throw() { return m_trait; }
    const TRAIT& operator*() const throw() { return m_trait; }

    TRAIT* operator->() throw() { return &m_trait; }
    const TRAIT* operator->() const throw() { return &m_trait; }

    TRAIT& trait() throw() { return m_trait; }
    const TRAIT& trait() const throw() { return m_trait; }

protected:
    TRAIT m_trait;
};

} // End namespace AlembicAsset

#endif
