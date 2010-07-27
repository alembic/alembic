
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

from maya import cmds as MayaCmds
import maya.OpenMaya as OpenMaya
import os
import subprocess
import unittest

class AnimMeshTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__takoStitcher = [os.environ['AlembicStitcher']]
        self.__files = []

    def tearDown(self):
        for f in self.__files :
            os.remove(f)

    def testAnimSPT_HwColor_ReadWrite(self):
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
        transFn.setName('poly')
        meshFn = OpenMaya.MFnMesh()
        meshFn.create(numVertices, numFaces, points, faceCounts,
            faceConnects, parent)
        shapeName = 'polyShape'
        meshFn.setName(shapeName)

        # add SPT_HwColor attributes
        MayaCmds.select( shapeName )
        MayaCmds.addAttr(longName='SPT_HwColorR', defaultValue=1.0,
            minValue=0.0, maxValue=1.0)
        MayaCmds.addAttr(longName='SPT_HwColorG', defaultValue=1.0,
            minValue=0.0, maxValue=1.0)
        MayaCmds.addAttr(longName='SPT_HwColorB', defaultValue=1.0,
            minValue=0.0, maxValue=1.0)
        MayaCmds.addAttr(longName='SPT_HwColor', usedAsColor=True,
            attributeType='float3')
        MayaCmds.addAttr(longName='Hw_ColorR', attributeType='float',
            parent='SPT_HwColor')
        MayaCmds.addAttr(longName='Hw_ColorG', attributeType='float',
            parent='SPT_HwColor')
        MayaCmds.addAttr(longName='Hw_ColorB', attributeType='float',
            parent='SPT_HwColor')

        # set colors
        MayaCmds.setAttr(shapeName+'.SPT_HwColorR', 0.50)
        MayaCmds.setAttr(shapeName+'.SPT_HwColorG', 0.15)
        MayaCmds.setAttr(shapeName+'.SPT_HwColorB', 0.75)
        MayaCmds.setAttr(shapeName+'.SPT_HwColor', 0.50, 0.15, 0.75,
            type='float3')

        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe('polyShape.vtx[0:7]')
        # colors
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColorR')
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColorG')
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColorB')
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColor')

        MayaCmds.currentTime(24, update=True)
        MayaCmds.setKeyframe('polyShape.vtx[0:7]')
        # colors
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColorR')
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColorG')
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColorB')
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColor')

        MayaCmds.currentTime(12, update=True)

        vtx_11 = OpenMaya.MFloatPoint(-1.0, -1.0, -1.0)
        vtx_22 = OpenMaya.MFloatPoint( 1.0, -1.0, -1.0)
        vtx_33 = OpenMaya.MFloatPoint( 1.0, -1.0,  1.0)
        vtx_44 = OpenMaya.MFloatPoint(-1.0, -1.0,  1.0)
        vtx_55 = OpenMaya.MFloatPoint(-1.0,  1.0, -1.0)
        vtx_66 = OpenMaya.MFloatPoint(-1.0,  1.0,  1.0)
        vtx_77 = OpenMaya.MFloatPoint( 1.0,  1.0,  1.0)
        vtx_88 = OpenMaya.MFloatPoint( 1.0,  1.0, -1.0)
        points.set(vtx_11, 0)
        points.set(vtx_22, 1)
        points.set(vtx_33, 2)
        points.set(vtx_44, 3)
        points.set(vtx_55, 4)
        points.set(vtx_66, 5)
        points.set(vtx_77, 6)
        points.set(vtx_88, 7)
        meshFn.setPoints(points)

        MayaCmds.setAttr( shapeName+'.SPT_HwColorR', 0.15)
        MayaCmds.setAttr( shapeName+'.SPT_HwColorG', 0.5)
        MayaCmds.setAttr( shapeName+'.SPT_HwColorB', 0.15)
        MayaCmds.setAttr( shapeName+'.SPT_HwColor', 0.15, 0.5, 0.15,
            type='float3')

        MayaCmds.setKeyframe('polyShape.vtx[0:7]')
        # colors
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColorR')
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColorG')
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColorB')
        MayaCmds.setKeyframe( shapeName+'.SPT_HwColor')

        self.__files.append('/tmp/animSPT_HwColor.hdf')
        self.__files.append('/tmp/animSPT_HwColor_01_14.hdf')
        self.__files.append('/tmp/animSPT_HwColor_15_24.hdf')

        MayaCmds.AlembicTakoExport('range  1 14 poly ' + self.__files[-2])
        MayaCmds.AlembicTakoExport('range 15 24 poly ' + self.__files[-1])

        subprocess.call(self.__takoStitcher + self.__files[-3:])

        # reading test
        MayaCmds.AlembicTakoImport(self.__files[-3], mode='open', timeRange=[1, 24])

        MayaCmds.currentTime(1, update=True)

        sList = OpenMaya.MSelectionList();
        sList.add(shapeName);
        meshObj = OpenMaya.MObject();
        sList.getDependNode(0, meshObj);
        meshFn = OpenMaya.MFnMesh(meshObj)

        # check the point list
        meshPoints = OpenMaya.MFloatPointArray()
        meshFn.getPoints(meshPoints)
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

        places = 7

        # check colors
        colorR = MayaCmds.getAttr(shapeName+'.SPT_HwColorR')
        colorG = MayaCmds.getAttr(shapeName+'.SPT_HwColorG')
        colorB = MayaCmds.getAttr(shapeName+'.SPT_HwColorB')
        colorVec = MayaCmds.getAttr(shapeName+'.SPT_HwColor')[0]
        self.failUnlessAlmostEqual(colorR, 0.50, places)
        self.failUnlessAlmostEqual(colorG, 0.15, places)
        self.failUnlessAlmostEqual(colorB, 0.75, places)
        self.failUnlessAlmostEqual(colorVec[0], 0.50, places)
        self.failUnlessAlmostEqual(colorVec[1], 0.15, places)
        self.failUnlessAlmostEqual(colorVec[2], 0.75, places)

        MayaCmds.currentTime(12, update=True)
        meshPoints = OpenMaya.MFloatPointArray()
        meshFn.getPoints(meshPoints)
        self.failUnlessEqual(vtx_11, meshPoints[0])
        self.failUnlessEqual(vtx_22, meshPoints[1])
        self.failUnlessEqual(vtx_33, meshPoints[2])
        self.failUnlessEqual(vtx_44, meshPoints[3])
        self.failUnlessEqual(vtx_55, meshPoints[4])
        self.failUnlessEqual(vtx_66, meshPoints[5])
        self.failUnlessEqual(vtx_77, meshPoints[6])
        self.failUnlessEqual(vtx_88, meshPoints[7])
        colorR = MayaCmds.getAttr( shapeName+'.SPT_HwColorR' )
        colorG = MayaCmds.getAttr( shapeName+'.SPT_HwColorG' )
        colorB = MayaCmds.getAttr( shapeName+'.SPT_HwColorB' )
        colorVec = MayaCmds.getAttr( shapeName+'.SPT_HwColor' )[0]
        self.failUnlessAlmostEqual( colorR, 0.15, places)
        self.failUnlessAlmostEqual( colorG, 0.50, places)
        self.failUnlessAlmostEqual( colorB, 0.15, places)
        self.failUnlessAlmostEqual( colorVec[0], 0.15, places)
        self.failUnlessAlmostEqual( colorVec[1], 0.50, places)
        self.failUnlessAlmostEqual( colorVec[2], 0.15, places)

        MayaCmds.currentTime(24, update=True)
        meshPoints = OpenMaya.MFloatPointArray()
        meshFn.getPoints(meshPoints)
        self.failUnlessEqual(vtx_1, meshPoints[0])
        self.failUnlessEqual(vtx_2, meshPoints[1])
        self.failUnlessEqual(vtx_3, meshPoints[2])
        self.failUnlessEqual(vtx_4, meshPoints[3])
        self.failUnlessEqual(vtx_5, meshPoints[4])
        self.failUnlessEqual(vtx_6, meshPoints[5])
        self.failUnlessEqual(vtx_7, meshPoints[6])
        self.failUnlessEqual(vtx_8, meshPoints[7])
        colorR = MayaCmds.getAttr(shapeName+'.SPT_HwColorR')
        colorG = MayaCmds.getAttr(shapeName+'.SPT_HwColorG')
        colorB = MayaCmds.getAttr(shapeName+'.SPT_HwColorB')
        colorVec = MayaCmds.getAttr(shapeName+'.SPT_HwColor')[0]
        self.failUnlessAlmostEqual(colorR, 0.50, places)
        self.failUnlessAlmostEqual(colorG, 0.15, places)
        self.failUnlessAlmostEqual(colorB, 0.75, places)
        self.failUnlessAlmostEqual(colorVec[0], 0.50, places)
        self.failUnlessAlmostEqual(colorVec[1], 0.15, places)
        self.failUnlessAlmostEqual(colorVec[2], 0.75, places)
