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

#include <Alembic/AbcGeom/OSimpleXform.h>

#include <ImathMath.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void
OSimpleXformSchema::ODefaultedDoubleProperty::set(
    const double &iVal,
    const Abc::OSampleSelector &iSS,
    const std::vector<chrono_t> &iTimes )
{
    chrono_t t = iSS.getTime();
    if ( m_property )
    {
        m_property.set( iVal, iSS );
        return;
    }

    if ( ! Imath::equalWithAbsError( iVal - m_default, 0.0, m_epsilon ) )
    {
        // A change!
        m_property = Abc::ODoubleProperty( m_parent, m_name,
                                           m_errorHandlerPolicy,
                                           m_timeSamplingType );

        // Run up the defaults.
        index_t idx = iSS.getIndex();

        for ( size_t jdx = 0 ; jdx < idx ; jdx++ )
        {
            Abc::OSampleSelector jSS;
            if ( ! iTimes.empty() )
            {
                jSS = Abc::OSampleSelector( jdx, iTimes[jdx] );
            }
            else
            {
                jSS = Abc::OSampleSelector( jdx );
            }

            if ( jdx == 0 )
            {
                m_property.set( m_default, jSS );
            }
            else
            {
                m_property.setFromPrevious( jSS );
            }
        }

        // set the final one.
        m_property.set( iVal, iSS );
    }
}

//-*****************************************************************************
void
OSimpleXformSchema::ODefaultedDoubleProperty::setFromPrevious(
    const Abc::OSampleSelector &iSS )
{
    if ( m_property )
    {
        m_property.setFromPrevious( iSS );
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
void OSimpleXformSchema::init( const AbcA::TimeSamplingType &iTst )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OSimpleXformSchema::init()" );

    AbcA::CompoundPropertyWriterPtr cptr = this->getPtr();
    Abc::ErrorHandler::Policy pcy = this->getErrorHandlerPolicy();

    m_scaleX = ODefaultedDoubleProperty( cptr, ".sx", pcy, iTst, 1.0 );
    m_scaleY = ODefaultedDoubleProperty( cptr, ".sy", pcy, iTst, 1.0 );
    m_scaleZ = ODefaultedDoubleProperty( cptr, ".sz", pcy, iTst, 1.0 );

    m_shear0 = ODefaultedDoubleProperty( cptr, ".h0", pcy, iTst, 0.0 );
    m_shear1 = ODefaultedDoubleProperty( cptr, ".h1", pcy, iTst, 0.0 );
    m_shear2 = ODefaultedDoubleProperty( cptr, ".h2", pcy, iTst, 0.0 );

    m_rotateX = ODefaultedDoubleProperty( cptr, ".rx", pcy, iTst, 0.0 );
    m_rotateY = ODefaultedDoubleProperty( cptr, ".ry", pcy, iTst, 0.0 );
    m_rotateZ = ODefaultedDoubleProperty( cptr, ".rz", pcy, iTst, 0.0 );

    m_translateX = ODefaultedDoubleProperty( cptr, ".tx", pcy, iTst, 0.0 );
    m_translateY = ODefaultedDoubleProperty( cptr, ".ty", pcy, iTst, 0.0 );
    m_translateZ = ODefaultedDoubleProperty( cptr, ".tz", pcy, iTst, 0.0 );

    m_childBounds = Abc::OBox3dProperty( cptr, ".childBnds", pcy, iTst );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void OSimpleXformSchema::set( const SimpleXformSample &iSamp,
                              const Abc::OSampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSimpleXformSchema::set()" );

    // Get the pieces.
    Abc::V3d scale = iSamp.getScale();
    Abc::V3d shear = iSamp.getShear();
    Abc::V3d rot = iSamp.getXYZRotation();;
    Abc::V3d trans = iSamp.getTranslation();

    // Get an index and a time
    index_t idx = iSS.getIndex();
    chrono_t time = iSS.getTime();

    if ( iSamp.getChildBounds().hasVolume() )
    { m_childBounds.set( iSamp.getChildBounds(), iSS ); }

    // Push back the times if we need to.
    if ( m_times.size() == idx  )
    {
        m_times.push_back( time );

        m_scaleX.set( scale.x, iSS, m_times );
        m_scaleY.set( scale.y, iSS, m_times );
        m_scaleZ.set( scale.z, iSS, m_times );

        m_shear0.set( shear.x, iSS, m_times );
        m_shear1.set( shear.y, iSS, m_times );
        m_shear2.set( shear.z, iSS, m_times );

        m_rotateX.set( rot.x, iSS, m_times );
        m_rotateY.set( rot.y, iSS, m_times );
        m_rotateZ.set( rot.z, iSS, m_times );

        m_translateX.set( trans.x, iSS, m_times );
        m_translateY.set( trans.y, iSS, m_times );
        m_translateZ.set( trans.z, iSS, m_times );
    }
    else
    {
        std::vector<chrono_t> empty;
        empty.clear();

        m_scaleX.set( scale.x, iSS, empty );
        m_scaleY.set( scale.y, iSS, empty );
        m_scaleZ.set( scale.z, iSS, empty );

        m_shear0.set( shear.x, iSS, empty );
        m_shear1.set( shear.y, iSS, empty );
        m_shear2.set( shear.z, iSS, empty );

        m_rotateX.set( rot.x, iSS, empty );
        m_rotateY.set( rot.y, iSS, empty );
        m_rotateZ.set( rot.z, iSS, empty );

        m_translateX.set( trans.x, iSS, empty );
        m_translateY.set( trans.y, iSS, empty );
        m_translateZ.set( trans.z, iSS, empty );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OSimpleXformSchema::setFromPrevious( const Abc::OSampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OSimpleXformSchema::setFromPrevious()" );

    // Get an index and a time
    index_t idx = iSS.getIndex();
    chrono_t time = iSS.getTime();

    if ( m_childBounds.getNumSamples() > 0 )
    { m_childBounds.setFromPrevious( iSS ); }

    // Push back the times if we need to.
    if ( m_times.size() == idx )
    {
        m_times.push_back( time );

        m_scaleX.setFromPrevious( iSS );
        m_scaleY.setFromPrevious( iSS );
        m_scaleZ.setFromPrevious( iSS );

        m_shear0.setFromPrevious( iSS );
        m_shear1.setFromPrevious( iSS );
        m_shear2.setFromPrevious( iSS );

        m_rotateX.setFromPrevious( iSS );
        m_rotateY.setFromPrevious( iSS );
        m_rotateZ.setFromPrevious( iSS );

        m_translateX.setFromPrevious( iSS );
        m_translateY.setFromPrevious( iSS );
        m_translateZ.setFromPrevious( iSS );
    }
    else
    {
        m_scaleX.setFromPrevious( iSS );
        m_scaleY.setFromPrevious( iSS );
        m_scaleZ.setFromPrevious( iSS );

        m_shear0.setFromPrevious( iSS );
        m_shear1.setFromPrevious( iSS );
        m_shear2.setFromPrevious( iSS );

        m_rotateX.setFromPrevious( iSS );
        m_rotateY.setFromPrevious( iSS );
        m_rotateZ.setFromPrevious( iSS );

        m_translateX.setFromPrevious( iSS );
        m_translateY.setFromPrevious( iSS );
        m_translateZ.setFromPrevious( iSS );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

} // End namespace AbcGeom
} // End namespace Alembic
