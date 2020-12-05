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

import unittest
from imath import *
from alembic.Abc import *

class InstanceTest(unittest.TestCase):
    def testInstanceExport(self):
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

    def testInstanceImport(self):
        """Read an archive with an instance in it, verify it's instancing correctly."""

        iarch = IArchive('instance.abc')

        a = IObject(iarch.getTop(), 'a')

        self.assertEqual(a.getNumChildren(), 2)

        self.assertEqual(a.getChild('c').getName(), 'c')
        self.assertEqual(a.getChild('c').getName(), a.getChild(1).getName())
        self.assertTrue(a.isChildInstance('c'))
        self.assertTrue(a.isChildInstance(1))

        b = a.getChild('b')
        self.assertTrue(b.valid())

        # c is an instance root pointing at b
        c = a.getChild('c')
        self.assertTrue(c.valid())
        self.assertTrue(c.isInstanceRoot())
        self.assertTrue(c.isInstanceDescendant())
        self.assertEqual(c.instanceSourcePath(), b.getFullName())

        # instanced child of c is d
        di = c.getChild('d')
        self.assertTrue(di.valid())
        self.assertEqual(di.getFullName(), '/a/c/d')
        self.assertEqual(di.getParent().getFullName(), c.getFullName())
        self.assertFalse(di.isInstanceRoot())
        self.assertTrue(di.isInstanceDescendant())

        # instanced child of d is e
        ei = di.getChild('e')
        self.assertTrue(ei.valid())
        self.assertTrue(ei.getFullName(), '/a/c/d/e')
        self.assertTrue(ei.getParent().getFullName(), di.getFullName())
        self.assertFalse(ei.isInstanceRoot())
        self.assertTrue(ei.isInstanceDescendant())

