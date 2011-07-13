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

class LoadWorldSpaceTranslateJobTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files :
            os.remove(f)

    def testStaticLoadWorldSpaceTranslateJobReadWrite(self):
        nodeName = MayaCmds.polyCube(n='cube')
        root = nodeName[0]
        MayaCmds.setAttr(root+'.rotateX', 45)
        MayaCmds.setAttr(root+'.scaleX', 1.5)
        MayaCmds.setAttr(root+'.translateY', -1.95443)

        nodeName = MayaCmds.group()
        MayaCmds.setAttr(nodeName+'.rotateY', 15)
        MayaCmds.setAttr(nodeName+'.translateY', -3.95443)

        nodeName = MayaCmds.group()
        MayaCmds.setAttr(nodeName+'.rotateZ',-90)
        MayaCmds.setAttr(nodeName+'.shearXZ',2.555)

        self.__files.append(util.expandFileName('testStaticLoadWorldSpaceTranslateJob.abc'))
        MayaCmds.AbcExport(j='-ws -root %s -file %s' % (root, self.__files[-1]))


        # reading test
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        self.failUnlessAlmostEqual(0, MayaCmds.getAttr(root+'.translateX'), 3)
        self.failUnlessAlmostEqual(-5.909,
            MayaCmds.getAttr(root+'.translateY'), 3)
        self.failUnlessAlmostEqual(0, MayaCmds.getAttr(root+'.translateZ'), 3)

        self.failUnlessAlmostEqual(68.211,
            MayaCmds.getAttr(root+'.rotateX'), 3)
        self.failUnlessAlmostEqual(40.351,
            MayaCmds.getAttr(root+'.rotateY'), 3)
        self.failUnlessAlmostEqual(-90, MayaCmds.getAttr(root+'.rotateZ'), 3)

        self.failUnlessAlmostEqual(0.600, MayaCmds.getAttr(root+'.scaleX'), 3)
        self.failUnlessAlmostEqual(1.905, MayaCmds.getAttr(root+'.scaleY'), 3)
        self.failUnlessAlmostEqual(1.313, MayaCmds.getAttr(root+'.scaleZ'), 3)

        self.failUnlessAlmostEqual(0.539, MayaCmds.getAttr(root+'.shearXY'), 3)
        self.failUnlessAlmostEqual(0.782, MayaCmds.getAttr(root+'.shearXZ'), 3)
        self.failUnlessAlmostEqual(1.051, MayaCmds.getAttr(root+'.shearYZ'), 3)

    def testAnimLoadWorldSpaceTranslateJobReadWrite(self):
        nodeName = MayaCmds.polyCube(n='cube')
        root = nodeName[0]
        MayaCmds.setAttr(root+'.rotateX', 45)
        MayaCmds.setAttr(root+'.scaleX', 1.5)
        MayaCmds.setAttr(root+'.translateY', -1.95443)
        MayaCmds.setKeyframe(root, value=0, attribute='translateX', t=[1, 24])
        MayaCmds.setKeyframe(root, value=1, attribute='translateX', t=12)

        nodeName = MayaCmds.group()
        MayaCmds.setAttr(nodeName+'.rotateY', 15)
        MayaCmds.setAttr(nodeName+'.translateY', -3.95443)

        nodeName = MayaCmds.group()
        MayaCmds.setAttr(nodeName+'.rotateZ',-90)
        MayaCmds.setAttr(nodeName+'.shearXZ',2.555)

        self.__files.append(util.expandFileName('testAnimLoadWorldSpaceTranslateJob.abc'))
        MayaCmds.AbcExport(j='-fr 1 24 -ws -root %s -file %s' %
            (root, self.__files[-1]))

        # reading test
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        # frame 1
        MayaCmds.currentTime(1, update=True)
        self.failUnlessAlmostEqual(0, MayaCmds.getAttr(root+'.translateX'), 3)
        self.failUnlessAlmostEqual(-5.909,
            MayaCmds.getAttr(root+'.translateY'), 3)
        self.failUnlessAlmostEqual(0, MayaCmds.getAttr(root+'.translateZ'), 3)

        self.failUnlessAlmostEqual(68.211, MayaCmds.getAttr(root+'.rotateX'), 3)
        self.failUnlessAlmostEqual(40.351, MayaCmds.getAttr(root+'.rotateY'), 3)
        self.failUnlessAlmostEqual(-90, MayaCmds.getAttr(root+'.rotateZ'), 3)

        self.failUnlessAlmostEqual(0.600, MayaCmds.getAttr(root+'.scaleX'), 3)
        self.failUnlessAlmostEqual(1.905, MayaCmds.getAttr(root+'.scaleY'), 3)
        self.failUnlessAlmostEqual(1.313, MayaCmds.getAttr(root+'.scaleZ'), 3)

        self.failUnlessAlmostEqual(0.539, MayaCmds.getAttr(root+'.shearXY'), 3)
        self.failUnlessAlmostEqual(0.782, MayaCmds.getAttr(root+'.shearXZ'), 3)
        self.failUnlessAlmostEqual(1.051, MayaCmds.getAttr(root+'.shearYZ'), 3)

        # frame 12
        MayaCmds.currentTime(12, update=True)
        self.failUnlessAlmostEqual(0, MayaCmds.getAttr(root+'.translateX'), 3)
        self.failUnlessAlmostEqual(-6.2135,
            MayaCmds.getAttr(root+'.translateY'), 3)
        self.failUnlessAlmostEqual(-0.258819,
            MayaCmds.getAttr(root+'.translateZ'), 3)

        self.failUnlessAlmostEqual(68.211, MayaCmds.getAttr(root+'.rotateX'),
            3)
        self.failUnlessAlmostEqual(40.351, MayaCmds.getAttr(root+'.rotateY'),
            3)
        self.failUnlessAlmostEqual(-90, MayaCmds.getAttr(root+'.rotateZ'), 3)

        self.failUnlessAlmostEqual(0.600, MayaCmds.getAttr(root+'.scaleX'), 3)
        self.failUnlessAlmostEqual(1.905, MayaCmds.getAttr(root+'.scaleY'), 3)
        self.failUnlessAlmostEqual(1.313, MayaCmds.getAttr(root+'.scaleZ'), 3)

        self.failUnlessAlmostEqual(0.539, MayaCmds.getAttr(root+'.shearXY'), 3)
        self.failUnlessAlmostEqual(0.782, MayaCmds.getAttr(root+'.shearXZ'), 3)
        self.failUnlessAlmostEqual(1.051, MayaCmds.getAttr(root+'.shearYZ'), 3)

        # frame 24
        MayaCmds.currentTime(24, update=True)
        self.failUnlessAlmostEqual(0, MayaCmds.getAttr(root+'.translateX'), 3)
        self.failUnlessAlmostEqual(-5.909,
            MayaCmds.getAttr(root+'.translateY'), 3)
        self.failUnlessAlmostEqual(0, MayaCmds.getAttr(root+'.translateZ'), 3)

        self.failUnlessAlmostEqual(68.211, MayaCmds.getAttr(root+'.rotateX'),
            3)
        self.failUnlessAlmostEqual(40.351, MayaCmds.getAttr(root+'.rotateY'),
            3)
        self.failUnlessAlmostEqual(-90, MayaCmds.getAttr(root+'.rotateZ'), 3)

        self.failUnlessAlmostEqual(0.600, MayaCmds.getAttr(root+'.scaleX'), 3)
        self.failUnlessAlmostEqual(1.905, MayaCmds.getAttr(root+'.scaleY'), 3)
        self.failUnlessAlmostEqual(1.313, MayaCmds.getAttr(root+'.scaleZ'), 3)

        self.failUnlessAlmostEqual(0.539, MayaCmds.getAttr(root+'.shearXY'), 3)
        self.failUnlessAlmostEqual(0.782, MayaCmds.getAttr(root+'.shearXZ'), 3)
        self.failUnlessAlmostEqual(1.051, MayaCmds.getAttr(root+'.shearYZ'), 3)
