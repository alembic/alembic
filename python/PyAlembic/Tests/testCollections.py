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
from alembic.AbcCoreAbstract import *
from alembic.AbcCollection import *

testList = []

def collectionOut():
    """test OCollecions Python bindings"""

    archive = OArchive("Collection.abc")
    test = OObject(archive.getTop(), "test")

    ts = TimeSampling(1/24.0, 2.0)
    md = MetaData()
    md.set("coupe", "de ville")

    group = OCollections(test, "Group1")
    group2 = OCollections(test, "Group2")

    strVec = StringArray(3)
    strVec[0] = "/a/b/c/1"
    strVec[1] = "/a/b/c/2"
    strVec[2] = "/a/b/c/3"

    prop = OStringArrayProperty()

    # this should throw an execption b/c collection name is bad
    try:
        prop = group2.getSchema().createCollection("/slashy")
    except:
        pass

    prop = group.getSchema().createCollection("prop")
    prop.setValue(strVec)

    assert group.getSchema().getNumCollections() == 1

    group.getSchema().createCollection("cool", md, ts)
    assert ( group.getSchema().getCollection(1).getMetaData().get("coupe")
             == "de ville" )
    strVec = StringArray(2)
    strVec[0] = "/foo"
    strVec[1] = "/bar"
    group.getSchema().getCollection(1).setValue(strVec)

    strVec = StringArray(1)
    strVec[0] = "potato"
    group.getSchema().getCollection("cool").setValue(strVec)

def collectionIn():
    """test ICollections Python bindings"""

    archive = IArchive("Collection.abc")

    test = IObject(archive.getTop(), "test")
    group = ICollections(test, "Group1")
    group2 = ICollections(test, "Group2")

    assert group.getSchema().getNumCollections() == 2
    assert group2.getSchema().getNumCollections() == 0

    assert not group2.getSchema().getCollection(45)
    assert not group2.getSchema().getCollection("potato")

    prop = group.getSchema().getCollection("prop")
    prop2 = group.getSchema().getCollection("cool")
    assert prop2.getMetaData().get("coupe") == "de ville"
    assert archive.getTimeSampling(1) == prop2.getTimeSampling()
    assert prop2.getNumSamples() == 2
    samp = prop.getValue(0)
    assert len(samp) == 3
    assert samp[0] == "/a/b/c/1"
    assert samp[1] == "/a/b/c/2"
    assert samp[2] == "/a/b/c/3"

def testCollectionsBinding():
    collectionOut()
    collectionIn()

testList.append(('testCollectionsBinding', testCollectionsBinding))

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""
