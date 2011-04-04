//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

//-*****************************************************************************
// CJH: I'm not terribly fond of the boost::variant class, and I particularly
// dislike that I'm copying MetaData by value. However, at the moment, it is
// a welcome shortcut. (I'm tired). I'll fix this soon, but it doesn't
// affect the public API at all.
class Arguments : public boost::static_visitor<>
{
public:
    Arguments( ErrorHandler::Policy iPolicy = ErrorHandler::kThrowPolicy,
                const AbcA::MetaData &iMetaData = AbcA::MetaData(),
                const AbcA::TimeSamplingType &iTimeSamplingType =
                AbcA::TimeSamplingType(),
                SchemaInterpMatching iMatch = kNoMatching )
      : m_errorHandlerPolicy( iPolicy ),
        m_metaData( iMetaData ),
        m_timeSamplingType( iTimeSamplingType ),
        m_matching( iMatch ){}

    void operator()( const int & ) {}
    void operator()( const ErrorHandler::Policy &iPolicy )
    { m_errorHandlerPolicy = iPolicy; }
    void operator()( const AbcA::MetaData &iMetaData )
    { m_metaData = iMetaData; }
    void operator()( const AbcA::TimeSamplingType &iTimeSamplingType )
    { m_timeSamplingType = iTimeSamplingType; }
    void operator()( const SchemaInterpMatching &iMatching )
    { m_matching = iMatching; }
    ErrorHandler::Policy getErrorHandlerPolicy() const
    { return m_errorHandlerPolicy; }
    const AbcA::MetaData &getMetaData() const
    { return m_metaData; }
    const AbcA::TimeSamplingType &getTimeSamplingType() const
    { return m_timeSamplingType; }

    SchemaInterpMatching getSchemaInterpMatching() const
    { return m_matching; }
    
private:
    ErrorHandler::Policy m_errorHandlerPolicy;
    AbcA::MetaData m_metaData;
    AbcA::TimeSamplingType m_timeSamplingType;
    SchemaInterpMatching m_matching;
};

//-*****************************************************************************
// Right now there are 4 types of arguments that you'd pass into
// our various classes for construction.
// ErrorHandlerPolicy - always defaults to QuietNoop
// MetaData - always defaults to ""
// TimeSamplingType - always defaults to Static
class Argument
{
public:
    Argument() : m_variant( ( int )0 ) {}
    Argument( ErrorHandler::Policy iPolicy ) : m_variant( iPolicy ) {}
    Argument( const AbcA::MetaData &iMetaData ) : m_variant( iMetaData ) {}
    Argument( const AbcA::TimeSamplingType &iTst ) : m_variant( iTst ) {}
    Argument( SchemaInterpMatching iMatch ) : m_variant( iMatch ) {}

    void setInto( Arguments &iArgs ) const
    {
        boost::apply_visitor( iArgs, m_variant );
    }

private:
    typedef boost::variant<int,
                           ErrorHandler::Policy,
                           AbcA::TimeSamplingType,
                           AbcA::MetaData,
                           SchemaInterpMatching> ArgVariant;

    ArgVariant m_variant;
};


//-*****************************************************************************
//! This is for when you need to get the error handler policy out inside
//! a constructor header.
template <class SOMETHING>
inline ErrorHandler::Policy GetErrorHandlerPolicy
( SOMETHING iSomething,
  const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument() )
{
    Arguments args( GetErrorHandlerPolicy( iSomething ) );
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    return args.getErrorHandlerPolicy();
}

inline ErrorHandler::Policy GetErrorHandlerPolicy
( const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument() )
{
    Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    return args.getErrorHandlerPolicy();
}

//-*****************************************************************************
inline AbcA::MetaData GetMetaData
( const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument() )
{
    Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    return args.getMetaData();
}

//-*****************************************************************************
inline AbcA::TimeSamplingType GetTimeSamplingType
( const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument() )
{
    Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    return args.getTimeSamplingType();
}

//-*****************************************************************************
inline SchemaInterpMatching GetSchemaInterpMatching
( const Argument &iArg0,
  const Argument &iArg1 = Argument(),
  const Argument &iArg2 = Argument() )
{
    Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    return args.getSchemaInterpMatching();
}

} // End namespace Abc
} // End namespace Alembic


#endif
