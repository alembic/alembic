#!/usr/bin/env python2_zeno

#   Copyright (c) 2012 Industrial   Light   and   Magic.
#   All   rights   reserved.    Used   under   authorization.
#   This material contains the confidential  and  proprietary
#   information   of   Industrial   Light   and   Magic   and
#   may not be copied in whole or in part without the express
#   written   permission   of  Industrial Light  and   Magic.
#   This  copyright  notice  does  not   imply   publication.

from imath import *
from alembic.Abc import *
from alembic.AbcA import *
from alembic.AbcG import *
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

    cbox = Box3d()
    cbox.extendBy( V3d( 1.0, -1.0, 0.0 ) )
    cbox.extendBy( V3d( -1.0, 1.0, 3.0 ) )
    mesh_samp.setChildBounds( cbox )

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

def testPolyMeshBinding():
    meshOut()
    meshIn()

testList.append( ( 'testPolyMeshBinding', testPolyMeshBinding ) )

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""

