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

from maya import cmds as MayaCmds
import os
import subprocess
import unittest
import util

class subframesTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testRangeFlag(self):

        MayaCmds.createNode('transform', name='node')
        MayaCmds.setKeyframe('node.translateX', time=1.0, v=1.0)
        MayaCmds.setKeyframe('node.translateX', time=11.0, v=11.0)

        self.__files.append(util.expandFileName('rangeTest.abc'))
        MayaCmds.AbcExport(j='-fr 1 11 -step 0.25 -root node -file ' + self.__files[-1])

        MayaCmds.AbcImport(self.__files[-1], m='open')

        abcNodeName = MayaCmds.ls(exactType='AlembicNode')

        MayaCmds.currentTime(0, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.translateX'), 1)

        MayaCmds.currentTime(1, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.translateX'), 1)

        MayaCmds.currentTime(1.0003, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.translateX'), 1)

        MayaCmds.currentTime(1.333333, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('node.translateX'),
            1.333333333, 2)

        MayaCmds.currentTime(9.66667, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('node.translateX'),
            9.6666666666, 2)

        MayaCmds.currentTime(11, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.translateX'), 11)

        MayaCmds.currentTime(12, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.translateX'), 11)

    def testPreRollStartFrameFlag(self):

        MayaCmds.createNode('transform', name='node')
        MayaCmds.setAttr('node.tx', 0.0)
        MayaCmds.expression(
            string="if(time==0)\n\tnode.tx=0;\n\nif (time*24 > 6 && node.tx > 0.8)\n\tnode.tx = 10;\n\nnode.tx = node.tx + time;\n",
            name="startAtExp", ae=1, uc=all)

        self.__files.append(util.expandFileName('startAtTest.abc'))
        MayaCmds.AbcExport(j='-fr 1 10 -root node -file ' + self.__files[-1], prs=0, duf=True)

        MayaCmds.AbcImport(self.__files[-1], m='open')

        abcNodeName = MayaCmds.ls(exactType='AlembicNode')

        # if the evaluation doesn't start at frame 0, node.tx < 10
        MayaCmds.currentTime(10, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        self.failUnless(MayaCmds.getAttr('node.translateX')-10 > 0)

    def testSkipFrames(self):

        MayaCmds.createNode('transform', name='node')
        MayaCmds.setKeyframe('node.translateX', time=1.0, v=1.0)
        MayaCmds.setKeyframe('node.translateX', time=10.0, v=10.0)

        MayaCmds.duplicate(name='dupNode')
        MayaCmds.setAttr('dupNode.tx', 0.0)
        MayaCmds.expression(
            string="if(time==11)\n\tdupNode.tx=-50;\n\ndupNode.tx = dupNode.tx + time;\n",
            name="startAtExp", ae=1, uc=all)

        self.__files.append(util.expandFileName('skipFrameTest1.abc'))
        self.__files.append(util.expandFileName('skipFrameTest2.abc'))

        MayaCmds.AbcExport(j=['-fr 1 10 -root node -file ' + self.__files[-2],
            '-fr 20 25 -root dupNode -file ' + self.__files[-1]])

        MayaCmds.AbcImport(self.__files[-2], m='open')

        abcNodeName = MayaCmds.ls(exactType='AlembicNode')

        # make sure all the frames needed are written out and correctly
        for val in range(1, 11):
            MayaCmds.currentTime(val, update=True)
            MayaCmds.dgeval(abcNodeName, verbose=False)
            self.failUnlessAlmostEqual(MayaCmds.getAttr('node.tx'), val, 3)

        # also make sure nothing extra gets written out
        MayaCmds.currentTime(11, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.tx'), 10.0)

        MayaCmds.AbcImport(self.__files[-1], m='open')

        abcNodeName = MayaCmds.ls(exactType='AlembicNode')

        # if dontSkipFrames flag is not set maya would evaluate frame 11 and
        # set dupNode.tx to a big negative number
        MayaCmds.currentTime(20, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        self.failUnless(MayaCmds.getAttr('dupNode.tx') > 0)

    def testWholeFrameGeoFlag(self):

        MayaCmds.polyCube(name='node')
        MayaCmds.setKeyframe('node.translateX', time=1.0, v=1.0)
        MayaCmds.setKeyframe('node.translateX', time=2.0, v=-3.0)
        MayaCmds.setKeyframe('node.translateX', time=5.0, v=9.0)

        MayaCmds.select('node.vtx[0:8]')
        MayaCmds.setKeyframe(time=1.0)
        MayaCmds.scale(1.5, 1.5, 1.8)
        MayaCmds.setKeyframe(time=5.0)

        self.__files.append(util.expandFileName('noSampleGeoTest.abc'))
        MayaCmds.AbcExport(j='-fr 1 5 -wfg -frs 0 -frs 0.9 -root node -file ' + self.__files[-1])

        MayaCmds.AbcImport(self.__files[-1], m='open')

        abcNodeName = MayaCmds.ls(exactType='AlembicNode')

        setTime = MayaCmds.currentTime(1, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        val_1 = MayaCmds.getAttr('node.vt[0]')[0][0]

        MayaCmds.currentTime(2.0, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        MayaCmds.getAttr('node.vt[0]')
        val_2 = MayaCmds.getAttr('node.vt[0]')[0][0]
        self.failUnlessAlmostEqual(val_2, -0.5625, 3)

        setTime = MayaCmds.currentTime(1.9, update=True)
        MayaCmds.dgeval(abcNodeName, verbose=False)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('node.tx'), -3.086, 3)
        # the vertex will get linearly interpolated
        alpha = (setTime - 1) / (2 - 1)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('node.vt[0]')[0][0],
                                   (1-alpha)*val_1+alpha*val_2, 3)

    # convenience functions for the tests following

    def noFrameRangeExists(self, fileName):
        #TODO make sure we just have the default time sampling (0)
        pass

    def isFrameRangeExists(self, fileName):
        #TODO make sure we have 1 other time sampling
        pass

    def isFrameRangeTransAndFrameRangeShapeExists(self, fileName):
        #TODO make sure we have 2 other time samplings
        pass

    def test_agat(self):

        # animated geometry, animated transform node
        nodename = 'agat_node'
        MayaCmds.polyCube(name=nodename)
        MayaCmds.setKeyframe(nodename+'.translateX', time=1.0, v=1.0)
        MayaCmds.setKeyframe(nodename+'.translateX', time=5.0, v=10.0)

        MayaCmds.select(nodename+'.vtx[0:8]')
        MayaCmds.setKeyframe(time=1.0)
        MayaCmds.scale(1.5, 1.5, 1.8)
        MayaCmds.setKeyframe(time=5.0)

        self.__files.append(util.expandFileName('agat_motionblur_noSampleGeo_Test.abc'))
        MayaCmds.AbcExport(j='-fr 1 5 -wfg -step 0.5 -root %s -file %s' % (
            nodename, self.__files[-1]))

        # frameRangeShape: 1, 2, 3, 4, 5, 6
        # frameRangeTrans: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6
        self.isFrameRangeTransAndFrameRangeShapeExists(self.__files[-1])

        self.__files.append(util.expandFileName('agat_motionblur_Test.abc'))
        MayaCmds.AbcExport(j='-fr 1 5 -step 0.5 -root %s -file %s' % (
            nodename, self.__files[-1]))

        # frameRange: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6
        self.isFrameRangeExists(self.__files[-1])

        self.__files.append(util.expandFileName('agat_norange_Test.abc'))
        MayaCmds.AbcExport(j='-root %s -f %s' % (nodename,self.__files[-1]))

        # no frameRange
        self.noFrameRangeExists(self.__files[-1])

    def test_agst(self):

        # animated geometry, static transform node
        nodename = 'agst_node'
        MayaCmds.polyCube(name=nodename)
        MayaCmds.select(nodename+'.vtx[0:8]')
        MayaCmds.setKeyframe(time=1.0)
        MayaCmds.scale(1.5, 1.5, 1.8)
        MayaCmds.setKeyframe(time=5.0)

        self.__files.append(util.expandFileName('agst_motionblur_noSampleGeo_Test.abc'))
        MayaCmds.AbcExport(j='-fr 1 5 -step 0.5 -wfg -root %s -file %s' % (
            nodename, self.__files[-1]))

        # frameRange: 1, 2, 3, 4, 5, 6
        self.isFrameRangeTransAndFrameRangeShapeExists(self.__files[-1])

        self.__files.append(util.expandFileName('agst_motionblur_Test.abc'))
        MayaCmds.AbcExport(j='-fr 1 5 -step 0.5 -root %s -f %s' % (
            nodename, self.__files[-1]))

        # frameRange: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6
        self.isFrameRangeExists(self.__files[-1])

        self.__files.append(util.expandFileName('agst_noSampleGeo_Test.abc'))
        MayaCmds.AbcExport(j='-fr 1 5 -wfg -root %s -f %s' % (nodename,
            self.__files[-1]))

        # frameRange: 1, 2, 3, 4, 5
        self.isFrameRangeExists(self.__files[-1])

    def test_sgat(self):
        # static geometry, animated transform node
        nodename = 'sgat_node'
        MayaCmds.polyCube(name=nodename)
        MayaCmds.setKeyframe(nodename+'.translateX', time=1.0, v=1.0)
        MayaCmds.setKeyframe(nodename+'.translateX', time=5.0, v=10.0)

        self.__files.append(util.expandFileName('sgat_motionblur_noSampleGeo_Test.abc'))
        MayaCmds.AbcExport(j='-fr 1 5 -step 0.5 -wfg -root %s -f %s' % (
            nodename, self.__files[-1]))

        # frameRange: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6
        self.isFrameRangeTransAndFrameRangeShapeExists(self.__files[-1])

        self.__files.append(util.expandFileName('sgat_motionblur_Test.abc'))
        MayaCmds.AbcExport(j='-fr 1 5 -step 0.5 -root %s -f %s ' % (
            nodename, self.__files[-1]))
        # frameRange: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6
        self.isFrameRangeExists(self.__files[-1])

    def test_sgst(self):

        # static geometry, static transform node
        nodename = 'sgst_node'
        MayaCmds.polyCube(name=nodename)

        self.__files.append(util.expandFileName('sgst_motionblur_noSampleGeo_Test.abc'))
        MayaCmds.AbcExport(j='-fr 1 5 -step 0.5 -wfg -root %s -file %s ' % (
            nodename, self.__files[-1]))

        self.failIf(MayaCmds.AbcImport(self.__files[-1]) != "")

        self.__files.append(util.expandFileName('sgst_moblur_noSampleGeo_norange_Test.abc'))
        MayaCmds.AbcExport(j='-step 0.5 -wfg -root %s -file %s' % (
            nodename, self.__files[-1]))

        # frameRange: NA
        self.noFrameRangeExists(self.__files[-1])
