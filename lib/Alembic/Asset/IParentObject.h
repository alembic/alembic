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

#ifndef _Alembic_Asset_IParentObject_h_
#define _Alembic_Asset_IParentObject_h_

#include <Alembic/Asset/Foundation.h>
#include <Alembic/Asset/IBase.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
// This is just a virtual base class for OAsset and OObject so that
// they can be passed into any constructor requiring a parenting object.
class IParentObject : public IBase
{
protected:
    IParentObject() : IBase() {}
    IParentObject( const IParentObject &copy ) : IBase( copy ) {}
    void operator=( const IParentObject &copy )
    {
        IBase::operator=( copy );
    }

public:
    virtual std::string comments() const = 0;
    virtual size_t numChildren() const = 0;
    virtual ObjectInfo childInfo( size_t p ) const = 0;
    virtual ObjectInfo childInfo( const std::string &name,
                                  const std::string &prot = "" ) const = 0;
    
    virtual SharedIParentBody asParentBody() const = 0;
};

} // End namespace Asset
} // End namespace Alembic

#endif
