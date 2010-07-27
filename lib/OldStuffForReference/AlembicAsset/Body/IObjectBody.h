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

#ifndef _AlembicAsset_IObjectBody_h_
#define _AlembicAsset_IObjectBody_h_

#include <AlembicAsset/Body/FoundationBody.h>
#include <AlembicAsset/Body/IParentBody.h>
#include <vector>
#include <string>

namespace AlembicAsset {

//-*****************************************************************************
// This function reads IPropertyInfo structs out of a group and puts them
// into a vector
void ReadPropertyInfo( const H5G &group,
                       std::vector<PropertyInfo> &into );

//-*****************************************************************************
// An Object (Oid, Node, PropertyContainer, Lump, Etc)
// consists of:
// 
// METADATA:
//    name
//    protocol (like maya's apiType)
//    (eventually) tags
//    comments
//
// PROPERTIES:
//    ordered list of properties
//
// CHILDREN:
//    ordered list of child objects.
//
// For properties and children, we don't actually load them at all,
// but we do iterate over them when we are loaded to build a list
// of metadata
//-*****************************************************************************

class IObjectBody : public IParentBody
{
public:
    // For construction given a name and a check protocol
    // Pass "" for checkProt to disable checks
    IObjectBody( const H5G &grp,
                 const std::string &nme,
                 const std::string &checkProt );

    // For construction given ObjectInfo
    IObjectBody( const H5G &grp,
                 ObjectInfo oinfo );

    // For construction given a parent IObjectBody and child index
    IObjectBody( const IParentBody &parent,
                 size_t whichChild );

    // Calls close.
    virtual ~IObjectBody() throw();

    const std::string &name() const { return m_name; }
    std::string fullPathName() const;
    const std::string &protocol() const { return m_protocol; }
    const std::string &comments() const { return m_comments; }

    //-*************************************************************************
    // PROPERTY INTERFACE
    //-*************************************************************************
    const H5G &propertyGroup() const { return m_propertyGroup; }
    size_t numProperties() const { return m_propertyInfo.size(); }
    PropertyInfo propertyInfo( size_t p ) const
    {
        //AAH5_ASSERT( p < m_propertyInfo.size(),
        //             "Out of range property index: " << p
        //             << " for object: " << m_name
        //             << " which only has: " << m_propertyInfo.size()
        //             << " properties." );
        if ( p < m_propertyInfo.size() )
        {
            return m_propertyInfo[p];
        }
        else
        {
            PropertyInfo pinfo;
            return pinfo;
        }
    }
    
    PropertyInfo propertyInfo( const std::string &nme,
                               const std::string &prot = "" ) const
    {
        for ( std::vector<PropertyInfo>::const_iterator iter =
                  m_propertyInfo.begin();
              iter != m_propertyInfo.end(); ++iter )
        {
            if ( (*iter)->name == nme &&
                 ( prot == "" ||
                   prot == (*iter)->protocol ) )
            {
                return (*iter);
            }
        }

        PropertyInfo pinfo;
        return pinfo;
    }

    //-*************************************************************************
    // CHILDREN INTERFACE
    // Inherited from IParentBody
    //-*************************************************************************

    //-*************************************************************************
    // CLOSE!
    //-*************************************************************************
    virtual void close();
    
protected:
    // Called by the various constructors
    void init( const H5G &grp,
               const std::string &nme,
               const std::string &checkProt );
    
    std::string m_name;
    std::string m_protocol;
    std::string m_comments;

    // Group corresponding to this object.
    H5G m_group;

    // Properties
    H5G m_propertyGroup;
    std::vector<PropertyInfo> m_propertyInfo;
};

} // End namespace AlembicAsset

#endif
