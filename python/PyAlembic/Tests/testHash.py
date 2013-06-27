#-******************************************************************************
#
# Copyright (c) 2012 - 2013
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

kTranslateOperation = XformOperationType.kTranslateOperation
kTranslateHint = 0

def writeHierarchy(filename):
    """write an oarchive with some matching hierarchies"""

    #
    # These hashes are only supported in ogawa caches.
    oarch = OArchive(filename, asOgawa = True)

    #
    #           a
    #         /   \
    #        b     c
    #       / \   / \
    #      d   e f   g
    #

    a = OXform(oarch.getTop(), 'a')
    b = OXform(a, 'b')
    c = OXform(a, 'c')
    d = OXform(b, 'd')
    e = OXform(b, 'e')
    f = OXform(c, 'f')
    g = OXform(c, 'g')

    bp = OStringProperty(b.getProperties(), "testprop")
    dp = OStringProperty(d.getProperties(), "othertestprop")

    bp.setValue("the testprop")
    dp.setValue("the othertestprop")

    transop = XformOp(kTranslateOperation, kTranslateHint)

    asamp = XformSample()
    asamp.addOp(transop, V3d(-1.0, -1.0, 0.0))

    bsamp = XformSample()
    bsamp.addOp(transop, V3d( 1.0, -1.0, 0.0))

    a.getSchema().set(asamp)
    b.getSchema().set(asamp)
    c.getSchema().set(bsamp)
    d.getSchema().set(asamp)
    e.getSchema().set(bsamp)
    f.getSchema().set(asamp)
    g.getSchema().set(bsamp)

def testHash():
    filename = 'hash.abc'
    writeHierarchy(filename)

    iarch = IArchive(filename)

    a = IObject(iarch.getTop(), 'a')
    b = a.getChild('b')
    c = a.getChild('c')
    d = b.getChild('d')
    e = b.getChild('e')
    f = c.getChild('f')
    g = c.getChild('g')

    assert a.valid()
    assert b.valid()
    assert c.valid()
    assert d.valid()
    assert e.valid()
    assert f.valid()
    assert g.valid()

    assert a.getPropertiesHash() != ""
    assert a.getChildrenHash()   != ""
    assert b.getPropertiesHash() != ""
    assert b.getChildrenHash()   != ""
    assert c.getPropertiesHash() != ""
    assert c.getChildrenHash()   != ""
    assert d.getPropertiesHash() != ""
    assert d.getChildrenHash()   != ""
    assert e.getPropertiesHash() != ""
    assert e.getChildrenHash()   != ""
    assert f.getPropertiesHash() != ""
    assert f.getChildrenHash()   != ""
    assert g.getPropertiesHash() != ""
    assert g.getChildrenHash()   != ""

    assert b.getPropertiesHash() != d.getPropertiesHash()
    assert e.getPropertiesHash() == g.getPropertiesHash()

testList.append(('testHash', testHash))

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print "\nRunning %s" % funcName
    test[1]()
    print "passed"

print ""

