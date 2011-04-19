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

#include <Alembic/AbcGeom/OXform.h>
#include <Alembic/AbcGeom/XformOp.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void OXformSchema::set( XformSample &ioSamp,
                        const Abc::OSampleSelector &iSS  )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OXformSchema::set()" );


    if ( ioSamp.m_childBounds.hasVolume() )
    { m_childBounds.set( ioSamp.getChildBounds(), iSS ); }

    m_inherits.set( ioSamp.getInheritsXforms(), iSS );

    m_opVec = ioSamp.getOpsArray();

    if ( iSS.getIndex() == 0 )
    {
        // set this to true, so that additional calls to sample's addOp()
        // won't change the topology of the sample, but instead will merely
        // update values.
        ioSamp.setHasBeenRead();

        m_protoSample = ioSamp;

        m_staticChans = std::vector<bool>( ioSamp.getNumOpChannels(), true );
    }
    else
    {
        ABCA_ASSERT( m_protoSample.getOpsArray() == ioSamp.getOpsArray(),
                     "Invalid sample topology!" );
    }

    std::vector<double> chanvals;
    chanvals.reserve( ioSamp.getNumOpChannels() );

    std::vector<Alembic::Util::uint32_t> animchans;
    animchans.reserve( ioSamp.getNumOpChannels() );


    Alembic::Util::int16_t opIdx = 0;
    const Alembic::Util::int16_t nopIdx = -1;

    for ( size_t i = 0, ii = 0 ; i < ioSamp.getNumOps() ; ++i )
    {
        const XformOp &op = ioSamp[i];

        const Alembic::Util::uint32_t &openc = m_opVec[i];

        if ( op.isDefault() )
        {
            m_opVec[i] = ( nopIdx << 16 ) | openc;
            continue;
        }
        else
        {
            m_opVec[i] = ( opIdx << 16 ) | openc;
            opIdx += op.getNumChannels();
        }

        const XformOp &protop = m_protoSample[i];

        for ( size_t j = 0 ; j < op.getNumChannels() ; ++j )
        {
            chanvals.push_back( op.getChannelValue( j ) );

            m_staticChans[j + ii] = m_staticChans[j + ii] &&
                Imath::equalWithAbsError( op.getChannelValue( j ),
                                          protop.getChannelValue( j ),
                                          kXFORM_DELTA_TOLERANCE );

        }

        ii += op.getNumChannels();
    }

    for ( Alembic::Util::uint32_t i = 0 ; i < m_staticChans.size() ; ++i )
    {
        if ( ! m_staticChans[i] ) { animchans.push_back( i ); }
    }

    m_vals.set( chanvals, iSS );

    m_ops.set( m_opVec, iSS );

    m_animChannels.set( animchans, iSS );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OXformSchema::setFromPrevious( const Abc::OSampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OXformSchema::setFromPrevious" );

    m_inherits.setFromPrevious( iSS );

    m_ops.setFromPrevious( iSS );

    m_vals.setFromPrevious( iSS );

    m_animChannels.setFromPrevious( iSS );

    if ( m_childBounds.getNumSamples() > 0 )
    { m_childBounds.setFromPrevious( iSS ); }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OXformSchema::init( const AbcA::TimeSamplingType &iTst )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OXformSchema::init()" );

    m_timeSamplingType = iTst;

    m_childBounds = Abc::OBox3dProperty( this->getPtr(), ".childBnds", iTst );

    m_inherits = Abc::OBoolProperty( this->getPtr(), ".inherits", iTst );

    // This will hold the shape of the xform
    m_timeSamplingType.setRetainConstantSampleTimes( true );
    m_ops = Abc::OUInt32ArrayProperty( this->getPtr(), ".ops",
                                       m_timeSamplingType );

    m_vals = Abc::ODoubleArrayProperty( this->getPtr(), ".vals" );

    m_animChannels = Abc::OUInt32ArrayProperty( this->getPtr(),
                                                ".animChans" );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
