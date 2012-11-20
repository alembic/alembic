//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <Alembic/AbcCollection/ICollections.h>

namespace Alembic {
namespace AbcCollection {
namespace ALEMBIC_VERSION_NS {

void ICollectionsSchema::init( const Abc::Argument &iArg0,
                               const Abc::Argument &iArg1 )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ICollectionsSchema::init()" );

    AbcCoreAbstract::CompoundPropertyReaderPtr _this = this->getPtr();
    m_collections.clear();

    size_t numProps = this->getNumProperties();
    for ( size_t i = 0; i < numProps; ++i )
    {
        AbcCoreAbstract::PropertyHeader header = this->getPropertyHeader( i );
        if ( Abc::IStringArrayProperty::matches( header ) )
        {
            m_collections.push_back( Abc::IStringArrayProperty( _this,
                header.getName(), iArg0, iArg1 ) );
        }
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

Abc::IStringArrayProperty ICollectionsSchema::getCollection( size_t i )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ICollectionsSchema::getCollection(size_t)" );

    if ( i < m_collections.size() )
    {
        return m_collections[i];
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return Abc::IStringArrayProperty();
}

Abc::IStringArrayProperty
ICollectionsSchema::getCollection( const std::string & iName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ICollectionsSchema::getCollection(string)" );
    const AbcCoreAbstract::PropertyHeader * header =
        this->getPropertyHeader( iName );

    if ( header != NULL && Abc::IStringArrayProperty::matches( *header ) )
    {
        AbcCoreAbstract::CompoundPropertyReaderPtr _this = this->getPtr();
        Abc::IStringArrayProperty prop( _this, iName );

        return prop;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return Abc::IStringArrayProperty();
}

std::string ICollectionsSchema::getCollectionName( size_t i )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ICollectionsSchema::getCollectionName" );

    if ( i < m_collections.size() )
    {
        return m_collections[i].getName();
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return std::string();
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCollection
} // End namespace Alembic

