##-*****************************************************************************
##
## Copyright (c) 2009-2011,
##  Sony Pictures Imageworks, Inc. and
##  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
##
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are
## met:
## *       Redistributions of source code must retain the above copyright
## notice, this list of conditions and the following disclaimer.
## *       Redistributions in binary form must reproduce the above
## copyright notice, this list of conditions and the following disclaimer
## in the documentation and/or other materials provided with the
## distribution.
## *       Neither the name of Sony Pictures Imageworks, nor
## Industrial Light & Magic nor the names of their contributors may be used
## to endorse or promote products derived from this software without specific
## prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
## OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##-*****************************************************************************

import maya.cmds as MayaCmds
import os
import unittest
import util

#
# Test AbcExport [-eulerFilter]
# Note that the Euler Filter is applied to the sampled curve instead of
# the original anim curve.
# The result would be different than applying Euler Filter in Graph Editor.
#
class EulerFilterTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        MayaCmds.file(new=True, force=True)
        for f in self.__files:
            os.remove(f)

    def testEulerFilter_RotateY(self):
        nodeName = MayaCmds.createNode('transform', n = 'test')

        # rotate Y
        MayaCmds.setKeyframe('test', attribute = 'rotateY', value = 10, t = 1)
        MayaCmds.setKeyframe('test', attribute = 'rotateY', value = 20, t = 2)
        MayaCmds.setKeyframe('test', attribute = 'rotateY', value = -330, t = 3)
        MayaCmds.setKeyframe('test', attribute = 'rotateY', value = -320, t = 4)
        MayaCmds.setKeyframe('test', attribute = 'rotateY', value = 50, t = 5)
        MayaCmds.setKeyframe('test', attribute = 'rotateY', value = -300, t = 6)
        MayaCmds.setKeyframe('test', attribute = 'rotateY', value = -290, t = 7)
        MayaCmds.setKeyframe('test', attribute = 'rotateY', value = -280, t = 8)

        self.__files.append(util.expandFileName('testEulerFilter_RotY.abc'))
        self.__files.append(util.expandFileName('testEulerFilter_RotY_EulerFiltered.abc'))

        MayaCmds.AbcExport(j = '-fr 1 8 -root test -file ' + self.__files[-2])
        MayaCmds.AbcExport(j = '-fr 1 8 -root test -eulerFilter -file ' + self.__files[-1])

        # Check file without euler filter
        MayaCmds.AbcImport(self.__files[-2], mode='open')

        MayaCmds.currentTime(1, update = True)
        self.failUnlessAlmostEqual(10, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(2, update = True)
        self.failUnlessAlmostEqual(20, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(3, update = True)
        self.failUnlessAlmostEqual(-330, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(4, update = True)
        self.failUnlessAlmostEqual(-320, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(5, update = True)
        self.failUnlessAlmostEqual(50, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(6, update = True)
        self.failUnlessAlmostEqual(-300, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(7, update = True)
        self.failUnlessAlmostEqual(-290, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(8, update = True)
        self.failUnlessAlmostEqual(-280, MayaCmds.getAttr('test.rotateY'))

        # Check file with euler filter
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        MayaCmds.currentTime(1, update = True)
        self.failUnlessAlmostEqual(10, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(2, update = True)
        self.failUnlessAlmostEqual(20, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(3, update = True)
        self.failUnlessAlmostEqual(30, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(4, update = True)
        self.failUnlessAlmostEqual(40, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(5, update = True)
        self.failUnlessAlmostEqual(50, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(6, update = True)
        self.failUnlessAlmostEqual(60, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(7, update = True)
        self.failUnlessAlmostEqual(70, MayaCmds.getAttr('test.rotateY'))

        MayaCmds.currentTime(8, update = True)
        self.failUnlessAlmostEqual(80, MayaCmds.getAttr('test.rotateY'))


    def testEulerFilter_Rotate(self):
        nodeName = MayaCmds.createNode('transform', n = 'test')

        # Rotate
        MayaCmds.setKeyframe('test', attribute = 'rotateX', value = 90, t = 1)
        MayaCmds.setKeyframe('test', attribute = 'rotateX', value = 460, t = 2)
        MayaCmds.setKeyframe('test', attribute = 'rotateX', value = 110, t = 3)
        MayaCmds.setKeyframe('test', attribute = 'rotateY', value = -10, t = 1)
        MayaCmds.setKeyframe('test', attribute = 'rotateY', value = -40, t = 2)
        MayaCmds.setKeyframe('test', attribute = 'rotateY', value = 290, t = 3)
        MayaCmds.setKeyframe('test', attribute = 'rotateZ', value = 270, t = 1)
        MayaCmds.setKeyframe('test', attribute = 'rotateZ', value = 0, t = 2)
        MayaCmds.setKeyframe('test', attribute = 'rotateZ', value = 810, t = 3)

        # Rotate Order
        MayaCmds.setAttr('test.rotateOrder', 1)

        # Rotate Axis
        MayaCmds.setKeyframe('test', attribute = 'rotateAxisX', value = 90, t = 1)
        MayaCmds.setKeyframe('test', attribute = 'rotateAxisX', value = 460, t = 2)
        MayaCmds.setKeyframe('test', attribute = 'rotateAxisX', value = 110, t = 3)
        MayaCmds.setKeyframe('test', attribute = 'rotateAxisY', value = -10, t = 1)
        MayaCmds.setKeyframe('test', attribute = 'rotateAxisY', value = -40, t = 2)
        MayaCmds.setKeyframe('test', attribute = 'rotateAxisY', value = 290, t = 3)
        MayaCmds.setKeyframe('test', attribute = 'rotateAxisZ', value = 270, t = 1)
        MayaCmds.setKeyframe('test', attribute = 'rotateAxisZ', value = 0, t = 2)
        MayaCmds.setKeyframe('test', attribute = 'rotateAxisZ', value = 810, t = 3)

        self.__files.append(util.expandFileName('testEulerFilter_Rotate.abc'))
        self.__files.append(util.expandFileName('testEulerFilter_Rotate_EulerFiltered.abc'))

        MayaCmds.AbcExport(j = '-fr 1 3 -root test -file ' + self.__files[-2])
        MayaCmds.AbcExport(j = '-fr 1 3 -root test -eulerFilter -file ' + self.__files[-1])

        # Check file without euler filter
        # Maya uses quaternion to represent Rotate Axis and Joint Orient internally.
        # The number from rotateAxis and jointOrient attribute might not be the same
        # as the values from their anim curves.
        MayaCmds.AbcImport(self.__files[-2], mode='open')
        MayaCmds.currentTime(3, update = True)  # refresh

        MayaCmds.currentTime(1, update = True)
        self.assertEqual(1, MayaCmds.getAttr('test.rotateOrder'))
        self.failUnlessAlmostEqual(90, MayaCmds.getAttr('test.rotateX'))
        self.failUnlessAlmostEqual(-10, MayaCmds.getAttr('test.rotateY'))
        self.failUnlessAlmostEqual(270, MayaCmds.getAttr('test.rotateZ'))
        self.failUnlessAlmostEqual(90, MayaCmds.getAttr('test.rotateAxisX'))
        self.failUnlessAlmostEqual(-10, MayaCmds.getAttr('test.rotateAxisY'))
        self.failUnlessAlmostEqual(-90, MayaCmds.getAttr('test.rotateAxisZ'))

        MayaCmds.currentTime(2, update = True)
        self.assertEqual(1, MayaCmds.getAttr('test.rotateOrder'))
        self.failUnlessAlmostEqual(460, MayaCmds.getAttr('test.rotateX'))
        self.failUnlessAlmostEqual(-40, MayaCmds.getAttr('test.rotateY'))
        self.failUnlessAlmostEqual(0, MayaCmds.getAttr('test.rotateZ'))
        self.failUnlessAlmostEqual(100, MayaCmds.getAttr('test.rotateAxisX'))
        self.failUnlessAlmostEqual(-40, MayaCmds.getAttr('test.rotateAxisY'))
        self.failUnlessAlmostEqual(0, MayaCmds.getAttr('test.rotateAxisZ'))

        MayaCmds.currentTime(3, update = True)
        self.assertEqual(1, MayaCmds.getAttr('test.rotateOrder'))
        self.failUnlessAlmostEqual(110, MayaCmds.getAttr('test.rotateX'))
        self.failUnlessAlmostEqual(290, MayaCmds.getAttr('test.rotateY'))
        self.failUnlessAlmostEqual(810, MayaCmds.getAttr('test.rotateZ'))
        self.failUnlessAlmostEqual(110, MayaCmds.getAttr('test.rotateAxisX'))
        self.failUnlessAlmostEqual(-70, MayaCmds.getAttr('test.rotateAxisY'))
        self.failUnlessAlmostEqual(90, MayaCmds.getAttr('test.rotateAxisZ'))

        # Check file with euler filter
        MayaCmds.AbcImport(self.__files[-1], mode='open')
        MayaCmds.currentTime(3, update = True)  # refresh

        MayaCmds.currentTime(1, update = True)
        self.assertEqual(1, MayaCmds.getAttr('test.rotateOrder'))
        self.failUnlessAlmostEqual(90, MayaCmds.getAttr('test.rotateX'))
        self.failUnlessAlmostEqual(-10, MayaCmds.getAttr('test.rotateY'))
        self.failUnlessAlmostEqual(270, MayaCmds.getAttr('test.rotateZ'))
        self.failUnlessAlmostEqual(90, MayaCmds.getAttr('test.rotateAxisX'))
        self.failUnlessAlmostEqual(-10, MayaCmds.getAttr('test.rotateAxisY'))
        self.failUnlessAlmostEqual(-90, MayaCmds.getAttr('test.rotateAxisZ'))

        MayaCmds.currentTime(2, update = True)
        self.assertEqual(1, MayaCmds.getAttr('test.rotateOrder'))
        self.failUnlessAlmostEqual(100, MayaCmds.getAttr('test.rotateX'))
        self.failUnlessAlmostEqual(-40, MayaCmds.getAttr('test.rotateY'))
        self.failUnlessAlmostEqual(360, MayaCmds.getAttr('test.rotateZ'))
        self.failUnlessAlmostEqual(100, MayaCmds.getAttr('test.rotateAxisX'))
        self.failUnlessAlmostEqual(-40, MayaCmds.getAttr('test.rotateAxisY'))
        self.failUnlessAlmostEqual(0, MayaCmds.getAttr('test.rotateAxisZ'))

        MayaCmds.currentTime(3, update = True)
        self.assertEqual(1, MayaCmds.getAttr('test.rotateOrder'))
        self.failUnlessAlmostEqual(110, MayaCmds.getAttr('test.rotateX'))
        self.failUnlessAlmostEqual(-70, MayaCmds.getAttr('test.rotateY'))
        self.failUnlessAlmostEqual(450, MayaCmds.getAttr('test.rotateZ'))
        self.failUnlessAlmostEqual(110, MayaCmds.getAttr('test.rotateAxisX'))
        self.failUnlessAlmostEqual(-70, MayaCmds.getAttr('test.rotateAxisY'))
        self.failUnlessAlmostEqual(90, MayaCmds.getAttr('test.rotateAxisZ'))

