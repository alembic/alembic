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
from imath import *
from alembic.AbcCoreAbstract import *
from alembic.Abc import *
from alembic.AbcGeom import *
from alembic.AbcMaterial import *


kFacevaryingScope = GeometryScope.kFacevaryingScope
class CameraTest(unittest.TestCase):
    def testMaterialExport(self):
        """write an oarchive with a material in it"""

        materials = OObject( OArchive( 'material.abc' ).getTop() , 'materials' )
        matObj = OMaterial( materials, "material1" )
        mat = matObj.getSchema()
        self.assertTrue(materials.valid())
        self.assertTrue(materials)
        self.assertTrue(matObj)
        self.assertTrue(matObj.valid())
        self.assertTrue(mat)
        self.assertTrue(mat.valid())


        mat.setShader( 'prman', 'surface', 'paintedplastic' )
        mat.setShader( 'prman', 'displacement', 'knobby' )

        mat.setShader( 'arnold', 'surface', 'paintedplastic' )

        prop = OFloatProperty(
            mat.getShaderParameters( 'prman', 'surface' ), 'Kd' )
        prop.setValue( 0.5 )

        prop = OStringProperty(
            mat.getShaderParameters( 'prman', 'surface' ), 'texname' )
        prop.setValue( 'taco' )

    def testMaterialImport(self):
        """read an iarchive with a material in it"""

        materials = IObject( IArchive( 'material.abc' ).getTop() , 'materials' )
        matObj = IMaterial( materials, "material1" )
        mat = matObj.getSchema()
        self.assertTrue(materials.valid())
        self.assertTrue(matObj.valid())
        self.assertTrue(mat.valid())

        self.assertTrue(materials)
        self.assertTrue(matObj)
        self.assertTrue(mat)

        targetNames = mat.getTargetNames()

        self.assertEqual(len( targetNames ), 2)
        self.assertTrue( ( targetNames[0] == 'arnold' and targetNames[1] == 'prman' ) or
                ( targetNames[1] == 'arnold' and targetNames[0] == 'prman' ) )

        for targetName in targetNames:
            shaderTypeNames = mat.getShaderTypesForTarget( targetName )

            if targetName == 'prman' :
                self.assertEqual(len( shaderTypeNames ), 2)
                self.assertTrue( ( shaderTypeNames[0] == 'surface' and
                       shaderTypeNames[1] == 'displacement' ) or
                     ( shaderTypeNames[1] == 'surface' and
                       shaderTypeNames[0] == 'displacement' ) )
            else :
                self.assertEqual(targetName, 'arnold')
                self.assertTrue(len( shaderTypeNames ) == 1 and shaderTypeNames[0])

            for shaderTypeName in shaderTypeNames:

                shaderName = mat.getShader( targetName, shaderTypeName )

                if targetName == 'prman' and shaderTypeName == 'surface':
                    self.assertEqual(shaderName, 'paintedplastic')
                elif targetName == 'prman' and shaderTypeName == 'displacement':
                    self.assertEqual(shaderName, 'knobby')
                elif targetName == 'arnold' and shaderTypeName == 'surface':
                    self.assertEqual(shaderName, 'paintedplastic')

                params = mat.getShaderParameters( targetName, shaderTypeName )

                if params.valid():
                    self.assertEqual(targetName, 'prman')
                    self.assertEqual(shaderTypeName, 'surface')

                    for i in range(0, params.getNumProperties()):
                        propHeader = params.getPropertyHeader( i )

                        if propHeader.isScalar():
                            if IStringProperty.matches( propHeader ):
                                prop = IStringProperty( params,
                                                    propHeader.getName() )
                                self.assertEqual(propHeader.getName(), 'texname')
                                self.assertEqual(prop.getValue(), 'taco' )
                            elif IFloatProperty.matches( propHeader ):
                                prop = IFloatProperty( params,
                                                   propHeader.getName() )
                                self.assertEqual( propHeader.getName(), 'Kd')
                                self.assertEqual( prop.getValue(), 0.5 )
