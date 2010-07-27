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

#ifndef _Alembic_Asset_OAsset_h_
#define _Alembic_Asset_OAsset_h_

#include <Alembic/Asset/Foundation.h>
#include <Alembic/Asset/OParentObject.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
class OAsset : public OParentObject
{
public:
    OAsset() throw();
    
    // This constructor will not throw an exception.
    explicit OAsset( const std::string &fname ) throw();

    // This one will.
    OAsset( const std::string &fname, ThrowExceptionFlag );

    // Copy
    OAsset( const OAsset &copy ) throw();

    // These constructors build a reference to an existing asset
    // by pulling from the base object's asset body.
    explicit OAsset( const OBase &copyBase ) throw();

    // Assign
    OAsset &operator=( const OAsset &copy ) throw();
    OAsset &operator=( const OBase &copyBase ) throw();
    
    virtual void setComments( const std::string &cmt ) throw();

    virtual std::string name() const throw();
    std::string fileName() const throw();

    //-*************************************************************************
    // COMPRESSION!!!
    //-*************************************************************************
    
    // Set the compression level. Defaults to half-way, 5 out of 10.
    int compressionLevel() const throw();
    void setCompressionLevel( int level ) throw();

    SharedOAssetBody body() const throw() { return m_assetBody; }
    virtual SharedOParentBody asParentBody() const throw();
    virtual bool valid() const throw();

    // Closes and releases.
    virtual void close() throw();
    virtual void close( ThrowExceptionFlag );

    //-*************************************************************************
    // API INHERITED FROM OBASE
    //-*************************************************************************
    // All Objects have a pointer to their asset. The asset class itself
    // returns the same value for assetBody() as it does from body().
    // SharedOAssetBody assetBody() const { return m_assetBody; }
    //
    // All objects can be asked if valid.
    // virtual bool valid() const;
    // 
    // All objects maintain an error string.
    // const std::string &errorString() const { return m_errorString; }
    //
    // All objects can be released.
    // virtual void release(); 
    //
    // All objects can be cast to a boolean. Same as calling valid()
    // operator unspecified_bool_type() const
    //
    // All objects have ! - same as !valid()
    // operator !() const
    //-*************************************************************************
};

} // End namespace Asset
} // End namespace Alembic


#endif
