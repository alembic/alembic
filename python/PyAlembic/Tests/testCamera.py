#!/usr/bin/env python2_zeno

#   Copyright (c) 2011-2012 Industrial   Light   and   Magic.
#   All   rights   reserved.    Used   under   authorization.
#   This material contains the confidential  and  proprietary
#   information   of   Industrial   Light   and   Magic   and
#   may not be copied in whole or in part without the express
#   written   permission   of  Industrial Light  and   Magic.
#   This  copyright  notice  does  not   imply   publication.

import imath
import alembic
import traceback

testList = []

kScaleFilmBackOperation = alembic.AbcG.FilmBackXformOperationType.kScaleFilmBackOperation
kTranslateFilmBackOperation = alembic.AbcG.FilmBackXformOperationType.kTranslateFilmBackOperation

def almostEqual(a0, a1, error=0.01):
    return abs(a0 - a1) <= error

def cameraOut():
    """write out camera archive"""

    archive = alembic.Abc.OArchive("camera1.abc")
    simpleCamObj = alembic.AbcG.OCamera(archive.getTop(), "simpleCam")
    samp = alembic.AbcG.CameraSample()
    simpleCamObj.getSchema().set(samp)

    camObj = alembic.AbcG.OCamera(archive.getTop(), "cam")
    camSchema = camObj.getSchema()
    samp.addOp(alembic.AbcG.FilmBackXformOp(kScaleFilmBackOperation, "scale"))
    samp.addOp(alembic.AbcG.FilmBackXformOp(kTranslateFilmBackOperation, "offset"))
    camSchema.set(samp)

    samp[0].setScale(imath.V2d(2.0, 3.0))
    samp[1].setChannelValue(0, 4.0)
    samp[1].setChannelValue(1, 5.0)

    samp.setLensSqueezeRatio(2.0)
    samp.setHorizontalAperture(4.8)
    samp.setVerticalFilmOffset(3.0)
    assert samp[0].getChannelValue(0) == 2.0
    camSchema.set(samp)

def cameraIn():
    """read in camera archive"""

    archive = alembic.Abc.IArchive("camera1.abc")
    identity = imath.M33d()
    identity.makeIdentity()

    samp = alembic.AbcG.CameraSample()

    simpleCam = alembic.AbcG.ICamera(archive.getTop(), "simpleCam")
    cam = alembic.AbcG.ICamera(archive.getTop(), "cam")

    # test camera "simpleCam" default values
    samp = simpleCam.getSchema().getValue()
    assert simpleCam.getSchema().getNumSamples() == 1 
    assert almostEqual(samp.getFocalLength(), 35.0) 
    assert almostEqual(samp.getHorizontalAperture(), 3.6) 
    assert almostEqual(samp.getVerticalAperture(), 2.4) 
    assert almostEqual(samp.getHorizontalFilmOffset(), 0.0) 
    assert almostEqual(samp.getVerticalFilmOffset(), 0.0) 
    assert almostEqual(samp.getLensSqueezeRatio(), 1.0) 
    assert almostEqual(samp.getOverScanLeft(), 0.0) 
    assert almostEqual(samp.getOverScanRight(), 0.0) 
    assert almostEqual(samp.getOverScanTop(), 0.0) 
    assert almostEqual(samp.getOverScanBottom(), 0.0) 
    assert almostEqual(samp.getFStop(), 5.6) 
    assert almostEqual(samp.getFocusDistance(), 5.0) 
    assert almostEqual(samp.getShutterOpen(), 0.0) 
    assert almostEqual(samp.getShutterClose(), 0.0208333333333) 
    assert almostEqual(samp.getNearClippingPlane(), 0.1) 
    assert almostEqual(samp.getFarClippingPlane(), 100000.0) 
    assert samp.getNumOps() == 0 
    assert samp.getNumOpChannels() == 0 
    assert samp.getFilmBackMatrix() == identity

    assert cam.getSchema().getNumSamples() == 2 

    # tes camera "cam" values at sample 0
    samp = cam.getSchema().getValue()
    assert almostEqual(samp.getFocalLength(), 35.0) 
    assert almostEqual(samp.getHorizontalAperture(), 3.6) 
    assert almostEqual(samp.getVerticalAperture(), 2.4) 
    assert almostEqual(samp.getHorizontalFilmOffset(), 0.0) 
    assert almostEqual(samp.getVerticalFilmOffset(), 0.0) 
    assert almostEqual(samp.getLensSqueezeRatio(), 1.0) 
    assert almostEqual(samp.getOverScanLeft(), 0.0) 
    assert almostEqual(samp.getOverScanRight(), 0.0) 
    assert almostEqual(samp.getOverScanTop(), 0.0) 
    assert almostEqual(samp.getOverScanBottom(), 0.0) 
    assert almostEqual(samp.getFStop(), 5.6) 
    assert almostEqual(samp.getFocusDistance(), 5.0) 
    assert almostEqual(samp.getShutterOpen(), 0.0) 
    assert almostEqual(samp.getShutterClose(), 0.0208333333333) 
    assert almostEqual(samp.getNearClippingPlane(), 0.1) 
    assert almostEqual(samp.getFarClippingPlane(), 100000.0) 
    assert samp.getNumOps() == 2 
    assert samp.getNumOpChannels() == 4 
    assert samp[0].getHint() == "scale" 
    assert samp[0].getType() == kScaleFilmBackOperation 
    assert samp[0].getNumChannels() == 2 
    assert almostEqual(samp[0].getChannelValue(0), 1.0) 
    assert almostEqual(samp[0].getChannelValue(1), 1.0) 
    assert samp[1].getHint() == "offset" 
    assert samp[1].getType() == kTranslateFilmBackOperation 
    assert samp[1].getNumChannels() == 2 
    assert almostEqual(samp[1].getChannelValue(0), 0.0) 
    assert almostEqual(samp[1].getChannelValue(1), 0.0) 
    assert samp.getFilmBackMatrix() == identity 

    # test cam values at sample 1
    samp = cam.getSchema().getValue(alembic.Abc.ISampleSelector(1))
    assert almostEqual(samp.getFocalLength(), 35.0) 
    assert almostEqual(samp.getHorizontalAperture(), 4.8) 
    assert almostEqual(samp.getVerticalAperture(), 2.4) 
    assert almostEqual(samp.getHorizontalFilmOffset(), 0.0) 
    assert almostEqual(samp.getVerticalFilmOffset(), 3.0) 
    assert almostEqual(samp.getLensSqueezeRatio(), 2.0) 
    assert almostEqual(samp.getOverScanLeft(), 0.0) 
    assert almostEqual(samp.getOverScanRight(), 0.0) 
    assert almostEqual(samp.getOverScanTop(), 0.0) 
    assert almostEqual(samp.getOverScanBottom(), 0.0) 
    assert almostEqual(samp.getFStop(), 5.6) 
    assert almostEqual(samp.getFocusDistance(), 5.0) 
    assert almostEqual(samp.getShutterOpen(), 0.0) 
    assert almostEqual(samp.getShutterClose(), 0.0208333333333) 
    assert almostEqual(samp.getNearClippingPlane(), 0.1) 
    assert almostEqual(samp.getFarClippingPlane(), 100000.0) 
    assert samp.getNumOps() == 2 
    assert samp.getNumOpChannels() == 4 
    assert samp[0].getHint() == "scale" 
    assert samp[0].getType() == kScaleFilmBackOperation 
    assert samp[0].getNumChannels() == 2 
    assert almostEqual(samp[0].getChannelValue(0), 2.0), samp[0].getChannelValue(0) 
    assert almostEqual(samp[0].getChannelValue(1), 3.0), samp[0].getChannelValue(1) 
    assert samp[1].getHint() == "offset" 
    assert samp[1].getType() == kTranslateFilmBackOperation 
    assert samp[1].getNumChannels() == 2 
    assert almostEqual(samp[1].getChannelValue(0), 4.0)
    assert almostEqual(samp[1].getChannelValue(1), 5.0)

def testCameraBinding():
    cameraOut()
    cameraIn()

testList.append(('testCameraBinding', testCameraBinding))

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""

