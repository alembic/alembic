#!/usr/bin/env python2_zeno

#   Copyright (c) 2011-2012 Industrial   Light   and   Magic.
#   All   rights   reserved.    Used   under   authorization.
#   This material contains the confidential  and  proprietary
#   information   of   Industrial   Light   and   Magic   and
#   may not be copied in whole or in part without the express
#   written   permission   of  Industrial Light  and   Magic.
#   This  copyright  notice  does  not   imply   publication.

import imath
import alembic
import traceback

testList = []

def equalWithAbsError (a0, a1, error):
    return abs (a0 - a1) <= error

def pointsOut():
    """write out points archive"""
    
    archive = alembic.Abc.OArchive("particlesOut2.abc")
    topObj = archive.getTop()
    ptsObj = alembic.AbcG.OPoints(topObj, "somePoints")

    positions = imath.V3fArray(100)
    velocities = imath.V3fArray(100)
    ids = imath.IntArray(100)
    widths = imath.FloatArray(100)

    for i in range(100):
        widthSamp = alembic.AbcG.OFloatGeomParamSample()
        widthSamp.setScope(alembic.AbcG.GeometryScope.kVertexScope)
        widthSamp.setVals(widths)

        psamp = alembic.AbcG.OPointsSchemaSample()
        psamp.setPositions(positions)
        psamp.setIds(ids)
        psamp.setVelocities(velocities)
        psamp.setWidths(widthSamp)
        ptsObj.getSchema().set(psamp)

        positions[i] = imath.V3f(i, i, i)
        velocities[i] = imath.V3f(100.0-i, 0, 0)
        ids[i] = i * 10
        widths[i] = 0.1 + i * 0.05

def pointsIn():
    """read in points archive"""

    archive = alembic.Abc.IArchive("particlesOut2.abc")
    topObj = archive.getTop()
    points = alembic.AbcG.IPoints(topObj, "somePoints")
    pointsSchema = points.getSchema()

    assert pointsSchema.valid()

    widthProp = pointsSchema.getWidthsParam()

    assert widthProp.getScope() == alembic.AbcG.GeometryScope.kVertexScope

    for i in range(100):
        pointsSamp = pointsSchema.getValue(alembic.Abc.ISampleSelector(i))
        widthSamp = widthProp.getExpandedValue(alembic.Abc.ISampleSelector(i))

        for j in range(i):
            assert pointsSamp.getPositions()[j] == imath.V3f(j, j, j)
            assert pointsSamp.getVelocities()[j] == imath.V3f(100-j, 0, 0)
            assert pointsSamp.getIds()[j] == j * 10
            assert equalWithAbsError(widthSamp.getVals()[j], 0.1 + j * 0.05, 7)

def testPointsBinding():
    pointsOut()
    pointsIn()

testList.append(('testPointsBinding', testPointsBinding))

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""

