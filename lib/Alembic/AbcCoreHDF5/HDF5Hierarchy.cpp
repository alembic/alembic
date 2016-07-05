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

#include <algorithm>
#include <Alembic/AbcCoreHDF5/HDF5Hierarchy.h>
#include <Alembic/AbcCoreHDF5/HDF5Util.h>
#include <Alembic/AbcCoreHDF5/ReadUtil.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ObjectVisitor
{
public:
    ObjectVisitor( HDF5Hierarchy &iParent )
      : m_parent( iParent ) {}

public:
    void addObject( hid_t iGroup, const char *iName )
    {
        m_parent.addObject( iGroup, iName );
    }

    void addAttr( hid_t iGroup, const char *iName )
    {
        m_parent.addAttr( iGroup, iName );
    }

private:
    HDF5Hierarchy &m_parent;
};

//-*****************************************************************************
static herr_t VisitAllAttrsCB( hid_t iGroup,
                               const char *iAttrName,
                               const H5A_info_t *iAinfo,
                               void *iOpData )
{
    ObjectVisitor *visitor = ( ObjectVisitor * )iOpData;
    visitor->addAttr( iGroup, iAttrName );

    return 0;
}

//-*****************************************************************************
static herr_t VisitAllGroupsCB( hid_t iGroup,
                                const char *iName,
                                const H5L_info_t *iOinfo,
                                void *iOpData )
{
    ObjectVisitor *visitor = ( ObjectVisitor * )iOpData;

    H5O_info_t Oinfo;
    H5Oget_info_by_name( iGroup, iName, &Oinfo, H5P_DEFAULT );

    if ( Oinfo.type == H5O_TYPE_GROUP )
    {
        H5Literate_by_name( iGroup, iName,
                            H5_INDEX_NAME,
                            H5_ITER_INC,
                            NULL,
                            VisitAllGroupsCB,
                            iOpData,
                            H5P_DEFAULT );

        visitor->addObject( iGroup, iName );

        H5Aiterate_by_name( iGroup, iName,
                            H5_INDEX_NAME,
                            H5_ITER_INC,
                            NULL,
                            VisitAllAttrsCB,
                            iOpData,
                            H5P_DEFAULT );

    }
    else if (Oinfo.type == H5O_TYPE_DATASET )
    {
        visitor->addObject( iGroup, iName );

        H5Aiterate_by_name( iGroup, iName,
                            H5_INDEX_NAME,
                            H5_ITER_INC,
                            NULL,
                            VisitAllAttrsCB,
                            iOpData,
                            H5P_DEFAULT );
    }

    // Keep iterating!
    return 0;
}

//-*****************************************************************************
H5Node HDF5Hierarchy::createNode( hid_t iObject )
{
    hobj_ref_t ref;
    H5Rcreate( &ref, iObject, ".", H5R_OBJECT, -1 );

    return H5Node( iObject, ref, isEnabled() ? this : NULL );
}

//-*****************************************************************************
void HDF5Hierarchy::build( hid_t iFile )
{
    clear();

    ObjectVisitor visitor( *this );
    H5Literate( iFile,
                H5_INDEX_NAME,
                H5_ITER_INC,
                NULL,
                VisitAllGroupsCB,
                ( void * )&visitor);
}

//-*****************************************************************************
void HDF5Hierarchy::clear()
{
    for( ObjectMap::iterator it = m_objectMap.begin();
         it != m_objectMap.end(); ++it )
    {
        it->second.m_attrs.clear();
    }

    m_objectMap.clear();
}

//-*****************************************************************************
void HDF5Hierarchy::addObject( hid_t iParent, const char *iName )
{
    hobj_ref_t parentRef, childRef;
    H5Rcreate( &parentRef, iParent, ".", H5R_OBJECT, -1 );
    H5Rcreate( &childRef, iParent, iName, H5R_OBJECT, -1 );

    m_objectMap[parentRef].m_children.push_back(
                                            ChildInfo( iName, childRef ) );
}

namespace {
    const std::string g_strInfo( ".info" );
    const std::string g_strMeta( ".meta" );
}

//-*****************************************************************************
void HDF5Hierarchy::addAttr( hid_t iParent, const char *iName )
{
    hobj_ref_t parentRef;
    H5Rcreate( &parentRef, iParent, ".", H5R_OBJECT, -1 );

    // attribute
    AttrInfoArray& attrs = m_objectMap[parentRef].m_attrs;
    attrs.push_back( AttrInfo( iName ) );
    AttrInfo& info = attrs[attrs.size()-1];

    std::string strName( iName );
    const size_t len = strName.size();
    if ( len < 6 ) return;

    // property header mask
    if ( strName.compare( len-5, 5, g_strInfo ) == 0 )
    {
        ABCA_ASSERT( !info.m_mask,
                     "A property header mask alreasy exists." );

        info.m_mask = new MaskInfo;
        info.m_mask->m_numFields = 0;

        ReadSmallArray( iParent, iName, H5T_STD_U32LE,
                        H5T_NATIVE_UINT32, 5,
                        info.m_mask->m_numFields,
                        ( void * ) info.m_mask->m_data );
    }

    // meta data string
    if ( strName.compare( len-5, 5, g_strMeta ) == 0 )
    {
        ReadString( iParent, strName, info.m_meta );
    }

}

//-*****************************************************************************
hobj_ref_t HDF5Hierarchy::getChildRef( hobj_ref_t iParentRef,
                                       const std::string& iName )
{
    ChildInfo info( iName );
    ChildInfoArray& children = m_objectMap[iParentRef].m_children;
    ChildInfoArray::iterator it = std::lower_bound( children.begin(),
                                                    children.end(),
                                                    info );

    ABCA_ASSERT ( ( it != children.end() && !( info < *it ) ),
                  "A child object does not exist with name: " << iName );

    return it->m_ref;
}

//-*****************************************************************************
bool HDF5Hierarchy::childExists( hobj_ref_t iParentRef,
                                 const std::string& iName )
{
    ChildInfo info( iName );
    ChildInfoArray& children = m_objectMap[iParentRef].m_children;
    ChildInfoArray::iterator it = std::lower_bound( children.begin(),
                                                    children.end(),
                                                    info );

    return ( it != children.end() && !( info < *it ) );
}

//-*****************************************************************************
bool HDF5Hierarchy::attrExists( hobj_ref_t iParentRef,
                                const std::string& iName )
{
    AttrInfo info( iName );
    AttrInfoArray& attrs = m_objectMap[iParentRef].m_attrs;
    AttrInfoArray::iterator it = std::lower_bound( attrs.begin(),
                                                   attrs.end(),
                                                   info );

    return ( it != attrs.end() && !( info < *it ) );
}

//-*****************************************************************************
void HDF5Hierarchy::readMetaDataString( hobj_ref_t iParentRef,
                                        const std::string &iMetaDataName,
                                        std::string &oMetaDataString )
{
    AttrInfo info( iMetaDataName );
    AttrInfoArray& attrs = m_objectMap[iParentRef].m_attrs;
    AttrInfoArray::iterator it = std::lower_bound( attrs.begin(),
                                                   attrs.end(),
                                                   info );
    if (it == attrs.end() || info < *it )
        return;

    oMetaDataString = it->m_meta;
}

//-*****************************************************************************
void HDF5Hierarchy::readMaskInfo( hobj_ref_t iParentRef,
                                  const std::string &iPropName,
                                  size_t& oNumFields,
                                  void* oData )
{
    AttrInfo info( iPropName );
    AttrInfoArray& attrs = m_objectMap[iParentRef].m_attrs;
    AttrInfoArray::iterator it = std::lower_bound( attrs.begin(),
                                                   attrs.end(),
                                                   info );

    ABCA_ASSERT ( it != attrs.end() && !( info < *it),
                  "Can't find a property header info attribute." );

    if ( !it->m_mask )
    {
        return;
    }

    MaskInfo& p = *it->m_mask;
    oNumFields = p.m_numFields;
    memcpy( oData, p.m_data, sizeof( uint32_t ) * oNumFields );
}

//-*****************************************************************************
void HDF5Hierarchy::makeCompactObjectHierarchy(
    std::vector<hobj_ref_t>     &oObjectRefs,

    std::vector<uint32_t>       &oChildrenSizes,
    std::vector<std::string>    &oChildrenNames,
    std::vector<hobj_ref_t>     &oChildrenRefs,

    std::vector<uint32_t>       &oAttrSizes,
    std::vector<std::string>    &oAttrNames,
    std::vector<char>           &oHasMask,
    std::vector<uint32_t>       &oMaskBits,
    std::vector<char>           &oHasMeta,
    std::vector<std::string>    &oMetaStrs
    )
{
    oObjectRefs.clear();

    oChildrenSizes.clear();
    oChildrenNames.clear();
    oChildrenRefs.clear();

    oAttrSizes.clear();
    oAttrNames.clear();
    oHasMask.clear();
    oMaskBits.clear();
    oHasMeta.clear();
    oMetaStrs.clear();

    for( ObjectMap::iterator itp = m_objectMap.begin();
         itp != m_objectMap.end(); ++itp )
    {
        ObjectInfo& info = itp->second;

        {
            oObjectRefs.push_back( itp->first );
        }

        // children
        {
            ChildInfoArray& array = info.m_children;

            oChildrenSizes.push_back( array.size() );

            for( ChildInfoArray::iterator itc = array.begin();
                 itc != array.end(); ++itc )
            {
                oChildrenNames.push_back( itc->m_name );
                oChildrenRefs.push_back( itc->m_ref );
            }
        }

        // attributes, property header masks and meta data string
        {
            AttrInfoArray& array = info.m_attrs;

            oAttrSizes.push_back( array.size() );

            for( AttrInfoArray::iterator itc = array.begin();
                 itc != array.end(); ++itc )
            {
                // attribute
                oAttrNames.push_back( itc->m_name );

                // mask
                if ( itc->m_mask && itc->m_mask->m_numFields > 0 )
                {
                    MaskInfo& p = *itc->m_mask;
                    oHasMask.push_back( 1 );

                    oMaskBits.push_back( p.m_numFields );
                    oMaskBits.push_back( p.m_data[0] );
                    oMaskBits.push_back( p.m_data[1] );
                    oMaskBits.push_back( p.m_data[2] );
                    oMaskBits.push_back( p.m_data[3] );
                    oMaskBits.push_back( p.m_data[4] );
                }
                else
                {
                    oHasMask.push_back( 0 );
                }

                // meta data
                if ( itc->m_meta.size() > 0 )
                {
                    oMetaStrs.push_back( itc->m_meta );
                    oHasMeta.push_back( 1 );
                }
                else
                {
                    oHasMeta.push_back( 0 );
                }
            }
        }
    }
}

//-*****************************************************************************
void HDF5Hierarchy::extractFromCompactObjectHierarchy(
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
    std::vector<std::string>    &iMetaStrs )
{
    clear();

    size_t idxCBegin(0), idxCEnd(0);
    size_t idxABegin(0), idxAEnd(0);
    size_t idxMask(0), idxMeta(0);

    for( size_t ip = 0; ip < iObjectRefs.size(); ++ip )
    {
        const hobj_ref_t parentRef = iObjectRefs[ip];

        ObjectInfo& object = m_objectMap[parentRef];

        // children
        idxCEnd = idxCBegin + iChildrenSizes[ip];
        for( size_t ic = idxCBegin; ic < idxCEnd; ++ic )
        {
            const hobj_ref_t ref    = iChildrenRefs[ic];
            const std::string &name = iChildrenNames[ic];

            object.m_children.push_back( ChildInfo( name, ref) );
        }
        idxCBegin = idxCEnd;

        // attributes, masks and metadata
        idxAEnd = idxABegin + iAttrSizes[ip];
        for( size_t ic = idxABegin; ic < idxAEnd; ++ic )
        {
            AttrInfoArray& attrs = object.m_attrs;

            // attribute
            const std::string &name = iAttrNames[ic];
            attrs.push_back( AttrInfo( name ) );
            AttrInfo& info = attrs[attrs.size()- 1];

            // masks
            if ( iHasMask[ic] )
            {
                const size_t length  =  iMaskBits[idxMask*6];
                const uint32_t* data = &iMaskBits[idxMask*6+1];

                info.m_mask = new MaskInfo;
                info.m_mask->m_numFields = length;
                memcpy( info.m_mask->m_data,
                        data, sizeof(uint32_t) * length );

                idxMask++;
            }

            // metadata
            if (iHasMeta[ic] )
            {
                info.m_meta = iMetaStrs[idxMeta++];
            }
        }
        idxABegin = idxAEnd;
    }
}

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreHDF5
} // End namespace Alembic

