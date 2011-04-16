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

#include <Alembic/AbcGeom/OCamera.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void OCameraSchema::set( const CameraSample &iSamp )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCameraSchema::set()" );

    // do we need to create child bounds?
    if ( iSamp.getChildBounds().hasVolume() && !m_childBounds)
    {
        m_childBounds = Abc::OBox3dProperty( this->getPtr(), ".childBnds",
            m_coreProperties.getTimeSampling() );
        Abc::Box3d emptyBox;
        emptyBox.makeEmpty();

        // -1 because we just dis an m_positions set above
        size_t numSamples = m_coreProperties.getNumSamples() - 1;

        // set all the missing samples
        for ( size_t i = 0; i < numSamples; ++i )
        {
            m_childBounds.set( emptyBox );
        }
    }

    double sampleData[16];
    sampleData[0] = m_focalLength;
    sampleData[1] = m_horizontalAperture;
    sampleData[2] = m_horizontalFilmOffset;
    sampleData[3] = m_verticleAperture;
    sampleData[4] = m_verticalFilmOffset;
    sampleData[5] = m_lensSqueezeRatio;

    sampleData[6] = m_overscanLeft;
    sampleData[7] = m_overscanRight;
    sampleData[8] = m_overscanTop;
    sampleData[9] = m_overscanBottom;

    sampleData[10] = m_fStop;
    sampleData[11] = m_focusDistance;
    sampleData[12] = m_shutterOpen;
    sampleData[13] = m_shutterClose;

    sampleData[14] = m_nearClippingPlane;
    sampleData[15] = m_farClippingPlane;

    if ( m_coreProperties.getNumSamples() == 0 )
    {
        for (size_t i = 0; i < 16; ++i)
        {
            m_initialSample[i] = sampleData[i];
        }

        if (m_childBounds)
        {
            m_childBounds.set( iSamp.getChildBounds() );
        }
    }
    else
    {
        if ( m_childBounds )
        {
            SetPropUsePrevIfNull( m_childBounds, iSamp.getChildBounds() );
        }
    }

    m_coreProperties.set( sampleData );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OCameraSchema::setFromPrevious()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCameraSchema::setFromPrevious" );

    m_coreProperties.setFromPrevious();

    if (m_childBounds)
        m_childBounds.setFromPrevious();

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OCameraSchema::setTimeSampling( uint32_t iIndex )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OCameraSchema::setTimeSampling( uint32_t )" );

    m_coreProperties.setTimeSampling( iIndex );

    if ( m_childBounds )
        m_childBounds.setTimeSampling( iIndex );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OCameraSchema::setTimeSampling( AbcA::TimeSamplingPtr iTime )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OCameraSchema::setTimeSampling( TimeSamplingPtr )" );

    if (iTime)
    {
        uint32_t tsIndex = getObject().getArchive().addTimeSampling(*iTime);
        setTimeSampling( tsIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OCameraSchema::init( uint32_t iTsIdx )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCameraSchema::init()" );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    // 14 double values
    AbcA::DataType dType( Util::kFloat64POD, 14 );
    m_coreProperties = Abc::OScalarProperty( _this, ".core", dType, iTsIdx );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
Abc::OCompoundProperty OCameraSchema::getArbGeomParams()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OCameraSchema::getArbGeomParams()" );

    if ( ! m_arbGeomParams )
    {
        m_arbGeomParams = Abc::OCompoundProperty( this->getPtr(),
                                                  ".arbGeomParams" );
    }

    return m_arbGeomParams;

    ALEMBIC_ABC_SAFE_CALL_END();

    Abc::OCompoundProperty ret;
    return ret;
}

} // End namespace AbcGeom
} // End namespace Alembic
