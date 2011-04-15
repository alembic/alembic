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

    m_ops = Abc::IUcharArrayProperty( ptr, ".ops", iMatching );

    m_vals = Abc::IDoubleArrayProperty( ptr, ".vals", iMatching );

    m_isConstantIdentity = false;

    if ( ptr->getPropertyHeader( ".staticChans" ) )
    {
        Abc::IBoolArrayProperty p( ptr, ".staticChannels" );
        m_staticChannels = *(p.getValue());
    }

    if ( ptr->getPropertyHeader( ".isIdty" ) )
    {
        // If the property is there at all, we're identity, and therefore
        // constant, too.
        m_isConstantIdentity = true;
    }

    m_isConstant = m_vals.isConstant();

    Abc::UcharArraySample opSampArray;
    if ( m_ops.getNumSamples() > 0 ) { opSampArray = *(m_ops.getValue()); }

    std::size_t numOps = 0;

    if ( opSampArray )
    { numOps = opSampArray.size(); }

    m_opArray.reserve( numOps );

    for ( std::size_t i = 0 ; i < numOps ; ++i )
    {
        m_opArray.push_back( opSampArray[i] );
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
AbcA::TimeSampling IXformSchema::getTimeSampling()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::getTimeSampling()" );

    return m_ops.getTimeSampling();

    ALEMBIC_ABC_SAFE_CALL_END();

    AbcA::TimeSampling ret;
    return ret;
}

//-*****************************************************************************
std::size_t IXformSchema::getNumSamples()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::getNumSamples()" );

    return m_ops.getNumSamples();

    ALEMBIC_ABC_SAFE_CALL_END();

    return 0;
}

//-*****************************************************************************
void IXformSchema::get( XformSample &oSamp, const Abc::ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::get()" );

    oSamp.clear();

    AbcA::index_t sampIdx = iSS.getIndex( m_ops.getTimeSampling() );

    if ( sampIdx < 0 ) { return; }

    std::size_t prevIdx = 0;
    for ( std::size_t i = 0 ; i < m_opArray.size() ; ++i )
    {
        XformOp op = m_opArray[i];
        for ( std::size_t j = 0 ; j < op.getNumChannels() ; ++j )
        {
            size_t pidx = j + prevIdx;

            op.setChannelValue( j, (*(m_vals.getValue( sampIdx )))[pidx] );

            if ( ! m_staticChannels[pidx] )
            {
                op.m_animChannels.insert( j );
            }
        }
        prevIdx += op.getNumChannels();
        oSamp.addOp( op );
    }

    oSamp.setInheritsXforms( m_inherits.getValue( sampIdx ) );

    if ( m_childBounds && m_childBounds.getNumSamples() > 0 )
    {
        oSamp.setChildBounds( m_childBounds.getValue( sampIdx ) );
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

    AbcA::index_t sampIdx = iSS.getIndex( m_ops.getTimeSampling() );

    if ( sampIdx < 0 ) { return true; }

    return m_inherits.getValue( sampIdx );

    ALEMBIC_ABC_SAFE_CALL_END();

    return true;
}

} // End namespace AbcGeom
} // End namespace Alembic
