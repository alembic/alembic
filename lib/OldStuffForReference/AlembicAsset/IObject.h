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

#ifndef _AlembicAsset_IObject_h_
#define _AlembicAsset_IObject_h_

#include <AlembicAsset/Foundation.h>
#include <AlembicAsset/IAsset.h>

namespace AlembicAsset {

//-*****************************************************************************
class IObject : public IParentObject
{
private:
    // Initialization function used by all the constructors.
    void init( const IParentObject &parent,
               const std::string &nme,
               const std::string &prot,
               const IContext &ctx );
    
public:
    //-*************************************************************************
    // EMPTY IOBJECT
    //-*************************************************************************
    IObject() throw() : IParentObject() {}
    
    //-*************************************************************************
    // CONSTRUCTION FROM PARENT
    //-*************************************************************************

    //-*************************************************************************
    // Construction with explicit name and protocol
    // If no context passed, parent's will be used.
    // NO EXCEPTIONS
    IObject( const IParentObject &parent,
             const std::string &nme,
             const std::string &prot = "",
             const IContext &ctx = IContext() ) throw();
    
    // EXCEPTIONS
    IObject( const IParentObject &parent,
             const std::string &nme,
             ThrowExceptionFlag tf,
             const std::string &prot = "",
             const IContext &ctx = IContext() );

    //-*************************************************************************
    // Construction with property info
    // NO EXCEPTIONS
    IObject( const IParentObject &parent,
             ObjectInfo oinfo,
             const IContext &ctx = IContext() ) throw();
    
    // EXCEPTIONS
    IObject( const IParentObject &parent,
             ObjectInfo oinfo,
             ThrowExceptionFlag flag,
             const IContext &ctx = IContext() );

    //-*************************************************************************
    // Construction from an asset with a given child index
    // NO EXCEPTIONS
    IObject( const IParentObject &parent,
             size_t idx,
             const IContext &ctx = IContext() ) throw();

    // EXCEPTIONS
    IObject( const IParentObject &parent,
             size_t idx,
             ThrowExceptionFlag flag,
             const IContext &ctx = IContext() );

    //-*************************************************************************
    // COPY CONSTRUCTION, ASSIGNMENT
    //-*************************************************************************
    IObject( const IObject &copy ) throw();

    IObject &operator=( const IObject &copy ) throw();
    
    //-*************************************************************************
    // FUNCTIONS
    //-*************************************************************************
    virtual std::string name() const throw();
    virtual std::string fullPathName() const throw();
    virtual std::string protocol() const throw();
    virtual std::string comments() const throw();

    size_t numProperties() const throw();
    PropertyInfo propertyInfo( size_t p ) const throw();
    PropertyInfo propertyInfo( const std::string &name,
                               const std::string &prot = "" ) const throw();


    // Inherited from IParentObject
    virtual size_t numChildren() const throw();
    virtual ObjectInfo childInfo( size_t c ) const throw();
    virtual ObjectInfo childInfo( const std::string &name,
                                  const std::string &prot = "" ) const throw();

    // Close & release
    virtual bool valid() const throw();
    virtual void close() throw(); // No exceptions
    virtual void close( ThrowExceptionFlag );
    virtual void release() throw();

    //-*************************************************************************
    // RETURN INTERNALS
    //-*************************************************************************
    SharedIObjectBody body() const throw() { return m_body; }
    virtual SharedIParentBody asParentBody() const throw();

protected:
    SharedIObjectBody m_body;
};

} // End namespace AlembicAsset

#endif
