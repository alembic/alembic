//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include <Alembic/AbcGeom/ISimpleXform.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
static bool isMoreDynamic( const AbcA::TimeSampling &a,
                           const AbcA::TimeSampling &b )
{
    //! Return true if a is more dynamic than b
    if ( b.isStatic() && ( ! a.isStatic() ) ) { return true; }

    AbcA::TimeSamplingType atst = a.getTimeSamplingType();
    AbcA::TimeSamplingType btst = b.getTimeSamplingType();

    if ( atst.getNumSamplesPerCycle() > btst.getNumSamplesPerCycle() )
    { return true; }

    return false;
}

//-*****************************************************************************
void ISimpleXformSchema::_getTimeData( Abc::IDoubleProperty& iProp )
{
    bool hasNoTime = iProp ? iProp.getTimeSampling().isStatic() : true;

    if ( ! hasNoTime )
    {
        uint32_t localNumSampsPerCycle = \
            m_timeSampling.getTimeSamplingType().getNumSamplesPerCycle();
        uint32_t propNumSampsPerCycle = \
            iProp.getTimeSampling().getTimeSamplingType().getNumSamplesPerCycle();

        size_t numSamps = iProp.getNumSamples();

        // if prop samples/cycle is greater than or equal to local
        // samples/cycle, AND prop num samples is greater than local,
        // use the prop for time
        if ( ( isMoreDynamic( iProp.getTimeSampling(), m_timeSampling )
               && numSamps >= m_numSamples )
             || ( localNumSampsPerCycle == propNumSampsPerCycle
                  && numSamps > m_numSamples )
           )
        {
            m_timeSampling = iProp.getTimeSampling();
            m_numSamples = numSamps;
        }

        m_isConstant = m_isConstant ? iProp.isConstant() : m_isConstant;
    }

    // just making sure we're not erroneously saying 0 samples when there is
    // at least 1.
    if ( iProp && ( iProp.getNumSamples() > m_numSamples ) )
    {
        m_numSamples = iProp.getNumSamples();
    }
}

//-*****************************************************************************
void ISimpleXformSchema::init( Abc::SchemaInterpMatching )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISimpleXformSchema::init()" );

    static const Abc::ErrorHandler::Policy qnp =
        Abc::ErrorHandler::kQuietNoopPolicy;

    m_scaleX = Abc::IDoubleProperty( *this, ".sx", qnp );
    m_scaleY = Abc::IDoubleProperty( *this, ".sy", qnp );
    m_scaleZ = Abc::IDoubleProperty( *this, ".sz", qnp );

    m_shear0 = Abc::IDoubleProperty( *this, ".h0", qnp );
    m_shear1 = Abc::IDoubleProperty( *this, ".h1", qnp );
    m_shear2 = Abc::IDoubleProperty( *this, ".h2", qnp );

    m_rotateX = Abc::IDoubleProperty( *this, ".rx", qnp );
    m_rotateY = Abc::IDoubleProperty( *this, ".ry", qnp );
    m_rotateZ = Abc::IDoubleProperty( *this, ".rz", qnp );

    m_translateX = Abc::IDoubleProperty( *this, ".tx", qnp );
    m_translateY = Abc::IDoubleProperty( *this, ".ty", qnp );
    m_translateZ = Abc::IDoubleProperty( *this, ".tz", qnp );

    m_numSamples = 0;
    m_isConstant = true;

    _getTimeData( m_scaleX );
    _getTimeData( m_scaleY );
    _getTimeData( m_scaleZ );

    _getTimeData( m_shear0 );
    _getTimeData( m_shear1 );
    _getTimeData( m_shear2 );

    _getTimeData( m_rotateX );
    _getTimeData( m_rotateY );
    _getTimeData( m_rotateZ );

    _getTimeData( m_translateX );
    _getTimeData( m_translateY );
    _getTimeData( m_translateZ );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void ISimpleXformSchema::get( SimpleXformSample &oSample,
                              const Abc::ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISimpleXformSchema::get()" );

    index_t sampIndex = iSS.getIndex( m_timeSampling );

    Abc::V3d scale( 1.0 );
    if ( m_scaleX ) { m_scaleX.get( scale.x, sampIndex ); }
    if ( m_scaleY ) { m_scaleY.get( scale.y, sampIndex ); }
    if ( m_scaleZ ) { m_scaleZ.get( scale.z, sampIndex ); }

    Abc::V3d shear( 0.0 );
    if ( m_shear0 ) { m_shear0.get( shear.x, sampIndex ); }
    if ( m_shear1 ) { m_shear1.get( shear.y, sampIndex ); }
    if ( m_shear2 ) { m_shear2.get( shear.z, sampIndex ); }

    Abc::V3d rotate( 0.0 );
    if ( m_rotateX ) { m_rotateX.get( rotate.x, sampIndex ); }
    if ( m_rotateY ) { m_rotateY.get( rotate.y, sampIndex ); }
    if ( m_rotateZ ) { m_rotateZ.get( rotate.z, sampIndex ); }

    Abc::V3d translate( 0.0 );
    if ( m_translateX ) { m_translateX.get( translate.x, sampIndex ); }
    if ( m_translateY ) { m_translateY.get( translate.y, sampIndex ); }
    if ( m_translateZ ) { m_translateZ.get( translate.z, sampIndex ); }

    oSample.makeIdentity();
    oSample.setScale( scale );
    oSample.setShear( shear );
    oSample.setXYZRotation( rotate );
    oSample.setTranslation( translate );

    ALEMBIC_ABC_SAFE_CALL_END();
}

} // End namespace AbcGeom
} // End namespace Alembic
