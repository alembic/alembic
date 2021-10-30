//-*****************************************************************************
//
// Copyright (c) 2009-2015,
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

#ifndef Alembic_AbcCollections_ICollections_h
#define Alembic_AbcCollections_ICollections_h

#include <Alembic/Abc/All.h>
#include <Alembic/Util/Export.h>
#include <Alembic/AbcCollection/SchemaInfoDeclarations.h>

namespace Alembic {
namespace AbcCollection {
namespace ALEMBIC_VERSION_NS {

class ALEMBIC_EXPORT ICollectionsSchema
    : public Abc::ISchema<CollectionsSchemaInfo>
{
public:

    typedef ICollectionsSchema this_type;

    // default constructor creates an empty ICollectionSchema
    ICollectionsSchema() {}

    //! This constructor creates a new collections reader.
    //! The first argument is the parent ICompoundProperty, from which the
    //! error handler policy for is derived.  The second argument is the name
    //! of the ICompoundProperty that contains this schemas properties.  The
    //! remaining optional arguments can be used to override the
    //! ErrorHandlerPolicy and to specify schema interpretation matching.
    ICollectionsSchema( const ICompoundProperty &iParent,
                        const std::string &iName,
                        const Abc::Argument &iArg0 = Abc::Argument(),
                        const Abc::Argument &iArg1 = Abc::Argument())
    : Abc::ISchema<CollectionsSchemaInfo>( iParent, iName, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! This constructor wraps an existing ICompoundProperty as the collection
    //! reader, and the error handler policy is derived from it.
    //! The  remaining optional arguments can be used to override the
    //! ErrorHandlerPolicy and to specify schema interpretation matching.
    ICollectionsSchema( const ICompoundProperty &iProp,
                        const Abc::Argument &iArg0 = Abc::Argument(),
                        const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<CollectionsSchemaInfo>( iProp, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Returns the number of collections that we have
    size_t getNumCollections() { return m_collections.size(); }

    //! Return a collection by index
    Abc::IStringArrayProperty getCollection( size_t i );

    //! Return a collection by name
    Abc::IStringArrayProperty getCollection( const std::string & iName );

    //! Returns the name of a collection at a given index
    std::string getCollectionName( size_t i );

    //! Returns whether this function set is valid.
    bool valid() const
    {
        return ( Abc::ISchema<CollectionsSchemaInfo>::valid() );
    }

protected:
    void init( const Abc::Argument &iArg0,
               const Abc::Argument &iArg1 );

    std::vector< Abc::IStringArrayProperty > m_collections;

};

//! Object declaration
typedef Abc::ISchemaObject<ICollectionsSchema> ICollections;

typedef Util::shared_ptr< ICollections > ICollectionsPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCollection
} // End namespace Alembic

#endif
