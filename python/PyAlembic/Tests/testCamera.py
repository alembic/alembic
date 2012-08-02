#-******************************************************************************
#
# Copyright (c) 2012,
#  Sony Pictures Imageworks Inc. and
#  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# *       Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# *       Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
# *       Neither the name of Sony Pictures Imageworks, nor
# Industrial Light & Magic, nor the names of their contributors may be used
# to endorse or promote products derived from this software without specific
# prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#-******************************************************************************

from imath import *
from alembic.AbcCoreAbstract import *
from alembic.Abc import *
from alembic.AbcGeom import *

testList = []

kScaleFilmBackOperation = FilmBackXformOperationType.kScaleFilmBackOperation
kTranslateFilmBackOperation = FilmBackXformOperationType.kTranslateFilmBackOperation

def almostEqual(a0, a1, error=0.01):
    return abs(a0 - a1) <= error

def cameraOut():
    """write out camera archive"""

    archive = OArchive("camera1.abc")
    simpleCamObj = OCamera(archive.getTop(), "simpleCam")
    samp = CameraSample()
    simpleCamObj.getSchema().set(samp)

    camObj = OCamera(archive.getTop(), "cam")
    camSchema = camObj.getSchema()
    samp.addOp(FilmBackXformOp(kScaleFilmBackOperation, "scale"))
    samp.addOp(FilmBackXformOp(kTranslateFilmBackOperation, "offset"))
    camSchema.set(samp)

    samp[0].setScale(V2d(2.0, 3.0))
    samp[1].setChannelValue(0, 4.0)
    samp[1].setChannelValue(1, 5.0)

    samp.setLensSqueezeRatio(2.0)
    samp.setHorizontalAperture(4.8)
    samp.setVerticalFilmOffset(3.0)
    assert samp[0].getChannelValue(0) == 2.0
    camSchema.set(samp)

def cameraIn():
    """read in camera archive"""

    archive = IArchive("camera1.abc")
    identity = M33d()
    identity.makeIdentity()

    samp = CameraSample()

    simpleCam = ICamera(archive.getTop(), "simpleCam")
    cam = ICamera(archive.getTop(), "cam")

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
    samp = cam.getSchema().getValue(ISampleSelector(1))
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
