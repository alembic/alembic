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

class animPropTest(unittest.TestCase):

#-------------------------------------------------------------------------
    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__abcStitcher = [os.environ['AbcStitcher']]
        self.__files = [util.expandFileName('animProp.abc'),
            util.expandFileName('animProp01_14.abc'),
            util.expandFileName('animProp15_24.abc')]

#-------------------------------------------------------------------------
    def tearDown(self):
        for f in self.__files :
            os.remove(f)

#-------------------------------------------------------------------------
    def setAndKeyProps( self, nodeName ):

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

    def verifyProps( self, root, nodeName, wfgAndMoblur ):

        # write to files
        if wfgAndMoblur:
            # reset __files so we wont try to tear down files that dont exist
            self.__files = [self.__files[0]]
            MayaCmds.AbcExport(j='-atp SPT_ -fr 1 24 -wfg -frs -0.25 -frs 0.0 -frs 0.25 -root %s -file %s' % (root, self.__files[0]))
        else:
            MayaCmds.AbcExport(j='-atp SPT_ -fr 1 14 -root %s -file %s' % (root, self.__files[1]))
            MayaCmds.AbcExport(j='-atp SPT_ -fr 15 24 -root %s -file %s' % (root, self.__files[2]))

            subprocess.call(self.__abcStitcher + self.__files)

        # read file and verify data
        MayaCmds.AbcImport(self.__files[0], mode='open')

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

    def testAnimTransformProp(self):
        nodeName = MayaCmds.createNode('transform')
        self.setAndKeyProps(nodeName)
        self.verifyProps(nodeName, nodeName, False)

    def testAnimCameraProp(self):
        root = MayaCmds.camera()
        nodeName = root[0]
        shapeName = root[1]
        self.setAndKeyProps(shapeName)
        self.verifyProps(nodeName, shapeName, False)

    def testAnimMeshProp(self):
        nodeName = 'polyCube'
        shapeName = 'polyCubeShape'
        MayaCmds.polyCube(name=nodeName, ch=False)
        self.setAndKeyProps(shapeName)
        self.verifyProps(nodeName, shapeName, False)

    def testAnimNurbsCurvePropReadWrite(self):
        nodeName = 'nCurve'
        shapeName = 'curveShape1'
        MayaCmds.curve(p=[(0, 0, 0), (3, 5, 6), (5, 6, 7), (9, 9, 9)], name=nodeName)
        self.setAndKeyProps(shapeName)
        self.verifyProps(nodeName, shapeName, False)

    def testAnimNurbsSurfaceProp(self):
        nodeName = MayaCmds.sphere(ch=False)[0]
        nodeNameList = MayaCmds.pickWalk(d='down')
        shapeName = nodeNameList[0]
        self.setAndKeyProps(shapeName)
        self.verifyProps(nodeName, shapeName, False)

    def testWFGAnimTransformProp(self):
        nodeName = MayaCmds.createNode('transform')
        self.setAndKeyProps(nodeName)
        self.verifyProps(nodeName, nodeName, True)

    def testWFGAnimCameraProp(self):
        root = MayaCmds.camera()
        nodeName = root[0]
        shapeName = root[1]
        self.setAndKeyProps(shapeName)
        self.verifyProps(nodeName, shapeName, True)

    def testWFGAnimMeshProp(self):
        nodeName = 'polyCube'
        shapeName = 'polyCubeShape'
        MayaCmds.polyCube(name=nodeName, ch=False)
        self.setAndKeyProps(shapeName)
        self.verifyProps(nodeName, shapeName, True)

    def testWFGAnimNurbsCurvePropReadWrite(self):
        nodeName = 'nCurve'
        shapeName = 'curveShape1'
        MayaCmds.curve(p=[(0, 0, 0), (3, 5, 6), (5, 6, 7), (9, 9, 9)], name=nodeName)
        self.setAndKeyProps(shapeName)
        self.verifyProps(nodeName, shapeName, True)

    def testWFGAnimNurbsSurfaceProp(self):
        nodeName = MayaCmds.sphere(ch=False)[0]
        nodeNameList = MayaCmds.pickWalk(d='down')
        shapeName = nodeNameList[0]
        self.setAndKeyProps(shapeName)
        self.verifyProps(nodeName, shapeName, True)
