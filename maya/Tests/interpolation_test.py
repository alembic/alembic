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

from   maya import cmds as MayaCmds
import maya.OpenMaya as OpenMaya
import unittest
import util
import os

class interpolationTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testSubD(self):
        trans = MayaCmds.polyPlane(n='plane', sx=1, sy=1, ch=False)[0]
        shape = MayaCmds.pickWalk(d='down')[0]
        MayaCmds.addAttr(attributeType='bool', defaultValue=1, keyable=True,
            longName='SubDivisionMesh')
        MayaCmds.select(trans+'.vtx[0:3]', r=True)
        MayaCmds.move(0, 1, 0, r=True)
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe()
        MayaCmds.currentTime(2, update=True)
        MayaCmds.move(0, 5, 0, r=True)
        MayaCmds.setKeyframe()

        self.__files.append(util.expandFileName('testSubDInterpolation.abc'))
        MayaCmds.AbcExport(j='-fr 1 2 -root %s -file %s' % (trans, self.__files[-1]))
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        MayaCmds.currentTime(1.004, update=True)
        ty = MayaCmds.getAttr(shape+'.vt[0]')[0][1]
        self.failUnlessAlmostEqual(1.02, ty)

        setTime = MayaCmds.currentTime(1.422, update=True)
        alpha = (setTime - 1) / (2 - 1)
        ty = MayaCmds.getAttr(shape+'.vt[0]')[0][1]
        self.failUnlessAlmostEqual(ty, (1-alpha)*1.0+alpha*6.0, 3)

    def testPoly(self):
        trans = MayaCmds.polyPlane(n='plane', sx=1, sy=1, ch=False)[0]
        shape = MayaCmds.pickWalk(d='down')[0]
        MayaCmds.select(trans+'.vtx[0:3]', r=True)
        MayaCmds.move(0, 1, 0, r=True)
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe()
        MayaCmds.currentTime(2, update=True)
        MayaCmds.move(0, 5, 0, r=True)
        MayaCmds.setKeyframe()

        self.__files.append(util.expandFileName('testPolyInterpolation.abc'))
        MayaCmds.AbcExport(j='-fr 1 2 -root %s -file %s' % (trans, self.__files[-1]))
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        MayaCmds.currentTime(1.004, update=True)
        ty = MayaCmds.getAttr(shape+'.vt[0]')[0][1]
        self.failUnlessAlmostEqual(1.02, ty)

        setTime = MayaCmds.currentTime(1.422, update=True)
        alpha = (setTime - 1) / (2 - 1)
        ty = MayaCmds.getAttr(shape+'.vt[0]')[0][1]
        self.failUnlessAlmostEqual(ty, (1-alpha)*1.0+alpha*6.0, 3)

    def testTransOp(self):
        nodeName = MayaCmds.createNode('transform', n='test')

        # shear
        MayaCmds.setKeyframe(nodeName, value=0, attribute='shearXY', t=1)
        MayaCmds.setKeyframe(nodeName, value=0, attribute='shearYZ', t=1)
        MayaCmds.setKeyframe(nodeName, value=0, attribute='shearXZ', t=1)
        MayaCmds.setKeyframe(nodeName, value=1.5, attribute='shearXY', t=2)
        MayaCmds.setKeyframe(nodeName, value=5,   attribute='shearYZ', t=2)
        MayaCmds.setKeyframe(nodeName, value=2.5, attribute='shearXZ', t=2)

        # translate
        MayaCmds.setKeyframe('test', value=0, attribute='translateX', t=1)
        MayaCmds.setKeyframe('test', value=0, attribute='translateY', t=1)
        MayaCmds.setKeyframe('test', value=0, attribute='translateZ', t=1)
        MayaCmds.setKeyframe('test', value=1.5, attribute='translateX', t=2)
        MayaCmds.setKeyframe('test', value=5,   attribute='translateY', t=2)
        MayaCmds.setKeyframe('test', value=2.5, attribute='translateZ', t=2)

        # rotate
        MayaCmds.setKeyframe('test', value=0, attribute='rotateX', t=1)
        MayaCmds.setKeyframe('test', value=0, attribute='rotateY', t=1)
        MayaCmds.setKeyframe('test', value=0, attribute='rotateZ', t=1)
        MayaCmds.setKeyframe('test', value=24, attribute='rotateX', t=2)
        MayaCmds.setKeyframe('test', value=53, attribute='rotateY', t=2)
        MayaCmds.setKeyframe('test', value=90, attribute='rotateZ', t=2)

        # scale
        MayaCmds.setKeyframe('test', value=1, attribute='scaleX', t=1)
        MayaCmds.setKeyframe('test', value=1, attribute='scaleY', t=1)
        MayaCmds.setKeyframe('test', value=1, attribute='scaleZ', t=1)
        MayaCmds.setKeyframe('test', value=1.2, attribute='scaleX', t=2)
        MayaCmds.setKeyframe('test', value=1.5, attribute='scaleY', t=2)
        MayaCmds.setKeyframe('test', value=1.5, attribute='scaleZ', t=2)

        # rotate pivot
        MayaCmds.setKeyframe('test', value=0.5, attribute='rotatePivotX', t=1)
        MayaCmds.setKeyframe('test', value=-0.1, attribute='rotatePivotY', t=1)
        MayaCmds.setKeyframe('test', value=1, attribute='rotatePivotZ', t=1)
        MayaCmds.setKeyframe('test', value=0.8, attribute='rotatePivotX', t=2)
        MayaCmds.setKeyframe('test', value=1.5, attribute='rotatePivotY', t=2)
        MayaCmds.setKeyframe('test', value=-1, attribute='rotatePivotZ', t=2)

        # scale pivot
        MayaCmds.setKeyframe('test', value=1.2, attribute='scalePivotX', t=1)
        MayaCmds.setKeyframe('test', value=1.0, attribute='scalePivotY', t=1)
        MayaCmds.setKeyframe('test', value=1.2, attribute='scalePivotZ', t=1)
        MayaCmds.setKeyframe('test', value=1.4, attribute='scalePivotX', t=2)
        MayaCmds.setKeyframe('test', value=1.5, attribute='scalePivotY', t=2)
        MayaCmds.setKeyframe('test', value=1.5, attribute='scalePivotZ', t=2)

        self.__files.append(util.expandFileName('testTransOpInterpolation.abc'))
        MayaCmds.AbcExport(j='-fr 1 2 -root %s -f %s' % (nodeName, self.__files[-1]))
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        MayaCmds.currentTime(1.004, update=True)
        # should read the content of frame #1
        self.failUnlessAlmostEqual(0.006, MayaCmds.getAttr('test.shearXY'))
        self.failUnlessAlmostEqual(0.02, MayaCmds.getAttr('test.shearYZ'))
        self.failUnlessAlmostEqual(0.01, MayaCmds.getAttr('test.shearXZ'))

        self.failUnlessAlmostEqual(0.006, MayaCmds.getAttr('test.translateX'))
        self.failUnlessAlmostEqual(0.02, MayaCmds.getAttr('test.translateY'))
        self.failUnlessAlmostEqual(0.01, MayaCmds.getAttr('test.translateZ'))

        self.failUnlessAlmostEqual(0.096, MayaCmds.getAttr('test.rotateX'))
        self.failUnlessAlmostEqual(0.212, MayaCmds.getAttr('test.rotateY'))
        self.failUnlessAlmostEqual(0.36, MayaCmds.getAttr('test.rotateZ'))

        self.failUnlessAlmostEqual(1.0008, MayaCmds.getAttr('test.scaleX'))
        self.failUnlessAlmostEqual(1.002, MayaCmds.getAttr('test.scaleY'))
        self.failUnlessAlmostEqual(1.002, MayaCmds.getAttr('test.scaleZ'))

        self.failUnlessAlmostEqual(0.5012, MayaCmds.getAttr('test.rotatePivotX'))
        self.failUnlessAlmostEqual(-0.0936, MayaCmds.getAttr('test.rotatePivotY'))
        self.failUnlessAlmostEqual(0.992, MayaCmds.getAttr('test.rotatePivotZ'))

        self.failUnlessAlmostEqual(1.2008, MayaCmds.getAttr('test.scalePivotX'))
        self.failUnlessAlmostEqual(1.002, MayaCmds.getAttr('test.scalePivotY'))
        self.failUnlessAlmostEqual(1.2012, MayaCmds.getAttr('test.scalePivotZ'))

        setTime = MayaCmds.currentTime(1.422, update=True)
        alpha = (setTime - 1) / (2 - 1)
        # should interpolate the content of frame #3 and frame #4
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.shearXY'), (1-alpha)*0+alpha*1.5)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.shearYZ'), (1-alpha)*0+alpha*5.0)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.shearXZ'), (1-alpha)*0+alpha*2.5)

        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.translateX'), (1-alpha)*0+alpha*1.5)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.translateY'), (1-alpha)*0+alpha*5.0)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.translateZ'), (1-alpha)*0+alpha*2.5)

        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.rotateX'), (1-alpha)*0+alpha*24)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.rotateY'), (1-alpha)*0+alpha*53)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.rotateZ'), (1-alpha)*0+alpha*90)

        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.scaleX'), (1-alpha)*1+alpha*1.2)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.scaleY'), (1-alpha)*1+alpha*1.5)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.scaleZ'), (1-alpha)*1+alpha*1.5)

        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.rotatePivotX'), (1-alpha)*0.5+alpha*0.8)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.rotatePivotY'), (1-alpha)*(-0.1)+alpha*1.5)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.rotatePivotZ'), (1-alpha)*1.0+alpha*(-1.0))

        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.scalePivotX'), (1-alpha)*1.2+alpha*1.4)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.scalePivotY'), (1-alpha)*1.0+alpha*1.5)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('test.scalePivotZ'), (1-alpha)*1.2+alpha*1.5)

    def testCamera(self):
        # create an animated camera and write out
        name = MayaCmds.camera()
        MayaCmds.setAttr(name[1]+'.horizontalFilmAperture', 0.962)
        MayaCmds.setAttr(name[1]+'.verticalFilmAperture', 0.731)
        MayaCmds.setAttr(name[1]+'.focalLength', 50)
        MayaCmds.setAttr(name[1]+'.focusDistance', 5)
        MayaCmds.setAttr(name[1]+'.shutterAngle', 144)

        # animate the camera
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe(name[1], attribute='horizontalFilmAperture')
        MayaCmds.setKeyframe(name[1], attribute='focalLength')
        MayaCmds.setKeyframe(name[1], attribute='focusDistance')
        MayaCmds.setKeyframe(name[1], attribute='shutterAngle')

        MayaCmds.currentTime(6, update=True)
        MayaCmds.setKeyframe(name[1], attribute='horizontalFilmAperture', value=0.95)
        MayaCmds.setKeyframe(name[1], attribute='focalLength', value=40)
        MayaCmds.setKeyframe(name[1], attribute='focusDistance', value=5.4)
        MayaCmds.setKeyframe(name[1], attribute='shutterAngle', value=174.94)

        self.__files.append(util.expandFileName('testCameraInterpolation.abc'))
        MayaCmds.AbcExport(j='-fr 1 6 -root %s -f %s' % (name[0], self.__files[-1]))
        MayaCmds.AbcImport(self.__files[-1], mode='import')
        camList = MayaCmds.ls(type='camera')

        t = 1.004
        MayaCmds.currentTime(t, update=True)
        self.failUnlessAlmostEqual(MayaCmds.getAttr(camList[0]+'.horizontalFilmAperture'), 0.962, 3)

        setTime = MayaCmds.currentTime(1.422, update=True)
        alpha = (setTime - 1) / (2 - 1)
        if not util.compareCamera(camList[0], camList[1]):
            self.fail('%s and %s are not the same at frame %d' % (camList[0], camList[1], t))

    def testProp(self):
        trans = MayaCmds.createNode("transform")

        # create animated props
        MayaCmds.select(trans)
        MayaCmds.addAttr(longName='SPT_int8', dv=0, attributeType='byte', keyable=True)
        MayaCmds.addAttr(longName='SPT_int16', dv=0, attributeType='short', keyable=True)
        MayaCmds.addAttr(longName='SPT_int32', dv=0, attributeType='long', keyable=True)
        MayaCmds.addAttr(longName='SPT_float', dv=0, attributeType='float', keyable=True)
        MayaCmds.addAttr(longName='SPT_double', dv=0, attributeType='double', keyable=True)

        MayaCmds.setKeyframe(trans, value=0, attribute='SPT_int8', t=1)
        MayaCmds.setKeyframe(trans, value=100, attribute='SPT_int16', t=1)
        MayaCmds.setKeyframe(trans, value=1000, attribute='SPT_int32', t=1)
        MayaCmds.setKeyframe(trans, value=0.57777777, attribute='SPT_float', t=1)
        MayaCmds.setKeyframe(trans, value=5.045643545457, attribute='SPT_double', t=1)

        MayaCmds.setKeyframe(trans, value=8, attribute='SPT_int8', t=2)
        MayaCmds.setKeyframe(trans, value=16, attribute='SPT_int16', t=2)
        MayaCmds.setKeyframe(trans, value=32, attribute='SPT_int32',  t=2)
        MayaCmds.setKeyframe(trans, value=3.141592654, attribute='SPT_float',  t=2)
        MayaCmds.setKeyframe(trans, value=3.141592654, attribute='SPT_double', t=2)

        self.__files.append(util.expandFileName('testPropInterpolation.abc'))
        MayaCmds.AbcExport(j='-fr 1 2 -atp SPT_ -root %s -f %s' % (trans, self.__files[-1]))
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        t = 1.004
        MayaCmds.currentTime(t, update=True)
        self.failUnlessEqual(MayaCmds.getAttr(trans+'.SPT_int8'), 0)
        self.failUnlessEqual(MayaCmds.getAttr(trans+'.SPT_int16'), 99)
        self.failUnlessEqual(MayaCmds.getAttr(trans+'.SPT_int32'), 996)
        self.failUnlessAlmostEqual(MayaCmds.getAttr(trans+'.SPT_float'), 0.5880330295359999, 7)
        self.failUnlessAlmostEqual(MayaCmds.getAttr(trans+'.SPT_double'), 5.038027341891171, 7)

        setTime = MayaCmds.currentTime(1.422, update=True)
        alpha = (setTime - 1) / (2 - 1)
        self.failUnlessAlmostEqual(MayaCmds.getAttr(trans+'.SPT_int8'), (1-alpha)*0+alpha*8, -1)
        self.failUnlessAlmostEqual(MayaCmds.getAttr(trans+'.SPT_int16'), (1-alpha)*100+alpha*16, -1)
        self.failUnlessAlmostEqual(MayaCmds.getAttr(trans+'.SPT_int32'), (1-alpha)*1000+alpha*32, -1)
        self.failUnlessAlmostEqual(MayaCmds.getAttr(trans+'.SPT_float'), (1-alpha)*0.57777777+alpha*3.141592654, 6)
        self.failUnlessAlmostEqual(MayaCmds.getAttr(trans+'.SPT_double'), (1-alpha)* 5.045643545457+alpha*3.141592654, 6)
