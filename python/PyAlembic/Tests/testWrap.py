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

from imath import *
from alembic.Abc import *
from alembic.AbcCoreAbstract import *
from alembic.AbcGeom import *
from meshData import *

testList = []

kFacevaryingScope = GeometryScope.kFacevaryingScope
kWrapExisting = WrapExistingFlag.kWrapExisting

def testOut():
    """write an oarchive with some data in it"""
    
    meshyObj = OPolyMesh( OArchive( 'wrapTest1.abc' ).getTop() , 'meshy' )
    mesh = meshyObj.getSchema()
    
    uvsamp = OV2fGeomParamSample( uvs, kFacevaryingScope ) 
    nsamp  = ON3fGeomParamSample( normals, kFacevaryingScope )
    mesh_samp = OPolyMeshSchemaSample( verts, indices, counts, uvsamp, nsamp )
    
    cbox = Box3d()
    cbox.extendBy( V3d( 1.0, -1.0, 0.0 ) )
    cbox.extendBy( V3d( -1.0, 1.0, 3.0 ) )
    
    mesh.getChildBoundsProperty().setValue( cbox )
    mesh.set( mesh_samp )
    mesh.set( mesh_samp )

def testIn():
    """read in an archive and try wrapping it as various objects"""
    
    arch = IArchive('wrapTest1.abc')
    obj = IObject(arch.getTop(), "meshy")
    md = obj.getMetaData()

    assert IPolyMesh.matches(md)
    assert not ISubD.matches(md)
    
    # wrap object as PolyMesh
    mesh = IPolyMesh(obj, kWrapExisting)
    assert mesh.valid()

    # wrap object as SubD
    subd = ISubD(obj, kWrapExisting)
    assert subd.valid()

    # this should throw an exception
    try:
        cam = ICamera(obj, kWrapExisting)
    except RuntimeError, e:
        pass

    # this should also throw an exception
    try:
        mesh = IPolyMesh(arch.getTop(), kWrapExisting)
    except RuntimeError, e:
        pass
    
def testWrapBinding():
    testOut()
    testIn()

testList.append( ( 'testWrapBinding', testWrapBinding ) )

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""

