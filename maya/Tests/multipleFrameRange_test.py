##-*****************************************************************************
##
## Copyright (c) 2009-2013,
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
# Test multiple frame range support
# We allow using multiple frame range within a job.
#
class MultipleFrameRangeTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        MayaCmds.file(new=True, force=True)
        for f in self.__files:
            os.remove(f)

    def createXformNode(self):
        xform = MayaCmds.createNode('transform', n = 'test')
        MayaCmds.setKeyframe(xform, attribute = 'translateX', itt = 'linear', ott = 'linear', t = 1, value = 10)
        MayaCmds.setKeyframe(xform, attribute = 'translateX', itt = 'linear', ott = 'linear', t = 3, value = 20)
        MayaCmds.setKeyframe(xform, attribute = 'translateX', itt = 'linear', ott = 'linear', t = 10, value = 20)
        MayaCmds.setKeyframe(xform, attribute = 'translateX', itt = 'linear', ott = 'linear', t = 11, value = 10)
        MayaCmds.setKeyframe(xform, attribute = 'translateX', itt = 'linear', ott = 'linear', t = 100, value = 20)
        MayaCmds.setKeyframe(xform, attribute = 'translateX', itt = 'linear', ott = 'linear', t = 110, value = 20)
        MayaCmds.setKeyframe(xform, attribute = 'translateX', itt = 'linear', ott = 'linear', t = 200, value = 10)

        #
        #20    ------        ____------___
        #    /        \  ___/             \___
        #10 /          \/                     \
        #   1  3    10 11       100  110      200

    def createCubeNode(self):
        cube, polyCube = MayaCmds.polyCube(n = 'cube')
        for i in range(1, 11):
            if i / 2 == 0:
                MayaCmds.setKeyframe(polyCube, attribute = 'width', itt = 'spline', ott = 'spline', t = i, value = 10)
            else:
                MayaCmds.setKeyframe(polyCube, attribute = 'width', itt = 'spline', ott = 'spline', t = i, value = 20)
        MayaCmds.parent(cube, 'test', relative = True)


    def failUnlessXformValuesEqual(self, timeValueList):
        for time, value in timeValueList:
            MayaCmds.currentTime(time, update = True)
            self.failUnlessAlmostEqual(
                    value, MayaCmds.getAttr('test.translateX'), 3,
                    'Time: %f, Value: %f (expected) != %f' % (time, value, MayaCmds.getAttr('test.translateX')))

    def failUnlessCubeWidthEqual(self, timeValueList):
        for time, value in timeValueList:
            MayaCmds.currentTime(time, update = True)
            bbox = MayaCmds.exactWorldBoundingBox('cube')
            width = bbox[3] - bbox[0]
            self.failUnlessAlmostEqual(
                    value, width, 3,
                    'Time: %f, Width: %f (expected) != %f' % (time, value, width))

    def testThreeFrameRanges(self):
        self.createXformNode()

        # Export 3 frame ranges: [1,3], [10,11], [110, 200]
        __builtins__['framesList'] = []
        self.__files.append(util.expandFileName('testThreeFrameRanges.abc'))
        MayaCmds.AbcExport(j = '-fr 1 3 -fr 10 11 -fr 110 200 -root test -pfc framesList.append(#FRAME#) -file ' + self.__files[-1])

        referenceList = []
        for i in range(1, 4):
            referenceList.append(i)
        for i in range(10, 12):
            referenceList.append(i)
        for i in range(110, 201):
            referenceList.append(i)
        self.failUnlessEqual(framesList, referenceList)

        # Import
        MayaCmds.AbcImport(self.__files[-1], mode = 'open')

        # Test sampling points
        self.failUnlessXformValuesEqual([(1, 10), (3, 20), (10, 20), (11, 10), (110, 20), (200, 10)])

        # Test not-sampled points
        self.failUnlessXformValuesEqual([(7, 20), (60.5, 15)])  # lerp

    def testPreRollRanges(self):
        self.createXformNode()

        # Export (1, 10), [11, 100], (101, 109), [110, 200]
        __builtins__['framesList'] = []
        self.__files.append(util.expandFileName('testPreRollRanges1.abc'))
        MayaCmds.AbcExport(j = '-fr 1 10 -pr -fr 11 100 -fr 101 109 -pr -fr 110 200 -root test -pfc framesList.append(#FRAME#) -file ' + self.__files[-1])

        referenceList = []
        for i in range(11, 101):
            referenceList.append(i)
        for i in range(110, 201):
            referenceList.append(i)
        self.failUnlessEqual(framesList, referenceList)

        # Import
        MayaCmds.AbcImport(self.__files[-1], mode = 'open')

        # Test sampling points
        self.failUnlessXformValuesEqual([(11, 10), (100, 20), (110, 20), (200, 10)])

        # Test not-sampled points
        self.failUnlessXformValuesEqual([(1, 10), (105, 20), (205, 10)])

        # Another pre roll test
        MayaCmds.file(new = True, force = True)
        self.createXformNode()

        # Export [1, 10], (11, 99), [100, 110], (111, 200)
        __builtins__['framesList'] = []
        self.__files.append(util.expandFileName('testPreRollRanges2.abc'))
        MayaCmds.AbcExport(j = '-fr 1 10 -fr 11 99 -pr -fr 100 110 -fr 111 200 -pr -root test -pfc framesList.append(#FRAME#) -file ' + self.__files[-1])

        referenceList = []
        for i in range(1, 11):
            referenceList.append(i)
        for i in range(100, 111):
            referenceList.append(i)
        self.failUnlessEqual(framesList, referenceList)

        # Import
        MayaCmds.AbcImport(self.__files[-1], mode = 'open')

        # Test sampling points
        self.failUnlessXformValuesEqual([(1, 10), (3, 20), (10, 20), (100, 20), (110, 20)])

        # Test not-sampled points
        self.failUnlessXformValuesEqual([(11, 20), (50, 20), (200, 20)])

    def testStep(self):
        self.createXformNode()

        # Export [1, 10]:5 [100, 200]:10
        __builtins__['framesList'] = []
        self.__files.append(util.expandFileName('testStepRanges.abc'))
        MayaCmds.AbcExport(j = '-fr 0 10 -s 5 -fr 100 200 -s 10 -root test -pfc framesList.append(#FRAME#) -file ' + self.__files[-1])

        referenceList = []
        for i in range(0, 11, 5):
            referenceList.append(i)
        for i in range(100, 201, 10):
            referenceList.append(i)
        self.failUnlessEqual(framesList, referenceList)

        # Import
        MayaCmds.AbcImport(self.__files[-1], mode = 'open')

        # Test sampling points
        self.failUnlessXformValuesEqual([(0, 10), (5, 20), (10, 20), (100, 20), (110, 20), (200, 10)])

        # Test not-sampled points
        self.failUnlessXformValuesEqual([(2.5, 15)])  # lerp

    def testFrameRelativeSample(self):
        self.createXformNode()

        # Export [3, 3]{-1, 0, 1} [100, 190]{10}:30
        # i.e. 2, 3, 4, 110, 140, 170, 200
        __builtins__['framesList'] = []
        self.__files.append(util.expandFileName('testFrameRelativeSamples.abc'))
        MayaCmds.AbcExport(j = '-fr 3 3 -frs -1 -frs 0 -frs 1 -fr 100 190 -s 30 -frs 10 -root test -pfc framesList.append(#FRAME#) -file ' + self.__files[-1])

        self.failUnlessEqual(framesList, [2, 3, 4, 110, 140, 170, 200])

        # Import
        MayaCmds.AbcImport(self.__files[-1], mode = 'open')

        # Test sampling points
        self.failUnlessXformValuesEqual([(2, 15), (3, 20), (4, 20), (110, 20), (140, 20-10.0/3), (170, 20-20.0/3), (200, 10)])

        # Test not-sampled points
        self.failUnlessXformValuesEqual([(1, 15), (10, 20), (100, 20)])


    def testWholeFrameGeo(self):
        self.createXformNode()
        self.createCubeNode()

        # Export [2, 4]{-0.2, 0, 0.2}:2 [10, 10]{-0.5, 0, 0.5} -wfg
        # i.e. xform: 1.8, 2, 2.2, 3.8, 4, 4.2, 9.5, 10, 10.5
        #   geometry: 2, 4, 10
        __builtins__['framesList'] = []
        self.__files.append(util.expandFileName('testWholeFrameGeos.abc'))
        MayaCmds.AbcExport(j = '-fr 2 4 -s 2 -frs -0.2 -frs 0 -frs 0.2 -fr 10 10 -frs -0.5 -frs 0 -frs 0.5 -wfg -root test -pfc framesList.append(#FRAME#) -file ' + self.__files[-1])

        self.failUnlessEqual(framesList, [1.8, 2, 2.2, 3.8, 4, 4.2, 9.5, 10, 10.5])

        # Import
        MayaCmds.AbcImport(self.__files[-1], mode = 'open')

        # Test sampling xform points
        self.failUnlessXformValuesEqual([(1.8, 14), (2, 15), (2.2, 16), (3.8, 20), (4, 20), (4.2, 20), (9.5, 20), (10, 20), (10.5, 15)])

        # Test not-sampled xform points
        self.failUnlessXformValuesEqual([(2.8, 17.5), (3, 18), (3.2, 18.5)]) # lerp

        # Test sampling mesh points
        self.failUnlessCubeWidthEqual([(2, 20), (4, 20), (10, 20)])

        # Test not-sampled cube points
        # No subsample for the cube
        self.failUnlessCubeWidthEqual([(2.2, 20), (3.8, 20), (4.2, 20)])

