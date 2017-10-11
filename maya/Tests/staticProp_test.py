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

class staticPropTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files :
            os.remove(f)

    def setProps(self, nodeName):
        MayaCmds.select(nodeName)
        MayaCmds.addAttr(longName='SPT_int8', defaultValue=8,
            attributeType='byte', keyable=True)
        MayaCmds.addAttr(longName='SPT_int16', defaultValue=16,
            attributeType='short', keyable=True)
        MayaCmds.addAttr(longName='SPT_int32', defaultValue=32,
            attributeType='long', keyable=True)
        MayaCmds.addAttr(longName='SPT_float',  defaultValue=3.2654,
            attributeType='float', keyable=True)

        MayaCmds.addAttr(longName='SPT_double', defaultValue=0.15724757,
            attributeType='double', keyable=True)
        MayaCmds.addAttr(longName='SPT_double_AbcGeomScope', dataType="string")
        MayaCmds.setAttr(nodeName+'.SPT_double_AbcGeomScope', "vtx",
            type="string")

        MayaCmds.addAttr(longName='SPT_string', dataType="string")
        MayaCmds.setAttr(nodeName+'.SPT_string', "empty", type="string")
        MayaCmds.addAttr(longName='SPT_string_AbcGeomScope', dataType="string")
        MayaCmds.setAttr(nodeName+'.SPT_string_AbcGeomScope', "potato",
            type="string")

        MayaCmds.addAttr(longName='SPT_int32_array', dataType='Int32Array')
        MayaCmds.setAttr(nodeName+'.SPT_int32_array', [6, 7, 8, 9, 10],
            type='Int32Array')

        MayaCmds.addAttr(longName='SPT_vector_array', dataType='vectorArray')
        MayaCmds.setAttr(nodeName+'.SPT_vector_array', 3,
            (1,1,1), (2,2,2), (3,3,3), type='vectorArray')
        MayaCmds.addAttr(longName='SPT_vector_array_AbcType', dataType="string")
        MayaCmds.setAttr(nodeName+'.SPT_vector_array_AbcType', "normal2",
            type="string")

        MayaCmds.addAttr(longName='SPT_point_array', dataType='pointArray')
        MayaCmds.setAttr(nodeName+'.SPT_point_array', 2,
            (2,4,6,8), (20,40,60,80), type='pointArray')

        MayaCmds.addAttr(longName='SPT_double_array', dataType='doubleArray')
        MayaCmds.addAttr(longName='SPT_double_array_AbcGeomScope',
            dataType="string")
        MayaCmds.setAttr(nodeName+'.SPT_double_array',
            [1.1, 2.2, 3.3, 4.4, 5.5], type='doubleArray')
        MayaCmds.setAttr(nodeName+'.SPT_double_array_AbcGeomScope', "vtx",
            type="string")

        MayaCmds.addAttr(longName='SPT_float_array', dataType='floatArray')
        MayaCmds.addAttr(longName='SPT_float_array_AbcGeomScope',
            dataType="string")
        MayaCmds.setAttr(nodeName+'.SPT_float_array',
            [5.0, 4.75, 3.5, 2.25, 1.0], type='floatArray')
        MayaCmds.setAttr(nodeName+'.SPT_float_array_AbcGeomScope', "vtx",
            type="string")

        MayaCmds.addAttr(longName='SPT_string_array', dataType='stringArray')
        MayaCmds.setAttr(nodeName+'.SPT_string_array', 3, "string1", "string2", "string3",
            type='stringArray')

    def verifyProps(self, nodeName, fileName):
        MayaCmds.AbcImport(fileName, mode='open')
        self.failUnlessEqual(8,  MayaCmds.getAttr(nodeName+'.SPT_int8'))
        self.failUnlessEqual(16, MayaCmds.getAttr(nodeName+'.SPT_int16'))
        self.failUnlessEqual(32, MayaCmds.getAttr(nodeName+'.SPT_int32'))
        self.failUnlessAlmostEqual(3.2654,
            MayaCmds.getAttr(nodeName+'.SPT_float'), 4)
        self.failUnlessAlmostEqual(0.15724757,
            MayaCmds.getAttr(nodeName+'.SPT_double'), 7)
        self.failUnlessEqual('vtx',
            MayaCmds.getAttr(nodeName+'.SPT_double_AbcGeomScope'))
        self.failUnlessEqual('empty', MayaCmds.getAttr(nodeName+'.SPT_string'))
        self.failUnlessEqual(0, MayaCmds.attributeQuery(
            'SPT_string_AbcGeomScope', node=nodeName, exists=True))
        self.failUnlessEqual([6, 7, 8, 9, 10],
            MayaCmds.getAttr(nodeName+'.SPT_int32_array'))
        self.failUnlessEqual(["string1", "string2", "string3"],
            MayaCmds.getAttr(nodeName+'.SPT_string_array'))
        self.failUnlessEqual([1.1, 2.2, 3.3, 4.4, 5.5],
            MayaCmds.getAttr(nodeName+'.SPT_double_array'))
        self.failUnlessEqual([5.0, 4.75, 3.5, 2.25, 1.0],
            MayaCmds.getAttr(nodeName+'.SPT_float_array'))
        self.failUnlessEqual([(1.0, 1.0, 0.0), (2.0, 2.0, 0.0), (3.0, 3.0, 0.0)],
            MayaCmds.getAttr(nodeName+'.SPT_vector_array'))
        self.failUnlessEqual('normal2',
            MayaCmds.getAttr(nodeName+'.SPT_vector_array_AbcType'))
        self.failUnlessEqual([(2.0, 4.0, 6.0, 1.0), (20.0, 40.0, 60.0, 1.0)],
            MayaCmds.getAttr(nodeName+'.SPT_point_array'))

    def testStaticTransformPropReadWrite(self):
        nodeName = MayaCmds.createNode('transform')
        self.setProps(nodeName)
        self.__files.append(util.expandFileName('staticPropTransform.abc'))
        MayaCmds.AbcExport(j='-atp SPT_ -root %s -file %s' % (nodeName, self.__files[-1]))
        self.verifyProps(nodeName, self.__files[-1])

    def testStaticCameraPropReadWrite(self):
        root = MayaCmds.camera()
        nodeName = root[0]
        shapeName = root[1]
        self.setProps(shapeName)
        self.__files.append(util.expandFileName('staticPropCamera.abc'))
        MayaCmds.AbcExport(j='-atp SPT_ -root %s -f %s' % (nodeName, self.__files[-1]))
        self.verifyProps(shapeName, self.__files[-1])

    def testStaticParticlePropReadWrite(self):
        root = MayaCmds.particle(p=[(0, 0, 0), (1, 1, 1)])
        nodeName = root[0]
        shapeName = root[1]
        self.setProps(shapeName)
        self.__files.append(util.expandFileName('staticPropParticles.abc'))
        MayaCmds.AbcExport(j='-atp SPT_ -root %s -f %s' % (nodeName, self.__files[-1]))
        self.verifyProps(shapeName, self.__files[-1])

    def testStaticMeshPropReadWrite(self):
        nodeName = 'polyCube'
        shapeName = 'polyCubeShape'
        MayaCmds.polyCube(name=nodeName)
        self.setProps(shapeName)
        self.__files.append(util.expandFileName('staticPropMesh.abc'))
        MayaCmds.AbcExport(j='-atp SPT_ -root %s -f %s' % (nodeName, self.__files[-1]))
        self.verifyProps(shapeName, self.__files[-1])

    def testStaticNurbsCurvePropReadWrite(self):
        nodeName = 'nCurve'
        shapeName = 'curveShape1'
        MayaCmds.curve(p=[(0, 0, 0), (3, 5, 6), (5, 6, 7), (9, 9, 9)],
            name=nodeName)
        self.setProps(shapeName)
        self.__files.append(util.expandFileName('staticPropCurve.abc'))
        MayaCmds.AbcExport(j='-atp SPT_ -root %s -f %s' % (nodeName, self.__files[-1]))
        self.verifyProps(shapeName, self.__files[-1])

    def testStaticNurbsSurfacePropReadWrite(self):
        nodeName = 'nSphere'
        shapeName = 'nSphereShape'
        MayaCmds.sphere(name=nodeName)
        self.setProps(shapeName)
        self.__files.append(util.expandFileName('staticPropNurbs.abc'))
        MayaCmds.AbcExport(j='-atp SPT_ -root %s -file %s' % (nodeName, self.__files[-1]))
        self.verifyProps(shapeName, self.__files[-1])
