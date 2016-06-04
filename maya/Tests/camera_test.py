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

def createCamera():
    name = MayaCmds.camera()
    MayaCmds.setAttr(name[1]+'.horizontalFilmAperture', 0.962)
    MayaCmds.setAttr(name[1]+'.verticalFilmAperture', 0.731)
    MayaCmds.setAttr(name[1]+'.focalLength', 50)
    MayaCmds.setAttr(name[1]+'.focusDistance', 3)
    MayaCmds.setAttr(name[1]+'.shutterAngle', 100)
    return name

class cameraTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []
        self.__abcStitcher = [os.environ['AbcStitcher']]

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testStaticCameraReadWrite(self):
        name = createCamera()

        # write to file
        self.__files.append(util.expandFileName('testStaticCameraReadWrite.abc'))
        MayaCmds.AbcExport(j='-root %s -file %s' % (name[0], self.__files[-1]))

        # read from file
        MayaCmds.AbcImport(self.__files[-1], mode='import')
        camList = MayaCmds.ls(type='camera')
        self.failUnless(util.compareCamera(camList[0], camList[1]))

    def testFakeAnimCameraReadWrite(self):
        name = createCamera()

        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe(name[1], attribute='horizontalFilmAperture')
        MayaCmds.setKeyframe(name[1], attribute='focalLength')
        MayaCmds.setKeyframe(name[1], attribute='focusDistance')
        MayaCmds.setKeyframe(name[1], attribute='shutterAngle')

        # write to file
        self.__files.append(util.expandFileName('testFakeAnimCameraReadWrite.abc'))
        MayaCmds.AbcExport(j='-fr 1 10 -root %s -file %s' % (name[0], self.__files[-1]))

        # read from file
        MayaCmds.AbcImport(self.__files[-1], mode='import')
        camList = MayaCmds.ls(type='camera')
        self.failUnless(util.compareCamera(camList[0], camList[1]))

    def testAnimCameraReadWrite(self):

        name = createCamera()
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe(name[1], attribute='horizontalFilmAperture')
        MayaCmds.setKeyframe(name[1], attribute='focalLength')
        MayaCmds.setKeyframe(name[1], attribute='focusDistance')
        MayaCmds.setKeyframe(name[1], attribute='shutterAngle')
        MayaCmds.currentTime(24, update=True)
        MayaCmds.setKeyframe(name[1], attribute='horizontalFilmAperture',
            value=0.95)
        MayaCmds.setKeyframe(name[1], attribute='focalLength', value=40)
        MayaCmds.setKeyframe(name[1], attribute='focusDistance', value=5.4)
        MayaCmds.setKeyframe(name[1], attribute='shutterAngle', value=174.94)

        self.__files.append(util.expandFileName('testAnimCameraReadWrite.abc'))
        self.__files.append(util.expandFileName('testAnimCameraReadWrite01_14.abc'))
        self.__files.append(util.expandFileName('testAnimCameraReadWrite15-24.abc'))

        # write to files
        MayaCmds.AbcExport(j='-fr 1 14 -root %s -file %s' % (name[0], self.__files[-2]))
        MayaCmds.AbcExport(j='-fr 15 24 -root %s -file %s' % (name[0], self.__files[-1]))

        subprocess.call(self.__abcStitcher + self.__files[-3:])

        # read from file
        MayaCmds.AbcImport(self.__files[-3], mode='import')
        camList = MayaCmds.ls(type='camera')

        for t in range(1, 25):
            MayaCmds.currentTime(t, update=True)
            if not util.compareCamera(camList[0], camList[1]):
                self.fail('%s and %s are not the same at frame %d' %
                    (camList[0], camList[1], t))

    def testAnimWholeFrameGeoCameraReadWrite(self):

        name = createCamera()
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe(name[1], attribute='horizontalFilmAperture')
        MayaCmds.setKeyframe(name[1], attribute='focalLength')
        MayaCmds.setKeyframe(name[1], attribute='focusDistance')
        MayaCmds.setKeyframe(name[1], attribute='shutterAngle')
        MayaCmds.currentTime(24, update=True)
        MayaCmds.setKeyframe(name[1], attribute='horizontalFilmAperture',
            value=0.95)
        MayaCmds.setKeyframe(name[1], attribute='focalLength', value=40)
        MayaCmds.setKeyframe(name[1], attribute='focusDistance', value=5.4)
        MayaCmds.setKeyframe(name[1], attribute='shutterAngle', value=174.94)

        self.__files.append(util.expandFileName('testAnimWFGCameraReadWrite.abc'))

        # write to files
        MayaCmds.AbcExport(j='-fr 1 24 -frs -0.25 -frs 0.0 -frs 0.25 -wfg -root %s -file %s' % (name[0], self.__files[-1]))

        # read from file
        MayaCmds.AbcImport(self.__files[-1], mode='import')
        camList = MayaCmds.ls(type='camera')

        for t in range(1, 25):
            MayaCmds.currentTime(t, update=True)
            if not util.compareCamera(camList[0], camList[1]):
                self.fail('%s and %s are not the same at frame %d' %
                    (camList[0], camList[1], t))
