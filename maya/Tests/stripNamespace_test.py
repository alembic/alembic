##-*****************************************************************************
##
## Copyright (c) 2009-2013,
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

class StripNamespaceTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testStrip(self):
        MayaCmds.namespace(addNamespace='delicious')
        MayaCmds.namespace(addNamespace='crispy', parent='delicious')
        MayaCmds.namespace(addNamespace='duck', parent='delicious:crispy')
        MayaCmds.namespace(addNamespace='skin', parent='delicious:crispy:duck')
        MayaCmds.createNode('transform', name='delicious:crispy:duck:skin:yum')

        self.__files.append(util.expandFileName('noStrip.abc'))
        MayaCmds.AbcExport(j='-sn 0 -root delicious:crispy:duck:skin:yum -file ' + self.__files[-1])

        self.__files.append(util.expandFileName('stripAll.abc'))
        MayaCmds.AbcExport(j='-sn -root delicious:crispy:duck:skin:yum -file ' + self.__files[-1])

        self.__files.append(util.expandFileName('stripAll2.abc'))
        MayaCmds.AbcExport(j='-sn 4 -root delicious:crispy:duck:skin:yum -file ' + self.__files[-1])

        self.__files.append(util.expandFileName('stripAll3.abc'))
        MayaCmds.AbcExport(j='-sn 5 -root delicious:crispy:duck:skin:yum -file ' + self.__files[-1])

        self.__files.append(util.expandFileName('stripOne.abc'))
        MayaCmds.AbcExport(j='-sn 1 -root delicious:crispy:duck:skin:yum -file ' + self.__files[-1])

        self.__files.append(util.expandFileName('stripTwo.abc'))
        MayaCmds.AbcExport(j='-sn 2 -root delicious:crispy:duck:skin:yum -file ' + self.__files[-1])

        self.__files.append(util.expandFileName('stripThree.abc'))
        MayaCmds.AbcExport(j='-sn 3 -root delicious:crispy:duck:skin:yum -file ' + self.__files[-1])

        # no strip
        MayaCmds.file(force=True, new=True)
        MayaCmds.namespace(addNamespace='delicious')
        MayaCmds.namespace(addNamespace='crispy', parent='delicious')
        MayaCmds.namespace(addNamespace='duck', parent='delicious:crispy')
        MayaCmds.namespace(addNamespace='skin', parent='delicious:crispy:duck')
        MayaCmds.AbcImport(self.__files[0], mode='import')
        self.failIf(len(MayaCmds.ls('delicious:crispy:duck:skin:yum')) != 1)

        # all stripped
        MayaCmds.AbcImport(self.__files[1], mode='import')
        MayaCmds.AbcImport(self.__files[2], mode='import')
        MayaCmds.AbcImport(self.__files[3], mode='import')
        self.failIf(len(MayaCmds.ls('yum*')) != 3)

        # strip one
        MayaCmds.file(force=True, new=True)
        MayaCmds.namespace(addNamespace='crispy')
        MayaCmds.namespace(addNamespace='duck', parent='crispy')
        MayaCmds.namespace(addNamespace='skin', parent='crispy:duck')
        MayaCmds.AbcImport(self.__files[4], mode='import')
        self.failIf(len(MayaCmds.ls('crispy:duck:skin:yum')) != 1)

        # strip two
        MayaCmds.file(force=True, new=True)
        MayaCmds.namespace(addNamespace='duck')
        MayaCmds.namespace(addNamespace='skin', parent='duck')
        MayaCmds.AbcImport(self.__files[5], mode='import')
        self.failIf(len(MayaCmds.ls('duck:skin:yum')) != 1)

        # strip three
        MayaCmds.file(force=True, new=True)
        MayaCmds.namespace(addNamespace='skin')
        MayaCmds.AbcImport(self.__files[6], mode='import')
        self.failIf(len(MayaCmds.ls('skin:yum')) != 1)
