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

#include <boost/lexical_cast.hpp>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void
OXformSchema::ODefaultedDoubleProperty::set(
    const double &iVal,
    const Abc::OSampleSelector &iSS,
    const std::size_t &iNumSampsSoFar )
{
    if ( m_property )
    {
        m_property.set( iVal, iSS );
        return;
    }

    if ( ! Imath::equalWithAbsError( iVal - m_default, 0.0, m_epsilon ) )
    {
        // A change!
        m_property = Abc::ODoubleProperty( m_parent, m_name,
                                           m_errorHandlerPolicy );

        // Run up the defaults.
        for ( size_t jdx = 0 ; jdx < iNumSampsSoFar ; ++jdx )
        {
            Abc::OSampleSelector jSS( jdx );

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
OXformSchema::ODefaultedDoubleProperty::setFromPrevious(
    const Abc::OSampleSelector &iSS )
{
    if ( m_property )
    {
        m_property.setFromPrevious( iSS );
    }
}


//-*****************************************************************************
void OXformSchema::set( XformSample &ioSamp,
                        const Abc::OSampleSelector &iSS  )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OXformSchema::set()" );


    if ( ioSamp.m_childBounds.hasVolume() )
    { m_childBounds.set( ioSamp.getChildBounds(), iSS ); }

    m_inherits.set( ioSamp.getInheritsXforms(), iSS );

    if ( iSS.getIndex() == 0 )
    {
        // set this to true, so that additional calls to sample's addOp()
        // won't change the topology of the sample, but instead will merely
        // update values.
        ioSamp.setHasBeenRead();

        ioSamp.m_id = reinterpret_cast<std::size_t>( this );

        m_sampID = ioSamp.getID();

        m_props.reserve( ioSamp.getNumOpChannels() );

        // This property will be constant, but it will also contain the xform's
        // timesampling information; the op properties won't have time info on
        // them.
        //
        // The "ops array" is actually an array of packed uchars that encode
        // the type of the op and the op's hint.  Actually getting the XformOps
        // from the sample is via XformSample::getOp( size_t ).
        m_ops.set( ioSamp.getOpsArray(), iSS );

        AbcA::CompoundPropertyWriterPtr cptr = this->getPtr();
        Abc::ErrorHandler::Policy pcy = this->getErrorHandlerPolicy();

        // Create our well-named Properties, push them into our propvec,
        // and set them.
        for ( size_t i = 0 ; i < ioSamp.getNumOps() ; ++i )
        {
            const XformOp &op = ioSamp.getOp( i );
            std::string oname = boost::lexical_cast<std::string>( i );

            for ( size_t j = 0 ; j < op.getNumChannels() ; ++j )
            {
                // eg, ".tx_"
                std::string channame = op.getChannelName( j );

                // name will be, eg, ".tx_0"
                ODefaultedDoubleProperty prop(
                    cptr, channame + oname, pcy,
                    op.getDefaultChannelValue( j ) );

                prop.set( op.getChannelValue( j ), iSS, m_numSetSamples );

                m_props.push_back( prop );
            }
        }
    }
    else
    {
        ABCA_ASSERT( ioSamp.getID() == 0 || m_sampID == ioSamp.getID(),
                     "Invalid sample ID!" );

        m_ops.setFromPrevious( iSS );

        size_t prevIdx = 0;
        for ( size_t i = 0 ; i < ioSamp.getNumOps() ; ++i )
        {
            XformOp op = ioSamp.getOp( i );

            for ( size_t j = 0 ; j < op.getNumChannels() ; ++j )
            {
                size_t idx = prevIdx + j;

                m_props[idx].set( op.getChannelValue( j ), iSS,
                                    m_numSetSamples );
            }
            prevIdx += op.getNumChannels();
        }
    }

    // bump our set count
    ++m_numSetSamples;

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OXformSchema::setFromPrevious( const Abc::OSampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OXformSchema::setFromPrevious" );

    ++m_numSetSamples;

    m_inherits.setFromPrevious( iSS );

    m_ops.setFromPrevious( iSS );

    if ( m_childBounds.getNumSamples() > 0 )
    { m_childBounds.setFromPrevious( iSS ); }

    for ( std::vector<ODefaultedDoubleProperty>::iterator it = m_props.begin()
              ; it != m_props.end() ; ++it )
    {
        it->setFromPrevious( iSS );
    }

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
    m_ops = Abc::OUcharArrayProperty( this->getPtr(), ".ops",
                                      m_timeSamplingType );

    m_sampID = 0;

    m_numSetSamples = 0;

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
