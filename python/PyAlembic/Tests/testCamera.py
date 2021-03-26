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
import unittest
from imath import *
from alembic.AbcCoreAbstract import *
from alembic.Abc import *
from alembic.AbcGeom import *

testList = []

kScaleFilmBackOperation = FilmBackXformOperationType.kScaleFilmBackOperation
kTranslateFilmBackOperation = FilmBackXformOperationType.kTranslateFilmBackOperation

class CameraTest(unittest.TestCase):
    def testCameraExport(self):
        """write out camera archive"""

        archive = OArchive("camera1.abc")
        simpleCamObj = OCamera(archive.getTop(), "simpleCam")
        samp = CameraSample()
        simpleCamObj.getSchema().set(samp)

        camObj = OCamera(archive.getTop(), "cam")
        camSchema = camObj.getSchema()
        samp.addOp(FilmBackXformOp(kScaleFilmBackOperation, "scale"))
        samp.addOp(FilmBackXformOp(kTranslateFilmBackOperation, "offset"))
        self.assertTrue(camObj)
        self.assertTrue(camObj.valid())
        camSchema.set(samp)

        samp[0].setScale(V2d(2.0, 3.0))
        samp[1].setChannelValue(0, 4.0)
        samp[1].setChannelValue(1, 5.0)

        samp.setLensSqueezeRatio(2.0)
        samp.setHorizontalAperture(4.8)
        samp.setVerticalFilmOffset(3.0)
        self.assertEqual(samp[0].getChannelValue(0), 2.0)
        camSchema.set(samp)

    def testCameraImport(self):
        """read in camera archive"""

        archive = IArchive("camera1.abc")
        identity = M33d()
        identity.makeIdentity()

        samp = CameraSample()

        simpleCam = ICamera(archive.getTop(), "simpleCam")
        cam = ICamera(archive.getTop(), "cam")

        self.assertTrue(cam)
        self.assertTrue(cam.valid())

        # test camera "simpleCam" default values
        samp = simpleCam.getSchema().getValue()
        self.assertEqual(simpleCam.getSchema().getNumSamples(), 1)
        self.assertAlmostEqual(samp.getFocalLength(), 35.0)
        self.assertAlmostEqual(samp.getHorizontalAperture(), 3.6)
        self.assertAlmostEqual(samp.getVerticalAperture(), 2.4)
        self.assertAlmostEqual(samp.getHorizontalFilmOffset(), 0.0)
        self.assertAlmostEqual(samp.getVerticalFilmOffset(), 0.0)
        self.assertAlmostEqual(samp.getLensSqueezeRatio(), 1.0)
        self.assertAlmostEqual(samp.getOverScanLeft(), 0.0)
        self.assertAlmostEqual(samp.getOverScanRight(), 0.0)
        self.assertAlmostEqual(samp.getOverScanTop(), 0.0)
        self.assertAlmostEqual(samp.getOverScanBottom(), 0.0)
        self.assertAlmostEqual(samp.getFStop(), 5.6)
        self.assertAlmostEqual(samp.getFocusDistance(), 5.0)
        self.assertAlmostEqual(samp.getShutterOpen(), 0.0)
        self.assertAlmostEqual(samp.getShutterClose(), 0.0208333333333)
        self.assertAlmostEqual(samp.getNearClippingPlane(), 0.1)
        self.assertAlmostEqual(samp.getFarClippingPlane(), 100000.0)
        self.assertEqual(samp.getNumOps(), 0)
        self.assertEqual(samp.getNumOpChannels(), 0)
        self.assertEqual(samp.getFilmBackMatrix(), identity)

        self.assertEqual(cam.getSchema().getNumSamples(), 2)

        # test camera "cam" values at sample 0
        samp = cam.getSchema().getValue()
        self.assertAlmostEqual(samp.getFocalLength(), 35.0)
        self.assertAlmostEqual(samp.getHorizontalAperture(), 3.6)
        self.assertAlmostEqual(samp.getVerticalAperture(), 2.4)
        self.assertAlmostEqual(samp.getHorizontalFilmOffset(), 0.0)
        self.assertAlmostEqual(samp.getVerticalFilmOffset(), 0.0)
        self.assertAlmostEqual(samp.getLensSqueezeRatio(), 1.0)
        self.assertAlmostEqual(samp.getOverScanLeft(), 0.0)
        self.assertAlmostEqual(samp.getOverScanRight(), 0.0)
        self.assertAlmostEqual(samp.getOverScanTop(), 0.0)
        self.assertAlmostEqual(samp.getOverScanBottom(), 0.0)
        self.assertAlmostEqual(samp.getFStop(), 5.6)
        self.assertAlmostEqual(samp.getFocusDistance(), 5.0)
        self.assertAlmostEqual(samp.getShutterOpen(), 0.0)
        self.assertAlmostEqual(samp.getShutterClose(), 0.0208333333333)
        self.assertAlmostEqual(samp.getNearClippingPlane(), 0.1)
        self.assertAlmostEqual(samp.getFarClippingPlane(), 100000.0)
        self.assertEqual(samp.getNumOps(), 2)
        self.assertEqual(samp.getNumOpChannels(), 4)
        self.assertEqual(samp[0].getHint(), "scale")
        self.assertEqual(samp[0].getType(), kScaleFilmBackOperation)
        self.assertEqual(samp[0].getNumChannels(), 2)
        self.assertAlmostEqual(samp[0].getChannelValue(0), 1.0)
        self.assertAlmostEqual(samp[0].getChannelValue(1), 1.0)
        self.assertEqual(samp[1].getHint(), "offset")
        self.assertEqual(samp[1].getType(), kTranslateFilmBackOperation)
        self.assertEqual(samp[1].getNumChannels(), 2)
        self.assertAlmostEqual(samp[1].getChannelValue(0), 0.0)
        self.assertAlmostEqual(samp[1].getChannelValue(1), 0.0)
        self.assertEqual(samp.getFilmBackMatrix(), identity)

        # test cam values at sample 1
        samp = cam.getSchema().getValue(ISampleSelector(1))
        self.assertAlmostEqual(samp.getFocalLength(), 35.0)
        self.assertAlmostEqual(samp.getHorizontalAperture(), 4.8)
        self.assertAlmostEqual(samp.getVerticalAperture(), 2.4)
        self.assertAlmostEqual(samp.getHorizontalFilmOffset(), 0.0)
        self.assertAlmostEqual(samp.getVerticalFilmOffset(), 3.0)
        self.assertAlmostEqual(samp.getLensSqueezeRatio(), 2.0)
        self.assertAlmostEqual(samp.getOverScanLeft(), 0.0)
        self.assertAlmostEqual(samp.getOverScanRight(), 0.0)
        self.assertAlmostEqual(samp.getOverScanTop(), 0.0)
        self.assertAlmostEqual(samp.getOverScanBottom(), 0.0)
        self.assertAlmostEqual(samp.getFStop(), 5.6)
        self.assertAlmostEqual(samp.getFocusDistance(), 5.0)
        self.assertAlmostEqual(samp.getShutterOpen(), 0.0)
        self.assertAlmostEqual(samp.getShutterClose(), 0.0208333333333)
        self.assertAlmostEqual(samp.getNearClippingPlane(), 0.1)
        self.assertAlmostEqual(samp.getFarClippingPlane(), 100000.0)
        self.assertEqual(samp.getNumOps(), 2)
        self.assertEqual(samp.getNumOpChannels(), 4)
        self.assertEqual(samp[0].getHint(), "scale")
        self.assertEqual(samp[0].getType(), kScaleFilmBackOperation)
        self.assertEqual(samp[0].getNumChannels(), 2)
        self.assertAlmostEqual(samp[0].getChannelValue(0), 2.0), samp[0].getChannelValue(0)
        self.assertAlmostEqual(samp[0].getChannelValue(1), 3.0), samp[0].getChannelValue(1)
        self.assertEqual(samp[1].getHint(), "offset")
        self.assertEqual(samp[1].getType(), kTranslateFilmBackOperation)
        self.assertEqual(samp[1].getNumChannels(), 2)
        self.assertAlmostEqual(samp[1].getChannelValue(0), 4.0)
        self.assertAlmostEqual(samp[1].getChannelValue(1), 5.0)
