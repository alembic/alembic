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

