#!/usr/bin/env mayapy
##-*****************************************************************************
##
## Copyright (c) 2009-2011,
##  Sony Pictures Imageworks, Inc. and
##  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
##
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are
## met:
## *       Redistributions of source code must retain the above copyright
## notice, this list of conditions and the following disclaimer.
## *       Redistributions in binary form must reproduce the above
## copyright notice, this list of conditions and the following disclaimer
## in the documentation and/or other materials provided with the
## distribution.
## *       Neither the name of Sony Pictures Imageworks, nor
## Industrial Light & Magic nor the names of their contributors may be used
## to endorse or promote products derived from this software without specific
## prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
## OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##-*****************************************************************************

# run this via mayapy
# runs the test suite

import glob
import os
import sys
import time
import unittest

import maya.standalone
maya.standalone.initialize(name='python')

from maya import cmds as MayaCmds
from maya.mel import eval as MelEval

usage = """
Usage:
mayapy RunTests.py AbcExport AbcImport AbcStitcher [tests.py]

Where:
AbcExport is the location of the AbcExport Maya plugin to load.
AbcImport is the location of the AbcImport Maya plugin to load.
AbcStitcher is the location of the AbcStitcher command to use.

If no specific python tests are specified, all python files named *_test.py
in the same directory as this file are used.
"""

if len(sys.argv) < 4:
    raise RuntimeError(usage)


MayaCmds.loadPlugin(sys.argv[1])
print 'LOADED', sys.argv[1]


MayaCmds.loadPlugin(sys.argv[2])
print 'LOADED', sys.argv[2]

if not os.path.exists(sys.argv[3]):
  raise RuntimeError (sys.argv[3] + ' does not exist')
else:
  os.environ['AbcStitcher'] = sys.argv[3]

suite = unittest.TestSuite()

main_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir( main_dir )
MelEval('chdir "%s"' % main_dir )

# For now, hack the path (so the import below works)
sys.path.append("..")

# Load all the tests specified by the command line or *_test.py
testFiles = sys.argv[4:]
if not testFiles:
    testFiles = glob.glob('*_test.py')

for file in testFiles:
    name = os.path.splitext(file)[0]
    __import__(name)
    test = unittest.defaultTestLoader.loadTestsFromName(name)
    suite.addTest(test)


# Run the tests
runner = unittest.TextTestRunner(verbosity=2)
runner.run(suite)
