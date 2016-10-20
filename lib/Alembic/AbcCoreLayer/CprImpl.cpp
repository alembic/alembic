//-*****************************************************************************
//
// Copyright (c) 2016,
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

#include <Alembic/AbcCoreLayer/CprImpl.h>
#include <Alembic/AbcCoreLayer/OrImpl.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//-*****************************************************************************
// CLASS
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
CprImpl::CprImpl( OrImplPtr iObject, CompoundReaderPtrs & iCompounds )
    : m_object( iObject )
    , m_parent( CprImplPtr() )
    , m_index( 0 )
{
    ABCA_ASSERT( m_object, "Invalid object in CprImpl(Object)" );
    std::string empty;

    // top level compounds have the same metadata as the parent object
    m_header.reset( new AbcA::PropertyHeader( empty,
        m_object->getHeader().getMetaData() ) );

    init( iCompounds );
}

CprImpl::CprImpl( CprImplPtr iParent, size_t iIndex )
    : m_parent( iParent )
    , m_index( iIndex )
{
    ABCA_ASSERT( m_parent, "Invalid compound in CprImpl(CprImplPtr, size_t)" );
    m_object = m_parent->m_object;

    m_header = m_parent->m_propertyHeaders[m_index];

    // get our compounds for the init
    CompoundReaderPtrs  & childVec = m_parent->m_children[m_index];

    CompoundReaderPtrs cmpndVec;
    cmpndVec.reserve( childVec.size() );

    std::string name = m_header->getName();

    CompoundReaderPtrs::iterator it = childVec.begin();
    for ( ; it != childVec.end(); ++it )
    {
        cmpndVec.push_back( (*it)->getCompoundProperty( name ) );
    }
    init( cmpndVec );
}

//-*****************************************************************************
CprImpl::~CprImpl()
{
    // Nothing
}

//-*****************************************************************************
const AbcA::PropertyHeader &CprImpl::getHeader() const
{
    return *( m_header );
}

AbcA::ObjectReaderPtr CprImpl::getObject()
{
    return m_object->asObjectPtr();
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr CprImpl::getParent()
{
    return m_parent;
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr CprImpl::asCompoundPtr()
{
    return shared_from_this();
}

//-*****************************************************************************
size_t CprImpl::getNumProperties()
{
    return m_propertyHeaders.size();
}

//-*****************************************************************************
const AbcA::PropertyHeader & CprImpl::getPropertyHeader( size_t i )
{
    ABCA_ASSERT( i < m_propertyHeaders.size(),
        "Out of range index in CprImpl::getPropertyHeader: " << i );

    return *( m_propertyHeaders[i] );
}

//-*****************************************************************************
const AbcA::PropertyHeader *
CprImpl::getPropertyHeader( const std::string &iName )
{

    ChildNameMap::iterator itr = m_childNameMap.find( iName );

    if( itr !=  m_childNameMap.end() )
    {
        return m_propertyHeaders[ itr->second ].get();
    }

    return 0;
}

//-*****************************************************************************
AbcA::ScalarPropertyReaderPtr
CprImpl::getScalarProperty( const std::string &iName )
{
    ChildNameMap::iterator itr = m_childNameMap.find( iName );

    ABCA_ASSERT( ( itr != m_childNameMap.end() ),
        "There is no child scalar property with that name");

    if( itr != m_childNameMap.end() )
    {
        return m_children[ itr->second ][0]->getScalarProperty( itr->first );
    }

    return AbcA::ScalarPropertyReaderPtr();
}

//-*****************************************************************************
AbcA::ArrayPropertyReaderPtr
CprImpl::getArrayProperty( const std::string &iName )
{
    ChildNameMap::iterator itr = m_childNameMap.find( iName );

    ABCA_ASSERT( ( itr != m_childNameMap.end() ),
                    "There is no child array property with that name");

    if( itr != m_childNameMap.end() )
    {
        return m_children[ itr->second ][0]->getArrayProperty( itr->first );
    }

    return AbcA::ArrayPropertyReaderPtr();
}

//-*****************************************************************************
AbcA::CompoundPropertyReaderPtr
CprImpl::getCompoundProperty( const std::string &iName )
{
    ChildNameMap::iterator itr = m_childNameMap.find( iName );

    ABCA_ASSERT( ( itr != m_childNameMap.end() ),
                    "There is no child compound property with that name");

    if( itr != m_childNameMap.end() )
    {
        return CprImplPtr( new CprImpl( shared_from_this(), itr->second ) );
    }

    return AbcA::CompoundPropertyReaderPtr();
}

//-*****************************************************************************
void CprImpl::init( CompoundReaderPtrs & iCompounds )
{
    CompoundReaderPtrs::iterator it = iCompounds.begin();

    for ( ; it != iCompounds.end(); ++it )
    {
        for ( size_t i = 0; i < (*it)->getNumProperties(); ++i )
        {
            AbcA::PropertyHeader propHeader = (*it)->getPropertyHeader( i );

            // since pruning is more destructive, it trumps replace
            bool shouldPrune =
                ( propHeader.getMetaData().get( "prune" ) == "1" );

            bool shouldReplace =
                ( propHeader.getMetaData().get( "replace" ) == "1" );

            ChildNameMap::iterator nameIt = m_childNameMap.find(
                propHeader.getName() );

            // brand new child, add it (if not a prune) and continue
            if ( nameIt == m_childNameMap.end() )
            {
                // new prop that was marked for pruning, so skip
                if ( shouldPrune )
                {
                    continue;
                }

                size_t index = m_childNameMap.size();
                m_childNameMap[ propHeader.getName() ] = index;

                // TODO? We may not need the whole property header, we may only
                // need the name especially if Compound MetaData doesn't need to
                // be combined
                PropertyHeaderPtr propPtr(
                    new AbcA::PropertyHeader( propHeader ) );
                m_propertyHeaders.push_back( propPtr );
                m_children.resize( index + 1 );
                m_children[ index ].push_back( *it );
                continue;
            }
            // prune
            else if ( shouldPrune )
            {
                size_t index = nameIt->second;

                // prune, time to clear out existing data
                m_children.erase( m_children.begin() + index );
                m_propertyHeaders.erase( m_propertyHeaders.begin() + index );
                m_childNameMap.erase( nameIt );

                // since we removed an element, update the indices in our map
                for ( nameIt = m_childNameMap.begin();
                      nameIt != m_childNameMap.end(); ++nameIt )
                {
                    if ( nameIt->second > index )
                    {
                        nameIt->second --;
                    }
                }

            }
            // only add this onto an existing one IF its a compound and the
            // prop added previously is a compound
            else if ( propHeader.isCompound() &&
                      m_propertyHeaders[ nameIt->second ]->isCompound() )
            {
                // add parent and index to the existing child element, and then
                // update the MetaData
                size_t index = nameIt->second;

                if ( shouldReplace )
                {
                    m_children[ index ].clear();
                }

                m_children[ index ].push_back( *it );

                // TODO, are there cases where the MetaData should be combined
                // for Compound properties?
            }
        }
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreLayer
} // End namespace Alembic
