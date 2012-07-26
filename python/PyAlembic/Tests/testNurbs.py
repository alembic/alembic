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
from nurbsData import *

testList = []

def nurbsOut1():
    """write an oarchive with a nurb in it"""

    myNurbs = ONuPatch( OArchive( 'nurbs1.abc' ).getTop(),
                        'nurbs_surface' )

    myNurbsSchema = myNurbs.getSchema()

    nurbsSamp = ONuPatchSchemaSample ( P, nu, nv, uOrder, vOrder, 
                                       uKnot, vKnot )

    nurbsSamp.setTrimCurve( trim_nLoops, trim_nCurves, trim_n, trim_order,
                            trim_knot, trim_min, trim_max, trim_u, trim_v,
                            trim_w )

    myNurbsSchema.set( nurbsSamp )

def nurbsOut2():
    """same as example 1 but without the trim curves"""

    myNurbs = ONuPatch( OArchive( 'nurbs2.abc' ).getTop(),
                        'nurbs_surface_noTrim' )

    myNurbsSchema = myNurbs.getSchema()

    nurbsSamp = ONuPatchSchemaSample ( P, nu, nv, uOrder, vOrder, 
                                       uKnot, vKnot )

    myNurbsSchema.set( nurbsSamp )

def nurbsOut3():
    """same as example 1 but without the trim curves, with position weights"""

    myNurbs = ONuPatch( OArchive( 'nurbs3.abc' ).getTop(),
                        'nurbs_surface_withW' )

    myNurbsSchema = myNurbs.getSchema()

    nurbsSamp = ONuPatchSchemaSample ( P, nu, nv, uOrder, vOrder, 
                                       uKnot, vKnot )

    nurbsSamp.setPositionWeights( Pw )

    myNurbsSchema.set( nurbsSamp )

def nurbsIn1():
    """read an iarchive with a nurb in it"""

    myNurbs = INuPatch( IArchive( 'nurbs1.abc' ).getTop(),
                        'nurbs_surface' )
    nurbsSchema = myNurbs.getSchema()

    nurbsSamp = nurbsSchema.getValue()

    assert nurbsSamp.getSelfBounds().min() == V3d( 0.0, 0.0, -3.0 )
    assert nurbsSamp.getSelfBounds().max() == V3d( 3.0, 3.0,  3.0 )

    assert nurbsSamp.getTrimNumLoops() == 1
    assert len( nurbsSamp.getTrimOrders() ) == 1
    assert nurbsSamp.hasTrimCurve() == True
    assert nurbsSchema.isConstant() == True

def nurbsIn2():
    """same as example 1 but without the trim curves"""

    myNurbs = INuPatch( IArchive( 'nurbs2.abc' ).getTop(),
                        'nurbs_surface_noTrim' )
    nurbsSchema = myNurbs.getSchema()

    nurbsSamp = nurbsSchema.getValue()

    assert nurbsSamp.getSelfBounds().min() == V3d( 0.0, 0.0, -3.0 )
    assert nurbsSamp.getSelfBounds().max() == V3d( 3.0, 3.0,  3.0 )

    assert nurbsSamp.getTrimNumLoops() == 0
    assert nurbsSamp.hasTrimCurve() == False
    assert not nurbsSamp.getPositionWeights()
    assert nurbsSchema.isConstant() == True

def nurbsIn3():
    """same as example 1 but without the trim curves, with position weights"""

    myNurbs = INuPatch( IArchive( 'nurbs3.abc' ).getTop(),
                        'nurbs_surface_withW' )
    nurbsSchema = myNurbs.getSchema()

    nurbsSamp = nurbsSchema.getValue()

    assert nurbsSamp.getSelfBounds().min() == V3d( 0.0, 0.0, -3.0 )
    assert nurbsSamp.getSelfBounds().max() == V3d( 3.0, 3.0,  3.0 )

    assert nurbsSamp.getTrimNumLoops() == 0
    assert nurbsSamp.hasTrimCurve() == False
    assert len( nurbsSamp.getPositionWeights() ) == len( P )
    assert nurbsSchema.isConstant() == True

def testNurbsBinding():
    nurbsOut1()
    nurbsOut2()
    nurbsOut3()
    nurbsIn1()
    nurbsIn2()
    nurbsIn3()

testList.append( ( 'testNurbsBinding', testNurbsBinding ) )

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""

