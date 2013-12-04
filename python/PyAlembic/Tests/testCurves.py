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
from alembic.AbcCoreAbstract import *
from alembic.Abc import *
from alembic.AbcGeom import *
from curvesData import *

testList = []

kVertexScope = GeometryScope.kVertexScope
kCubic = CurveType.kCubic
kNonPeriodic = CurvePeriodicity.kNonPeriodic

def doSample( iCurves ):
    
    curves = iCurves.getSchema()

    widthSamp = OFloatGeomParamSample( widths, kVertexScope )
    uvSamp = OV2fGeomParamSample( uvs, kVertexScope )

    curvesSamp = OCurvesSchemaSample( verts, numVerts, kCubic, kNonPeriodic,
                                       widthSamp, uvSamp )

    knots = curvesSamp.getKnots()
    assert len(knots) == 0

    newKnots = FloatArray(4)
    for ii in range(4):
        newKnots[ii] = ii
    curvesSamp.setKnots(newKnots)

    knots = curvesSamp.getKnots()
    for ii in range(4):
        assert knots[ii] == ii

    orders = curvesSamp.getOrders()
    assert len(orders) == 0

    newOrder = UnsignedCharArray(3)
    for ii in range(3):
        newOrder[ii] = ii
    curvesSamp.setOrders(newOrder)

    orders = curvesSamp.getOrders()
    for ii in range(3):
        assert newOrder[ii] == ii

    curves.set( curvesSamp )

def curvesOut():
    """write an oarchive with a curve in it"""

    myCurves = OCurves( OArchive( 'curves1.abc' ).getTop(),
                        'really_long_curves_name' )

    for i in range(0,5):
        doSample( myCurves )

def curvesIn():
    """read an iarchive with a curve in it"""

    myCurves = ICurves( IArchive( 'curves1.abc' ).getTop(),
                        'really_long_curves_name' )
    curves = myCurves.getSchema()

    curvesSamp = curves.getValue()

    assert curvesSamp.getSelfBounds().min() == V3d( -1.0, -1.0, -1.0 )
    assert curvesSamp.getSelfBounds().max() == V3d(  1.0,  1.0,  1.0 )

    knots = curvesSamp.getKnots()
    for ii in range(4):
        assert knots[ii] == ii

    orders = curvesSamp.getOrders()
    for ii in range(3):
        assert orders[ii] == ii

    positions = curvesSamp.getPositions()

    assert len( positions ) == 12

    for i in range( len( positions ) ):
        assert positions[i] == verts[i]

    widthSamp = curves.getWidthsParam().getExpandedValue()

    assert len( widthSamp.getVals() ) == 12
    assert IFloatGeomParam.matches( curves.getWidthsParam().getHeader() )
    assert widthSamp.getScope() == kVertexScope

def testCurvesBinding():
    curvesOut()
    curvesIn()

testList.append( ( 'testCurvesBinding', testCurvesBinding ) )

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""
