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
import math

# adds the current working directory so tools don't get confused about where we
# are storing files
def expandFileName(name):
    return os.getcwd() + os.path.sep + name

# compare the two floating point values
def floatDiff(val1, val2, tolerance):
    diff = math.fabs(val1 - val2)
    if diff < math.pow(10, -tolerance):
        return True
    return False

# function that returns a node object given a name
def getObjFromName(nodeName):
    selectionList = OpenMaya.MSelectionList()
    selectionList.add( nodeName )
    obj = OpenMaya.MObject()
    selectionList.getDependNode(0, obj)
    return obj

# function that finds a plug given a node object and plug name
def getPlugFromName(attrName, nodeObj):
    fnDepNode = OpenMaya.MFnDependencyNode(nodeObj)
    attrObj = fnDepNode.attribute(attrName)
    plug = OpenMaya.MPlug(nodeObj, attrObj)
    return plug

# meaning of return value:
#   0 if array1 = array2
#   1 if array1 and array2 are of the same length, array1[i] == array2[i] for 0<=i<m<len, and array1[m] < array2[m]
#  -1 if array1 and array2 are of the same length, array1[i] == array2[i] for 0<=i<m<len, and array1[m] > array2[m]
#   2 if array1.length() < array2.length()
#  -2 if array1.length() > array2.length()
def compareArray(array1, array2):
    len1 = array1.length()
    len2 = array2.length()
    if len1 > len2 : return -2
    if len1 < len2 : return  2
    for i in range(0, len1):
        if array1[i] < array2[i] :
            return 1
        if array1[i] > array2[i] :
            return -1
    return 0

# return True if the two point arrays are exactly the same
def comparePointArray(array1, array2):
    len1 = array1.length()
    len2 = array2.length()
    if len1 != len2 :
        return False
    for i in range(0, len1):
        if not array1[i].isEquivalent(array2[i], 1e-6):
            return False
    return True

# return True if the two meshes are identical
def compareMesh( nodeName1, nodeName2 ):

    # basic error checking
    obj1 = getObjFromName(nodeName1)
    if not obj1.hasFn(OpenMaya.MFn.kMesh):
        return False
    obj2 = getObjFromName(nodeName2)
    if not obj2.hasFn(OpenMaya.MFn.kMesh):
        return False

    polyIt1 = OpenMaya.MItMeshPolygon( obj1 )
    polyIt2 = OpenMaya.MItMeshPolygon( obj2 )

    if polyIt1.count() != polyIt2.count():
        return False

    if polyIt1.polygonVertexCount() != polyIt2.polygonVertexCount():
        return False

    vertices1 = OpenMaya.MIntArray()
    vertices2 = OpenMaya.MIntArray()
    pointArray1 = OpenMaya.MPointArray()
    pointArray2 = OpenMaya.MPointArray()

    while  polyIt1.isDone()==False and polyIt2.isDone()==False :

        # compare vertex indices
        polyIt1.getVertices(vertices1)
        polyIt2.getVertices(vertices2)
        if compareArray(vertices1, vertices2) != 0:
            return False

        # compare vertex positions
        polyIt1.getPoints(pointArray1)
        polyIt2.getPoints(pointArray2)
        if not comparePointArray( pointArray1, pointArray2 ):
            return False

        polyIt1.next()
        polyIt2.next()

    if polyIt1.isDone() and polyIt2.isDone() :
        return True

    return False

# return True if the two Nurbs Surfaces are identical
def compareNurbsSurface(nodeName1, nodeName2):

    # basic error checking
    obj1 = getObjFromName(nodeName1)
    if not obj1.hasFn(OpenMaya.MFn.kNurbsSurface):
        return False
    obj2 = getObjFromName(nodeName2)
    if not obj2.hasFn(OpenMaya.MFn.kNurbsSurface):
        return False

    fn1 = OpenMaya.MFnNurbsSurface(obj1)
    fn2 = OpenMaya.MFnNurbsSurface(obj2)

    # degree
    if fn1.degreeU() != fn2.degreeU():
        return False
    if fn1.degreeV() != fn2.degreeV():
        return False

    # span
    if fn1.numSpansInU() != fn2.numSpansInU():
        return False
    if fn1.numSpansInV() != fn2.numSpansInV():
        return False

    # form
    if fn1.formInU() != fn2.formInU():
        return False
    if fn1.formInV() != fn2.formInV():
        return False

    # control points
    if fn1.numCVsInU() != fn2.numCVsInU():
        return False
    if fn1.numCVsInV() != fn2.numCVsInV():
        return False

    cv1 = OpenMaya.MPointArray()
    fn1.getCVs(cv1)
    cv2 = OpenMaya.MPointArray()
    fn2.getCVs(cv2)
    if not comparePointArray(cv1, cv2):
        return False

    # knots
    if fn1.numKnotsInU() != fn2.numKnotsInU():
        return False
    if fn1.numKnotsInV() != fn2.numKnotsInV():
        return False

    knotsU1 = OpenMaya.MDoubleArray()
    fn1.getKnotsInU(knotsU1)
    knotsV1 = OpenMaya.MDoubleArray()
    fn1.getKnotsInV(knotsV1)
    knotsU2 = OpenMaya.MDoubleArray()
    fn2.getKnotsInU(knotsU2)
    knotsV2 = OpenMaya.MDoubleArray()
    fn2.getKnotsInV(knotsV2)

    if compareArray( knotsU1, knotsU2 ) != 0:
        return False

    if compareArray( knotsV1, knotsV2 ) != 0:
        return False

    # trim curves
    if fn1.isTrimmedSurface() != fn2.isTrimmedSurface():
        return False

    # may need to add more trim checks

    return True


# return True if the two locators are idential
def compareLocator(nodeName1, nodeName2):

    # basic error checking
    obj1 = getObjFromName(nodeName1)
    if not obj1.hasFn(OpenMaya.MFn.kLocator):
        return False

    obj2 = getObjFromName(nodeName2)
    if not obj2.hasFn(OpenMaya.MFn.kLocator):
        return False

    if not floatDiff(MayaCmds.getAttr(nodeName1+'.localPositionX'),
        MayaCmds.getAttr(nodeName2+'.localPositionX'), 4):
        return False

    if not floatDiff(MayaCmds.getAttr(nodeName1+'.localPositionY'),
        MayaCmds.getAttr(nodeName2+'.localPositionY'), 4):
        return False

    if not floatDiff(MayaCmds.getAttr(nodeName1+'.localPositionZ'),
        MayaCmds.getAttr(nodeName2+'.localPositionZ'), 4):
        return False

    if not floatDiff(MayaCmds.getAttr(nodeName1+'.localScaleX'),
        MayaCmds.getAttr(nodeName2+'.localScaleX'), 4):
        return False

    if not floatDiff(MayaCmds.getAttr(nodeName1+'.localScaleY'),
        MayaCmds.getAttr(nodeName2+'.localScaleY'), 4):
        return False

    if not floatDiff(MayaCmds.getAttr(nodeName1+'.localScaleZ'),
        MayaCmds.getAttr(nodeName2+'.localScaleZ'), 4):
        return False

    return True


# return True if the two cameras are identical
def compareCamera( nodeName1, nodeName2 ):

    # basic error checking
    obj1 = getObjFromName(nodeName1)
    if not obj1.hasFn(OpenMaya.MFn.kCamera):
        return False

    obj2 = getObjFromName(nodeName2)
    if not obj2.hasFn(OpenMaya.MFn.kCamera):
        return False

    fn1 = OpenMaya.MFnCamera( obj1 )
    fn2 = OpenMaya.MFnCamera( obj2 )

    if fn1.filmFit() != fn2.filmFit():
        print "differ in filmFit"
        return False

    if not floatDiff(fn1.filmFitOffset(), fn2.filmFitOffset(), 4):
        print "differ in filmFitOffset"
        return False

    if fn1.isOrtho() != fn2.isOrtho():
        print "differ in isOrtho"
        return False

    if not floatDiff(fn1.orthoWidth(), fn2.orthoWidth(), 4):
        print "differ in orthoWidth"
        return False

    if not floatDiff(fn1.focalLength(), fn2.focalLength(), 4):
        print "differ in focalLength"
        return False

    if not floatDiff(fn1.lensSqueezeRatio(), fn2.lensSqueezeRatio(), 4):
        print "differ in lensSqueezeRatio"
        return False

    if not floatDiff(fn1.cameraScale(), fn2.cameraScale(), 4):
        print "differ in cameraScale"
        return False

    if not floatDiff(fn1.horizontalFilmAperture(),
        fn2.horizontalFilmAperture(), 4):
        print "differ in horizontalFilmAperture"
        return False

    if not floatDiff(fn1.verticalFilmAperture(), fn2.verticalFilmAperture(), 4):
        print "differ in verticalFilmAperture"
        return False

    if not floatDiff(fn1.horizontalFilmOffset(), fn2.horizontalFilmOffset(), 4):
        print "differ in horizontalFilmOffset"
        return False

    if not floatDiff(fn1.verticalFilmOffset(), fn2.verticalFilmOffset(), 4):
        print "differ in verticalFilmOffset"
        return False

    if not floatDiff(fn1.overscan(), fn2.overscan(), 4):
        print "differ in overscan"
        return False

    if not floatDiff(fn1.nearClippingPlane(), fn2.nearClippingPlane(), 4):
        print "differ in nearClippingPlane"
        return False

    if not floatDiff(fn1.farClippingPlane(), fn2.farClippingPlane(), 4):
        print "differ in farClippingPlane"
        return False

    if not floatDiff(fn1.preScale(), fn2.preScale(), 4):
        print "differ in preScale"
        return False

    if not floatDiff(fn1.postScale(), fn2.postScale(), 4):
        print "differ in postScale"
        return False

    if not floatDiff(fn1.filmTranslateH(), fn2.filmTranslateH(), 4):
        print "differ in filmTranslateH"
        return False

    if not floatDiff(fn1.filmTranslateV(), fn2.filmTranslateV(), 4):
        print "differ in filmTranslateV"
        return False

    if not floatDiff(fn1.horizontalRollPivot(), fn2.horizontalRollPivot(), 4):
        print "differ in horizontalRollPivot"
        return False

    if not floatDiff(fn1.verticalRollPivot(), fn2.verticalRollPivot(), 4):
        print "differ in verticalRollPivot"
        return False

    if fn1.filmRollOrder() != fn2.filmRollOrder():
        print "differ in filmRollOrder"
        return False

    if not floatDiff(fn1.filmRollValue(), fn2.filmRollValue(), 4):
        print "differ in filmRollValue"
        return False

    if not floatDiff(fn1.fStop(), fn2.fStop(), 4):
        print "differ in fStop"
        return False

    if not floatDiff(fn1.focusDistance(), fn2.focusDistance(), 4,):
        print "differ in focusDistance"
        return False

    if not floatDiff(fn1.shutterAngle(), fn2.shutterAngle(), 4):
        print "differ in shutterAngle"
        return False

    if fn1.usePivotAsLocalSpace() != fn2.usePivotAsLocalSpace():
        print "differ in usePivotAsLocalSpace"
        return False

    if fn1.tumblePivot() != fn2.tumblePivot():
        print "differ in tumblePivot"
        return False

    return True

# return True if the two Nurbs curves are identical
def compareNurbsCurve(nodeName1, nodeName2):
    # basic error checking
    obj1 = getObjFromName(nodeName1)
    if not obj1.hasFn(OpenMaya.MFn.kNurbsCurve):
        print nodeName1, "not a curve."
        return False

    obj2 = getObjFromName(nodeName2)
    if not obj2.hasFn(OpenMaya.MFn.kNurbsCurve):
        print nodeName2, "not a curve."
        return False

    fn1 = OpenMaya.MFnNurbsCurve(obj1)
    fn2 = OpenMaya.MFnNurbsCurve(obj2)

    if fn1.degree() != fn2.degree():
        print nodeName1, nodeName2, "degrees differ."
        return False

    if fn1.numCVs() != fn2.numCVs():
        print nodeName1, nodeName2, "numCVs differ."
        return False

    if fn1.numSpans() != fn2.numSpans():
        print nodeName1, nodeName2, "spans differ."
        return False

    if fn1.numKnots() != fn2.numKnots():
        print nodeName1, nodeName2, "numKnots differ."
        return False

    if fn1.form() != fn2.form():
        print nodeName1, nodeName2, "form differ."
        return False

    cv1 = OpenMaya.MPointArray()
    fn1.getCVs(cv1)
    cv2 = OpenMaya.MPointArray()
    fn2.getCVs(cv2)

    if not comparePointArray(cv1, cv2):
        print nodeName1, nodeName2, "points differ."
        return False

    # we do not need to compare knots, since they aren't stored in Alembic
    # and are currently recreated as uniformly distributed between 0 and 1

    return True
