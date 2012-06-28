##-*****************************************************************************
##
## Copyright (c) 2009-2012,
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

from maya import cmds as MayaCmds
import os
import unittest
import util

def checkEqualRotate(test, nodeName1, nodeName2, precision):
    test.failUnlessAlmostEqual(MayaCmds.getAttr(nodeName1 + '.rotateX'),
        MayaCmds.getAttr(nodeName2 + '.rotateX'), precision)
    test.failUnlessAlmostEqual(MayaCmds.getAttr(nodeName1 + '.rotateY'),
        MayaCmds.getAttr(nodeName2 + '.rotateY'), precision)
    test.failUnlessAlmostEqual(MayaCmds.getAttr(nodeName1 + '.rotateZ'),
        MayaCmds.getAttr(nodeName2 + '.rotateZ'), precision)

def checkEqualTranslate(test, nodeName1, nodeName2, precision):
    test.failUnlessAlmostEqual(MayaCmds.getAttr(nodeName1 + '.translateX'),
        MayaCmds.getAttr(nodeName2 + '.translateX'), precision)
    test.failUnlessAlmostEqual(MayaCmds.getAttr(nodeName1 + '.translateY'),
        MayaCmds.getAttr(nodeName2 + '.translateY'), precision)
    test.failUnlessAlmostEqual(MayaCmds.getAttr(nodeName1+'.translateZ'),
        MayaCmds.getAttr(nodeName2 + '.translateZ'), precision)

def createStaticSolarSystem():
    MayaCmds.file(new=True, force=True)

    moon = MayaCmds.polySphere( radius=0.5, name="moon" )[0]
    MayaCmds.move( -5, 0.0, 0.0, r=1 )
    earth  = MayaCmds.polySphere( radius=2, name="earth" )[0]
    MayaCmds.select( moon, earth )
    MayaCmds.group(name='group1')

    MayaCmds.polySphere( radius=5, name="sun" )[0]
    MayaCmds.move( 25, 0.0, 0.0, r=1 )
    MayaCmds.group(name='group2')

def createAnimatedSolarSystem():
    MayaCmds.file(new=True, force=True)

    moon = MayaCmds.polySphere(radius=0.5, name="moon")[0]
    MayaCmds.move(-5, 0.0, 0.0, r=1)
    earth  = MayaCmds.polySphere(radius=2, name="earth")[0]
    MayaCmds.select(moon, earth)
    MayaCmds.group(name='group1')

    MayaCmds.polySphere(radius=5, name="sun")[0]
    MayaCmds.move(25, 0.0, 0.0, r=1)
    MayaCmds.group(name='group2')

    # the sun's simplified self-rotation
    MayaCmds.currentTime(1, update=True)
    MayaCmds.setKeyframe('sun', at='rotateY', v=0)
    MayaCmds.currentTime(240, update=True)
    MayaCmds.setKeyframe('sun', at='rotateY', v=360)

    # the earth rotate around the sun
    MayaCmds.expression(
        s='$angle = (frame-91)/180*3.1415;\n\
        earth.translateX = 25+25*sin($angle)')
    MayaCmds.expression(
        s='$angle = (frame-91)/180*3.1415;\n\
        earth.translateZ = 25*cos($angle)')

    # the moon rotate around the earth
    MayaCmds.expression(
        s='$angle = (frame-91)/180*3.1415+frame;\n\
        moon.translateX = earth.translateX+5*sin($angle)')
    MayaCmds.expression(
        s='$angle = (frame-91)/180*3.1415+frame;\n\
        moon.translateZ = earth.translateZ+5*cos($angle)')

class AbcImportSwapTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

        # write out an animated Alembic file
        createAnimatedSolarSystem()
        self.__files.append(util.expandFileName('testAnimatedSolarSystem.abc'))
        MayaCmds.AbcExport(j='-fr 1 24 -root group1 -root group2 -file ' + self.__files[-1])

        # write out a static Alembic file that's different than the static scene
        # created by createStaticSolarSystem()
        MayaCmds.currentTime(12, update=True)
        self.__files.append(util.expandFileName('testStaticSolarSystem.abc'))
        MayaCmds.AbcExport(j='-fr 12 12 -root group1 -root group2 -file ' + self.__files[-1])

        # write out an animated mesh with animated parent transform node
        MayaCmds.polyPlane(sx=2, sy=2, w=1, h=1, ch=0, n='polyMesh')
        MayaCmds.createNode('transform', n='group')
        MayaCmds.parent('polyMesh', 'group')

        # key the transform node
        MayaCmds.setKeyframe('group', attribute='translate', t=[1, 4])
        MayaCmds.move(0.36, 0.72, 0.36)
        MayaCmds.setKeyframe('group', attribute='translate', t=2)

        #key the mesh node
        MayaCmds.select('polyMesh.vtx[0:8]')
        MayaCmds.setKeyframe(t=[1, 4])
        MayaCmds.scale(0.1, 0.1, 0.1, r=True)
        MayaCmds.setKeyframe(t=2)
        self.__files.append(util.expandFileName('testAnimatedMesh.abc'))
        MayaCmds.AbcExport(j='-fr 1 4 -root group -file ' + self.__files[-1])

        MayaCmds.file(new=True, force=True)

    def tearDown(self):
        for f in self.__files :
            os.remove(f)

    def testStaticSceneSwapInStaticAlembicTransform(self):
        createStaticSolarSystem()
        MayaCmds.AbcImport(self.__files[1], connect='/', debug=False )

        # check the swapped scene is the same as frame #12
        # tranform node moon
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateX'), -4.1942, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateY'),  0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateZ'),  2.9429, 4)
        # transform node earth
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateX'), 0.4595, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateY'), 0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateZ'), 4.7712, 4)
        # transform node sun
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateX'),  0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateY'), 16.569, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateZ'),  0.0000, 4)

    def testStaticSceneSwapInAnimatedAbcTransform(self):
        createStaticSolarSystem()
        # swap in the animated hierarchy
        MayaCmds.AbcImport(self.__files[0], connect='/', debug=False)

        # this is loaded in for value comparison purpose only
        MayaCmds.AbcImport(self.__files[0], mode='import')

        # check the swapped scene at every frame
        for frame in range(1, 25):
            MayaCmds.currentTime(frame, update=True)
            # tranform node moon
            checkEqualTranslate(self, 'group1|moon', 'group3|moon', 4)
            # transform node earth
            checkEqualTranslate(self, 'group1|earth', 'group3|earth', 4)
            # transform node sun
            checkEqualRotate(self, 'group2|sun', 'group4|sun', 4)

    def testAnimatedSceneSwapInStaticAbcTransform(self):
        createAnimatedSolarSystem()
        MayaCmds.AbcImport(self.__files[1], connect='/')

        # check the swapped scene is the same as frame #12
        # tranform node moon
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateX'), -4.1942, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateY'), 0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateZ'), 2.9429, 4)
        # transform node earth
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateX'), 0.4595, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateY'), 0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateZ'), 4.7712, 4)
        # transform node sun
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateX'), 0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateY'), 16.569, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateZ'), 0.0000, 4)

    def testRemoveIfNoUpdate(self):
        createStaticSolarSystem()
        # add a few nodes that don't exist in the Alembic file
        MayaCmds.createNode('transform', name='saturn')
        MayaCmds.parent('saturn', 'group1')
        MayaCmds.createNode('transform', name='venus')
        MayaCmds.parent('venus', 'group2')
        MayaCmds.AbcImport(self.__files[1], connect='/',
            removeIfNoUpdate=True)

        # check if venus and saturn is deleted
        self.failUnlessEqual(MayaCmds.objExists('venus'), False)
        self.failUnlessEqual(MayaCmds.objExists('saturn'), False)

        # check the swapped scene is the same as frame #12
        # tranform node moon
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateX'), -4.1942, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateY'),  0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateZ'),  2.9429, 4)
        # transform node earth
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateX'), 0.4595, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateY'), 0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateZ'), 4.7712, 4)
        # transform node sun
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateX'),  0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateY'), 16.569, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateZ'),  0.0000, 4)

    def testRemoveIfNoUpdateLessGeo(self):
        createStaticSolarSystem()

        # delete earth so that only the sun and the moon should exist
        MayaCmds.delete('earth')

        MayaCmds.AbcImport(self.__files[1], connect='/',
            removeIfNoUpdate=True)

        # check to make sure earth still does not exist
        self.failUnlessEqual(MayaCmds.objExists('earth'), False)

        # check the swapped scene is the same as frame #12
        # tranform node moon
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateX'), -4.1942, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateY'),  0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateZ'),  2.9429, 4)
        # transform node sun
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateX'),  0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateY'), 16.569, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateZ'),  0.0000, 4)

    def testCreateIfNotFound(self):
        createStaticSolarSystem()
        # delete some nodes
        MayaCmds.delete( 'sunShape' )
        MayaCmds.delete( 'moon' )
        MayaCmds.AbcImport(self.__files[1], connect='/',
            createIfNotFound=True)

        # check the swapped scene is the same as frame #12
        # tranform node moon
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateX'), -4.1942, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateY'),  0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateZ'),  2.9429, 4)
        # transform node earth
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateX'), 0.4595, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateY'), 0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateZ'), 4.7712, 4)
        # transform node sun
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateX'),  0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateY'), 16.569, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateZ'),  0.0000, 4)

    def testPartialSwap(self):
        createStaticSolarSystem()
        MayaCmds.AbcImport(self.__files[1], connect='group1',
            createIfNotFound=True)

        # check the swapped scene is the same as frame #12
        # tranform node moon
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateX'), -4.1942, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateY'),  0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('moon.translateZ'),  2.9429, 4)
        # transform node earth
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateX'), 0.4595, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateY'), 0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('earth.translateZ'), 4.7712, 4)
        # transform node sun
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateX'),  0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateY'), 0.0000, 4)
        self.failUnlessAlmostEqual(
            MayaCmds.getAttr('sun.rotateZ'),  0.0000, 4)

    def testStaticMeshSwap(self):
        moon = MayaCmds.polyCube( sx=1, name="moon", ch=False )[0]
        MayaCmds.move( -2, 0.0, 0.0, r=1 )
        earth  = MayaCmds.polyCube( sx=1, name="earth", ch=False )[0]
        MayaCmds.select( moon, earth )
        MayaCmds.group(name='group1')

        MayaCmds.polyCube( sx=1, name="sun", ch=False )[0]
        MayaCmds.move( 10, 0.0, 0.0, r=1 )
        MayaCmds.group(name='group2')
        MayaCmds.AbcImport(self.__files[1], connect='/', debug=False )

        self.failUnless(len(MayaCmds.ls(type='mesh')) == 3)
        self.failUnless(MayaCmds.polyEvaluate('sunShape', face=True) == 400)
        self.failUnless(MayaCmds.polyEvaluate('earthShape', face=True) == 400)
        self.failUnless(MayaCmds.polyEvaluate('moonShape', face=True) == 400)

    def testAnimatedMeshSwap(self):

        MayaCmds.polyPlane( sx=2, sy=2, w=1, h=1, ch=0, n='polyMesh')
        MayaCmds.createNode('transform', n='group')
        MayaCmds.parent('polyMesh', 'group')
        MayaCmds.AbcImport(self.__files[2], connect='group')

        # this is loaded in for value comparison purpose only
        MayaCmds.AbcImport(self.__files[2], mode='import')

        # check the swapped scene at every frame
        for frame in range(1, 4):
            MayaCmds.currentTime(frame, update=True)
            # tranform node group
            checkEqualTranslate(self, 'group', 'group1', 4)
            # tranform node group
            checkEqualTranslate(self, 'group|polyMesh', 'group1|polyMesh', 4)
            # mesh node polyMesh
            for index in range(0, 9):
                string1 = 'group|polyMesh.vt[%d]' % index
                string2 = 'group1|polyMesh.vt[%d]' % index
                self.failUnlessAlmostEqual(
                    MayaCmds.getAttr(string1)[0][0],
                    MayaCmds.getAttr(string2)[0][0],
                    4, '%s.x != %s.x' % (string1, string2))
                self.failUnlessAlmostEqual(
                    MayaCmds.getAttr(string1)[0][1],
                    MayaCmds.getAttr(string2)[0][1],
                    4, '%s.y != %s.y' % (string1, string2))
                self.failUnlessAlmostEqual(
                    MayaCmds.getAttr(string1)[0][2],
                    MayaCmds.getAttr(string2)[0][2],
                    4, '%s.z != %s.z' % (string1, string2))

    def testMeshTopoChange(self):

        MayaCmds.polySphere( sx=10, sy=15, r=0, n='polyMesh')
        MayaCmds.createNode('transform', n='group')
        MayaCmds.parent('polyMesh', 'group')
        MayaCmds.AbcImport(self.__files[2], connect='group')

        # this is loaded in for value comparison purpose only
        MayaCmds.AbcImport(self.__files[2], mode='import')

        # check the swapped scene at every frame
        for frame in range(1, 4):
            MayaCmds.currentTime(frame, update=True)
            # tranform node group
            checkEqualTranslate(self, 'group', 'group1', 4)
            # tranform node group
            checkEqualTranslate(self, 'group|polyMesh', 'group1|polyMesh', 4)
            # mesh node polyMesh
            for index in range(0, 9):
                string1 = 'group|polyMesh.vt[%d]' % index
                string2 = 'group1|polyMesh.vt[%d]' % index
                self.failUnlessAlmostEqual(
                    MayaCmds.getAttr(string1)[0][0],
                    MayaCmds.getAttr(string2)[0][0],
                    4, '%s.x != %s.x' % (string1, string2))
                self.failUnlessAlmostEqual(
                    MayaCmds.getAttr(string1)[0][1],
                    MayaCmds.getAttr(string2)[0][1],
                    4, '%s.y != %s.y' % (string1, string2))
                self.failUnlessAlmostEqual(
                    MayaCmds.getAttr(string1)[0][2],
                    MayaCmds.getAttr(string2)[0][2],
                    4, '%s.z != %s.z' % (string1, string2))
