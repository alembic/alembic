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

#include <Alembic/Abc/OArrayProperty.h>

namespace Alembic {
namespace Abc {

//-*****************************************************************************
OArrayProperty::~OArrayProperty()
{
    // Nothing for now.
    // Mostly here in case we need to add reference-counting debug code.
}

//-*****************************************************************************
size_t OArrayProperty::getNumSamples()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OArrayProperty::getNumSamples()" );

    return m_property->getNumSamples();

    ALEMBIC_ABC_SAFE_CALL_END();

    return 0;
}

//-*****************************************************************************
void OArrayProperty::set( const AbcA::ArraySample &iSamp,
                          const OSampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OArrayProperty::set()" );

    m_property->setSample( iSS.getIndex(), iSS.getTime(), iSamp );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OArrayProperty::setFromPrevious( const OSampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OArrayProperty::setFromPrevious()" );

    m_property->setFromPreviousSample( iSS.getIndex(), iSS.getTime() );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
OCompoundProperty OArrayProperty::getParent()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OArrayProperty::getParent()" );

    return OCompoundProperty( m_property->getParent(),
                              kWrapExisting,
                              getErrorHandlerPolicy() );

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw. Have a default.
    return OCompoundProperty();
}

//-*****************************************************************************
void OArrayProperty::init( AbcA::CompoundPropertyWriterPtr iParent,
                           const std::string &iName,
                           const AbcA::DataType &iDataType,

                           ErrorHandler::Policy iParentPolicy,
                           const OArgument &iArg0,
                           const OArgument &iArg1,
                           const OArgument &iArg2 )
{
    OArguments args( iParentPolicy );
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );

    getErrorHandler().setPolicy( args.getErrorHandlerPolicy() );

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OArrayProperty::init()" );

    AbcA::PropertyHeader phdr( iName, AbcA::kArrayProperty, args.getMetaData(),
                               iDataType, args.getTimeSamplingType() );
    m_property = iParent->createArrayProperty( phdr );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace Abc
} // End namespace Alembic
