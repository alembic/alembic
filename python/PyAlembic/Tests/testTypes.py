import sys, imath
import alembic

def equalWithAbsError (a0, a1, error):
    return abs (a0 - a1) <= error

testList = []

def testPODEnumWrapper():
    print alembic.Util.POD.kBooleanPOD
    print alembic.Util.POD.kUint8POD
    print alembic.Util.POD.kInt8POD
    print alembic.Util.POD.kUint16POD
    print alembic.Util.POD.kInt16POD
    print alembic.Util.POD.kUint32POD
    print alembic.Util.POD.kInt32POD
    print alembic.Util.POD.kUint64POD
    print alembic.Util.POD.kInt64POD
    print alembic.Util.POD.kFloat16POD
    print alembic.Util.POD.kFloat32POD
    print alembic.Util.POD.kFloat64POD
    print alembic.Util.POD.kStringPOD
    print alembic.Util.POD.kWstringPOD
    print alembic.Util.POD.kNumPlainOldDataTypes
    print alembic.Util.POD.kUnknownPOD

    # If a new POD type is added, this will throw an exception
    assert alembic.Util.POD.kNumPlainOldDataTypes == 14

testList.append( ( 'testPODEnumWrapper', testPODEnumWrapper ) )

def testDataTypeWrapper():
    # Default Constructor
    dt1 = alembic.AbcA.DataType()
    assert dt1.getPod() == alembic.Util.POD.kUnknownPOD
    assert dt1.getExtent() == 0

    # Access to POD and Extent
    dt1.setPod (alembic.Util.POD.kBooleanPOD)
    dt1.setExtent (2)

    assert dt1.getPod() == alembic.Util.POD.kBooleanPOD
    assert dt1.getExtent() == 2

    # Constructor
    dt2 = alembic.AbcA.DataType(alembic.Util.POD.kStringPOD, 3)

    assert dt2.getPod() == alembic.Util.POD.kStringPOD
    assert dt2.getExtent() == 3

    # Constructor with a default argument
    dt3 = alembic.AbcA.DataType (alembic.Util.POD.kFloat64POD)

    assert dt3.getPod() == alembic.Util.POD.kFloat64POD
    assert dt3.getExtent() == 1

    # Stringification
    assert ( '%s' % dt1 ) == 'bool_t[2]'
    assert ( '%s' % dt2 ) == 'string[3]'
    assert ( '%s' % dt3 ) == 'float64_t'

testList.append( ('testDataTypeWrapper', testDataTypeWrapper ) )

def testTimeSampling():
    # Partial copy of TestTimingSampling.cpp

    # Test CyclicTime
    tvec = alembic.AbcA.TimeVector()
    tvec[:] = [ -0.7, -0.1, 0.2]

    timePerCycle = 1.0
    numSamplesPerCycle = 3;
    numSamps = 97

    tSampTyp = alembic.AbcA.TimeSamplingType( numSamplesPerCycle, timePerCycle )
    tSamp = alembic.AbcA.TimeSampling( tSampTyp, tvec )

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
    rank1 = alembic.Util.Dimensions( 1 )
    assert rank1.rank() == 1
    assert rank1[0] == 1

    # rank 2
    rank2 = alembic.Util.Dimensions()
    rank2.setRank( 2 )
    assert rank2.rank() == 2

    rank2[0] = 11
    rank2[1] = 12;
    assert rank2[0] == 11
    assert rank2[1] == 12
    assert rank2.numPoints() == 11 * 12

    rank2Copy = alembic.Util.Dimensions( rank2 )
    assert rank2Copy[0] == 11
    assert rank2Copy[1] == 12
    assert rank2Copy == rank2

    # rank 3
    rank3 = alembic.Util.Dimensions()
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

# Local Variables:
# mode:python
# End:
