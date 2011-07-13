#
# Copyright (c) 2010 Sony Pictures Imageworks Inc.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the
# distribution. Neither the name of Sony Pictures Imageworks nor the
# names of its contributors may be used to endorse or promote
# products derived from this software without specific prior written
# permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.

from maya import cmds as MayaCmds
from maya import mel as Mel
import os
import unittest
import util

def makeRobot():
    MayaCmds.polyCube(name="head")
    MayaCmds.move(0, 4, 0, r=1)

    MayaCmds.polyCube(name="chest")
    MayaCmds.scale(2, 2.5, 1)
    MayaCmds.move(0, 2, 0, r=1)
    MayaCmds.polyCube(name="leftArm")
    MayaCmds.move(0, 3, 0, r=1)
    MayaCmds.scale(2, 0.5, 1, r=1)
    MayaCmds.duplicate(name="rightArm")
    MayaCmds.select("leftArm")
    MayaCmds.move(1.25, 0, 0, r=1)
    MayaCmds.rotate(0, 0, 32, r=1, os=1)
    MayaCmds.select("rightArm")
    MayaCmds.move(-1.25, 0, 0, r=1)
    MayaCmds.rotate(0, 0, -32, r=1, os=1)
    MayaCmds.select("rightArm", "leftArm", "chest", r=1)
    MayaCmds.group(name="body")

    MayaCmds.polyCube(name="bottom")
    MayaCmds.scale(2, 0.5, 1)
    MayaCmds.move(0, 0.5, 0, r=1)
    MayaCmds.polyCube(name="leftLeg")
    MayaCmds.scale(0.65, 2.8, 1, r=1)
    MayaCmds.move(-0.5, -1, 0, r=1)
    MayaCmds.duplicate(name="rightLeg")
    MayaCmds.move(1, 0, 0, r=1)
    MayaCmds.select("rightLeg", "leftLeg", "bottom", r=1)
    MayaCmds.group(name="lower")

    MayaCmds.select("head", "body", "lower", r=1)
    MayaCmds.group(name="robot")

def makeRobotAnimated():
    makeRobot()

    #change pivot point of arms and legs
    MayaCmds.move(0.65, -0.40, 0, 'rightArm.scalePivot',
        'rightArm.rotatePivot', relative=True)
    MayaCmds.move(-0.65, -0.40, 0, 'leftArm.scalePivot', 'leftArm.rotatePivot',
        relative=True)
    MayaCmds.move(0, 1.12, 0, 'rightLeg.scalePivot', 'rightLeg.rotatePivot',
        relative=True)
    MayaCmds.move(0, 1.12, 0, 'leftLeg.scalePivot', 'leftLeg.rotatePivot',
        relative=True)

    MayaCmds.setKeyframe('leftLeg', at='rotateX', value=25, t=[1, 12])
    MayaCmds.setKeyframe('leftLeg', at='rotateX', value=-40, t=[6])
    MayaCmds.setKeyframe('rightLeg', at='scaleY', value=2.8, t=[1, 12])
    MayaCmds.setKeyframe('rightLeg', at='scaleY', value=5.0, t=[6])

    MayaCmds.setKeyframe('leftArm', at='rotateZ', value=55, t=[1, 12])
    MayaCmds.setKeyframe('leftArm', at='rotateZ', value=-50, t=[6])
    MayaCmds.setKeyframe('rightArm', at='scaleX', value=0.5, t=[1, 12])
    MayaCmds.setKeyframe('rightArm', at='scaleX', value=3.6, t=[6])

class AbcNodeNameTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files :
            os.remove(f)

    def testReturnAbcNodeName(self):
        makeRobotAnimated()

        self.__files.append(util.expandFileName('returnAlembicNodeNameTest.abc'))
        MayaCmds.AbcExport(j='-fr 1 12 -root robot -file ' + self.__files[-1])

        ret = MayaCmds.AbcImport(self.__files[-1], mode='open')
        ret1 = MayaCmds.AbcImport(self.__files[-1], mode='import')
        self.failUnless(MayaCmds.objExists(ret))
        self.failUnless(MayaCmds.objExists(ret1))
        self.failIf(ret == ret1)
