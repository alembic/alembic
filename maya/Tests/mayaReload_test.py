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

from maya import cmds as MayaCmds
import maya.OpenMaya as OpenMaya
import os
import unittest

import util

class MayaReloadTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    # this test makes sure that not just the vertex positions but the
    # connection info is all correct
    def testAnimMeshReload(self):

        MayaCmds.polyCube( name = 'mesh')
        MayaCmds.setKeyframe('meshShape.vtx[0:7]', time=[1, 24])
        MayaCmds.setKeyframe('meshShape.vtx[0:7]')
        MayaCmds.currentTime(12, update=True)
        MayaCmds.select('meshShape.vtx[0:7]')
        MayaCmds.scale(5, 5, 5, r=True)
        MayaCmds.setKeyframe('meshShape.vtx[0:7]', time=[12])
        self.__files.append(util.expandFileName('testAnimMeshReadWrite.abc'))
        MayaCmds.AbcExport(j='-fr 1 24 -root mesh -f ' + self.__files[-1])

        # reading test
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        # save as a maya file
        self.__files.append(util.expandFileName('test.mb'))
        MayaCmds.file(rename=self.__files[-1])
        MayaCmds.file(save=True)

        # reload as a maya file
        MayaCmds.file(self.__files[-1], open=True)
        MayaCmds.AbcImport(self.__files[-2], mode='import')

        retVal = True
        mesh1 = '|mesh|meshShape'
        mesh2 = '|mesh1|meshShape'
        for t in range(1, 25):
            MayaCmds.currentTime(t, update=True)
            if not util.compareMesh( mesh1, mesh2 ):
                self.fail('%s and %s were not equal at frame %d' % (mesh1,
                    mesh2, t))


#-------------------------------------------------------------------------------
# The following tests each creates four animated nodes of the same data
# type, writes out to Abc file, loads back the file and deletes one node.
# Then the scene is saved as a Maya file, and load back to check if the
# reload works as expected
#-------------------------------------------------------------------------------

    def testAnimPolyDeleteReload(self):

        # create a poly cube and animate
        shapeName = 'pCube'
        MayaCmds.polyCube( name=shapeName )
        MayaCmds.move(5, 0, 0, r=True)
        MayaCmds.setKeyframe( shapeName+'.vtx[2:5]', time=[1, 24] )
        MayaCmds.currentTime( 12 )
        MayaCmds.select( shapeName+'.vtx[2:5]',replace=True )
        MayaCmds.move(0, 4, 0, r=True)
        MayaCmds.setKeyframe( shapeName+'.vtx[2:5]', time=[12] )

        # create a poly sphere and animate
        shapeName = 'pSphere'
        MayaCmds.polySphere( name=shapeName )
        MayaCmds.move(-5, 0, 0, r=True)
        MayaCmds.setKeyframe( shapeName+'.vtx[200:379]',
            shapeName+'.vtx[381]', time=[1, 24])
        MayaCmds.currentTime(12)
        MayaCmds.select( shapeName+'.vtx[200:379]',
            shapeName+'.vtx[381]',replace=True)
        MayaCmds.scale(0.5, 0.5, 0.5, relative=True)
        MayaCmds.setKeyframe( shapeName+'.vtx[200:379]',
            shapeName+'.vtx[381]', time=[12])
        MayaCmds.currentTime(1)

        # create a poly torus and animate
        shapeName = 'pTorus'
        MayaCmds.polyTorus(name=shapeName)
        MayaCmds.setKeyframe(shapeName+'.vtx[200:219]',time=[1, 24])
        MayaCmds.currentTime(12)
        MayaCmds.select(shapeName+'.vtx[200:219]',replace=True)
        MayaCmds.scale(2, 1, 2, relative=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[200:219]', time=[12])

        # create a poly cone and animate
        shapeName = 'pCone'
        MayaCmds.polyCone(name=shapeName)
        MayaCmds.move(0, 0, -5, r=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[20]', time=[1, 24])
        MayaCmds.currentTime(12)
        MayaCmds.select(shapeName+'.vtx[20]',replace=True)
        MayaCmds.move(0, 4, 0, r=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[20]', time=[12])

        # write it out to Abc file and load back in
        self.__files.append(util.expandFileName('testPolyReload.abc'))
        MayaCmds.AbcExport(j='-fr 1 24 -root pCube -root pSphere -root pTorus -root pCone -file %s' %
            self.__files[-1])

        # load back the Abc file, delete the sphere and save to a maya file
        MayaCmds.AbcImport( self.__files[-1], mode='open')
        MayaCmds.delete('pSphere')
        self.__files.append(util.expandFileName('test.mb'))
        MayaCmds.file(rename=self.__files[-1])
        MayaCmds.file(save=True)

        # import the saved maya file to compare with the original scene
        MayaCmds.file(self.__files[-1], open=True)
        MayaCmds.select('pCube', 'pTorus', 'pCone', replace=True)
        MayaCmds.group(name='ReloadGrp')
        MayaCmds.AbcImport(self.__files[-2], mode='import')

        shapeList = MayaCmds.ls(type='mesh')
        self.failUnlessEqual(len(shapeList), 7)

        meshes = [('|pCube|pCubeShape', '|ReloadGrp|pCube|pCubeShape'),
            ('|pTorus|pTorusShape', '|ReloadGrp|pTorus|pTorusShape'),
            ('|pCone|pConeShape', '|ReloadGrp|pCone|pConeShape')]
        for m in meshes:
            for t in range(1, 25):
                MayaCmds.currentTime(t, update=True)
                if not util.compareMesh(m[0], m[1]):
                    self.fail('%s and %s are not the same at frame %d' %
                        (m[0], m[1], t))

    def testAnimSubDDeleteReload(self):

        # create a subD cube and animate
        shapeName = 'cube'
        MayaCmds.polyCube( name=shapeName )
        MayaCmds.select('cubeShape')
        MayaCmds.addAttr(longName='SubDivisionMesh', attributeType='bool',
            defaultValue=True)
        MayaCmds.move(5, 0, 0, r=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[2:5]', time=[1, 24])
        MayaCmds.currentTime(12)
        MayaCmds.select(shapeName+'.vtx[2:5]',replace=True)
        MayaCmds.move(0, 4, 0, r=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[2:5]', time=[12])

        # create a subD sphere and animate
        shapeName = 'sphere'
        MayaCmds.polySphere(name=shapeName)
        MayaCmds.select('sphereShape')
        MayaCmds.addAttr(longName='SubDivisionMesh', attributeType='bool',
            defaultValue=True)
        MayaCmds.move(-5, 0, 0, r=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[200:379]', shapeName+'.vtx[381]',
            time=[1, 24])
        MayaCmds.currentTime(12)
        MayaCmds.select(shapeName+'.vtx[200:379]',  shapeName+'.vtx[381]',
            replace=True)
        MayaCmds.scale(0.5, 0.5, 0.5, relative=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[200:379]',  shapeName+'.vtx[381]',
            time=[12])
        MayaCmds.currentTime(1)

        # create a subD torus and animate
        shapeName = 'torus'
        MayaCmds.polyTorus(name=shapeName)
        MayaCmds.select('torusShape')
        MayaCmds.addAttr(longName='SubDivisionMesh', attributeType='bool',
            defaultValue=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[200:219]',time=[1, 24])
        MayaCmds.currentTime(12)
        MayaCmds.select(shapeName+'.vtx[200:219]',replace=True)
        MayaCmds.scale(2, 1, 2, relative=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[200:219]', time=[12])

        # create a subD cone and animate
        shapeName = 'cone'
        MayaCmds.polyCone( name=shapeName )
        MayaCmds.select('coneShape')
        MayaCmds.addAttr(longName='SubDivisionMesh', attributeType='bool',
            defaultValue=True)
        MayaCmds.move(0, 0, -5, r=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[20]', time=[1, 24])
        MayaCmds.currentTime(12)
        MayaCmds.select(shapeName+'.vtx[20]',replace=True)
        MayaCmds.move(0, 4, 0, r=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[20]', time=[12])

        self.__files.append(util.expandFileName('testSubDReload.abc'))

        # write it out to Abc file and load back in
        MayaCmds.AbcExport(j='-fr 1 24 -root cube -root sphere -root torus -root cone -file ' +
            self.__files[-1])

        # load back the Abc file, delete the sphere and save to a maya file
        MayaCmds.AbcImport( self.__files[-1], mode='open' )
        MayaCmds.delete('sphere')
        self.__files.append(util.expandFileName('test.mb'))
        MayaCmds.file(rename=self.__files[-1])
        MayaCmds.file(save=True)

        # import the saved maya file to compare with the original scene
        MayaCmds.file(self.__files[-1], open=True)
        MayaCmds.select('cube', 'torus', 'cone', replace=True)
        MayaCmds.group(name='ReloadGrp')
        MayaCmds.AbcImport(self.__files[-2], mode='import')

        shapeList = MayaCmds.ls(type='mesh')
        self.failUnlessEqual(len(shapeList), 7)

        # test the equality of cubes
        meshes = [('|cube|cubeShape', '|ReloadGrp|cube|cubeShape'),
            ('|torus|torusShape', '|ReloadGrp|torus|torusShape'),
            ('|cone|coneShape', '|ReloadGrp|cone|coneShape')]

        for m in meshes:
            for t in range(1, 25):
                MayaCmds.currentTime(t, update=True)
                if not util.compareMesh(m[0], m[1]):
                    self.fail('%s and %s are not the same at frame %d' %
                        (m[0], m[1], t))

    def testAnimNSurfaceDeleteReload(self):

        # create an animated Nurbs sphere
        MayaCmds.sphere(ch=False, name='nSphere')
        MayaCmds.move(5, 0, 0, relative=True)
        MayaCmds.select('nSphere.cv[0:1][0:7]', 'nSphere.cv[5:6][0:7]',
            replace=True)
        MayaCmds.setKeyframe(time=[1, 24])
        MayaCmds.currentTime(12, update=True)
        MayaCmds.scale(1.5, 1, 1, relative=True)
        MayaCmds.setKeyframe(time=12)

        # create an animated Nurbs torus
        MayaCmds.torus(ch=False, name='nTorus')
        MayaCmds.move(-5, 0, 0, relative=True)
        MayaCmds.select('nTorus.cv[0][0:7]', 'nTorus.cv[2][0:7]',
            replace=True)
        MayaCmds.setKeyframe(time=[1, 24])
        MayaCmds.currentTime(12, update=True)
        MayaCmds.scale(1, 2, 2, relative=True)
        MayaCmds.setKeyframe(time=12)

        # create an animated Nurbs plane
        # should add the trim curve test on this surface, will be easier
        # than the rest
        MayaCmds.nurbsPlane(ch=False, name='nPlane')
        MayaCmds.move(-5, 5, 0, relative=True)
        MayaCmds.select('nPlane.cv[0:3][0:3]', replace=True)
        MayaCmds.setKeyframe(time=1)
        MayaCmds.currentTime(12, update=True)
        MayaCmds.rotate(0, 0, 90, relative=True)
        MayaCmds.setKeyframe(time=12)
        MayaCmds.currentTime(24, update=True)
        MayaCmds.rotate(0, 0, 90, relative=True)
        MayaCmds.setKeyframe(time=24)

        # create an animated Nurbs cylinder
        MayaCmds.cylinder(ch=False, name='nCylinder')
        MayaCmds.select('nCylinder.cv[0][0:7]', replace=True)
        MayaCmds.setKeyframe(time=[1, 24])
        MayaCmds.currentTime(12, update=True)
        MayaCmds.move(-3, 0, 0, relative=True)
        MayaCmds.setKeyframe(time=12)

        # write it out to Abc file and load back in
        self.__files.append(util.expandFileName('testNSurfaceReload.abc'))
        MayaCmds.AbcExport(j='-fr 1 24 -root nSphere -root nTorus -root nPlane -root nCylinder -file ' +
            self.__files[-1])

        # load back the Abc file, delete the torus and save to a maya file
        MayaCmds.AbcImport(self.__files[-1], mode='open')
        MayaCmds.delete('nTorus')
        self.__files.append(util.expandFileName('test.mb'))
        MayaCmds.file(rename=self.__files[-1])
        MayaCmds.file(save=True)

        # import the saved maya file to compare with the original scene
        MayaCmds.file(self.__files[-1], open=True)
        MayaCmds.select('nSphere', 'nPlane', 'nCylinder', replace=True)
        MayaCmds.group(name='ReloadGrp')
        MayaCmds.AbcImport(self.__files[-2], mode='import')

        surfaceList = MayaCmds.ls(type='nurbsSurface')
        self.failUnlessEqual(len(surfaceList), 7)

        surfaces = [('|nSphere|nSphereShape',
            '|ReloadGrp|nSphere|nSphereShape'),
            ('|nPlane|nPlaneShape', '|ReloadGrp|nPlane|nPlaneShape'),
            ('|nCylinder|nCylinderShape',
            '|ReloadGrp|nCylinder|nCylinderShape')]

        for s in surfaces:
            for t in range(1, 25):
                MayaCmds.currentTime(t, update=True)
                if not util.compareNurbsSurface(s[0], s[1]):
                    self.fail('%s and %s are not the same at frame %d' %
                        (s[0], s[1], t))

    def testAnimNSurfaceAndPolyDeleteReload(self):

        # create a poly cube and animate
        shapeName = 'pCube'
        MayaCmds.polyCube(name=shapeName)
        MayaCmds.move(5, 0, 0, r=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[2:5]', time=[1, 24])
        MayaCmds.currentTime(12)
        MayaCmds.select(shapeName+'.vtx[2:5]',replace=True)
        MayaCmds.move(0, 4, 0, r=True)
        MayaCmds.setKeyframe(shapeName+'.vtx[2:5]', time=[12])

        # create an animated Nurbs plane
        MayaCmds.nurbsPlane(ch=False, name='nPlane')
        MayaCmds.move(-5, 5, 0, relative=True)
        MayaCmds.select('nPlane.cv[0:3][0:3]', replace=True)
        MayaCmds.setKeyframe(time=1)
        MayaCmds.currentTime(12, update=True)
        MayaCmds.rotate(0, 0, 90, relative=True)
        MayaCmds.setKeyframe(time=12)
        MayaCmds.currentTime(24, update=True)
        MayaCmds.rotate(0, 0, 90, relative=True)
        MayaCmds.setKeyframe(time=24)

        # write it out to Abc file and load back in
        self.__files.append(util.expandFileName('testNSurfaceAndPolyReload.abc'))
        MayaCmds.AbcExport(j='-fr 1 24 -root pCube -root nPlane -file ' + self.__files[-1])
        # load back the Abc file, delete the cube and save to a maya file
        MayaCmds.AbcImport(self.__files[-1], mode='open')
        MayaCmds.delete('pCube')
        self.__files.append(util.expandFileName('test.mb'))
        MayaCmds.file(rename=self.__files[-1])
        MayaCmds.file(save=True)

        # import the saved maya file to compare with the original scene
        MayaCmds.file(self.__files[-1], open=True)
        MayaCmds.select('nPlane', replace=True)
        MayaCmds.group(name='ReloadGrp')
        MayaCmds.AbcImport(self.__files[-2], mode='import')

        shapeList = MayaCmds.ls(type='mesh')
        self.failUnlessEqual(len(shapeList), 1)
        surfaceList = MayaCmds.ls(type='nurbsSurface')
        self.failUnlessEqual(len(surfaceList), 2)

        # test the equality of plane
        surface1 = '|nPlane|nPlaneShape'
        surface2 = '|ReloadGrp|nPlane|nPlaneShape'
        for t in range(1, 25):
            MayaCmds.currentTime(t, update=True)
            if not util.compareNurbsSurface( surface1, surface2 ):
                self.fail('%s and %s are not the same at frame %d' %
                    (surface1, surface2, t))

    def testAnimCameraDeleteReload(self):

        # cam1
        MayaCmds.camera(name='cam1')
        MayaCmds.setAttr('cam1Shape1.horizontalFilmAperture', 0.962)
        MayaCmds.setAttr('cam1Shape1.verticalFilmAperture', 0.731)
        MayaCmds.setAttr('cam1Shape1.focalLength', 50)
        MayaCmds.setAttr('cam1Shape1.focusDistance', 5)
        MayaCmds.setAttr('cam1Shape1.shutterAngle', 144)
        MayaCmds.setAttr('cam1Shape1.centerOfInterest', 1384.825)
        # cam2
        MayaCmds.duplicate('cam1', returnRootsOnly=True)
        # cam3
        MayaCmds.duplicate('cam1', returnRootsOnly=True)
        # cam4
        MayaCmds.duplicate('cam1', returnRootsOnly=True)

        # animate each camera slightly different
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe('cam1Shape1', attribute='horizontalFilmAperture')
        MayaCmds.setKeyframe('cam2Shape', attribute='focalLength')
        MayaCmds.setKeyframe('cam3Shape', attribute='focusDistance')
        MayaCmds.setKeyframe('cam4Shape', attribute='shutterAngle')
        MayaCmds.setKeyframe('cam4Shape', attribute='centerOfInterest')
        MayaCmds.currentTime(24, update=True)
        MayaCmds.setKeyframe('cam1Shape1', attribute='horizontalFilmAperture',
            value=0.95)
        MayaCmds.setKeyframe('cam2Shape', attribute='focalLength', value=40)
        MayaCmds.setKeyframe('cam3Shape', attribute='focusDistance', value=5.4)
        MayaCmds.setKeyframe('cam4Shape', attribute='shutterAngle',
            value=174.94)
        MayaCmds.setKeyframe('cam4Shape', attribute='centerOfInterest',
            value=67.418)

        # write them out to an Abc file and load back in
        self.__files.append(util.expandFileName('testCamReload.abc'))
        MayaCmds.AbcExport(j='-fr 1 24 -root cam1 -root cam2 -root cam3 -root cam4 -file ' +
            self.__files[-1])
        # load back the Abc file, delete the 2nd camera and save to a maya file
        MayaCmds.AbcImport(self.__files[-1], mode='open')
        MayaCmds.delete('cam2')
        self.__files.append(util.expandFileName('test.mb'))
        MayaCmds.file(rename=self.__files[-1])
        MayaCmds.file(save=True)

        # import the saved maya file to compare with the original scene
        MayaCmds.file(self.__files[-1], open=True)
        MayaCmds.select('cam1', 'cam3', 'cam4', replace=True)
        MayaCmds.group(name='ReloadGrp')
        MayaCmds.AbcImport(self.__files[-2], mode='import')

        camList = MayaCmds.ls(type='camera')
        # should be 7, but this query will return the four standard cameras in
        # the scene too
        self.failUnlessEqual(len(camList), 11)

        # test the equality of cameras
        cameras = [('|cam1|cam1Shape1', '|ReloadGrp|cam1|cam1Shape1'),
            ('|cam3|cam3Shape', '|ReloadGrp|cam3|cam3Shape'),
            ('|cam4|cam4Shape', '|ReloadGrp|cam4|cam4Shape')]

        for c in cameras:
            for t in range(1, 25):
                MayaCmds.currentTime(t, update=True)
                if not util.compareCamera(c[0], c[1]):
                    self.fail('%s and %s are not the same at frame %d' %
                        (c[0], c[1], t))

    def testAnimNCurvesDeleteReload(self):

        # create some animated curves
        MayaCmds.textCurves(ch=False, t='Maya', name='Curves', font='Courier')
        MayaCmds.currentTime(1, update=True)
        MayaCmds.select('curve1.cv[0:27]', 'curve2.cv[0:45]',
            'curve3.cv[0:15]', 'curve4.cv[0:19]', 'curve5.cv[0:45]',
            'curve6.cv[0:15]', replace=True)
        MayaCmds.setKeyframe()
        MayaCmds.currentTime(24, update=True)
        MayaCmds.select('curve1.cv[0:27]', replace=True)
        MayaCmds.move(-3, 3, 0, relative=True)
        MayaCmds.select('curve2.cv[0:45]', 'curve3.cv[0:15]', replace=True)
        MayaCmds.scale(1.5, 1.5, 1.5, relative=True)
        MayaCmds.select('curve4.cv[0:19]', replace=True)
        MayaCmds.move(1.5, 0, 0, relative=True)
        MayaCmds.rotate(0, 90, 0, relative=True)
        MayaCmds.select('curve5.cv[0:45]', 'curve6.cv[0:15]', replace=True)
        MayaCmds.move(3, 0, 0, relative=True)
        MayaCmds.select('curve1.cv[0:27]', 'curve2.cv[0:45]',
            'curve3.cv[0:15]', 'curve4.cv[0:19]', 'curve5.cv[0:45]',
            'curve6.cv[0:15]', replace=True)
        MayaCmds.setKeyframe()

        # write them out to an Abc file and load back in
        self.__files.append(util.expandFileName('testNCurvesReload.abc'))
        MayaCmds.AbcExport(j='-fr 1 24 -root CurvesShape -file ' + self.__files[-1])

        # load back the Abc file, delete the 2nd letter and save to a maya file
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        # delete letter "a" which has two curves
        MayaCmds.delete('Char_a_1')
        self.__files.append(util.expandFileName('test.mb'))
        MayaCmds.file(rename=self.__files[-1])
        MayaCmds.file(save=True)

        # import the saved maya file to compare with the original scene
        MayaCmds.file(self.__files[-1], open=True)
        MayaCmds.select('CurvesShape', replace=True)
        MayaCmds.group(name='ReloadGrp')
        MayaCmds.AbcImport(self.__files[-2], mode='import')

        curveList = MayaCmds.ls(type='nurbsCurve')
        self.failUnlessEqual(len(curveList), 10)

        # test the equality of curves
        curves = [('|CurvesShape|Char_M_1|curve1|curveShape1',
            '|ReloadGrp|CurvesShape|Char_M_1|curve1|curveShape1'),
            ('|CurvesShape|Char_y_1|curve4|curveShape4',
            '|ReloadGrp|CurvesShape|Char_y_1|curve4|curveShape4'),
            ('|CurvesShape|Char_a_2|curve5|curveShape5',
            '|ReloadGrp|CurvesShape|Char_a_2|curve5|curveShape5'),
            ('|CurvesShape|Char_a_2|curve6|curveShape6',
            '|ReloadGrp|CurvesShape|Char_a_2|curve6|curveShape6')]
        for c in curves:
            for t in range(1, 25):
                MayaCmds.currentTime(t, update=True)
                if not util.compareNurbsCurve(c[0], c[1]):
                    self.fail('%s and %s are not the same at frame %d' %
                        (c[0], c[1], t))

#-------------------------------------------------------------------------
    def testAnimNCurveGrpDeleteReload(self):

        # create an animated curves group
        MayaCmds.textCurves(ch=False, t='haka', name='Curves', font='Courier')
        MayaCmds.addAttr(longName='riCurves', at='bool', dv=True)
        MayaCmds.currentTime(1, update=True)
        MayaCmds.select('curve1.cv[0:27]', 'curve2.cv[0:45]',
            'curve3.cv[0:15]', 'curve4.cv[0:19]', 'curve5.cv[0:45]',
            'curve6.cv[0:15]', replace=True)
        MayaCmds.setKeyframe()
        MayaCmds.currentTime(24, update=True)
        MayaCmds.select('curve1.cv[0:27]', replace=True)
        MayaCmds.move(-3, 3, 0, relative=True)
        MayaCmds.select('curve2.cv[0:45]', 'curve3.cv[0:15]', replace=True)
        MayaCmds.scale(1.5, 1.5, 1.5, relative=True)
        MayaCmds.select('curve4.cv[0:19]', replace=True)
        MayaCmds.move(1.5, 0, 0, relative=True)
        MayaCmds.rotate(0, 90, 0, relative=True)
        MayaCmds.select('curve5.cv[0:45]', 'curve6.cv[0:15]', replace=True)
        MayaCmds.move(3, 0, 0, relative=True)
        MayaCmds.select('curve1.cv[0:27]', 'curve2.cv[0:45]',
            'curve3.cv[0:15]', 'curve4.cv[0:19]', 'curve5.cv[0:45]',
            'curve6.cv[0:15]', replace=True)
        MayaCmds.setKeyframe()

        # write them out to an Abc file and load back in
        self.__files.append(util.expandFileName('testNCurveGrpReload.abc'))
        MayaCmds.AbcExport(j='-fr 1 24 -root CurvesShape -file ' + self.__files[-1])

        # load back the Abc file, delete the 2nd letter and save to a maya file
        MayaCmds.AbcImport(self.__files[-1], mode='open')

        # delete letter "a" which has two curves, but as a curve group.
        # the curve shapes are renamed under the group node
        MayaCmds.delete('CurvesShape1')
        MayaCmds.delete('CurvesShape2')
        self.__files.append(util.expandFileName('testCurves.mb'))
        MayaCmds.file(rename=self.__files[-1])
        MayaCmds.file(save=True)

        # import the saved maya file to compare with the original scene
        MayaCmds.file(self.__files[-1], open=True)
        MayaCmds.select('|CurvesShape', replace=True)
        MayaCmds.group(name='ReloadGrp')
        MayaCmds.AbcImport(self.__files[-2], mode='import')

        curveList = MayaCmds.ls(type='nurbsCurve')
        self.failUnlessEqual(len(curveList), 10)

        curves = [('|CurvesShape|CurvesShape',
            '|ReloadGrp|CurvesShape|CurvesShape'),
            ('|CurvesShape|CurvesShape8',
            '|ReloadGrp|CurvesShape|CurvesShape3'),
            ('|CurvesShape|CurvesShape9',
            '|ReloadGrp|CurvesShape|CurvesShape4'),
            ('|CurvesShape|CurvesShape10',
            '|ReloadGrp|CurvesShape|CurvesShape5')]
        for c in curves:
            for t in range(1, 25):
                MayaCmds.currentTime(t, update=True)
                if not util.compareNurbsCurve(c[0], c[1]):
                    self.fail('%s and %s are not the same at frame %d' %
                        (c[0], c[1], t))

    def testAnimPropDeleteReload(self):

        # create some animated properties on a transform node ( could be any type )
        nodeName = MayaCmds.polyPrism(ch=False, name = 'prism')
        MayaCmds.addAttr(longName='SPT_int8',   defaultValue=0,
            attributeType='byte', keyable=True)
        MayaCmds.addAttr(longName='SPT_int16', defaultValue=100,
            attributeType='short', keyable=True)
        MayaCmds.addAttr(longName='SPT_int32', defaultValue=1000,
            attributeType='long', keyable=True)
        MayaCmds.addAttr(longName='SPT_float', defaultValue=0.57777777,
            attributeType='float', keyable=True)
        MayaCmds.addAttr(longName='SPT_double', defaultValue=5.0456435,
            attributeType='double', keyable=True)
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe(nodeName, attribute='SPT_int8')
        MayaCmds.setKeyframe(nodeName, attribute='SPT_int16')
        MayaCmds.setKeyframe(nodeName, attribute='SPT_int32')
        MayaCmds.setKeyframe(nodeName, attribute='SPT_float')
        MayaCmds.setKeyframe(nodeName, attribute='SPT_double')
        MayaCmds.currentTime(24, update=True)
        MayaCmds.setKeyframe(nodeName, attribute='SPT_int8', value=8)
        MayaCmds.setKeyframe(nodeName, attribute='SPT_int16', value=16)
        MayaCmds.setKeyframe(nodeName, attribute='SPT_int32', value=32)
        MayaCmds.setKeyframe(nodeName, attribute='SPT_float', value=5.24847)
        MayaCmds.setKeyframe(nodeName, attribute='SPT_double', value=3.14154)

        # create SPT_HWColor on the shape node
        MayaCmds.select('prismShape')
        MayaCmds.addAttr(longName='SPT_HwColorR', defaultValue=1.0,
            minValue=0.0, maxValue=1.0)
        MayaCmds.addAttr(longName='SPT_HwColorG', defaultValue=1.0,
            minValue=0.0, maxValue=1.0)
        MayaCmds.addAttr(longName='SPT_HwColorB', defaultValue=1.0,
            minValue=0.0, maxValue=1.0)
        MayaCmds.addAttr( longName='SPT_HwColor', usedAsColor=True,
            attributeType='float3')
        MayaCmds.currentTime(1, update=True)
        MayaCmds.setKeyframe(at='SPT_HwColorR')
        MayaCmds.setKeyframe(at='SPT_HwColorG')
        MayaCmds.setKeyframe(at='SPT_HwColorB')
        MayaCmds.currentTime(24, update=True)
        MayaCmds.setKeyframe(at='SPT_HwColorR', value=0.5)
        MayaCmds.setKeyframe(at='SPT_HwColorG', value=0.15)
        MayaCmds.setKeyframe(at='SPT_HwColorB', value=0.75)

        # write them out to an Abc file and load back in
        self.__files.append(util.expandFileName('testPropReload.abc'))
        MayaCmds.AbcExport(j='-atp SPT_ -fr 1 24 -root prism -file ' + self.__files[-1])

        # load back the Abc file, delete the 2nd letter and save to a maya file
        abcNode = MayaCmds.AbcImport(
            self.__files[-1], mode='open' )

        # delete connections to animated props
        prop = MayaCmds.listConnections('|prism.SPT_float', p=True)[0]
        MayaCmds.disconnectAttr(prop, '|prism.SPT_float')

        attr = '|prism|prismShape.SPT_HwColorG'
        prop = MayaCmds.listConnections(attr, p=True)[0]
        MayaCmds.disconnectAttr(prop, attr)

        self.__files.append(util.expandFileName('test.mb'))
        MayaCmds.file(rename=self.__files[-1])
        MayaCmds.file(save=True)


        # import the saved maya file to compare with the original scene
        MayaCmds.file(self.__files[-1], open=True)
        MayaCmds.select('prism', replace=True)
        MayaCmds.group(name='ReloadGrp')
        MayaCmds.AbcImport(self.__files[-2], mode='import')

        # test the equality of props
        for t in range(1, 25):
            MayaCmds.currentTime(t, update=True)
            self.failUnlessEqual(MayaCmds.getAttr('|prism.SPT_int8'),
                MayaCmds.getAttr('|ReloadGrp|prism.SPT_int8'),
                'prism.SPT_int8 not equal' )
            self.failUnlessEqual(MayaCmds.getAttr('|prism.SPT_int16'),
                MayaCmds.getAttr('|ReloadGrp|prism.SPT_int16'),
                'prism.SPT_int16 not equal')
            self.failUnlessEqual(MayaCmds.getAttr('|prism.SPT_int32'),
                MayaCmds.getAttr('|ReloadGrp|prism.SPT_int32'),
                'prism.SPT_int32 not equal')
            self.failUnlessAlmostEqual(MayaCmds.getAttr('|prism.SPT_double'),
                MayaCmds.getAttr('|ReloadGrp|prism.SPT_double'), 4,
                'prism.SPT_double not equal')
            self.failUnlessAlmostEqual(
                MayaCmds.getAttr('|prism|prismShape.SPT_HwColorR'),
                MayaCmds.getAttr('|ReloadGrp|prism|prismShape.SPT_HwColorR'),
                4, 'prismShape.SPT_HwColorR not equal')
            self.failUnlessAlmostEqual(
                MayaCmds.getAttr('|prism|prismShape.SPT_HwColorB'),
                MayaCmds.getAttr('|ReloadGrp|prism|prismShape.SPT_HwColorB'),
                4, 'prismShape.SPT_HwColorB not equal')

