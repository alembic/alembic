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

class StaticPointPrimitiveTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files :
            os.remove(f)

    def testPointPrimitiveReadWrite(self):

        # Creates a particle object with four particles
        name = MayaCmds.particle( p=[(0, 0, 0), (3, 5, 6), (5, 6, 7),
            (9, 9, 9)])

        posVec = [0, 0, 0, 3, 5, 6, 5, 6, 7, 9, 9, 9]

        self.__files.append(util.expandFileName('testPointPrimitiveReadWrite.abc'))
        MayaCmds.AbcExport(j='-root %s -f %s' % (name[0], self.__files[-1]))

        # reading test
        MayaCmds.AbcImport(self.__files[-1], mode='open')
        pos = MayaCmds.getAttr(name[1] + '.position')
        ids = MayaCmds.getAttr(name[1] + '.particleId')

        self.failUnlessEqual(len(ids), 4)
        for i in range(0, 4):
            index = 3*i
            self.failUnlessAlmostEqual(pos[i][0], posVec[index], 4)
            self.failUnlessAlmostEqual(pos[i][1], posVec[index+1], 4)
            self.failUnlessAlmostEqual(pos[i][2], posVec[index+2], 4)
