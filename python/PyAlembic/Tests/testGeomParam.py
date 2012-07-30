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

