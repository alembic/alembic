
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
import subprocess
import unittest

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

        self.__files.append('/tmp/rangeTest.hdf')
        MayaCmds.AlembicTakoExport('range 1 10 shutterOpen 0.0 shutterClose 1.0 ' +
            'numSamples 4 node ' + self.__files[-1])

        MayaCmds.AlembicTakoImport(self.__files[-1], m='open', timeRange=(1, 10), debug=False)

        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])

        MayaCmds.currentTime(0, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.translateX'), 1)

        MayaCmds.currentTime(1, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.translateX'), 1)

        MayaCmds.currentTime(1.3, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.translateX'), 1)

        MayaCmds.currentTime(1.333333, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('node.translateX'),
            1.333333333, 2)

        MayaCmds.currentTime(9.66667, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('node.translateX'),
            9.6666666666, 2)

        MayaCmds.currentTime(11, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.translateX'), 11)

        MayaCmds.currentTime(12, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.translateX'), 11)

    def testStartAtFlag(self):

        MayaCmds.createNode('transform', name='node')
        MayaCmds.setAttr('node.tx', 0.0)
        MayaCmds.expression(
            string="if(time==0)\n\tnode.tx=0;\n\nif (time*24 > 6 && node.tx > 0.8)\n\tnode.tx = 10;\n\nnode.tx = node.tx + time;\n",
            name="startAtExp", ae=1, uc=all)

        self.__files.append('/tmp/startAtTest.hdf')
        MayaCmds.AlembicTakoExport('range 1 10 node ' + self.__files[-1], startAt=0)

        MayaCmds.AlembicTakoImport(self.__files[-1], m='open', timeRange=(1, 10))

        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])

        # if the evaluation doesn't start at frame 0, node.tx < 10
        MayaCmds.currentTime(10, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnless(MayaCmds.getAttr('node.translateX')-10 > 0)

    def testSkipFrameFlag(self):

        MayaCmds.createNode('transform', name='node')
        MayaCmds.setKeyframe('node.translateX', time=1.0, v=1.0)
        MayaCmds.setKeyframe('node.translateX', time=10.0, v=10.0)

        MayaCmds.duplicate(name='dupNode')
        MayaCmds.setAttr('dupNode.tx', 0.0)
        MayaCmds.expression(
            string="if(time==11)\n\tdupNode.tx=-50;\n\ndupNode.tx = dupNode.tx + time;\n",
            name="startAtExp", ae=1, uc=all)

        self.__files.append('/tmp/skipFrameTest1.hdf')
        self.__files.append('/tmp/skipFrameTest2.hdf')

        MayaCmds.AlembicTakoExport('range 1 10 node ' + self.__files[-2] +
            '; range 20 25 dupNode ' + self.__files[-1], skipFrame=True)

        MayaCmds.AlembicTakoImport(self.__files[-2], m='open', timeRange=(1, 10))

        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])

        # make sure all the frames needed are written out and correctly
        for val in range(1, 11):
            MayaCmds.currentTime(val, update=True)
            MayaCmds.dgeval(takoNodeName, verbose=False)
            self.failUnlessAlmostEqual(MayaCmds.getAttr('node.tx'), val, 3)

        # also make sure nothing extra gets written out
        MayaCmds.currentTime(11, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessEqual(MayaCmds.getAttr('node.tx'), 10.0)

        MayaCmds.AlembicTakoImport(self.__files[-1], m='open', timeRange=(20, 25))

        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])

        # if skipFrame flag is not set, maya would evaluate frame 11 and set
        # dupNode.tx to a big negative number
        MayaCmds.currentTime(20, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnless(MayaCmds.getAttr('dupNode.tx') > 0)

    def testNoSampleGeoFlag(self):

        MayaCmds.polyCube(name='node')
        MayaCmds.setKeyframe('node.translateX', time=1.0, v=1.0)
        MayaCmds.setKeyframe('node.translateX', time=5.0, v=9.0)

        MayaCmds.select('node.vtx[0:8]')
        MayaCmds.setKeyframe(time=1.0)
        MayaCmds.scale(1.5, 1.5, 1.8)
        MayaCmds.setKeyframe(time=5.0)

        self.__files.append('/tmp/noSampleGeoTest.hdf')
        MayaCmds.AlembicTakoExport('range 1 5 noSampleGeo shutterOpen 0.0 ' +
            'shutterClose 0.9 numSamples 2 node ' + self.__files[-1])

        MayaCmds.AlembicTakoImport(self.__files[-1], m='open', timeRange=(1, 5))

        takoNodeName = MayaCmds.ls(exactType=os.environ['AlembicNodeType'])

        MayaCmds.currentTime(1.9, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('node.tx'), 1, 3)
        self.failUnlessAlmostEqual(MayaCmds.getAttr('node.vt[0]')[0][0],
            -0.75, 3)

        MayaCmds.currentTime(2.0, update=True)
        MayaCmds.dgeval(takoNodeName, verbose=False)
        MayaCmds.getAttr('node.vt[0]')
        self.failUnlessAlmostEqual(MayaCmds.getAttr('node.vt[0]')[0][0],
            -0.562500007450580597, 3)

    # convenience functions for the tests following

    def noFrameRangeExists(self, filename):
        retVal = subprocess.call(['h5ls',
            filename + '/root/.prop/frameRangeTrans'])
        self.failUnless(retVal != 0)
        retVal = subprocess.call(['h5ls',
            filename + '/root/.prop/frameRangeShape'])
        self.failUnless(retVal != 0)
        retVal = os.system('h5ls '+filename+'/root/.prop/frameRange')
        self.failUnless(retVal != 0)

    def isFrameRangeExists(self, filename):
        retVal = subprocess.call(['h5ls',
            filename + '/root/.prop/frameRangeTrans'])
        self.failUnless(retVal != 0)
        retVal = subprocess.call(['h5ls',
            filename + '/root/.prop/frameRangeShape'])
        self.failUnless(retVal != 0)
        retVal = os.system('h5ls '+filename+'/root/.prop/frameRange')
        self.failUnless(retVal == 0)

    def isFrameRangeTransAndFrameRangeShapeExists(self, filename):
        retVal = subprocess.call(['h5ls',
            filename + '/root/.prop/frameRangeTrans'])
        self.failUnless(retVal == 0)
        retVal = subprocess.call(['h5ls',
            filename + '/root/.prop/frameRangeShape'])
        self.failUnless(retVal == 0)
        retVal = os.system('h5ls '+filename+'/root/.prop/frameRange')
        self.failUnless(retVal != 0)

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

        self.__files.append('/tmp/agat_motionblur_noSampleGeo_Test.hdf')
        MayaCmds.AlembicTakoExport('range 1 5 noSampleGeo shutterOpen 0.0 ' +
            'shutterClose 0.5 numSamples 2 %s %s' % (nodename,
            self.__files[-1]))

        # frameRangeShape: 1, 2, 3, 4, 5, 6
        # frameRangeTrans: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6
        self.isFrameRangeTransAndFrameRangeShapeExists(self.__files[-1])

        self.__files.append('/tmp/agat_motionblur_Test.hdf')
        MayaCmds.AlembicTakoExport('range 1 5 shutterOpen 0.0 shutterClose 0.5 ' +
            'numSamples 2 %s %s' % (nodename, self.__files[-1]))

        # frameRange: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6
        self.isFrameRangeExists(self.__files[-1])

        self.__files.append('/tmp/agat_norange_Test.hdf')
        MayaCmds.AlembicTakoExport(nodename + ' ' + self.__files[-1])

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

        self.__files.append('/tmp/agst_motionblur_noSampleGeo_Test.hdf')
        MayaCmds.AlembicTakoExport('range 1 5 shutterOpen 0.0 shutterClose 0.5 ' +
            'numSamples 2 noSampleGeo %s %s' % (nodename, self.__files[-1]))

        # frameRange: 1, 2, 3, 4, 5, 6
        self.isFrameRangeExists(self.__files[-1])

        self.__files.append('/tmp/agst_motionblur_Test.hdf')
        MayaCmds.AlembicTakoExport('range 1 5 shutterOpen 0.0 shutterClose 0.5 ' +
            'numSamples 2 %s %s' % (nodename, self.__files[-1]))

        # frameRange: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6
        self.isFrameRangeExists(self.__files[-1])

        self.__files.append('/tmp/agst_noSampleGeo_Test.hdf')
        MayaCmds.AlembicTakoExport('range 1 5 noSampleGeo %s %s' % (nodename,
            self.__files[-1]))

        # frameRange: 1, 2, 3, 4, 5
        self.isFrameRangeExists(self.__files[-1])

    def test_sgat(self):
        # static geometry, animated transform node
        nodename = 'sgat_node'
        MayaCmds.polyCube(name=nodename)
        MayaCmds.setKeyframe(nodename+'.translateX', time=1.0, v=1.0)
        MayaCmds.setKeyframe(nodename+'.translateX', time=5.0, v=10.0)

        self.__files.append('/tmp/sgat_motionblur_noSampleGeo_Test.hdf')
        MayaCmds.AlembicTakoExport('range 1 5 shutterOpen 0.0 shutterClose 0.5 ' +
            'numSamples 2 noSampleGeo %s %s' % (nodename, self.__files[-1]))

        # frameRange: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6
        self.isFrameRangeExists(self.__files[-1])

        self.__files.append('/tmp/sgat_motionblur_Test.hdf')
        MayaCmds.AlembicTakoExport('range 1 5 shutterOpen 0.0 shutterClose 0.5 ' +
            'numSamples 2 %s %s' % (nodename, self.__files[-1]))
        # frameRange: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6
        self.isFrameRangeExists(self.__files[-1])

    def test_sgst(self):

        # static geometry, static transform node
        nodename = 'sgst_node'
        MayaCmds.polyCube(name=nodename)

        self.__files.append('/tmp/sgst_motionblur_noSampleGeo_Test.hdf')
        MayaCmds.AlembicTakoExport('range 1 5 shutterOpen 0.0 shutterClose 0.5 ' +
            'numSamples 2 noSampleGeo %s %s' % (nodename, self.__files[-1]))

        # frameRange: NA
        self.noFrameRangeExists(self.__files[-1])

        self.__files.append('/tmp/sgst_moblur_noSampleGeo_norange_Test.hdf')
        MayaCmds.AlembicTakoExport('shutterOpen 0.0 shutterClose 0.5 numSamples 2 ' +
            'noSampleGeo %s %s' % (nodename, self.__files[-1]))

        # frameRange: NA
        self.noFrameRangeExists(self.__files[-1])
