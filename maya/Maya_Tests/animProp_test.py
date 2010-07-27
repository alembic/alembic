
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

import maya.cmds as MayaCmds
import os
import subprocess
import unittest

class animPropTest(unittest.TestCase):

#-------------------------------------------------------------------------
    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__takoStitcher = [os.environ['AlembicStitcher']]
        self.__files = ['/tmp/animProp.hdf', '/tmp/animProp01_14.hdf',
            '/tmp/animProp15_24.hdf']

#-------------------------------------------------------------------------
    def tearDown(self):
        for f in self.__files :
            os.remove(f)

#-------------------------------------------------------------------------
    def setAndKeyProps( self, nodeName, isMesh=False ):

        MayaCmds.select(nodeName)
        MayaCmds.addAttr(longName='SPT_int8',   defaultValue=0,
            attributeType='byte',   keyable=True)
        MayaCmds.addAttr(longName='SPT_int16',  defaultValue=0,
            attributeType='short',  keyable=True)
        MayaCmds.addAttr(longName='SPT_int32',  defaultValue=0,
            attributeType='long',   keyable=True)
        MayaCmds.addAttr(longName='SPT_float',  defaultValue=0,
            attributeType='float',  keyable=True)
        MayaCmds.addAttr(longName='SPT_double', defaultValue=0,
            attributeType='double', keyable=True)
        MayaCmds.setKeyframe(nodeName, value=0, attribute='SPT_int8',
            t=[1, 24])
        MayaCmds.setKeyframe(nodeName, value=100, attribute='SPT_int16',
            t=[1, 24])
        MayaCmds.setKeyframe(nodeName, value=1000, attribute='SPT_int32',
            t=[1, 24])
        MayaCmds.setKeyframe(nodeName, value=0.57777777, attribute='SPT_float',
            t=[1, 24])
        MayaCmds.setKeyframe(nodeName, value=5.045643545457,
            attribute='SPT_double', t=[1, 24])
        MayaCmds.setKeyframe(nodeName, value=8, attribute='SPT_int8', t=12)
        MayaCmds.setKeyframe(nodeName, value=16, attribute='SPT_int16', t=12)
        MayaCmds.setKeyframe(nodeName, value=32, attribute='SPT_int32',  t=12)
        MayaCmds.setKeyframe(nodeName, value=3.141592654,
            attribute='SPT_float',  t=12)
        MayaCmds.setKeyframe(nodeName, value=3.141592654,
            attribute='SPT_double', t=12)
        if isMesh:
            MayaCmds.addAttr(longName='SPT_FakeColorR', defaultValue=0.0,
                attributeType='float')
            MayaCmds.addAttr(longName='SPT_FakeColorG', defaultValue=0.0,
                attributeType='float')
            MayaCmds.addAttr(longName='SPT_FakeColorB', defaultValue=0.0,
                attributeType='float')
            MayaCmds.setKeyframe(nodeName, value=0,
                attribute='SPT_FakeColorR', t=1)
            MayaCmds.setKeyframe(nodeName, value=0,
                attribute='SPT_FakeColorG', t=1)
            MayaCmds.setKeyframe(nodeName, value=0,
                attribute='SPT_FakeColorB', t=1)
            MayaCmds.setKeyframe(nodeName, value=0.25,
                attribute='SPT_FakeColorR', t=12)
            MayaCmds.setKeyframe(nodeName, value=0.2,
                attribute='SPT_FakeColorG', t=12)
            MayaCmds.setKeyframe(nodeName, value=0.5,
                attribute='SPT_FakeColorB', t=12)
            MayaCmds.setKeyframe(nodeName, value=0.5,
                attribute='SPT_FakeColorR', t=24)
            MayaCmds.setKeyframe(nodeName, value=0.8,
                attribute='SPT_FakeColorG', t=24)
            MayaCmds.setKeyframe(nodeName, value=1.0,
                attribute='SPT_FakeColorB', t=24)

    def verifyProps( self, root, nodeName, isMesh=False ):

        # write to files
        MayaCmds.AlembicTakoExport('range  1 14 %s %s' % (root, self.__files[1]))
        MayaCmds.AlembicTakoExport('range 15 24 %s %s' % (root, self.__files[2]))

        subprocess.call(self.__takoStitcher + self.__files)

        # read file and verify data
        MayaCmds.AlembicTakoImport(self.__files[0], mode='open', timeRange=[1, 24])

        t = 1    # frame 1
        MayaCmds.currentTime(t, update=True)
        self.failUnlessEqual(0, MayaCmds.getAttr(nodeName+'.SPT_int8'),
            '%s.SPT_int8 != 0 at frame %d' %( nodeName, t))
        self.failUnlessEqual(100, MayaCmds.getAttr(nodeName+'.SPT_int16'),
            '%s.SPT_int16 != 100 at frame %d' %( nodeName, t))
        self.failUnlessEqual(1000, MayaCmds.getAttr(nodeName+'.SPT_int32'),
            '%s.SPT_int32 != 1000 at frame %d' %( nodeName, t))
        self.failUnlessAlmostEqual(0.57777777,
            MayaCmds.getAttr(nodeName+'.SPT_float'),  4,
            '%s.SPT_float != 0.57777777 at frame %d' %( nodeName, t))
        self.failUnlessAlmostEqual(5.045643545457,
            MayaCmds.getAttr(nodeName+'.SPT_double'), 7,
            '%s.SPT_double != 5.045643545457 at frame %d' %( nodeName, t))
        if isMesh:
            self.failUnlessAlmostEqual(0.0,
                MayaCmds.getAttr(nodeName+'.SPT_FakeColorR'), 4,
                '%s.SPT_FakeColorR != 0.0 at frame %d' %( nodeName, t))
            self.failUnlessAlmostEqual(0.0,
                MayaCmds.getAttr(nodeName+'.SPT_FakeColorG'), 4,
                    '%s.SPT_FakeColorG != 0.0 at frame %d' %( nodeName, t))
            self.failUnlessAlmostEqual(0.0,
                MayaCmds.getAttr(nodeName+'.SPT_FakeColorB'), 4,
                '%s.SPT_FakeColorB != 0.0 at frame %d' %( nodeName, t))

        t = 12  # frame 12
        MayaCmds.currentTime(t, update=True)
        self.failUnlessEqual(8, MayaCmds.getAttr(nodeName+'.SPT_int8'),
            '%s.SPT_int8 != 8 at frame %d' %( nodeName, t))
        self.failUnlessEqual(16, MayaCmds.getAttr(nodeName+'.SPT_int16'),
            '%s.SPT_int16 != 16 at frame %d' %( nodeName, t))
        self.failUnlessEqual(32, MayaCmds.getAttr(nodeName+'.SPT_int32'),
            '%s.SPT_int32 != 32 at frame %d' %( nodeName, t))
        self.failUnlessAlmostEqual(3.141592654,
            MayaCmds.getAttr(nodeName+'.SPT_float'), 4,
            '%s.SPT_float != 3.141592654 at frame %d' %( nodeName, t))
        self.failUnlessAlmostEqual(3.1415926547,
            MayaCmds.getAttr(nodeName+'.SPT_double'), 7,
            '%s.SPT_double != 3.141592654 at frame %d' %( nodeName, t))
        if isMesh:
            self.failUnlessAlmostEqual(0.25,
                MayaCmds.getAttr(nodeName+'.SPT_FakeColorR'), 4,
                '%s.SPT_FakeColorR != 0.25 at frame %d' %( nodeName, t))
            self.failUnlessAlmostEqual(0.20,
                MayaCmds.getAttr(nodeName+'.SPT_FakeColorG'), 4,
                '%s.SPT_FakeColorG != 0.20 at frame %d' %( nodeName, t))
            self.failUnlessAlmostEqual(0.50,
                MayaCmds.getAttr(nodeName+'.SPT_FakeColorB'), 4,
                '%s.SPT_FakeColorB != 0.50 at frame %d' %( nodeName, t))

        t = 24  # frame 24
        MayaCmds.currentTime(t, update=True)
        self.failUnlessEqual(0,MayaCmds.getAttr(nodeName+'.SPT_int8'),
            '%s.SPT_int8 != 0 at frame %d' % (nodeName, t))
        self.failUnlessEqual(100, MayaCmds.getAttr(nodeName+'.SPT_int16'),
            '%s.SPT_int16 != 100 at frame %d' % (nodeName, t))
        self.failUnlessEqual(1000, MayaCmds.getAttr(nodeName+'.SPT_int32'),
            '%s.SPT_int32 != 1000 at frame %d' % (nodeName, t))
        self.failUnlessAlmostEqual(0.57777777,
            MayaCmds.getAttr(nodeName+'.SPT_float'), 4,
            '%s.SPT_float != 0.57777777 at frame %d' % (nodeName, t))
        self.failUnlessAlmostEqual(5.045643545457,
            MayaCmds.getAttr(nodeName+'.SPT_double'), 7,
            '%s.SPT_double != 5.045643545457 at frame %d' % (nodeName, t))
        if isMesh:
            self.failUnlessAlmostEqual(0.5,
                MayaCmds.getAttr(nodeName+'.SPT_FakeColorR'), 4,
                '%s.SPT_FakeColorR != 0.5 at frame %d' % (nodeName, t))
            self.failUnlessAlmostEqual(0.8,
                MayaCmds.getAttr(nodeName+'.SPT_FakeColorG'), 4,
                '%s.SPT_FakeColorR != 0.8 at frame %d' % (nodeName, t))
            self.failUnlessAlmostEqual(1.0,
                MayaCmds.getAttr(nodeName+'.SPT_FakeColorB'), 4,
                '%s.SPT_FakeColorR != 1.0 at frame %d' % (nodeName, t))

    def testAnimTransformProp(self):
        nodeName = MayaCmds.createNode('transform')
        self.setAndKeyProps(nodeName)
        self.verifyProps(nodeName, nodeName)

    def testAnimCameraProp(self):
        root = MayaCmds.camera()
        nodeName = root[0]
        shapeName = root[1]
        self.setAndKeyProps(shapeName)
        self.verifyProps(nodeName, shapeName)

    def testAnimLightProp(self):
        nodeName = 'spotLight'
        shapeName = 'spotLightShape'
        MayaCmds.spotLight(name=nodeName)
        self.setAndKeyProps(shapeName)
        self.verifyProps(nodeName, shapeName)

    def testAnimMeshProp(self):
        nodeName = 'polyCube'
        shapeName = 'polyCubeShape'
        MayaCmds.polyCube(name=nodeName, ch=False)
        self.setAndKeyProps(shapeName, True)
        self.verifyProps(nodeName, shapeName, True)

    def testAnimNurbsCurvePropReadWrite(self):
        nodeName = 'nCurve'
        shapeName = 'curveShape1'
        MayaCmds.curve(p=[(0, 0, 0), (3, 5, 6), (5, 6, 7), (9, 9, 9)], name=nodeName)
        self.setAndKeyProps(shapeName, True)
        self.verifyProps(nodeName, shapeName, True)

    def testAnimNurbsSurfaceProp(self):
        nodeName = MayaCmds.sphere(ch=False)[0]
        nodeNameList = MayaCmds.pickWalk(d='down')
        shapeName = nodeNameList[0]
        self.setAndKeyProps(shapeName, True)
        self.verifyProps(nodeName, shapeName, True)
