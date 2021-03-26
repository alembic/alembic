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
from alembic.Abc import *
from alembic.AbcGeom import *

testList = []

kConstantScope = GeometryScope.kConstantScope

class LightTest(unittest.TestCase):

    def testLighExport(self):
        """write out light archive"""

        archive = OArchive("light1.abc")
        emptyLightObj = OLight(archive.getTop(), "emptyLight")
        lightObj = OLight(archive.getTop(), "myLight" )

        samp = CameraSample()
        lightObj.getSchema().setCameraSample( samp )

        samp = CameraSample( -0.35, 0.75, 0.1, 0.5 )
        lightObj.getSchema().getChildBoundsProperty().setValue(
            Box3d( V3d( 0.0, 0.1, 0.2 ), V3d( 0.3, 0.4, 0.5 ) ) )

        lightObj.getSchema().setCameraSample( samp )

        arg = lightObj.getSchema().getArbGeomParams()
        param = OFloatGeomParam( arg, "test", False, kConstantScope, 1 )
        user = lightObj.getSchema().getUserProperties()
        OFloatProperty( user, "test" )

    def testLightImport(self):
        """read in light archive"""

        archive = IArchive("light1.abc")
        emptyLightObj = ILight(archive.getTop(), "emptyLight" )
        lightObj = ILight(archive.getTop(), "myLight" )

        self.assertFalse(emptyLightObj.getSchema().getArbGeomParams().valid())
        self.assertFalse(emptyLightObj.getSchema().getUserProperties().valid())
        self.assertEqual(lightObj.getSchema().getArbGeomParams().getNumProperties(), 1)
        self.assertEqual(lightObj.getSchema().getUserProperties().getNumProperties(), 1)

        samp = lightObj.getSchema().getCameraSchema().getValue( 0 )
        window = samp.getScreenWindow();
        self.assertAlmostEqual( window['top'], 0.666666666666667 )
        self.assertAlmostEqual( window['bottom'], -0.666666666666667 )
        self.assertAlmostEqual( window['left'], -1.0 )
        self.assertAlmostEqual( window['right'], 1.0 )

        samp = lightObj.getSchema().getCameraSchema().getValue( 1 )
        window = samp.getScreenWindow();
        self.assertAlmostEqual( window['top'], -0.35 )
        self.assertAlmostEqual( window['bottom'], 0.75 )
        self.assertAlmostEqual( window['left'], 0.1 )
        self.assertAlmostEqual( window['right'], 0.5 )

        self.assertFalse(lightObj.getSchema().getCameraSchema().getChildBoundsProperty().valid())
