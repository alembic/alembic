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

#include <Alembic/AbcGeom/OPoints.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void OPointsSchema::set( const Sample &iSamp,
                         const Abc::OSampleSelector &iSS  )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::set()" );

    // We could add sample integrity checking here.
    if ( iSS.getIndex() == 0 )
    {
        // First sample must be valid on all points.
        ABCA_ASSERT( iSamp.getPositions() &&
                     iSamp.getIds(),
                     "Sample 0 must have valid data for points and ids" );
        m_positions.set( iSamp.getPositions(), iSS );
        m_ids.set( iSamp.getIds(), iSS );

        m_childBounds.set( iSamp.getChildBounds(), iSS );

        if ( iSamp.getSelfBounds().isEmpty() )
        {
            // OTypedScalarProperty::set() is not referentially transparent,
            // so we need a a placeholder variable.
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() )
                           );
            m_selfBounds.set( bnds, iSS );
        }
        else { m_selfBounds.set( iSamp.getSelfBounds(), iSS ); }
    }
    else
    {
        SetPropUsePrevIfNull( m_positions, iSamp.getPositions(), iSS );
        SetPropUsePrevIfNull( m_ids, iSamp.getIds(), iSS );

        if ( iSamp.getSelfBounds().hasVolume() )
        {
            m_selfBounds.set( iSamp.getSelfBounds(), iSS );
        }
        else if ( iSamp.getPositions() )
        {
            Abc::Box3d bnds(
                ComputeBoundsFromPositions( iSamp.getPositions() )
                           );
            m_selfBounds.set( bnds, iSS );
        }
        else
        {
            m_selfBounds.setFromPrevious( iSS );
        }
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OPointsSchema::setFromPrevious( const Abc::OSampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::setFromPrevious" );

    m_positions.setFromPrevious( iSS );
    m_ids.setFromPrevious( iSS );

    m_selfBounds.setFromPrevious( iSS );
    m_childBounds.setFromPrevious( iSS );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
Abc::OCompoundProperty OPointsSchema::getArbGeomParams()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::getArbGeomParams()" );

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

//-*****************************************************************************
void OPointsSchema::init( const AbcA::TimeSamplingType &iTst )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OPointsSchema::init()" );

    AbcA::MetaData mdata;
    SetGeometryScope( mdata, kVaryingScope );

    AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

    m_positions = Abc::OV3fArrayProperty( _this, "P", mdata, iTst );

    m_ids = Abc::OUInt64ArrayProperty( _this, ".pointIds", mdata, iTst );

    m_selfBounds = Abc::OBox3dProperty( _this, ".selfBnds", iTst );

    m_childBounds = Abc::OBox3dProperty( _this, ".childBnds", iTst );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
