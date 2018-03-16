//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreFactory/IFactory.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

using namespace Alembic::AbcGeom; // Contains Abc, AbcCoreAbstract

//-*****************************************************************************
void cameraTest()
{
    std::string fileName = "camera1.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName );
        CameraSample samp;
        OCamera simpleCamObj( OObject( archive, kTop ), "simpleCam" );
        simpleCamObj.getSchema().set( samp );

        OCamera camObj( OObject( archive, kTop ), "cam" );
        OCameraSchema camSchema = camObj.getSchema();
        samp.addOp( FilmBackXformOp( kScaleFilmBackOperation, "scale" ) );
        samp.addOp( FilmBackXformOp( kTranslateFilmBackOperation, "offset" ) );
        camSchema.set( samp );

        samp[0].setScale( V2d( 2.0, 3.0 ) );
        samp[1].setChannelValue( 0, 4.0 );
        samp[1].setChannelValue( 1, 5.0 );
        samp.setLensSqueezeRatio( 2.0 );
        samp.setHorizontalAperture( 4.8 );
        samp.setVerticalFilmOffset( 3.0 );
        camSchema.set( samp );
    }

    {
        M33d identity;
        identity.makeIdentity();

        CameraSample samp;

        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), fileName );
        ICamera simpleCam( IObject( archive, kTop ), "simpleCam" );
        ICamera cam( IObject( archive, kTop ), "cam" );

        // all default data
        simpleCam.getSchema().get( samp );
        TESTING_ASSERT( simpleCam.getSchema().getNumSamples() == 1 );
        TESTING_ASSERT( almostEqual( samp.getFocalLength(), 35.0 ) );
        TESTING_ASSERT( almostEqual( samp.getHorizontalAperture(), 3.6 ) );
        TESTING_ASSERT( almostEqual( samp.getVerticalAperture(), 2.4 ) );
        TESTING_ASSERT( almostEqual( samp.getHorizontalFilmOffset(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getVerticalFilmOffset(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getLensSqueezeRatio(), 1.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanLeft(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanRight(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanTop(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanBottom(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getFStop(), 5.6 ) );
        TESTING_ASSERT( almostEqual( samp.getFocusDistance(), 5.0 ) );
        TESTING_ASSERT( almostEqual( samp.getShutterOpen(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getShutterClose(),
                        0.020833333333333332 ) );
        TESTING_ASSERT( almostEqual( samp.getNearClippingPlane(), 0.1 ) );
        TESTING_ASSERT( almostEqual( samp.getFarClippingPlane(), 100000.0 ) );
        TESTING_ASSERT( samp.getNumOps() == 0 );
        TESTING_ASSERT( samp.getNumOpChannels() == 0 );
        TESTING_ASSERT( samp.getFilmBackMatrix() == identity );

        TESTING_ASSERT( cam.getSchema().getNumSamples() == 2 );

        cam.getSchema().get( samp );
        TESTING_ASSERT( almostEqual( samp.getFocalLength(), 35.0 ) );
        TESTING_ASSERT( almostEqual( samp.getHorizontalAperture(), 3.6 ) );
        TESTING_ASSERT( almostEqual( samp.getVerticalAperture(), 2.4 ) );
        TESTING_ASSERT( almostEqual( samp.getHorizontalFilmOffset(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getVerticalFilmOffset(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getLensSqueezeRatio(), 1.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanLeft(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanRight(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanTop(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanBottom(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getFStop(), 5.6 ) );
        TESTING_ASSERT( almostEqual( samp.getFocusDistance(), 5.0 ) );
        TESTING_ASSERT( almostEqual( samp.getShutterOpen(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getShutterClose(),
                        0.020833333333333332 ) );
        TESTING_ASSERT( almostEqual( samp.getNearClippingPlane(), 0.1 ) );
        TESTING_ASSERT( almostEqual( samp.getFarClippingPlane(), 100000.0 ) );
        TESTING_ASSERT( samp.getNumOps() == 2 );
        TESTING_ASSERT( samp.getNumOpChannels() == 4 );
        TESTING_ASSERT( samp[0].getHint() == "scale" );
        TESTING_ASSERT( samp[0].getType() == kScaleFilmBackOperation );
        TESTING_ASSERT( samp[0].getNumChannels() == 2 );
        TESTING_ASSERT( almostEqual( samp[0].getChannelValue(0), 1.0 ) );
        TESTING_ASSERT( almostEqual( samp[0].getChannelValue(1), 1.0 ) );
        TESTING_ASSERT( samp[1].getHint() == "offset" );
        TESTING_ASSERT( samp[1].getType() == kTranslateFilmBackOperation );
        TESTING_ASSERT( samp[1].getNumChannels() == 2 );
        TESTING_ASSERT( almostEqual( samp[1].getChannelValue(0), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp[1].getChannelValue(1), 0.0 ) );
        TESTING_ASSERT( samp.getFilmBackMatrix() == identity );

        cam.getSchema().get( samp, 1 );
        TESTING_ASSERT( almostEqual( samp.getFocalLength(), 35.0 ) );
        TESTING_ASSERT( almostEqual( samp.getHorizontalAperture(), 4.8 ) );
        TESTING_ASSERT( almostEqual( samp.getVerticalAperture(), 2.4 ) );
        TESTING_ASSERT( almostEqual( samp.getHorizontalFilmOffset(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getVerticalFilmOffset(), 3.0 ) );
        TESTING_ASSERT( almostEqual( samp.getLensSqueezeRatio(), 2.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanLeft(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanRight(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanTop(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getOverScanBottom(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getFStop(), 5.6 ) );
        TESTING_ASSERT( almostEqual( samp.getFocusDistance(), 5.0 ) );
        TESTING_ASSERT( almostEqual( samp.getShutterOpen(), 0.0 ) );
        TESTING_ASSERT( almostEqual( samp.getShutterClose(),
                        0.020833333333333332) );
        TESTING_ASSERT( almostEqual( samp.getNearClippingPlane(), 0.1 ) );
        TESTING_ASSERT( almostEqual( samp.getFarClippingPlane(), 100000.0 ) );
        TESTING_ASSERT( samp.getNumOps() == 2 );
        TESTING_ASSERT( samp.getNumOpChannels() == 4 );
        TESTING_ASSERT( samp[0].getHint() == "scale" );
        TESTING_ASSERT( samp[0].getType() == kScaleFilmBackOperation );
        TESTING_ASSERT( samp[0].getNumChannels() == 2 );
        TESTING_ASSERT( almostEqual( samp[0].getChannelValue(0), 2.0 ) );
        TESTING_ASSERT( almostEqual( samp[0].getChannelValue(1), 3.0 ) );
        TESTING_ASSERT( samp[0].getScale().equalWithAbsError( V2d( 2.0, 3.0 ),
            VAL_EPSILON ) );
        TESTING_ASSERT( samp[1].getHint() == "offset" );
        TESTING_ASSERT( samp[1].getType() == kTranslateFilmBackOperation );
        TESTING_ASSERT( samp[1].getNumChannels() == 2 );
        TESTING_ASSERT( almostEqual( samp[1].getChannelValue(0), 4.0 ) );
        TESTING_ASSERT( almostEqual( samp[1].getChannelValue(1), 5.0 ) );
        TESTING_ASSERT( samp[1].getTranslate().equalWithAbsError(
            V2d( 4.0, 5.0 ), VAL_EPSILON ) );

        M33d m;
        m.makeIdentity();
        m.scale( V2d( 2.0, 3.0 ) );
        m.translate( V2d( 4.0, 5.0 ) );
        TESTING_ASSERT( samp.getFilmBackMatrix().equalWithAbsError( m,
            VAL_EPSILON ) );
    }
}

//-*****************************************************************************
void corePropertiesTest()
{
    std::string fileName = "camera2.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileName );
        CameraSample samp;

        OCamera camObj( OObject( archive, kTop ), "cam" );
        OCameraSchema camSchema = camObj.getSchema();

        // set some nonsense values just to test reading/writing
        for ( std::size_t i = 0; i < 10; ++i )
        {
            samp.setFocalLength( i * 1000.0 + 1.0 );
            samp.setHorizontalAperture( i * 1000.0 + 2.0 );
            samp.setVerticalAperture( i * 1000.0 + 3.0 );
            samp.setHorizontalFilmOffset( i * 1000.0 + 4.0 );
            samp.setVerticalFilmOffset( i * 1000.0 + 5.0 );
            samp.setLensSqueezeRatio( i * 1000.0 + 6.0 );
            samp.setOverScanLeft( i * 1000.0 + 7.0 );
            samp.setOverScanRight( i * 1000.0 + 8.0 );
            samp.setOverScanTop( i * 1000.0 + 9.0 );
            samp.setOverScanBottom( i * 1000.0 + 10.0 );
            samp.setFStop( i * 1000.0 + 11.0 );
            samp.setFocusDistance( i * 1000.0 + 12.0 );
            samp.setShutterOpen( i * 1000.0 + 13.0 );
            samp.setShutterClose( i * 1000.0 + 14.0 );
            samp.setNearClippingPlane( i * 1000.0 + 15.0 );
            samp.setFarClippingPlane( i * 1000.0 + 16.0 );
            camSchema.set( samp );
        }
    }

    {
        M33d identity;
        identity.makeIdentity();

        CameraSample samp;

        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), fileName );
        ICamera cam( IObject( archive, kTop ), "cam" );

        TESTING_ASSERT( cam.getSchema().getNumSamples() == 10 );

        for ( std::size_t i = 0; i < 10; ++i )
        {
            cam.getSchema().get( samp, i );
            TESTING_ASSERT( almostEqual( samp.getFocalLength(),
                1000.0 * i + 1.0 ) );
            TESTING_ASSERT( almostEqual( samp.getHorizontalAperture(),
                1000.0 * i + 2.0 ) );
            TESTING_ASSERT( almostEqual( samp.getVerticalAperture(),
                1000.0 * i + 3.0 ) );
            TESTING_ASSERT( almostEqual( samp.getHorizontalFilmOffset(),
                1000.0 * i + 4.0 ) );
            TESTING_ASSERT( almostEqual( samp.getVerticalFilmOffset(),
                1000.0 * i + 5.0 ) );
            TESTING_ASSERT( almostEqual( samp.getLensSqueezeRatio(),
                1000.0 * i + 6.0 ) );
            TESTING_ASSERT( almostEqual( samp.getOverScanLeft(),
                1000.0 * i + 7.0 ) );
            TESTING_ASSERT( almostEqual( samp.getOverScanRight(),
                1000.0 * i + 8.0 ) );
            TESTING_ASSERT( almostEqual( samp.getOverScanTop(),
                1000.0 * i + 9.0 ) );
            TESTING_ASSERT( almostEqual( samp.getOverScanBottom(),
                1000.0 * i + 10.0 ) );
            TESTING_ASSERT( almostEqual( samp.getFStop(),
                1000.0 * i + 11.0 ) );
            TESTING_ASSERT( almostEqual( samp.getFocusDistance(),
                1000.0 * i + 12.0 ) );
            TESTING_ASSERT( almostEqual( samp.getShutterOpen(),
                1000.0 * i + 13.0 ) );
            TESTING_ASSERT( almostEqual( samp.getShutterClose(),
                1000.0 * i + 14.0 ) );
            TESTING_ASSERT( almostEqual( samp.getNearClippingPlane(),
                1000.0 * i + 15.0 ) );
            TESTING_ASSERT( almostEqual( samp.getFarClippingPlane(),
                1000.0 * i + 16.0 ) );
            TESTING_ASSERT( samp.getNumOps() == 0 );
            TESTING_ASSERT( samp.getNumOpChannels() == 0 );
        }
    }
}

//-*****************************************************************************
void sparseTest()
{
    std::string fileA = "sparseCameraA.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileA );
        CameraSample samp;
        OCamera simpleCamObj( OObject( archive, kTop ), "simpleCam" );
        simpleCamObj.getSchema().set( samp );

        OCamera camObj( OObject( archive, kTop ), "staticCam" );
        OCameraSchema camSchema = camObj.getSchema();
        samp.addOp( FilmBackXformOp( kScaleFilmBackOperation, "scale" ) );
        samp.addOp( FilmBackXformOp( kTranslateFilmBackOperation, "offset" ) );
        camSchema.set( samp );

        OCamera animCamObj( OObject( archive, kTop ), "animCam" );
        OCameraSchema animCamSchema = animCamObj.getSchema();
        animCamSchema.set( samp );

        samp[0].setScale( V2d( 2.0, 3.0 ) );
        samp[1].setChannelValue( 0, 4.0 );
        samp[1].setChannelValue( 1, 5.0 );
        samp.setLensSqueezeRatio( 2.0 );
        samp.setHorizontalAperture( 4.8 );
        samp.setVerticalFilmOffset( 3.0 );
        animCamSchema.set( samp );
    }

    std::string fileB = "sparseCameraB.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), fileB );

        CameraSample samp;
        samp.addOp( FilmBackXformOp( kTranslateFilmBackOperation, "test" ) );
        samp.setLensSqueezeRatio( 5.0 );
        samp.setHorizontalAperture( 4.0 );
        samp.setVerticalFilmOffset( 3.0 );
        samp[0].setChannelValue( 0, 2.0 );
        samp[0].setChannelValue( 1, 1.0 );

        OCamera simpleCamObj( OObject( archive ), "simpleCam", kSparse );
        simpleCamObj.getSchema().set( samp );

        OCamera camObj( OObject( archive  ), "staticCam", kSparse );
        OCameraSchema camSchema = camObj.getSchema();
        camSchema.set( samp );
        samp.setLensSqueezeRatio( 10.0 );
        samp.setHorizontalAperture( 9.0 );
        samp.setVerticalFilmOffset( 8.0 );
        samp[0].setChannelValue( 0, 7.0 );
        samp[0].setChannelValue( 1, 6.0 );
        camSchema.set( samp );

        OCamera animCamObj( OObject( archive ), "animCam", kSparse );
        OCameraSchema animCamSchema = animCamObj.getSchema();
        CameraSample simpleSamp;
        animCamSchema.set( simpleSamp );
    }

    {
        std::vector< std::string > names;
        names.push_back( fileB );
        names.push_back( fileA );
        Alembic::AbcCoreFactory::IFactory factory;
        IArchive archive = factory.getArchive( names );

        ICamera simpleCamObj( IObject( archive ), "simpleCam" );
        CameraSample samp;
        ICameraSchema schema = simpleCamObj.getSchema();
        TESTING_ASSERT( schema.getNumSamples() == 1 );
        schema.get( samp );
        TESTING_ASSERT( samp.getNumOps() == 1 &&
                        samp.getOp(0).getHint() == "test" );

        ICamera staticCamObj( IObject( archive ), "staticCam" );
        schema = staticCamObj.getSchema();
        TESTING_ASSERT( schema.getNumSamples() == 2 );
        schema.get( samp );
        TESTING_ASSERT( samp.getLensSqueezeRatio() == 5.0 &&
                        samp.getHorizontalAperture() == 4.0 &&
                        samp.getVerticalFilmOffset() == 3.0 &&
                        samp[0].getChannelValue( 0 ) == 2.0 &&
                        samp[0].getChannelValue( 1 ) == 1.0 );

        schema.get( samp, 1 );
        TESTING_ASSERT( samp.getLensSqueezeRatio() == 10.0 &&
                        samp.getHorizontalAperture() == 9.0 &&
                        samp.getVerticalFilmOffset() == 8.0 &&
                        samp[0].getChannelValue( 0 ) == 7.0 &&
                        samp[0].getChannelValue( 1 ) == 6.0 );

        ICamera animCamObj( IObject( archive ), "animCam" );
        schema = animCamObj.getSchema();
        schema.get( samp );
        TESTING_ASSERT( schema.getNumSamples() == 1 && samp.getNumOps() == 0 );
    }
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    cameraTest();
    corePropertiesTest();
    sparseTest();
    return 0;
}
