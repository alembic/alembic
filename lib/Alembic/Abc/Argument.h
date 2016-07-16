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

#ifndef _Alembic_Abc_Argument_h_
#define _Alembic_Abc_Argument_h_

#include <Alembic/Abc/Foundation.h>
#include <Alembic/Abc/ErrorHandler.h>

namespace Alembic {
namespace Abc {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class Arguments
{
public:
    Arguments( ErrorHandler::Policy iPolicy = ErrorHandler::kThrowPolicy,
               const AbcA::MetaData &iMetaData = AbcA::MetaData(),
               AbcA::TimeSamplingPtr iTimeSampling =
               AbcA::TimeSamplingPtr(),
               uint32_t iTimeIndex = 0,
               SchemaInterpMatching iMatch = kNoMatching,
               SparseFlag iSparse = kFull )
      : m_errorHandlerPolicy( iPolicy ),
        m_metaData( iMetaData ),
        m_timeSampling( iTimeSampling ),
        m_timeSamplingIndex( iTimeIndex ),
        m_matching( iMatch ),
        m_sparse( iSparse ) {}

    void operator()( const uint32_t & iTimeSamplingIndex)
    { m_timeSamplingIndex = iTimeSamplingIndex; }

    void operator()( const ErrorHandler::Policy &iPolicy )
    { m_errorHandlerPolicy = iPolicy; }

    void operator()( const AbcA::MetaData &iMetaData )
    { m_metaData = iMetaData; }

    void operator()( const AbcA::TimeSamplingPtr & iTimeSampling )
    { m_timeSampling = iTimeSampling; }

    void operator()( const SchemaInterpMatching &iMatching )
    { m_matching = iMatching; }

    void operator()( const SparseFlag &iSparse )
    { m_sparse = iSparse; }

    ErrorHandler::Policy getErrorHandlerPolicy() const
    { return m_errorHandlerPolicy; }

    const AbcA::MetaData &getMetaData() const
    { return m_metaData; }

    AbcA::TimeSamplingPtr getTimeSampling() const
    { return m_timeSampling; }

    uint32_t getTimeSamplingIndex() const
    { return m_timeSamplingIndex; }

    SchemaInterpMatching getSchemaInterpMatching() const
    { return m_matching; }

    bool isSparse() const
    { return m_sparse == kSparse; }

private:
    ErrorHandler::Policy m_errorHandlerPolicy;
    AbcA::MetaData m_metaData;
    AbcA::TimeSamplingPtr m_timeSampling;
    uint32_t m_timeSamplingIndex;
    SchemaInterpMatching m_matching;
    SparseFlag m_sparse;
};

//-*****************************************************************************
// Right now there are 6 types of arguments that you'd pass into
// our various classes for construction.
// ErrorHandlerPolicy - always defaults to QuietNoop
// MetaData - always defaults to ""
// matching - schema interpretation matching
// TimeSampling - always defaults to default uniform
// TimeSamplingIndex - always defaults to 0
// Sparse - always defaults to kFull
class Argument
{
public:
    Argument() :
        m_whichVariant( kArgumentNone ) {}

    Argument( ErrorHandler::Policy iPolicy ) :
        m_whichVariant( kArgumentErrorHandlerPolicy ),
        m_variant( iPolicy ) {}

    Argument( Alembic::Util::uint32_t iTsIndex ) :
        m_whichVariant( kArgumentTimeSamplingIndex ),
        m_variant( iTsIndex ) {}

    Argument( const AbcA::MetaData &iMetaData ) :
            m_whichVariant( kArgumentMetaData ),
            m_variant( &iMetaData ) {}

    Argument( const AbcA::TimeSamplingPtr &iTsPtr ) :
        m_whichVariant( kArgumentTimeSamplingPtr ),
        m_variant( &iTsPtr ) {}

    Argument( SchemaInterpMatching iMatch ) :
        m_whichVariant( kArgumentSchemaInterpMatching ),
        m_variant( iMatch ) {}

    Argument( SparseFlag iSparse ) :
        m_whichVariant( kArgumentSparse ),
        m_variant( iSparse ) {}

    void setInto( Arguments &iArgs ) const
    {
        switch ( m_whichVariant )
        {
            case kArgumentErrorHandlerPolicy:
                iArgs( m_variant.policy );
            break;

            case kArgumentTimeSamplingIndex:
                iArgs( m_variant.timeSamplingIndex );
            break;

            case kArgumentMetaData:
                iArgs( *m_variant.metaData );
            break;

            case kArgumentTimeSamplingPtr:
                iArgs( *m_variant.timeSamplingPtr );
            break;

            case kArgumentSchemaInterpMatching:
                iArgs( m_variant.schemaInterpMatching );
            break;

            case kArgumentSparse:
                iArgs( m_variant.sparseFlag );
            break;

            // no-op
            case kArgumentNone:
            break;

            default:
                // we added something that we forgot to support in the switch
                assert(false);
            break;
        }


    }

private:

    const Argument& operator= (const Argument&);

    enum ArgumentWhichFlag
    {
        kArgumentNone,
        kArgumentErrorHandlerPolicy,
        kArgumentTimeSamplingIndex,
        kArgumentMetaData,
        kArgumentTimeSamplingPtr,
        kArgumentSchemaInterpMatching,
        kArgumentSparse
    } const m_whichVariant;

    union ArgumentVariant
    {
        ArgumentVariant() : timeSamplingIndex( 0 ) {}

        explicit ArgumentVariant( ErrorHandler::Policy iPolicy ) :
            policy( iPolicy ) {}

        explicit ArgumentVariant( Alembic::Util::uint32_t iTsIndex ) :
            timeSamplingIndex( iTsIndex ) {}

        explicit ArgumentVariant( const AbcA::MetaData * iMetaData ) :
            metaData( iMetaData ) {}

        explicit ArgumentVariant( const AbcA::TimeSamplingPtr * iTsPtr ) :
            timeSamplingPtr( iTsPtr ) {}

        explicit ArgumentVariant( SchemaInterpMatching iMatch ) :
            schemaInterpMatching( iMatch ) {}

        explicit ArgumentVariant( SparseFlag iSparse ) :
            sparseFlag( iSparse ) {}

        ErrorHandler::Policy policy;
        Alembic::Util::uint32_t timeSamplingIndex;
        const AbcA::MetaData * metaData;
        const AbcA::TimeSamplingPtr * timeSamplingPtr;
        SchemaInterpMatching schemaInterpMatching;
        SparseFlag sparseFlag;
    } const m_variant;
};


//-*****************************************************************************
//! This is for when you need to get the error handler policy out inside
//! a constructor header.
template <class SOMETHING>
inline ErrorHandler::Policy GetErrorHandlerPolicy
( SOMETHING iSomething,
  const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument(),
  const Argument &iArg3 = Argument() )
{
    Arguments args( GetErrorHandlerPolicy( iSomething ) );
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    iArg3.setInto( args );
    return args.getErrorHandlerPolicy();
}

//-*****************************************************************************
inline ErrorHandler::Policy GetErrorHandlerPolicyFromArgs
( const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument(),
  const Argument &iArg3 = Argument() )
{
    Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    iArg3.setInto( args );
    return args.getErrorHandlerPolicy();
}

//-*****************************************************************************
inline AbcA::MetaData GetMetaData
( const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument(),
  const Argument &iArg3 = Argument() )
{
    Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    iArg3.setInto( args );
    return args.getMetaData();
}

//-*****************************************************************************
inline AbcA::TimeSamplingPtr GetTimeSampling
( const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument(),
  const Argument &iArg3 = Argument() )
{
    Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    iArg3.setInto( args );
    return args.getTimeSampling();
}

//-*****************************************************************************
inline uint32_t GetTimeSamplingIndex
( const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument(),
  const Argument &iArg3 = Argument() )
{
    Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    iArg3.setInto( args );
    return args.getTimeSamplingIndex();
}

//-*****************************************************************************
inline SchemaInterpMatching GetSchemaInterpMatching
( const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument(),
  const Argument &iArg3 = Argument() )
{
    Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    iArg3.setInto( args );
    return args.getSchemaInterpMatching();
}

//-*****************************************************************************
inline bool IsSparse
( const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument(),
  const Argument &iArg3 = Argument() )
{
    Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    iArg3.setInto( args );
    return args.isSparse();
}

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Abc
} // End namespace Alembic


#endif
