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
from imath import *
from alembic.Abc import *
from alembic.AbcCoreAbstract import *
from alembic.AbcCollection import *

class CollectionTest(unittest.TestCase):
    def testCollectionExport(self):
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
        self.assertRaises(RuntimeError, group2.getSchema().createCollection, "/slashy")

        prop = group.getSchema().createCollection("prop")
        prop.setValue(strVec)

        self.assertEquals(group.getSchema().getNumCollections(), 1)

        # TODO passing in time sampling here causes a segfault, most likely because
        # of how createCollection currently takes an Argument &, see #274
        cool = group.getSchema().createCollection("cool", md)
        cool.setTimeSampling(ts)

        self.assertEquals(
            group.getSchema().getCollection(1).getMetaData().get("coupe"),
            "de ville" )

        strVec = StringArray(2)
        strVec[0] = "/foo"
        strVec[1] = "/bar"
        group.getSchema().getCollection(1).setValue(strVec)

        strVec = StringArray(1)
        strVec[0] = "potato"
        group.getSchema().getCollection("cool").setValue(strVec)

    def testCollectionImport(self):
        """test ICollections Python bindings"""

        archive = IArchive("Collection.abc")

        test = IObject(archive.getTop(), "test")
        group = ICollections(test, "Group1")
        group2 = ICollections(test, "Group2")

        self.assertEqual(group.getSchema().getNumCollections(), 2)
        self.assertEqual(group2.getSchema().getNumCollections(), 0)

        self.assertFalse(group2.getSchema().getCollection(45).valid())
        self.assertFalse(group2.getSchema().getCollection("potato").valid())

        prop = group.getSchema().getCollection("prop")
        prop2 = group.getSchema().getCollection("cool")
        self.assertEqual(prop2.getMetaData().get("coupe"), "de ville")
        self.assertEqual(archive.getTimeSampling(1), prop2.getTimeSampling())
        self.assertEqual(prop2.getNumSamples(), 2)
        samp = prop.getValue(0)
        self.assertEqual(len(samp), 3)
        self.assertEqual(samp[0], "/a/b/c/1")
        self.assertEqual(samp[1], "/a/b/c/2")
        self.assertEqual(samp[2], "/a/b/c/3")
