//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

    // ".prop" is special
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
                hid_t iParentGroup,
                int32_t iArchiveVersion )
    : m_group( -1 )
{
    ABCA_ASSERT( iHeader, "Invalid header" );
    ABCA_ASSERT( iParentGroup > -1, "Invalid group" );

    m_group = H5Gopen2( iParentGroup, iHeader->getName().c_str(), H5P_DEFAULT );
    ABCA_ASSERT( m_group >= 0,
        "Could not open object group: "
        << iHeader->getFullName() );

    std::vector<std::string> objNames;

    // the old way
    if ( iArchiveVersion < -7 )
    {
        herr_t status = H5Literate( m_group,
                                    H5_INDEX_CRT_ORDER,
                                    H5_ITER_INC,
                                    NULL,
                                    VisitAllLinksCB,
                                    ( void * )&objNames );

        ABCA_ASSERT( status >= 0,
                     "OrData::OrData: H5Literate failed" );
    }
    else if ( H5Aexists( m_group, ".obj_names" ) )
    {
        ReadStrings( m_group, ".obj_names", objNames );
    }

    std::vector < std::string >::iterator namesIt;
    uint32_t i = 0;
    m_children.resize(objNames.size());
    for ( namesIt = objNames.begin(); namesIt != objNames.end();
          ++namesIt, ++i )
    {
        m_childrenMap[ *namesIt ] = i;
        m_children[i].header.reset( new AbcA::ObjectHeader( *namesIt,
            iHeader->getFullName() + "/" + *namesIt, AbcA::MetaData() ) );
        m_children[i].loadedMetaData = false;
    }


    m_data.reset( new CprData( m_group, iArchiveVersion, "" ) );
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr
OrData::getProperties( AbcA::ObjectReaderPtr iParent )
{
    AbcA::CompoundPropertyReaderPtr ret = m_top.lock();
    if ( ! ret )
    {
        // time to make a new one
        ret.reset( new CprImpl( iParent, m_data, m_group ) );
        m_top = ret;
    }

    return ret;
}

//-*****************************************************************************
size_t OrData::getNumChildren()
{
    return m_children.size();
}

//-*****************************************************************************
const AbcA::ObjectHeader &
OrData::getChildHeader( AbcA::ObjectReaderPtr iParent, size_t i )
{
    ABCA_ASSERT( i < m_children.size(),
        "Out of range index in OrData::getChildHeader: " << i );

    boost::mutex::scoped_lock l( m_childObjectsMutex );
    if ( ! m_children[i].loadedMetaData )
    {
        hid_t group = H5Gopen2( m_group,
            m_children[i].header->getName().c_str(), H5P_DEFAULT );

        ABCA_ASSERT( group >= 0,
        "Could not open object group: "
        << m_children[i].header->getFullName() );

        // Read the property info and meta data.
        // Meta data and property info is shared with the underlying
        // property
        bool dummyBool = false;
        uint32_t dummyVal;
        AbcA::PropertyHeader propHeader;
        ReadPropertyHeader( group, "", propHeader, dummyBool,
                            dummyVal, dummyVal, dummyVal, dummyVal );
        m_children[i].header->getMetaData() = propHeader.getMetaData();
        m_children[i].loadedMetaData = true;
        H5Gclose( group );
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
    ABCA_ASSERT( i < m_children.size(),
        "Out of range index in OrData::getChild: " << i );

    AbcA::ObjectReaderPtr optr = m_children[i].made.lock();
    if ( ! optr )
    {
        // we haven't fully loaded the meta data
        if ( ! m_children[i].loadedMetaData )
        {
            getChildHeader( iParent, i );
        }

        // Make a new one.
        optr.reset ( new OrImpl( iParent, m_group, m_children[i].header ) );
        m_children[i].made = optr;
    }
    return optr;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
