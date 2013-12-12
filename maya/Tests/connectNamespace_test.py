##-*****************************************************************************
##
## Copyright (c) 2013,
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
from maya import mel as Mel
import os
import subprocess
import unittest
import util

class ConnectNamespaceTest(unittest.TestCase):

    def buildScene(self):
        MayaCmds.file(new=True, force=True)

        MayaCmds.namespace(addNamespace='foo')
        MayaCmds.namespace(addNamespace='bar')
        MayaCmds.createNode('transform', name='foo:a')
        MayaCmds.createNode('transform', name='bar:a')
        MayaCmds.createNode('transform', name='a')
        MayaCmds.polyPlane(sx=1, sy=1, w=1, h=1, ch=0, n='foo:b')
        MayaCmds.polyPlane(sx=1, sy=1, w=1, h=1, ch=0, n='bar:b')
        MayaCmds.polyPlane(sx=1, sy=1, w=1, h=1, ch=0, n='b')
        MayaCmds.parent('foo:b', 'foo:a')
        MayaCmds.parent('bar:b', 'bar:a')
        MayaCmds.parent('b', 'a')

        MayaCmds.select('foo:b.vtx[0:8]')
        MayaCmds.setKeyframe(t=[1, 4])
        MayaCmds.scale(0.1, 0.1, 0.1, r=True)
        MayaCmds.setKeyframe(t=2)

        MayaCmds.select('bar:b.vtx[0:8]')
        MayaCmds.setKeyframe(t=[1, 4])
        MayaCmds.scale(2, 2, 2, r=True)
        MayaCmds.setKeyframe(t=2)

        MayaCmds.select('b.vtx[0:8]')
        MayaCmds.setKeyframe(t=[1, 4])
        MayaCmds.scale(5, 5, 5, r=True)
        MayaCmds.setKeyframe(t=2)

    def buildStatic(self):
        MayaCmds.file(new=True, force=True)

        MayaCmds.namespace(addNamespace='foo')
        MayaCmds.namespace(addNamespace='bar')
        MayaCmds.createNode('transform', name='foo:a')
        MayaCmds.createNode('transform', name='bar:a')
        MayaCmds.createNode('transform', name='a')
        MayaCmds.polyPlane(sx=1, sy=1, w=1, h=1, ch=0, n='foo:b')
        MayaCmds.polyPlane(sx=1, sy=1, w=1, h=1, ch=0, n='bar:b')
        MayaCmds.polyPlane(sx=1, sy=1, w=1, h=1, ch=0, n='b')
        MayaCmds.parent('foo:b', 'foo:a')
        MayaCmds.parent('bar:b', 'bar:a')
        MayaCmds.parent('b', 'a')

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []


    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testNamespace(self):
        self.buildScene()
        self.__files.append(util.expandFileName('testNamespace_foo_a.abc'))
        self.__files.append(util.expandFileName('testNamespace_bar_a.abc'))
        self.__files.append(util.expandFileName('testNamespace_a.abc'))

        MayaCmds.AbcExport(j='-root foo:a -fr 1 4 -file ' + self.__files[-3])
        MayaCmds.AbcExport(j='-root bar:a -fr 1 4 -file ' + self.__files[-2])
        MayaCmds.AbcExport(j='-root a -fr 1 4 -file ' + self.__files[-1])

        self.buildStatic()
        MayaCmds.AbcImport(self.__files[-3], connect='foo:a')
        self.failUnless('testNamespace_foo_a_AlembicNode.outPolyMesh[0]' ==
            MayaCmds.connectionInfo("|foo:a|foo:b|foo:bShape.inMesh", sfd=True))

        MayaCmds.AbcImport(self.__files[-2], connect='bar:a')
        self.failUnless('testNamespace_bar_a_AlembicNode.outPolyMesh[0]' ==
            MayaCmds.connectionInfo("|bar:a|bar:b|bar:bShape.inMesh", sfd=True))

        MayaCmds.AbcImport(self.__files[-1], connect='a')
        self.failUnless('testNamespace_a_AlembicNode.outPolyMesh[0]' ==
            MayaCmds.connectionInfo("|a|b|bShape.inMesh", sfd=True))

    def testNamespaceChild(self):
        self.buildScene()
        self.__files.append(util.expandFileName('testNamespace_foo_b.abc'))
        self.__files.append(util.expandFileName('testNamespace_bar_b.abc'))
        self.__files.append(util.expandFileName('testNamespace_b.abc'))

        MayaCmds.AbcExport(j='-root foo:b -fr 1 4 -file ' + self.__files[-3])
        MayaCmds.AbcExport(j='-root bar:b -fr 1 4 -file ' + self.__files[-2])
        MayaCmds.AbcExport(j='-root b -fr 1 4 -file ' + self.__files[-1])

        self.buildStatic()
        MayaCmds.AbcImport(self.__files[-3], connect='foo:a')
        self.failUnless('testNamespace_foo_b_AlembicNode.outPolyMesh[0]' ==
            MayaCmds.connectionInfo("|foo:a|foo:b|foo:bShape.inMesh", sfd=True))

        MayaCmds.AbcImport(self.__files[-3], connect='bar:a')
        self.failUnless('testNamespace_foo_b_AlembicNode1.outPolyMesh[0]' ==
            MayaCmds.connectionInfo("|bar:a|bar:b|bar:bShape.inMesh", sfd=True))

        MayaCmds.AbcImport(self.__files[-3], connect='a')
        self.failUnless('testNamespace_foo_b_AlembicNode2.outPolyMesh[0]' ==
            MayaCmds.connectionInfo("|a|b|bShape.inMesh", sfd=True))

        self.buildStatic()
        MayaCmds.AbcImport(self.__files[-2], connect='foo:a')
        self.failUnless('testNamespace_bar_b_AlembicNode.outPolyMesh[0]' ==
            MayaCmds.connectionInfo("|foo:a|foo:b|foo:bShape.inMesh", sfd=True))

        MayaCmds.AbcImport(self.__files[-2], connect='bar:a')
        self.failUnless('testNamespace_bar_b_AlembicNode1.outPolyMesh[0]' ==
            MayaCmds.connectionInfo("|bar:a|bar:b|bar:bShape.inMesh", sfd=True))

        MayaCmds.AbcImport(self.__files[-2], connect='a')
        self.failUnless('testNamespace_bar_b_AlembicNode2.outPolyMesh[0]' ==
            MayaCmds.connectionInfo("|a|b|bShape.inMesh", sfd=True))
