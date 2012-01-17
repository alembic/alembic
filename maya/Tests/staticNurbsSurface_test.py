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

def testStaticNurbsWithoutTrim(self, surfacetype, abcFileName):

    if (surfacetype == 0):
        ret  = MayaCmds.nurbsPlane(p=(0, 0, 0), ax=(0, 1, 0), w=1, lr=1, d=3,
            u=5, v=5, ch=0)
        errmsg = 'Nurbs Plane'
    elif (surfacetype == 1):
        ret = MayaCmds.sphere(p=(0, 0, 0), ax=(0, 1, 0), ssw=0, esw=360, r=1,
            d=3, ut=0, tol=0.01, s=8, nsp=4, ch=0)
        errmsg = 'Nurbs Sphere'
    elif (surfacetype == 2):
        ret = MayaCmds.torus(p=(0, 0, 0), ax=(0, 1, 0), ssw=0, esw=360,
            msw=360, r=1, hr=0.5, ch=0)
        errmsg = 'Nurbs Torus'

    name = ret[0]

    degreeU = MayaCmds.getAttr(name+'.degreeU')
    degreeV = MayaCmds.getAttr(name+'.degreeV')
    spansU  = MayaCmds.getAttr(name+'.spansU')
    spansV  = MayaCmds.getAttr(name+'.spansV')
    minU    = MayaCmds.getAttr(name+'.minValueU')
    maxU    = MayaCmds.getAttr(name+'.maxValueU')
    minV    = MayaCmds.getAttr(name+'.minValueV')
    maxV    = MayaCmds.getAttr(name+'.maxValueV')

    surfaceInfoNode = MayaCmds.createNode('surfaceInfo')
    MayaCmds.connectAttr(name+'.worldSpace', surfaceInfoNode+'.inputSurface',
        force=True)

    controlPoints = MayaCmds.getAttr(surfaceInfoNode+'.controlPoints[*]')
    knotsU = MayaCmds.getAttr(surfaceInfoNode+'.knotsU[*]')
    knotsV = MayaCmds.getAttr(surfaceInfoNode+'.knotsV[*]')

    MayaCmds.AbcExport(j='-root %s -f %s' % (name, abcFileName))

    # reading test
    MayaCmds.AbcImport(abcFileName, mode='open')

    self.failUnlessEqual(degreeU, MayaCmds.getAttr(name+'.degreeU'))
    self.failUnlessEqual(degreeV, MayaCmds.getAttr(name+'.degreeV'))
    self.failUnlessEqual(spansU, MayaCmds.getAttr(name+'.spansU'))
    self.failUnlessEqual(spansV, MayaCmds.getAttr(name+'.spansV'))
    self.failUnlessEqual(minU, MayaCmds.getAttr(name+'.minValueU'))
    self.failUnlessEqual(maxU, MayaCmds.getAttr(name+'.maxValueU'))
    self.failUnlessEqual(minV, MayaCmds.getAttr(name+'.minValueV'))
    self.failUnlessEqual(maxV, MayaCmds.getAttr(name+'.maxValueV'))


    if (surfacetype == 0):
        self.failUnlessEqual(0, MayaCmds.getAttr(name+'.formU'))
        self.failUnlessEqual(0, MayaCmds.getAttr(name+'.formV'))
    elif (surfacetype == 1):
        self.failUnlessEqual(0, MayaCmds.getAttr(name+'.formU'))
        self.failUnlessEqual(2, MayaCmds.getAttr(name+'.formV'))
    elif (surfacetype == 2):
        self.failUnlessEqual(2, MayaCmds.getAttr(name+'.formU'))
        self.failUnlessEqual(2, MayaCmds.getAttr(name+'.formV'))


    surfaceInfoNode = MayaCmds.createNode('surfaceInfo')
    MayaCmds.connectAttr(name+'.worldSpace', surfaceInfoNode+'.inputSurface',
        force=True)

    controlPoints2 = MayaCmds.getAttr(surfaceInfoNode + '.controlPoints[*]')
    self.failUnlessEqual(len(controlPoints), len(controlPoints2))
    for i in range(0, len(controlPoints)):
        cp1 = controlPoints[i]
        cp2 = controlPoints2[i]
        self.failUnlessAlmostEqual(cp1[0], cp2[0], 3, 'cp[%d].x not equal' % i)
        self.failUnlessAlmostEqual(cp1[1], cp2[1], 3, 'cp[%d].y not equal' % i)
        self.failUnlessAlmostEqual(cp1[2], cp2[2], 3, 'cp[%d].z not equal' % i)

    for i in range(0, len(knotsU)):
        ku1 = knotsU[i]
        ku2 = MayaCmds.getAttr('surfaceInfo1.knotsU[%d]' % i)
        self.failUnlessAlmostEqual(ku1, ku2, 3,
            'control knotsU # %d not equal' % i)

    for i in range(0, len(knotsV)):
        kv1 = knotsV[i]
        kv2 = MayaCmds.getAttr('surfaceInfo1.knotsV[%d]' % i)
        self.failUnlessAlmostEqual(kv1, kv2, 3,
            'control knotsV # %d not equal' % i)

def testStaticNurbsWithOneCloseCurveTrim(self, surfacetype, abcFileName,
    trimtype):

    if (surfacetype == 0):
        ret  = MayaCmds.nurbsPlane(p=(0, 0, 0), ax=(0, 1, 0), w=1, lr=1,
            d=3, u=5, v=5, ch=0)
    elif (surfacetype == 1):
        ret  = MayaCmds.sphere(p=(0, 0, 0), ax=(0, 1, 0), ssw=0, esw=360, r=1,
            d=3, ut=0, tol=0.01, s=8, nsp=4, ch=0)
    elif (surfacetype == 2):
        ret = MayaCmds.torus(p=(0, 0, 0), ax=(0, 1, 0), ssw=0, esw=360,
            msw=360, r=1, hr=0.5, ch=0)

    name = ret[0]

    MayaCmds.curveOnSurface(name, uv=((0.170718,0.565967),
        (0.0685088,0.393034), (0.141997,0.206296), (0.95,0.230359),
        (0.36264,0.441381), (0.251243,0.569889)),
        k=(0,0,0,0.200545,0.404853,0.598957,0.598957,0.598957))
    MayaCmds.closeCurve(name+'->curve1', ch=1, ps=1, rpo=1, bb=0.5, bki=0,
        p=0.1, cos=1)

    if trimtype == 0 :
        MayaCmds.trim(name, lu=0.68, lv=0.39)
    elif 1 :
        MayaCmds.trim(name, lu=0.267062, lv=0.39475)

    degreeU = MayaCmds.getAttr(name+'.degreeU')
    degreeV = MayaCmds.getAttr(name+'.degreeV')
    spansU = MayaCmds.getAttr(name+'.spansU')
    spansV = MayaCmds.getAttr(name+'.spansV')
    formU = MayaCmds.getAttr(name+'.formU')
    formV = MayaCmds.getAttr(name+'.formV')
    minU = MayaCmds.getAttr(name+'.minValueU')
    maxU = MayaCmds.getAttr(name+'.maxValueU')
    minV = MayaCmds.getAttr(name+'.minValueV')
    maxV = MayaCmds.getAttr(name+'.maxValueV')

    surfaceInfoNode = MayaCmds.createNode('surfaceInfo')
    MayaCmds.connectAttr(name+'.worldSpace', surfaceInfoNode+'.inputSurface',
        force=True)

    controlPoints = MayaCmds.getAttr(surfaceInfoNode+'.controlPoints[*]')
    knotsU = MayaCmds.getAttr(surfaceInfoNode+'.knotsU[*]')
    knotsV = MayaCmds.getAttr(surfaceInfoNode+'.knotsV[*]')

    MayaCmds.AbcExport(j='-root %s -f %s' % (name, abcFileName))
    MayaCmds.AbcImport(abcFileName, mode='open')

    self.failUnlessEqual(degreeU, MayaCmds.getAttr(name+'.degreeU'))
    self.failUnlessEqual(degreeV, MayaCmds.getAttr(name+'.degreeV'))
    self.failUnlessEqual(spansU, MayaCmds.getAttr(name+'.spansU'))
    self.failUnlessEqual(spansV, MayaCmds.getAttr(name+'.spansV'))
    self.failUnlessEqual(minU, MayaCmds.getAttr(name+'.minValueU'))
    self.failUnlessEqual(maxU, MayaCmds.getAttr(name+'.maxValueU'))
    self.failUnlessEqual(minV, MayaCmds.getAttr(name+'.minValueV'))
    self.failUnlessEqual(maxV, MayaCmds.getAttr(name+'.maxValueV'))

    surfaceInfoNode = MayaCmds.createNode('surfaceInfo')
    MayaCmds.connectAttr(name+'.worldSpace', surfaceInfoNode+'.inputSurface',
        force=True)

    controlPoints2 = MayaCmds.getAttr( surfaceInfoNode + '.controlPoints[*]')
    self.failUnlessEqual(len(controlPoints), len(controlPoints2))
    for i in range(0, len(controlPoints)):
        cp1 = controlPoints[i]
        cp2 = controlPoints2[i]
        self.failUnlessAlmostEqual(cp1[0], cp2[0], 3, 'cp[%d].x not equal' % i)
        self.failUnlessAlmostEqual(cp1[1], cp2[1], 3, 'cp[%d].y not equal' % i)
        self.failUnlessAlmostEqual(cp1[2], cp2[2], 3, 'cp[%d].z not equal' % i)

    for i in range(0, len(knotsU)):
        ku1 = knotsU[i]
        ku2 = MayaCmds.getAttr('surfaceInfo1.knotsU[%d]' % i)
        self.failUnlessAlmostEqual(ku1, ku2, 3,
            'control knotsU # %d not equal' % i)

    for i in range(0, len(knotsV)):
        kv1 = knotsV[i]
        kv2 = MayaCmds.getAttr('surfaceInfo1.knotsV[%d]' % i)
        self.failUnlessAlmostEqual(kv1, kv2, 3,
            'control knotsV # %d not equal' % i)

class StaticNurbsSurfaceTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files :
            os.remove(f)

    def testStaticNurbsSurfaceWithoutTrimReadWrite(self):

        # open - open surface
        self.__files.append(util.expandFileName('testStaticNurbsPlaneWithoutTrim.abc'))
        testStaticNurbsWithoutTrim(self, 0, self.__files[-1])


        # open - periodic surface
        self.__files.append(util.expandFileName('testStaticNurbsSphereWithoutTrim.abc'))
        testStaticNurbsWithoutTrim(self, 1, self.__files[-1])


        # periodic - periodic surface
        self.__files.append(util.expandFileName('testStaticNurbsTorusWithoutTrim.abc'))
        testStaticNurbsWithoutTrim(self, 2 , self.__files[-1])

    def testStaticNurbsSurfaceWithOneCloseCurveTrimInsideReadWrite(self):

        # open - open surface
        self.__files.append(util.expandFileName('testStaticNurbsPlaneWithOneCloseCurveTrimInside.abc'))
        testStaticNurbsWithOneCloseCurveTrim(self, 0 , self.__files[-1], 0)


        # open - periodic surface
        self.__files.append(util.expandFileName('testStaticNurbsSphereWithOneCloseCurveTrimInside.abc'))
        testStaticNurbsWithOneCloseCurveTrim(self, 1, self.__files[-1], 0)


        # periodic - periodic surface
        self.__files.append(util.expandFileName('testStaticNurbsTorusWithOneCloseCurveTrimInside.abc'))
        testStaticNurbsWithOneCloseCurveTrim(self, 2, self.__files[-1], 0)

    def testStaticNurbsSurfaceWithOneCloseCurveTrimOutsideReadWrite(self):

        # open - open surface
        self.__files.append(util.expandFileName('testStaticNurbsPlaneWithOneCloseCurveTrimOutside.abc'))
        testStaticNurbsWithOneCloseCurveTrim(self, 0, self.__files[-1], 1)


        # open - periodic surface
        self.__files.append(util.expandFileName('testStaticNurbsSphereWithOneCloseCurveTrimOutside.abc'))
        testStaticNurbsWithOneCloseCurveTrim(self, 1, self.__files[-1], 1)


        # periodic - periodic surface
        self.__files.append(util.expandFileName('testStaticNurbsTorusWithOneCloseCurveTrimOutside.abc'))
        testStaticNurbsWithOneCloseCurveTrim(self, 2, self.__files[-1], 1)

    def testStaticNurbsPlaneWithOneSegmentTrimReadWrite(self):

        ret  = MayaCmds.nurbsPlane(p=(0, 0, 0), ax=(0, 1, 0), w=1, lr=1, d=3,
            u=5, v=5, ch=0)

        name = ret[0]

        MayaCmds.curveOnSurface(name, uv=((0.597523,0), (0.600359,0.271782),
            (0.538598,0.564218), (0.496932,0.779936),  (0.672153,1)),
            k=(0,0,0,0.263463,0.530094,0.530094,0.530094))
        MayaCmds.trim(name, lu=0.68, lv=0.39)

        degreeU = MayaCmds.getAttr(name+'.degreeU')
        degreeV = MayaCmds.getAttr(name+'.degreeV')
        spansU = MayaCmds.getAttr(name+'.spansU')
        spansV = MayaCmds.getAttr(name+'.spansV')
        minU = MayaCmds.getAttr(name+'.minValueU')
        maxU = MayaCmds.getAttr(name+'.maxValueU')
        minV = MayaCmds.getAttr(name+'.minValueV')
        maxV = MayaCmds.getAttr(name+'.maxValueV')

        MayaCmds.createNode('surfaceInfo')
        MayaCmds.connectAttr(name+'.worldSpace', 'surfaceInfo1.inputSurface',
            force=True)

        controlPoints = MayaCmds.getAttr('surfaceInfo1.controlPoints[*]')
        knotsU = MayaCmds.getAttr('surfaceInfo1.knotsU[*]')
        knotsV = MayaCmds.getAttr('surfaceInfo1.knotsV[*]')

        self.__files.append(util.expandFileName('testStaticNurbsPlaneWithOneSegmentTrim.abc'))
        MayaCmds.AbcExport(j='-root %s -f %s' % (name, self.__files[-1]))

        # reading test
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        self.failUnlessEqual(degreeU, MayaCmds.getAttr(name+'.degreeU'))
        self.failUnlessEqual(degreeV, MayaCmds.getAttr(name+'.degreeV'))
        self.failUnlessEqual(spansU, MayaCmds.getAttr(name+'.spansU'))
        self.failUnlessEqual(spansV, MayaCmds.getAttr(name+'.spansV'))
        self.failUnlessEqual(0, MayaCmds.getAttr(name+'.formU'))
        self.failUnlessEqual(0, MayaCmds.getAttr(name+'.formV'))
        self.failUnlessEqual(minU, MayaCmds.getAttr(name+'.minValueU'))
        self.failUnlessEqual(maxU, MayaCmds.getAttr(name+'.maxValueU'))
        self.failUnlessEqual(minV, MayaCmds.getAttr(name+'.minValueV'))
        self.failUnlessEqual(maxV, MayaCmds.getAttr(name+'.maxValueV'))

        MayaCmds.createNode('surfaceInfo')
        MayaCmds.connectAttr(name+'.worldSpace', 'surfaceInfo1.inputSurface',
            force=True)

        for i in range(0, len(controlPoints)):
            cp1 = controlPoints[i]
            cp2 = MayaCmds.getAttr('surfaceInfo1.controlPoints[%d]' % i)
            self.failUnlessAlmostEqual(cp1[0], cp2[0][0], 3,
                'control point [%d].x not equal' % i)
            self.failUnlessAlmostEqual(cp1[1], cp2[0][1], 3,
                'control point [%d].y not equal' % i)
            self.failUnlessAlmostEqual(cp1[2], cp2[0][2], 3,
                'control point [%d].z not equal' % i)

        for i in range(0, len(knotsU)):
            ku1 = knotsU[i]
            ku2 = MayaCmds.getAttr('surfaceInfo1.knotsU[%d]' % i)
            self.failUnlessAlmostEqual(ku1, ku2, 3,
                'control knotsU # %d not equal' % i)

        for i in range(0, len(knotsV)):
            kv1 = knotsV[i]
            kv2 = MayaCmds.getAttr('surfaceInfo1.knotsV[%d]' % i)
            self.failUnlessAlmostEqual(kv1, kv2, 3,
                'control knotsV # %d not equal' % i)
