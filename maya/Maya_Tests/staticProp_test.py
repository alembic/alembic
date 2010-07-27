
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
import os
import unittest


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
        MayaCmds.addAttr(longName='SPT_double_AttrScope', dataType="string")
        MayaCmds.addAttr(longName='SPT_double_AttrInput', dataType="string")
        MayaCmds.addAttr(longName='SPT_double_AttrOutput', dataType="string")

        MayaCmds.addAttr(longName='SPT_string', dataType="string")
        MayaCmds.addAttr(longName='SPT_string_AttrScope', dataType="string")
        MayaCmds.addAttr(longName='SPT_string_AttrInput', dataType="string")
        MayaCmds.addAttr(longName='SPT_string_AttrOutput', dataType="string")

        MayaCmds.addAttr(longName='SPT_vector_int16', dataType='Int32Array')
        MayaCmds.addAttr(longName='SPT_vector_int32', dataType='Int32Array')

        MayaCmds.addAttr(longName='SPT_vector_double', dataType='doubleArray')
        MayaCmds.addAttr(longName='SPT_vector_double_AttrScope',
            dataType="string")
        MayaCmds.addAttr(longName='SPT_vector_double_AttrInput',
            dataType="string")
        MayaCmds.addAttr(longName='SPT_vector_double_AttrOutput',
            dataType="string")

        MayaCmds.addAttr(longName='SPT_vector_string', dataType='stringArray')

        MayaCmds.setAttr(nodeName+'.SPT_double_AttrScope', "face",
            type="string")
        MayaCmds.setAttr(nodeName+'.SPT_double_AttrInput', "normal2",
            type="string")
        MayaCmds.setAttr(nodeName+'.SPT_double_AttrOutput', "point3",
            type="string")

        MayaCmds.setAttr(nodeName+'.SPT_string', "empty", type="string")
        MayaCmds.setAttr(nodeName+'.SPT_string_AttrScope', "point",
            type="string")
        MayaCmds.setAttr(nodeName+'.SPT_string_AttrInput', "color4",
            type="string")
        MayaCmds.setAttr(nodeName+'.SPT_string_AttrOutput', "bar",
            type="string")

        MayaCmds.setAttr(nodeName+'.SPT_vector_int16', [1, 2, 3, 4, 5],
            type='Int32Array')
        MayaCmds.setAttr(nodeName+'.SPT_vector_int32', [6, 7, 8, 9, 10],
            type='Int32Array')

        MayaCmds.setAttr(nodeName+'.SPT_vector_double',
            [1.1, 2.2, 3.3, 4.4, 5.5], type='doubleArray')
        MayaCmds.setAttr(nodeName+'.SPT_vector_double_AttrScope', "fake",
            type="string")
        MayaCmds.setAttr(nodeName+'.SPT_vector_double_AttrInput', "boo",
            type="string")
        MayaCmds.setAttr(nodeName+'.SPT_vector_double_AttrOutput', "vector3",
            type="string")

        MayaCmds.setAttr(nodeName+'.SPT_vector_string', 3, "string1", "string2", "string3",
            type='stringArray')

    def verifyProps(self, nodeName, fileName):
        MayaCmds.AlembicTakoImport(fileName, mode='open')
        self.failUnlessEqual(8,  MayaCmds.getAttr(nodeName+'.SPT_int8'))
        self.failUnlessEqual(16, MayaCmds.getAttr(nodeName+'.SPT_int16'))
        self.failUnlessEqual(32, MayaCmds.getAttr(nodeName+'.SPT_int32'))
        self.failUnlessAlmostEqual(3.2654,
            MayaCmds.getAttr(nodeName+'.SPT_float'), 4)
        self.failUnlessAlmostEqual(0.15724757,
            MayaCmds.getAttr(nodeName+'.SPT_double'), 7)
        self.failUnlessEqual('face',
            MayaCmds.getAttr(nodeName+'.SPT_double_AttrScope'))
        self.failUnlessEqual('normal2',
            MayaCmds.getAttr(nodeName+'.SPT_double_AttrInput'))
        self.failUnlessEqual('point3',
            MayaCmds.getAttr(nodeName+'.SPT_double_AttrOutput'))
        self.failUnlessEqual('empty', MayaCmds.getAttr(nodeName+'.SPT_string'))
        self.failUnlessEqual('point',
            MayaCmds.getAttr(nodeName+'.SPT_string_AttrScope'))
        self.failUnlessEqual('color4',
            MayaCmds.getAttr(nodeName+'.SPT_string_AttrInput'))
        self.failUnlessEqual(0, MayaCmds.attributeQuery(
            'SPT_string_AttrOutput', node=nodeName, exists=True))
        self.failUnlessEqual([1, 2, 3, 4, 5],
            MayaCmds.getAttr(nodeName+'.SPT_vector_int16'))
        self.failUnlessEqual([6, 7, 8, 9, 10],
            MayaCmds.getAttr(nodeName+'.SPT_vector_int32'))
        self.failUnlessEqual(0, MayaCmds.attributeQuery(
            'SPT_vector_double_AttrScope', node=nodeName, exists=True))
        self.failUnlessEqual(0, MayaCmds.attributeQuery(
            'SPT_vector_double_AttrInput', node=nodeName, exists=True))
        self.failUnlessEqual('vector3',
            MayaCmds.getAttr(nodeName+'.SPT_vector_double_AttrOutput'))
        self.failUnlessEqual(["string1", "string2", "string3"],
            MayaCmds.getAttr(nodeName+'.SPT_vector_string'))
        self.failUnlessEqual([1.1, 2.2, 3.3, 4.4, 5.5],
            MayaCmds.getAttr(nodeName+'.SPT_vector_double'))

    def testStaticTransformPropReadWrite(self):
        nodeName = MayaCmds.createNode('transform')
        self.setProps(nodeName)
        self.__files.append('/tmp/staticPropTransform.hdf')
        MayaCmds.AlembicTakoExport(nodeName + ' ' + self.__files[-1])
        self.verifyProps(nodeName, self.__files[-1])

    def testStaticCameraPropReadWrite(self):
        root = MayaCmds.camera()
        nodeName = root[0]
        shapeName = root[1]
        self.setProps(shapeName)
        self.__files.append('/tmp/staticPropCamera.hdf')
        MayaCmds.AlembicTakoExport(nodeName + ' ' + self.__files[-1])
        self.verifyProps(shapeName, self.__files[-1])

    def testStaticLightPropReadWrite(self):
        nodeName = 'spotLight'
        shapeName = 'spotLightShape'
        MayaCmds.spotLight(name=nodeName)
        self.setProps(shapeName)
        self.__files.append('/tmp/staticPropLight.hdf')
        MayaCmds.AlembicTakoExport(nodeName + ' ' + self.__files[-1])
        self.verifyProps(shapeName, self.__files[-1])


    def testStaticParticlePropReadWrite(self):
        root = MayaCmds.particle(p=[(0, 0, 0), (1, 1, 1)])
        nodeName = root[0]
        shapeName = root[1]
        self.setProps(shapeName)
        self.__files.append('/tmp/staticPropParticles.hdf')
        MayaCmds.AlembicTakoExport(nodeName + ' ' + self.__files[-1])
        self.verifyProps(shapeName, self.__files[-1])

    def testStaticMeshPropReadWrite(self):
        nodeName = 'polyCube'
        shapeName = 'polyCubeShape'
        MayaCmds.polyCube(name=nodeName)
        self.setProps(shapeName)
        self.__files.append('/tmp/staticPropMesh.hdf')
        MayaCmds.AlembicTakoExport(nodeName + ' ' + self.__files[-1])
        self.verifyProps(shapeName, self.__files[-1])

    def testStaticNurbsCurvePropReadWrite(self):
        nodeName = 'nCurve'
        shapeName = 'curveShape1'
        MayaCmds.curve(p=[(0, 0, 0), (3, 5, 6), (5, 6, 7), (9, 9, 9)],
            name=nodeName)
        self.setProps(shapeName)
        self.__files.append('/tmp/staticPropCurve.hdf')
        MayaCmds.AlembicTakoExport(nodeName + ' ' + self.__files[-1])
        self.verifyProps(shapeName, self.__files[-1])

    def testStaticNurbsSurfacePropReadWrite(self):
        nodeName = 'nSphere'
        shapeName = 'nSphereShape'
        MayaCmds.sphere(name=nodeName)
        self.setProps(shapeName)
        self.__files.append('/tmp/staticPropNurbs.hdf')
        MayaCmds.AlembicTakoExport(nodeName + ' ' + self.__files[-1])
        self.verifyProps(shapeName, self.__files[-1])
