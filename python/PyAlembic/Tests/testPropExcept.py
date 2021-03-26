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

import unittest
import os
import alembic

class PropExceptTest(unittest.TestCase):
    """This tests for exceptions being thrown by PyAlembic when an invalid property
    index or name is passed into getProperty on either an IObject or an OObject."""

    def testExport(self):
        """
        Tests exception raising for invalid property indices and names
        on the write side.
        """
        a = alembic.Abc.OArchive("testPropException.abc")
        t = a.getTop()
        x = alembic.AbcGeom.OXform(t, "myxform")
        p = alembic.Abc.OStringProperty(x.getProperties(), "myprop")
        test = x.getProperties().getProperty("myprop")
        self.assertEqual(test.getName(), "myprop")
        test = x.getProperties().getProperty(1)
        self.assertEqual(test.getName(), "myprop")

        self.assertRaises(KeyError, x.getProperties().getProperty, "notfound")
        self.assertRaises(IndexError, x.getProperties().getProperty, 99)

    def testImport(self):
        """
        Tests exception raising for invalid property indices and names
        on the read side.
        """
        a = alembic.Abc.IArchive("testPropException.abc")
        t = a.getTop()
        props = t.children[0].getProperties()
        p = props.getProperty("myprop")
        self.assertEqual(p.getName(), "myprop")

        self.failUnlessRaises(KeyError, props.getProperty, "notfound")
        self.assertRaises(IndexError, props.getProperty, 99)
