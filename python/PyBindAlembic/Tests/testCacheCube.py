#-******************************************************************************
#
# Copyright (c) 2012-2013,
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
from imath import *
import numpy as np
import alembic
from alembic.AbcCoreAbstract import *
from alembic.Abc import *
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
tvec = TimeVector([1, 2, 3])
#tvec[:] = [ 1, 2, 3 ]

timePerCycle = 3.0
numSamplesPerCycle = len(tvec)

tst = TimeSamplingType( numSamplesPerCycle, timePerCycle )
ts = TimeSampling( tst, tvec )

# Pseudo wrapper for AbcGeom Objects and their schemas
def OXformSchema( iObject ):
    meta = MetaData();
    meta.set( 'schema', 'AbcGeom_Xform_v3' )
    return OCompoundProperty( iObject.getProperties(), '.xform', meta )

def OPolyMeshSchema( iObject ):
    meta = MetaData();
    meta.set( 'schema', 'AbcGeom_PolyMesh_v1' )
    meta.set( 'schemaBaseType', 'AbcGeom_GeomBase_v1' )
    return OCompoundProperty( iObject.getProperties(), '.geom', meta )

def OXform( iObject, iName, iTimeSamplingIdx ):
    meta = MetaData()
    meta.set( 'schema', 'AbcGeom_Xform_v3' )
    meta.set( 'schemaObjTitle', 'AbcGeom_Xform_v3:.xform' )
    return OObject( iObject, iName, meta, iTimeSamplingIdx )

def OPolyMesh( iObject, iName ):
    meta = MetaData()
    meta.set( 'schema', 'AbcGeom_PolyMesh_v1' )
    meta.set( 'schemaObjTitle', 'AbcGeom_PolyMesh_v1:.geom' )
    meta.set( 'schemaBaseType', 'AbcGeom_GeomBase_v1' )
    return OObject( iObject, iName, meta )


class CacheCubeTest(unittest.TestCase):
    # Test exporting a simple cube quad mesh
    def testExportCubeGeom(self):

        top = OArchive( 'cube.abc' ).getTop()

        # Add our time sampling
        tsidx = top.getArchive().addTimeSampling(ts)

        ###########################################################################
        # cube OXform object
        xform = OXform( top, 'cube1', tsidx )

        ###########################################################################
        # OXform schema property
        xformCP = OXformSchema( xform )

        # Properties
        # .inherits
        inherits = OBoolProperty( xformCP, '.inherits' )
        for i in range( 0, numSamplesPerCycle ):
            inherits.setValue( True )

        # .ops
        ops = OUcharProperty( xformCP, '.ops' )
        for i in range( 0, numSamplesPerCycle ):
            ops.setValue( 48 )

        # .vals
        vals = OM44dProperty( xformCP, '.vals', tsidx )
        vals.setValue ( xformvec[0] )
        vals.setValue ( xformvec[1] )
        vals.setValue ( xformvec[2] )

        ###########################################################################
        # /cube/cubeShape OPolyMesh object
        shape = OPolyMesh( xform, 'cube1Shape' )

        ###########################################################################
        # OPolyMesh schema
        mesh = OPolyMeshSchema( shape )

        # Properties
        # .faceCounts
        counts = OInt32ArrayProperty( mesh, '.faceCounts' )
        counts.setValue( faceCounts )

        # .faceIndices
        indices = OInt32ArrayProperty( mesh, '.faceIndices' )
        indices.setValue( faceIndices )

        # P
        p = OP3fArrayProperty( mesh, 'P' )
        p.setValue( points )

        # selfBnds
        bnds = OBox3dProperty( mesh, '.selfBnds' )
        bnds.setValue( selfBnds )

    # Test importing the exported quad mesh
    def testImportCubeGeom(self):
        archive = IArchive( 'cube.abc' )
        self.assertEqual(archive.getMaxNumSamplesForTimeSamplingIndex(0), 1)
        self.assertEqual(archive.getMaxNumSamplesForTimeSamplingIndex(1), 3)

        top = archive.getTop()

        self.assertEqual(top.getNumChildren(), 1)

        # IXform Object
        xform = top.getChild( 0 )

        self.assertEqual(xform.getName(), 'cube1')

        # IXform Properties
        xformTopCP = xform.getProperties()
        xformCP = xformTopCP.getProperty( 0 );

        inherits = xformCP.getProperty( '.inherits' )
        ops = xformCP.getProperty( '.ops' )
        vals = xformCP.getProperty( '.vals' )

        self.assertEqual(inherits.getNumSamples(), numSamplesPerCycle)
        self.assertEqual(ops.getNumSamples(), numSamplesPerCycle)
        self.assertEqual(vals.getNumSamples(), numSamplesPerCycle)

        # Get the time sampling associated with vals.
        tSamp = vals.getTimeSampling()

        # Access index at a given time
        index0 = tSamp.getNearIndex( tvec[0] + 0.1, numSamplesPerCycle )
        index1 = tSamp.getCeilIndex( tvec[0] + 0.1, numSamplesPerCycle )
        index2 = tSamp.getFloorIndex( tvec[2] + 0.1, numSamplesPerCycle )

        self.assertEqual(index0, 0)
        self.assertEqual(index1, 1)
        self.assertEqual(index2, 2)
        val0 = vals.samples()[index0]
        val1 = vals.samples()[index1]
        val2 = vals.samples()[index2]

        # self.assertEqual(val0, xformvec[0])
        # self.assertEqual(val1, xformvec[1])
        # self.assertEqual(val2, xformvec[2])
        np.testing.assert_equal(val0, xformvec[0])
        np.testing.assert_equal(val1, xformvec[1])
        np.testing.assert_equal(val2, xformvec[2])

        # IPolyMesh Object
        mesh = xform.getChild( 'cube1Shape' )
        meshTopCP = mesh.getProperties()
        meshCP = meshTopCP.getProperty(0)

        # IPolyMesh Properties
        counts = meshCP.getProperty( '.faceCounts' )
        indices = meshCP.getProperty( '.faceIndices' )
        p = meshCP.getProperty( 'P' )
        bnds = meshCP.getProperty( '.selfBnds' )

        count0 = counts.samples()[0]
        indices0 = indices.samples()[0]
        p0 = p.samples()[0]
        bnds0 = bnds.samples()[0]

        # self.assertEqual(count0, faceCounts)
        # self.assertEqual(indices0, faceIndices)
        # self.assertEqual(p0, points)
        # self.assertEqual(bnds0, selfBnds)
        np.testing.assert_equal(count0, faceCounts)
        np.testing.assert_equal(indices0, faceIndices)
        np.testing.assert_equal(p0, points)
        np.testing.assert_equal(bnds0, selfBnds)
