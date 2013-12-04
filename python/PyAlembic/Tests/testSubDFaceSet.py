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
from alembic.AbcCoreAbstract import MetaData
from alembic.AbcGeom import *
from meshData import *

testList = []

kFaceSetExclusive = FaceSetExclusivity.kFaceSetExclusive

def subDFaceSetOut():
    """Tests OSubD and OFaceSet"""

    archive = OArchive("facesetSubD1.abc")

    meshyObj = OSubD(archive.getTop(), "subd")
    mesh = meshyObj.getSchema()

    mesh.setUVSourceName("Chewbacca")

    mesh_samp = OSubDSchemaSample( verts, indices, counts )

    creases = IntArray(24)
    corners = IntArray(24)
    creaseLengths = IntArray(24)
    creaseSharpnesses = FloatArray(24)
    cornerSharpnesses = FloatArray(24)

    for i in range(24):
        creases[i] = indices[i]
        corners[i] = indices[i]
        cornerSharpnesses[i] = 1.0e38

    for i in range(6):
        creaseLengths[i] = 4
        creaseSharpnesses[i] = 1.0e38

    mesh_samp.setCreases(creases, creaseLengths, creaseSharpnesses)
    mesh_samp.setCorners(corners, cornerSharpnesses)

    # UVs
    uvsamp = OV2fGeomParamSample(uvs,
            GeometryScope.kFacevaryingScope)
    mesh_samp.setUVs(uvsamp)

    # setIsUV / isUV / getSourceName / setSourceName metadata test.
    meta = MetaData()
    meta.setIsUV(True)

    meta.setSourceName("Illum")
    assert meta.getSourceName() == "Illum"

    arb = mesh.getArbGeomParams()

    uv2 = None

    if True:
        # scoped so it gets added before we check it
        uv2 = OV2fGeomParam( arb, "uv2", True, GeometryScope.kFacevaryingScope, 1, meta )

    assert uv2
    header = arb.getPropertyHeader("uv2")
    assert header.isUV()

    # set the sample
    mesh.set(mesh_samp)

    # change one of the schema's parameters
    mesh_samp.setInterpolateBoundary(1)
    mesh.set(mesh_samp)

    # test that the integer property doesn't latch to non-zero
    mesh_samp.setInterpolateBoundary(0)
    mesh.set(mesh_samp)

    # faceset testing
    faceSetNames = []
    my_face_set_obj = mesh.createFaceSet("testing_faceset")
    faceSetNames = mesh.getFaceSetNames()
    assert len(faceSetNames) == 1
    my_face_set = my_face_set_obj.getSchema()
    #print "created faceset called", my_face_set_obj.getName()

    # our FaceSet is composed of faces 1-3
    face_nums = IntArray(3)
    face_nums[0] = 1
    face_nums[1] = 2
    face_nums[2] = 3
    my_face_set_samp = OFaceSetSchemaSample(face_nums)

    # faceset is visible, doesn't change
    my_face_set.set(my_face_set_samp)
    my_face_set.setFaceExclusivity(kFaceSetExclusive)

    # Test that we've computed selfBounds correctly
    face_set_bounds = my_face_set_samp.getSelfBounds()

    parentOfFaceSet = my_face_set_obj.getParent()
    grandParent = parentOfFaceSet.getParent()

def subDFaceSetIn():
    """tests ISubD and IFaceSet"""

    archive = IArchive("facesetSubD1.abc")
    meshyObj = ISubD(archive.getTop(), "subd")
    mesh = meshyObj.getSchema()

    assert mesh.getNumSamples() == 3

    # faceset testing
    assert mesh.hasFaceSet("testing_faceset")

    faceSetNames = mesh.getFaceSetNames()
    assert len(faceSetNames) == 1

    #for name in faceSetNames:
    #    print "meshyObj has faceSet", name
    assert faceSetNames[0] == "testing_faceset"
    faceSetObj = mesh.getFaceSet("testing_faceset")

    faceSet = faceSetObj.getSchema()
    assert faceSet.getFaceExclusivity() == kFaceSetExclusive

    faceSetSamp0 = faceSet.getValue(ISampleSelector(0))
    faces = faceSetSamp0.getFaces()
    assert faces[0] == 1 and faces[1] == 2 and faces[2] == 3

    # UVs
    uv = mesh.getUVsParam()
    assert uv.getMetaData().getSourceName() == "Chewbacca"
    assert not uv.isIndexed()

    arb = mesh.getArbGeomParams()
    header = arb.getPropertyHeader("uv2")
    assert header.isUV()

    meta = header.getMetaData()
    assert meta.getSourceName() == "Illum"

    # we can fake like the UVs are indexed
    uvsamp = uv.getIndexedValue()
    assert uvsamp.getIndices()[1] == 1
    uv2 = uvsamp.getVals()[2]
    assert uv2 == V2f(1.0, 1.0)

    samp1 = mesh.getValue(ISampleSelector(1))
    assert samp1.getSelfBounds().min() == V3d(-1.0, -1.0, -1.0)
    assert samp1.getSelfBounds().max() == V3d(1.0, 1.0, 1.0 )
    assert samp1.getInterpolateBoundary() == 1

    samp2 = mesh.getValue(ISampleSelector(2))
    assert samp2.getSelfBounds().min() == V3d(-1.0, -1.0, -1.0)
    assert samp2.getSelfBounds().max() == V3d(1.0, 1.0, 1.0)
    assert samp2.getInterpolateBoundary() == 0

def testSubDFaceSetBinding():
    subDFaceSetOut()
    subDFaceSetIn()

testList.append(('testSubDFaceSetBinding', testSubDFaceSetBinding))

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""
