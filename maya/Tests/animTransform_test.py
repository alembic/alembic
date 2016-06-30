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
import subprocess
import unittest
import util

class AnimTransformTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__abcStitcher = [os.environ['AbcStitcher']]
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testAnimWFGTransformReadWrite(self):
        self.testAnimTransformReadWrite(True)

    def testAnimTransformReadWrite(self, wfg=False):
        nodeName = MayaCmds.createNode('transform', n='test')

        # shear
        MayaCmds.setKeyframe(nodeName, value=0, attribute='shearXY', t=[1, 24])
        MayaCmds.setKeyframe(nodeName, value=0, attribute='shearYZ', t=[1, 24])
        MayaCmds.setKeyframe(nodeName, value=0, attribute='shearXZ', t=[1, 24])
        MayaCmds.setKeyframe(nodeName, value=1.5,   attribute='shearXY', t=12)
        MayaCmds.setKeyframe(nodeName, value=5,     attribute='shearYZ', t=12)
        MayaCmds.setKeyframe(nodeName, value=2.5,   attribute='shearXZ', t=12)

        # translate
        MayaCmds.setKeyframe('test', value=0, attribute='translateX',
            t=[1, 24])
        MayaCmds.setKeyframe('test', value=0, attribute='translateY',
            t=[1, 24])
        MayaCmds.setKeyframe('test', value=0, attribute='translateZ',
            t=[1, 24])
        MayaCmds.setKeyframe('test', value=1.5, attribute='translateX', t=12)
        MayaCmds.setKeyframe('test', value=5, attribute='translateY', t=12)
        MayaCmds.setKeyframe('test', value=2.5, attribute='translateZ', t=12)

        # rotate
        MayaCmds.setKeyframe('test', value=0, attribute='rotateX', t=[1, 24])
        MayaCmds.setKeyframe('test', value=0, attribute='rotateY', t=[1, 24])
        MayaCmds.setKeyframe('test', value=0, attribute='rotateZ', t=[1, 24])
        MayaCmds.setKeyframe('test', value=24, attribute='rotateX', t=12)
        MayaCmds.setKeyframe('test', value=53, attribute='rotateY', t=12)
        MayaCmds.setKeyframe('test', value=90, attribute='rotateZ', t=12)

        # scale
        MayaCmds.setKeyframe('test', value=1, attribute='scaleX', t=[1, 24])
        MayaCmds.setKeyframe('test', value=1, attribute='scaleY', t=[1, 24])
        MayaCmds.setKeyframe('test', value=1, attribute='scaleZ', t=[1, 24])
        MayaCmds.setKeyframe('test', value=1.2, attribute='scaleX', t=12)
        MayaCmds.setKeyframe('test', value=1.5, attribute='scaleY', t=12)
        MayaCmds.setKeyframe('test', value=1.5, attribute='scaleZ', t=12)

        # rotate pivot
        MayaCmds.setKeyframe('test', value=0.5, attribute='rotatePivotX',
            t=[1, 24])
        MayaCmds.setKeyframe('test', value=-0.1, attribute='rotatePivotY',
            t=[1, 24])
        MayaCmds.setKeyframe('test', value=1, attribute='rotatePivotZ',
            t=[1, 24])
        MayaCmds.setKeyframe('test', value=0.8, attribute='rotatePivotX', t=12)
        MayaCmds.setKeyframe('test', value=1.5, attribute='rotatePivotY', t=12)
        MayaCmds.setKeyframe('test', value=-1, attribute='rotatePivotZ', t=12)

        # scale pivot
        MayaCmds.setKeyframe('test', value=1.2, attribute='scalePivotX',
            t=[1, 24])
        MayaCmds.setKeyframe('test', value=1.0, attribute='scalePivotY',
            t=[1, 24])
        MayaCmds.setKeyframe('test', value=1.2, attribute='scalePivotZ',
            t=[1, 24])
        MayaCmds.setKeyframe('test', value=1.4, attribute='scalePivotX', t=12)
        MayaCmds.setKeyframe('test', value=1.5, attribute='scalePivotY', t=12)
        MayaCmds.setKeyframe('test', value=1.5, attribute='scalePivotZ', t=12)

        if wfg:
            self.__files.append(util.expandFileName('testAnimWFGTransformReadWrite.abc'))

            MayaCmds.AbcExport(j='-fr 1 24 -wfg -frs -0.25 -frs 0.0 -frs 0.25 -root test -file ' + self.__files[-1])
            MayaCmds.AbcImport(self.__files[-1], mode='open')
        else:
            self.__files.append(util.expandFileName('testAnimTransformReadWrite.abc'))
            self.__files.append(util.expandFileName('testAnimTransformReadWrite01_14.abc'))
            self.__files.append(util.expandFileName('testAnimTransformReadWrite15_24.abc'))

            MayaCmds.AbcExport(j='-fr 1 14 -root test -file ' + self.__files[-2])
            MayaCmds.AbcExport(j='-fr 15 24 -root test -file ' + self.__files[-1])

            subprocess.call(self.__abcStitcher + self.__files[-3:])
            MayaCmds.AbcImport(self.__files[-3], mode='open')

        # frame 1
        MayaCmds.currentTime(1, update=True)

        self.failUnlessEqual(0, MayaCmds.getAttr('test.shearXY'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.shearYZ'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.shearXZ'))

        self.failUnlessEqual(0, MayaCmds.getAttr('test.translateX'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.translateY'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.translateZ'))

        self.failUnlessEqual(0, MayaCmds.getAttr('test.rotateX'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.rotateY'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.rotateZ'))

        self.failUnlessEqual(1, MayaCmds.getAttr('test.scaleX'))
        self.failUnlessEqual(1, MayaCmds.getAttr('test.scaleY'))
        self.failUnlessEqual(1, MayaCmds.getAttr('test.scaleZ'))

        self.failUnlessEqual(0.5, MayaCmds.getAttr('test.rotatePivotX'))
        self.failUnlessEqual(-0.1, MayaCmds.getAttr('test.rotatePivotY'))
        self.failUnlessEqual(1, MayaCmds.getAttr('test.rotatePivotZ'))

        self.failUnlessEqual(1.2, MayaCmds.getAttr('test.scalePivotX'))
        self.failUnlessEqual(1.0, MayaCmds.getAttr('test.scalePivotY'))
        self.failUnlessEqual(1.2, MayaCmds.getAttr('test.scalePivotZ'))

        # frame 12
        MayaCmds.currentTime(12, update=True);
        abcNodeName = MayaCmds.ls(exactType='AlembicNode')
        MayaCmds.dgeval(abcNodeName, verbose=True)

        self.failUnlessEqual(1.5, MayaCmds.getAttr('test.shearXY'))
        self.failUnlessEqual(5, MayaCmds.getAttr('test.shearYZ'))
        self.failUnlessEqual(2.5, MayaCmds.getAttr('test.shearXZ'))

        self.failUnlessEqual(1.5, MayaCmds.getAttr('test.translateX'))
        self.failUnlessEqual(5, MayaCmds.getAttr('test.translateY'))
        self.failUnlessEqual(2.5, MayaCmds.getAttr('test.translateZ'))

        self.failUnlessAlmostEqual(24.0, MayaCmds.getAttr('test.rotateX'), 4)
        self.failUnlessAlmostEqual(53.0, MayaCmds.getAttr('test.rotateY'), 4)
        self.failUnlessAlmostEqual(90.0, MayaCmds.getAttr('test.rotateZ'), 4)

        self.failUnlessEqual(1.2, MayaCmds.getAttr('test.scaleX'))
        self.failUnlessEqual(1.5, MayaCmds.getAttr('test.scaleY'))
        self.failUnlessEqual(1.5, MayaCmds.getAttr('test.scaleZ'))

        self.failUnlessEqual(0.8, MayaCmds.getAttr('test.rotatePivotX'))
        self.failUnlessEqual(1.5, MayaCmds.getAttr('test.rotatePivotY'))
        self.failUnlessEqual(-1, MayaCmds.getAttr('test.rotatePivotZ'))

        self.failUnlessEqual(1.4, MayaCmds.getAttr('test.scalePivotX'))
        self.failUnlessEqual(1.5, MayaCmds.getAttr('test.scalePivotY'))
        self.failUnlessEqual(1.5, MayaCmds.getAttr('test.scalePivotZ'))

        # frame 24
        MayaCmds.currentTime(24, update=True);
        abcNodeName = MayaCmds.ls(exactType='AlembicNode')
        MayaCmds.dgeval(abcNodeName, verbose=True)

        self.failUnlessEqual(0, MayaCmds.getAttr('test.shearXY'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.shearYZ'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.shearXZ'))

        self.failUnlessEqual(0, MayaCmds.getAttr('test.translateX'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.translateY'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.translateZ'))

        self.failUnlessEqual(0, MayaCmds.getAttr('test.rotateX'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.rotateY'))
        self.failUnlessEqual(0, MayaCmds.getAttr('test.rotateZ'))

        self.failUnlessEqual(1, MayaCmds.getAttr('test.scaleX'))
        self.failUnlessEqual(1, MayaCmds.getAttr('test.scaleY'))
        self.failUnlessEqual(1, MayaCmds.getAttr('test.scaleZ'))

        self.failUnlessEqual(0.5, MayaCmds.getAttr('test.rotatePivotX'))
        self.failUnlessEqual(-0.1, MayaCmds.getAttr('test.rotatePivotY'))
        self.failUnlessEqual(1, MayaCmds.getAttr('test.rotatePivotZ'))

        self.failUnlessEqual(1.2, MayaCmds.getAttr('test.scalePivotX'))
        self.failUnlessEqual(1.0, MayaCmds.getAttr('test.scalePivotY'))
        self.failUnlessEqual(1.2, MayaCmds.getAttr('test.scalePivotZ'))

    def testSampledConnectionDetectionRW(self):

        # connect to plugs at parent level and see if when loaded back
        # the sampled channels are recognized correctly

        driver = MayaCmds.createNode('transform', n='driverTrans')

        # shear
        MayaCmds.setKeyframe(driver, value=0, attribute='shearXY', t=[1, 24])
        MayaCmds.setKeyframe(driver, value=0, attribute='shearYZ', t=[1, 24])
        MayaCmds.setKeyframe(driver, value=0, attribute='shearXZ', t=[1, 24])
        MayaCmds.setKeyframe(driver, value=1.5, attribute='shearXY', t=12)
        MayaCmds.setKeyframe(driver, value=5, attribute='shearYZ', t=12)
        MayaCmds.setKeyframe(driver, value=2.5, attribute='shearXZ', t=12)

        # translate
        MayaCmds.setKeyframe(driver, value=0, attribute='translateX',
            t=[1, 24])
        MayaCmds.setKeyframe(driver, value=0, attribute='translateY',
            t=[1, 24])
        MayaCmds.setKeyframe(driver, value=0, attribute='translateZ',
            t=[1, 24])
        MayaCmds.setKeyframe(driver, value=1.5, attribute='translateX', t=12)
        MayaCmds.setKeyframe(driver, value=5, attribute='translateY', t=12)
        MayaCmds.setKeyframe(driver, value=2.5, attribute='translateZ', t=12)

        # rotate
        MayaCmds.setKeyframe(driver, value=0, attribute='rotateX', t=[1, 24])
        MayaCmds.setKeyframe(driver, value=0, attribute='rotateY', t=[1, 24])
        MayaCmds.setKeyframe(driver, value=0, attribute='rotateZ', t=[1, 24])
        MayaCmds.setKeyframe(driver, value=24, attribute='rotateX', t=12)
        MayaCmds.setKeyframe(driver, value=53, attribute='rotateY', t=12)
        MayaCmds.setKeyframe(driver, value=90, attribute='rotateZ', t=12)

        # scale
        MayaCmds.setKeyframe(driver, value=1, attribute='scaleX', t=[1, 24])
        MayaCmds.setKeyframe(driver, value=1, attribute='scaleY', t=[1, 24])
        MayaCmds.setKeyframe(driver, value=1, attribute='scaleZ', t=[1, 24])
        MayaCmds.setKeyframe(driver, value=1.2, attribute='scaleX', t=12)
        MayaCmds.setKeyframe(driver, value=1.5, attribute='scaleY', t=12)
        MayaCmds.setKeyframe(driver, value=1.5, attribute='scaleZ', t=12)

        # rotate pivot
        MayaCmds.setKeyframe(driver, value=0.5, attribute='rotatePivotX',
            t=[1, 24])
        MayaCmds.setKeyframe(driver, value=-0.1, attribute='rotatePivotY',
            t=[1, 24])
        MayaCmds.setKeyframe(driver, value=1, attribute='rotatePivotZ',
            t=[1, 24])
        MayaCmds.setKeyframe(driver, value=0.8, attribute='rotatePivotX', t=12)
        MayaCmds.setKeyframe(driver, value=1.5, attribute='rotatePivotY', t=12)
        MayaCmds.setKeyframe(driver, value=-1, attribute='rotatePivotZ', t=12)

        # scale pivot
        MayaCmds.setKeyframe(driver, value=1.2, attribute='scalePivotX',
            t=[1, 24])
        MayaCmds.setKeyframe(driver, value=1.0, attribute='scalePivotY',
            t=[1, 24])
        MayaCmds.setKeyframe(driver, value=1.2, attribute='scalePivotZ',
            t=[1, 24])
        MayaCmds.setKeyframe(driver, value=1.4, attribute='scalePivotX', t=12)
        MayaCmds.setKeyframe(driver, value=1.5, attribute='scalePivotY', t=12)
        MayaCmds.setKeyframe(driver, value=1.5, attribute='scalePivotZ', t=12)

        # create the transform node that's been driven by the connections
        driven = MayaCmds.createNode('transform', n = 'drivenTrans')
        MayaCmds.connectAttr(driver+'.translate', driven+'.translate')
        MayaCmds.connectAttr(driver+'.scale', driven+'.scale')
        MayaCmds.connectAttr(driver+'.rotate', driven+'.rotate')
        MayaCmds.connectAttr(driver+'.shear', driven+'.shear')
        MayaCmds.connectAttr(driver+'.rotatePivot', driven+'.rotatePivot')
        MayaCmds.connectAttr(driver+'.scalePivot', driven+'.scalePivot')

        self.__files.append(util.expandFileName('testSampledTransformDetection.abc'))
        MayaCmds.AbcExport(j='-fr 1 24 -root drivenTrans -file ' + self.__files[-1])

        # reading test
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        # frame 1
        MayaCmds.currentTime(1, update=True)

        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.shearXY'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.shearYZ'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.shearXZ'))

        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.translateX'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.translateY'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.translateZ'))

        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.rotateX'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.rotateY'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.rotateZ'))

        self.failUnlessEqual(1, MayaCmds.getAttr(driven+'.scaleX'))
        self.failUnlessEqual(1, MayaCmds.getAttr(driven+'.scaleY'))
        self.failUnlessEqual(1, MayaCmds.getAttr(driven+'.scaleZ'))

        self.failUnlessEqual(0.5, MayaCmds.getAttr(driven+'.rotatePivotX'))
        self.failUnlessEqual(-0.1, MayaCmds.getAttr(driven+'.rotatePivotY'))
        self.failUnlessEqual(1, MayaCmds.getAttr(driven+'.rotatePivotZ'))

        self.failUnlessEqual(1.2, MayaCmds.getAttr(driven+'.scalePivotX'))
        self.failUnlessEqual(1.0, MayaCmds.getAttr(driven+'.scalePivotY'))
        self.failUnlessEqual(1.2, MayaCmds.getAttr(driven+'.scalePivotZ'))

        # frame 12
        MayaCmds.currentTime(12, update=True);
        abcNodeName = MayaCmds.ls(exactType='AlembicNode')
        MayaCmds.dgeval(abcNodeName, verbose=True)

        self.failUnlessEqual(1.5, MayaCmds.getAttr(driven+'.shearXY'))
        self.failUnlessEqual(5, MayaCmds.getAttr(driven+'.shearYZ'))
        self.failUnlessEqual(2.5, MayaCmds.getAttr(driven+'.shearXZ'))

        self.failUnlessEqual(1.5, MayaCmds.getAttr(driven+'.translateX'))
        self.failUnlessEqual(5, MayaCmds.getAttr(driven+'.translateY'))
        self.failUnlessEqual(2.5, MayaCmds.getAttr(driven+'.translateZ'))

        self.failUnlessAlmostEqual(24.0, MayaCmds.getAttr(driven+'.rotateX'),
            4)
        self.failUnlessAlmostEqual(53.0, MayaCmds.getAttr(driven+'.rotateY'),
            4)
        self.failUnlessAlmostEqual(90.0, MayaCmds.getAttr(driven+'.rotateZ'), 4)

        self.failUnlessEqual(1.2, MayaCmds.getAttr(driven+'.scaleX'))
        self.failUnlessEqual(1.5, MayaCmds.getAttr(driven+'.scaleY'))
        self.failUnlessEqual(1.5, MayaCmds.getAttr(driven+'.scaleZ'))

        self.failUnlessEqual(0.8, MayaCmds.getAttr(driven+'.rotatePivotX'))
        self.failUnlessEqual(1.5, MayaCmds.getAttr(driven+'.rotatePivotY'))
        self.failUnlessEqual(-1, MayaCmds.getAttr(driven+'.rotatePivotZ'))

        self.failUnlessEqual(1.4, MayaCmds.getAttr(driven+'.scalePivotX'))
        self.failUnlessEqual(1.5, MayaCmds.getAttr(driven+'.scalePivotY'))
        self.failUnlessEqual(1.5, MayaCmds.getAttr(driven+'.scalePivotZ'))

        # frame 24
        MayaCmds.currentTime(24, update=True);
        abcNodeName = MayaCmds.ls(exactType='AlembicNode')
        MayaCmds.dgeval(abcNodeName, verbose=True)

        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.shearXY'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.shearYZ'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.shearXZ'))

        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.translateX'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.translateY'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.translateZ'))

        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.rotateX'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.rotateY'))
        self.failUnlessEqual(0, MayaCmds.getAttr(driven+'.rotateZ'))

        self.failUnlessEqual(1, MayaCmds.getAttr(driven+'.scaleX'))
        self.failUnlessEqual(1, MayaCmds.getAttr(driven+'.scaleY'))
        self.failUnlessEqual(1, MayaCmds.getAttr(driven+'.scaleZ'))

        self.failUnlessEqual(0.5, MayaCmds.getAttr(driven+'.rotatePivotX'))
        self.failUnlessEqual(-0.1, MayaCmds.getAttr(driven+'.rotatePivotY'))
        self.failUnlessEqual(1, MayaCmds.getAttr(driven+'.rotatePivotZ'))

        self.failUnlessEqual(1.2, MayaCmds.getAttr(driven+'.scalePivotX'))
        self.failUnlessEqual(1.0, MayaCmds.getAttr(driven+'.scalePivotY'))
        self.failUnlessEqual(1.2, MayaCmds.getAttr(driven+'.scalePivotZ'))
