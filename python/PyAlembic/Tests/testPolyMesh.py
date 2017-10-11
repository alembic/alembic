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

from imath import *
from alembic.Abc import *
from alembic.AbcCoreAbstract import *
from alembic.AbcGeom import *
from meshData import *

testList = []

kFacevaryingScope = GeometryScope.kFacevaryingScope

def meshOut():
    """write an oarchive with a mesh in it"""

    meshyObj = OPolyMesh( OArchive( 'polyMesh1.abc' ).getTop() , 'meshy' )
    mesh = meshyObj.getSchema()

    uvsamp = OV2fGeomParamSample( uvs, kFacevaryingScope )
    nsamp  = ON3fGeomParamSample( normals, kFacevaryingScope )
    mesh_samp = OPolyMeshSchemaSample( verts, indices, counts, uvsamp, nsamp )

    mesh.setUVSourceName("NienNunb")

    cbox = Box3d()
    cbox.extendBy( V3d( 1.0, -1.0, 0.0 ) )
    cbox.extendBy( V3d( -1.0, 1.0, 3.0 ) )
    mesh.getChildBoundsProperty().setValue( cbox )
    mesh.getChildBoundsProperty().setValue( cbox )

    mesh.set( mesh_samp )
    mesh.set( mesh_samp )

def meshIn():
    """read an iarchive with a mesh in it"""

    geomBase = IGeomBaseObject( IArchive( 'polyMesh1.abc' ).getTop(), 'meshy' )
    assert geomBase.getSchema().getSelfBoundsProperty().valid()

    meshyObj = IPolyMesh( IArchive( 'polyMesh1.abc' ).getTop(), 'meshy' )
    mesh = meshyObj.getSchema()
    N = mesh.getNormalsParam()
    uv = mesh.getUVsParam()

    assert uv.getMetaData().getSourceName() == "NienNunb"
    assert not N.isIndexed()
    assert not uv.isIndexed()

    meshSamp = mesh.getValue()
    baseSamp = geomBase.getSchema().getValue()

    assert meshSamp.getSelfBounds().min() == V3d( -1.0, -1.0, -1.0 )
    assert meshSamp.getSelfBounds().max() == V3d(  1.0,  1.0,  1.0 )
    assert baseSamp.getSelfBounds().min() == V3d( -1.0, -1.0, -1.0 )
    assert baseSamp.getSelfBounds().max() == V3d(  1.0,  1.0,  1.0 )

    arbattrs = mesh.getArbGeomParams()
    assert not arbattrs

    nsp = N.getExpandedValue().getVals()

    assert N.isConstant()
    assert uv.isConstant()

    for i in range( len( nsp ) ):
        assert nsp[i] == normals[i]

    uvsamp = uv.getIndexedValue()

    assert uvsamp.getIndices()[1] == 1
    uv2 = uvsamp.getVals()[2]

    assert uv2 == V2f( 1.0, 1.0 )

    positions = meshSamp.getPositions()
    for i in range( len( positions ) ):
        assert positions[i] == verts[i]

def meshLayerOut():
    """write a boring oarchive with a mesh and an oarchive with just uvs"""

    # the boring one
    meshyObj = OPolyMesh( OArchive( 'polyMeshLayerA.abc' ).getTop() , 'meshy' )
    mesh = meshyObj.getSchema()

    mesh_samp = OPolyMeshSchemaSample( verts, indices, counts )
    mesh.set( mesh_samp )

    # now just the UVs
    rootB = OArchive( 'polyMeshLayerB.abc' ).getTop()
    meshyObjB = OPolyMesh( rootB, 'meshy', SparseFlag.kSparse )
    mesh = meshyObjB.getSchema()

    mesh_samp = OPolyMeshSchemaSample()
    uvsamp = OV2fGeomParamSample( uvs, kFacevaryingScope )
    mesh_samp.setUVs( uvsamp )
    mesh.set( mesh_samp )

def meshLayerIn():
    """read the mesh layering the uvs on top"""

    layers = ['polyMeshLayerA.abc', 'polyMeshLayerB.abc']
    meshyObj = IPolyMesh( IArchive( layers ).getTop(), 'meshy' )
    mesh = meshyObj.getSchema()

    uv = mesh.getUVsParam()
    assert uv.valid()

    uvsamp = uv.getIndexedValue()

    assert uvsamp.getIndices()[1] == 1

    meshSamp = mesh.getValue()

    positions = meshSamp.getPositions()
    for i in range( len( positions ) ):
        assert positions[i] == verts[i]

def testPolyMeshBinding():
    meshOut()
    meshIn()

testList.append( ( 'testPolyMeshBinding', testPolyMeshBinding ) )

def testPolyMeshLayering():
    meshLayerOut()
    meshLayerIn()

testList.append( ( 'testPolyMeshLayering', testPolyMeshLayering ) )

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""
