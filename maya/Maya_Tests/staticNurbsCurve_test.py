
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

from maya import cmds as MayaCmds
import os
import unittest
import util


def testNurbsCurveRW( self, isGroup, hdfFileName, inText):

    MayaCmds.file(new=True, force=True)
    name = MayaCmds.textCurves( font='Courier', text=inText )
    if isGroup == True :
        MayaCmds.addAttr(name[0], longName='riCurves', at='bool', dv=True)

    shapeNames = MayaCmds.ls(type='nurbsCurve')

    miscinfo=[]
    cv=[]
    knot=[]

    curveInfoNode = MayaCmds.createNode('curveInfo')
    for i in range(0, len(shapeNames)) :
        shapeName = shapeNames[i]
        MayaCmds.connectAttr(shapeName+'.worldSpace',
            curveInfoNode+'.inputCurve', force=True)
        controlPoints = MayaCmds.getAttr(curveInfoNode + '.controlPoints[*]')
        cv.append(controlPoints)
        numCV = len(controlPoints)
        spans  = MayaCmds.getAttr(shapeName+'.spans')
        degree = MayaCmds.getAttr(shapeName+'.degree')
        form   = MayaCmds.getAttr(shapeName+'.form')
        minVal = MayaCmds.getAttr(shapeName+'.minValue')
        maxVal = MayaCmds.getAttr(shapeName+'.maxValue')
        misc = [numCV, spans, degree, form, minVal, maxVal]
        miscinfo.append(misc)
        knots = MayaCmds.getAttr(curveInfoNode + '.knots[*]')
        knot.append(knots)

    MayaCmds.AlembicTakoExport(name[0] + ' ' + hdfFileName)

    # reading test

    MayaCmds.AlembicTakoImport(hdfFileName, mode='open')

    if isGroup == True:
        shapeNames = MayaCmds.ls(exactType='nurbsCurve')
        self.failUnless(MayaCmds.getAttr(name[0]+'.riCurves'))

    miscinfo2 = []
    cv2 = []
    knot2 = []

    curveInfoNode = MayaCmds.createNode('curveInfo')
    for i in range(0, len(shapeNames)):
        name = shapeNames[i]
        MayaCmds.connectAttr(name+'.worldSpace', curveInfoNode+'.inputCurve',
            force=True)
        controlPoints = MayaCmds.getAttr(curveInfoNode + '.controlPoints[*]')
        cv2.append(controlPoints)
        numCV = len(controlPoints)
        spans  = MayaCmds.getAttr(name+'.spans')
        degree = MayaCmds.getAttr(name+'.degree')
        form   = MayaCmds.getAttr(name+'.form')
        minVal = MayaCmds.getAttr(name+'.minValue')
        maxVal = MayaCmds.getAttr(name+'.maxValue')
        misc = [numCV, spans, degree, form, minVal, maxVal]
        miscinfo2.append(misc)
        knots = MayaCmds.getAttr(curveInfoNode + '.knots[*]')
        knot2.append(knots)

    for i in range(0, len(shapeNames)) :
        name = shapeNames[i]
        self.failUnlessEqual(len(cv[i]), len(cv2[i]))
        for j in range(0, len(cv[i])) :
            cp1 = cv[i][j]
            cp2 = cv2[i][j]
            self.failUnlessAlmostEqual(cp1[0], cp2[0], 3,
                'curve[%d].cp[%d].x not equal' % (i,j))
            self.failUnlessAlmostEqual(cp1[1], cp2[1], 3,
                'curve[%d].cp[%d].y not equal' % (i,j))
            self.failUnlessAlmostEqual(cp1[2], cp2[2], 3,
                'curve[%d].cp[%d].z not equal' % (i,j))

        self.failUnlessEqual(len(knot[i]), len(knot2[i]))
        for j in range(0, len(cv2)):
            k1 = knot[i][j]
            k2 = knot2[i][j]
            self.failUnlessAlmostEqual(k1, k2, 3)

        if isGroup:
            self.failUnlessAlmostEqual(0.1,
                MayaCmds.getAttr(name+'.constantwidth'), 4)

class StaticNurbsCurveTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testNurbsSingleCurveReadWrite(self):

        name = MayaCmds.curve(d=3, p=[(0, 0, 0), (3, 5, 6), (5, 6, 7),
            (9, 9, 9), (12, 10, 2)], k=[0,0,0,1,2,2,2])
        self.__files.append('/tmp/testStaticNurbsSingleCurve.hdf')
        MayaCmds.AlembicTakoExport(name + ' ' + self.__files[-1])

        # reading test
        MayaCmds.AlembicTakoImport(self.__files[-1], mode='import')
        shapeNames = MayaCmds.ls(exactType='nurbsCurve')
        util.compareNurbsCurve(shapeNames[0], shapeNames[1])

    def testNurbsCurveGrpReadWrite(self):

        # test w/r of simple Nurbs Curve
        self.__files.append('/tmp/testStaticNurbsCurves.hdf')
        testNurbsCurveRW(self, False, self.__files[-1], 'haka')

        self.__files.append('/tmp/testStaticNurbsCurveGrp.hdf')
        testNurbsCurveRW(self, True, self.__files[-1], 'haka')

        # test if some curves have different degree or close states information
        MayaCmds.file(new=True, force=True)
        name = MayaCmds.textCurves(font='Courier', text='Maya')
        MayaCmds.addAttr(name[0], longName='riCurves', at='bool', dv=True)
        self.__files.append('/tmp/testStaticNurbsCurveGrp2.hdf')
        MayaCmds.AlembicTakoExport(name[0] + ' ' + self.__files[-1])
        MayaCmds.AlembicTakoImport(self.__files[-1], mode='open')
        if 'riCurves' in MayaCmds.listAttr(name[0]):
            self.fail(name[0]+".riCurves shouldn't exist")

    def testNurbsSingleCurveWidthReadWrite(self):

        MayaCmds.file(new=True, force=True)

        # single curve with no width information
        MayaCmds.curve(d=3, p=[(0, 0, 0), (3, 5, 0), (5, 6, 0), (9, 9, 0),
            (12, 10, 0)], k=[0,0,0,1,2,2,2], name='curve1')

        # single curve with constantwidth information
        MayaCmds.curve(d=3, p=[(0, 0, 3), (3, 5, 3), (5, 6, 3), (9, 9, 3),
            (12, 10, 3)], k=[0,0,0,1,2,2,2], name='curve2')
        MayaCmds.select('curveShape2')
        MayaCmds.addAttr(longName='constantwidth', attributeType='double',
            dv=0.75)
        # single open curve with width information
        MayaCmds.curve(d=3, p=[(0, 0, 6), (3, 5, 6), (5, 6, 6), (9, 9, 6),
            (12, 10, 6)], k=[0,0,0,1,2,2,2], name='curve3')
        MayaCmds.select('curveShape3')
        MayaCmds.addAttr(longName='width', dataType='doubleArray')
        MayaCmds.setAttr('curveShape3.width', [0.2, 0.4], type='doubleArray')

        # single open curve with wrong width information
        MayaCmds.curve(d=3, p=[(0, 0, 9), (3, 5, 9), (5, 6, 9), (9, 9, 9),
            (12, 10, 9)], k=[0,0,0,1,2,2,2], name='curve4')
        MayaCmds.select('curveShape4')
        MayaCmds.addAttr(longName='width', dataType='doubleArray')
        MayaCmds.setAttr('curveShape4.width', [0.12, 1.4, 0.37],
            type='doubleArray')

        # single curve circle with width information
        MayaCmds.circle(ch=False, name='curve5')
        MayaCmds.select('curve5Shape')
        MayaCmds.addAttr(longName='width', dataType='doubleArray')
        MayaCmds.setAttr('curve5Shape.width', [0.1, 0.2, 0.3, 0.4, 0.5, 0.6,
            0.7, 0.8], type='doubleArray')

        # single curve circle with wrong width information
        MayaCmds.circle(ch=False, name='curve6')
        MayaCmds.select('curve6Shape')
        MayaCmds.addAttr(longName='width', dataType='doubleArray')
        MayaCmds.setAttr('curve6Shape.width', [0.12, 1.4, 0.37],
            type='doubleArray')

        self.__files.append('/tmp/testStaticNurbsCurveWidthTest.hdf')
        MayaCmds.AlembicTakoExport('curve1 curve2 curve3 curve4 curve5 curve6 ' +
            self.__files[-1])
        MayaCmds.AlembicTakoImport(self.__files[-1], mode='open')

        # check the width

        # curve 1
        self.failIf('width' in MayaCmds.listAttr('curveShape1'))
        self.failUnless('constantwidth' in MayaCmds.listAttr('curveShape1'))
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('curveShape1.constantwidth'), 0.1, 4)

        # curve 2
        self.failIf('width' in MayaCmds.listAttr('curveShape2'))
        self.failUnless('constantwidth' in MayaCmds.listAttr('curveShape2'))
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('curveShape2.constantwidth'), 0.75, 4)

        # curve 3
        self.failIf('width' in MayaCmds.listAttr('curveShape3'))
        self.failUnless('constantwidth' in MayaCmds.listAttr('curveShape3'))
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('curveShape3.constantwidth'), 0.1, 4)

        # curve 4
        self.failUnless('width' in MayaCmds.listAttr('curveShape4'))
        self.failIf('constantwidth' in MayaCmds.listAttr('curveShape4'))
        width = MayaCmds.getAttr('curveShape4.width')
        self.failUnlessAlmostEqual(width[0], 0.12, 4)
        self.failUnlessAlmostEqual(width[1], 1.4, 4)
        self.failUnlessAlmostEqual(width[2], 0.37, 4)

        # curve 5
        self.failUnlessEqual('width' in MayaCmds.listAttr('curve5Shape'), True)
        self.failIfEqual('constantwidth' in MayaCmds.listAttr('curve5Shape'),
            True)
        width = MayaCmds.getAttr('curve5Shape.width')
        for i in range(0, 8):
            self.failUnlessAlmostEqual(width[i], 0.1+i*0.1, 4)

        # curve 6
        self.failIf('width' in MayaCmds.listAttr('curve6Shape'))
        self.failUnless('constantwidth' in MayaCmds.listAttr('curve6Shape'))
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('curve6Shape.constantwidth'), 0.1, 4)

    def testNurbsCurveGrpWidthRW1(self):
        # curve group with no width information
        # if the length of the width vector is not the same as the number of
        # span
        # if within the same curve group, one curve has width and the other has
        # constantwidth
        # if within the same curve group, curves have different values for
        # constantwidth

        widthVec = [0.12, 0.13, 0.14, 0.22, 0.23, 0.24, 0.33, 0.34, 0.36]
        MayaCmds.curve(d=3, p=[(0, 0, 0), (3, 5, 0), (5, 6, 0), (9, 9, 0), 
            (12, 10, 0)], k=[0,0,0,1,2,2,2], name='curve1')
        MayaCmds.select('curveShape1')
        MayaCmds.addAttr(longName='width', dataType='doubleArray')
        MayaCmds.setAttr('curveShape1.width', widthVec[0:3], type='doubleArray')

        MayaCmds.curve(d=3, p=[(0, 0, 3), (3, 5, 3), (5, 6, 3), (9, 9, 3),
            (12, 10, 3)], k=[0,0,0,1,2,2,2], name='curve2')
        MayaCmds.select('curveShape2')
        MayaCmds.addAttr(longName='width', dataType='doubleArray')
        MayaCmds.setAttr('curveShape2.width', widthVec[3:6], type='doubleArray')

        MayaCmds.curve(d=3, p=[(0, 0, 6), (3, 5, 6), (5, 6, 6), (9, 9, 6),
            (12, 10, 6)], k=[0,0,0,1,2,2,2], name='curve3')
        MayaCmds.select('curveShape3')
        MayaCmds.addAttr(longName='width', dataType='doubleArray')
        MayaCmds.setAttr('curveShape3.width', widthVec[6:], type='doubleArray')

        MayaCmds.group('curve1', 'curve2', 'curve3', name='group')
        MayaCmds.addAttr('group', longName='riCurves', at='bool', dv=True)

        self.__files.append('/tmp/testStaticNurbsCurveGrpWidthTest1.hdf')
        MayaCmds.AlembicTakoExport('group ' + self.__files[-1])
        MayaCmds.AlembicTakoImport(self.__files[-1], mode='open')

        MayaCmds.file(rename='/mcp/scene.mb')
        MayaCmds.file(save=True)

        # width check
        for i in range(0,3):
            shapeName = 'groupShape' + str(i+1)
            self.failUnless('width' in MayaCmds.listAttr(shapeName))
            self.failIf('constantwidth' in MayaCmds.listAttr(shapeName))
            for j in range(0, 3):
                width = MayaCmds.getAttr(shapeName + '.width')
                self.failUnlessAlmostEqual(width[j], widthVec[3*i+j], 4)

    def testNurbsCurveGrpWidthRW2(self):
        # if the length of the width vector is not the same as the number of
        # span
        # if within the same curve group, one curve has width and the other
        # has constantwidth
        # if within the same curve group, curves have different values for
        # constantwidth

        MayaCmds.curve(d=3, p=[(0, 0, 0), (3, 5, 0), (5, 6, 0), (9, 9, 0),
            (12, 10, 0)], k=[0,0,0,1,2,2,2], name='curve1')
        MayaCmds.select('curveShape1')
        MayaCmds.addAttr(longName='width', dataType='doubleArray')
        MayaCmds.setAttr('curveShape1.width', [0.1, 0.2, 0.3],
            type='doubleArray')
        MayaCmds.curve(d=3, p=[(0, 0, 3), (3, 5, 3), (5, 6, 3), (9, 9, 3),
            (12, 10, 3)], k=[0,0,0,1,2,2,2], name='curve2')
        MayaCmds.curve(d=3, p=[(0, 0, 6), (3, 5, 6), (5, 6, 6), (9, 9, 6),
            (12, 10, 6)], k=[0,0,0,1,2,2,2], name='curve3')
        MayaCmds.select('curveShape3')
        MayaCmds.addAttr(longName='constantwidth', attributeType='double',
            dv=0.75)
        MayaCmds.group('curve1', 'curve2', 'curve3', name='group')
        MayaCmds.addAttr('group', longName='riCurves', at='bool', dv=True)

        self.__files.append('/tmp/testStaticNurbsCurveGrpWidthTest2.hdf')
        MayaCmds.AlembicTakoExport('group ' + self.__files[-1])
        MayaCmds.AlembicTakoImport(self.__files[-1], mode='open')

        # width check
        for i in range(0,3):
            shapeName = 'groupShape' + str(i+1)
            self.failIf('width' in MayaCmds.listAttr(shapeName))
            self.failUnless('constantwidth' in MayaCmds.listAttr(shapeName))
            self.failUnlessAlmostEqual(
                MayaCmds.getAttr(shapeName + '.constantwidth'), 0.75, 4)
