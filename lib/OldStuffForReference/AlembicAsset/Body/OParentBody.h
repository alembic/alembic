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

#ifndef _AlembicAsset_Body_OParentBody_h_
#define _AlembicAsset_Body_OParentBody_h_

#include <AlembicAsset/Body/FoundationBody.h>
#include <AlembicAsset/Body/OContextBody.h>

namespace AlembicAsset {

//-*****************************************************************************
// A Parent object just has a group to contain children
class OParentBody
{
public:
    // Won't throw exceptions
    virtual ~OParentBody() throw();
    
    // Return context.
    SharedOContextBody context() const throw() { return m_context; }

    // Close.
    virtual void close();
    
    //-*************************************************************************
    // CHILDREN INTERFACE
    //-*************************************************************************
    const H5G &childrenGroup() const throw() { return m_childrenGroup; }

protected:
    OParentBody( SharedOContextBody ctx )
      : m_context( ctx )
    {
        AAH5_ASSERT( ( bool )ctx,
                     "Null SharedOContexBody passed to OParentBody ctor." );
    }

    // Call from subclass constructors
    // Could throw many things.
    void parentInit( const H5G &parentGroup,
                     const std::string &childGroupName );

    // Children
    H5G m_childrenGroup;

    // And context.
    SharedOContextBody m_context;
};

} // End namespace AlembicAsset

#endif
