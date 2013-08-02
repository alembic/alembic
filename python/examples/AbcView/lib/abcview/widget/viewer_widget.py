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
from functools import wraps

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

import abcview
from abcview.io import Mode
from abcview.gl import GLCamera, GLICamera, GLScene
from abcview import log, config

kWrapExisting = alembic.Abc.WrapExistingFlag.kWrapExisting

# GL drawing mode map
GL_MODE_MAP = {
    Mode.OFF: 0,
    Mode.FILL: GL_FILL,
    Mode.LINE: GL_LINE,
    Mode.POINT: GL_POINT
}

def accumXform(xf, obj):
    if alembic.AbcGeom.IXform.matches(obj.getHeader()):
        x = alembic.AbcGeom.IXform(obj, kWrapExisting)
        xs = x.getSchema().getValue()
        xf *= xs.getMatrix()

def get_final_matrix(obj):
    xf = imath.M44d()
    xf.makeIdentity()
    parent = obj.getParent()
    while parent:
        accumXform(xf, parent)
        parent = parent.getParent()
    return xf

def update_camera(func):
    """
    GL camera update decorator
    """
    @wraps(func)
    def with_wrapped_func(*args, **kwargs):
        func(*args, **kwargs)
        wid = args[0]
        wid.camera.apply()
        wid.updateGL()
        wid.signal_camera_updated.emit(wid.camera)
        wid.state.signal_state_change.emit()
    return with_wrapped_func

def set_ambient_light():
    glDisable(GL_LIGHT0)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (0.4, 0.4, 0.4, 1.0))

def set_diffuse_light():
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE)
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE)
    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (0.1, 0.1, 0.1, 1.0))

def set_default_materials():
    """
    Hardcoded GL material shaders.
    """
    mat_specular = (1.0, 1.0, 1.0, 1.0)
    mat_shininess = 100.0
    mat_front_emission = (0.0, 0.0, 0.0, 0.0)
    mat_back_emission = (1.0, 0.0, 0.0, 1.0)

    glClearColor(0.12, 0.12, 0.12, 0.0)
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_front_emission)
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular)
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess)

    glMaterialfv(GL_BACK, GL_EMISSION, mat_back_emission)
    glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular)
    glMaterialfv(GL_BACK, GL_SHININESS, mat_shininess)

    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE)
    glEnable(GL_COLOR_MATERIAL)

def draw_bounding_box(bounds):
    """
    Draws a bounding box given a box3d object.

    :param bounds: Imath.box3d
    """
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

def create_viewer_app(filepath=None):
    """
    Creates a standalone viewer app. ::

        >>> from abcview.widget.viewer_widget import create_viewer_app
        >>> create_viewer_app("file.abc")

    """
    app = QtGui.QApplication(sys.argv)

    # create the viewer widget
    viewer = GLWidget()
    viewer_group = QtGui.QGroupBox()
    viewer_group.setLayout(QtGui.QVBoxLayout())
    viewer_group.layout().setSpacing(0)
    viewer_group.layout().setMargin(0)
    viewer_group.layout().addWidget(viewer)
    viewer_group.setWindowTitle("GLWidget")

    # set default size
    viewer_group.setMinimumSize(QtCore.QSize(100, 100))
    viewer_group.resize(500, 300)

    # display the viewer app
    viewer_group.show()
    viewer_group.raise_()

    # override key press event handler
    viewer_group.keyPressEvent = viewer.keyPressEvent

    # open a file
    if filepath:
        if os.path.exists(filepath):
            viewer.add_file(filepath)
        else:
            log.warn("file not found: %s" % filepath)

    return app.exec_()

def message(info):
    dialog = QtGui.QMessageBox()
    dialog.setText(info)
    dialog.exec_()

class GLSplitter(QtGui.QSplitter):
    def __init__(self, orientation, wipe=False):
        super(GLSplitter, self).__init__(orientation)
        self.wipe = wipe
        #self.splitterMoved.connect(self.handle_splitter_moved)

class GLState(QtCore.QObject):
    """
    Global GL viewer state manager. Manages list of Cameras and Scenes, 
    which can be shared between viewers.
    """
    SECOND = 1000.0
    signal_state_change = QtCore.pyqtSignal()
    signal_play_fwd = QtCore.pyqtSignal()
    signal_play_stop = QtCore.pyqtSignal()
    signal_current_time = QtCore.pyqtSignal(float)
    signal_current_frame = QtCore.pyqtSignal(int)

    def __init__(self, fps=24.0):
        """
        :param fps: play scene at frames per second (fps)
        """
        super(GLState, self).__init__()
        self.timer = QtCore.QTimer(self)
        self.active_viewer = None
        self.frames_per_second = fps
        self.clear()
        
        # for calculating frames per second during playback
        self.fps = 0.0
        self.__fps_timer = QtCore.QTimer(self)
        self.__fps_timer.setInterval(self.SECOND)
        self.connect(self.__fps_timer, QtCore.SIGNAL("timeout ()"), 
                self._fps_timer_cb)

    def clear(self):
        self.__scenes = []
        self.__cameras = {}
        self.__time = 0
        self.__frame = 0
        self.__playing = False
        self.__fps_counter = 0
        self.signal_state_change.emit()

    def _get_cameras(self):
        return self.__cameras.values()

    def _set_cameras(self):
        log.debug("use add_camera()")

    cameras = property(_get_cameras, _set_cameras, doc="cameras")

    def _get_scenes(self):
        return self.__scenes

    def _set_scenes(self):
        log.debug("use add_scene() to add scenes")

    scenes = property(_get_scenes, _set_scenes, doc="scenes")

    def add_scene(self, scene):
        """
        Adds a scene to the viewer session. 

        :param scene: GLScene object
        """
        if type(scene) == GLScene:
            if scene not in self.__scenes:
                self.__scenes.append(scene)
            else:
                scene.visible = True
        self.signal_state_change.emit()

    def add_file(self, filepath):
        self.add_scene(GLScene(filepath))

    def remove_scene(self, scene):
        """
        Removes a given GLScene object from the master scene.

        :param scene: GLScene to remove.
        """
        scene.visible = False
        self.signal_state_change.emit()

    def add_camera(self, camera):
        """
        Adds a new GLCamera to the state.

        :param camera: GLCamera object
        """
        log.debug("GLState.add_camera %s" % camera)
        if camera and camera.name not in self.__cameras.keys():
            self.__cameras[camera.name] = camera
            return True
        return False

    def remove_camera(self, camera):
        """
        Removes a GLCamera object from the state.

        :param camera: GLCamera object
        """
        if type(camera) in [str, unicode]:
            del self.__cameras[name]
        else:
            del self.__cameras[camera.name]
        self.signal_state_change.emit()

    def get_camera(self, name):
        """
        Returns a named camera for a given viewer.

        :param camera: GLCamera object
        """
        return self.__cameras.get(name)

    def _get_time(self):
        return self.__time

    def _set_time(self, new_time):
        self.__time = new_time
        self.__frame = new_time * self.frames_per_second
        for scene in self.scenes:
            if scene.visible:
                scene.set_time(new_time)
        self.signal_current_time.emit(new_time) 
        self.signal_current_frame.emit(int(round(new_time * self.frames_per_second)))

        # update other viewers
        self.signal_state_change.emit()

    current_time = property(_get_time, _set_time, doc="set/get current time")

    def _get_frame(self):
        return self.__frame

    def _set_frame(self, frame):
        if frame > self.frame_range()[1]:
            frame = self.frame_range()[0]
        elif frame < self.frame_range()[0]:
            frame = self.frame_range()[1]
        self.current_time = frame / float(self.frames_per_second)

    current_frame = property(_get_frame, _set_frame, doc="set/get current frame")
    
    def time_range(self):
        """
        Returns total time range in seconds.
        """
        min = None
        max = None
        if self.scenes:
            for scene in self.scenes:
                if min is None or scene.min_time() < min:
                    min = scene.min_time()
                if max is None or scene.max_time() > max:
                    max = scene.max_time()
        else:
            min, max = 0, 0
        return (min, max)

    def frame_range(self):
        """
        Returns total frame range.
        """
        (min, max) = self.time_range()
        return (min * self.frames_per_second, max * self.frames_per_second)

    def frame_count(self):
        """
        Returns total frame count.
        """
        return len(range(*self.frame_range())) + 1
    
    def is_playing(self):
        return self.__playing

    def play(self):
        """
        plays loaded scenes by activating timer and setting callback
        """
        self.timer.setInterval(self.SECOND / (float(self.frames_per_second)))
        self.connect(self.timer, QtCore.SIGNAL("timeout ()"), self._play_fwd_cb)
        self.timer.start()
        self.__fps_timer.start()
        self.signal_play_fwd.emit()

    def _play_fwd_cb(self):
        """
        play callback, sets current time for all scenes
        """
        self.__playing = True
        min_time, max_time = self.time_range()
        self.current_time += 1.0 / float(self.frames_per_second)
        if self.current_time > max_time:
            self.current_time = min_time
        self.__fps_counter += 1

    def stop(self):
        """
        stops scene playback
        """
        self.__playing = False
        self.disconnect(self.timer,  QtCore.SIGNAL("timeout ()"), self._play_fwd_cb)
        self.timer.stop()
        self.__fps_timer.stop()
        self.__fps_counter = 0
        self.signal_play_stop.emit()

    def _fps_timer_cb(self):
        """
        frames per second timer callback
        """
        self.fps = (self.__fps_counter / float(self.frames_per_second)) * \
                      self.frames_per_second
        self.__fps_counter = 0

class GLWidget(QtOpenGL.QGLWidget):
    """
    AbcView OpenGL Widget.

    Basic usage ::

        >>> create_viewer_app("file.abc")

    or inside a larger Qt application ::

        >>> viewer = GLWidget()
        >>> viewer.add_file("file.abc")
    """
    signal_scene_opened = QtCore.pyqtSignal(GLScene)
    signal_scene_removed = QtCore.pyqtSignal(GLScene)
    signal_scene_error = QtCore.pyqtSignal(str)
    signal_scene_drawn = QtCore.pyqtSignal()
    signal_set_camera = QtCore.pyqtSignal(GLCamera)
    signal_new_camera = QtCore.pyqtSignal(GLCamera)
    signal_camera_updated = QtCore.pyqtSignal(GLCamera)

    def __init__(self, parent=None, state=None):
        """
        :param parent: parent Qt object
        :param state: GLState object (for shared states)
        """
        self.camera = None
        format = QtOpenGL.QGLFormat()
        format.setDirectRendering(True)
        format.setSampleBuffers(True)
        self.state = state or GLState()
        self.state.signal_state_change.connect(self.handle_state_change)
        super(GLWidget, self).__init__(format, parent)
        self.setAutoBufferSwap(True)
        self.setMouseTracking(True)
        self.setFocusPolicy(QtCore.Qt.StrongFocus)
        self.setCursor(QtCore.Qt.OpenHandCursor)
      
        # embedded in larger app?
        self._main = parent

        # various matrices and vectors
        self.__bounds_default = imath.Box3d((-5,-5,-5), (5,5,5))
        self.__bounds = None
        self.__radius = 5.0
        self.__last_pok = False
        self.__last_p2d = QtCore.QPoint()
        self.__last_p3d = [1.0, 0.0, 0.0]
        self.__rotating = False

        # viewers must have at least one camera
        self.setup_default_camera()

    def uid(self):
        return id(self)

    def __repr__(self):
        return "<GLWidget %s>" % self.uid()

    def setup_default_camera(self):
        """
        Creates the default interactive camera for this view (and others if the
        state is shared between viewers).
        """
        self.add_camera(GLCamera(self, name="interactive"))
        self.set_camera("interactive")

    def clear(self):
        """
        Resets this GL viewer widget, clears the shared state
        and creates a new default interactive camera.
        """
        self.state.clear()
        self.setup_default_camera()

    def add_file(self, filepath):
        """
        Loads a given filepath into the GL viewer.

        :param filepath: path to Alembic file.
        """
        self.add_scene(GLScene(filepath))

    def add_scene(self, scene):
        """
        Adds a scene to the viewer session. 

        :param scene: GLScene object
        """
        self.state.add_scene(scene)
        self.signal_scene_opened.emit(scene)
        self.updateGL()

    def remove_scene(self, scene):
        """
        Removes a given GLScene object from the master scene.

        :param scene: GLScene to remove.
        """
        self.state.remove_scene(scene)
        self.signal_scene_removed.emit(scene)
        self.updateGL()

    def add_camera(self, camera):
        """
        :param camera: GLCamera object
        """
        log.debug("GLWidget.add_camera: %s" % camera)
        if self.state.add_camera(camera):
            camera.add_view(self)
            self.signal_new_camera.emit(camera)

    def remove_camera(self, camera):
        """
        :param camera: GLCamera object to remove
        """
        self.state.remove_camera(camera)
        self.signal_state_change.emit()

    @update_camera
    def set_camera(self, camera):
        """
        Sets the scene camera from a given camera name string

        :param camera: Name of camera or GLCamera object
        """
        log.debug("GLWidget.set_camera %s" % camera)
        if type(camera) in [str, unicode]:
            if "/" in camera:
                camera = os.path.split("/")[-1]
            elif camera not in [cam.name for cam in self.state.cameras]:
                log.warn("camera not found: %s" % camera)
                return
            self.camera = self.state.get_camera(camera)
        else:
            self.camera = camera
        self.camera.add_view(self)
        self.resizeGL(self.width(), self.height())
        self.signal_set_camera.emit(self.camera)

    def aspect_ratio(self):
        """
        Returns current aspect ration of the viewer.
        """
        return self.width() / float(self.height())
    
    def _get_bounds(self):
        #TODO: ugly code needs refactor
        if self.state.scenes:
            bounds = None
            for scene in self.state.scenes:
                if not scene.loaded:
                    continue
                if bounds is None or scene.bounds().max() > bounds.max():
                    bounds = scene.bounds()
                    min = bounds.min()
                    max = bounds.max()
                    if scene.properties.get("translate"):
                        max = max * imath.V3d(*scene.translate)
                        min = min * imath.V3d(*scene.translate)
                    if scene.properties.get("scale"):
                        max = max * imath.V3d(*scene.scale)
                        min = min * imath.V3d(*scene.scale)
                    bounds = imath.Box3d(min, max)
            
            if bounds is None:
                return self.__bounds_default
            self.__bounds = bounds
            return self.__bounds
        else:
            return self.__bounds_default

    def _set_bounds(self, bounds):
        self.__bounds = bounds

    bounds = property(_get_bounds, _set_bounds, doc="scene bounding box")

    @update_camera
    def frame(self, bounds=None):
        """
        Frames the viewer's active camera on the bounds of the currently
        loaded and visible scenes.

        :param bounds: imath.Box3d bounds object.
        """
        log.debug("GLWidget.frame %s" % bounds)
        if bounds is None:
            bounds = self.bounds
        if self.camera:
            self.camera.frame(bounds)

    def split(self, orientation=QtCore.Qt.Vertical,
                    wipe=False):
        """
        Splist the viewer into two separate widgets according
        to the orientation param. ::

            QGroupBox
                `- QSplitter
                        |- QGroupBox
                        |       `- GLWidget
                        `- QGroupBox
                                `- GLWidget

        """
        if not self.parent():
            return

        item = self.parent().layout().itemAt(0)

        # create the splitter
        splitter = GLSplitter(orientation, wipe)
        self.parent().layout().addWidget(splitter)

        # left/top viewer group
        group1 = QtGui.QGroupBox()
        group1.setLayout(QtGui.QVBoxLayout())
        group1.layout().setSpacing(0)
        group1.layout().setMargin(0)
        group1.layout().addWidget(item.widget())

        # right/bottom viewer group
        group2 = QtGui.QGroupBox()
        group2.setLayout(QtGui.QVBoxLayout())
        group2.layout().setSpacing(0)
        group2.layout().setMargin(0)
        new_viewer = GLWidget(self._main, state=self.state)
        self.camera.add_view(new_viewer)
        group2.layout().addWidget(new_viewer)

        # link the two groups for unsplitting later
        group1.other = group2
        group1.viewer = self
        group2.other = group1
        group2.viewer = new_viewer

        # add the two groups to the splitter
        splitter.addWidget(group1)
        splitter.addWidget(group2)

    def split_vert(self):
        """
        Splits the viewer vertically.
        """
        self.split(QtCore.Qt.Horizontal)

    def split_horz(self):
        """
        Splits the viewer horizontally.
        """
        self.split(QtCore.Qt.Vertical)

    #TODO: support viewer pop-outs, better garbage collection
    def unsplit(self):
        """
        Unsplits and deletes current viewer
        """
        if not self.parent():
            return

        # get the parent splitter object
        splitter = self.parent()
        while splitter and type(splitter) != GLSplitter:
            splitter = splitter.parent()

        # we've reached the top splitter, do nothing
        if splitter is None:
            return

        # disconnect signals
        try:
            self.state.signal_state_change.disconnect()
        except Exception:
            log.debug("error disconnecting state signal")

        splitter.parent().layout().removeWidget(splitter)
        splitter.parent().layout().addWidget(self.parent().other)

        # delete this view from all cameras
        for camera in self.state.cameras:
            camera.remove_view(self)

        # reassign the viewer attribute on main
        if self._main:
            self._main.viewer = self.parent().other.viewer

        # cleanup
        del splitter
        del self

    def _paint_normals(self):
        """
        Paints normals for polys and subds.
        """
        glColor3f(1, 1, 1)
        def _draw(obj):
            md = obj.getMetaData()
            if alembic.AbcGeom.IPolyMesh.matches(md) or alembic.AbcGeom.ISubD.matches(md):
                meshObj = alembic.AbcGeom.IPolyMesh(obj.getParent(), obj.getName())
                mesh = meshObj.getSchema()
                
                xf = get_final_matrix(meshObj)
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
                    log.warn("unhandled exception: %s" % e)

        for scene in self.state.scenes:
            _draw(scene.top())

    def _paint_grid(self):
        """
        Paints the grid.
        """
        glColor3f(1, 1, 1)
        set_ambient_light() 
        for x in range(-10, 11):
            if x == 0:
                continue
            glBegin(GL_LINES)
            glVertex3f(x, 0, -10)
            glVertex3f(x, 0, 10)
            glVertex3f(-10, 0, x)
            glVertex3f(10, 0, x)
            glEnd()
        
        glBegin(GL_LINES)
        glVertex3f(0, 0, -10)
        glVertex3f(0, 0, 10)
        glVertex3f(-10, 0, 0)
        glVertex3f(10, 0, 0)
        glEnd()

        set_diffuse_light()
    
    def _paint_hud(self):
        """
        Paints the heads-up-display information.
        """
        glColor3f(1, 1, 1)
        def _format(array):
            return ", ".join(["%.2f" %f for f in array])

        glViewport(0, 0, self.width(), self.height())
        
        # draw the camera name
        glColor3f(0.5, 1, 0.5)
        if self.camera.fixed:
            self.renderText(15, 20, "%s (%.02f)" 
                    % (self.camera.name, self.camera.aspect_ratio))
        else:
            self.renderText(15, 20, "%s" % self.camera.name)

        # draw the camera info
        glColor3f(0.6, 0.6, 0.6)
        font = QtGui.QFont("Arial", 8)
        self.renderText(15, 35, "T [%s]" 
                % _format(self.camera.translation), font)
        self.renderText(15, 50, "R [%s]" 
                % _format(self.camera.rotation), font)
        self.renderText(15, 66, "S [%s]" 
                % _format(self.camera.scale), font)

        # draw the FPS info
        glColor3f(0.7, 0.7, 0.7)
        font = QtGui.QFont("Arial", 9)
        self.renderText(self.width()-100, self.height()-10, "%.1f / %.1f FPS" 
                % (self.state.fps, self.state.frames_per_second), font)

        glColor3f(1, 1, 1)

    def _paint_fixed(self):
        """
        Changes the GL viewport according to the camera's aspect ratio.
        """
        # get basic size values for this viewer
        camera_width = self.camera.get_size(self)[0]
        camera_height = self.camera.get_size(self)[1]
        camera_aspect_ratio = camera_width / float(camera_height)
        
        # if fixed, lock the aspect ratio
        if self.camera.fixed:
            camera_aspect_ratio = self.camera.aspect_ratio
            if self.aspect_ratio() > self.camera.aspect_ratio:
                w = int(self.width() / (self.aspect_ratio() \
                        / camera_aspect_ratio))
                h = camera_height
            else:
                w = camera_width
                h = int(self.height() * (self.aspect_ratio() \
                        / camera_aspect_ratio))
            x = int(abs(w-self.width()) / 2.0)
            y = int(abs(h-self.height()) / 2.0)

        # camera size matches viewer
        else:
            x = y = 0
            w = self.width()
            h = self.height()
        
        # do some GL stuff
        glViewport(x, y, w, h)
        self.makeCurrent()
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(self.camera.fovy, camera_aspect_ratio,
                       self.camera.near, self.camera.far)
        glMatrixMode(GL_MODELVIEW)

    def set_camera_clipping_planes(self, near=None, far=None):
        self.camera.set_clipping_planes(self.camera.near, self.camera.far)

    def map_to_sphere(self, v2d):
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

    def handle_state_change(self):
        """
        State change signal handler.
        """
        self.updateGL()

    def handle_set_camera(self, action):
        """
        Sets camera from name derived from the text of a QAction.

        :param action: QAction
        """
        self.set_camera(str(action.text().toAscii()))

    def handle_set_mode(self, mode):
        """
        Set active camera drawing mode.

        :param mode: abcview.io.Mode enum value
        """
        if self.sender(): # via menu action 
            mode = self.sender().data().toInt()[0]
        if mode not in GL_MODE_MAP.keys():
            raise Exception("Invalid drawing mode: %s" % mode)
        self.camera.mode = mode

    def handle_camera_action(self, action):
        """
        New camera menu handler.
        """
        action_name = str(action.text().toAscii())
        if action_name == "New":
            text, ok = QtGui.QInputDialog.getText(self, "New Camera",
                                  "Camera Name:", QtGui.QLineEdit.Normal)
            if ok and not text.isEmpty():
                name = str(text.toAscii())
                camera = GLCamera(self, name)
                self.add_camera(camera)
                self.set_camera(name)

    ## base class overrides

    def initializeGL(self):
        self.makeCurrent()
        glPointSize(3.0)
        glEnable(GL_BLEND)
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_AUTO_NORMAL)
        glEnable(GL_COLOR_MATERIAL)
        glEnable(GL_NORMALIZE)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glDisable(GL_CULL_FACE)
        glShadeModel(GL_SMOOTH)
        glClearColor(0.15, 0.15, 0.15, 0.0)
        set_diffuse_light()

    def paintGL(self):
        """
        OpenGL painting override
        """
        if self.isHidden() or not self.state:
            return

        if self not in self.camera.views:
            return

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glDrawBuffer(GL_BACK)
        glLoadIdentity()

        # update camera
        self.camera.apply()

        # change background color to indicate which viewer is active
        #if self.state.active_viewer == self:
        #    glClearColor(0.15, 0.15, 0.15, 0.0)
        #else:
        #    glClearColor(0.2, 0.2, 0.2, 0.0) 

        # draw the heads-up-display
        if self.camera.draw_hud:
            self._paint_hud()

        # adjusts the camera size
        self._paint_fixed()
        
        # draw the grid lines
        if self.camera.draw_grid:
            self._paint_grid()
        
        # draw geom normals
        if self.camera.draw_normals:
            self._paint_normals()

        # draw each scene
        for scene in self.state.scenes:
           
            if not scene.visible:
                continue

            # color override
            glEnable(GL_COLOR_MATERIAL)
            glMaterialfv(GL_BACK, GL_EMISSION, scene.properties.get("color", 
                         (1, 0, 0)))

            # draw mode override
            mode = GL_MODE_MAP.get(scene.properties.get("mode"), 
                   GL_MODE_MAP.get(self.camera.mode, GL_LINE)
                   )
            if mode != Mode.OFF:
                glPolygonMode(GL_FRONT_AND_BACK, mode)

            # apply local transforms
            glPushMatrix()
            glTranslatef(*scene.translate)
            glRotatef(*scene.rotate)
            glScalef(*scene.scale)

            # draw bounds
            if self.camera.draw_bounds:
                glColor3f(1, 1, 1)
                set_ambient_light()
                draw_bounding_box(scene.scene.bounds())
                set_diffuse_light()

            # draw scene
            if mode != Mode.OFF:
                scene.draw()
            
            glPopMatrix()
            self.signal_scene_drawn.emit()
        
    def resizeGL(self, width, height):
        log.debug("GLWidget.resizeGL %s %s %s" % (self, width, height))
        try:
            self.camera.resize()
        except AttributeError, e:
            pass

    @update_camera
    def keyPressEvent(self, event):
        """
        key press event handler
        """
        key = event.key()
        mod = event.modifiers()

        # space bar - playback control
        if key == QtCore.Qt.Key_Space:
            if len(self.state.scenes) == 0 or \
                   self.state.frame_count <= 1:
                return

            if self.state.is_playing():
                self.state.stop()
            else:
                self.state.play()

        # right arroy - increment frame
        elif key == QtCore.Qt.Key_Right:
            self.state.current_frame = self.state.current_frame + 1

        # left arrow - decrement frame
        elif key == QtCore.Qt.Key_Left:
            self.state.current_frame = self.state.current_frame - 1

        # alt+f - toggle fixed aspect ratio
        elif mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_F:
            self.camera._set_fixed()
    
        # alt+b - toggle bounding boxes
        elif mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_B:
            self.camera._set_draw_bounds()

        # alt+n - toggle draw normals
        elif mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_N:
            self.camera._set_draw_normals()

        # alt+g - toggle grid
        elif mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_G:
            self.camera._set_draw_grid()

        # alt+h - toggle hud
        elif mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_H:
            self.camera._set_draw_hud()

        # shift+| - split vertically
        elif mod == QtCore.Qt.ShiftModifier and key == QtCore.Qt.Key_Bar:
            self.split_vert()

        # shift+| - split horizontally
        elif mod == QtCore.Qt.ShiftModifier and key == QtCore.Qt.Key_Underscore:
            self.split_horz()
        
        # alt+- - unsplit
        elif mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_Minus:
            self.unsplit()

        # "f" - frame scene
        elif key == QtCore.Qt.Key_F:
            self.frame()

    def mousePressEvent(self, event):
        """
        mouse press event handler
        """

        # use weakref for this
        self.state.active_viewer = self
        
        # make this viewer the active one
        if self._main:
            self._main.viewer = self

        # process mouse event
        if event.button() == QtCore.Qt.RightButton and \
           event.modifiers() == QtCore.Qt.NoModifier:
            self.setCursor(QtCore.Qt.ArrowCursor)
            
            menu = QtGui.QMenu(self)

            # splits
            layout_menu = QtGui.QMenu("Layout", self)
            self.splitHAct = QtGui.QAction(QtGui.QIcon("%s/split_vert.png" % config.ICON_DIR),
                                          "Split Vertical (Shift+|)", self)
            self.connect(self.splitHAct, QtCore.SIGNAL("triggered (bool)"), self.split_vert)
            layout_menu.addAction(self.splitHAct)

            self.splitVAct = QtGui.QAction(QtGui.QIcon("%s/split_horz.png" % config.ICON_DIR),
                                          "Split Horizontal (Shift+_)", self)
            self.connect(self.splitVAct, QtCore.SIGNAL("triggered (bool)"), self.split_horz)
            layout_menu.addAction(self.splitVAct)
            
            self.closeAct = QtGui.QAction("Close (Alt+-)", self)
            self.connect(self.closeAct, QtCore.SIGNAL("triggered (bool)"), self.unsplit)
            layout_menu.addAction(self.closeAct)
            
            menu.addMenu(layout_menu)

            # cameras
            camera_menu = QtGui.QMenu("Cameras", self)
            camera_group = QtGui.QActionGroup(camera_menu) 

            for camera in self.state.cameras:
                camera_action = QtGui.QAction(camera.name, self)
                camera_action.setCheckable(True)
                if camera.name == self.camera.name:
                    camera_action.setChecked(True)
                camera_action.setActionGroup(camera_group)
                camera_menu.addAction(camera_action)
            self.connect(camera_group, QtCore.SIGNAL("triggered (QAction *)"), 
                        self.handle_set_camera)
            self.connect(camera_menu, QtCore.SIGNAL("triggered (QAction *)"), 
                        self.handle_camera_action)
            camera_menu.addSeparator()
            camera_menu.addAction("New")

            menu.addMenu(camera_menu)
          
            options_menu = QtGui.QMenu("Options", self)

            # bounds toggle menu item
            #self.aframeAct = QtGui.QAction("Autoframe", self)
            #self.aframeAct.setCheckable(True)
            #self.aframeAct.setChecked(self.camera.auto_frame)
            #self.connect(self.aframeAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_auto_frame)
            #options_menu.addAction(self.aframeAct)

            # bounds toggle menu item
            self.boundsAct = QtGui.QAction("Bounds (Alt+B)", self)
            self.boundsAct.setCheckable(True)
            self.boundsAct.setChecked(self.camera.draw_bounds)
            self.connect(self.boundsAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_draw_bounds)
            options_menu.addAction(self.boundsAct)
 
            # fixed aspect ratio toggle menu item
            self.fixedAct = QtGui.QAction("Fixed Aspect Ratio (Alt+F)", self)
            self.fixedAct.setCheckable(True)
            self.fixedAct.setChecked(self.camera.fixed)
            self.connect(self.fixedAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_fixed)
            options_menu.addAction(self.fixedAct)

            # heads-up-display menu item
            self.hudAct = QtGui.QAction("HUD (Alt+H)", self)
            self.hudAct.setCheckable(True)
            self.hudAct.setChecked(self.camera.draw_hud)
            self.connect(self.hudAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_draw_hud)
            options_menu.addAction(self.hudAct)

            # grid toggle menu item
            self.gridAct = QtGui.QAction("Grid (Alt+G)", self)
            self.gridAct.setCheckable(True)
            self.gridAct.setChecked(self.camera.draw_grid)
            self.connect(self.gridAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_draw_grid)
            options_menu.addAction(self.gridAct)
          
            # normals toggle menu item
            self.normalsAct = QtGui.QAction("Normals (Alt+N)", self)
            self.normalsAct.setCheckable(True)
            self.normalsAct.setChecked(self.camera.draw_normals)
            self.connect(self.normalsAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_draw_normals)
            options_menu.addAction(self.normalsAct)

            # shading toggle menu item
            self.shading_menu = QtGui.QMenu("Shading", self)
            shading_group = QtGui.QActionGroup(self.shading_menu)

            self.offAct = QtGui.QAction("Off", self)
            self.offAct.setCheckable(True)
            self.offAct.setActionGroup(shading_group)
            self.offAct.setData(Mode.OFF)
            self.offAct.setChecked(self.camera.mode == Mode.OFF)
            self.offAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.offAct)

            self.fillAct = QtGui.QAction("Fill", self)
            self.fillAct.setCheckable(True)
            self.fillAct.setActionGroup(shading_group)
            self.fillAct.setData(Mode.FILL)
            self.fillAct.setChecked(self.camera.mode == Mode.FILL)
            self.fillAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.fillAct)
            
            self.lineAct = QtGui.QAction("Line", self)
            self.lineAct.setCheckable(True)
            self.lineAct.setActionGroup(shading_group)
            self.lineAct.setData(Mode.LINE)
            self.lineAct.setChecked(self.camera.mode == Mode.LINE)
            self.lineAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.lineAct)

            self.pointAct = QtGui.QAction("Point", self)
            self.pointAct.setCheckable(True)
            self.pointAct.setActionGroup(shading_group)
            self.pointAct.setData(Mode.POINT)
            self.pointAct.setChecked(self.camera.mode == Mode.POINT)
            self.pointAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.pointAct)
            
            options_menu.addMenu(self.shading_menu)

            menu.addMenu(options_menu)
            menu.popup(QtCore.QPoint(event.globalX(), event.globalY()))

        else:
            self.__last_p2d = event.pos()
            self.__last_pok, self.__last_p3d = self.map_to_sphere(self.__last_p2d)
        
        self.setCursor(QtCore.Qt.OpenHandCursor)

    @update_camera
    def mouseMoveEvent(self, event):
        """
        mouse move event handler
        """
        newPoint2D = event.pos()
        if ((newPoint2D.x() < 0) or (newPoint2D.x() > self.width()) or
            (newPoint2D.y() < 0) or (newPoint2D.y() > self.height())):
            return
        value_y = 0
        newPoint_hitSphere, newPoint3D = self.map_to_sphere(newPoint2D)
        dx = float(newPoint2D.x() - self.__last_p2d.x())
        dy = float(newPoint2D.y() - self.__last_p2d.y())
        w = float(self.width())
        h = float(self.height())
        self.makeCurrent()

        if (((event.buttons() & QtCore.Qt.LeftButton) and (event.buttons() & QtCore.Qt.MidButton))
            or (event.buttons() & QtCore.Qt.LeftButton and event.modifiers() & QtCore.Qt.ControlModifier)
            or (event.buttons() & QtCore.Qt.RightButton and event.modifiers() & QtCore.Qt.AltModifier)):
            self.camera.dolly(dx, dy)
        
        elif (event.buttons() & QtCore.Qt.MidButton
              or (event.buttons() & QtCore.Qt.LeftButton and event.modifiers() & QtCore.Qt.ShiftModifier)):
            self.camera.track(dx, dy)

        elif event.buttons() & QtCore.Qt.LeftButton:
            self.__rotating = True
            self.camera.rotate(dx, dy)
            
        # TODO: replace with global state (with these as attrs)
        self.__last_p2d = newPoint2D
        self.__last_p3d = newPoint3D
        self.__last_pok = newPoint_hitSphere
    
    def mouseReleaseEvent(self, event):
        """
        mouse release event handler
        """
        self.__rotating = False
        self.__last_pok = False
        super(GLWidget, self).mouseReleaseEvent(event)

    @update_camera
    def wheelEvent(self, event):
        """
        mouse wheel event handler
        """
        dx = float(event.delta()) / 10
        self.camera.dolly(dx, 0)
        event.accept()

if __name__ == "__main__":
    if len(sys.argv) > 1:
        filepath = sys.argv[1]
    else:
        filepath = None
    create_viewer_app(filepath)
