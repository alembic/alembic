//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#include <Alembic/AbcCoreHDF5/OrData.h>
#include <Alembic/AbcCoreHDF5/OrImpl.h>
#include <Alembic/AbcCoreHDF5/CprData.h>
#include <Alembic/AbcCoreHDF5/CprImpl.h>
#include <Alembic/AbcCoreHDF5/ReadUtil.h>
#include <Alembic/AbcCoreHDF5/HDF5Util.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
static herr_t VisitAllLinksCB( hid_t iGroup,
                               const char *iName,
                               const H5L_info_t *iLinfo,
                               void *iOpData )
{
    std::vector<std::string> *visitor = ( std::vector<std::string> * )iOpData;

    // in the old days ".prop" was the special top compound
    std::string name = iName;
    if ( name != ".prop" )
    {
        // Create a proto object.
        visitor->push_back( name );
    }

    // Keep iterating!
    return 0;
}

//-*****************************************************************************
OrData::OrData( ObjectHeaderPtr iHeader,
                H5Node & iParentGroup,
                int32_t iArchiveVersion )
    : m_children( NULL )
{
    ABCA_ASSERT( iHeader, "Invalid header" );
    ABCA_ASSERT( iParentGroup.isValidObject(), "Invalid group" );

    m_group = OpenGroup( iParentGroup, iHeader->getName().c_str() );
    ABCA_ASSERT( m_group.isValidObject(),
        "Could not open object group: "
        << iHeader->getFullName() );

    std::vector<std::string> objNames;

    herr_t status = H5Literate( m_group.getObject(),
                                H5_INDEX_CRT_ORDER,
                                H5_ITER_INC,
                                NULL,
                                VisitAllLinksCB,
                                ( void * )&objNames );

    ABCA_ASSERT( status >= 0,
                 "OrData::OrData: H5Literate failed" );

    std::vector < std::string >::iterator namesIt;
    uint32_t i = 0;
    if ( !objNames.empty() )
    {
        m_children = new Child[ objNames.size() ];
    }

    std::string parentFullName = iHeader->getFullName();
    if ( parentFullName != "/" )
    {
        parentFullName += "/";
    }

    for ( namesIt = objNames.begin(); namesIt != objNames.end();
          ++namesIt, ++i )
    {
        m_childrenMap[ *namesIt ] = i;

        m_children[i].header.reset( new AbcA::ObjectHeader( *namesIt,
            parentFullName + *namesIt, AbcA::MetaData() ) );
        m_children[i].loadedMetaData = false;
    }

    m_oldGroup = m_group;


    m_data = Alembic::Util::shared_ptr<CprData>(
        new CprData( m_group, iArchiveVersion, ".prop" ) );
}

//-*****************************************************************************
OrData::~OrData()
{
    CloseObject( m_oldGroup );
    if ( m_children )
    {
        delete [] m_children;
    }
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr
OrData::getProperties( AbcA::ObjectReaderPtr iParent )
{
    Alembic::Util::scoped_lock l( m_childObjectsMutex );
    AbcA::CompoundPropertyReaderPtr ret = m_top.lock();
    if ( ! ret )
    {
        // time to make a new one
        ret = Alembic::Util::shared_ptr<CprImpl>(
            new CprImpl( iParent, m_data ) );

        m_top = ret;
    }

    return ret;
}

//-*****************************************************************************
size_t OrData::getNumChildren()
{
    return m_childrenMap.size();
}

//-*****************************************************************************
const AbcA::ObjectHeader &
OrData::getChildHeader( AbcA::ObjectReaderPtr iParent, size_t i )
{
    ABCA_ASSERT( i < m_childrenMap.size(),
        "Out of range index in OrData::getChildHeader: " << i );

    Alembic::Util::scoped_lock l( m_childObjectsMutex );
    if ( ! m_children[i].loadedMetaData )
    {
        H5Node group = OpenGroup( m_group,
            m_children[i].header->getName().c_str() );
;
        ABCA_ASSERT( group.isValidObject(),
        "Could not open object group: "
        << m_children[i].header->getFullName() );

        ReadMetaData( group, ".prop.meta",
            m_children[i].header->getMetaData() );

        CloseObject( group );
    }

    return *( m_children[i].header );
}

//-*****************************************************************************
const AbcA::ObjectHeader *
OrData::getChildHeader( AbcA::ObjectReaderPtr iParent,
                        const std::string &iName )
{
    ChildrenMap::iterator fiter = m_childrenMap.find( iName );
    if ( fiter == m_childrenMap.end() )
    {
        return NULL;
    }

    return & getChildHeader( iParent, fiter->second );
}

//-*****************************************************************************
AbcA::ObjectReaderPtr
OrData::getChild( AbcA::ObjectReaderPtr iParent, const std::string &iName )
{
    ChildrenMap::iterator fiter = m_childrenMap.find( iName );
    if ( fiter == m_childrenMap.end() )
    {
        return AbcA::ObjectReaderPtr();
    }

    return getChild( iParent, fiter->second );
}

//-*****************************************************************************
AbcA::ObjectReaderPtr
OrData::getChild( AbcA::ObjectReaderPtr iParent, size_t i )
{
    ABCA_ASSERT( i < m_childrenMap.size(),
        "Out of range index in OrData::getChild: " << i );

    Alembic::Util::scoped_lock l( m_children[i].lock );
    AbcA::ObjectReaderPtr optr = m_children[i].made.lock();
    if ( ! optr )
    {
        // we haven't fully loaded the meta data
        if ( ! m_children[i].loadedMetaData )
        {
            getChildHeader( iParent, i );
        }

        // Make a new one.
        optr = Alembic::Util::shared_ptr<OrImpl>(
            new OrImpl( iParent, m_group, m_children[i].header ) );

        m_children[i].made = optr;
    }
    return optr;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
