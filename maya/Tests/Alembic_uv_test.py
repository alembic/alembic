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
import maya.OpenMaya as OpenMaya
import os
import unittest
import util

def getObjFromName( nodeName ):
    selectionList = OpenMaya.MSelectionList()
    selectionList.add( nodeName )
    obj = OpenMaya.MObject()
    selectionList.getDependNode(0, obj)
    return obj

class MeshUVsTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    # we only support writing out the most basic uv sets (and static only)
    def testPolyUVs(self):
        MayaCmds.polyCube(name = 'cube')
        cubeObj = getObjFromName('cubeShape')
        fnMesh = OpenMaya.MFnMesh(cubeObj)

        # get the name of the current UV set
        uvSetName = fnMesh.currentUVSetName()

        uArray = OpenMaya.MFloatArray()
        vArray = OpenMaya.MFloatArray()
        fnMesh.getUVs(uArray, vArray, uvSetName)

        newUArray = [0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 2, -1, -1]
        newVArray = [0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 0, 1 , 0,  1]
        for i in range(0, 14):
            uArray[i] = newUArray[i]
            vArray[i] = newVArray[i]
        fnMesh.setUVs(uArray, vArray, uvSetName)

        self.__files.append(util.expandFileName('polyUvsTest.abc'))
        MayaCmds.AbcExport(j='-uv -root cube -file ' + self.__files[-1])

        # reading test
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        MayaCmds.select('cube.map[0:13]', replace=True)
        uvs = MayaCmds.polyEditUV(query=True)
        for i in range(0, 14):
            self.failUnlessAlmostEqual(newUArray[i], uvs[2*i], 4,
                'map[%d].u is not the same' % i)
            self.failUnlessAlmostEqual(newVArray[i], uvs[2*i+1], 4,
                'map[%d].v is not the same' % i)
