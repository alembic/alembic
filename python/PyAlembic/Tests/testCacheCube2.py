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
from alembic.AbcGeom import *
from alembic.Util import *
from cubeData import *

# In this test, we export a polygon cube with xform data
# The cube is animated by manipulating the xform transforms
# over time.

# This is the scatch of the file hierarchy.
# Abc
#   |- Xform
#       |- .inherits
#       |- .ops
#       |- .vals
#       |- PolyMesh
#           |- .faceCounts
#           |- .faceIndices
#           |- P
#           |- .selfBnds

# TimeSampling data
tvec = TimeVector()
tvec[:] = [ 1, 2, 3 ]

timePerCycle = 3.0
numSamplesPerCycle = len(tvec)

tst = TimeSamplingType( numSamplesPerCycle, timePerCycle )
ts = TimeSampling( tst, tvec )

class CacheCubeTest2(unittest.TestCase):
    # Test exporting a simple cube quad mesh
    def testExportCubeGeom(self):

        # create the archive
        top = OArchive( 'cube2.abc' ).getTop()
        tsidx = top.getArchive().addTimeSampling(ts)

        # create the top xform
        xform = OXform(top, 'cube1', tsidx)
        xsamp = XformSample()
        xform.getSchema().set(xsamp)

        # the mesh shape
        meshObj = OPolyMesh(xform, 'cube1Shape')
        mesh = meshObj.getSchema()
        mesh_samp = OPolyMeshSchemaSample(points, faceIndices, faceCounts)
        mesh_samp.setSelfBounds(selfBnds)
        mesh.set(mesh_samp)

    # Test importing the exported quad mesh
    def testImportCubeGeom(self):

        top = IArchive( 'cube2.abc' ).getTop()
        self.assertEqual(top.getNumChildren(), 1)

        # xform
        xform = IXform(top, 'cube1')
        self.assertEqual(xform.getName(), 'cube1')
        self.assertEqual(xform.getSchema().getNumSamples(), 1)

        # polymesh
        meshObj = IPolyMesh(xform, 'cube1Shape')
        mesh = meshObj.getSchema()
        self.assertEqual(mesh.getNumSamples(), 1)

        mesh_samp = mesh.getValue(ISampleSelector(0))
        self.assertEqual(mesh_samp.getPositions(), points)
        self.assertEqual(mesh_samp.getFaceCounts(), faceCounts)
        self.assertEqual(mesh_samp.getFaceIndices(), faceIndices)
        self.assertEqual(mesh_samp.getSelfBounds(), selfBnds)
