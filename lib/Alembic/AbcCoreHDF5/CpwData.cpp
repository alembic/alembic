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

#include <Alembic/AbcCoreHDF5/CpwData.h>
#include <Alembic/AbcCoreHDF5/WriteUtil.h>
#include <Alembic/AbcCoreHDF5/SpwImpl.h>
#include <Alembic/AbcCoreHDF5/ApwImpl.h>
#include <Alembic/AbcCoreHDF5/CpwImpl.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
CpwData::CpwData( const std::string & iName, hid_t iParentGroup )
    : m_parentGroup( iParentGroup )
    , m_group( -1 )
    , m_name( iName )
{
    // the "top" compound property has no name
    if ( m_name == "" )
    {
        m_group = m_parentGroup;
    }
}

//-*****************************************************************************
CpwData::~CpwData()
{
    if ( m_group >= 0 )
    {
        H5Gclose( m_group );
        m_group = -1;
    }
}

//-*****************************************************************************
size_t CpwData::getNumProperties()
{
    return m_propertyHeaders.size();
}

//-*****************************************************************************
const AbcA::PropertyHeader &
CpwData::getPropertyHeader( size_t i )
{
    if ( i > m_propertyHeaders.size() )
    {
        ABCA_THROW( "Out of range index in " <<
                    "CpwImpl::getPropertyHeader: " << i );
    }

    PropertyHeaderPtr ptr = m_propertyHeaders[i];
    ABCA_ASSERT( ptr, "Invalid property header ptr in CpwImpl" );

    return *ptr;
}

//-*****************************************************************************
const AbcA::PropertyHeader *
CpwData::getPropertyHeader( const std::string &iName )
{
    for ( PropertyHeaderPtrs::iterator piter = m_propertyHeaders.begin();
          piter != m_propertyHeaders.end(); ++piter )
    {
        if ( (*piter)->getName() == iName )
        {
            return (*piter).get();
        }
    }
    return NULL;
}

//-*****************************************************************************
AbcA::BasePropertyWriterPtr
CpwData::getProperty( const std::string &iName )
{
    MadeProperties::iterator fiter = m_madeProperties.find( iName );
    if ( fiter == m_madeProperties.end() )
    {
        return AbcA::BasePropertyWriterPtr();
    }

    WeakBpwPtr wptr = (*fiter).second;

    return wptr.lock();
}

//-*****************************************************************************
hid_t CpwData::getGroup()
{
    // If we've already made it (or set it), return it!
    if ( m_group >= 0 )
    {
        return m_group;
    }

    ABCA_ASSERT( m_parentGroup >= 0, "invalid parent group" );

    // Create the HDF5 group corresponding to this property.
    if ( m_name != "" )
    {
        hid_t copl = CreationOrderPlist();
        m_group = H5Gcreate2( m_parentGroup, m_name.c_str(),
                              H5P_DEFAULT, copl, H5P_DEFAULT );
        H5Pclose( copl );
    }
    else
    {
        m_group = m_parentGroup;
    }

    ABCA_ASSERT( m_group >= 0,
                 "Could not create compound property group named: "
                 << m_name );

    return m_group;
}

//-*****************************************************************************
AbcA::ScalarPropertyWriterPtr
CpwData::createScalarProperty( AbcA::CompoundPropertyWriterPtr iParent,
                               const std::string & iName,
                               const AbcA::MetaData & iMetaData,
                               const AbcA::DataType & iDataType,
                               uint32_t iTimeSamplingIndex )
{
    if ( m_madeProperties.count( iName ) )
    {
        ABCA_THROW( "Already have a property named: " << iName );
    }

    hid_t myGroup = getGroup();

    Alembic::Util::shared_ptr<SpwImpl>
        ret( new SpwImpl( iParent, myGroup, iName, iMetaData,
            iDataType, iTimeSamplingIndex ) );

    PropertyHeaderPtr headerPtr( new AbcA::PropertyHeader( ret->getHeader() ) );
    m_propertyHeaders.push_back( headerPtr );
    m_madeProperties[iName] = WeakBpwPtr( ret );

    return ret;
}

//-*****************************************************************************
AbcA::ArrayPropertyWriterPtr
CpwData::createArrayProperty( AbcA::CompoundPropertyWriterPtr iParent,
                              const std::string & iName,
                              const AbcA::MetaData & iMetaData,
                              const AbcA::DataType & iDataType,
                              uint32_t iTimeSamplingIndex )
{
    if ( m_madeProperties.count( iName ) )
    {
        ABCA_THROW( "Already have a property named: " << iName );
    }

    hid_t myGroup = getGroup();

    Alembic::Util::shared_ptr<ApwImpl>
        ret( new ApwImpl( iParent, myGroup, iName, iMetaData,
            iDataType, iTimeSamplingIndex ) );

    PropertyHeaderPtr headerPtr( new AbcA::PropertyHeader( ret->getHeader() ) );
    m_propertyHeaders.push_back( headerPtr );
    m_madeProperties[iName] = WeakBpwPtr( ret );

    return ret;
}

//-*****************************************************************************
AbcA::CompoundPropertyWriterPtr
CpwData::createCompoundProperty( AbcA::CompoundPropertyWriterPtr iParent,
                                 const std::string & iName,
                                 const AbcA::MetaData & iMetaData )
{
    if ( m_madeProperties.count( iName ) )
    {
        ABCA_THROW( "Already have a property named: " << iName );
    }

    hid_t myGroup = getGroup();

    Alembic::Util::shared_ptr<CpwImpl>
        ret( new CpwImpl( iParent, myGroup, iName, iMetaData ) );

    PropertyHeaderPtr headerPtr( new AbcA::PropertyHeader( ret->getHeader() ) );
    m_propertyHeaders.push_back( headerPtr );
    m_madeProperties[iName] = WeakBpwPtr( ret );

    return ret;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
