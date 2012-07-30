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
from alembic.Util import *

C3c = Color3c
C3f = Color3f
C4c = Color4c
C4f = Color4f

# Test data for number type traits
ScalarTraitsData = {}
SmallArrayTraitsData = {}
ArrayTraitsData = {}

# Comparison measures for number type traits
TraitsComparison = {}

# -----------------------------------------------------------------
# Test helper functions
# -----------------------------------------------------------------
# Comparison functions for various data types
EPS = 0.01 
def compareExact (a0, a1):
    return a0 == a1

def compareEPS (a0, a1):
    return abs (a0 - a1) <= EPS

def compareVec3Exact (a0, a1):
    return a0[0] == a1[0] and a0[1] == a1[1] and a0[2] == a1[2]

def compareVec4Exact (a0, a1):
    return a0[0] == a1[0] and a0[1] == a1[1] and a0[2] == a1[2] and \
           a0[3] == a1[3]

def compareVecEPS (a0, a1):
    return a0.equalWithRelError (a1, EPS)

def compareBoxExact (a0, a1):
    return compareExact (a0.min(), a1.min()) and \
           compareExact (a0.max(), a1.max())

def compareBoxEPS (a0, a1):
    return compareVecEPS (a0.min(), a1.min()) and \
           compareVecEPS (a0.max(), a1.max())

def compareQuatEPS (a0, a1):
    return compareEPS (a0.angle(), a1.angle()) and \
           compareVecEPS (a0.axis(), a1.axis())

def compareC3EPS (a0, a1):
    return compareEPS (a0[0], a1[0]) and compareEPS (a0[1], a1[1]) and \
           compareEPS (a0[2], a1[2])

def compareC4EPS (a0, a1):
    return compareEPS (a0[0], a1[0]) and compareEPS (a0[1], a1[1]) and \
           compareEPS (a0[2], a1[2]) and compareEPS (a0[3], a1[3])


def WRITES( *iArgs ):
    iTraits = iArgs[0];
    name = iTraits.__name__
    ScalarTraitsData[name] = iArgs

def WRITESA( iTraits, *iArgs ):
    name = iTraits.__name__
    SmallArrayTraitsData[name] = iTraits, createArray( iTraits.arrayType, iArgs )

def createArray( iArray, iList ):
    a = iArray( len( iList ) )
    for i in range( len( iList ) ):
        a[i] = iList[i]
    return a

def WRITEA( iTraits, *iArgs ):
    name = iTraits.__name__
    ArrayTraitsData[name] = iTraits, createArray( iTraits.arrayType, iArgs )

def buildTestScalarData():
    # Scalar DataType
    
    # POD types
    # extent 1
    WRITES (BooleanTPTraits, 0, 1, 0)
    WRITES (Uint8TPTraits, 3, 4)
    WRITES (Int8TPTraits, -5 )
    WRITES (Uint16TPTraits, 6 )
    WRITES (Int16TPTraits, -7, 8 )
    WRITES (Uint32TPTraits, 9, 10, 11, 12 )
    WRITES (Int32TPTraits, -13, 14, -15 )
    WRITES (Uint64TPTraits, 16 )
    WRITES (Float16TPTraits, 1.234, 6.789)
    WRITES (Float32TPTraits, -1234.5678)
    WRITES (Float64TPTraits, 1234.5678)
    WRITES (StringTPTraits, "abc", "def")
    WRITES (WstringTPTraits, u"asdf" )


    # Compound Types
    # extent 2
    # vector2
    WRITES (V2sTPTraits, V2s (1, 2), V2s (-1, 0))
    WRITES (V2iTPTraits, V2i (3, 4))
    WRITES (V2fTPTraits, V2f (1.2, 3.4), V2f (5.6, 0))
    WRITES (V2dTPTraits, V2d (5.6, 7.8))

    # point2
    WRITES (P2sTPTraits, V2s (10, 20), V2s (-10, 0))
    WRITES (P2iTPTraits, V2i (30, 40))
    WRITES (P2fTPTraits, V2f (1.02, 3.04), V2f (5.06, 0))
    WRITES (P2dTPTraits, V2d (5.06, 7.08))

    # normal2
    WRITES (N2fTPTraits, V2f (1.1, 1.2), V2f (-1.1, 0.1))
    WRITES (N2dTPTraits, V2d (3.4, 4.4))

    # extent 3
    # color3
    WRITES (C3cTPTraits, Color3c (1, 2, 3))
    # TODO: Color3h types are not wrapped. C3f is used instead.
    WRITES (C3hTPTraits, Color3f (0.1, 0.2, 0.3))
    WRITES (C3fTPTraits, Color3f (15.6, 7.8, 9.0))

    # vector3
    WRITES (V3sTPTraits, V3s (4, 5, 6))
    WRITES (V3iTPTraits, V3i (7, 8, 9))
    WRITES (V3fTPTraits, V3f (25.6, 7.8, 9.0))
    WRITES (V3dTPTraits, V3d (35.6, 7.8, 14.2))

    # point3
    WRITES (P3sTPTraits, V3s (40, 50, 60))
    WRITES (P3iTPTraits, V3i (70, 80, 90))
    WRITES (P3fTPTraits, V3f (25.06, 7.08, 90.0))
    WRITES (P3dTPTraits, V3d (35.06, 7.08, 14.02))

    # normal3
    WRITES (N3fTPTraits, V3f (11.1, 11.2, 11.3), V3f (-11.1, 0.11, 0.22))
    WRITES (N3dTPTraits, V3d (31.4, 41.4, 41.5))

    # extent 4
    # color4
    WRITES (C4cTPTraits, Color4c (1, 2, 3, 4) )
    # TODO: Color4h types are not wrapped. C4f is used instead.
    WRITES (C4hTPTraits, Color4f (5.6, 7.8, 1.2, 3.4) )
    WRITES (C4fTPTraits, Color4f (1.111, 2.222, 3.333, 4.444) )

    # box2
    WRITES (Box2sTPTraits, Box2s (V2s (0, 0), V2s (1, 1)));
    WRITES (Box2iTPTraits, Box2i (V2i (0, 0), V2i (2, 2)));
    WRITES (Box2fTPTraits, Box2f (V2f (0, 0), V2f (0.3, 0.3)));
    WRITES (Box2dTPTraits, Box2d (V2d (0, 0), V2d (0.4, 0.4)));

    # quat
    WRITES (QuatfTPTraits, Quatf (-1.0, 2.0, 3.0, -4.0))
    WRITES (QuatdTPTraits, Quatd (0.5, 0.6, -0.7, -0.8))

    # extent 6
    # box3
    WRITES (Box3sTPTraits, Box3s (V3s (-1, -1, -1), V3s (1, 1, 1)))
    WRITES (Box3iTPTraits, Box3i (V3i (0, 0, 0), V3i (1, 1, 1)))
    WRITES (Box3fTPTraits, Box3f (V3f(1.2, 3.4, 5.6), V3f(-1.2, 3.4, -5.6)))
    WRITES (Box3dTPTraits, Box3d (V3d(0, 0, 0), V3d(5.6, 7.8, 9.0)))

    # extent 9
    # matrix33
    WRITES (M33fTPTraits, M33f(1)*2)
    WRITES (M33dTPTraits, M33d(1)*3)

    # extent 16
    # matrix44
    WRITES (M44fTPTraits, M44f(1)*4)
    WRITES (M44dTPTraits, M44d(1)*5)
    
def buildTestSmallArrayData():
    # Scalar DataType
    
    # POD types
    # extent > 1
    WRITESA (BooleanTPTraits, 0, 0, 1)
    WRITESA (Uint8TPTraits, 1, 2, 3, 4, 5, 6)
    WRITESA (Int8TPTraits, -1, 2, -3, 4, -5, 6)
    WRITESA (Uint16TPTraits, 10, 20, 30, 40, 50, 60)
    WRITESA (Int16TPTraits, -10, 20, -30, 40, -50, 60)
    WRITESA (Uint32TPTraits, 11, 22, 33, 44, 55, 66)
    WRITESA (Int32TPTraits, -11, 22, -33, 44, -55, 66)
    WRITESA (Uint64TPTraits, 13, 23, 33, 43, 53)
    WRITESA (Int64TPTraits, -13, 23, -33, 43, -53)
    WRITESA (Float16TPTraits, 1.23, 2.34, 3.45, 4.56)
    WRITESA (Float32TPTraits, 1.234, 2.345, 3.456, 4.567)
    WRITESA (Float64TPTraits, 1.2345, 2.3456, 3.4567, 4.5678)
    WRITESA (StringTPTraits, "a", "b", "c", "d", "e")
    WRITESA (WstringTPTraits, u"A", u"B", u"C", u"D", u"E")

def buildTestArrayData():
    # POD types
    # extent 1
    WRITEA (BooleanTPTraits, 0, 0, 1)
    WRITEA (Uint8TPTraits, 1, 2, 3, 4, 5, 6)
    WRITEA (Int8TPTraits, -1, 2, -3, 4, -5, 6)
    WRITEA (Uint16TPTraits, 10, 20, 30, 40, 50, 60)
    WRITEA (Int16TPTraits, -10, 20, -30, 40, -50, 60)
    WRITEA (Uint32TPTraits, 11, 22, 33, 44, 55, 66)
    WRITEA (Int32TPTraits, -11, 22, -33, 44, -55, 66)
    WRITEA (Uint64TPTraits, 13, 23, 33, 43, 53)
    WRITEA (Int64TPTraits, -13, 23, -33, 43, -53)
    WRITEA (Float16TPTraits, 1.23, 2.34, 3.45, 4.56)
    WRITEA (Float32TPTraits, 1.234, 2.345, 3.456, 4.567)
    WRITEA (Float64TPTraits, 1.2345, 2.3456, 3.4567, 4.5678)
    WRITEA (StringTPTraits, "a", "b", "c", "d", "e");
    WRITEA (WstringTPTraits, u"A", u"B", u"C", u"D", u"E");
    # Compound Types
    # extent 2
    # vector2
    WRITEA (V2sTPTraits, V2s (1, 2), V2s (-1, 1))
    WRITEA (V2iTPTraits, V2i (3, 4), V2i (5, 6))
    WRITEA (V2fTPTraits, V2f (1.2, 3.4), V2f (5.6, 7.8))
    WRITEA (V2dTPTraits, V2d (5.6, 7.8), V2d (9.10, 10.11))

    # point2
    WRITEA (P2sTPTraits, V2s (10, 20), V2s (-10, 0))
    WRITEA (P2iTPTraits, V2i (30, 40), V2i (10, 20))
    WRITEA (P2fTPTraits, V2f (1.02, 3.04), V2f (5.06, 0.11))
    WRITEA (P2dTPTraits, V2d (5.06, 7.08), V2d (-102.4, -9.5))

    # normal2
    WRITEA (N2fTPTraits, V2f (1.1, 1.2), V2f (-1.1, 0.1))
    WRITEA (N2dTPTraits, V2d (3.4, 4.4), V2d (-104.2, 104.2))

    # extent 3
    # color3
    WRITEA (C3cTPTraits, C3c (1, 2, 3), C3c (4, 5, 6))
    # TODO: C3h types are not wrapped. C3f is used instead.
    WRITEA (C3hTPTraits, C3f (0.1, 0.2, 0.3), C3f(0.4, 0.5, 0.6))
    WRITEA (C3fTPTraits, C3f (15.6, 7.8, 9.0), C3f(-1.4, -235.2, 11.1))

    # vector3
    WRITEA (V3sTPTraits, V3s (4, 5, 6), V3s (14, 132, 13))
    WRITEA (V3iTPTraits, V3i (7, 8, 9), V3i (-1, -14, 32))
    WRITEA (V3fTPTraits, V3f (25.6, 7.8, 9.0), V3f (-0.2, -1.3, 1.5))
    WRITEA (V3dTPTraits, V3d (35.6, 7.8, 14.2), V3d (13.2, 1403, -12.3))

    # point3
    WRITEA (P3sTPTraits, V3s (40, 50, 60), V3s(1, 2, 3))
    WRITEA (P3iTPTraits, V3i (70, 80, 90), V3i(-1, -2, -3))
    WRITEA (P3fTPTraits, V3f (25.06, 7.08, 90.0), V3f(-0.1, -0.2, -0.3))
    WRITEA (P3dTPTraits, V3d (35.06, 7.08, 14.02), V3d(-0.3, -0.4, -0.5))

    # normal3
    WRITEA (N3fTPTraits, V3f (11.1, 11.2, 11.3), V3f (-11.1, 0.11, 0.22))
    WRITEA (N3dTPTraits, V3d (31.4, 41.4, 41.5), V3d (-0.1, -0.2, -0.4))

    # extent 4
    # color4
    WRITEA (C4cTPTraits, C4c (1, 2, 3, 4) )
    # TODO: C4h types are not wrapped. C4f is used instead.
    WRITEA (C4hTPTraits, C4f (5.6, 7.8, 1.2, 3.4) )
    WRITEA (C4fTPTraits, C4f (1.111, 2.222, 3.333, 4.444) )

    # box2
    WRITEA (Box2sTPTraits, Box2s (V2s (0, 0), V2s (1, 1)));
    WRITEA (Box2iTPTraits, Box2i (V2i (0, 0), V2i (2, 2)));
    WRITEA (Box2fTPTraits, Box2f (V2f (0, 0), V2f (0.3, 0.3)));
    WRITEA (Box2dTPTraits, Box2d (V2d (0, 0), V2d (0.4, 0.4)));

    # quat
    WRITEA (QuatfTPTraits, Quatf (-1.0, 2.0, 3.0, -4.0))
    WRITEA (QuatdTPTraits, Quatd (0.5, 0.6, -0.7, -0.8))

    # extent 6
    # box3
    WRITEA (Box3sTPTraits, Box3s (V3s (-1, -1, -1), V3s (1, 1, 1)))
    WRITEA (Box3iTPTraits, Box3i (V3i (0, 0, 0), V3i (1, 1, 1)))
    WRITEA (Box3fTPTraits, Box3f (V3f (1.2, 3.4, 5.6), V3f (1.2, 4.4, 5.6)))
    WRITEA (Box3dTPTraits, Box3d (V3d (0, 0, 0), V3d (5.6, 7.8, 9.0)))

    # extent 9
    # matrix33
    WRITEA (M33fTPTraits, M33f (1)*2, M33f (0.1))
    WRITEA (M33dTPTraits, M33d (1)*3, M33d (3.4))

    # extent 16
    # matrix44
    WRITEA (M44fTPTraits, M44f (1)*4, M44f (1.2))
    WRITEA (M44dTPTraits, M44d (1)*5, M44d (5.6))

def buildComparisonMeasure():
    # Assign the property name and proper comparison function
    # for each type
    TraitsComparison[BooleanTPTraits.__name__] = compareExact
    TraitsComparison[Uint8TPTraits.__name__] = compareExact
    TraitsComparison[Int8TPTraits.__name__] = compareExact
    TraitsComparison[Uint16TPTraits.__name__] = compareExact
    TraitsComparison[Int16TPTraits.__name__] = compareExact
    TraitsComparison[Uint32TPTraits.__name__] = compareExact
    TraitsComparison[Int32TPTraits.__name__] = compareExact
    TraitsComparison[Uint64TPTraits.__name__] = compareExact
    TraitsComparison[Int64TPTraits.__name__] = compareExact
    TraitsComparison[Float16TPTraits.__name__] = compareEPS
    TraitsComparison[Float32TPTraits.__name__] = compareEPS
    TraitsComparison[Float64TPTraits.__name__] = compareEPS
    TraitsComparison[StringTPTraits.__name__] = compareExact
    TraitsComparison[WstringTPTraits.__name__] = compareExact

    TraitsComparison[V2sTPTraits.__name__] = compareExact
    TraitsComparison[V2iTPTraits.__name__] = compareExact
    TraitsComparison[V2fTPTraits.__name__] = compareVecEPS
    TraitsComparison[V2dTPTraits.__name__] = compareVecEPS
    TraitsComparison[P2sTPTraits.__name__] = compareExact
    TraitsComparison[P2iTPTraits.__name__] = compareExact 
    TraitsComparison[P2fTPTraits.__name__] = compareVecEPS
    TraitsComparison[P2dTPTraits.__name__] = compareVecEPS
    TraitsComparison[N2fTPTraits.__name__] = compareVecEPS
    TraitsComparison[N2dTPTraits.__name__] = compareVecEPS

    TraitsComparison[C3cTPTraits.__name__] = compareVec3Exact
    TraitsComparison[C3hTPTraits.__name__] = compareVecEPS
    TraitsComparison[C3fTPTraits.__name__] = compareC3EPS
    TraitsComparison[V3sTPTraits.__name__] = compareExact
    TraitsComparison[V3iTPTraits.__name__] = compareExact
    TraitsComparison[V3fTPTraits.__name__] = compareVecEPS
    TraitsComparison[V3dTPTraits.__name__] = compareVecEPS
    TraitsComparison[P3sTPTraits.__name__] = compareExact
    TraitsComparison[P3iTPTraits.__name__] = compareExact
    TraitsComparison[P3fTPTraits.__name__] = compareVecEPS
    TraitsComparison[P3dTPTraits.__name__] = compareVecEPS
    TraitsComparison[N3fTPTraits.__name__] = compareVecEPS
    TraitsComparison[N3dTPTraits.__name__] = compareVecEPS

    TraitsComparison[C4cTPTraits.__name__] = compareVec4Exact
    TraitsComparison[C4hTPTraits.__name__] = compareC4EPS
    TraitsComparison[C4fTPTraits.__name__] = compareC4EPS
    TraitsComparison[Box2sTPTraits.__name__] = compareBoxExact
    TraitsComparison[Box2iTPTraits.__name__] = compareBoxExact
    TraitsComparison[Box2fTPTraits.__name__] = compareBoxEPS
    TraitsComparison[Box2dTPTraits.__name__] = compareBoxEPS
    TraitsComparison[QuatfTPTraits.__name__] = compareQuatEPS
    TraitsComparison[QuatdTPTraits.__name__] = compareQuatEPS

    TraitsComparison[Box3sTPTraits.__name__] = compareBoxExact
    TraitsComparison[Box3iTPTraits.__name__] = compareBoxExact
    TraitsComparison[Box3fTPTraits.__name__] = compareBoxEPS
    TraitsComparison[Box3dTPTraits.__name__] = compareBoxEPS

    TraitsComparison[M33fTPTraits.__name__] = compareVecEPS
    TraitsComparison[M33dTPTraits.__name__] = compareVecEPS
    TraitsComparison[M44fTPTraits.__name__] = compareVecEPS
    TraitsComparison[M44dTPTraits.__name__] = compareVecEPS

# -------------------------------------------------------------------------
# Main loop

buildTestScalarData()
buildTestSmallArrayData()
buildTestArrayData()
buildComparisonMeasure()


