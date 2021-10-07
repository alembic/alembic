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
import numpy as np
import alembic
from alembic.Abc import *

# Test helper functions
def setArray( iTPTraits, *iList ):
    array = np.asarray(iList, iTPTraits)
    return array

# Xform data
#xformvec = [M44d(), M44d() * 2, M44d() * 4]
xformvec = [(1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.), (2., 0., 0., 0., 0., 2., 0., 0., 0., 0., 2., 0., 0., 0., 0., 2.), (4., 0., 0., 0., 0., 4., 0., 0., 0., 0., 4., 0., 0., 0., 0., 4.)]

# Cube mesh data
faceCounts = setArray( np.intc, 4, 4, 4, 4, 4, 4 )

faceIndices = setArray(
    np.intc,

    1, 0, 2, 3, 3, 2, 6, 7, 7, 6, 4, 5, 5, 4, 0, 1, 5, 1, 3, 7, 0, 4, 6, 2
)

points = setArray(
    np.float32,

    V3f(-1, -1, 1 ),
    V3f(1, -1, 1),
    V3f(-1, 1, 1),
    V3f(1, 1, 1),
    V3f(-1, -1, -1),
    V3f(1, -1, -1),
    V3f(-1, 1, -1),
    V3f(1, 1, -1)
)

selfBnds = setArray( np.float64, -1, -1, -1, 1, 1, 1 )
#selfBnds = Box3d( V3d(-1, -1, -1), V3d( 1, 1, 1) )
