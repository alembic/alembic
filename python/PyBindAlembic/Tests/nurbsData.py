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

def setArray( iTPTraits, *iList ):
    array = np.asarray( iList, iTPTraits )
    return array

nu = 4
nv = 4
uOrder = 4
vOrder = 4

uKnot = setArray( np.float32, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 )
vKnot = setArray( np.float32, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 )

P = setArray(
    np.float32,

    V3f( 0.0, 0.0, -3.0 ),
    V3f( 0.0, 1.0, -3.0 ),
    V3f( 0.0, 2.0, -3.0 ),
    V3f( 0.0, 3.0, -3.0 ),
    V3f( 1.0, 0.0, -3.0 ),
    V3f( 1.0, 1.0, 3.0 ),
    V3f( 1.0, 2.0, 3.0 ),
    V3f( 1.0, 3.0, -3.0 ),
    V3f( 2.0, 0.0, -3.0 ),
    V3f( 2.0, 1.0, 3.0 ),
    V3f( 2.0, 2.0, 3.0 ),
    V3f( 2.0, 3.0, -3.0 ),
    V3f( 3.0, 0.0, -3.0 ),
    V3f( 3.0, 1.0, -3.0 ),
    V3f( 3.0, 2.0, -3.0 ),
    V3f( 3.0, 3.0, -3.0 )
)

Pw = setArray(
    np.float32,

    1.0, 0.5, 1.0, 0.5, 1.0, 0.5, 1.0, 0.5,
    1.0, 0.5, 1.0, 0.5, 1.0, 0.5, 1.0, 0.5
)

trim_nLoops = 1
trim_nCurves = setArray( np.intc, 1 )
trim_order = setArray( np.intc, 3 )
trim_knot = setArray(
    np.float32,

    0.0, 0.0, 0.0, 1.0, 1.0, 2.0, 2.0,
    3.0, 3.0, 4.0, 4.0, 4.0
)

trim_min = setArray( np.float32, 0.0 )
trim_max = setArray( np.float32, 1.0 )
trim_n = setArray( np.intc, 9 )
trim_u = setArray(
    np.float32,

    1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0
)

trim_v = setArray(
    np.float32,

    0.5, 1.0, 2.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.5
)

trim_w = setArray(
    np.float32,

    1.0, 1.0, 2.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0
)
