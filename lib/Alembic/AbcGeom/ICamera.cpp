//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <Alembic/AbcGeom/ICamera.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {


//-*****************************************************************************
void ICameraSchema::init( const Abc::Argument &iArg0,
                          const Abc::Argument &iArg1 )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ICameraSchema::init()" );

    AbcA::CompoundPropertyReaderPtr _this = this->getPtr();

    Abc::Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );


    m_coreProperties = Abc::IScalarProperty( _this, ".core",
        args.getErrorHandlerPolicy() );

    // none of the things below here are guaranteed to exist

    if ( this->getPropertyHeader( ".childBnds" ) != NULL )
    {
        m_childBoundsProperty = Abc::IBox3dProperty( _this, ".childBnds",
            iArg0, iArg1 );
    }

    if ( this->getPropertyHeader( ".arbGeomParams" ) != NULL )
    {
        m_arbGeomParams = Abc::ICompoundProperty( _this, ".arbGeomParams",
            args.getErrorHandlerPolicy() );
    }

    if ( this->getPropertyHeader( ".userProperties" ) != NULL )
    {
        m_userProperties = Abc::ICompoundProperty( _this, ".userProperties",
            args.getErrorHandlerPolicy() );
    }

    // read the film back operations
    const AbcA::PropertyHeader * header =
        this->getPropertyHeader(".filmBackOps");

    // read it from the scalar property
    if ( header != NULL && header->isScalar() )
    {
        Abc::IScalarProperty opsProp( _this, ".filmBackOps",
            args.getErrorHandlerPolicy() );

        size_t numOps = opsProp.getDataType().getExtent();

        std::vector < std::string > ops( numOps );
        opsProp.get( &ops.front() );
        m_ops.resize( ops.size() );


        for ( size_t i = 0; i < numOps; ++i )
        {
            m_ops[i] = FilmBackXformOp( ops[i] );
        }

    }
    // read it from the array property
    else if ( header != NULL && header->isArray() )
    {
        Abc::IStringArrayProperty opsProp( _this, ".filmBackOps",
            iArg0, iArg1 );

        Abc::StringArraySamplePtr ops;
        opsProp.get( ops );
        size_t numOps = ops->size();
        m_ops.resize( numOps );


        for ( size_t i = 0; i < numOps; ++i )
        {
            m_ops[i] = FilmBackXformOp( (*ops)[i] );
        }
    }

    header = this->getPropertyHeader( ".filmBackChannels" );

    if ( header != NULL && header->isScalar() )
    {
        m_smallFilmBackChannels = Abc::IScalarProperty( _this,
            ".filmBackChannels", args.getErrorHandlerPolicy() );
    }
    else if ( header != NULL && header->isArray() )
    {
        m_largeFilmBackChannels = Abc::IDoubleArrayProperty( _this,
            ".filmBackChannels", iArg0, iArg1 );
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

bool ICameraSchema::isConstant() const
{
    return ( m_coreProperties.isConstant() &&
        ( !m_smallFilmBackChannels.valid() ||
          m_smallFilmBackChannels.isConstant() ) &&
        ( !m_largeFilmBackChannels.valid() ||
          m_largeFilmBackChannels.isConstant() ) );
}

void ICameraSchema::get( CameraSample & oSample,
    const Abc::ISampleSelector &iSS ) const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ICameraSchema::get()" );

    double sampleData[16];
    m_coreProperties.get( sampleData, iSS );

    oSample.reset();

    oSample.setFocalLength( sampleData[0] );
    oSample.setHorizontalAperture( sampleData[1] );
    oSample.setHorizontalFilmOffset( sampleData[2] );
    oSample.setVerticalAperture( sampleData[3] );
    oSample.setVerticalFilmOffset( sampleData[4] );
    oSample.setLensSqueezeRatio( sampleData[5] );

    oSample.setOverScanLeft(sampleData[6]);
    oSample.setOverScanRight(sampleData[7]);
    oSample.setOverScanTop(sampleData[8]);
    oSample.setOverScanBottom(sampleData[9]);

    oSample.setFStop(sampleData[10]);
    oSample.setFocusDistance(sampleData[11]);
    oSample.setShutterOpen(sampleData[12]);
    oSample.setShutterClose(sampleData[13]);

    oSample.setNearClippingPlane(sampleData[14]);
    oSample.setFarClippingPlane(sampleData[15]);

    if ( m_smallFilmBackChannels &&
         m_smallFilmBackChannels.getNumSamples() > 0 )
    {
        AbcA::index_t sampIdx = iSS.getIndex(
            m_coreProperties.getTimeSampling(),
            m_smallFilmBackChannels.getNumSamples() );

        std::vector < double > channels (
            m_smallFilmBackChannels.getDataType().getExtent() );
        m_smallFilmBackChannels.get( &channels.front(), sampIdx );

        std::size_t numOps = m_ops.size();
        std::size_t curChan = 0;
        for ( std::size_t i = 0; i < numOps; ++i )
        {
            oSample.addOp( m_ops[i] );
            FilmBackXformOp & curOp = oSample[i];
            std::size_t opChannels = curOp.getNumChannels();
            for ( std::size_t j = 0; j < opChannels; ++j, ++curChan )
            {
                curOp.setChannelValue( j, channels[curChan] );
            }
        }
    }
    else if ( m_largeFilmBackChannels &&
              m_largeFilmBackChannels.getNumSamples() > 0 )
    {
        AbcA::index_t sampIdx = iSS.getIndex(
            m_coreProperties.getTimeSampling(),
            m_largeFilmBackChannels.getNumSamples() );

        Abc::DoubleArraySamplePtr chanSamp;
        m_largeFilmBackChannels.get( chanSamp, sampIdx );

        std::size_t numOps = m_ops.size();
        std::size_t curChan = 0;
        for ( std::size_t i = 0; i < numOps; ++i )
        {
            oSample.addOp( m_ops[i] );
            FilmBackXformOp & curOp = oSample[i];
            std::size_t opChannels = curOp.getNumChannels();
            for ( std::size_t j = 0; j < opChannels; ++j, ++curChan )
            {
                curOp.setChannelValue( j, (*chanSamp)[curChan] );
            }
        }
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcGeom
} // End namespace Alembic
