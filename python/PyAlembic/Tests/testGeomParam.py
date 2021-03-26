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
import unittest
from buildTestData import *

class GeomParamTest(unittest.TestCase):
    def geomParamImpl(self,  iTPTraits, iTypedGeomParamSample ):

        compare = TraitsComparison[iTPTraits.__name__]

        ovals = ArrayTraitsData[iTPTraits.__name__][1]
        samp  = iTypedGeomParamSample( ovals, GeometryScope.kConstantScope )
        ivals = samp.getVals()

        self.assertEqual(len( ivals ), len( ovals ))
        for i in range( len( ivals ) ):
            self.assertTrue( compare( ivals[i], ovals[i] ) )

        samp2 = iTypedGeomParamSample()
        samp2.setVals( ovals )
        ivals = samp2.getVals()

        self.assertEqual(len( ivals ), len( ovals ))

        for i in range( len( ivals ) ):
            self.assertTrue( compare( ivals[i], ovals[i] ) )

    def testOGeomParamSample(self):
        self.geomParamImpl( BooleanTPTraits, OBooleanGeomParamSample )
        self.geomParamImpl( Uint8TPTraits  , OUcharGeomParamSample )
        self.geomParamImpl( Int8TPTraits   , OCharGeomParamSample )
        self.geomParamImpl( Uint16TPTraits , OUInt16GeomParamSample )
        self.geomParamImpl( Int16TPTraits  , OInt16GeomParamSample )
        self.geomParamImpl( Uint32TPTraits , OUInt32GeomParamSample )
        self.geomParamImpl( Int32TPTraits  , OInt32GeomParamSample )
        self.geomParamImpl( Uint64TPTraits , OUInt64GeomParamSample )
        self.geomParamImpl( Int64TPTraits  , OInt64GeomParamSample )
        self.geomParamImpl( Float16TPTraits, OHalfGeomParamSample )
        self.geomParamImpl( Float32TPTraits, OFloatGeomParamSample )
        self.geomParamImpl( Float64TPTraits, ODoubleGeomParamSample )
        self.geomParamImpl( StringTPTraits , OStringGeomParamSample )
        self.geomParamImpl( WstringTPTraits, OWstringGeomParamSample )

        self.geomParamImpl( V2sTPTraits    , OV2sGeomParamSample )
        self.geomParamImpl( V2iTPTraits    , OV2iGeomParamSample )
        self.geomParamImpl( V2fTPTraits    , OV2fGeomParamSample )
        self.geomParamImpl( V2dTPTraits    , OV2dGeomParamSample )

        self.geomParamImpl( V3sTPTraits    , OV3sGeomParamSample )
        self.geomParamImpl( V3iTPTraits    , OV3iGeomParamSample )
        self.geomParamImpl( V3fTPTraits    , OV3fGeomParamSample )
        self.geomParamImpl( V3dTPTraits    , OV3dGeomParamSample )

        self.geomParamImpl( P2sTPTraits    , OP2sGeomParamSample )
        self.geomParamImpl( P2iTPTraits    , OP2iGeomParamSample )
        self.geomParamImpl( P2fTPTraits    , OP2fGeomParamSample )
        self.geomParamImpl( P2dTPTraits    , OP2dGeomParamSample )

        self.geomParamImpl( P3sTPTraits    , OP3sGeomParamSample )
        self.geomParamImpl( P3iTPTraits    , OP3iGeomParamSample )
        self.geomParamImpl( P3fTPTraits    , OP3fGeomParamSample )
        self.geomParamImpl( P3dTPTraits    , OP3dGeomParamSample )

        self.geomParamImpl( Box2sTPTraits  , OBox2sGeomParamSample )
        self.geomParamImpl( Box2iTPTraits  , OBox2iGeomParamSample )
        self.geomParamImpl( Box2fTPTraits  , OBox2fGeomParamSample )
        self.geomParamImpl( Box2dTPTraits  , OBox2dGeomParamSample )

        self.geomParamImpl( Box3sTPTraits  , OBox3sGeomParamSample )
        self.geomParamImpl( Box3iTPTraits  , OBox3iGeomParamSample )
        self.geomParamImpl( Box3fTPTraits  , OBox3fGeomParamSample )
        self.geomParamImpl( Box3dTPTraits  , OBox3dGeomParamSample )

        self.geomParamImpl( M33fTPTraits   , OM33fGeomParamSample )
        self.geomParamImpl( M33dTPTraits   , OM33dGeomParamSample )
        self.geomParamImpl( M44fTPTraits   , OM44fGeomParamSample )
        self.geomParamImpl( M44dTPTraits   , OM44dGeomParamSample )

        self.geomParamImpl( QuatfTPTraits  , OQuatfGeomParamSample )
        self.geomParamImpl( QuatdTPTraits  , OQuatdGeomParamSample )

        self.geomParamImpl( C3hTPTraits    , OC3hGeomParamSample )
        self.geomParamImpl( C3fTPTraits    , OC3fGeomParamSample )
        self.geomParamImpl( C3cTPTraits    , OC3cGeomParamSample )

        self.geomParamImpl( C4hTPTraits    , OC4hGeomParamSample )
        self.geomParamImpl( C4fTPTraits    , OC4fGeomParamSample )
        self.geomParamImpl( C4cTPTraits    , OC4cGeomParamSample )

        self.geomParamImpl( N2fTPTraits    , ON2fGeomParamSample )
        self.geomParamImpl( N2dTPTraits    , ON2dGeomParamSample )

        self.geomParamImpl( N3fTPTraits    , ON3fGeomParamSample )
        self.geomParamImpl( N3dTPTraits    , ON3dGeomParamSample )
