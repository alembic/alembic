import sys
import imath
from alembic.Abc import *

# Aliases
M44d = imath.M44d
V3f = imath.V3f
V3d = imath.V3d
Box3d = imath.Box3d

# Test helper functions
def setArray( iTPTraits, *iList ):
    array = iTPTraits.arrayType( len( iList ) )
    for i in range( len( iList ) ):
        array[i] = iList[i]
    return array

# Xform data
xformvec = [M44d(), M44d() * 2, M44d() * 4]

# Cube mesh data
faceCounts = setArray(Int32TPTraits, 4, 4, 4, 4, 4, 4) 

faceIndices = setArray(Int32TPTraits,
                       1, 0, 2, 3, 3, 2, 6, 7, 7, 6, 4, 5, 5, 4, 0,
                       1, 5, 1, 3, 7, 0, 4, 6, 2
                       ) 

points = setArray(P3fTPTraits,
                  V3f(-1, -1, 1 ), V3f(1, -1, 1),
                  V3f(-1, 1, 1), V3f(1, 1, 1),
                  V3f(-1, -1, -1), V3f(1, -1, -1),
                  V3f(-1, 1, -1), V3f(1, 1, -1)
                  )

selfBnds = Box3d(V3d(-1, -1, -1), 
                 V3d( 1, 1, 1)
                 ) 

