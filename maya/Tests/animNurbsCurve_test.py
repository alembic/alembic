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
import subprocess
import unittest
import util

class AnimNurbsCurveTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__abcStitcher = [os.environ['AbcStitcher']]
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    # a test for animated non-curve group Nurbs Curve
    def testAnimSimpleNurbsCurveRW(self):

        # create the Nurbs Curve
        name = MayaCmds.curve( d=3, p=[(0, 0, 0), (3, 5, 6), (5, 6, 7),
            (9, 9, 9), (12, 10, 2)], k=[0,0,0,1,2,2,2] )

        MayaCmds.select(name+'.cv[0:4]')
        # frame 1
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe()
        # frame 24
        MayaCmds.currentTime(24, update=True)
        MayaCmds.setKeyframe()
        # frame 12
        MayaCmds.currentTime(12, update=True)
        MayaCmds.move(3, 0, 0, r=True)
        MayaCmds.setKeyframe()

        self.__files.append(util.expandFileName('testAnimNurbsSingleCurve.abc'))
        self.__files.append(util.expandFileName('testAnimNurbsSingleCurve01_14.abc'))
        self.__files.append(util.expandFileName('testAnimNurbsSingleCurve15_24.abc'))

        MayaCmds.AbcExport(j='-fr 1 14 -root %s -file %s' % (name, self.__files[-2]))
        MayaCmds.AbcExport(j='-fr 15 24 -root %s -file %s' % (name, self.__files[-1]))

        # use AbcStitcher to combine two files into one
        subprocess.call(self.__abcStitcher + self.__files[-3:])

        MayaCmds.AbcImport(self.__files[-3], mode='import')
        shapeNames = MayaCmds.ls(exactType='nurbsCurve')

        MayaCmds.currentTime(1, update=True)
        self.failUnless(util.compareNurbsCurve(shapeNames[0], shapeNames[1]))
        MayaCmds.currentTime(12, update=True)
        self.failUnless(util.compareNurbsCurve(shapeNames[0], shapeNames[1]))
        MayaCmds.currentTime(24, update=True)
        self.failUnless(util.compareNurbsCurve(shapeNames[0], shapeNames[1]))

    # a test for animated non-curve group Nurbs Curve
    def testAnimWFGSimpleNurbsCurveRW(self):

        # create the Nurbs Curve
        name = MayaCmds.curve( d=3, p=[(0, 0, 0), (3, 5, 6), (5, 6, 7),
            (9, 9, 9), (12, 10, 2)], k=[0,0,0,1,2,2,2] )

        MayaCmds.select(name+'.cv[0:4]')
        # frame 1
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe()
        # frame 24
        MayaCmds.currentTime(24, update=True)
        MayaCmds.setKeyframe()
        # frame 12
        MayaCmds.currentTime(12, update=True)
        MayaCmds.move(3, 0, 0, r=True)
        MayaCmds.setKeyframe()

        self.__files.append(util.expandFileName('testAnimWFGNurbsSingleCurve.abc'))

        MayaCmds.AbcExport(j='-fr 1 24 -wfg -frs -0.25 -frs 0.0 -frs 0.25 -root %s -file %s' % (name, self.__files[-1]))

        MayaCmds.AbcImport(self.__files[-1], mode='import')
        shapeNames = MayaCmds.ls(exactType='nurbsCurve')

        MayaCmds.currentTime(1, update=True)
        self.failUnless(util.compareNurbsCurve(shapeNames[0], shapeNames[1]))
        MayaCmds.currentTime(12, update=True)
        self.failUnless(util.compareNurbsCurve(shapeNames[0], shapeNames[1]))
        MayaCmds.currentTime(24, update=True)
        self.failUnless(util.compareNurbsCurve(shapeNames[0], shapeNames[1]))

    def testAnimNurbsCurveGrpRW(self):

        # create Nurbs Curve group
        knotVec = [0,0,0,1,2,2,2]
        curve1CV = [(0, 0, 0), (3, 5, 0), (5, 6, 0), (9, 9, 0), (12, 10, 0)]
        curve2CV = [(0, 0, 3), (3, 5, 3), (5, 6, 3), (9, 9, 3), (12, 10, 3)]
        curve3CV = [(0, 0, 6), (3, 5, 6), (5, 6, 6), (9, 9, 6), (12, 10, 6)]

        MayaCmds.curve(d=3, p=curve1CV, k=knotVec, name='curve1')
        MayaCmds.curve(d=3, p=curve2CV, k=knotVec, name='curve2')
        MayaCmds.curve(d=3, p=curve3CV, k=knotVec, name='curve3')

        MayaCmds.group('curve1', 'curve2', 'curve3', name='group')
        MayaCmds.addAttr('group', longName='riCurves', at='bool', dv=True)

        # frame 1
        MayaCmds.currentTime(1, update=True)
        MayaCmds.select('curve1.cv[0:4]', 'curve2.cv[0:4]', 'curve3.cv[0:4]', replace=True)
        MayaCmds.setKeyframe()
        # frame 24
        MayaCmds.currentTime(24, update=True)
        MayaCmds.select('curve1.cv[0:4]')
        MayaCmds.rotate(0.0, '90deg', 0.0, relative=True )
        MayaCmds.select('curve2.cv[0:4]')
        MayaCmds.move(0.0, 0.5, 0.0, relative=True )
        MayaCmds.select('curve3.cv[0:4]')
        MayaCmds.scale(1.0, 0.5, 1.0, relative=True )
        MayaCmds.select('curve1.cv[0:4]', 'curve2.cv[0:4]', 'curve3.cv[0:4]', replace=True)
        MayaCmds.setKeyframe()

        self.__files.append(util.expandFileName('testAnimNCGrp.abc'))
        self.__files.append(util.expandFileName('testAnimNCGrp01_14.abc'))
        self.__files.append(util.expandFileName('testAnimNCGrp15_24.abc'))

        MayaCmds.AbcExport(j='-fr 1 14 -root %s -file %s' % ('group', self.__files[-2]))
        MayaCmds.AbcExport(j='-fr 15 24 -root %s -file %s' % ('group', self.__files[-1]))

        # use AbcStitcher to combine two files into one
        subprocess.call(self.__abcStitcher + self.__files[-3:])

        # reading test
        MayaCmds.AbcImport(self.__files[-3], mode='import')
        shapeNames = MayaCmds.ls(exactType='nurbsCurve')

        MayaCmds.currentTime(1, update=True)
        for i in range(0, 3):
            self.failUnless(
                util.compareNurbsCurve(shapeNames[i], shapeNames[i+3]))
        MayaCmds.currentTime(12, update=True)
        for i in range(0, 3):
            self.failUnless(
                util.compareNurbsCurve(shapeNames[i], shapeNames[i+3]))
        MayaCmds.currentTime(24, update=True)
        for i in range(0, 3):
            self.failUnless(
                util.compareNurbsCurve(shapeNames[i], shapeNames[i+3]))
