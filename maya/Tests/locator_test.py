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

def createLocator():
    shape = MayaCmds.createNode("locator")
    name = MayaCmds.pickWalk(shape, d="up")
    MayaCmds.setAttr(shape+'.localPositionX', 0.962)
    MayaCmds.setAttr(shape+'.localPositionY', 0.731)
    MayaCmds.setAttr(shape+'.localPositionZ', 5.114)
    MayaCmds.setAttr(shape+'.localScaleX', 5)
    MayaCmds.setAttr(shape+'.localScaleY', 1.44)
    MayaCmds.setAttr(shape+'.localScaleZ', 1.38)
    return name[0], shape

class locatorTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []
        self.__abcStitcher = [os.environ['AbcStitcher']]

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testStaticLocatorRW(self):

        name = createLocator()

        # write to file
        self.__files.append(util.expandFileName('testStaticLocatorRW.abc'))
        MayaCmds.AbcExport(j='-root %s -file %s' % (name[0], self.__files[-1]))

        # read from file
        MayaCmds.AbcImport(self.__files[-1], mode='import')
        locatorList = MayaCmds.ls(type='locator')
        self.failUnless(util.compareLocator(locatorList[0], locatorList[1]))

    def testAnimLocatorRW(self):

        name = createLocator()

        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe(name[1], attribute='localPositionX')
        MayaCmds.setKeyframe(name[1], attribute='localPositionY')
        MayaCmds.setKeyframe(name[1], attribute='localPositionZ')
        MayaCmds.setKeyframe(name[1], attribute='localScaleX')
        MayaCmds.setKeyframe(name[1], attribute='localScaleY')
        MayaCmds.setKeyframe(name[1], attribute='localScaleZ')
        MayaCmds.currentTime(24, update=True)
        MayaCmds.setKeyframe(name[1], attribute='localPositionX', value=0.0)
        MayaCmds.setKeyframe(name[1], attribute='localPositionY', value=0.0)
        MayaCmds.setKeyframe(name[1], attribute='localPositionZ', value=0.0)
        MayaCmds.setKeyframe(name[1], attribute='localScaleX', value=1.0)
        MayaCmds.setKeyframe(name[1], attribute='localScaleY', value=1.0)
        MayaCmds.setKeyframe(name[1], attribute='localScaleZ', value=1.0)

        self.__files.append(util.expandFileName('testAnimLocatorRW.abc'))
        self.__files.append(util.expandFileName('testAnimLocatorRW01_14.abc'))
        self.__files.append(util.expandFileName('testAnimLocatorRW15-24.abc'))

        # write to files
        MayaCmds.AbcExport(j='-fr 1 14 -root %s -file %s' % (name[0], self.__files[-2]))
        MayaCmds.AbcExport(j='-fr 15 24 -root %s -file %s' % (name[0], self.__files[-1]))

        subprocess.call(self.__abcStitcher + self.__files[-3:])

        # read from file
        MayaCmds.AbcImport(self.__files[-3], mode='import')
        locatorList = MayaCmds.ls(type='locator')

        for t in range(1, 25):
            MayaCmds.currentTime(t, update=True)
            if not util.compareLocator(locatorList[0], locatorList[1]):
                self.fail('%s and %s are not the same at frame %d' %
                    (locatorList[0], locatorList[1], t))
