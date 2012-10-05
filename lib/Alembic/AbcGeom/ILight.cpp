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

#include <Alembic/AbcGeom/ILight.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
void ILightSchema::init( const Abc::Argument &iArg0,
                         const Abc::Argument &iArg1 )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ILightSchema::init()" );

    Abc::Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );

    AbcA::CompoundPropertyReaderPtr _this = this->getPtr();

    // none of these things are guaranteed to exist

    if ( this->getPropertyHeader( ".camera" ) != NULL )
    {
        m_cameraSchema = ICameraSchema( _this, ".camera", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader( ".childBnds" ) != NULL )
    {
        m_childBoundsProperty = Abc::IBox3dProperty( _this, ".childBnds",
            iArg0, iArg1 );
    }

    if ( this->getPropertyHeader( ".arbGeomParams" ) != NULL )
    {
        m_arbGeomParams = Abc::ICompoundProperty( _this, ".arbGeomParams",
            args.getErrorHandlerPolicy() );
    }

    if ( this->getPropertyHeader( ".userProperties" ) != NULL )
    {
        m_userProperties = Abc::ICompoundProperty( _this, ".userProperties",
            args.getErrorHandlerPolicy() );
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

AbcA::TimeSamplingPtr ILightSchema::getTimeSampling() const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ILightSchema::getTimeSampling" );

    if ( m_childBoundsProperty )
    {
        return m_childBoundsProperty.getTimeSampling();
    }
    else if ( m_cameraSchema )
    {
        return m_cameraSchema.getTimeSampling();
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return AbcA::TimeSamplingPtr();
}

bool ILightSchema::isConstant() const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ILightSchema::isConstant" );

    return getNumSamples() < 2;

    ALEMBIC_ABC_SAFE_CALL_END();

    return true;
}

size_t ILightSchema::getNumSamples() const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ILightSchema::getNumSamples" );

    if ( m_childBoundsProperty )
    {
        return m_childBoundsProperty.getNumSamples();
    }
    else if ( m_cameraSchema )
    {
        return m_cameraSchema.getNumSamples();
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return 0;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcGeom
} // End namespace Alembic
