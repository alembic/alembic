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

from   maya import cmds as MayaCmds
import maya.OpenMaya as OpenMaya
import os
import subprocess
import unittest
import util


def createJoints():

    name = MayaCmds.joint(position=(0, 0, 0))
    MayaCmds.rotate(33.694356, 4.000428, 61.426019, r=True, ws=True)
    MayaCmds.joint(position=(0, 4, 0), orientation=(0.0, 45.0, 90.0))
    MayaCmds.rotate(62.153171, 0.0, 0.0, r=True, os=True)
    MayaCmds.joint(position=(0, 8, -1), orientation=(90.0, 0.0, 0.0))
    MayaCmds.rotate(70.245162, -33.242019, 41.673097, r=True, os=True)
    MayaCmds.joint(position=(0, 12, 3))
    MayaCmds.rotate(0.0, 0.0, -58.973851, r=True, os=True)

    return name

class JointTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []
        self.__abcStitcher = [os.environ['AbcStitcher']]

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testStaticJointRW(self):

        name = createJoints()

        # write to file
        self.__files.append(util.expandFileName('testStaticJoints.abc'))
        MayaCmds.AbcExport(j='-root %s -file %s' % (name, self.__files[-1]))
        MayaCmds.select(name)
        MayaCmds.group(name='original')

        # read from file
        MayaCmds.AbcImport(self.__files[-1], mode='import')

        # make sure the translate and rotation are the same
        nodes1 = ["|original|joint1", "|original|joint1|joint2", "|original|joint1|joint2|joint3", "|original|joint1|joint2|joint3|joint4"]
        nodes2 = ["|joint1", "|joint1|joint2", "|joint1|joint2|joint3", "|joint1|joint2|joint3|joint4"]
        for i in range(0, 4):
            self.failUnlessAlmostEqual(MayaCmds.getAttr(nodes1[i]+'.tx'), MayaCmds.getAttr(nodes2[i]+'.tx'), 4)
            self.failUnlessAlmostEqual(MayaCmds.getAttr(nodes1[i]+'.ty'), MayaCmds.getAttr(nodes2[i]+'.ty'), 4)
            self.failUnlessAlmostEqual(MayaCmds.getAttr(nodes1[i]+'.tz'), MayaCmds.getAttr(nodes2[i]+'.tz'), 4)

    def testStaticIKRW(self):

        name = createJoints()
        MayaCmds.ikHandle(sj=name, ee='joint4')
        MayaCmds.move(-1.040057, -7.278225, 6.498725, r=True)

        # write to file
        self.__files.append(util.expandFileName('testStaticIK.abc'))
        MayaCmds.AbcExport(j='-root %s -f %s' % (name, self.__files[-1]))
        MayaCmds.select(name)
        MayaCmds.group(name='original')

        # read from file
        MayaCmds.AbcImport(self.__files[-1], mode='import')

        # make sure the translate and rotation are the same
        nodes1 = ["|original|joint1", "|original|joint1|joint2", "|original|joint1|joint2|joint3", "|original|joint1|joint2|joint3|joint4"]
        nodes2 = ["|joint1", "|joint1|joint2", "|joint1|joint2|joint3", "|joint1|joint2|joint3|joint4"]
        for i in range(0, 4):
            self.failUnlessAlmostEqual(MayaCmds.getAttr(nodes1[i]+'.tx'), MayaCmds.getAttr(nodes2[i]+'.tx'), 4)
            self.failUnlessAlmostEqual(MayaCmds.getAttr(nodes1[i]+'.ty'), MayaCmds.getAttr(nodes2[i]+'.ty'), 4)
            self.failUnlessAlmostEqual(MayaCmds.getAttr(nodes1[i]+'.tz'), MayaCmds.getAttr(nodes2[i]+'.tz'), 4)

    def testAnimIKRW(self):

        name = createJoints()
        handleName = MayaCmds.ikHandle(sj=name, ee='joint4')[0]
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe(handleName, breakdown=0, hierarchy='none', controlPoints=False, shape=False)
        MayaCmds.currentTime(16, update=True)
        MayaCmds.move(-1.040057, -7.278225, 6.498725, r=True)
        MayaCmds.setKeyframe(handleName, breakdown=0, hierarchy='none', controlPoints=False, shape=False)

        self.__files.append(util.expandFileName('testAnimIKRW.abc'))
        self.__files.append(util.expandFileName('testAnimIKRW01_08.abc'))
        self.__files.append(util.expandFileName('testAnimIKRW09-16.abc'))

        # write to files
        MayaCmds.AbcExport(j='-fr 1 8 -root %s -f %s' % (name, self.__files[-2]))
        MayaCmds.AbcExport(j='-fr 9 16 -root %s -f %s' % (name, self.__files[-1]))
        MayaCmds.select(name)
        MayaCmds.group(name='original')

        subprocess.call(self.__abcStitcher + self.__files[-3:])

        # read from file
        MayaCmds.AbcImport(self.__files[-3], mode='import')

        # make sure the translate and rotation are the same
        nodes1 = ["|original|joint1", "|original|joint1|joint2", "|original|joint1|joint2|joint3", "|original|joint1|joint2|joint3|joint4"]
        nodes2 = ["|joint1", "|joint1|joint2", "|joint1|joint2|joint3", "|joint1|joint2|joint3|joint4"]

        for t in range(1, 25):
            MayaCmds.currentTime(t, update=True)
            for i in range(0, 4):
                self.failUnlessAlmostEqual(MayaCmds.getAttr(nodes1[i]+'.tx'), MayaCmds.getAttr(nodes2[i]+'.tx'), 4)
                self.failUnlessAlmostEqual(MayaCmds.getAttr(nodes1[i]+'.ty'), MayaCmds.getAttr(nodes2[i]+'.ty'), 4)
                self.failUnlessAlmostEqual(MayaCmds.getAttr(nodes1[i]+'.tz'), MayaCmds.getAttr(nodes2[i]+'.tz'), 4)
