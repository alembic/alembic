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

class VisibilityTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files :
            os.remove(f)

    def testStaticVisibility(self):
        poly1  = MayaCmds.polyPlane(sx=2, sy=2, w=1, h=1, ch=0)[0]
        group1 = MayaCmds.group()
        group2 = MayaCmds.createNode("transform")
        MayaCmds.select(group1, group2)
        group3 = MayaCmds.group()

        group4 = (MayaCmds.duplicate(group1, rr=1))[0]
        group5 = MayaCmds.group()

        MayaCmds.select(group3, group5)
        root = MayaCmds.group(name='root')

        MayaCmds.setAttr(group1 + '.visibility', 0)
        MayaCmds.setAttr(group2 + '.visibility', 0)
        MayaCmds.setAttr(group5 + '.visibility', 0)

        self.__files.append(util.expandFileName('staticVisibilityTest.abc'))
        MayaCmds.AbcExport(j='-wv -root %s -file %s' % (root, self.__files[-1]))

        MayaCmds.AbcImport(self.__files[-1], m='open')
        self.failIf(MayaCmds.getAttr(group1+'.visibility'))
        self.failIf(MayaCmds.getAttr(group2+'.visibility'))
        self.failIf(MayaCmds.getAttr(group5+'.visibility'))

        self.failUnless(MayaCmds.getAttr(group1+'|'+poly1+'.visibility'))
        self.failUnless(MayaCmds.getAttr(group4+'|'+poly1+'.visibility'))
        self.failUnless(MayaCmds.getAttr(group3+'.visibility'))
        self.failUnless(MayaCmds.getAttr(group4+'.visibility'))
        self.failUnless(MayaCmds.getAttr(root+'.visibility'))

    def testAnimVisibility(self):
        poly1  = MayaCmds.polyPlane( sx=2, sy=2, w=1, h=1, ch=0)[0]
        group1 = MayaCmds.group()
        group2 = MayaCmds.createNode("transform")
        MayaCmds.select(group1, group2)
        group3 = MayaCmds.group()

        group4 = (MayaCmds.duplicate(group1, rr=1))[0]
        group5 = MayaCmds.group()

        MayaCmds.select(group3, group5)
        root = MayaCmds.group(name='root')

        MayaCmds.setKeyframe(group1 + '.visibility', v=0, t=[1, 4])
        MayaCmds.setKeyframe(group2 + '.visibility', v=0, t=[1, 4])
        MayaCmds.setKeyframe(group5 + '.visibility', v=0, t=[1, 4])

        MayaCmds.setKeyframe(group1 + '.visibility', v=1, t=2)
        MayaCmds.setKeyframe(group2 + '.visibility', v=1, t=2)
        MayaCmds.setKeyframe(group5 + '.visibility', v=1, t=2)

        self.__files.append(util.expandFileName('animVisibilityTest.abc'))
        MayaCmds.AbcExport(j='-wv -fr 1 4 -root %s -file %s' %
            (root, self.__files[-1]))

        MayaCmds.AbcImport(self.__files[-1], mode='open')

        MayaCmds.currentTime(1, update = True)
        self.failIf(MayaCmds.getAttr(group1 + '.visibility'))
        self.failIf(MayaCmds.getAttr(group2 + '.visibility'))
        self.failIf(MayaCmds.getAttr(group5 + '.visibility'))

        self.failUnless(MayaCmds.getAttr(group1 + '|' + poly1 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(group4 + '|' + poly1 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(group3 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(group4 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(root+'.visibility'))

        MayaCmds.currentTime(2, update = True)
        self.failUnless(MayaCmds.getAttr(group1 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(group2 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(group5 + '.visibility'))

        self.failUnless(MayaCmds.getAttr(group1 + '|' + poly1 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(group4 + '|' + poly1 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(group3 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(group4 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(root + '.visibility'))

        MayaCmds.currentTime(4, update = True )
        self.failIf(MayaCmds.getAttr(group1 + '.visibility'))
        self.failIf(MayaCmds.getAttr(group2 + '.visibility'))
        self.failIf(MayaCmds.getAttr(group5 + '.visibility'))

        self.failUnless(MayaCmds.getAttr(group1 + '|' + poly1 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(group4 + '|' + poly1 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(group3 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(group4 + '.visibility'))
        self.failUnless(MayaCmds.getAttr(root+'.visibility'))
