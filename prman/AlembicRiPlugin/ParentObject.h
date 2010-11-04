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

#ifndef _AlembicRiPlugin_ParentObject_h_
#define _AlembicRiPlugin_ParentObject_h_

#include "Foundation.h"

namespace AlembicRiPlugin {

//-*****************************************************************************
// Forward Declarations
class Factory;
class Request;
class ParentObject;

typedef boost::shared_ptr<ParentObject> ParentObjectPtr;
typedef std::vector<ParentObjectPtr> ParentObjects;

//-*****************************************************************************
class ParentObject
{
protected:
    ParentObject() {}
    void init( Factory &factory,
               const Abc::IParentObject &thisObject );

public:
    typedef ParentObject this_type;
    
    virtual ~ParentObject() {}
    
    const std::string &name() const { return m_name; }
    const std::string &fullPathName() const { return m_fullPathName; }
    const std::string &protocol() const { return m_protocol; }

    //-*************************************************************************
    // INSTANTIATION!!!
    // 
    // So - sometimes we want to instantiate the whole tree, but this is
    // rare. We more often will want to instantiate objects that match
    // a regex of some kind. When that happens, we still (usually) want
    // the parent objects to be able to pass SOME things down, like transforms,
    // possibly shader statements or RiColor things. Usually just Transforms.
    // 
    // We have a "Request" object which is just an abstract base class
    // that lets you specify whether or not to instantiate local
    // and whether to instantiate children. It also provides information
    // about which part of the asset to render - time samples, etc.
    // 
    //-*************************************************************************
    void instantiate( Request &q );

    //-*************************************************************************
    // Child classes should supply a cast to their parent object.
    virtual const Abc::IParentObject &alembicParentObject() const throw() = 0;
    bool valid() const throw()
    {
        return alembicParentObject().valid();
    }
    std::string errorString() const throw()
    {
        return alembicParentObject().errorString();
    }

    ALEMBIC_OPERATOR_BOOL_NOTHROW( valid() );

protected:
    // This is for any attribute state that would get passed to children.
    // Transforms, at least, possibly more like Color or Shader.
    virtual void instantiateAttributeState( Request &q ) {}
    
    // This is just for the internals of the object itself.
    virtual void instantiateLocal( Request &q ) {}

    // We store a copy of our name, fullPathName, and protocol
    std::string m_name;
    std::string m_fullPathName;
    std::string m_protocol;

    // Note that we don't store a copy of our "self" body.
    // This will be stored by subclasses based on each's individual
    // needs.
    ParentObjects m_children;
};

} // End namespace AlembicRiPlugin

#endif
