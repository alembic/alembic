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

#include <boost/lexical_cast.hpp>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void
IXformSchema::IDefaultedDoubleProperty::init()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IXformSchema::IDefaultedDoubleProperty::init()" );

    AbcA::ScalarPropertyReaderPtr ptr = m_parent->getScalarProperty( m_name );

    if ( ptr )
    {
        m_property = Abc::IDoubleProperty( ptr, kWrapExisting,
                                           m_errorHandler.getPolicy() );

        m_isConstant = m_property.isConstant();

        m_property.get( m_constantValue );
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
double
IXformSchema::IDefaultedDoubleProperty::getValue(
    const Abc::ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IXformSchema::IDefaultedDoubleProperty::getValue()" );

    if ( m_isConstant )
    { return m_constantValue; }
    else
    { return m_property.getValue( iSS ); }

    ALEMBIC_ABC_SAFE_CALL_END();

    return 0.0;
}

//-*****************************************************************************
void IXformSchema::init( Abc::SchemaInterpMatching iMatching )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::init()" );

    AbcA::CompoundPropertyReaderPtr ptr = this->getPtr();

    m_childBounds = Abc::IBox3dProperty( ptr, ".childBnds", iMatching );

    m_isToWorld = Abc::IBoolProperty( ptr, ".istoworld", iMatching );

    m_ops = Abc::IUcharArrayProperty( ptr, ".ops", iMatching );

    // OK, now the fun stuff!
    //
    // All the above Properties are guaranteed to exist.  None of the actual
    // xform-data-having properties need to exist, though.  So we use the same
    // well-formed naming mechanism from OXform::set() to make an array of
    // IDefaultedDoubleProperties.

    Abc::UcharArraySample opSampArray = *(m_ops.getValue());

    std::size_t numOps = opSampArray.size();

    m_opArray.reserve( numOps );

    m_isConstant = true;

    for ( std::size_t i = 0 ; i < numOps ; ++i )
    {
        XformOp op( opSampArray[i] );
        std::string oname = boost::lexical_cast<std::string>( i );

        m_opArray.push_back( op );

        for ( std::size_t j = 0 ; j < op.getNumChannels() ; ++j )
        {
            std::string channame = op.getChannelName( j );

            IDefaultedDoubleProperty prop(
                ptr, channame + oname, this->getErrorHandler(),
                op.getDefaultChannelValue( j ) );

            m_props.push_back( prop );

            m_isConstant = m_isConstant && prop.isConstant();
        }
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

    AbcA::index_t sampIdx = iSS.getIndex( m_ops.getTimeSampling() );

    oSamp.clear();

    std::size_t prevIdx = 0;
    for ( std::size_t i = 0 ; i < m_opArray.size() ; ++i )
    {
        XformOp op = m_opArray[i];
        for ( std::size_t j = 0 ; j < op.getNumChannels() ; ++j )
        {
            size_t pidx = j + prevIdx;

            op.setChannelValue( j, m_props[pidx].getValue( sampIdx ) );

            if ( ! m_props[pidx].isConstant() )
            {
                op.m_animChannels.insert( j );
            }
        }
        prevIdx += op.getNumChannels();
        oSamp.addOp( op );
    }

    oSamp.setIsToWorld( m_isToWorld.getValue( sampIdx ) );

    oSamp.setChildBounds( m_childBounds.getValue( sampIdx ) );

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
bool IXformSchema::getIsToWorld( const Abc::ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::getIsToWorld()" );

    AbcA::index_t sampIdx = iSS.getIndex( m_ops.getTimeSampling() );

    return m_isToWorld.getValue( sampIdx );

    ALEMBIC_ABC_SAFE_CALL_END();

    return false;
}

} // End namespace AbcGeom
} // End namespace Alembic
