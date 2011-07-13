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
from maya import mel as Mel
import os
import unittest
import util

def makeRobot():
    MayaCmds.polyCube(name="head")
    MayaCmds.move(0, 4, 0, r=1)

    MayaCmds.polyCube(name="chest")
    MayaCmds.scale(2, 2.5, 1)
    MayaCmds.move(0, 2, 0, r=1)
    MayaCmds.polyCube(name="leftArm")
    MayaCmds.move(0, 3, 0, r=1)
    MayaCmds.scale(2, 0.5, 1, r=1)
    MayaCmds.duplicate(name="rightArm")
    MayaCmds.select("leftArm")
    MayaCmds.move(1.25, 0, 0, r=1)
    MayaCmds.rotate(0, 0, 32, r=1, os=1)
    MayaCmds.select("rightArm")
    MayaCmds.move(-1.25, 0, 0, r=1)
    MayaCmds.rotate(0, 0, -32, r=1, os=1)
    MayaCmds.select("rightArm", "leftArm", "chest", r=1)
    MayaCmds.group(name="body")

    MayaCmds.polyCube(name="bottom")
    MayaCmds.scale(2, 0.5, 1)
    MayaCmds.move(0, 0.5, 0, r=1)
    MayaCmds.polyCube(name="leftLeg")
    MayaCmds.scale(0.65, 2.8, 1, r=1)
    MayaCmds.move(-0.5, -1, 0, r=1)
    MayaCmds.duplicate(name="rightLeg")
    MayaCmds.move(1, 0, 0, r=1)
    MayaCmds.select("rightLeg", "leftLeg", "bottom", r=1)
    MayaCmds.group(name="lower")

    MayaCmds.select("head", "body", "lower", r=1)
    MayaCmds.group(name="robot")


def makeRobotAnimated():
    makeRobot()
    #change pivot point of arms and legs
    MayaCmds.move(0.65, -0.40, 0, 'rightArm.scalePivot',
        'rightArm.rotatePivot', relative=True)
    MayaCmds.move(-0.65, -0.40, 0, 'leftArm.scalePivot', 'leftArm.rotatePivot',
        relative=True)
    MayaCmds.move(0, 1.12, 0, 'rightLeg.scalePivot', 'rightLeg.rotatePivot',
        relative=True)
    MayaCmds.move(0, 1.12, 0, 'leftLeg.scalePivot', 'leftLeg.rotatePivot',
        relative=True)

    MayaCmds.setKeyframe('leftLeg', at='rotateX', value=25, t=[1, 12])
    MayaCmds.setKeyframe('leftLeg', at='rotateX', value=-40, t=[6])
    MayaCmds.setKeyframe('rightLeg', at='scaleY', value=2.8, t=[1, 12])
    MayaCmds.setKeyframe('rightLeg', at='scaleY', value=5.0, t=[6])

    MayaCmds.setKeyframe('leftArm', at='rotateZ', value=55, t=[1, 12])
    MayaCmds.setKeyframe('leftArm', at='rotateZ', value=-50, t=[6])
    MayaCmds.setKeyframe('rightArm', at='scaleX', value=0.5, t=[1, 12])
    MayaCmds.setKeyframe('rightArm', at='scaleX', value=3.6, t=[6])

def drawBBox(llx, lly, llz, urx, ury, urz):
    # delete the old bounding box
    if MayaCmds.objExists('bbox'):
        MayaCmds.delete('bbox')

    p0 = (llx, lly, urz)
    p1 = (urx, lly, urz)
    p2 = (urx, lly, llz)
    p3 = (llx, lly, llz)
    p4 = (llx, ury, urz)
    p5 = (urx, ury, urz)
    p6 = (urx, ury, llz)
    p7 = (llx, ury, llz)

    MayaCmds.curve(d=1, p=[p0, p1])
    MayaCmds.curve(d=1, p=[p1, p2])
    MayaCmds.curve(d=1, p=[p2, p3])
    MayaCmds.curve(d=1, p=[p3, p0])
    MayaCmds.curve(d=1, p=[p4, p5])
    MayaCmds.curve(d=1, p=[p5, p6])
    MayaCmds.curve(d=1, p=[p6, p7])
    MayaCmds.curve(d=1, p=[p7, p4])
    MayaCmds.curve(d=1, p=[p0, p4])
    MayaCmds.curve(d=1, p=[p1, p5])
    MayaCmds.curve(d=1, p=[p2, p6])
    MayaCmds.curve(d=1, p=[p3, p7])

    MayaCmds.select(MayaCmds.ls("curve?"))
    MayaCmds.select(MayaCmds.ls("curve??"), add=True)
    MayaCmds.group(name="bbox")

class callbackTest(unittest.TestCase):

    def setUp(self):
        MayaCmds.file(new=True, force=True)
        self.__files = []

    def tearDown(self):
        for f in self.__files:
            os.remove(f)

    def testMelPerFrameCallbackFlag(self):

        makeRobotAnimated()

        # make a non-zero transform on top of it
        MayaCmds.select('robot')
        MayaCmds.group(n='parent')

        MayaCmds.setAttr('parent.tx', 5)
        MayaCmds.setAttr('parent.ty', 3.15)
        MayaCmds.setAttr('parent.tz', 0.8)
        MayaCmds.setAttr('parent.rx', 2)
        MayaCmds.setAttr('parent.ry', 90)
        MayaCmds.setAttr('parent.rz', 1)
        MayaCmds.setAttr('parent.sx', 0.9)
        MayaCmds.setAttr('parent.sy', 1.1)
        MayaCmds.setAttr('parent.sz', 0.5)

        # create a camera looking at the robot
        camNames = MayaCmds.camera()
        MayaCmds.move(50.107, 20.968, 5.802, r=True)
        MayaCmds.rotate(-22.635, 83.6, 0, r=True)
        MayaCmds.camera(camNames[1], e=True, centerOfInterest=55.336,
            focalLength=50, horizontalFilmAperture=0.962,
            verticalFilmAperture=0.731)
        MayaCmds.rename(camNames[0], "cam_master")
        MayaCmds.setKeyframe('cam_master', t=[1, 12])

        __builtins__['bboxDict'] = {}
        self.__files.append(util.expandFileName('pyPerFrameTest.abc'))
        MayaCmds.AbcExport(j='-fr 1 12 -pfc bboxDict[#FRAME#]=#BOUNDSARRAY# -root robot -file ' + self.__files[-1])

        # push the numbers into a flat list
        array = []
        for i in range(1, 13):
            array.append(bboxDict[i][0])
            array.append(bboxDict[i][1])
            array.append(bboxDict[i][2])
            array.append(bboxDict[i][3])
            array.append(bboxDict[i][4])
            array.append(bboxDict[i][5])

        bboxList_worldSpaceOff = [
            -1.10907, -2.4, -1.51815, 1.8204, 4.5, 0.571487,
            -1.64677, -2.85936, -0.926652, 2.24711, 4.5, 0.540761,
            -2.25864, -3.38208, -0.531301, 2.37391, 4.5, 0.813599,
            -2.83342, -3.87312, -0.570038, 2.3177, 4.5, 1.45821,
            -3.25988, -4.23744, -0.569848, 2.06639, 4.5, 1.86489,
            -3.42675, -4.38, -0.563003, 1.92087, 4.5, 2.00285,
            -3.30872, -4.27917, -0.568236, 2.02633, 4.5, 1.9066,
            -2.99755, -4.01333, -0.572918, 2.24279, 4.5, 1.62326,
            -2.55762, -3.6375, -0.556938, 2.3781, 4.5, 1.16026,
            -2.05331, -3.20667, -0.507072, 2.37727, 4.5, 0.564985,
            -1.549, -2.77583, -1.04022, 2.18975, 4.5, 0.549216,
            -1.10907, -2.4, -1.51815, 1.8204, 4.5, 0.571487]

        self.failUnlessEqual(len(array), len(bboxList_worldSpaceOff))

        for i in range(0, len(array)):
            self.failUnlessAlmostEqual(array[i], bboxList_worldSpaceOff[i], 4,
            '%d element in bbox array does not match.' % i)


        # test the bounding box calculation when worldSpace flag is on

        __builtins__['bboxDict'] = {}
        self.__files.append(util.expandFileName('pyPerFrameWorldspaceTest.abc'))
        MayaCmds.AbcExport(j='-fr 1 12 -ws -pfc bboxDict[#FRAME#]=#BOUNDSARRAY# -root robot -file ' + self.__files[-1])

        # push the numbers into a flat list
        array = []
        for i in range(1, 13):
            array.append(bboxDict[i][0])
            array.append(bboxDict[i][1])
            array.append(bboxDict[i][2])
            array.append(bboxDict[i][3])
            array.append(bboxDict[i][4])
            array.append(bboxDict[i][5])

        bboxList_worldSpaceOn = [
            4.77569, 0.397085, -0.991594, 5.90849, 7.99465, 1.64493,
            5.06518, -0.108135, -1.37563, 5.90849, 7.99465, 2.12886,
            5.25725, -0.683039, -1.48975, 5.9344, 7.99465, 2.67954,
            5.24782, -1.223100, -1.43916, 6.26283, 7.99465, 3.19685,
            5.24083, -1.62379, -1.21298, 6.47282, 7.99465, 3.58066,
            5.23809, -1.78058, -1.08202, 6.54482, 7.99465, 3.73084,
            5.24003, -1.66968, -1.17693, 6.49454, 7.99465, 3.62461,
            5.24513, -1.37731, -1.37175, 6.34772, 7.99465, 3.34456,
            5.25234, -0.963958, -1.49352, 6.11048, 7.99465, 2.94862,
            5.26062, -0.490114, -1.49277, 5.90849, 7.99465, 2.49475,
            5.00931, -0.0162691, -1.32401, 5.90849, 7.99465, 2.04087,
            4.77569, 0.397085, -0.991594, 5.90849, 7.99465, 1.64493]

        self.failUnlessEqual(len(array), len(bboxList_worldSpaceOn))

        for i in range(0, len(array)):
            self.failUnlessAlmostEqual(array[i], bboxList_worldSpaceOn[i], 4,
            '%d element in bbox array does not match.' % i)

        # test the bounding box calculation for camera
        __builtins__['bboxDict'] = {}
        self.__files.append(util.expandFileName('camPyPerFrameTest.abc'))
        MayaCmds.AbcExport(j='-fr 1 12 -pfc bboxDict[#FRAME#]=#BOUNDSARRAY# -root cam_master -file ' + self.__files[-1])

        # push the numbers into a flat list ( since each frame is the same )
        array = []
        array.append(bboxDict[1][0])
        array.append(bboxDict[1][1])
        array.append(bboxDict[1][2])
        array.append(bboxDict[1][3])
        array.append(bboxDict[1][4])
        array.append(bboxDict[1][5])

        # cameras aren't included in the bounds
        bboxList_CAM = [0, 0, 0, 0, 0, 0]

        for i in range(0, len(array)):
            self.failUnlessAlmostEqual(array[i], bboxList_CAM[i], 4,
                '%d element in camera bbox array does not match.' % i)
