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

def getObjFromName(nodeName):
    selectionList = OpenMaya.MSelectionList()
    selectionList.add(nodeName)
    obj = OpenMaya.MObject()
    selectionList.getDependNode(0, obj)
    return obj

class PolyNormalsTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testSet_noNormals_Attr(self):

        MayaCmds.polyCube(name='polyCube')
        # add the necessary props
        MayaCmds.select('polyCubeShape')
        MayaCmds.addAttr(longName='SubDivisionMesh', attributeType='bool',
            defaultValue=False)
        MayaCmds.addAttr(longName='interpolateBoundary', attributeType='bool',
            defaultValue=True)
        MayaCmds.addAttr(longName='noNormals', attributeType='bool',
            defaultValue=False)
        MayaCmds.addAttr(longName='flipNormals', attributeType='bool',
            defaultValue=False)
        MayaCmds.addAttr(longName='faceVaryingInterpolateBoundary',
            attributeType='bool', defaultValue=False)

        MayaCmds.polySphere(name='polySphere')

        # add the necessary props
        MayaCmds.select('polySphereShape')
        MayaCmds.addAttr(longName='SubDivisionMesh', attributeType='bool',
            defaultValue=False)
        MayaCmds.addAttr(longName='interpolateBoundary', attributeType='bool',
            defaultValue=True)
        MayaCmds.addAttr(longName='noNormals', attributeType='bool',
            defaultValue=True)
        MayaCmds.addAttr(longName='flipNormals', attributeType='bool',
            defaultValue=False)
        MayaCmds.addAttr(longName='faceVaryingInterpolateBoundary',
            attributeType='bool', defaultValue=False)

        #ignore facevaryingType, subdPaintLev
        MayaCmds.group('polyCube', 'polySphere', name='polygons')

        self.__files.append(util.expandFileName('staticPoly_noNormals_AttrTest.abc'))
        MayaCmds.AbcExport(j='-root polygons -f ' + self.__files[-1])

        # reading test
        MayaCmds.AbcImport(self.__files[-1], mode='open', debug=False)
        # make sure the noNormal attribute is set correctly when the file is loaded
        self.failIf(MayaCmds.listAttr('polyCubeShape').count('noNormals') != 0)
        self.failUnless(MayaCmds.getAttr('polySphereShape.noNormals'))

    def testStaticMeshPolyNormals(self):

        # create a polygon cube
        polyName = 'polyCube'
        polyShapeName = 'polyCubeShape'
        MayaCmds.polyCube( sx=1, sy=1, sz=1, name=polyName,
            constructionHistory=False)

        # add the necessary props
        MayaCmds.select(polyShapeName)
        MayaCmds.addAttr(longName='SubDivisionMesh', attributeType='bool',
            defaultValue=False)
        MayaCmds.addAttr(longName='noNormals', attributeType='bool',
            defaultValue=False)
        MayaCmds.addAttr(longName='flipNormals', attributeType='bool',
            defaultValue=False)

        # tweek some normals
        MayaCmds.select(polyName+'.vtxFace[2][1]', replace=True)
        MayaCmds.polyNormalPerVertex(xyz=(0.707107, 0.707107, 0))
        MayaCmds.select(polyName+'.vtxFace[7][4]', replace=True)
        MayaCmds.polyNormalPerVertex(xyz=(-0.707107, 0.707107, 0))

        # write to file
        self.__files.append(util.expandFileName('staticPolyNormalsTest.abc'))
        MayaCmds.AbcExport(j='-root %s -f %s' % (polyName, self.__files[-1]))

        # read back from file
        MayaCmds.AbcImport(self.__files[-1], mode='import')


        self.failIf(MayaCmds.listAttr('polyCube1|polyCubeShape').count('noNormals') != 0)

        # make sure the normals are the same
        shapeObj = getObjFromName('polyCube1|polyCubeShape')
        fnMesh = OpenMaya.MFnMesh(shapeObj)
        numFaces = fnMesh.numPolygons()
        for faceIndex in range(0, numFaces):
            vertexList = OpenMaya.MIntArray()
            fnMesh.getPolygonVertices(faceIndex, vertexList)
            numVertices = vertexList.length()
            for v in range(0, numVertices):
                vertexIndex = vertexList[v]
                normal = OpenMaya.MVector()
                fnMesh.getFaceVertexNormal(faceIndex, vertexIndex, normal)
                vtxFaceAttrName = '.vtxFace[%d][%d]' % (vertexIndex, faceIndex)
                MayaCmds.select(polyName+vtxFaceAttrName, replace=True)
                oNormal = MayaCmds.polyNormalPerVertex( query=True, xyz=True)
                self.failUnlessAlmostEqual(normal[0], oNormal[0], 4)
                self.failUnlessAlmostEqual(normal[1], oNormal[1], 4)
                self.failUnlessAlmostEqual(normal[2], oNormal[2], 4)

    def testAnimatedMeshPolyNormals(self):

        # create a polygon cube
        polyName = 'polyCube'
        polyShapeName = 'polyCubeShape'
        MayaCmds.polyCube(sx=1, sy=1, sz=1, name=polyName, constructionHistory=False)

        # add the necessary props
        MayaCmds.select(polyShapeName)
        MayaCmds.addAttr(longName='SubDivisionMesh', attributeType='bool',
            defaultValue=False)
        MayaCmds.addAttr(longName='noNormals', attributeType='bool',
            defaultValue=False)
        MayaCmds.addAttr(longName='flipNormals', attributeType='bool',
            defaultValue=False)

        # tweek some normals
        MayaCmds.select(polyName+'.vtxFace[2][1]', replace=True)
        MayaCmds.polyNormalPerVertex(xyz=(0.707107, 0.707107, 0))
        MayaCmds.select(polyName+'.vtxFace[7][4]', replace=True)
        MayaCmds.polyNormalPerVertex(xyz=(-0.707107, 0.707107, 0))

        # set keyframes to make sure normals are written out as animated
        MayaCmds.setKeyframe(polyShapeName+'.vtx[0:7]', time=[1, 4])
        MayaCmds.currentTime(2, update=True)
        MayaCmds.select(polyShapeName+'.vtx[0:3]')
        MayaCmds.move(0, 0.5, 1, relative=True)
        MayaCmds.setKeyframe(polyShapeName+'.vtx[0:7]', time=[2])

        # write to file
        self.__files.append(util.expandFileName('animPolyNormalsTest.abc'))
        MayaCmds.AbcExport(j='-fr 1 4 -root %s -f %s' % (polyName, self.__files[-1]))


        # read back from file
        MayaCmds.AbcImport(self.__files[-1], mode='import')

        # make sure the noNormal attribute is set correctly when the file is
        # loaded
        self.failIf(MayaCmds.listAttr('polyCube1|polyCubeShape').count('noNormals') != 0)

        # make sure the normals are the same
        for time in range(1, 5):

            MayaCmds.currentTime(time, update=True)

            shapeObj = getObjFromName('polyCube1|polyCubeShape')
            fnMesh = OpenMaya.MFnMesh(shapeObj)
            numFaces = fnMesh.numPolygons()

            for faceIndex in range(0, numFaces):
                vertexList = OpenMaya.MIntArray()
                fnMesh.getPolygonVertices(faceIndex, vertexList)
                numVertices = vertexList.length()
                for v in range(0, numVertices):
                    vertexIndex = vertexList[v]
                    normal = OpenMaya.MVector()
                    fnMesh.getFaceVertexNormal(faceIndex, vertexIndex, normal)
                    vtxFaceAttrName = '.vtxFace[%d][%d]' % (vertexIndex,
                        faceIndex)
                    MayaCmds.select(polyName+vtxFaceAttrName, replace=True)
                    oNormal = MayaCmds.polyNormalPerVertex(query=True, xyz=True)
                    self.failUnlessAlmostEqual(normal[0], oNormal[0], 4)
                    self.failUnlessAlmostEqual(normal[1], oNormal[1], 4)
                    self.failUnlessAlmostEqual(normal[2], oNormal[2], 4)
