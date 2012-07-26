#!/usr/bin/env python2_zeno

#   Copyright (c) 2012 Industrial   Light   and   Magic.
#   All   rights   reserved.    Used   under   authorization.
#   This material contains the confidential  and  proprietary
#   information   of   Industrial   Light   and   Magic   and
#   may not be copied in whole or in part without the express
#   written   permission   of  Industrial Light  and   Magic.
#   This  copyright  notice  does  not   imply   publication.

from buildTestData import *

testList = []

def testOGeomParamImpl( iTPTraits, iTypedGeomParamSample ):

    compare = TraitsComparison[iTPTraits.__name__]

    ovals = ArrayTraitsData[iTPTraits.__name__][1]
    samp  = iTypedGeomParamSample( ovals, GeometryScope.kConstantScope )
    ivals = samp.getVals()

    assert len( ivals ) == len( ovals )
    for i in range( len( ivals ) ):
        assert compare( ivals[i], ovals[i] )

    samp2 = iTypedGeomParamSample()
    samp2.setVals( ovals )
    ivals = samp2.getVals()

    assert len( ivals ) == len( ovals )
    for i in range( len( ivals ) ):
        assert compare( ivals[i], ovals[i] )

def testOGeomParamSample():
    testOGeomParamImpl( BooleanTPTraits, OBooleanGeomParamSample )
    testOGeomParamImpl( Uint8TPTraits  , OUcharGeomParamSample )
    testOGeomParamImpl( Int8TPTraits   , OCharGeomParamSample )
    testOGeomParamImpl( Uint16TPTraits , OUInt16GeomParamSample )
    testOGeomParamImpl( Int16TPTraits  , OInt16GeomParamSample )
    testOGeomParamImpl( Uint32TPTraits , OUInt32GeomParamSample )
    testOGeomParamImpl( Int32TPTraits  , OInt32GeomParamSample )
    testOGeomParamImpl( Uint64TPTraits , OUInt64GeomParamSample )
    testOGeomParamImpl( Int64TPTraits  , OInt64GeomParamSample )
    testOGeomParamImpl( Float16TPTraits, OHalfGeomParamSample )
    testOGeomParamImpl( Float32TPTraits, OFloatGeomParamSample )
    testOGeomParamImpl( Float64TPTraits, ODoubleGeomParamSample )
    testOGeomParamImpl( StringTPTraits , OStringGeomParamSample )
    testOGeomParamImpl( WstringTPTraits, OWstringGeomParamSample )

    testOGeomParamImpl( V2sTPTraits    , OV2sGeomParamSample )
    testOGeomParamImpl( V2iTPTraits    , OV2iGeomParamSample )
    testOGeomParamImpl( V2fTPTraits    , OV2fGeomParamSample )
    testOGeomParamImpl( V2dTPTraits    , OV2dGeomParamSample )

    testOGeomParamImpl( V3sTPTraits    , OV3sGeomParamSample )
    testOGeomParamImpl( V3iTPTraits    , OV3iGeomParamSample )
    testOGeomParamImpl( V3fTPTraits    , OV3fGeomParamSample )
    testOGeomParamImpl( V3dTPTraits    , OV3dGeomParamSample )

    testOGeomParamImpl( P2sTPTraits    , OP2sGeomParamSample )
    testOGeomParamImpl( P2iTPTraits    , OP2iGeomParamSample )
    testOGeomParamImpl( P2fTPTraits    , OP2fGeomParamSample )
    testOGeomParamImpl( P2dTPTraits    , OP2dGeomParamSample )

    testOGeomParamImpl( P3sTPTraits    , OP3sGeomParamSample )
    testOGeomParamImpl( P3iTPTraits    , OP3iGeomParamSample )
    testOGeomParamImpl( P3fTPTraits    , OP3fGeomParamSample )
    testOGeomParamImpl( P3dTPTraits    , OP3dGeomParamSample )

    testOGeomParamImpl( Box2sTPTraits  , OBox2sGeomParamSample )
    testOGeomParamImpl( Box2iTPTraits  , OBox2iGeomParamSample )
    testOGeomParamImpl( Box2fTPTraits  , OBox2fGeomParamSample )
    testOGeomParamImpl( Box2dTPTraits  , OBox2dGeomParamSample )

    testOGeomParamImpl( Box3sTPTraits  , OBox3sGeomParamSample )
    testOGeomParamImpl( Box3iTPTraits  , OBox3iGeomParamSample )
    testOGeomParamImpl( Box3fTPTraits  , OBox3fGeomParamSample )
    testOGeomParamImpl( Box3dTPTraits  , OBox3dGeomParamSample )

    testOGeomParamImpl( M33fTPTraits   , OM33fGeomParamSample )
    testOGeomParamImpl( M33dTPTraits   , OM33dGeomParamSample )
    testOGeomParamImpl( M44fTPTraits   , OM44fGeomParamSample )
    testOGeomParamImpl( M44dTPTraits   , OM44dGeomParamSample )

    testOGeomParamImpl( QuatfTPTraits  , OQuatfGeomParamSample )
    testOGeomParamImpl( QuatdTPTraits  , OQuatdGeomParamSample )

    testOGeomParamImpl( C3hTPTraits    , OC3hGeomParamSample )
    testOGeomParamImpl( C3fTPTraits    , OC3fGeomParamSample )
    testOGeomParamImpl( C3cTPTraits    , OC3cGeomParamSample )

    testOGeomParamImpl( C4hTPTraits    , OC4hGeomParamSample )
    testOGeomParamImpl( C4fTPTraits    , OC4fGeomParamSample )
    testOGeomParamImpl( C4cTPTraits    , OC4cGeomParamSample )

    testOGeomParamImpl( N2fTPTraits    , ON2fGeomParamSample )
    testOGeomParamImpl( N2dTPTraits    , ON2dGeomParamSample )

    testOGeomParamImpl( N3fTPTraits    , ON3fGeomParamSample )
    testOGeomParamImpl( N3dTPTraits    , ON3dGeomParamSample )

testList.append( ( 'testOGeomParamSample', testOGeomParamSample ) )

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""

