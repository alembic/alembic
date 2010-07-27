
###############################################################################
#
# Copyright (c) 2009-2010,
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
###############################################################################

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

if os.getenv('AlembicTakoExport'):
    MayaCmds.loadPlugin(os.getenv('AlembicTakoExport'))
    print 'LOADED', os.getenv('AlembicTakoExport')
else:
    raise RuntimeError('env variable AlembicTakoExport not set')

if os.getenv('AlembicTakoImport'):
    MayaCmds.loadPlugin(os.getenv('AlembicTakoImport'))
    print 'LOADED', os.getenv('AlembicTakoImport')
else:
    raise RuntimeError('env variable AlembicTakoImport not set')

if not os.getenv('AlembicStitcher'):
    raise RunTimeError('env variable AlembicStitcher not set')
if not os.path.exists(os.environ['AlembicStitcher']):
  raise RuntimeError (os.environ['AlembicStitcher'] + 'does not exist')

# optional, this only should be overriden if the node name from AlembicTakoImport
# is changed
if not os.getenv('AlembicNodeType'):
    os.environ['AlembicNodeType'] = 'AlembicTakoCacheNode'

suite = unittest.TestSuite()

main_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir( main_dir )

# For now, hack the path (so the import below works)
sys.path.append("..")

# Load all the tests specified by the command line or *_test.py
testFiles = sys.argv[1:]
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
