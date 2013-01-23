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

import os
import sys
import math
import traceback

from PyQt4 import QtCore
from PyQt4 import QtGui
from PyQt4 import uic
from PyQt4 import QtOpenGL

import numpy
import numpy.linalg as linalg
import OpenGL
OpenGL.ERROR_CHECKING = True
from OpenGL.GL import *
from OpenGL.GLU import *

import imath
import alembic
import alembicgl

kWrapExisting = alembic.Abc.WrapExistingFlag.kWrapExisting

def accumXform(xf, obj):
    if alembic.AbcGeom.IXform.matches(obj.getHeader()):
        x = alembic.AbcGeom.IXform(obj, kWrapExisting)
        xs = x.getSchema().getValue()
        xf *= xs.getMatrix()

def getFinalMatrix(obj):
    xf = imath.M44d()
    xf.makeIdentity()
    parent = obj.getParent()
    while parent:
        accumXform(xf, parent)
        parent = parent.getParent()
    return xf

class AbcGLWidget(QtOpenGL.QGLWidget):
    signalSceneOpened = QtCore.pyqtSignal()
    signalSceneError = QtCore.pyqtSignal(str)
    signalSceneDrawn = QtCore.pyqtSignal()

    def __init__(self, parent=None, filepath=None):
        format = QtOpenGL.QGLFormat()
        format.setSampleBuffers(True)
        QtOpenGL.QGLWidget.__init__(self, format, parent)

        # draw toggles
        self.__drawGrid = True
        self.__drawNormals = False
        self.__drawBounds = False
        self.__drawHUD = False

        # various matrices and vectors
        self.__matrix = []
        self.__translate  = [0.0, 0.0, 0.0]
        self.__center = [0.0, 0.0, 0.0]
        self.__near = 0.1
        self.__far = 100000.0
        self.__fovy = 45.0
        self.__radius = 5.0
        self.__last_pok = False
        self.__last_p2d = QtCore.QPoint()
        self.__last_p3d = [1.0, 0.0, 0.0]
        self.__rotating = False

        # alembic scene params
        self.archive = None
        self.scene = None

        self.setMouseTracking(True)
        self.setCursor(QtCore.Qt.OpenHandCursor)

        self.openScene(filepath)

    def openScene(self, filepath):
        if filepath and os.path.isfile(filepath):
            try:
                self.archive = alembic.Abc.IArchive(filepath)
                self.scene = alembicgl.SceneWrapper(filepath)
                self.signalSceneOpened.emit()
            except RuntimeError, e:
                self.signalSceneError.emit("Error drawing GL scene for %s" 
                                           % os.path.basename(filepath))
                print e

    def toggleGrid(self, force=None):
        if force is not None:
            self.__drawGrid = force
        else:
            self.__drawGrid = not self.__drawGrid
        self.updateGL()

    def toggleNormals(self, force=None):
        if force is not None:
            self.__drawNormals = force
        else:
            self.__drawNormals = not self.__drawNormals
        self.updateGL()

    def toggleBounds(self, force=None):
        if force is not None:
            self.__drawBounds = force
        else:
            self.__drawBounds = not self.__drawBounds
        self.updateGL()

    def toggleHUD(self, force=None):
        if force is not None:
            self.__drawHUD = force
        else:
            self.__drawHUD = not self.__drawHUD
        self.updateGL()
   
    def viewAll(self):
        to = imath.V3d([0, 0, -25.0]) - imath.V3d(self.__translate)
        self.translate(to)
        self.rotate([1.2, -2.8, -0.35], 50.0)
   
    def initializeGL(self):
        glPointSize(3.0)
        glEnable(GL_BLEND)
        glShadeModel(GL_SMOOTH)
        glEnable(GL_AUTO_NORMAL)
        glEnable(GL_COLOR_MATERIAL)
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_NORMALIZE)
        glDisable(GL_CULL_FACE)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

        # light settings
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE)
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE)
        glEnable(GL_LIGHTING)
        glEnable(GL_LIGHT0)
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (0.1, 0.1, 0.1, 1.0))
        
        self.resetView()

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glMatrixMode(GL_MODELVIEW)
        glLoadMatrixd(self.__matrix)

        if self.__drawGrid:
            self.drawGrid()
        if self.__drawNormals:
            self.drawNormals()
        if self.__drawBounds:
            self.drawBounds()

        glColor3f(1, 1, 1)
        if self.scene:
            self.scene.draw()
            self.signalSceneDrawn.emit()

        if self.__drawHUD:
            self.drawHUD()

    def drawNormals(self):
        def _draw(obj):
            md = obj.getMetaData()
            if alembic.AbcGeom.IPolyMesh.matches(md) or alembic.AbcGeom.ISubD.matches(md):
                meshObj = alembic.AbcGeom.IPolyMesh(obj.getParent(), obj.getName())
                mesh = meshObj.getSchema()
                
                xf = getFinalMatrix(meshObj)
                iss = alembic.Abc.ISampleSelector(0)

                facesProp = mesh.getFaceIndicesProperty()
                if not facesProp.valid():
                    return
                pointsProp = mesh.getPositionsProperty()
                if not pointsProp.valid():
                    return
                normalsProp = mesh.getNormalsParam().getValueProperty()
                if not normalsProp.valid():
                    return
                boundsProp = mesh.getSelfBoundsProperty()
                if not boundsProp.valid():
                    return
                
                faces = facesProp.getValue(iss)
                points = pointsProp.getValue(iss)
                normals = normalsProp.getValue(iss)
                bounds = boundsProp.getValue(iss)

                for i, fi in enumerate(faces):
                    p = points[fi] * xf
                    n = normals[i]
                    v = p + n
                    glBegin(GL_LINES)
                    glColor3f(0, 1, 0)
                    glVertex3f(p[0], p[1], p[2])
                    glVertex3f(v[0], v[1], v[2])
                    glEnd()
        
            for child in obj.children:
                try:
                    _draw(child)
                except Exception, e:
                    print "Unhandled exception", e

        _draw(self.archive.getTop())

    def drawBounds(self):
        bounds = self.scene.bounds()
        min_x = bounds.min()[0]
        min_y = bounds.min()[1]
        min_z = bounds.min()[2]
        max_x = bounds.max()[0]
        max_y = bounds.max()[1]
        max_z = bounds.max()[2]
        
        w = max_x - min_x
        h = max_y - min_y
        d = max_z - min_z

        glBegin(GL_LINES)
        glColor3f(0.0, 0.2, 1.0)
        glVertex3f(min_x, min_y, min_z)
        glVertex3f(min_x+w, min_y, min_z)
        glVertex3f(min_x, min_y, min_z)
        glVertex3f(min_x, min_y+h, min_z)
        glVertex3f(min_x, min_y, min_z)
        glVertex3f(min_x, min_y, min_z+d)
        glVertex3f(min_x+w, min_y, min_z)
        glVertex3f(min_x+w, min_y+h, min_z)
        glVertex3f(min_x+w, min_y+h, min_z)
        glVertex3f(min_x, min_y+h, min_z)
        glVertex3f(min_x, min_y, min_z+d)
        glVertex3f(min_x+w, min_y, min_z+d)
        glVertex3f(min_x+w, min_y, min_z+d)
        glVertex3f(min_x+w, min_y, min_z)
        glVertex3f(min_x, min_y, min_z+d)
        glVertex3f(min_x, min_y+h, min_z+d)
        glVertex3f(min_x, min_y+h, min_z+d)
        glVertex3f(min_x, min_y+h, min_z)
        glVertex3f(min_x+w, min_y+h, min_z)
        glVertex3f(min_x+w, min_y+h, min_z+d)
        glVertex3f(min_x+w, min_y, min_z+d)
        glVertex3f(min_x+w, min_y+h, min_z+d)
        glVertex3f(min_x, min_y+h, min_z+d)
        glVertex3f(min_x+w, min_y+h, min_z+d)

        cx, cy, cz = bounds.center()
        glVertex3f(cx-1, cy, cz)
        glVertex3f(cx+1, cy, cz)
        glVertex3f(cx, cy-1, cz)
        glVertex3f(cx, cy+1, cz)
        glVertex3f(cx, cy, cz-1)
        glVertex3f(cx, cy, cz+1)

        glEnd()

    def drawGrid(self):
        for x in range(-10, 11):
            if x == 0:
                continue
            glBegin(GL_LINES)
            glColor3f(0.5, 0.5, 0.5)
            glVertex3f(x, 0, -10)
            glVertex3f(x, 0, 10)
            glVertex3f(-10, 0, x)
            glVertex3f(10, 0, x)
            glEnd()
        
        glBegin(GL_LINES)
        glColor3f(1, 0, 0)
        glVertex3f(0, 0, -10)
        glVertex3f(0, 0, 10)
        glVertex3f(-10, 0, 0)
        glVertex3f(10, 0, 0)
        glEnd()

    def drawHUD(self):
        def _format(array):
            return " ".join(["%.2f" %f for f in array])
        glColor3f(1, 1, 0.5)
        self.renderText(15, 20, _format(self.__translate))
        glColor3f(0.5, 1, 0.5)
        self.renderText(15, 40, _format(self.__center))
    
    def resizeGL(self, width, height):
        glViewport(0, 0, width, height)
        self.setProjection(self.__near, self.__far, self.__fovy)
        self.updateGL()

    def setProjection(self, near, far, fovy):
        self.makeCurrent()
        self.__near = near
        self.__far = far
        self.__fovy = fovy
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(self.__fovy, float(self.width()) / float(self.height()),
                       self.__near, self.__far)
        self.updateGL()

    def setCenter(self, center):
        self.__center = center
        self.viewAll()
    
    def translate(self, trans):
        self.makeCurrent()
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        glTranslated(trans[0], trans[1], trans[2])
        glMultMatrixd(self.__matrix)
        self.__matrix = glGetDoublev(GL_MODELVIEW_MATRIX)
        self.__translate[0] = self.__matrix[3][0]
        self.__translate[1] = self.__matrix[3][1]
        self.__translate[2] = self.__matrix[3][2]

    def rotate(self, axis, angle):
        self.makeCurrent()
        
        x = self.__matrix[0][0] * self.__center[0] + \
            self.__matrix[1][0] * self.__center[1] + \
            self.__matrix[2][0] * self.__center[2] + \
            self.__matrix[3][0]
        y = self.__matrix[0][1] * self.__center[0] + \
            self.__matrix[1][1] * self.__center[1] + \
            self.__matrix[2][1] * self.__center[2] + \
            self.__matrix[3][1]
        z = self.__matrix[0][2] * self.__center[0] + \
            self.__matrix[1][2] * self.__center[1] + \
            self.__matrix[2][2] * self.__center[2] + \
            self.__matrix[3][2]
        t = [x, y, z]

        glLoadIdentity()
        glTranslatef(t[0], t[1], t[2])
        glRotated(angle, axis[0], axis[1], axis[2])
        glTranslatef(-t[0], -t[1], -t[2])
        glMultMatrixd(self.__matrix)
        self.__matrix = glGetDoublev(GL_MODELVIEW_MATRIX)

    def resetView(self):
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        self.__matrix = glGetDoublev(GL_MODELVIEW_MATRIX)
        self.setCenter([0.0, 0.0, 0.0])

    def resetRotation(self):
        self.__matrix[0] = [1.0, 0.0, 0.0, 0.0]
        self.__matrix[1] = [0.0, 1.0, 0.0, 0.0]
        self.__matrix[2] = [0.0, 0.0, 1.0, 0.0]
        glMatrixMode(GL_MODELVIEW)
        glLoadMatrixd(self.__matrix)
        self.updateGL()

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.RightButton:
            self.setCursor(QtCore.Qt.ArrowCursor)
            menu = QtGui.QMenu(self)
            menu.addAction("HUD " + ("off" if self.__drawHUD else "on"), 
                    self.toggleHUD)
            menu.addAction("Grid " + ("off" if self.__drawGrid else "on"), 
                    self.toggleGrid)
            menu.addAction("Bounds " + ("off" if self.__drawBounds else "on"), 
                    self.toggleBounds)
            menu.addAction("Normals " + ("off" if self.__drawNormals else "on"), 
                    self.toggleNormals)
            menu.popup(QtCore.QPoint(event.globalX(), event.globalY()))
        else:
            self.__last_p2d = event.pos()
            self.__last_pok, self.__last_p3d = self.mapToSphere(self.__last_p2d)
        
        self.setCursor(QtCore.Qt.OpenHandCursor)

    def mouseMoveEvent(self, event):
        newPoint2D = event.pos()

        if ((newPoint2D.x() < 0) or (newPoint2D.x() > self.width()) or
            (newPoint2D.y() < 0) or (newPoint2D.y() > self.height())):
            return
        
        value_y = 0
        newPoint_hitSphere, newPoint3D = self.mapToSphere(newPoint2D)

        dx = float(newPoint2D.x() - self.__last_p2d.x())
        dy = float(newPoint2D.y() - self.__last_p2d.y())

        w = float(self.width())
        h = float(self.height())
        self.makeCurrent()

        if (((event.buttons() & QtCore.Qt.LeftButton) and (event.buttons() & QtCore.Qt.MidButton))
            or (event.buttons() & QtCore.Qt.LeftButton and event.modifiers() & QtCore.Qt.ControlModifier)):
            value_y = self.__radius * dy * 2.0 / h
            self.translate([0.0, 0.0, value_y])
        
        elif (event.buttons() & QtCore.Qt.MidButton
              or (event.buttons() & QtCore.Qt.LeftButton and event.modifiers() & QtCore.Qt.ShiftModifier)):
            z = - (self.__matrix[0][2] * self.__center[0] +
                   self.__matrix[1][2] * self.__center[1] +
                   self.__matrix[2][2] * self.__center[2] +
                   self.__matrix[3][2]) / (self.__matrix[0][3] * self.__center[0] +
                                                    self.__matrix[1][3] * self.__center[1] +
                                                    self.__matrix[2][3] * self.__center[2] +
                                                    self.__matrix[3][3])

            fovy = 45.0
            aspect = w / h
            n = 0.01 * self.__radius
            up = math.tan(fovy / 2.0 * math.pi / 180.0) * n
            right = aspect * up

            self.translate([2.0 * dx / w * right / n * z,
                            -2.0 * dy / h * up / n * z,
                             0.0])
    
        elif event.buttons() & QtCore.Qt.LeftButton:
            if not self.__rotating:
                self.__rotating = True
       
            axis = [0.0, 0.0, 0.0]
            angle = 0.0

            if self.__last_pok and newPoint_hitSphere:
                axis = numpy.cross(self.__last_p3d, newPoint3D)
                cos_angle = numpy.dot(self.__last_p3d, newPoint3D)
                if (abs(cos_angle) < 1.0):
                    angle = math.acos(cos_angle) * 180.0 / math.pi
                    angle *= 2.0
                self.rotate(axis, angle)

        self.__last_p2d = newPoint2D
        self.__last_p3d = newPoint3D
        self.__last_pok = newPoint_hitSphere
        self.updateGL()

    def mouseReleaseEvent(self, event):
        self.__rotating = False
        self.__last_pok = False
        super(AbcGLWidget, self).mouseReleaseEvent(event)

    def mapToSphere(self, v2d):
        v3d = [0.0, 0.0, 0.0]
        if ((v2d.x() >= 0) and (v2d.x() <= self.width()) and
            (v2d.y() >= 0) and (v2d.y() <= self.height())):
            x = float(v2d.x() - 0.5 * self.width())  / self.width()
            y = float(0.5 * self.height() - v2d.y()) / self.height()
            v3d[0] = x
            v3d[1] = y
            z2 = 2.0 * 0.5 * 0.5 - x * x - y * y
            v3d[2] = math.sqrt(max( z2, 0.0 ))
            n = linalg.norm(v3d)
            v3d = numpy.array(v3d) / float(n)
            return True, v3d
        else:
            return False, v3d

    def wheelEvent(self, event):
        d = float(event.delta()) / 200 * self.__radius
        self.translate([0.0, 0.0, d])
        self.updateGL()
        event.accept()
