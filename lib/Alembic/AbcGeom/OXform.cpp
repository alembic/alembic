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

#define MAX_SCALAR_CHANS 256

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void OXformSchema::setChannelValues( const std::vector<double> &iVals )
{
    if ( ! m_vals ) { return; }

    if ( m_useArrayProp )
    {
        Alembic::Util::Dimensions dims(m_numChannels);
        m_vals->asArrayPtr()->setSample(
            AbcA::ArraySample( &(iVals.front()),
                               AbcA::DataType( Alembic::Util::kFloat64POD, 1 ),
                               dims )
                                       );
    }
    else
    {
        m_vals->asScalarPtr()->setSample( &(iVals.front()) );
    }
}

//-*****************************************************************************
void OXformSchema::set( XformSample &ioSamp )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OXformSchema::set()" );

    // do we need to create child bounds?
    if ( ioSamp.getChildBounds().hasVolume() && !m_childBounds )
    {
        m_childBounds = Abc::OBox3dProperty( this->getPtr(), ".childBnds",
                                             m_inherits.getTimeSampling() );

        Abc::Box3d emptyBox;
        emptyBox.makeEmpty();

        size_t numSamples = m_inherits.getNumSamples();

        // set all the missing samples
        for ( size_t i = 0; i < numSamples; ++i )
        {
            m_childBounds.set( emptyBox );
        }
    }


    if ( m_inherits.getNumSamples() == 0 )
    {
        // set this to true, so that additional calls to sample's addOp()
        // won't change the topology of the sample, but instead will merely
        // update values.
        ioSamp.freezeTopology();

        m_protoSample = ioSamp;

        m_numChannels = ioSamp.getNumOpChannels();
        m_numOps = ioSamp.getNumOps();

        m_staticChans = std::vector<bool>( m_numChannels, true );

        if ( m_numOps > 0 )
        {
            m_ops = this->getPtr()->createScalarProperty(
                ".ops", AbcA::MetaData(),
                AbcA::DataType( Alembic::Util::kUint8POD, m_numOps ), 0
                                                        );
        }

        if ( m_numChannels > 0 )
        {
            uint32_t tsIndex = getObject().getArchive().addTimeSampling(
                                       *(m_inherits.getTimeSampling()) );
            if ( m_numChannels <= MAX_SCALAR_CHANS )
            {
                m_useArrayProp = false;

                m_vals = this->getPtr()->createScalarProperty(
                    ".vals", AbcA::MetaData(),
                    AbcA::DataType( Alembic::Util::kFloat64POD, m_numChannels ),
                    tsIndex
                                                             );
            }
            else
            {
                m_useArrayProp = true;

                m_vals = this->getPtr()->createArrayProperty(
                    ".vals", AbcA::MetaData(),
                    // the DataType for an ArrayProperty describes not how big
                    // each Sample is, but how many values constitute a single
                    // "element". What is here is the same as creating an
                    // Abc::ODoubleArrayProperty.
                    AbcA::DataType( Alembic::Util::kFloat64POD, 1 ), tsIndex
                                                            );
            }
        }

    }
    else
    {
        ABCA_ASSERT( m_protoSample.isTopologyEqual(ioSamp),
                     "Invalid sample topology!" );
    }

    if ( ioSamp.m_childBounds.hasVolume() )
    { m_childBounds.set( ioSamp.getChildBounds() ); }

    m_inherits.set( ioSamp.getInheritsXforms() );

    if ( ! m_ops ) { return; }

    std::vector<double> chanvals;
    chanvals.reserve( ioSamp.getNumOpChannels() );

    std::vector<Alembic::Util::uint32_t> animchans;
    animchans.reserve( ioSamp.getNumOpChannels() );

    for ( size_t i = 0, ii = 0 ; i < m_numOps ; ++i )
    {
        const XformOp &op = ioSamp[i];

        const XformOp &protop = m_protoSample[i];

        for ( size_t j = 0 ; j < op.getNumChannels() ; ++j )
        {
            chanvals.push_back( op.getChannelValue( j ) );

            m_staticChans[j + ii] = m_staticChans[j + ii] &&
                Imath::equalWithAbsError( op.getChannelValue( j ),
                                          protop.getChannelValue( j ),
                                          kXFORM_DELTA_TOLERANCE );


            m_isIdentity = m_isIdentity &&
                Imath::equalWithAbsError( op.getChannelValue( j ),
                                          op.getDefaultChannelValue( j ),
                                          kXFORM_DELTA_TOLERANCE );
        }

        ii += op.getNumChannels();
    }

    for ( Alembic::Util::uint32_t i = 0 ; i < m_staticChans.size() ; ++i )
    {
        if ( ! m_staticChans[i] )
        {
            animchans.push_back( i );
        }
    }

    this->setChannelValues( chanvals );

    if ( m_ops && m_ops->getNumSamples() == 0 )
    {
        std::vector < Alembic::Util::uint8_t > opVec(
            m_protoSample.getNumOps() );

        for ( std::size_t i = 0; i < opVec.size(); ++i )
        {
            opVec[i] = m_protoSample[i].getOpEncoding();
        }

        m_ops->setSample( &(opVec.front()) );
    }
    else if ( m_ops )
    {
        m_ops->setFromPreviousSample();
    }

    if ( !m_isNotConstantIdentity && !m_isIdentity )
    {
        m_isNotConstantIdentity = Abc::OBoolProperty( this->getPtr(),
                                                      "isNotConstantIdentity" );

        m_isNotConstantIdentity.set( true );
    }

    m_animChannels.set( animchans );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OXformSchema::setFromPrevious()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OXformSchema::setFromPrevious" );

    m_inherits.setFromPrevious();

    m_ops->setFromPreviousSample();

    if ( m_vals )
    {
        if ( m_useArrayProp )
        { m_vals->asArrayPtr()->setFromPreviousSample(); }
        else
        { m_vals->asScalarPtr()->setFromPreviousSample(); }
    }

    m_animChannels.setFromPrevious();

    if ( m_childBounds.getNumSamples() > 0 )
    { m_childBounds.setFromPrevious(); }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OXformSchema::init( const AbcA::index_t iTsIdx )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OXformSchema::init()" );

    m_inherits = Abc::OBoolProperty( this->getPtr(), ".inherits",
                                     iTsIdx );

    m_animChannels = Abc::OUInt32ArrayProperty( this->getPtr(),
                                                ".animChans", iTsIdx );

    m_isIdentity = true;

    m_numOps = 0;
    m_numChannels = 0;

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
Abc::OCompoundProperty OXformSchema::getArbGeomParams()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OXformSchema::getArbGeomParams()" );

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
void OXformSchema::setTimeSampling( uint32_t iIndex )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OXformSchema::setTimeSampling( uint32_t )" );

    m_animChannels.setTimeSampling( iIndex );
    m_inherits.setTimeSampling( iIndex );

    if ( m_vals )
    {
        if ( m_useArrayProp )
        { m_vals->asArrayPtr()->setTimeSamplingIndex( iIndex ); }
        else
        { m_vals->asScalarPtr()->setTimeSamplingIndex( iIndex ); }
    }

    if ( m_childBounds )
    {
        m_childBounds.setTimeSampling( iIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OXformSchema::setTimeSampling( AbcA::TimeSamplingPtr iTime )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OXformSchema::setTimeSampling( TimeSamplingPtr )" );

    if ( iTime )
    {
        uint32_t tsIndex = getObject().getArchive().addTimeSampling( *iTime );
        setTimeSampling( tsIndex );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

} // End namespace AbcGeom
} // End namespace Alembic
