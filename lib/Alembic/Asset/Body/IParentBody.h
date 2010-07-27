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

#ifndef _Alembic_Asset_Body_IParentBody_h_
#define _Alembic_Asset_Body_IParentBody_h_

#include <Alembic/Asset/Body/FoundationBody.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
// This function reads IObjectInfo structs out of a group and puts them
// into a vector.
void ReadObjectInfo( const H5G &group,
                     std::vector<ObjectInfo> &into );

//-*****************************************************************************
// A Parent has children objects and a group to contain them.
class IParentBody
{
public:
    //-*************************************************************************
    // CHILDREN INTERFACE
    //-*************************************************************************
    const H5G &childrenGroup() const throw() { return m_childrenGroup; }
    size_t numChildren() const throw() { return m_childrenInfo.size(); }
    ObjectInfo childInfo( size_t p ) const throw()
    {
        if ( p >= m_childrenInfo.size() )
        {
            ObjectInfo o;
            return o;
        }
        else
        {
            return m_childrenInfo[p];
        }
    }

    ObjectInfo childInfo( const std::string &nme,
                          const std::string &prot = "" ) const throw()
    {
        for ( std::vector<ObjectInfo>::const_iterator iter =
                  m_childrenInfo.begin();
              iter != m_childrenInfo.end(); ++iter )
        {
            if ( (*iter)->name == nme &&
                 ( prot == "" ||
                   prot == (*iter)->protocol ) )
            {
                return (*iter);
            }
        }
        ObjectInfo c;
        return c;
    }
    
    virtual ~IParentBody() throw();
    
    virtual void close();

protected:
    IParentBody() throw() {}

    // Call from subclass constructors
    void parentInit( const H5G &parentGroup,
                     const std::string &childGroupName );

    // Children
    H5G m_childrenGroup;
    std::vector<ObjectInfo> m_childrenInfo;
};

} // End namespace Asset
} // End namespace Alembic

#endif
