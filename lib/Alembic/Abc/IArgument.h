//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#ifndef _Alembic_Abc_IArgument_h_
#define _Alembic_Abc_IArgument_h_

#include <Alembic/Abc/Foundation.h>
#include <Alembic/Abc/ErrorHandler.h>

namespace Alembic {
namespace Abc {

//-*****************************************************************************
// CJH: I'm not terribly fond of the boost::variant class, and I particularly
// dislike that I'm copying MetaData by value. However, at the moment, it is
// a welcome shortcut. (I'm tired). I'll fix this soon, but it doesn't
// affect the public API at all.
class IArguments : public boost::static_visitor<>
{
public:
    IArguments( ErrorHandler::Policy iPolicy = ErrorHandler::kQuietNoopPolicy,
                SchemaInterpMatching iMatch = kNoMatching )
      : m_errorHandlerPolicy( iPolicy )
      , m_matching( iMatch )
      , m_cachePtr()
      , m_cacheWasSet( false ) {}

    void operator()( const int & ) {}
    void operator()( const ErrorHandler::Policy &iPolicy )
    { m_errorHandlerPolicy = iPolicy; }
    void operator()( const SchemaInterpMatching &iMatch )
    { m_matching = iMatch; }
    void operator()( const AbcA::ReadArraySampleCachePtr &iCachePtr )
    {
        m_cachePtr = iCachePtr;
        m_cacheWasSet = true;
    }

    ErrorHandler::Policy getErrorHandlerPolicy() const
    { return m_errorHandlerPolicy; }

    SchemaInterpMatching getSchemaInterpMatching() const
    { return m_matching; }

    AbcA::ReadArraySampleCachePtr getReadArraySampleCachePtr() const
    { return m_cachePtr; }

    bool getCacheWasSet() const
    { return m_cacheWasSet; }

private:
    ErrorHandler::Policy m_errorHandlerPolicy;
    SchemaInterpMatching m_matching;
    AbcA::ReadArraySampleCachePtr m_cachePtr;
    bool m_cacheWasSet;
};

//-*****************************************************************************
// Right now there are 4 types of arguments that you'd pass into
// our various O classes for construction.
// ErrorHandlerPolicy - always defaults to QuietNoop
// MetaData - always defaults to ""
// TimeSamplingType - always defaults to Static
class IArgument
{
public:
    IArgument() : m_variant( ( int )0 ) {}
    IArgument( ErrorHandler::Policy iPolicy ) : m_variant( iPolicy ) {}
    IArgument( SchemaInterpMatching iMatch ) : m_variant( iMatch ) {}
    IArgument( AbcA::ReadArraySampleCachePtr iCache ) : m_variant( iCache ) {}

    void setInto( IArguments &iArgs ) const
    {
        boost::apply_visitor( iArgs, m_variant );
    }

private:
    typedef boost::variant<int,
                           ErrorHandler::Policy,
                           SchemaInterpMatching,
                           AbcA::ReadArraySampleCachePtr> ArgVariant;

    ArgVariant m_variant;
};


//-*****************************************************************************
//! This is for when you need to get the error handler policy out inside
//! a constructor header.
template <class SOMETHING>
inline ErrorHandler::Policy GetErrorHandlerPolicy
( SOMETHING iSomething,
  const IArgument &iArg0,
  const IArgument &iArg1 = IArgument(),
  const IArgument &iArg2 = IArgument() )
{
    IArguments args( GetErrorHandlerPolicy( iSomething ) );
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    return args.getErrorHandlerPolicy();
}

//-*****************************************************************************
inline SchemaInterpMatching GetSchemaInterpMatching
( const IArgument &iArg0,
  const IArgument &iArg1 = IArgument(),
  const IArgument &iArg2 = IArgument() )
{
    IArguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );
    return args.getSchemaInterpMatching();
}

} // End namespace Abc
} // End namespace Alembic


#endif
