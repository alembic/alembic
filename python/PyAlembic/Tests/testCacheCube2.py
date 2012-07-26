import sys, imath
from alembic.Abc import *
from alembic.AbcA import *
from alembic.AbcG import *
from alembic.Util import *

from cubeData import *

# In this test, we export a polygon cube with xform data
# The cube is animated by manipulating the xform transforms
# over time.

# This is the scatch of the file hierarchy.
# Abc
#   |- Xform
#       |- .inherits
#       |- .ops
#       |- .vals
#       |- PolyMesh
#           |- .faceCounts
#           |- .faceIndices
#           |- P 
#           |- .selfBnds

# TimeSampling data
tvec = TimeVector()
tvec[:] = [ 1, 2, 3 ]

timePerCycle = 3.0
numSamplesPerCycle = len(tvec)

tst = TimeSamplingType( numSamplesPerCycle, timePerCycle )
ts = TimeSampling( tst, tvec )

# Alembic file name
fileName = 'cube2.abc'

transop = XformOp(XformOperationType.kTranslateOperation)
rotatop = XformOp(XformOperationType.kRotateOperation)
scaleop = XformOp(XformOperationType.kScaleOperation)
matrixop = XformOp(XformOperationType.kMatrixOperation)

testList = []

# Test exporting a simple cube quad mesh
def testExportCubeGeom():

    # create the archive
    top = OArchive( fileName ).getTop()
    tsidx = top.getArchive().addTimeSampling(ts)

    # create the top xform
    xform = OXform(top, 'cube1', tsidx)
    xsamp = XformSample()
    xform.getSchema().set(xsamp)

    # the mesh shape
    meshObj = OPolyMesh(xform, 'cube1Shape')
    mesh = meshObj.getSchema()
    mesh_samp = OPolyMeshSchemaSample(points, faceIndices, faceCounts)
    mesh_samp.setSelfBounds(selfBnds)
    mesh.set(mesh_samp)

testList.append(('testExportCubeGeom2', testExportCubeGeom))

# Test importing the exported quad mesh
def testImportCubeGeom():

    top = IArchive(fileName).getTop()
    assert top.getNumChildren() == 1

    # xform
    xform = IXform(top, 'cube1')
    assert xform.getName() == 'cube1'
    assert xform.getSchema().getNumSamples() == 1

    # polymesh
    meshObj = IPolyMesh(xform, 'cube1Shape')
    mesh = meshObj.getSchema()
    assert mesh.getNumSamples() == 1

    mesh_samp = mesh.getValue(ISampleSelector(0))
    assert mesh_samp.getPositions() == points
    assert mesh_samp.getFaceCounts() == faceCounts
    assert mesh_samp.getFaceIndices() == faceIndices
    assert mesh_samp.getSelfBounds() == selfBnds

testList.append(('testImportCubeGeom2', testImportCubeGeom))

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""

