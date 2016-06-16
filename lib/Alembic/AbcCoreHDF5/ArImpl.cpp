//-*****************************************************************************
//
// Copyright (c) 2009-2013,
//  Sony Pictures Imageworks Inc. and
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
// Industrial Light & Magic, nor the names of their contributors may be used
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

#include <Alembic/AbcCoreHDF5/ArImpl.h>
#include <Alembic/AbcCoreHDF5/OrData.h>
#include <Alembic/AbcCoreHDF5/OrImpl.h>
#include <Alembic/AbcCoreHDF5/ReadUtil.h>
#include <Alembic/AbcCoreHDF5/HDF5Util.h>
#include <Alembic/AbcCoreHDF5/HDF5Util.h>
#include <Alembic/AbcCoreHDF5/HDF5HierarchyReader.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
ArImpl::ArImpl( const std::string &iFileName,
                AbcA::ReadArraySampleCachePtr iCache,
                const bool iCacheHierarchy )
  : m_fileName( iFileName )
  , m_file( -1 )
  , m_readArraySampleCache( iCache )
{
    // OPEN THE FILE!
    htri_t exi = H5Fis_hdf5( m_fileName.c_str() );
    ABCA_ASSERT( exi == 1, "Nonexistent or not an Alembic file: "
        << m_fileName );

    m_file = H5Fopen( m_fileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT );
    ABCA_ASSERT( m_file >= 0,
                 "Could not open file: " << m_fileName );

    // get the version using HDF5 native calls
    int version = -INT_MAX;
    if (H5Aexists(m_file, "abc_version"))
    {
        H5LTget_attribute_int(m_file, ".", "abc_version", &version);
    }
    ABCA_ASSERT(version >= -8 && version <= ALEMBIC_HDF5_FILE_VERSION,
        "Unsupported file version detected: " << version);

    // if it isn't there, it's pre 1.0
    int fileVersion = 9999;
    if (H5Aexists( m_file, "abc_release_version" ))
    {
        H5LTget_attribute_int( m_file, ".", "abc_release_version",
                               &fileVersion);
    }
    m_archiveVersion = fileVersion;

    HDF5HierarchyReader reader( m_file, m_H5H, iCacheHierarchy );
    H5Node node = m_H5H.createNode( m_file );
    H5Node abcRoot = OpenGroup( node, "ABC" );

    AbcA::MetaData metaData;
    ReadMetaData( abcRoot, ".prop.meta", metaData );
    m_header.reset( new AbcA::ObjectHeader( "ABC", "/", metaData ) );

    m_data.reset( new OrData( m_header, node, m_archiveVersion ) );
    CloseObject( abcRoot );
    ReadTimeSamples( m_file, m_timeSamples );

    if ( H5Aexists( m_file, "abc_max_samples" ) )
    {
        hid_t aid = H5Aopen( m_file, "abc_max_samples", H5P_DEFAULT );

        if ( aid < 0 )
        {
            return;
        }

        AttrCloser attrCloser( aid );

        // figure out how big it is
        hid_t sid = H5Aget_space( aid );

        if ( sid < 0 )
        {
            return;
        }

        DspaceCloser dspaceCloser( sid );

        hssize_t numPoints = H5Sget_simple_extent_npoints( sid );

        if ( numPoints < 1 )
        {
            return;
        }

        m_maxSamples.resize( numPoints );

        // do the read
        H5Aread( aid, H5T_NATIVE_LLONG, &( m_maxSamples.front() ) );

    }
}

//-*****************************************************************************
const std::string &ArImpl::getName() const
{
    return m_fileName;
}

//-*****************************************************************************
const AbcA::MetaData &ArImpl::getMetaData() const
{
    return m_header->getMetaData();
}

//-*****************************************************************************
AbcA::ObjectReaderPtr ArImpl::getTop()
{
    Alembic::Util::scoped_lock l( m_orlock );
    AbcA::ObjectReaderPtr ret = m_top.lock();
    if ( ! ret )
    {
        // time to make a new one
        ret = Alembic::Util::shared_ptr<OrImpl>(
            new OrImpl( asArchivePtr(), m_data, m_header ) );
        m_top = ret;
    }

    return ret;
}

//-*****************************************************************************
AbcA::TimeSamplingPtr ArImpl::getTimeSampling( uint32_t iIndex )
{
    ABCA_ASSERT( iIndex < m_timeSamples.size(),
        "Invalid index provided to getTimeSampling." );

    return m_timeSamples[iIndex];
}

//-*****************************************************************************
AbcA::ArchiveReaderPtr ArImpl::asArchivePtr()
{
    return shared_from_this();
}

//-*****************************************************************************
AbcA::index_t ArImpl::getMaxNumSamplesForTimeSamplingIndex( uint32_t iIndex )
{
    if ( iIndex < m_maxSamples.size() )
    {
        return m_maxSamples[iIndex];
    }

    return INDEX_UNKNOWN;
}

//-*****************************************************************************
ArImpl::~ArImpl()
{

    m_data.reset();

    if ( m_file >= 0 )
    {
        int dsetCount = H5Fget_obj_count( m_file,
            H5F_OBJ_LOCAL | H5F_OBJ_DATASET);
        int grpCount = H5Fget_obj_count( m_file,
            H5F_OBJ_LOCAL | H5F_OBJ_GROUP );
        int dtypCount = H5Fget_obj_count( m_file,
            H5F_OBJ_LOCAL | H5F_OBJ_DATATYPE );
        int attrCount = H5Fget_obj_count( m_file,
            H5F_OBJ_LOCAL | H5F_OBJ_ATTR );

        int objCount = dsetCount + grpCount + dtypCount + attrCount;

        if ( objCount != 0 )
        {
            std::stringstream strm;
            strm << "Open HDF5 handles detected during reading:" << std::endl
                 << "DataSets: " << dsetCount
                 << ", Groups: " << grpCount
                 << ", DataTypes: " << dtypCount
                 << ", Attributes: " << attrCount;

            std::vector< hid_t > objList;

            // when getting the name corresponding to a hid_t, the get_name
            // functions always append a NULL character, which we strip off
            // when injecting into out stream.
            std::string name;

            if ( dsetCount > 0 )
            {
                strm << std::endl << "DataSets: " << std::endl;
                objList.resize( dsetCount );
                H5Fget_obj_ids( m_file, H5F_OBJ_LOCAL | H5F_OBJ_DATASET,
                    dsetCount, &objList.front() );
                for ( int i = 0; i < dsetCount; ++i )
                {
                    int strLen = H5Iget_name( objList[i], NULL, 0 ) + 1;
                    name.resize( strLen );
                    H5Iget_name( objList[i], &(name[0]), strLen );
                    strm << name.substr(0, name.size() - 1) << std::endl;
                }
            }

            if ( grpCount > 0 )
            {
                strm << std::endl << std::endl << "Groups:" << std::endl;
                objList.resize( grpCount );
                H5Fget_obj_ids( m_file, H5F_OBJ_LOCAL | H5F_OBJ_GROUP,
                    grpCount, &objList.front() );
                for ( int i = 0; i < grpCount; ++i )
                {
                    int strLen = H5Iget_name( objList[i], NULL, 0 ) + 1;
                    name.resize( strLen );
                    H5Iget_name( objList[i], &(name[0]), strLen );
                    strm << std::endl << name.substr(0, name.size() - 1);
                }
            }

            if ( attrCount > 0 )
            {
                strm << std::endl << std::endl << "Attrs:" << std::endl;
                objList.resize( attrCount );
                H5Fget_obj_ids( m_file, H5F_OBJ_LOCAL | H5F_OBJ_ATTR,
                    attrCount, &objList.front() );
                for ( int i = 0; i < attrCount; ++i )
                {
                    int strLen = H5Aget_name( objList[i], 0, NULL ) + 1;
                    name.resize( strLen );
                    H5Aget_name( objList[i], strLen, &(name[0]) );
                    strm << std::endl << name.substr(0, name.size() - 1);
                }
            }

            // just for formatting purposes
            if ( dtypCount > 0 )
            {
                strm << std::endl;
            }

            m_file = -1;
            ABCA_THROW( strm.str() );
        }

        H5Fclose( m_file );
        m_file = -1;
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
