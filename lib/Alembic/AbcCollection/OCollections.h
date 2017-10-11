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

#ifndef _Alembic_AbcCollection_OCollections_h_
#define _Alembic_AbcCollection_OCollections_h_

#include <Alembic/Abc/All.h>
#include <Alembic/Util/Export.h>
#include <Alembic/AbcCollection/SchemaInfoDeclarations.h>

namespace Alembic {
namespace AbcCollection {
namespace ALEMBIC_VERSION_NS {

class ALEMBIC_EXPORT OCollectionsSchema
    : public Abc::OSchema<CollectionsSchemaInfo>
{
public:

    typedef OCollectionsSchema this_type;

    OCollectionsSchema() {}

    //! This constructor creates a new collections writer.
    //! The first argument is an CompoundPropertyWriterPtr to use as a parent.
    //! The next is the name to give the schema which is usually the default
    //! name given by OCollections (.collection)   The remaining optional
    //! arguments can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, specify sparse sampling and to set TimeSampling.
    OCollectionsSchema(
        Alembic::AbcCoreAbstract::CompoundPropertyWriterPtr iParent,
        const std::string &iName,
        const Abc::Argument &iArg0 = Abc::Argument(),
        const Abc::Argument &iArg1 = Abc::Argument(),
        const Abc::Argument &iArg2 = Abc::Argument(),
        const Abc::Argument &iArg3 = Abc::Argument() );

    //! This constructor creates a new collections writer.
    //! The first argument is an OCompundProperty to use as a parent, and from
    //! which the ErrorHandlerPolicy is derived.  The next is the name to give
    //! the schema which is usually the default name given by OCollections
    //! (.collection) The remaining optional arguments can be used to specify
    //! MetaData, specify sparse sampling and to set TimeSampling.
    OCollectionsSchema( Abc::OCompoundProperty iParent,
                        const std::string &iName,
                        const Abc::Argument &iArg0 = Abc::Argument(),
                        const Abc::Argument &iArg1 = Abc::Argument(),
                        const Abc::Argument &iArg2 = Abc::Argument() );

    //! Copy constructor.
    OCollectionsSchema( const OCollectionsSchema& iCopy )
        : Abc::OSchema<CollectionsSchemaInfo>()
    {
        *this = iCopy;
    }

    //! Create a named collection, if the collection already exists
    //! return it.
    Abc::OStringArrayProperty
    createCollection( const std::string &iName,
                      const Abc::Argument &iArg0 = Abc::Argument(),
                      const Abc::Argument &iArg1 = Abc::Argument(),
                      const Abc::Argument &iArg2 = Abc::Argument() );

    //! Returns the number of collections that we have
    size_t getNumCollections() { return m_collections.size(); }

    //! Return a collection by index
    Abc::OStringArrayProperty getCollection( size_t i );

    //! Return a collection by name
    Abc::OStringArrayProperty getCollection( const std::string & iName );

    //! Returns whether this function set is valid.
    bool valid() const
    {
        return ( Abc::OSchema<CollectionsSchemaInfo>::valid() );
    }

protected:
    std::vector< Abc::OStringArrayProperty > m_collections;

};

//! Object declaration
typedef Abc::OSchemaObject<OCollectionsSchema> OCollections;

typedef Util::shared_ptr< OCollections > OCollectionsPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCollection
} // End namespace Alembic

#endif

