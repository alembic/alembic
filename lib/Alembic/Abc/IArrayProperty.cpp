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

#include <Alembic/Abc/IArrayProperty.h>

namespace Alembic {
namespace Abc {

//-*****************************************************************************
IArrayProperty::~IArrayProperty()
{
    // Nothing for now.
    // Mostly here in case we need to add reference-counting debug code.
}

//-*****************************************************************************
size_t IArrayProperty::getNumSamples()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IArrayProperty::getNumSamples()" );

    return m_property->getNumSamples();

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, so return a default.
    return 0;
}

//-*****************************************************************************
bool IArrayProperty::isConstant()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IArrayProperty::isConstant()" );

    return m_property->isConstant();

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, so return a default.
    return true;
}

//-*****************************************************************************
AbcA::TimeSampling IArrayProperty::getTimeSampling()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IArrayProperty::getTimeSampling()" );

    return m_property->getTimeSampling();

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, so return a default.
    return AbcA::TimeSampling();
}

//-*****************************************************************************
void IArrayProperty::get( AbcA::ArraySamplePtr& oSamp,
                          const ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IArrayProperty::get()" );

    m_property->getSample( iSS.getIndex( m_property->getTimeSampling() ),
                           oSamp );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
bool IArrayProperty::getKey( AbcA::ArraySampleKey& oKey,
                          const ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IArrayProperty::getKey()" );

    return m_property->getKey( iSS.getIndex( m_property->getTimeSampling() ),
                           oKey );

    ALEMBIC_ABC_SAFE_CALL_END();

    // for error handler that don't throw
    return false;
}

//-*****************************************************************************
ICompoundProperty IArrayProperty::getParent()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IArrayProperty::getParent()" );

    return ICompoundProperty( m_property->getParent(),
                              kWrapExisting,
                              getErrorHandlerPolicy() );

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw. Have a default.
    return ICompoundProperty();
}

//-*****************************************************************************
void IArrayProperty::init( AbcA::CompoundPropertyReaderPtr iParent,
                           const std::string &iName,

                           ErrorHandler::Policy iParentPolicy,
                           const IArgument &iArg0,
                           const IArgument &iArg1 )
{
    IArguments args( iParentPolicy );
    iArg0.setInto( args );
    iArg1.setInto( args );

    getErrorHandler().setPolicy( args.getErrorHandlerPolicy() );

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IArrayProperty::init()" );

    const AbcA::PropertyHeader *pheader =
        iParent->getPropertyHeader( iName );

    ABCA_ASSERT( pheader != NULL,
                 "Nonexistent array property: " << iName );

    m_property = iParent->getArrayProperty( iName );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace Abc
} // End namespace Alembic
