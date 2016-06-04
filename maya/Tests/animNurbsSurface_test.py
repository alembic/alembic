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

class AnimNurbsSurfaceTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__abcStitcher = [os.environ['AbcStitcher']]
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testAnimWFGNurbsPlaneWrite(self):
        self.testAnimNurbsPlaneWrite(True)

    def testAnimNurbsPlaneWrite(self, wfg=False):

        ret = MayaCmds.nurbsPlane(p=(0, 0, 0), ax=(0, 1, 0), w=1, lr=1, d=3,
            u=5, v=5, ch=0)
        name = ret[0]

        MayaCmds.lattice(name, dv=(4, 5, 4), oc=True)
        MayaCmds.select('ffd1Lattice.pt[1:2][0:4][1:2]', r=True)

        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe()
        MayaCmds.currentTime(24, update=True)
        MayaCmds.setKeyframe()
        MayaCmds.currentTime(12, update=True)
        MayaCmds.move( 0, 0.18, 0, r=True)
        MayaCmds.scale( 2.5, 1.0, 2.5, r=True)
        MayaCmds.setKeyframe()

        MayaCmds.curveOnSurface(name,
            uv=((0.597523,0), (0.600359,0.271782), (0.538598,0.564218),
                (0.496932,0.779936),  (0.672153,1)),
            k=(0,0,0,0.263463,0.530094,0.530094,0.530094))

        curvename = MayaCmds.curveOnSurface(name,
            uv=((0.170718,0.565967), (0.0685088,0.393034), (0.141997,0.206296),
                (0.95,0.230359), (0.36264,0.441381), (0.251243,0.569889)),
            k=(0,0,0,0.200545,0.404853,0.598957,0.598957,0.598957))
        MayaCmds.closeCurve(curvename, ch=1, ps=1, rpo=1, bb=0.5, bki=0, p=0.1,
            cos=1)

        MayaCmds.trim(name, lu=0.23, lv=0.39)

        degreeU = MayaCmds.getAttr(name+'.degreeU')
        degreeV = MayaCmds.getAttr(name+'.degreeV')
        spansU  = MayaCmds.getAttr(name+'.spansU')
        spansV  = MayaCmds.getAttr(name+'.spansV')
        formU   = MayaCmds.getAttr(name+'.formU')
        formV   = MayaCmds.getAttr(name+'.formV')
        minU    = MayaCmds.getAttr(name+'.minValueU')
        maxU    = MayaCmds.getAttr(name+'.maxValueU')
        minV    = MayaCmds.getAttr(name+'.minValueV')
        maxV    = MayaCmds.getAttr(name+'.maxValueV')

        MayaCmds.createNode('surfaceInfo')
        MayaCmds.connectAttr(name+'.worldSpace', 'surfaceInfo1.inputSurface',
            force=True)

        MayaCmds.currentTime(1, update=True)
        controlPoints = MayaCmds.getAttr('surfaceInfo1.controlPoints[*]')
        knotsU = MayaCmds.getAttr('surfaceInfo1.knotsU[*]')
        knotsV = MayaCmds.getAttr('surfaceInfo1.knotsV[*]')

        MayaCmds.currentTime(12, update=True)
        controlPoints2 = MayaCmds.getAttr('surfaceInfo1.controlPoints[*]')
        knotsU2 = MayaCmds.getAttr('surfaceInfo1.knotsU[*]')
        knotsV2 = MayaCmds.getAttr('surfaceInfo1.knotsV[*]')

        if wfg:
            self.__files.append(util.expandFileName('testAnimNurbsPlane.abc'))

            MayaCmds.AbcExport(j='-fr 1 24 -frs -0.25 -frs 0.0 -frs 0.25 -wfg -root %s -file %s' % (name, self.__files[-1]))

            # reading test
            MayaCmds.AbcImport(self.__files[-1], mode='open')
        else:
            self.__files.append(util.expandFileName('testAnimNurbsPlane.abc'))
            self.__files.append(util.expandFileName('testAnimNurbsPlane01_14.abc'))
            self.__files.append(util.expandFileName('testAnimNurbsPlane15_24.abc'))

            MayaCmds.AbcExport(j='-fr 1 14 -root %s -file %s' % (name, self.__files[-2]))
            MayaCmds.AbcExport(j='-fr 15 24 -root %s -file %s' % (name, self.__files[-1]))

            # use AbcStitcher to combine two files into one
            subprocess.call(self.__abcStitcher + self.__files[-3:])

            # reading test
            MayaCmds.AbcImport(self.__files[-3], mode='open')

        self.failUnlessEqual(degreeU, MayaCmds.getAttr(name+'.degreeU'))
        self.failUnlessEqual(degreeV, MayaCmds.getAttr(name+'.degreeV'))
        self.failUnlessEqual(spansU, MayaCmds.getAttr(name+'.spansU'))
        self.failUnlessEqual(spansV, MayaCmds.getAttr(name+'.spansV'))
        self.failUnlessEqual(formU, MayaCmds.getAttr(name+'.formU'))
        self.failUnlessEqual(formV, MayaCmds.getAttr(name+'.formV'))
        self.failUnlessEqual(minU, MayaCmds.getAttr(name+'.minValueU'))
        self.failUnlessEqual(maxU, MayaCmds.getAttr(name+'.maxValueU'))
        self.failUnlessEqual(minV, MayaCmds.getAttr(name+'.minValueV'))
        self.failUnlessEqual(maxV, MayaCmds.getAttr(name+'.maxValueV'))

        MayaCmds.createNode('surfaceInfo')
        MayaCmds.connectAttr(name+'.worldSpace', 'surfaceInfo1.inputSurface',
            force=True)

        MayaCmds.currentTime(1, update=True)
        errmsg = "At frame #1, Nurbs Plane's control point #%d.%s not equal"
        for i in range(0, len(controlPoints)):
            cp1 = controlPoints[i]
            cp2 = MayaCmds.getAttr('surfaceInfo1.controlPoints[%d]' % (i))
            self.failUnlessAlmostEqual(cp1[0], cp2[0][0], 3, errmsg % (i, 'x'))
            self.failUnlessAlmostEqual(cp1[1], cp2[0][1], 3, errmsg % (i, 'y'))
            self.failUnlessAlmostEqual(cp1[2], cp2[0][2], 3, errmsg % (i, 'z'))

        errmsg = "At frame #1, Nurbs Plane's control knotsU #%d not equal"
        for i in range(0, len(knotsU)):
            ku1 = knotsU[i]
            ku2 = MayaCmds.getAttr('surfaceInfo1.knotsU[%d]' % (i))
            self.failUnlessAlmostEqual(ku1, ku2, 3, errmsg % (i))

        errmsg = "At frame #1, Nurbs Plane's control knotsV #%d not equal"
        for i in range(0, len(knotsV)):
            kv1 = knotsV[i]
            kv2 = MayaCmds.getAttr('surfaceInfo1.knotsV[%d]' % (i))
            self.failUnlessAlmostEqual(kv1, kv2, 3, errmsg % (i))

        MayaCmds.currentTime(12, update=True)
        errmsg = "At frame #12, Nurbs Plane's control point #%d.%s not equal"
        for i in range(0, len(controlPoints2)):
            cp1 = controlPoints2[i]
            cp2 = MayaCmds.getAttr('surfaceInfo1.controlPoints[%d]' % (i))
            self.failUnlessAlmostEqual(cp1[0], cp2[0][0], 3, errmsg % (i, 'x'))
            self.failUnlessAlmostEqual(cp1[1], cp2[0][1], 3, errmsg % (i, 'y'))
            self.failUnlessAlmostEqual(cp1[2], cp2[0][2], 3, errmsg % (i, 'z'))

        errmsg = "At frame #12, Nurbs Plane's control knotsU #%d not equal"
        for i in range(0, len(knotsU2)):
            ku1 = knotsU2[i]
            ku2 = MayaCmds.getAttr('surfaceInfo1.knotsU[%d]' % (i))
            self.failUnlessAlmostEqual(ku1, ku2, 3, errmsg % (i))

        errmsg = "At frame #12, Nurbs Plane's control knotsV #%d not equal"
        for i in range(0, len(knotsV2)):
            kv1 = knotsV2[i]
            kv2 = MayaCmds.getAttr('surfaceInfo1.knotsV[%d]' % (i))
            self.failUnlessAlmostEqual(kv1, kv2, 3, errmsg % (i))

        MayaCmds.currentTime(24, update=True)
        errmsg = "At frame #24, Nurbs Plane's control point #%d.%s not equal"
        for i in range(0, len(controlPoints)):
            cp1 = controlPoints[i]
            cp2 = MayaCmds.getAttr('surfaceInfo1.controlPoints[%d]' % (i))
            self.failUnlessAlmostEqual(cp1[0], cp2[0][0], 3, errmsg % (i, 'x'))
            self.failUnlessAlmostEqual(cp1[1], cp2[0][1], 3, errmsg % (i, 'y'))
            self.failUnlessAlmostEqual(cp1[2], cp2[0][2], 3, errmsg % (i, 'z'))

        errmsg = "At frame #24, Nurbs Plane's control knotsU #%d not equal"
        for i in range(0, len(knotsU)):
            ku1 = knotsU[i]
            ku2 = MayaCmds.getAttr('surfaceInfo1.knotsU[%d]' % (i))
            self.failUnlessAlmostEqual(ku1, ku2, 3, errmsg % (i))

        errmsg = "At frame #24, Nurbs Plane's control knotsV #%d not equal"
        for i in range(0, len(knotsV)):
            kv1 = knotsV[i]
            kv2 = MayaCmds.getAttr('surfaceInfo1.knotsV[%d]' % (i))
            self.failUnlessAlmostEqual(kv1, kv2, 3, errmsg % (i))
