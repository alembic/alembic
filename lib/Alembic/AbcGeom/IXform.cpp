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

#include <Alembic/AbcGeom/IXform.h>
#include <Alembic/AbcGeom/XformOp.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void IXformSchema::init( Abc::SchemaInterpMatching iMatching )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::init()" );

    AbcA::CompoundPropertyReaderPtr ptr = this->getPtr();

    m_childBounds = Abc::IBox3dProperty( ptr, ".childBnds", iMatching );

    m_inherits = Abc::IBoolProperty( ptr, ".inherits", iMatching );

    m_ops = ptr->getScalarProperty(  ".ops" );

    m_vals = ptr->getScalarProperty( ".vals" );

    m_isConstantIdentity = true;

    m_isConstant = true;

    m_numChannels = 0;

    m_numOps = 0;

    if ( m_vals )
    {
        m_numChannels = m_vals->getHeader().getDataType().getExtent();
        m_isConstant = m_vals->isConstant();
    }

    m_isConstant = m_isConstant && m_inherits.isConstant();

    m_valVec.resize( m_numChannels );

    if ( ptr->getPropertyHeader( ".animChans" ) )
    {
        Abc::IUInt32ArrayProperty p( ptr, ".animChans" );
        if ( p.getNumSamples() > 0 )
        { m_animChannels = (*(p.getValue( p.getNumSamples() - 1 ))); }
    }

    if ( m_ops && m_ops->getNumSamples() > 0 )
    {
        m_numOps = m_ops->getHeader().getDataType().getExtent();
    }

    m_opVec.resize( m_numOps );

    if ( m_vals && m_vals->getNumSamples() > 0 )
    {
        m_isConstantIdentity = m_isConstant && m_numChannels == 0;
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
AbcA::TimeSamplingPtr IXformSchema::getTimeSampling()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::getTimeSampling()" );

    return m_inherits.getTimeSampling();

    ALEMBIC_ABC_SAFE_CALL_END();

    AbcA::TimeSamplingPtr ret;
    return ret;
}

//-*****************************************************************************
std::size_t IXformSchema::getNumSamples()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::getNumSamples()" );

    return m_inherits.getNumSamples();

    ALEMBIC_ABC_SAFE_CALL_END();

    return 0;
}

//-*****************************************************************************
void IXformSchema::get( XformSample &oSamp, const Abc::ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::get()" );

    oSamp.clear();

    if ( ! valid() ) { return; }

    oSamp.setInheritsXforms( m_inherits.getValue( iSS ) );

    if ( m_childBounds && m_childBounds.getNumSamples() > 0 )
    {
        oSamp.setChildBounds( m_childBounds.getValue( iSS ) );
    }

    if ( m_ops == NULL ) { return; }

    AbcA::index_t sampIdx = iSS.getIndex( m_ops->getTimeSampling(),
                                          m_ops->getNumSamples() );

    if ( sampIdx < 0 ) { return; }

    m_ops->getSample( sampIdx, &(m_opVec.front()) );
    m_vals->getSample( sampIdx, &(m_valVec.front()) );

    std::size_t curIdx = 0;
    for ( std::size_t i = 0 ; i < m_numOps ; ++i )
    {
        Alembic::Util::uint32_t openc = m_opVec[i];
        XformOp op( openc );

        for ( std::size_t j = 0 ; j < op.getNumChannels() ; ++j )
        {
            const std::size_t animIdx = curIdx + j;
            for ( std::size_t k = 0 ; k < m_animChannels.size() ; ++k )
            {
                if ( static_cast<std::size_t>( m_animChannels[k] ) == animIdx )
                {
                    op.m_animChannels.insert( j );
                }
            }

            op.setChannelValue( j, m_valVec[animIdx] );
        }
        oSamp.addOp( op );
        curIdx += op.getNumChannels();
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
XformSample IXformSchema::getValue( const Abc::ISampleSelector &iSS )
{
    XformSample ret;
    this->get( ret, iSS );
    return ret;
}

//-*****************************************************************************
bool IXformSchema::getInheritsXforms( const Abc::ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::getInheritsXforms()" );

    if ( ! m_inherits ) { return true; }

    AbcA::index_t sampIdx = iSS.getIndex( m_inherits.getTimeSampling(),
                                          m_inherits.getNumSamples() );

    if ( sampIdx < 0 ) { return true; }

    return m_inherits.getValue( sampIdx );

    ALEMBIC_ABC_SAFE_CALL_END();

    return true;
}

} // End namespace AbcGeom
} // End namespace Alembic
