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
from alembic.AbcCoreAbstract import *
from alembic.Abc import *
from alembic.Util import *


class TypesTest(unittest.TestCase):
    def testPODEnumWrapper(self):
        self.assertIsNotNone(POD.kBooleanPOD)
        self.assertIsNotNone(POD.kUint8POD)
        self.assertIsNotNone(POD.kInt8POD)
        self.assertIsNotNone(POD.kUint16POD)
        self.assertIsNotNone(POD.kInt16POD)
        self.assertIsNotNone(POD.kUint32POD)
        self.assertIsNotNone(POD.kInt32POD)
        self.assertIsNotNone(POD.kUint64POD)
        self.assertIsNotNone(POD.kInt64POD)
        self.assertIsNotNone(POD.kFloat16POD)
        self.assertIsNotNone(POD.kFloat32POD)
        self.assertIsNotNone(POD.kFloat64POD)
        self.assertIsNotNone(POD.kStringPOD)
        self.assertIsNotNone(POD.kWstringPOD)
        self.assertIsNotNone(POD.kNumPlainOldDataTypes)
        self.assertIsNotNone(POD.kUnknownPOD)

        # If a new POD type is added, this will throw an exception
        self.assertEquals(POD.kNumPlainOldDataTypes, 14)

    def testDataTypeWrapper(self):
        # Default Constructor
        dt1 = DataType()
        self.assertEqual(dt1.getPod(), POD.kUnknownPOD)
        self.assertEqual(dt1.getExtent(), 0)

        # Access to POD and Extent
        dt1.setPod (POD.kBooleanPOD)
        dt1.setExtent (2)

        self.assertEqual(dt1.getPod(), POD.kBooleanPOD)
        self.assertEqual(dt1.getExtent(), 2)

        # Constructor
        dt2 = DataType(POD.kStringPOD, 3)

        self.assertEqual(dt2.getPod(), POD.kStringPOD)
        self.assertEqual(dt2.getExtent(), 3)

        # Constructor with a default argument
        dt3 = DataType (POD.kFloat64POD)

        self.assertEqual(dt3.getPod(), POD.kFloat64POD)
        self.assertEqual(dt3.getExtent(), 1)

        # Stringification
        self.assertEqual( str(dt1), 'bool_t[2]')
        self.assertEqual( str(dt2), 'string[3]')
        self.assertEqual( str(dt3), 'float64_t')

    def testTimeSampling(self):
        # Partial copy of TestTimingSampling.cpp

        # Test CyclicTime
        tvec = TimeVector()
        tvec[:] = [ -0.7, -0.1, 0.2]

        timePerCycle = 1.0
        numSamplesPerCycle = 3;
        numSamps = 97

        tSampTyp = TimeSamplingType( numSamplesPerCycle, timePerCycle )
        tSamp = TimeSampling( tSampTyp, tvec )

        # Test TimeSamplingType
        self.assertTrue(tSampTyp.isCyclic())
        self.assertEqual(tSampTyp.getNumSamplesPerCycle(), numSamplesPerCycle)
        self.assertEqual(tSampTyp.getTimePerCycle(), timePerCycle)

        # Test TimeSampling
        self.assertEqual(tSamp.getNumStoredTimes(), numSamplesPerCycle)
        self.assertEqual(tSamp.getTimeSamplingType(), tSampTyp)

        for i in range(0, numSamps):
            timeI = tSamp.getSampleTime( i )
            floorIndex = tSamp.getFloorIndex( timeI, numSamps )
            ceilIndex = tSamp.getCeilIndex( timeI, numSamps )
            nearIndex = tSamp.getNearIndex( timeI, numSamps )

            self.assertTrue(floorIndex == i and ceilIndex == i and nearIndex == i)

            if i > 0:
                timeIm1 = tSamp.getSampleTime( i - 1 )
                self.assertLess(timeIm1, timeI)

                if i > numSamplesPerCycle:
                    cur = tSamp.getSampleTime( i )
                    prev = tSamp.getSampleTime( i - numSamplesPerCycle )
                    self.assertAlmostEqual(cur - prev, timePerCycle)


    def testDimensions(self):
        # rank 1
        rank1 = Dimensions( 1 )
        self.assertEqual(rank1.rank(), 1)
        self.assertEqual(rank1[0], 1)

        # rank 2
        rank2 = Dimensions()
        rank2.setRank( 2 )
        self.assertEqual(rank2.rank(), 2)

        rank2[0] = 11
        rank2[1] = 12
        self.assertEqual(rank2[0], 11)
        self.assertEqual(rank2[1], 12)
        self.assertEqual(rank2.numPoints(), 11 * 12)

        rank2Copy = Dimensions( rank2 )
        self.assertEqual(rank2Copy[0], 11)
        self.assertEqual(rank2Copy[1], 12)
        self.assertEqual(rank2Copy, rank2)

        # rank 3
        rank3 = Dimensions()
        rank3.setRank( 3 )
        rank3[:] = 20
        self.assertEqual(rank3[0], 20)
        self.assertEqual(rank3[1], 20)
        self.assertEqual(rank3[2], 20)

        rank2Copy = rank3
        self.assertEqual(rank2Copy, rank3)
