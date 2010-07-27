
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

from maya import cmds as MayaCmds
import os
import unittest

class AnimPointPrimitiveTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []


    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testAnimPointPrimitiveReadWrite(self):

        # Creates a point emitter.
        MayaCmds.emitter(dx=1, dy=0, dz=0, sp=0.33, pos=(1, 1, 1),
            n='myEmitter')

        MayaCmds.particle(n='emittedParticles')
        MayaCmds.setAttr('emittedParticles.lfm', 2)
        MayaCmds.setAttr('emittedParticles.lifespan', 50)
        MayaCmds.setAttr('emittedParticles.lifespanRandom', 2)
        MayaCmds.connectDynamic('emittedParticles', em='myEmitter')

        self.__files.append('/tmp/testAnimParticleReadWrite.hdf')
        MayaCmds.AlembicTakoExport('range 1 24 emittedParticles ' + self.__files[-1])

        # reading test
        MayaCmds.AlembicTakoImport(self.__files[-1], mode='open', timeRange=[1, 24])

        # reading animated point clouds into Maya aren't fully supported
        # yet which is why we don't have any checks on the data here