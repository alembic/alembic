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
from alembic.Abc import *
from alembic.AbcCoreAbstract import *
from alembic.AbcGeom import *
from meshData import *

testList = []

kFacevaryingScope = GeometryScope.kFacevaryingScope
kWrapExisting = WrapExistingFlag.kWrapExisting

class Wrapest(unittest.TestCase):
    def testExport(self):
        """write an oarchive with some data in it"""

        meshyObj = OPolyMesh( OArchive( 'wrapTest1.abc' ).getTop() , 'meshy' )
        mesh = meshyObj.getSchema()

        # no pybind11 imath support for now
        #uvsamp = OV2fGeomParamSample( uvs, kFacevaryingScope )
        uvsamp = OV2fGeomParamSample()
        uvsamp.setVals(uvs)
        uvsamp.setScope(kFacevaryingScope)
        #nsamp  = ON3fGeomParamSample( normals, kFacevaryingScope )
        nsamp  = ON3fGeomParamSample()
        nsamp.setVals( normals )
        nsamp.setScope( kFacevaryingScope )
        indicesSample = Int32ArraySample(Int32Vector(indices.tolist()))
        countsSample = Int32ArraySample(Int32Vector(counts.tolist()))
        mesh_samp = OPolyMeshSchemaSample( P3fArraySample(), indicesSample, countsSample, uvsamp, nsamp )
        mesh_samp.setPositions(verts)

        # no pybind11 imath support for now
        # cbox = Box3d()
        # cbox.extendBy( V3d( 1.0, -1.0, 0.0 ) )
        # cbox.extendBy( V3d( -1.0, 1.0, 3.0 ) )

        # mesh.getChildBoundsProperty().setValue( cbox )
        mesh.set( mesh_samp )
        mesh.set( mesh_samp )

    def testImport(self):
        """read in an archive and try wrapping it as various objects"""

        arch = IArchive('wrapTest1.abc')
        obj = IObject(arch.getTop(), "meshy")
        md = obj.getMetaData()

        self.assertTrue(IPolyMesh.matches(md))
        self.assertFalse(ISubD.matches(md))

        # wrap object as PolyMesh
        mesh = IPolyMesh(obj, kWrapExisting)
        self.assertTrue(mesh.valid())

        # wrap object as SubD
        subd = ISubD(obj, kWrapExisting)
        self.assertTrue(subd.valid())

        # this should throw an exception
        self.assertRaises(RuntimeError, ICamera, obj, kWrapExisting)
        self.assertRaises(RuntimeError, IPolyMesh, arch.getTop(), kWrapExisting)
