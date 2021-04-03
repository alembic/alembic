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

import imath
import alembic
import traceback
import unittest
class IteratorTest(unittest.TestCase):
    def testArchiveExport(self):
        """write out an archive with an object tree"""

        archive = alembic.Abc.OArchive("iterator.abc")
        for i in range(3):
            child = alembic.Abc.OObject(archive.getTop(), "childObj" + str(i))
            for j in range(3):
                gchild = alembic.Abc.OObject(child, "grandChild" + str(j))
                for k in range(3):
                    cp = alembic.Abc.OCompoundProperty(gchild.getProperties(), "prop" + str(k))
                    sp = alembic.Abc.OStringProperty(cp, "scalar")
                    sp.setValue("a")
                    sp.setValue("b")
                    sp.setValue("c")
                    ap = alembic.Abc.OStringArrayProperty(cp, "array")
                    stra = imath.StringArray(3)
                    stra[0] = 'a'
                    stra[1] = 'b'
                    stra[2] = 'c'
                    ap.setValue(stra)
                    strb = imath.StringArray(2)
                    strb[0] = 'd'
                    strb[1] = 'e'
                    ap.setValue(strb)
                    strc = imath.StringArray(1)
                    strc[0] = 'f'
                    ap.setValue(strc)



    def testArchiveImport(self):
        """read in archive with an object tree"""

        archive = alembic.Abc.IArchive("iterator.abc")
        top = archive.getTop()

        # lets check the iterators
        self.assertEqual(len(top.children), 3)

        curI = 0
        for i in top.children:
            self.assertEqual(len(i.children), 3)
            self.assertEqual(i.getName(), 'childObj' + str(curI))
            curI += 1

            curJ = 0
            for j in i.children:
                self.assertEqual(j.getName(), "grandChild" + str(curJ))
                curJ += 1
                self.assertEqual(len(j.getProperties().propertyheaders), 3)
                curK = 0
                for k in j.getProperties().propertyheaders:
                    self.assertEqual(k.getName(), 'prop' + str(curK))
                    cp = alembic.Abc.ICompoundProperty(j.getProperties(), 'prop' + str(curK))
                    curK += 1

                    sp = alembic.Abc.IStringProperty(cp, 'scalar')
                    samp = sp.samples
                    self.assertEqual(len(samp), 3)
                    self.assertEqual(samp[0], "a")
                    self.assertEqual(samp[1], "b")
                    self.assertEqual(samp[2], "c")

                    ap = alembic.Abc.IStringArrayProperty(cp, 'array')
                    samp = ap.samples
                    self.assertEqual(len(samp), 3)
                    self.assertEqual(len(samp[0]), 3)
                    self.assertEqual(len(samp[1]), 2)
                    self.assertEqual(len(samp[2]), 1)
                    self.assertEqual(samp[0][0], 'a')
                    self.assertEqual(samp[0][1], 'b')
                    self.assertEqual(samp[0][2], 'c')
                    self.assertEqual(samp[1][0], 'd')
                    self.assertEqual(samp[1][1], 'e')
                    self.assertEqual(samp[2][0], 'f')