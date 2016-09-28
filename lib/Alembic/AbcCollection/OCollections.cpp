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

#include <Alembic/AbcCollection/OCollections.h>

namespace Alembic {
namespace AbcCollection {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
OCollectionsSchema::OCollectionsSchema(
    Alembic::AbcCoreAbstract::CompoundPropertyWriterPtr iParent,
    const std::string &iName,
    const Abc::Argument &iArg0,
    const Abc::Argument &iArg1,
    const Abc::Argument &iArg2,
    const Abc::Argument &iArg3 )
: Abc::OSchema<CollectionsSchemaInfo>( iParent, iName, iArg0, iArg1,
                                       iArg2, iArg3 )
{
}

//-*****************************************************************************
OCollectionsSchema::OCollectionsSchema( Abc::OCompoundProperty iParent,
                                        const std::string &iName,
                                        const Abc::Argument &iArg0,
                                        const Abc::Argument &iArg1,
                                        const Abc::Argument &iArg2)
: Abc::OSchema<CollectionsSchemaInfo>( iParent.getPtr(), iName,
                                       GetErrorHandlerPolicy( iParent ),
                                       iArg0, iArg1, iArg2 )
{
}

//-*****************************************************************************
Abc::OStringArrayProperty
OCollectionsSchema::createCollection( const std::string &iName,
    const Abc::Argument &iArg0,
    const Abc::Argument &iArg1,
    const Abc::Argument &iArg2 )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCollectionsSchema::createCollection" );

    Abc::OStringArrayProperty prop = getCollection( iName );

    if ( prop.valid() )
        return prop;

    prop = Abc::OStringArrayProperty( this->getPtr(), iName,
                                      iArg0, iArg1, iArg2 );

    if ( prop.valid() )
    {
        m_collections.push_back( prop );

        return prop;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return Abc::OStringArrayProperty();
}

//-*****************************************************************************
Abc::OStringArrayProperty
OCollectionsSchema::getCollection( size_t i )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCollectionsSchema::getCollection(size_t)" );

    if ( i < m_collections.size() )
    {
        return m_collections[i];
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return Abc::OStringArrayProperty();
}

//-*****************************************************************************
Abc::OStringArrayProperty
OCollectionsSchema::getCollection( const std::string & iName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCollectionsSchema::getCollection(string)" );

    const AbcCoreAbstract::PropertyHeader * header =
        this->getPropertyHeader( iName );

    if ( header != NULL && Abc::OStringArrayProperty::matches( *header ) )
    {
        Abc::OStringArrayProperty prop(
            this->getProperty( iName ).getPtr()->asArrayPtr(),
                Abc::kWrapExisting );

        return prop;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return Abc::OStringArrayProperty();
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCollection
} // End namespace Alembic

