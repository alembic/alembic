#-******************************************************************************
#
# Copyright (c) 2012-2013,
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

kFacevaryingScope = GeometryScope.kFacevaryingScope

class PolymeshTest(unittest.TestCase):
    def testMeshExport(self):
        """write an oarchive with a mesh in it"""

        meshyObj = OPolyMesh( OArchive( 'polyMesh1.abc' ).getTop() , 'meshy' )
        mesh = meshyObj.getSchema()

        # no pybind11 imath support for now
        # uvsamp = OV2fGeomParamSample( uvs, kFacevaryingScope )
        # uvsamp = OV2fGeomParamSample( uvs, kFacevaryingScope )
        # nsamp  = ON3fGeomParamSample( normals, kFacevaryingScope )
        # mesh_samp = OPolyMeshSchemaSample( verts, indices, counts, uvsamp, nsamp )

        indicesSample = Int32ArraySample(Int32Vector(indices.tolist()))
        countsSample = Int32ArraySample(Int32Vector(counts.tolist()))
        mesh_samp = OPolyMeshSchemaSample( P3fArraySample(), indicesSample, countsSample, OV2fGeomParamSample(), ON3fGeomParamSample() )
        mesh_samp.setPositions(verts)
        mesh_samp.setNormals(normals)
        mesh_samp.setUVs(uvs, kFacevaryingScope)

        mesh.setUVSourceName("NienNunb")

        # cbox = Box3d()
        # cbox.extendBy( V3d( 1.0, -1.0, 0.0 ) )
        # cbox.extendBy( V3d( -1.0, 1.0, 3.0 ) )
        # mesh.getChildBoundsProperty().setValue( cbox )
        # mesh.getChildBoundsProperty().setValue( cbox )

        mesh.set( mesh_samp )
        mesh.set( mesh_samp )

    def testMeshImport(self):
        """read an iarchive with a mesh in it"""

        geomBase = IGeomBaseObject( IArchive( 'polyMesh1.abc' ).getTop(), 'meshy' )
        self.assertTrue(geomBase.getSchema().getSelfBoundsProperty().valid())

        meshyObj = IPolyMesh( IArchive( 'polyMesh1.abc' ).getTop(), 'meshy' )
        mesh = meshyObj.getSchema()
        N = mesh.getNormalsParam()
        uv = mesh.getUVsParam()

        self.assertEqual(uv.getMetaData().getSourceName(), "NienNunb")
        self.assertFalse(N.isIndexed())
        self.assertFalse(uv.isIndexed())

        meshSamp = mesh.getValue()
        baseSamp = geomBase.getSchema().getValue()

        # no imath support for pybind11
        # self.assertEqual(meshSamp.getSelfBounds().min(), V3d( -1.0, -1.0, -1.0 ))
        # self.assertEqual(meshSamp.getSelfBounds().max(), V3d(  1.0,  1.0,  1.0 ))
        # self.assertEqual(baseSamp.getSelfBounds().min(), V3d( -1.0, -1.0, -1.0 ))
        # self.assertEqual(baseSamp.getSelfBounds().max(), V3d(  1.0,  1.0,  1.0 ))

        arbattrs = mesh.getArbGeomParams()
        self.assertFalse(arbattrs)

        nsp = N.getExpandedValue().getVals()

        # self.assertTrue(N.isConstant())
        # self.assertTrue(uv.isConstant())

        # for i in range( len( nsp ) ):
        #     self.assertEqual(nsp[i], normals[i])

        uvsamp = uv.getIndexedValue()

        self.assertEqual(uvsamp.getIndices()[1], 1)
        # uv2 = uvsamp.getVals()[2]

        # self.assertEqual(uv2, V2f( 1.0, 1.0 ))

        positions = meshSamp.getPositions()
        # for i in range( len( positions ) ):
        #     self.assertEqual(positions[i], verts[i])

    def testMeshLayerExport(self):
        """write a boring oarchive with a mesh and an oarchive with just uvs"""

        # the boring one
        meshyObj = OPolyMesh( OArchive( 'polyMeshLayerA.abc' ).getTop() , 'meshy' )
        mesh = meshyObj.getSchema()

        # mesh_samp = OPolyMeshSchemaSample( verts, indices, counts )
        indicesSample = Int32ArraySample(Int32Vector(indices.tolist()))
        countsSample = Int32ArraySample(Int32Vector(counts.tolist()))
        mesh_samp = OPolyMeshSchemaSample( P3fArraySample(), indicesSample, countsSample )
        mesh_samp.setPositions(verts)
        mesh.set( mesh_samp )

        # now just the UVs
        rootB = OArchive( 'polyMeshLayerB.abc' ).getTop()
        meshyObjB = OPolyMesh( rootB, 'meshy', SparseFlag.kSparse )
        mesh = meshyObjB.getSchema()

        mesh_samp = OPolyMeshSchemaSample()
        # uvsamp = OV2fGeomParamSample( uvs, kFacevaryingScope )
        mesh_samp.setUVs( uvs, kFacevaryingScope )
        mesh.set( mesh_samp )

    def testMeshLayerImport(self):
        """read the mesh layering the uvs on top"""

        layers = ['polyMeshLayerA.abc', 'polyMeshLayerB.abc']
        meshyObj = IPolyMesh( IArchive( layers ).getTop(), 'meshy' )
        mesh = meshyObj.getSchema()

        uv = mesh.getUVsParam()
        self.assertTrue(uv.valid())

        uvsamp = uv.getIndexedValue()

        self.assertEqual(uvsamp.getIndices()[1], 1)

        meshSamp = mesh.getValue()

        positions = meshSamp.getPositions()
        # for i in range(  positions.size() ):
        #     self.assertEqual(positions[i], verts[i])
