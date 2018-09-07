//-*****************************************************************************
//
// Copyright (c) 2012,
//  Sony Pictures Imageworks, Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
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

#ifndef Alembic_AbcCoreHDF5_HDF5Hierarchy_h
#define Alembic_AbcCoreHDF5_HDF5Hierarchy_h

#include <Alembic/AbcCoreHDF5/Foundation.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

class HDF5Hierarchy;

//-*****************************************************************************
class H5Node
{
public:
    H5Node()
        : m_object( -1 ), m_ref( 0 ), m_H5HPtr( NULL ) {}
    H5Node( hid_t iObject, hobj_ref_t iRef, HDF5Hierarchy* iH5HPtr )
        : m_object( iObject ), m_ref( iRef ), m_H5HPtr( iH5HPtr ) {}

    hid_t               getObject() const           { return m_object; }
    void                setObject( hid_t iObject )  { m_object = iObject; }

    hobj_ref_t          getRef() const              { return m_ref; }
    HDF5Hierarchy*      getH5HPtr() const           { return m_H5HPtr; }

    bool                isValidObject() const       { return m_object >= 0; }

private:
    hid_t               m_object;
    hobj_ref_t          m_ref;
    HDF5Hierarchy*      m_H5HPtr;
};

//-*****************************************************************************
class HDF5Hierarchy
{
public:
    HDF5Hierarchy() {}
    ~HDF5Hierarchy() { clear(); }

    H5Node createNode( hid_t iId );

    void build( hid_t iFile );
    void clear();
    bool isEnabled()                    { return m_enabled; }
    void setEnabled( bool iEnabled )    { m_enabled = iEnabled; }

    hobj_ref_t  getChildRef( hobj_ref_t iParentRef, const std::string &iName );
    bool        childExists( hobj_ref_t iParentRef, const std::string &iName );
    bool        attrExists( hobj_ref_t iParentRef, const std::string &iName );

    void        readMetaDataString( hobj_ref_t iParentRef,
                                    const std::string &iMetaDataName,
                                    std::string &oMetaDataString );

    void        readMaskInfo( hobj_ref_t iParentRef,
                              const std::string &iPropName,
                              size_t& oNumFields,
                              void *oData );

    template<class T>
    void visitAllChildObjects( H5Node iParent,
                               const std::string &iName,
                               T &iVisitor )
    {
        hobj_ref_t objectRef     = getChildRef( iParent.getRef(), iName );
        ChildInfoArray& children = m_objectMap[objectRef].m_children;

        for( ChildInfoArray::iterator it = children.begin();
             it != children.end(); ++it )
        {
            iVisitor( it->m_name );
        }
    }

    template<class T>
    void visitAllAttributes( hobj_ref_t iParentRef,
                             const std::string &iName,
                             T& iVisitor )
    {
        hobj_ref_t objectRef = getChildRef( iParentRef, iName );
        AttrInfoArray& attrs = m_objectMap[objectRef].m_attrs;

        for( AttrInfoArray::iterator it = attrs.begin();
             it != attrs.end(); ++it )
        {
            std::string attrName( it->m_name );
            size_t attrNameLen = attrName.size();
            if ( attrNameLen < 6 )
            {
                return;
            }

            // Last 5 characters.
            std::string suffix( attrName, attrNameLen-5 );
            if ( suffix == ".info" )
            {
                std::string propertyName( attrName, 0, attrNameLen-5 );
                iVisitor( propertyName );
            }
        }
    }

    void makeCompactObjectHierarchy(
        std::vector<hobj_ref_t>     &oObjectRefs,
        std::vector<uint32_t>       &oChildrenSizes,
        std::vector<std::string>    &oChildrenNames,
        std::vector<hobj_ref_t>     &oChildrenRefs,
        std::vector<uint32_t>       &oAttrSizes,
        std::vector<std::string>    &oAttrNames,
        std::vector<char>           &oHasMask,
        std::vector<uint32_t>       &oMaskBits,
        std::vector<char>           &oHasMeta,
        std::vector<std::string>    &oMetaStrs );

    void extractFromCompactObjectHierarchy(
        hid_t                       iFile,
        std::vector<hobj_ref_t>     &iObjectRefs,
        std::vector<uint32_t>       &iChildrenSizes,
        std::vector<std::string>    &iChildrenNames,
        std::vector<hobj_ref_t>     &iChildrenRefs,
        std::vector<uint32_t>       &iAttrSizes,
        std::vector<std::string>    &iAttrNames,
        std::vector<char>           &iHasMask,
        std::vector<uint32_t>       &iMaskBits,
        std::vector<char>           &iHasMeta,
        std::vector<std::string>    &iMetaStrs );

private:
    void addObject( hid_t iParent, const char *iName );
    void addAttr( hid_t iParent, const char *iName );

    friend class ObjectVisitor;

    struct ChildInfo
    {
        ChildInfo( const string &iName, hobj_ref_t iRef = 0 )
            : m_name( iName ), m_ref( iRef ) {}

        string     m_name;
        hobj_ref_t m_ref;

        friend inline bool operator<( const ChildInfo &x, const ChildInfo &y )
        {
            return x.m_name < y.m_name;
        }
    };

    struct MaskInfo
    {
        size_t          m_numFields;
        uint32_t        m_data[5];
    };

    struct AttrInfo
    {
        AttrInfo( const string &iName ) : m_name( iName ), m_mask( NULL ) {}

        string    m_name;
        string    m_meta;
        MaskInfo* m_mask;

        void clear()
        {
            delete m_mask;
            m_mask = NULL;
        }

        friend inline bool operator<( const AttrInfo &x, const AttrInfo &y )
        {
            return x.m_name < y.m_name;
        }
    };

    typedef std::vector<ChildInfo>              ChildInfoArray;
    typedef std::vector<AttrInfo>               AttrInfoArray;

    struct ObjectInfo
    {
        ChildInfoArray  m_children;
        AttrInfoArray   m_attrs;
    };

    typedef std::map<hobj_ref_t, ObjectInfo> ObjectMap;

    ObjectMap   m_objectMap;
    bool        m_enabled;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreHDF5
} // End namespace Alembic

#endif
