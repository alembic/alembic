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

#ifndef _AlembicAsset_IAsset_h_
#define _AlembicAsset_IAsset_h_

#include <AlembicAsset/IParentObject.h>

namespace AlembicAsset {

//-*****************************************************************************
class IAsset : public IParentObject
{
public:
    IAsset() throw() : IParentObject() {}

    // This constructor will not throw an exception.
    IAsset( const std::string &fname,
            const IContext &ctx = IContext() ) throw();

    // This constructor WILL throw an exception.
    IAsset( const std::string &fname, ThrowExceptionFlag,
            const IContext &ctx = IContext() );

    // Copy constructors.
    IAsset( const IAsset &copy ) throw();

    // This gets the assetBody from the base object.
    explicit IAsset( const IBase &copyOther ) throw();

    // Assignment
    IAsset &operator=( const IAsset &copy ) throw();
    IAsset &operator=( const IBase &copyOther ) throw();

    std::string fileName() const throw();
    virtual std::string name() const throw();
    virtual std::string fullPathName() const throw();
    virtual std::string protocol() const throw();
    virtual std::string comments() const throw();
    

    virtual size_t numChildren() const throw();
    virtual ObjectInfo childInfo( size_t p ) const throw();
    virtual ObjectInfo childInfo( const std::string &nme,
                                  const std::string &prot = "" ) const throw();
    
    virtual bool valid() const throw();
    virtual void close() throw();
    virtual void close( ThrowExceptionFlag );
    virtual void release() throw();

    SharedIAssetBody body() const throw(){ return m_assetBody; }
    virtual SharedIParentBody asParentBody() const throw();
};

} // End namespace AlembicAsset

#endif
