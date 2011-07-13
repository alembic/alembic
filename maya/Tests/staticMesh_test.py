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
from maya.mel import eval as MelEval

import os
import maya.OpenMaya as OpenMaya
import unittest
import util

class StaticMeshTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files :
            os.remove(f)

    def testStaticMeshReadWrite(self):

        numFaces = 6
        numVertices = 8
        numFaceConnects = 24

        vtx_1 = OpenMaya.MFloatPoint(-0.5, -0.5, -0.5)
        vtx_2 = OpenMaya.MFloatPoint( 0.5, -0.5, -0.5)
        vtx_3 = OpenMaya.MFloatPoint( 0.5, -0.5,  0.5)
        vtx_4 = OpenMaya.MFloatPoint(-0.5, -0.5,  0.5)
        vtx_5 = OpenMaya.MFloatPoint(-0.5,  0.5, -0.5)
        vtx_6 = OpenMaya.MFloatPoint(-0.5,  0.5,  0.5)
        vtx_7 = OpenMaya.MFloatPoint( 0.5,  0.5,  0.5)
        vtx_8 = OpenMaya.MFloatPoint( 0.5,  0.5, -0.5)

        points = OpenMaya.MFloatPointArray()
        points.setLength(8)
        points.set(vtx_1, 0)
        points.set(vtx_2, 1)
        points.set(vtx_3, 2)
        points.set(vtx_4, 3)
        points.set(vtx_5, 4)
        points.set(vtx_6, 5)
        points.set(vtx_7, 6)
        points.set(vtx_8, 7)

        faceConnects = OpenMaya.MIntArray()
        faceConnects.setLength(numFaceConnects)
        faceConnects.set(0, 0)
        faceConnects.set(1, 1)
        faceConnects.set(2, 2)
        faceConnects.set(3, 3)
        faceConnects.set(4, 4)
        faceConnects.set(5, 5)
        faceConnects.set(6, 6)
        faceConnects.set(7, 7)
        faceConnects.set(3, 8)
        faceConnects.set(2, 9)
        faceConnects.set(6, 10)
        faceConnects.set(5, 11)
        faceConnects.set(0, 12)
        faceConnects.set(3, 13)
        faceConnects.set(5, 14)
        faceConnects.set(4, 15)
        faceConnects.set(0, 16)
        faceConnects.set(4, 17)
        faceConnects.set(7, 18)
        faceConnects.set(1, 19)
        faceConnects.set(1, 20)
        faceConnects.set(7, 21)
        faceConnects.set(6, 22)
        faceConnects.set(2, 23)

        faceCounts = OpenMaya.MIntArray()
        faceCounts.setLength(6)
        faceCounts.set(4, 0)
        faceCounts.set(4, 1)
        faceCounts.set(4, 2)
        faceCounts.set(4, 3)
        faceCounts.set(4, 4)
        faceCounts.set(4, 5)

        transFn = OpenMaya.MFnTransform()
        parent = transFn.create()
        transFn.setName('mesh')
        meshFn = OpenMaya.MFnMesh()
        meshFn.create(numVertices, numFaces, points, faceCounts, faceConnects, parent)
        meshFn.setName("meshShape");

        self.__files.append(util.expandFileName('testStaticMeshReadWrite.abc'))
        MayaCmds.AbcExport(j='-root mesh -file ' + self.__files[-1])

        MayaCmds.AbcImport(self.__files[-1], mode='open')

        sList = OpenMaya.MSelectionList()
        sList.add('meshShape')
        meshObj = OpenMaya.MObject()
        sList.getDependNode(0, meshObj)
        meshFn = OpenMaya.MFnMesh(meshObj)

        # check the point list
        meshPoints = OpenMaya.MFloatPointArray()
        meshFn.getPoints(meshPoints);
        self.failUnlessEqual(vtx_1, meshPoints[0])
        self.failUnlessEqual(vtx_2, meshPoints[1])
        self.failUnlessEqual(vtx_3, meshPoints[2])
        self.failUnlessEqual(vtx_4, meshPoints[3])
        self.failUnlessEqual(vtx_5, meshPoints[4])
        self.failUnlessEqual(vtx_6, meshPoints[5])
        self.failUnlessEqual(vtx_7, meshPoints[6])
        self.failUnlessEqual(vtx_8, meshPoints[7])

        # check the polygonvertices list
        vertexList = OpenMaya.MIntArray()
        faceConnects = OpenMaya.MIntArray()
        faceConnects.setLength(4)

        faceConnects.set(0, 0)
        faceConnects.set(1, 1)
        faceConnects.set(2, 2)
        faceConnects.set(3, 3)
        meshFn.getPolygonVertices(0, vertexList)
        self.failUnlessEqual(faceConnects, vertexList)

        faceConnects.set(4, 0)
        faceConnects.set(5, 1)
        faceConnects.set(6, 2)
        faceConnects.set(7, 3)
        meshFn.getPolygonVertices(1, vertexList)
        self.failUnlessEqual(faceConnects, vertexList)

        faceConnects.set(3, 0)
        faceConnects.set(2, 1)
        faceConnects.set(6, 2)
        faceConnects.set(5, 3)
        meshFn.getPolygonVertices(2, vertexList)
        self.failUnlessEqual(faceConnects, vertexList)

        faceConnects.set(0, 0)
        faceConnects.set(3, 1)
        faceConnects.set(5, 2)
        faceConnects.set(4, 3)
        meshFn.getPolygonVertices(3, vertexList)
        self.failUnlessEqual(faceConnects, vertexList)

        faceConnects.set(0, 0)
        faceConnects.set(4, 1)
        faceConnects.set(7, 2)
        faceConnects.set(1, 3)
        meshFn.getPolygonVertices(4, vertexList)
        self.failUnlessEqual(faceConnects, vertexList)

        faceConnects.set(1, 0)
        faceConnects.set(7, 1)
        faceConnects.set(6, 2)
        faceConnects.set(2, 3)
        meshFn.getPolygonVertices(5, vertexList)
        self.failUnlessEqual(faceConnects, vertexList)
