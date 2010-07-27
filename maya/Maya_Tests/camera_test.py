
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
    MayaCmds.setAttr(name[1]+'.focusDistance', 5)
    MayaCmds.setAttr(name[1]+'.shutterAngle', 144)
    MayaCmds.setAttr(name[1]+'.centerOfInterest', 1384.825)
    return name

class cameraTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []
        self.__takoStitcher = [os.environ['AlembicStitcher']]

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testStaticCameraReadWrite(self):
        name = createCamera()

        # write to file
        self.__files.append('/tmp/testStaticCameraReadWrite.hdf')
        MayaCmds.AlembicTakoExport(name[0] + ' ' + self.__files[-1])

        # read from file
        MayaCmds.AlembicTakoImport(self.__files[-1], mode='import')
        camList = MayaCmds.ls(type='camera')
        self.failUnless(util.compareCamera(camList[0], camList[1]))

    def testAnimCameraReadWrite(self):

        name = createCamera()
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe(name[1], attribute='horizontalFilmAperture')
        MayaCmds.setKeyframe(name[1], attribute='focalLength')
        MayaCmds.setKeyframe(name[1], attribute='focusDistance')
        MayaCmds.setKeyframe(name[1], attribute='shutterAngle')
        MayaCmds.setKeyframe(name[1], attribute='centerOfInterest')
        MayaCmds.currentTime(24, update=True)
        MayaCmds.setKeyframe(name[1], attribute='horizontalFilmAperture',
            value=0.95)
        MayaCmds.setKeyframe(name[1], attribute='focalLength', value=40)
        MayaCmds.setKeyframe(name[1], attribute='focusDistance', value=5.4)
        MayaCmds.setKeyframe(name[1], attribute='shutterAngle', value=174.94)
        MayaCmds.setKeyframe(name[1], attribute='centerOfInterest',
            value=67.418)

        self.__files.append('/tmp/testAnimCameraReadWrite.hdf')
        self.__files.append('/tmp/testAnimCameraReadWrite01_14.hdf')
        self.__files.append('/tmp/testAnimCameraReadWrite15-24.hdf')

        # write to files
        MayaCmds.AlembicTakoExport('range 1 14 %s %s' % (name[0], self.__files[-2]))
        MayaCmds.AlembicTakoExport('range 15 24 %s %s' % (name[0], self.__files[-1]))

        subprocess.call(self.__takoStitcher + self.__files[-3:])

        # read from file
        MayaCmds.AlembicTakoImport(self.__files[-3], mode='import', timeRange=[1, 24])
        camList = MayaCmds.ls(type='camera')

        for t in range(1, 25):
            MayaCmds.currentTime(t, update=True)
            if not util.compareCamera(camList[0], camList[1]):
                self.fail('%s and %s are not the same at frame %d' %
                    (camList[0], camList[1], t))
