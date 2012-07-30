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
from alembic.AbcGeom import *

testList = []

def equalWithAbsError (a0, a1, error):
    return abs (a0 - a1) <= error

def pointsOut():
    """write out points archive"""
    
    archive = OArchive("particlesOut2.abc")
    topObj = archive.getTop()
    ptsObj = OPoints(topObj, "somePoints")

    positions = V3fArray(100)
    velocities = V3fArray(100)
    ids = IntArray(100)
    widths = FloatArray(100)

    for i in range(100):
        widthSamp = OFloatGeomParamSample()
        widthSamp.setScope(GeometryScope.kVertexScope)
        widthSamp.setVals(widths)

        psamp = OPointsSchemaSample()
        psamp.setPositions(positions)
        psamp.setIds(ids)
        psamp.setVelocities(velocities)
        psamp.setWidths(widthSamp)
        ptsObj.getSchema().set(psamp)

        positions[i] = V3f(i, i, i)
        velocities[i] = V3f(100.0-i, 0, 0)
        ids[i] = i * 10
        widths[i] = 0.1 + i * 0.05

def pointsIn():
    """read in points archive"""

    archive = IArchive("particlesOut2.abc")
    topObj = archive.getTop()
    points = IPoints(topObj, "somePoints")
    pointsSchema = points.getSchema()

    assert pointsSchema.valid()

    widthProp = pointsSchema.getWidthsParam()

    assert widthProp.getScope() == GeometryScope.kVertexScope

    for i in range(100):
        pointsSamp = pointsSchema.getValue(ISampleSelector(i))
        widthSamp = widthProp.getExpandedValue(ISampleSelector(i))

        for j in range(i):
            assert pointsSamp.getPositions()[j] == V3f(j, j, j)
            assert pointsSamp.getVelocities()[j] == V3f(100-j, 0, 0)
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

