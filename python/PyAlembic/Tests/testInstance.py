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

testList = []

def instanceOut():
    """Write an oarchive with an instance in it"""

    oarch = OArchive('instance.abc')

    #
    #     a
    #    / \
    #   b   c   <-- c is an instance of b
    #   |
    #   d
    #   |
    #   e
    #

    a = OObject(oarch.getTop(), 'a')
    b = OObject(a, 'b')
    d = OObject(b, 'd')
    e = OObject(d, 'e')

    a.addChildInstance(b, 'c')

testList.append(('instanceOut', instanceOut))

def instanceIn():
    """Read an archive with an instance in it, verify it's instancing correctly."""

    iarch = IArchive('instance.abc')

    a = IObject(iarch.getTop(), 'a')

    assert a.getNumChildren() == 2

    assert a.getChild('c').getName() == 'c'
    assert a.getChild('c').getName() == a.getChild(1).getName()
    assert a.isChildInstance('c')
    assert a.isChildInstance(1)

    b = a.getChild('b')
    assert b.valid()

    # c is an instance root pointing at b
    c = a.getChild('c')
    assert c.valid()
    assert c.isInstanceRoot()
    assert c.isInstanceDescendant()
    assert c.instanceSourcePath() == b.getFullName()

    # instanced child of c is d
    di = c.getChild('d')
    assert di.valid()
    assert di.getFullName() == '/a/c/d'
    assert di.getParent().getFullName() == c.getFullName()
    assert not di.isInstanceRoot()
    assert di.isInstanceDescendant()

    # instanced child of d is e
    ei = di.getChild('e')
    assert ei.valid()
    assert ei.getFullName() == '/a/c/d/e'
    assert ei.getParent().getFullName() == di.getFullName()
    assert not ei.isInstanceRoot()
    assert ei.isInstanceDescendant()

testList.append(('instanceIn', instanceIn))

for test in testList:
    funcName = test[0]
    print "\nRunning %s" % funcName
    test[1]()
    print "passed"

print ""
