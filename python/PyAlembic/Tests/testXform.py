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
#-*****************************************************************************

import imath
import alembic
import traceback

testList = []

kScaleOperation = alembic.AbcG.XformOperationType.kScaleOperation
kTranslateOperation = alembic.AbcG.XformOperationType.kTranslateOperation
kRotateOperation = alembic.AbcG.XformOperationType.kRotateOperation
kMatrixOperation = alembic.AbcG.XformOperationType.kMatrixOperation
kRotateXOperation = alembic.AbcG.XformOperationType.kRotateXOperation
kRotateYOperation = alembic.AbcG.XformOperationType.kRotateYOperation
kRotateZOperation = alembic.AbcG.XformOperationType.kRotateZOperation
kMatrixHint = 0
kRotateHint = 0
kScaleHint = 0
kTranslateHint = 0

def xformOut():
    """write an oarchive with an xform in it"""

    oarch = alembic.Abc.OArchive('xform1.abc')

    a = alembic.AbcG.OXform(oarch.getTop(), 'a')
    b = alembic.AbcG.OXform(a, 'b')
    c = alembic.AbcG.OXform(b, 'c')
    d = alembic.AbcG.OXform(c, 'd')
    e = alembic.AbcG.OXform(d, 'e')
    f = alembic.AbcG.OXform(e, 'f')
    g = alembic.AbcG.OXform(f, 'g')

    transop = alembic.AbcG.XformOp(kTranslateOperation, kTranslateHint)
    rotatop = alembic.AbcG.XformOp(kRotateOperation, kRotateHint)
    scaleop = alembic.AbcG.XformOp(kScaleOperation, kScaleHint)
    matrixop = alembic.AbcG.XformOp(kMatrixOperation, kMatrixHint)

    assert a.getSchema().getNumSamples() == 0

    asamp = alembic.AbcG.XformSample()
    for i in range(20):
        asamp.addOp(transop, imath.V3d(12.0, i+42.0, 20.0))
        if i == 18:
            asamp.setChildBounds(imath.Box3d(imath.V3d(-1.0, -1.0, -1.0), 
                                             imath.V3d(1.0, 1.0, 1.0)))
        a.getSchema().set(asamp)

    bsamp = alembic.AbcG.XformSample()
    for i in range(20):
        bsamp.setInheritsXforms(i % 2)
        b.getSchema().set(bsamp)

    # for c we write nothing

    dsamp = alembic.AbcG.XformSample()
    dsamp.addOp(scaleop, imath.V3d(3.0, 6.0, 9.0))
    d.getSchema().set(dsamp)

    esamp = alembic.AbcG.XformSample()
    identmat = imath.M44d()
    identmat.makeIdentity()

    esamp.addOp(transop, imath.V3d(0.0, 0.0, 0.0))
    esamp.addOp(alembic.AbcG.XformOp(kMatrixOperation, kMatrixHint), identmat)
    esamp.addOp(scaleop, imath.V3d(1.0, 1.0, 1.0))
    e.getSchema().set(esamp)

    fsamp = alembic.AbcG.XformSample()
    fsamp.addOp(transop, imath.V3d(3.0, -4.0, 5.0))
    f.getSchema().set(fsamp)

    # this will cause the Xform's values property to be an ArrayProperty
    # since there will be 20 * 16 channels.
    gsamp = alembic.AbcG.XformSample()
    gmatrix = imath.M44d()
    gmatrix.makeIdentity()
    for i in range(20):
        gmatrix[0][1] = float(i)
        gsamp.addOp(matrixop, gmatrix)
    g.getSchema().set(gsamp)

def xformIn():
    """read in an iarchive with an xform and check vals"""

    iarch = alembic.Abc.IArchive('xform1.abc')

    identity = imath.M44d()
    xs = alembic.AbcG.XformSample()

    a = alembic.AbcG.IXform(iarch.getTop(), 'a')

    assert a.getSchema().getNumSamples() == 20
    assert a.getSchema().getNumOps() == 1

    for i in range(20):
        xs = a.getSchema().getValue(alembic.Abc.ISampleSelector(i))
        assert xs.getNumOps() == 1
        assert xs[0].isTranslateOp()
        assert xs[0].isYAnimated()
        assert xs[0].isXAnimated() == False
        assert xs[0].isZAnimated() == False
        assert xs.getTranslation() == imath.V3d(12.0, i+42.0, 20.0)
        assert xs.getMatrix() == imath.M44d().setTranslation(
                imath.V3d(12.0, i+42.0, 20.0))

    b = alembic.AbcG.IXform(a, 'b')
    xs = b.getSchema().getValue()
    assert b.getSchema().getTimeSampling().getTimeSamplingType().isUniform()
    # the schema is not static, because set() was called 20 times on it
    assert not b.getSchema().isConstant()
    assert b.getSchema().getNumSamples() == 20
    assert xs.getNumOps() == 0
    assert b.getSchema().getNumOps() == 0
    assert xs.getMatrix() == identity

    c = alembic.AbcG.IXform(b, 'c')
    xs = c.getSchema().getValue()
    assert xs.getNumOps() == 0 
    assert c.getSchema().getNumOps() == 0 
    assert xs.getMatrix() == identity 
    assert c.getSchema().getInheritsXforms() 
    assert c.getSchema().isConstantIdentity() 

    d = alembic.AbcG.IXform(c, 'd') 
    xs = d.getSchema().getValue()
    assert xs.getNumOps() == 1 
    assert d.getSchema().getNumOps() == 1 
    assert xs[0].isScaleOp() 
    assert not (xs[0].isXAnimated() or xs[0].isYAnimated()
                        or xs[0].isZAnimated())
    assert xs.getMatrix() == imath.M44d().setScale(imath.V3d(3.0, 6.0, 9.0)) 
    assert d.getSchema().getInheritsXforms() 

    e = alembic.AbcG.IXform(d, 'e')
    assert e.getSchema().isConstantIdentity() 
    assert e.getSchema().isConstant() 
    assert e.getSchema().getNumOps() == 3 

    f = alembic.AbcG.IXform(e, 'f')
    assert f.getSchema().isConstant()
    assert not f.getSchema().isConstantIdentity()

    g = alembic.AbcG.IXform(f, 'g')
    gmatrix = imath.M44d()
    gmatrix.makeIdentity()
    gsamp = g.getSchema().getValue()
    assert gsamp.getNumOps() == 20
    assert gsamp.getNumOpChannels() == 20 * 16 
    assert g.getSchema().getNumSamples() == 1 
    assert g.getSchema().isConstant() 
    assert not g.getSchema().isConstantIdentity() 

    for i in range(20):
        assert gsamp[i].getChannelValue(1) == float(i) 

def testXformBinding():
    xformOut()
    xformIn()

testList.append(('testXformBinding', testXformBinding))

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""

