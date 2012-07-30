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

from alembic.AbcCoreAbstract import *
from alembic.Abc import *
from alembic.Util import *

def equalWithAbsError (a0, a1, error):
    return abs (a0 - a1) <= error

testList = []

def testPODEnumWrapper():
    print POD.kBooleanPOD
    print POD.kUint8POD
    print POD.kInt8POD
    print POD.kUint16POD
    print POD.kInt16POD
    print POD.kUint32POD
    print POD.kInt32POD
    print POD.kUint64POD
    print POD.kInt64POD
    print POD.kFloat16POD
    print POD.kFloat32POD
    print POD.kFloat64POD
    print POD.kStringPOD
    print POD.kWstringPOD
    print POD.kNumPlainOldDataTypes
    print POD.kUnknownPOD

    # If a new POD type is added, this will throw an exception
    assert POD.kNumPlainOldDataTypes == 14

testList.append( ( 'testPODEnumWrapper', testPODEnumWrapper ) )

def testDataTypeWrapper():
    # Default Constructor
    dt1 = DataType()
    assert dt1.getPod() == POD.kUnknownPOD
    assert dt1.getExtent() == 0

    # Access to POD and Extent
    dt1.setPod (POD.kBooleanPOD)
    dt1.setExtent (2)

    assert dt1.getPod() == POD.kBooleanPOD
    assert dt1.getExtent() == 2

    # Constructor
    dt2 = DataType(POD.kStringPOD, 3)

    assert dt2.getPod() == POD.kStringPOD
    assert dt2.getExtent() == 3

    # Constructor with a default argument
    dt3 = DataType (POD.kFloat64POD)

    assert dt3.getPod() == POD.kFloat64POD
    assert dt3.getExtent() == 1

    # Stringification
    assert ( '%s' % dt1 ) == 'bool_t[2]'
    assert ( '%s' % dt2 ) == 'string[3]'
    assert ( '%s' % dt3 ) == 'float64_t'

testList.append( ('testDataTypeWrapper', testDataTypeWrapper ) )

def testTimeSampling():
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
    assert tSampTyp.isCyclic()
    assert tSampTyp.getNumSamplesPerCycle() == numSamplesPerCycle
    assert tSampTyp.getTimePerCycle() == timePerCycle

    # Test TimeSampling
    assert tSamp.getNumStoredTimes() == numSamplesPerCycle
    assert tSamp.getTimeSamplingType() == tSampTyp

    for i in range(0, numSamps):
        timeI = tSamp.getSampleTime( i )
        floorIndex = tSamp.getFloorIndex( timeI, numSamps )
        ceilIndex = tSamp.getCeilIndex( timeI, numSamps )
        nearIndex = tSamp.getNearIndex( timeI, numSamps )

        assert floorIndex == i and ceilIndex == i and nearIndex == i

        if i > 0:
            timeIm1 = tSamp.getSampleTime( i - 1 )
            assert timeIm1 < timeI

            if i > numSamplesPerCycle:
                cur = tSamp.getSampleTime( i )
                prev = tSamp.getSampleTime( i - numSamplesPerCycle )
                assert equalWithAbsError( cur - prev, timePerCycle, 0.00001 )

testList.append( ( 'testTimeSampling', testTimeSampling ) )

def testDimensions():
    # rank 1
    rank1 = Dimensions( 1 )
    assert rank1.rank() == 1
    assert rank1[0] == 1

    # rank 2
    rank2 = Dimensions()
    rank2.setRank( 2 )
    assert rank2.rank() == 2

    rank2[0] = 11
    rank2[1] = 12;
    assert rank2[0] == 11
    assert rank2[1] == 12
    assert rank2.numPoints() == 11 * 12

    rank2Copy = Dimensions( rank2 )
    assert rank2Copy[0] == 11
    assert rank2Copy[1] == 12
    assert rank2Copy == rank2

    # rank 3
    rank3 = Dimensions()
    rank3.setRank( 3 ) 
    rank3[:] = 20
    assert rank3[0] == 20
    assert rank3[1] == 20
    assert rank3[2] == 20

    rank2Copy = rank3
    assert rank2Copy == rank3
    
testList.append( ( 'testDimensions', testDimensions ) )

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""
