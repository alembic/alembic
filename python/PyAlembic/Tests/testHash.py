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
from alembic.AbcGeom import *

kTranslateOperation = XformOperationType.kTranslateOperation
kTranslateHint = 0

class HashTest(unittest.TestCase):

    def writeHierarchy(self, filename):
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

    def testHash(self):
        filename = 'hash.abc'
        self.writeHierarchy(filename)

        iarch = IArchive(filename)

        a = IObject(iarch.getTop(), 'a')
        b = a.getChild('b')
        c = a.getChild('c')
        d = b.getChild('d')
        e = b.getChild('e')
        f = c.getChild('f')
        g = c.getChild('g')

        self.assertTrue(a.valid())
        self.assertTrue(b.valid())
        self.assertTrue(c.valid())
        self.assertTrue(d.valid())
        self.assertTrue(e.valid())
        self.assertTrue(f.valid())
        self.assertTrue(g.valid())

        self.assertNotEqual(a.getPropertiesHash(), "")
        self.assertNotEqual(a.getChildrenHash(), "")
        self.assertNotEqual(b.getPropertiesHash(), "")
        self.assertNotEqual(b.getChildrenHash(), "")
        self.assertNotEqual(c.getPropertiesHash(), "")
        self.assertNotEqual(c.getChildrenHash(), "")
        self.assertNotEqual(d.getPropertiesHash(), "")
        self.assertNotEqual(d.getChildrenHash(), "")
        self.assertNotEqual(e.getPropertiesHash(), "")
        self.assertNotEqual(e.getChildrenHash(), "")
        self.assertNotEqual(f.getPropertiesHash(), "")
        self.assertNotEqual(f.getChildrenHash(), "")
        self.assertNotEqual(g.getPropertiesHash(), "")
        self.assertNotEqual(g.getChildrenHash(), "")

        self.assertNotEqual(b.getPropertiesHash(), d.getPropertiesHash())
        self.assertEqual(e.getPropertiesHash(), g.getPropertiesHash())

