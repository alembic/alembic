#-******************************************************************************
#
# Copyright (c) 2013,
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

from alembic.Abc import *
from alembic.AbcCoreAbstract import *
from alembic.AbcGeom import *

testList = []

def testReferenceOut():

    a = OArchive('testReference.abc')
    t = a.getTop()
    x = OXform(t, 'root')

    md = MetaData()
    md.setReference()

    assert md.get('reference') == '1'

    pr = OStringProperty(x.getProperties(), 'refprop', md)
    p0 = OStringProperty(x.getProperties(), 'norefprop')

    assert pr.valid()
    assert p0.valid()

testList.append(('testReferenceOut', testReferenceOut))

def testReferenceIn():

    a = IArchive('testReference.abc')
    t = a.getTop()
    x = t.getChild('root')

    props = x.getProperties()
    pr = props.getProperty('refprop')
    p0 = props.getProperty('norefprop')

    assert pr.valid()
    assert p0.valid()

    prh = pr.getHeader()
    assert prh.isReference()

    p0h = p0.getHeader()
    assert not p0h.isReference()

testList.append(('testReferenceIn', testReferenceIn))

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print "\nRunning %s" % funcName
    test[1]()
    print "passed"

print ""
