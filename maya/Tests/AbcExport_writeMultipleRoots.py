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

def makeRobot():
    MayaCmds.polyCube(name="head")
    MayaCmds.move(0, 4, 0, r=1)

    MayaCmds.polyCube(name="chest")
    MayaCmds.scale(2, 2.5, 1)
    MayaCmds.move(0, 2, 0, r=1)
    MayaCmds.polyCube(name="leftArm")
    MayaCmds.move(0, 3, 0, r=1)
    MayaCmds.scale(2, 0.5, 1, r=1)
    MayaCmds.duplicate(name="rightArm")
    MayaCmds.select("leftArm")
    MayaCmds.move(1.25, 0, 0, r=1)
    MayaCmds.rotate(0, 0, 32, r=1, os=1)
    MayaCmds.select("rightArm")
    MayaCmds.move(-1.25, 0, 0, r=1)
    MayaCmds.rotate(0, 0, -32, r=1, os=1)
    MayaCmds.select("rightArm", "leftArm", "chest", r=1)
    MayaCmds.group(name="body")

    MayaCmds.polyCube(name="bottom")
    MayaCmds.scale(2, 0.5, 1)
    MayaCmds.move(0, 0.5, 0, r=1)
    MayaCmds.polyCube(name="leftLeg")
    MayaCmds.scale(0.65, 2.8, 1, r=1)
    MayaCmds.move(-0.5, -1, 0, r=1)
    MayaCmds.duplicate(name="rightLeg")
    MayaCmds.move(1, 0, 0, r=1)
    MayaCmds.select("rightLeg", "leftLeg", "bottom", r=1)
    MayaCmds.group(name="lower")

    MayaCmds.select("head", "body", "lower", r=1)
    MayaCmds.group(name="robot")

class selectionTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testWriteMultipleRoots(self):

        makeRobot()
        MayaCmds.duplicate("robot", name="dupRobot")
        self.__files.append(util.expandFileName('writeMultipleRootsTest.abc'))
        MayaCmds.AbcExport(j='-root dupRobot -root head -root lower -root chest -file ' + self.__files[-1])

        MayaCmds.AbcImport(self.__files[-1], m='open')
        self.failUnless(MayaCmds.objExists("dupRobot"))
        self.failUnless(MayaCmds.objExists("head"))
        self.failUnless(MayaCmds.objExists("lower"))
        self.failUnless(MayaCmds.objExists("chest"))

        self.failIf(MayaCmds.objExists("robot"))
        self.failIf(MayaCmds.objExists("robot|body"))
