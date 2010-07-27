
###############################################################################
#
# Copyright (c) 2009-2010,
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
###############################################################################

import maya.cmds as MayaCmds
import os
import subprocess
import unittest

class animLightTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__takoStitcher = [os.environ['AlembicStitcher']]
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testAnimAmbientLight(self):

        # ambient light
        light = MayaCmds.ambientLight(intensity=0.8)
        root = MayaCmds.group(light, name="lightTest")

        intensity = MayaCmds.getAttr(light+'.intensity')
        color = MayaCmds.getAttr(light+'.color')
        centerOfIllumination = MayaCmds.getAttr(light+'.centerOfIllumination')
        decayRate = 0

        # setting keys
        MayaCmds.setKeyframe(light, at='intensity', time=[1, 24])
        MayaCmds.setKeyframe(light, at='intensity', value=2, time=12)

        self.__files.append('/tmp/testAnimAmbientLight.hdf')
        self.__files.append('/tmp/testAnimAmbientLight01_14.hdf')
        self.__files.append('/tmp/testAnimAmbientLight15_24.hdf')

        MayaCmds.AlembicTakoExport('range  1 14 lightTest ' + self.__files[-2])
        MayaCmds.AlembicTakoExport('range 15 24 lightTest ' + self.__files[-1])

        # use AlembicStitcher to combine two files into one
        subprocess.call(self.__takoStitcher + self.__files[-3:])

        # reading test
        MayaCmds.AlembicTakoImport(self.__files[-3], mode='open', timeRange=[1, 24])

        lightShape = MayaCmds.ls(exactType='pointLight')
        MayaCmds.select(lightShape[0])
        light = MayaCmds.pickWalk(d='up')

        MayaCmds.currentTime(1, update=True)
        self.failUnlessEqual(intensity,
            MayaCmds.getAttr(light[0]+'.intensity'))

        self.failUnlessEqual(color, MayaCmds.getAttr(light[0]+'.color'))
        self.failUnlessEqual(centerOfIllumination,
            MayaCmds.getAttr(light[0]+'.centerOfIllumination'))
        self.failUnlessEqual(decayRate, 0)

        MayaCmds.currentTime(12, update=True)
        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(2, MayaCmds.getAttr(light[0]+'.intensity'))

        MayaCmds.currentTime(24, update=True)
        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(intensity,
            MayaCmds.getAttr(light[0]+'.intensity'))

    def testAnimPointLight(self):

        # point light
        light = MayaCmds.pointLight(intensity=0.7)
        root = MayaCmds.group(light, name="lightTest")

        intensity = MayaCmds.getAttr(light+'.intensity')
        color = MayaCmds.getAttr(light+'.color')
        centerOfIllumination = MayaCmds.getAttr(light+'.centerOfIllumination')
        decayRate = MayaCmds.getAttr(light+'.decayRate')

        # setting keys
        MayaCmds.setKeyframe(light, at='intensity', time=[1, 24])
        MayaCmds.setKeyframe(light, at='intensity', value=2, time=12)

        self.__files.append('/tmp/testAnimPointLight.hdf')
        self.__files.append('/tmp/testAnimPointLight01_14.hdf')
        self.__files.append('/tmp/testAnimPointLight15_24.hdf')

        MayaCmds.AlembicTakoExport('range  1 14 lightTest ' + self.__files[-2])
        MayaCmds.AlembicTakoExport('range 15 24 lightTest ' + self.__files[-1])

        # use AlembicStitcher to combine two files into one
        subprocess.call(self.__takoStitcher + self.__files[-3:])

        # reading test
        MayaCmds.AlembicTakoImport(self.__files[-3], mode='open', timeRange=[1, 24])

        lightShape = MayaCmds.ls(exactType='pointLight')
        MayaCmds.select(lightShape[0])
        light = MayaCmds.pickWalk(d='up')

        MayaCmds.currentTime(1, update=True)

        self.failUnlessEqual(intensity,
            MayaCmds.getAttr(light[0]+'.intensity'))

        self.failUnlessEqual(color, MayaCmds.getAttr(light[0]+'.color'))
        self.failUnlessEqual(centerOfIllumination,
            MayaCmds.getAttr(light[0]+'.centerOfIllumination'))
        self.failUnlessEqual(decayRate,
            MayaCmds.getAttr(light[0]+'.decayRate'))

        MayaCmds.currentTime(12, update=True)
        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(2, MayaCmds.getAttr(light[0]+'.intensity'))

        MayaCmds.currentTime(24, update=True)
        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(intensity,
            MayaCmds.getAttr(light[0]+'.intensity'))

    def testAnimDirectionalLight(self):

        # directional light
        light = MayaCmds.pointLight(intensity=0.7)
        root = MayaCmds.group(light, name="lightTest")

        intensity = MayaCmds.getAttr(light+'.intensity')
        color = MayaCmds.getAttr(light+'.color')
        centerOfIllumination = MayaCmds.getAttr(light+'.centerOfIllumination')
        decayRate = MayaCmds.getAttr(light+'.decayRate')

        # setting keys
        MayaCmds.setKeyframe(light, at='intensity', time=[1, 24])
        MayaCmds.setKeyframe(light, at='intensity', value=2, time=12)

        self.__files.append('/tmp/testAnimDirectionalLight.hdf')
        self.__files.append('/tmp/testAnimDirectionalLight01_14.hdf')
        self.__files.append('/tmp/testAnimDirectionalLight15_24.hdf')

        MayaCmds.AlembicTakoExport('range  1 14 lightTest ' + self.__files[-2])
        MayaCmds.AlembicTakoExport('range 15 24 lightTest ' + self.__files[-1])

        # use AlembicStitcher to combine two files into one
        subprocess.call(self.__takoStitcher + self.__files[-3:])

        # reading test
        MayaCmds.AlembicTakoImport(self.__files[-3], mode='open', timeRange=[1, 24])

        lightShape = MayaCmds.ls(exactType='pointLight')
        MayaCmds.select(lightShape[0])
        light = MayaCmds.pickWalk(d='up')

        MayaCmds.currentTime(1, update=True)

        self.failUnlessEqual(intensity,
            MayaCmds.getAttr(light[0]+'.intensity'))
        self.failUnlessEqual(color, MayaCmds.getAttr(light[0]+'.color'))
        self.failUnlessEqual(centerOfIllumination,
            MayaCmds.getAttr(light[0]+'.centerOfIllumination'))
        self.failUnlessEqual(decayRate,
            MayaCmds.getAttr(light[0]+'.decayRate'))

        MayaCmds.currentTime(12, update=True)
        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(2, MayaCmds.getAttr(light[0]+'.intensity'))

        MayaCmds.currentTime(24, update=True)
        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(intensity,
            MayaCmds.getAttr(light[0]+'.intensity'))

#-------------------------------------------------------------------------
    def testAnimSpotLight(self):

        # spot light
        light = MayaCmds.spotLight(intensity=0.6, decayRate=2,
            rgb=(0.34, 0.866, 1.0))
        root = MayaCmds.group(light, name="lightTest")

        intensity = MayaCmds.getAttr(light+'.intensity')
        color = MayaCmds.getAttr(light+'.color')
        centerOfIllumination = MayaCmds.getAttr(light+'.centerOfIllumination')
        decayRate = MayaCmds.getAttr(light+'.decayRate')
        coneAngle   = MayaCmds.getAttr(light+'.coneAngle')
        penumbraAngle   = MayaCmds.getAttr(light+'.penumbraAngle')
        dropOff     = MayaCmds.getAttr(light+'.dropoff')
        barnDoors   = MayaCmds.getAttr(light+'.barnDoors')
        leftBarnDoor    = MayaCmds.getAttr(light+'.leftBarnDoor')
        rightBarnDoor   = MayaCmds.getAttr(light+'.rightBarnDoor')
        topBarnDoor = MayaCmds.getAttr(light+'.topBarnDoor')
        bottomBarnDoor  = MayaCmds.getAttr(light+'.bottomBarnDoor')
        useDecayRegions = MayaCmds.getAttr(light+'.useDecayRegions')
        startDistance1  = MayaCmds.getAttr(light+'.startDistance1')
        startDistance2  = MayaCmds.getAttr(light+'.startDistance2')
        startDistance3  = MayaCmds.getAttr(light+'.startDistance3')
        endDistance1 = MayaCmds.getAttr(light+'.endDistance1')
        endDistance2 = MayaCmds.getAttr(light+'.endDistance2')
        endDistance3 = MayaCmds.getAttr(light+'.endDistance3')

        # setting keys
        MayaCmds.setKeyframe(light, at='intensity', time=[1, 24])
        MayaCmds.setKeyframe(light, at='intensity', value=2, time=12)

        self.__files.append('/tmp/testAnimSpotLight.hdf')
        self.__files.append('/tmp/testAnimSpotLight01_14.hdf')
        self.__files.append('/tmp/testAnimSpotLight15_24.hdf')

        MayaCmds.AlembicTakoExport('range  1 14 lightTest ' + self.__files[-2])
        MayaCmds.AlembicTakoExport('range 15 24 lightTest ' + self.__files[-1])

        # use AlembicStitcher to combine two files into one
        subprocess.call(self.__takoStitcher + self.__files[-3:])

        # reading test
        MayaCmds.AlembicTakoImport(self.__files[-3], mode='open', timeRange=[1, 24])

        MayaCmds.currentTime(1, update=True)
        self.failUnlessEqual(intensity, MayaCmds.getAttr(light+'.intensity'))
        self.failUnlessEqual(color, MayaCmds.getAttr(light+'.color'))
        self.failUnlessEqual(centerOfIllumination,
            MayaCmds.getAttr(light+'.centerOfIllumination'))
        self.failUnlessEqual(decayRate, MayaCmds.getAttr(light+'.decayRate'))
        self.failUnlessEqual(coneAngle, MayaCmds.getAttr(light+'.coneAngle'))
        self.failUnlessEqual(penumbraAngle,
            MayaCmds.getAttr(light+'.penumbraAngle'))
        self.failUnlessEqual(dropOff, MayaCmds.getAttr(light+'.dropoff'))
        self.failUnlessEqual(barnDoors, MayaCmds.getAttr(light+'.barnDoors'))
        self.failUnlessEqual(leftBarnDoor,
            MayaCmds.getAttr(light+'.leftBarnDoor'))
        self.failUnlessEqual(rightBarnDoor,
            MayaCmds.getAttr(light+'.rightBarnDoor'))
        self.failUnlessEqual(topBarnDoor,
            MayaCmds.getAttr(light+'.topBarnDoor'))
        self.failUnlessEqual(bottomBarnDoor,
            MayaCmds.getAttr(light+'.bottomBarnDoor'))
        self.failUnlessEqual(useDecayRegions,
            MayaCmds.getAttr(light+'.useDecayRegions'))
        self.failUnlessEqual(startDistance1,
            MayaCmds.getAttr(light+'.startDistance1'))
        self.failUnlessEqual(startDistance2,
            MayaCmds.getAttr(light+'.startDistance2'))
        self.failUnlessEqual(startDistance3,
            MayaCmds.getAttr(light+'.startDistance3'))
        self.failUnlessEqual(endDistance1,
            MayaCmds.getAttr(light+'.endDistance1'))
        self.failUnlessEqual(endDistance2,
            MayaCmds.getAttr(light+'.endDistance2'))
        self.failUnlessEqual(endDistance3,
            MayaCmds.getAttr(light+'.endDistance3'))

        MayaCmds.currentTime(12, update=True)
        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(2, MayaCmds.getAttr(light+'.intensity'))

        MayaCmds.currentTime(24, update=True)
        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(intensity, MayaCmds.getAttr(light+'.intensity'))
