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

#ifndef _AlembicAsset_OBase_h_
#define _AlembicAsset_OBase_h_

#include <AlembicAsset/Foundation.h>
#include <AlembicUtil/OperatorBool.h>

//-*****************************************************************************
// All output objects depend on the underlying asset remaining in existence.
// for this reason, all the base objects must keep a shared pointer to
// the asset body, all the way down. Users will never directly see this class.
//-*****************************************************************************

namespace AlembicAsset {

//-*****************************************************************************
class OBase
{
protected:
    OBase() throw() : m_errorString( "" ), m_assetBody() {}
    OBase( const OBase &copy ) throw()
      : m_errorString( copy.m_errorString ),
        m_assetBody( copy.m_assetBody ) {}
    
    void operator=( const OBase &copy ) throw()
    {
        m_errorString = copy.m_errorString;
        m_assetBody = copy.m_assetBody;
    }
    
public:
    typedef OBase this_type;

    // Virtual destructor. Releases resources, but does
    // not close them.
    virtual ~OBase() throw() {}
    
    SharedOAssetBody assetBody() const throw() { return m_assetBody; }
    virtual bool valid() const throw() = 0;

    // Closes the object, whatever it may be,
    // and releases this object's hold on its body.
    virtual void close() throw() = 0;

    // Same, but with exceptions if failure.
    virtual void close( ThrowExceptionFlag ) = 0;

    // Release lets go of this object's hold on its resources.
    // Like smart_ptr->reset(), but error string is preserved.
    virtual void release() throw();

    // Error string stuff.
    const std::string &errorString() const throw() { return m_errorString; }

    // Add operator bool and operator!
    ALEMBIC_OPERATOR_BOOL_NOTHROW( this->valid() );

protected:
    void setErrorString( const std::string &err = "" ) throw()
    { m_errorString = err; }
    
    std::string m_errorString;

    // Every object keeps a pointer to its parent asset body.
    // This is to prevent the parent asset from going away too soon.
    SharedOAssetBody m_assetBody;
};

} // End namespace AlembicAsset

#endif
