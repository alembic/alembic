#-******************************************************************************
#
# Copyright (c) 2014,
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

import os
import unittest
import tempfile

from abcview.io import idict, Session

# temporary directory for holding test data
TEMPDIR = tempfile.mkdtemp()

class Test1_iDict(unittest.TestCase):
    def test_basic(self):

        p = idict()
        self.assertEqual(p.local, {})
        self.assertEqual(p.inherited, {})
        
        # update the (local) values
        p.update(x=1, y=2, z=3)
        self.assertEqual(p, p.local)
        self.assertNotEqual(p, p.inherited)
        self.assertEqual(len(p), 3)
        self.assertEqual(len(p.local), 3)
        self.assertEqual(len(p.inherited), 0)

        # update inherited values
        p.inherited.update(a=1, b=2, c=3)
        self.assertNotEqual(p, p.local)
        self.assertNotEqual(p, p.inherited)
        self.assertEqual(len(p), 6)
        self.assertEqual(len(p.local), 3)
        self.assertEqual(len(p.inherited), 3)
        
        # perform some updates in place
        p["a"] = 100
        self.assertEqual(p.get("a"), 100)
        self.assertEqual(p.local.get("a"), 100)

        # local now has an override
        self.assertEqual(len(p), 6)
        self.assertEqual(len(p.local), 4)
        self.assertEqual(len(p.inherited), 3)

    def test_init_args(self):
        p = idict({"a": 1, "b":2})
        self.assertEqual(len(p), 2)
        self.assertEqual(len(p.local), 2)
        self.assertEqual(len(p.inherited), 0)

        p.inherited.update(a=2, b=3, c=4)
        self.assertEqual(len(p), 3)
        self.assertEqual(len(p.local), 2)
        self.assertEqual(len(p.inherited), 3)

        self.assertEqual(p.get("a"), 1)
        self.assertEqual(p.get("b"), 2)
        self.assertEqual(p.get("c"), 4)

    def test_init_kwargs(self):
        p = idict(a=1, b=2, c=3)

        self.assertEqual(len(p), 3)
        self.assertEqual(len(p.local), 3)
        self.assertEqual(len(p.inherited), 0)

        self.assertEqual(p.get("a"), 1)
        self.assertEqual(p.get("b"), 2)
        self.assertEqual(p.get("c"), 3)
    
class Test2_Session(unittest.TestCase):
    def test_basic(self):
        s = Session()

        # local
        s.properties.update(a=1, b="foo")

        self.assertEqual(len(s.properties), 2)
        self.assertEqual(len(s.properties.local), 2)
        self.assertEqual(len(s.properties.inherited), 0)
        self.assertEqual(s.properties.get("a"), 1)
        self.assertEqual(s.properties.get("b"), "foo")

        # inherited
        s.properties.inherited.update(a=2, b="bar", c=3)

        self.assertEqual(len(s.properties), 3)
        self.assertEqual(len(s.properties.local), 2)
        self.assertEqual(len(s.properties.inherited), 3)
        self.assertEqual(s.properties.get("a"), 1)
        self.assertEqual(s.properties.get("b"), "foo")
        self.assertEqual(s.properties.get("c"), 3)

    def test_nested(self):
        s1 = Session()
        s2 = Session()

        # create some properties and save
        s1.properties.update(a=1, b="foo")
        s1.save(os.path.join(TEMPDIR, "s1.io"))

        # add this session to s2
        s2.add_item(s1)

        # override one of the properties in s1
        s2.items[0].properties.update(b="bar")

        self.assertEqual(s2.items[0].properties.get("a"), 1)
        self.assertEqual(s2.items[0].properties.get("b"), "bar")

        # save
        s2.save(os.path.join(TEMPDIR, "s2.io"))

        # read it back
        s3 = Session(os.path.join(TEMPDIR, "s2.io"))
        self.assertEqual(s3.items[0].properties.get("a"), 1)
        self.assertEqual(s3.items[0].properties.get("b"), "bar")

    def test_updates(self):
        s1 = Session()
        s2 = Session()

        # create some properties and save
        s1.properties.update(a=1, b="foo")

        # add this session to s2
        s2.add_item(s1)

        self.assertEqual(s1.properties["a"], 1)
        self.assertEqual(s2.items[0].properties["a"], 1)
        self.assertEqual(s1.properties["b"], "foo")
        self.assertEqual(s2.items[0].properties["b"], "foo")

        # update a property on s1
        s1.properties.update(b="bar")

        self.assertEqual(s1.properties.get("b"), "bar")
        self.assertEqual(s2.items[0].properties.get("b"), "bar")

if __name__ == "__main__":
    unittest.main()
