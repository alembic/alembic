#-******************************************************************************
#
# Copyright (c) 2012,
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
#-******************************************************************************

import unittest
from alembic import Abc
from alembic import AbcMaterial

def setFloatParameter(schema, target, shaderType, paramName, value):
    prop = Abc.OFloatProperty(schema.getShaderParameters(target, shaderType),
            paramName)
    prop.setValue(value)

class MaterialAssignTest(unittest.TestCase):
    def testMaterialExport(self):
        archive = Abc.OArchive("MaterialAssignment.abc")
        root = archive.getTop()
        materials = Abc.OObject(root, "materials")
        geometry = Abc.OObject(root, "geometry")

        # parent material
        materialA = AbcMaterial.OMaterial(materials, "materialA")
        materialA.getSchema().setShader("prman", "surface", "paintedplastic")
        setFloatParameter(materialA.getSchema(),
            "prman", "surface", "Kd", 0.5)
        setFloatParameter(materialA.getSchema(),
            "prman", "surface", "roughness", 0.1)

        # child material
        materialB = AbcMaterial.OMaterial(materialA, "materialB")
        materialB.getSchema().setShader("prman", "displacement", "knobby")
        setFloatParameter(materialB.getSchema(),
            "prman", "surface", "roughness", 0.2)

        geoA = Abc.OObject(geometry, "geoA")
        AbcMaterial.addMaterialAssignment(geoA, "/materials/materialA")

        geoB = Abc.OObject(geometry, "geoB");
        AbcMaterial.addMaterialAssignment(geoB, "/materials/materialA/materialB")

        geoC = Abc.OObject(geometry, "geoC");
        AbcMaterial.addMaterialAssignment(geoC, "/materials/materialA/materialB")
        geoCMat = AbcMaterial.addMaterial(geoC);
        setFloatParameter(geoCMat, "prman", "surface", "roughness", 0.3)

    def testMaterialImport(self):
        archive = Abc.IArchive("MaterialAssignment.abc")
        root = archive.getTop()
        self.assertEqual(root.getNumChildren(), 2)

        materials = Abc.IObject(root, "materials")
        geometry = Abc.IObject(root, "geometry")

        materialA = AbcMaterial.IMaterial(materials, "materialA")
        ms = materialA.getSchema()
        self.assertEqual(ms.getShader("prman", "surface"), "paintedplastic")
        cp = ms.getShaderParameters("prman", "surface")
        kd = cp.getProperty('Kd')
        rg = cp.getProperty('roughness')
        self.assertTrue(kd.getNumSamples() == rg.getNumSamples() == 1)
        self.assertAlmostEqual(kd.samples()[0], 0.5)
        self.assertAlmostEqual(rg.samples()[0], 0.1)

        materialB = AbcMaterial.IMaterial(materialA, "materialB")
        ms = materialB.getSchema()
        self.assertEqual(ms.getShader("prman", "displacement"), "knobby")
        cp = ms.getShaderParameters("prman", "surface")
        rg = cp.getProperty('roughness')
        self.assertEqual(rg.getNumSamples(), 1)
        self.assertAlmostEqual(rg.samples()[0], 0.2)

        geoA = Abc.IObject(geometry, "geoA")
        geoB = Abc.IObject(geometry, "geoB")
        geoC = Abc.IObject(geometry, "geoC")

        self.assertEqual(AbcMaterial.getMaterialAssignmentPath(geoA), '/materials/materialA')
        self.assertEqual(AbcMaterial.getMaterialAssignmentPath(geoB), '/materials/materialA/materialB')
        self.assertEqual(AbcMaterial.getMaterialAssignmentPath(geoC), '/materials/materialA/materialB')

        matC = AbcMaterial.hasMaterial(geoC)
        cp = matC.getShaderParameters("prman", "surface")
        rg = cp.getProperty("roughness")
        self.assertEqual(rg.getNumSamples(), 1)
        self.assertAlmostEqual(rg.samples()[0], 0.3)
