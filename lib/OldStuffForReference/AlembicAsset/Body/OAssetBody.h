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

#ifndef _AlembicAsset_Body_OAssetBody_h_
#define _AlembicAsset_Body_OAssetBody_h_

#include <AlembicAsset/Body/FoundationBody.h>
#include <AlembicAsset/Body/OParentBody.h>
#include <string>

namespace AlembicAsset {

//-*****************************************************************************
// An Asset consists of:
// FileName,
// FileHandle
// Comments.
// A list of children. (children functionality inherited from IParentBody)
class OAssetBody : public OParentBody
{
public:
    OAssetBody( const std::string &fName ) ;

    // Destroying an asset body will not throw an exception, so
    // use 'close' to catch those things if you need to.
    virtual ~OAssetBody() throw();
    
    const std::string &fileName() const { return m_fileName; }
    
    const H5F &file() const { return m_file; }

    // Set the compression level. Defaults to half-way, 5 out of 10.
    int compressionLevel() const;
    void setCompressionLevel( int level );

    // Close the file!
    virtual void close();

    void setComments( const std::string &cmt ) { m_file.setComment( cmt ); }
    
    // CHILDREN INTERFACE
    // Inherited from OParentBody
    
protected:
    std::string m_fileName;
    H5F m_file;

    // The Asset needs to store weak pointer references to every
    // single object that has been created underneath it, to prevent
    // 'uncloseable' assets. 
};

} // End namespace AlembicAsset

#endif
