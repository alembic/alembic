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

class ColorSetsTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files :
            os.remove(f)

    def testStaticMeshStaticColor(self):
        red_array = OpenMaya.MColorArray()
        red_array.append(1.0, 0.0, 0.0)

        blue_array = OpenMaya.MColorArray()
        blue_array.append(0.0, 0.0, 1.0)
        single_indices =  OpenMaya.MIntArray(4, 0)

        mixed_colors = [[1.0, 1.0, 0.0, 1.0], [0.0, 1.0, 1.0, 0.75],
            [1.0, 0.0, 1.0, 0.5], [1.0, 1.0, 1.0, 0.25]]

        mixed_array = OpenMaya.MColorArray()
        for x in mixed_colors:
            mixed_array.append(x[0], x[1], x[2], x[3])

        mixed_indices =  OpenMaya.MIntArray()
        for i in range(4):
            mixed_indices.append(i)

        MayaCmds.polyPlane(sx=1, sy=1, name='poly')
        MayaCmds.select('polyShape')
        sel = OpenMaya.MSelectionList()
        OpenMaya.MGlobal.getActiveSelectionList(sel)
        obj = OpenMaya.MObject()
        sel.getDependNode(0, obj)
        fn = OpenMaya.MFnMesh(obj)

        fn.createColorSetWithName('reds')
        fn.createColorSetWithName('mixed')

        fn.setColors(red_array, 'reds', OpenMaya.MFnMesh.kRGB)
        fn.assignColors(single_indices, 'reds')

        fn.setColors(mixed_array, 'mixed', OpenMaya.MFnMesh.kRGBA)
        fn.assignColors(mixed_indices, 'mixed')

        fn.setCurrentColorSetName('mixed')

        MayaCmds.polyPlane(sx=1, sy=1, name='subd')
        MayaCmds.select('subdShape')

        sel = OpenMaya.MSelectionList()
        OpenMaya.MGlobal.getActiveSelectionList(sel)
        obj = OpenMaya.MObject()
        sel.getDependNode(0, obj)
        fn = OpenMaya.MFnMesh(obj)

        fn.createColorSetWithName('blues')
        fn.createColorSetWithName('mixed')

        fn.setColors(blue_array, 'blues', OpenMaya.MFnMesh.kRGB)
        fn.assignColors(single_indices, 'blues')

        fn.setColors(mixed_array, 'mixed', OpenMaya.MFnMesh.kRGBA)
        fn.assignColors(mixed_indices, 'mixed')

        fn.setCurrentColorSetName('blues')

        MayaCmds.addAttr(longName='SubDivisionMesh', attributeType='bool',
            defaultValue=True)

        self.__files.append(util.expandFileName('staticColorSets.abc'))
        MayaCmds.AbcExport(j='-root poly -root subd -wcs -file ' +
            self.__files[-1])

        MayaCmds.AbcImport(self.__files[-1], mode='open')

        MayaCmds.select('polyShape')
        sel = OpenMaya.MSelectionList()
        OpenMaya.MGlobal.getActiveSelectionList(sel)
        obj = OpenMaya.MObject()
        sel.getDependNode(0, obj)
        fn = OpenMaya.MFnMesh(obj)

        self.failUnless(fn.currentColorSetName() == 'mixed')

        setNames = []
        fn.getColorSetNames(setNames)
        self.failUnless(len(setNames) == 2)
        self.failUnless(setNames.count('mixed') == 1)
        self.failUnless(setNames.count('reds') == 1)

        colArray = OpenMaya.MColorArray()
        fn.getFaceVertexColors(colArray, 'reds')
        self.failUnless(colArray.length() == 4)
        for x in range(colArray.length()):
            self.failUnless(colArray[x].r == 1)
            self.failUnless(colArray[x].g == 0)
            self.failUnless(colArray[x].b == 0)
        fn.getFaceVertexColors(colArray, 'mixed')
        self.failUnless(colArray.length() == 4)
        for x in range(colArray.length()):
            self.failUnless(colArray[x].r == mixed_colors[x][0])
            self.failUnless(colArray[x].g == mixed_colors[x][1])
            self.failUnless(colArray[x].b == mixed_colors[x][2])
            self.failUnless(colArray[x].a == mixed_colors[x][3])

        MayaCmds.select('subdShape')
        self.failUnless(MayaCmds.getAttr('subdShape.SubDivisionMesh') == 1)
        sel = OpenMaya.MSelectionList()
        OpenMaya.MGlobal.getActiveSelectionList(sel)
        obj = OpenMaya.MObject()
        sel.getDependNode(0, obj)
        fn = OpenMaya.MFnMesh(obj)

        self.failUnless(fn.currentColorSetName() == 'blues')

        setNames = []
        fn.getColorSetNames(setNames)
        self.failUnless(len(setNames) == 2)
        self.failUnless(setNames.count('mixed') == 1)
        self.failUnless(setNames.count('blues') == 1)

        colArray = OpenMaya.MColorArray()
        fn.getFaceVertexColors(colArray, 'blues')
        self.failUnless(colArray.length() == 4)
        for x in range(colArray.length()):
            self.failUnless(colArray[x].r == 0)
            self.failUnless(colArray[x].g == 0)
            self.failUnless(colArray[x].b == 1)

        fn.getFaceVertexColors(colArray, 'mixed')
        self.failUnless(colArray.length() == 4)
        for x in range(colArray.length()):
            self.failUnless(colArray[x].r == mixed_colors[x][0])
            self.failUnless(colArray[x].g == mixed_colors[x][1])
            self.failUnless(colArray[x].b == mixed_colors[x][2])
            self.failUnless(colArray[x].a == mixed_colors[x][3])

    def testStaticMeshAnimColor(self):
        MayaCmds.currentTime(1)
        MayaCmds.polyPlane(sx=1, sy=1, name='poly')
        MayaCmds.polyColorPerVertex(r=0.0,g=1.0,b=0.0, cdo=True)
        MayaCmds.setKeyframe(["polyColorPerVertex1"])
        MayaCmds.currentTime(10)
        MayaCmds.polyColorPerVertex(r=0.0,g=0.0,b=1.0, cdo=True)
        MayaCmds.setKeyframe(["polyColorPerVertex1"])

        MayaCmds.currentTime(1)
        MayaCmds.polyPlane(sx=1, sy=1, name='subd')
        MayaCmds.select('subdShape')
        MayaCmds.addAttr(longName='SubDivisionMesh', attributeType='bool',
            defaultValue=True)
        MayaCmds.polyColorPerVertex(r=1.0,g=1.0,b=0.0, cdo=True)
        MayaCmds.setKeyframe(["polyColorPerVertex2"])
        MayaCmds.currentTime(10)
        MayaCmds.polyColorPerVertex(r=1.0,g=0.0,b=0.0, cdo=True)
        MayaCmds.setKeyframe(["polyColorPerVertex2"])

        self.__files.append(util.expandFileName('animColorSets.abc'))
        MayaCmds.AbcExport(j='-fr 1 10 -root poly -root subd -wcs -file ' +
            self.__files[-1])

        MayaCmds.AbcImport(self.__files[-1], mode='open')

        MayaCmds.select('polyShape')
        sel = OpenMaya.MSelectionList()
        OpenMaya.MGlobal.getActiveSelectionList(sel)
        obj = OpenMaya.MObject()
        sel.getDependNode(0, obj)
        fn = OpenMaya.MFnMesh(obj)

        MayaCmds.currentTime(1)
        colArray = OpenMaya.MColorArray()
        fn.getFaceVertexColors(colArray)
        self.failUnless(colArray.length() == 4)
        for x in range(colArray.length()):
            self.failUnlessAlmostEqual(colArray[x].r, 0)
            self.failUnlessAlmostEqual(colArray[x].g, 1)
            self.failUnlessAlmostEqual(colArray[x].b, 0)

        MayaCmds.currentTime(5)
        colArray = OpenMaya.MColorArray()
        fn.getFaceVertexColors(colArray)
        self.failUnless(colArray.length() == 4)
        for x in range(colArray.length()):
            self.failUnless(colArray[x].r == 0)
            self.failUnlessAlmostEqual(colArray[x].g, 0.555555582047)
            self.failUnlessAlmostEqual(colArray[x].b, 0.444444447756)

        MayaCmds.currentTime(10)
        colArray = OpenMaya.MColorArray()
        fn.getFaceVertexColors(colArray)
        self.failUnless(colArray.length() == 4)
        for x in range(colArray.length()):
            self.failUnlessAlmostEqual(colArray[x].r, 0)
            self.failUnlessAlmostEqual(colArray[x].g, 0)
            self.failUnlessAlmostEqual(colArray[x].b, 1)

        self.failUnless(MayaCmds.getAttr('subdShape.SubDivisionMesh') == 1)
        MayaCmds.select('subdShape')
        sel = OpenMaya.MSelectionList()
        OpenMaya.MGlobal.getActiveSelectionList(sel)
        obj = OpenMaya.MObject()
        sel.getDependNode(0, obj)
        fn = OpenMaya.MFnMesh(obj)

        MayaCmds.currentTime(1)
        colArray = OpenMaya.MColorArray()
        fn.getFaceVertexColors(colArray)
        self.failUnless(colArray.length() == 4)
        for x in range(colArray.length()):
            self.failUnlessAlmostEqual(colArray[x].r, 1)
            self.failUnlessAlmostEqual(colArray[x].g, 1)
            self.failUnlessAlmostEqual(colArray[x].b, 0)

        MayaCmds.currentTime(5)
        colArray = OpenMaya.MColorArray()
        fn.getFaceVertexColors(colArray)
        self.failUnless(colArray.length() == 4)
        for x in range(colArray.length()):
            self.failUnlessAlmostEqual(colArray[x].r, 1)
            self.failUnlessAlmostEqual(colArray[x].g, 0.555555582047)
            self.failUnlessAlmostEqual(colArray[x].b, 0)

        MayaCmds.currentTime(10)
        colArray = OpenMaya.MColorArray()
        fn.getFaceVertexColors(colArray)
        self.failUnless(colArray.length() == 4)
        for x in range(colArray.length()):
            self.failUnlessAlmostEqual(colArray[x].r, 1)
            self.failUnlessAlmostEqual(colArray[x].g, 0)
            self.failUnlessAlmostEqual(colArray[x].b, 0)
