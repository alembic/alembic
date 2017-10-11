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

#include <Alembic/AbcGeom/OLight.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
OLightSchema::OLightSchema(
    AbcA::CompoundPropertyWriterPtr iParent,
    const std::string &iName,
    const Abc::Argument &iArg0,
    const Abc::Argument &iArg1,
    const Abc::Argument &iArg2,
    const Abc::Argument &iArg3 )
: Abc::OSchema<LightSchemaInfo>( iParent, iName, iArg0, iArg1, iArg2, iArg3 )
{
    init( iParent, iArg0, iArg1, iArg2, iArg3 );
}

//-*****************************************************************************
OLightSchema::OLightSchema( Abc::OCompoundProperty iParent,
                            const std::string &iName,
                            const Abc::Argument &iArg0,
                            const Abc::Argument &iArg1,
                            const Abc::Argument &iArg2 )
: Abc::OSchema<LightSchemaInfo>( iParent.getPtr(), iName,
                                    GetErrorHandlerPolicy( iParent ),
                                    iArg0, iArg1, iArg2 )
{
    init( iParent.getPtr(), iArg0, iArg1, iArg2, Abc::Argument() );
}

//-*****************************************************************************
void OLightSchema::init( AbcA::CompoundPropertyWriterPtr iParent,
                         const Abc::Argument &iArg0,
                         const Abc::Argument &iArg1,
                         const Abc::Argument &iArg2,
                         const Abc::Argument &iArg3 )
{

    AbcA::TimeSamplingPtr tsPtr = Abc::GetTimeSampling( iArg0, iArg1,
                                                        iArg2, iArg3 );

    uint32_t tsIndex = Abc::GetTimeSamplingIndex( iArg0, iArg1, iArg2, iArg3 );

    // if we specified a valid TimeSamplingPtr, use it to determine the index
    // otherwise we'll use the index, which defaults to the intrinsic 0 index
    if (tsPtr)
    {
        iParent->getObject()->getArchive()->addTimeSampling( *tsPtr );
        m_tsPtr = tsPtr;
    }
    else
    {
        m_tsPtr = iParent->getObject()->getArchive()->getTimeSampling(
            tsIndex );
    }
}

//-*****************************************************************************
void OLightSchema::setCameraSample( const CameraSample &iSamp )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OLightSchema::setCameraSample" );

    if ( ! m_cameraSchema.valid() )
    {
        m_cameraSchema = OCameraSchema( this->getPtr(), ".camera", m_tsPtr );
    }

    // clear the child bounds if they happen to be set on the camera sample
    // since that data is meaninless because we have child bounds on the
    // light schema
    CameraSample samp = iSamp;
    Abc::Box3d childBounds;
    samp.setChildBounds(childBounds);

    m_cameraSchema.set( samp );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OLightSchema::setFromPrevious()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OLightSchema::setFromPrevious" );

    if ( m_cameraSchema )
        m_cameraSchema.setFromPrevious();

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OLightSchema::setTimeSampling( uint32_t iIndex )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OLightSchema::setTimeSampling( uint32_t )" );

    if ( m_cameraSchema )
        m_cameraSchema.setTimeSampling( iIndex );

    m_tsPtr = getObject().getArchive().getTimeSampling( iIndex );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OLightSchema::setTimeSampling( AbcA::TimeSamplingPtr iTime )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OLightSchema::setTimeSampling( TimeSamplingPtr )" );

    if (iTime)
    {
        uint32_t tsIndex = getObject().getArchive().addTimeSampling(*iTime);
        setTimeSampling( tsIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
size_t OLightSchema::getNumSamples()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OLightSchema::getNumSamples" );

    if ( m_childBoundsProperty )
        return m_childBoundsProperty.getNumSamples();

    ALEMBIC_ABC_SAFE_CALL_END();

    return 0;
}

//-*****************************************************************************
Abc::OCompoundProperty OLightSchema::getArbGeomParams()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OLightSchema::getArbGeomParams()" );

    if ( ! m_arbGeomParams )
    {
        m_arbGeomParams = Abc::OCompoundProperty( this->getPtr (),
                                                  ".arbGeomParams" );
    }

    return m_arbGeomParams;

    ALEMBIC_ABC_SAFE_CALL_END();

    Abc::OCompoundProperty ret;
    return ret;
}

//-*****************************************************************************
Abc::OCompoundProperty OLightSchema::getUserProperties()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OLightSchema::getUserProperties()" );

    if ( ! m_userProperties )
    {
        m_userProperties = Abc::OCompoundProperty( this->getPtr(),
                                                   ".userProperties" );
    }

    return m_userProperties;

    ALEMBIC_ABC_SAFE_CALL_END();

    Abc::OCompoundProperty ret;
    return ret;
}

//-*****************************************************************************
Abc::OBox3dProperty OLightSchema::getChildBoundsProperty()
{
    // Accessing Child Bounds Property will create it if needed
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OLightSchema::getChildBoundsProperty()" );

    if ( ! m_childBoundsProperty )
    {
        AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

        // for now, use the core properties time sampling, this
        // can and should be changed depending on how the children
        // are sampled
        m_childBoundsProperty = Abc::OBox3dProperty( _this,
            ".childBnds", m_tsPtr );

    }

    ALEMBIC_ABC_SAFE_CALL_END();
    return m_childBoundsProperty;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcGeom
} // End namespace Alembic
