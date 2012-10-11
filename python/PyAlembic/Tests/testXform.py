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
from alembic.AbcGeom import *

testList = []

kScaleOperation = XformOperationType.kScaleOperation
kTranslateOperation = XformOperationType.kTranslateOperation
kRotateOperation = XformOperationType.kRotateOperation
kMatrixOperation = XformOperationType.kMatrixOperation
kRotateXOperation = XformOperationType.kRotateXOperation
kRotateYOperation = XformOperationType.kRotateYOperation
kRotateZOperation = XformOperationType.kRotateZOperation
kMatrixHint = 0
kRotateHint = 0
kScaleHint = 0
kTranslateHint = 0

def xformOut():
    """write an oarchive with an xform in it"""

    oarch = OArchive('xform1.abc')

    a = OXform(oarch.getTop(), 'a')
    b = OXform(a, 'b')
    c = OXform(b, 'c')
    d = OXform(c, 'd')
    e = OXform(d, 'e')
    f = OXform(e, 'f')
    g = OXform(f, 'g')

    transop = XformOp(kTranslateOperation, kTranslateHint)
    rotatop = XformOp(kRotateOperation, kRotateHint)
    scaleop = XformOp(kScaleOperation, kScaleHint)
    matrixop = XformOp(kMatrixOperation, kMatrixHint)

    assert a.getSchema().getNumSamples() == 0

    asamp = XformSample()
    for i in range(20):
        asamp.addOp(transop, V3d(12.0, i+42.0, 20.0))
        if i == 18:
            a.getSchema().getChildBoundsProperty().setValue(
                Box3d(V3d(-1.0, -1.0, -1.0), V3d(1.0, 1.0, 1.0)))
        a.getSchema().set(asamp)

    bsamp = XformSample()
    for i in range(20):
        bsamp.setInheritsXforms(i % 2)
        b.getSchema().set(bsamp)

    # for c we write nothing

    dsamp = XformSample()
    dsamp.addOp(scaleop, V3d(3.0, 6.0, 9.0))
    d.getSchema().set(dsamp)

    esamp = XformSample()
    identmat = M44d()
    identmat.makeIdentity()

    esamp.addOp(transop, V3d(0.0, 0.0, 0.0))
    esamp.addOp(XformOp(kMatrixOperation, kMatrixHint), identmat)
    esamp.addOp(scaleop, V3d(1.0, 1.0, 1.0))
    e.getSchema().set(esamp)

    fsamp = XformSample()
    fsamp.addOp(transop, V3d(3.0, -4.0, 5.0))
    f.getSchema().set(fsamp)

    # this will cause the Xform's values property to be an ArrayProperty
    # since there will be 20 * 16 channels.
    gsamp = XformSample()
    gmatrix = M44d()
    gmatrix.makeIdentity()
    for i in range(20):
        gmatrix[0][1] = float(i)
        gsamp.addOp(matrixop, gmatrix)
    g.getSchema().set(gsamp)

def xformIn():
    """read in an iarchive with an xform and check vals"""

    iarch = IArchive('xform1.abc')

    identity = M44d()
    xs = XformSample()

    a = IXform(iarch.getTop(), 'a')

    assert a.getSchema().getNumSamples() == 20
    assert a.getSchema().getNumOps() == 1

    for i in range(20):
        xs = a.getSchema().getValue(ISampleSelector(i))
        assert xs.getNumOps() == 1
        assert xs[0].isTranslateOp()
        assert xs[0].isYAnimated()
        assert xs[0].isXAnimated() == False
        assert xs[0].isZAnimated() == False
        assert xs.getTranslation() == V3d(12.0, i+42.0, 20.0)
        assert xs.getMatrix() == M44d().setTranslation(
                V3d(12.0, i+42.0, 20.0))

    b = IXform(a, 'b')
    xs = b.getSchema().getValue()
    assert b.getSchema().getTimeSampling().getTimeSamplingType().isUniform()
    # the schema is not static, because set() was called 20 times on it
    assert not b.getSchema().isConstant()
    assert b.getSchema().getNumSamples() == 20
    assert xs.getNumOps() == 0
    assert b.getSchema().getNumOps() == 0
    assert xs.getMatrix() == identity

    c = IXform(b, 'c')
    xs = c.getSchema().getValue()
    assert xs.getNumOps() == 0 
    assert c.getSchema().getNumOps() == 0 
    assert xs.getMatrix() == identity 
    assert c.getSchema().getInheritsXforms() 
    assert c.getSchema().isConstantIdentity() 

    d = IXform(c, 'd') 
    xs = d.getSchema().getValue()
    assert xs.getNumOps() == 1 
    assert d.getSchema().getNumOps() == 1 
    assert xs[0].isScaleOp() 
    assert not (xs[0].isXAnimated() or xs[0].isYAnimated()
                        or xs[0].isZAnimated())
    assert xs.getMatrix() == M44d().setScale(V3d(3.0, 6.0, 9.0)) 
    assert d.getSchema().getInheritsXforms() 

    e = IXform(d, 'e')
    assert e.getSchema().isConstantIdentity() 
    assert e.getSchema().isConstant() 
    assert e.getSchema().getNumOps() == 3 

    f = IXform(e, 'f')
    assert f.getSchema().isConstant()
    assert not f.getSchema().isConstantIdentity()

    g = IXform(f, 'g')
    gmatrix = M44d()
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

