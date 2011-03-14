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

#include <Alembic/AbcGeom/OXform.h>
#include <Alembic/AbcGeom/GeometryScope.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
// minor helper function
void OXformSchema::_setXformOpProps( const XformSample &iSamp,
                                     const OSampleSelector &iSS,
                                     const std::vector<chrono_t> &iTimes )
{
    for ( size_t i = 0 ; i < iSamp.getNumOps() ; ++i )
    {
        XformOp op = iSamp.getOp( i );

        for ( size_t j = 0 ; j < op.getNumChannels() ; ++j )
        {
            m_props[i + j].set( op.getChannelValue( j ), iSS, iTimes );
        }
    }
}


//-*****************************************************************************
void OXformSchema::set( XformSample &ioSamp,
                        const Abc::OSampleSelector &iSS  )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OXformSchema::set()" );

    ABCA_ASSERT( !iSamp.getID().is_nil(), "Sample has been reset!" );

    index_t idx = iSS.getIndex();
    chrono_t time = iSS.getTime();

    if ( ioSamp.getChildBounds.hasVolume() )
    { m_childBounds.set( ioSamp.getChildBounds(), iSS ); }

    m_isToWorld.set( ioSamp.getIsToWorld(), iSS );


    if ( iSS.getIndex() == 0 )
    {
        // set this to true, so that additional calls to sample's addOp()
        // won't change the topology of the sample, but instead will merely
        // update values.
        ioSamp.setHasBeenRead( true );

        m_sampID = ioSamp.getID();

        // this property will be constant, but it will also contain the xform's
        // timesampling information; the op properties won't have time info on
        // them.
        m_ops.set( ioSamp.getOpsArray(), iSS );

        m_times.push_back( time );

        AbcA::CompoundPropertyWriterPtr cptr = this->getPtr();
        Abc::ErrorHandler::Policy pcy = this->getErrorHandlerPolicy();

        // Create our well-named Properties, push them into our propvec,
        // and set them.
        for ( size_t i = 0 ; i < ioSamp.getNumOps() ; ++i )
        {
            XformOp op = ioSamp.getOp( i );
            std::string oname = boost::lexical_cast<std::string>( i );

            for ( size_t j = 0 ; j < op.getNumChannels() ; ++j )
            {
                // eg, ".tx"
                std::string channame = op.getChannelName( j );

                // name will be, eg, ".tx0"
                prop = ODefaultedDoubleProperty(
                    cptr, channame + oname, pcy,
                    op.getDefaultChannelValue( j ) );

                prop.set( op.getChannelValue( j ), iSS, m_times );

                m_props.push_back( prop );
            }
        }
    }
    else
    {
        ABCA_ASSERT( m_sampID == ioSamp.getID() && !m_sampID.is_nil(),
                     "Invalid sample ID!" );

        m_ops.setFromPrevious( iSS );

        if ( m_times.size() == idx )
        {
            m_times.push_back( time );
            this->_setXformOpProps( ioSamp, iSS, m_times );
        }
        else
        {
            std::vector<chrono_t> empty;
            empty.clear();
            this->_setXformOpProps( ioSamp, iSS, empty );
        }
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OXformSchema::setFromPrevious( const Abc::OSampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OXformSchema::setFromPrevious" );

    m_isToWorld.setFromPrevious( iSS );

    m_ops.setFromPrevious( iSS );

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

    m_isToWorld = Abc::OBoolProperty( this->getPtr(), ".istoworld", iTst );

    // This will hold the shape of the xform
    m_timeSamplingType.setRetainConstantSampleTimes( true );
    m_ops = Abc::OUcharArrayProperty( this->getPtr(), ".ops",
                                      m_timeSamplingType );

    boost::uuids::nil_generator ng;
    m_sampID = ng();

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
