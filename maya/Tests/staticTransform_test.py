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

from maya import cmds as MayaCmds
import os
import unittest
import util

class TransformTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files :
            os.remove(f)

    def testStaticTransformReadWrite(self):
        nodeName = MayaCmds.polyCube(n='cube')
        MayaCmds.setAttr('cube.translate', 2.456, -3.95443, 0, type="double3")
        MayaCmds.setAttr('cube.rotate', 45, 15, -90, type="double3")
        MayaCmds.setAttr('cube.rotateOrder', 4)
        MayaCmds.setAttr('cube.scale', 1.5, 5, 1, type="double3")
        MayaCmds.setAttr('cube.shearXY',1)
        MayaCmds.setAttr('cube.rotatePivot', 13.52, 0.0, 20.25, type="double3")
        MayaCmds.setAttr('cube.rotatePivotTranslate', 0.5, 0.0, 0.25,
            type="double3")
        MayaCmds.setAttr('cube.scalePivot', 10.7, 2.612, 0.2, type="double3")
        MayaCmds.setAttr('cube.scalePivotTranslate', 0.0, 0.0, 0.25,
            type="double3")
        MayaCmds.setAttr('cube.inheritsTransform', 0)

        self.__files.append(util.expandFileName('testStaticTransformReadWrite.abc'))
        MayaCmds.AbcExport(j='-root %s -file %s' % (nodeName[0], self.__files[-1]))

        # reading test
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        self.failUnlessAlmostEqual(2.456,
            MayaCmds.getAttr('cube.translateX'), 4)
        self.failUnlessAlmostEqual(-3.95443,
            MayaCmds.getAttr('cube.translateY'), 4)
        self.failUnlessAlmostEqual(0.0, MayaCmds.getAttr('cube.translateZ'), 4)

        self.failUnlessAlmostEqual(45, MayaCmds.getAttr('cube.rotateX'), 4)
        self.failUnlessAlmostEqual(15, MayaCmds.getAttr('cube.rotateY'), 4)
        self.failUnlessAlmostEqual(-90, MayaCmds.getAttr('cube.rotateZ'), 4)

        self.failUnlessEqual(4, MayaCmds.getAttr('cube.rotateOrder'))

        self.failUnlessAlmostEqual(1.5, MayaCmds.getAttr('cube.scaleX'), 4)
        self.failUnlessAlmostEqual(5, MayaCmds.getAttr('cube.scaleY'), 4)
        self.failUnlessAlmostEqual(1, MayaCmds.getAttr('cube.scaleZ'), 4)

        self.failUnlessAlmostEqual(1, MayaCmds.getAttr('cube.shearXY'), 4)

        self.failUnlessAlmostEqual(13.52,
            MayaCmds.getAttr('cube.rotatePivotX'), 4)
        self.failUnlessAlmostEqual(0.0,
            MayaCmds.getAttr('cube.rotatePivotY'), 4)
        self.failUnlessAlmostEqual(20.25,
            MayaCmds.getAttr('cube.rotatePivotZ'), 4)

        self.failUnlessAlmostEqual(0.5,
            MayaCmds.getAttr('cube.rotatePivotTranslateX'), 4)
        self.failUnlessAlmostEqual(0.0,
            MayaCmds.getAttr('cube.rotatePivotTranslateY'), 4)
        self.failUnlessAlmostEqual(0.25,
            MayaCmds.getAttr('cube.rotatePivotTranslateZ'), 4)

        self.failUnlessAlmostEqual(10.7,
            MayaCmds.getAttr('cube.scalePivotX'), 4)
        self.failUnlessAlmostEqual(2.612,
            MayaCmds.getAttr('cube.scalePivotY'), 4)
        self.failUnlessAlmostEqual(0.2,
            MayaCmds.getAttr('cube.scalePivotZ'), 4)

        self.failUnlessAlmostEqual(0.0,
            MayaCmds.getAttr('cube.scalePivotTranslateX'), 4)
        self.failUnlessAlmostEqual(0.0,
            MayaCmds.getAttr('cube.scalePivotTranslateY'), 4)
        self.failUnlessAlmostEqual(0.25,
            MayaCmds.getAttr('cube.scalePivotTranslateZ'), 4)

        self.failUnlessEqual(0, MayaCmds.getAttr('cube.inheritsTransform'))

    def testStaticTransformRotateOrder(self):
        nodeName = MayaCmds.polyCube(n='cube')
        MayaCmds.setAttr('cube.rotate', 45, 1, -90, type="double3")
        MayaCmds.setAttr('cube.rotateOrder', 5)

        self.__files.append(util.expandFileName('testStaticTransformRotateOrder.abc'))
        MayaCmds.AbcExport(j='-root %s -file %s' % (nodeName[0], self.__files[-1]))
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        self.failUnlessEqual(5, MayaCmds.getAttr('cube.rotateOrder'))
        self.failUnlessAlmostEqual(45, MayaCmds.getAttr('cube.rotateX'), 4)
        self.failUnlessAlmostEqual(1, MayaCmds.getAttr('cube.rotateY'), 4)
        self.failUnlessAlmostEqual(-90, MayaCmds.getAttr('cube.rotateZ'), 4)

    def testStaticTransformRotateOrder2(self):
        nodeName = MayaCmds.polyCube(n='cube')
        MayaCmds.setAttr('cube.rotate', 45, 0, -90, type="double3")
        MayaCmds.setAttr('cube.rotateOrder', 5)

        self.__files.append(util.expandFileName('testStaticTransformRotateOrder2.abc'))
        MayaCmds.AbcExport(j='-root %s -file %s' % (nodeName[0], self.__files[-1]))
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        self.failUnlessEqual(5, MayaCmds.getAttr('cube.rotateOrder'))

        self.failUnlessAlmostEqual(45, MayaCmds.getAttr('cube.rotateX'), 4)
        self.failUnlessAlmostEqual(0, MayaCmds.getAttr('cube.rotateY'), 4)
        self.failUnlessAlmostEqual(-90, MayaCmds.getAttr('cube.rotateZ'), 4)
