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
from alembic.Abc import *

def setArray( iTPTraits, *iList ):
    array = iTPTraits.arrayType( len( iList ) )
    for i in range( len( iList ) ):
        array[i] = iList[i]
    return array  

verts = setArray(
    V3fTPTraits,

    V3f(-1.0, -1.0, -1.0 ),
    V3f( 1.0, -1.0, -1.0 ),
    V3f(-1.0,  1.0, -1.0 ),
    V3f( 1.0,  1.0, -1.0 ),
    V3f(-1.0, -1.0,  1.0 ),
    V3f( 1.0, -1.0,  1.0 ),
    V3f(-1.0,  1.0,  1.0 )
)

velos = setArray(
    Float32TPTraits,

    -0.5, -0.5, -0.5,
    0.25, -0.5, -0.5,
     0.5, 0.25, -0.5,
    0.25, 0.25, -0.5,
    -0.5, -0.5, 0.25,
    0.25, -0.5, 0.25,
    -0.5, 0.25, 0.25,
    0.25, 0.25, 0.25
)

indices = setArray(
    Int32TPTraits,

    0, 4, 6, 2,
    5, 1, 3, 7,
    0, 1, 5, 4,
    6, 7, 3, 2,
    1, 0, 2, 3,
    4, 5, 7, 6
)

counts = setArray( Int32TPTraits,  4, 4, 4, 4, 4, 4 )

normals = setArray(
    V3fTPTraits,

    V3f( -1.0, 0.0, 0.0 ),
    V3f( -1.0, 0.0, 0.0 ),
    V3f( -1.0, 0.0, 0.0 ),
    V3f( -1.0, 0.0, 0.0 ),
    V3f( 1.0, 0.0, 0.0 ),
    V3f( 1.0, 0.0, 0.0 ),
    V3f( 1.0, 0.0, 0.0 ),
    V3f( 1.0, 0.0, 0.0 ),
    V3f( 0.0, -1.0, 0.0 ),
    V3f( 0.0, -1.0, 0.0 ),
    V3f( 0.0, -1.0, 0.0 ),
    V3f( 0.0, -1.0, 0.0 ),
    V3f( 0.0, 1.0, 0.0 ),
    V3f( 0.0, 1.0, 0.0 ),
    V3f( 0.0, 1.0, 0.0 ),
    V3f( 0.0, 1.0, 0.0 ),
    V3f( 0.0, 0.0, -1.0 ),
    V3f( 0.0, 0.0, -1.0 ),
    V3f( 0.0, 0.0, -1.0 ),
    V3f( 0.0, 0.0, -1.0 ),
    V3f( 0.0, 0.0, 1.0 ),
    V3f( 0.0, 0.0, 1.0 ),
    V3f( 0.0, 0.0, 1.0 ),
    V3f( 0.0, 0.0, 1.0 )
)

uvs = setArray(
    V2fTPTraits,

    V2f( 0.0, 0.0 ),
    V2f( 1.0, 0.0 ),
    V2f( 1.0, 1.0 ),
    V2f( 0.0, 1.0 ),
    V2f( 0.0, 0.0 ),
    V2f( 1.0, 0.0 ),
    V2f( 1.0, 1.0 ),
    V2f( 0.0, 1.0 ),
    V2f( 0.0, 0.0 ),
    V2f( 1.0, 0.0 ),
    V2f( 1.0, 1.0 ),
    V2f( 0.0, 1.0 ),
    V2f( 0.0, 0.0 ),
    V2f( 1.0, 0.0 ),
    V2f( 1.0, 1.0 ),
    V2f( 0.0, 1.0 ),
    V2f( 0.0, 0.0 ),
    V2f( 1.0, 0.0 ),
    V2f( 1.0, 1.0 ),
    V2f( 0.0, 1.0 ),
    V2f( 0.0, 0.0 ),
    V2f( 1.0, 0.0 ),
    V2f( 1.0, 1.0 ),
    V2f( 0.0, 1.0 )
)




